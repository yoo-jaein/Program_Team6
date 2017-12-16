// file: bounce_async.c
// author: 2016110879_유재인
// datetime: 2017-11-21 09:18
// description: O_ASYNC를 사용한 유저컨트롤 애니메이션을 보여준다.

#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#define	MESSAGE	"hello"
#define	BLANK "     "

int row = 10;	 // 현재 row
int col = 0;	 // 현재 col
int dir = 1;	 // 진행 방향
int delay = 200; // delay 200ms (0.2sec)	
int done = 0;

int set_ticker(int);

main()
{
	void on_alarm(int);	// alarm
	void on_input(int);	// 사용자 input
	void enable_kbd_signals();

	initscr();		// cmd 화면 초기화
	crmode();
	noecho();
	clear();

	signal(SIGIO, on_input);          
	enable_kbd_signals();             
	signal(SIGALRM, on_alarm);        
	set_ticker(delay);	

	move(row,col);		// row와 col의 위치로 간다
	addstr(MESSAGE);	// 메시지를 보여준다

	while(!done)		// 메인 루프	 
		pause();
	endwin();
}

void on_input(int signum)
{
	int c = getch();		 

	if( c == 'Q' || c == EOF )
		done = 1;
	else if( c == ' ' )
		dir = -dir;
}

void on_alarm(int signum)
{
	signal(SIGALRM, on_alarm);	// 초기화	
	mvaddstr( row, col, BLANK );	
	col += dir;			
	mvaddstr( row, col, MESSAGE );
	refresh();	

	if( dir == -1 && col <= 0 )
		dir = 1;
	else if( dir == 1 && col+strlen(MESSAGE) >= COLS )
		dir = -1;
}

void enable_kbd_signals()
{
	int fd_flags;

	fcntl(0, F_SETOWN, getpid());
	fd_flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));
}

