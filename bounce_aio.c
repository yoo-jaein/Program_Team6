// file: bounce_aio.c
// author: 2016110879_유재인
// datetime: 2017-11-21 09:22
// description: aio_read()를 이용한 유저컨트롤 애니메이션을 보여준다.

#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <aio.h>
#include <string.h>
#include <termios.h>

#define	MESSAGE	"hello"
#define	BLANK "     "

int row = 10;	// 현재 row
int col = 0;	// 현재 col
int dir = 1;	// 진행방향
int delay = 200;
int done = 0;

struct aiocb kbcbuf;

int set_ticker(int);

main()
{
	void on_alarm(int);	// alarm을 위한 핸들러
	void on_input(int);	// 키보드를 위한 핸들러
	void setup_aio_buffer();

	initscr();	// 화면 초기화
	crmode();
	noecho();
	clear();

	signal(SIGIO, on_input);	// 키보드 핸들러 초기화
	setup_aio_buffer();
	aio_read(&kbcbuf);

	signal(SIGALRM, on_alarm);	// alarm 핸들러 초기화
	set_ticker(delay);

	mvaddstr(row, col, MESSAGE);
	
	while(!done)	// 메인 루프
		pause();
	endwin();
}

void on_input(int s)	// aio_read()에서 에러 코드인지 확인한다
{
	int  c;
	char *cp = (char *)kbcbuf.aio_buf;	// 읽을 문자

	if( aio_error(&kbcbuf) != 0 )	// 에러 코드
		perror("reading failed");
	else
	 	// 읽을 문자의 개수를 가져온다
		if( aio_return(&kbcbuf) == 1 )
		{
			c = *cp;
			if( c == 'Q' || c == EOF )
				done = 1;
			else if( c == ' ' )
				dir = -dir;
		}

	aio_read(&kbcbuf);
}

void on_alarm(int s)
{
	signal(SIGALRM, on_alarm);	// 초기화
	mvaddstr( row, col, BLANK );	// 그 전 메시지를 지운다
	col += dir;			// 새로운 col로 움직인다
	mvaddstr( row, col, MESSAGE );	// 새로운 메시지를 그린다
	refresh();			// 보여준다

	if( dir == -1 && col <= 0 )
		dir = 1;
	else if( dir == 1 && col+strlen(MESSAGE) >= COLS )
		dir = -1;
}

void setup_aio_buffer()
{
	static char input[1];

	kbcbuf.aio_fildes = 0;
	kbcbuf.aio_buf = input;
	kbcbuf.aio_nbytes = 1;
	kbcbuf.aio_offset = 0;
	
	// read가 준비되었을 때
	kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	kbcbuf.aio_sigevent.sigev_signo = SIGIO;	// SIGIO를 보낸다
}
