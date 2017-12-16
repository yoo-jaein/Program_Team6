// file: bounce2d.c
// author: 2016110879_유재인
// datetime: 2017-11-15 16:50
// description: Default_Symbol인 'o'를 x축방향, y축방향으로 움직이는 애니메이션.
// 사용자의 입력에 따라 x축방향 속도와 y축방향 속도를 각각 조절한다.
// 타이머 역할을 하는 2개의 카운터가 있다.

#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <termios.h>

#include "bounce.h"

struct ppball the_ball;

int set_ticker(int);
int bounce_or_lose(struct ppball*);
void set_up();
void wrap_up();

int main()
{
	int c;

	set_up();

	while( ( c = getchar()) != 'Q' ){	// 키보드 입력에 따라 속도가 변한다
		if( c == 'f' )	    the_ball.x_ttm--;
		else if( c == 's' ) the_ball.x_ttm++;
		else if( c == 'F' ) the_ball.y_ttm--;
		else if( c == 'S' ) the_ball.y_ttm++;
	}

	wrap_up();
}


void set_up()  // 초기화한다
{
	void ball_move(int);

	the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;
	the_ball.y_ttg = the_ball.y_ttm = Y_TTM;
	the_ball.x_ttg = the_ball.x_ttm = X_TTM;
	the_ball.y_dir = 1;
	the_ball.x_dir = 1;
	the_ball.symbol = DFL_SYMBOL;

	initscr();
	noecho();
	crmode();

	signal( SIGINT , SIG_IGN );
	mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol  );
	refresh();
	
	signal( SIGALRM, ball_move );
	set_ticker( 1000 / TICKS_PER_SEC );
}

void wrap_up()
{

	set_ticker(0);
	endwin();	// 기본으로 돌아간다
}

void ball_move(int signum)
{
	int y_cur, x_cur, moved;

	signal( SIGALRM , SIG_IGN );
	y_cur = the_ball.y_pos;
	x_cur = the_ball.x_pos;
	moved = 0;

	if( the_ball.y_ttm > 0 && the_ball.y_ttg-- == 1 ){
		the_ball.y_pos += the_ball.y_dir;	/* move	*/
		the_ball.y_ttg = the_ball.y_ttm;	/* reset*/
		moved = 1;
	}

	if( the_ball.x_ttm > 0 && the_ball.x_ttg-- == 1 ){
		the_ball.x_pos += the_ball.x_dir;	/* move	*/
		the_ball.x_ttg = the_ball.x_ttm;	/* reset*/
		moved = 1;
	}

	if( moved ){
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
		bounce_or_lose( &the_ball );
		move(LINES-1,COLS-1);
		refresh();
	}
	signal(SIGALRM, ball_move);	
}

int bounce_or_lose(struct ppball *bp)
{
	int return_val = 0;

	if( bp->y_pos == TOP_ROW ){
		bp->y_dir = 1; 
		return_val = 1;
	} else if( bp->y_pos == BOT_ROW ){
		bp->y_dir = -1;
	       	return_val = 1;
	}

	if( bp->x_pos == LEFT_EDGE ){
		bp->x_dir = 1;
	       	return_val = 1;
	} else if( bp->x_pos == RIGHT_EDGE ){
		bp->x_dir = -1;
	       	return_val = 1;
	}

	return return_val;
}
