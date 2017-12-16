#include <curses.h>
#include <signal.h>
#include "bounce.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <aio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


struct ppball ball1;
struct ppball ball2;
struct ppball ball3;
struct ppball ball4;

struct ppball my_ball;
struct aiocb kbcbuf;


void set_up();
void wrap_up();
int set_ticker(long);
int bounce_or_lose(struct ppball*);
long timeval;
int done=0;

int check_condition()
{
	int result = 1;

	if((my_ball.x_pos==ball1.x_pos)&&(my_ball.y_pos==ball1.y_pos))
		result = 0;
	
	if((my_ball.x_pos==ball2.x_pos)&&(my_ball.y_pos==ball2.y_pos))
		result = 0;

	if((my_ball.x_pos==ball3.x_pos)&&(my_ball.y_pos==ball3.y_pos))
		result = 0;

	if((my_ball.x_pos==ball4.x_pos)&&(my_ball.y_pos==ball4.y_pos))
		result = 0;

	return result;
}

double get_time_diff(struct timeval tv1, struct timeval tv2)
{
	double result=0;
	int tv1_sec = (int)tv1.tv_sec;
	int tv1_usec = (int)tv1.tv_usec;
	int tv2_sec = (int)tv2.tv_sec;
	int tv2_usec = (int)tv2.tv_usec;
	
	if(tv1_usec > tv2_usec)
	{
		tv2_sec = tv2_sec-1;
		tv2_usec = tv2_usec + 1000000;
	}
	
	result += tv2_sec - tv1_sec;
	result += (tv2_usec - tv2_usec)/1000000.0;
	
	return result;
}

void refresh_util(int moved,int y_cur, int x_cur, struct ppball* ball);
void util(struct ppball* ball, int* moved);

int main(int argc, char* argv[])
{

	struct timeval tv1, tv2;
	int time_sec, time_usec;

	int c;
	

	double time_diff_ms;

	if(strcmp(argv[1], "1000")==0)
	{
		timeval = 1000;
	}


	set_up();
	gettimeofday(&tv1, NULL);

	while(!done)
	{
		pause();

		// check_condition return 이 0이면 부딪친 공이 있단 뜻
		if((check_condition())==0)
		{

			gettimeofday(&tv2, NULL);

			time_diff_ms = get_time_diff(tv1, tv2);
			set_ticker(0);
			break;
		}
	}



	
	if(done==1)// 'Q' 입력으로 게임 종료시킨경우
	{
		set_ticker(0);
		signal(SIGIO, SIG_IGN);

		clear();
		printw("You've dropped the game, so your life has been decreased!!\n");
		printw("Enter any two keys.....");
		refresh();

		getch();	// 아무키나 누르세요
		wrap_up();
		exit(1);	// 정상종료 아님!!!
	}
	else
	{
		signal(SIGIO, SIG_IGN);

		clear();
		printw("ELAPSED!!!\n");
		printw("It takes %.2lf seconds\n", time_diff_ms);
	
		if(time_diff_ms > 20.0)
		{
			printw("mission SUCCESS!!\n");
			printw("Press any two keys...");
			refresh();

			getch();
			wrap_up();
			exit(0);
		}
		else
		{
			printw("mission FAIL!!\n");
			printw("Press any key...");
			refresh();
			
			getch();
			wrap_up();
			exit(1);
		}	// complete 실패
	}

	return 0;
}


void set_up()
{
	void	ball_move(int);

	//
	void on_input(int);
	void setup_aio_buffer();

	signal(SIGIO, on_input);
	setup_aio_buffer();
	aio_read(&kbcbuf);

	my_ball.y_pos = 12;
	my_ball.x_pos = 40;
	my_ball.symbol = 'x';
	//
	
	////////////// ball1 setting
	ball1.y_pos = 6;
	ball1.x_pos = 11;
	ball1.y_ttg=ball1.y_ttm=2;
	ball1.x_ttg=ball1.x_ttm=2;
	ball1.y_dir=1;
	ball1.x_dir=1;
	ball1.symbol = 'z';

	//////////// ball2 setting
	ball2.y_pos = 19;
	ball2.x_pos = 11;
	ball2.y_ttg=ball2.y_ttm=3;
	ball2.x_ttg=ball2.x_ttm=3;
	ball2.y_dir= -1;
	ball2.x_dir= 1;
	ball2.symbol = 'y';

	//////////// ball3 setting
	ball3.y_pos = 11;
	ball3.x_pos = 69;
	ball3.y_ttg=ball3.y_ttm=4;
	ball3.x_ttg=ball3.x_ttm=4;
	ball3.y_dir=-1;
	ball3.x_dir=-1;
	ball3.symbol = 'v';
	
	
	//////////// ball4 setting
	ball4.y_pos = 6;
	ball4.x_pos = 69;
	ball4.y_ttg=ball4.y_ttm=5;
	ball4.x_ttg=ball4.x_ttm=5;
	ball4.y_dir=1;
	ball4.x_dir=-1;
	ball4.symbol = 'w';


	initscr();
	noecho();
	crmode();
	clear();

	signal( SIGINT , SIG_IGN );
	mvaddch( ball1.y_pos, ball1.x_pos, ball1.symbol);
	mvaddch( ball2.y_pos, ball2.x_pos, ball2.symbol);
	mvaddch( ball3.y_pos, ball3.x_pos, ball3.symbol);
	mvaddch( ball4.y_pos, ball4.x_pos, ball4.symbol);

	//
	mvaddch(my_ball.y_pos, my_ball.x_pos, my_ball.symbol);
	move(LINES-1, COLS-1);
	//
	refresh();

	

	signal( SIGALRM, ball_move );

	set_ticker(timeval);
	set_ticker(1000/TICKS_PER_SEC);
}



void wrap_up()
{
	set_ticker( 0 );
	endwin();		/* put back to normal	*/
}

void ball_move(int signum)
{
	int y_cur1, x_cur1, moved1;
	int y_cur2, x_cur2, moved2;
	int y_cur3, x_cur3, moved3;
	int y_cur4, x_cur4, moved4;

	signal( SIGALRM , SIG_IGN );		/* dont get caught now 	*/

	y_cur1 = ball1.y_pos ;		/* old spot		*/
	x_cur1 = ball1.x_pos ;
	moved1 = 0;

	y_cur2 = ball2.y_pos;
	x_cur2 = ball2.x_pos;
	moved2 = 0;

	y_cur3 = ball3.y_pos;
	x_cur3 = ball3.x_pos;
	moved3 = 0;

	y_cur4 = ball4.y_pos;
	x_cur4 = ball4.x_pos;
	moved4 = 0;

	util(&ball1, &moved1);
	util(&ball2, &moved2);
	util(&ball3, &moved3);
	util(&ball4, &moved4);

	refresh_util(moved1, y_cur1, x_cur1, &ball1);
	refresh_util(moved2, y_cur2, x_cur2, &ball2);
	refresh_util(moved3, y_cur3, x_cur3, &ball3);
	refresh_util(moved4, y_cur4, x_cur4, &ball4);

//	refresh();

/*	if(ball1.y_ttm>0 && ball1.y_ttg-- ==1)
	{
		ball1.y_pos += ball1.y_dir;
		ball1.y_ttg = ball1.y_ttm;
		moved1 = 1;
	}
	if(ball1.x_ttm>0 && ball1.x_ttg--==1)
	{
		ball1.x_pos += ball1.x_dir;
		ball1.x_ttg = ball1.x_ttm;
		moved1 = 1;
	}

	if ( moved1 ){
		mvaddch( y_cur1, x_cur1, BLANK );
		mvaddch( y_cur1, x_cur1, BLANK );
		mvaddch(ball1.y_pos, ball1.x_pos, ball1.symbol);

		bounce_or_lose( &ball1 );
		move(LINES-1, COLS-1);
		
		refresh();
	}*/
	

	signal( SIGALRM, ball_move);		/* for unreliable systems */
}

void util(struct ppball* ball, int* moved)
{
	if(ball->y_ttm>0 && ball->y_ttg-- == 1)
	{
		ball->y_pos += ball->y_dir;
		ball->y_ttg = ball->y_ttm;
		*moved = 1;
	}

	if(ball->x_ttm>0 && ball->x_ttg-- == 1)
	{
		ball->x_pos += ball->x_dir;
		ball->x_ttg = ball->x_ttm;
		*moved = 1;
	}
}

void refresh_util(int moved,int y_cur, int x_cur, struct ppball* ball)
{
	if(moved)
	{
		mvaddch(y_cur, x_cur, BLANK);
		mvaddch(y_cur, x_cur, BLANK);
		mvaddch(ball->y_pos, ball->x_pos, ball->symbol);

		bounce_or_lose(ball);
		move(LINES-1, COLS-1);

		refresh();
	}
}



int bounce_or_lose(struct ppball *bp)
{
	int	return_val = 0 ;

	if ( bp->y_pos == TOP_ROW ){
		bp->y_dir = 1 ; 
		return_val = 1 ;
	} else if ( bp->y_pos == BOT_ROW ){

		bp->y_dir = -1 ;

	       	return_val = 1;

	}

	if ( bp->x_pos == LEFT_EDGE ){

		bp->x_dir = 1 ;

	       	return_val = 1 ;
	} else if ( bp->x_pos == RIGHT_EDGE ){
		bp->x_dir = -1;
	       	return_val = 1;
	}

	return return_val;
}

void on_input(int sig)
{
	int  c;

	char *cp = (char *) kbcbuf.aio_buf;

	int y_cur = my_ball.y_pos;
	int x_cur = my_ball.x_pos;

	
	if ( aio_error(&kbcbuf) != 0 )

		perror("reading failed");

	else 
	{
		
		if ( aio_return(&kbcbuf) == 1 )

		{

			c = *cp;

			if ( c == 'Q' || c == EOF )

				done = 1;

			else 
			{
			
				if(c=='w')
				{
					if(my_ball.y_pos!=TOP_ROW)
						my_ball.y_pos--;
				}
				else if(c=='s')
				{
					if(my_ball.y_pos!=BOT_ROW)
						my_ball.y_pos++;
				}
				else if(c=='a')
				{
					if(my_ball.x_pos!=LEFT_EDGE)
						my_ball.x_pos--;
				}
				else if(c=='d')
				{
					if(my_ball.x_pos!=RIGHT_EDGE)
						my_ball.x_pos++;
				}
				else ;

				mvaddch(y_cur, x_cur, BLANK);
				mvaddch(y_cur, x_cur, BLANK);
				mvaddch(my_ball.y_pos, my_ball.x_pos, my_ball.symbol);
				move(LINES-1, COLS-1);
				refresh();
				//aio_read(&kbcbuf);
			}
		}
	}
	aio_read(&kbcbuf);
}

void setup_aio_buffer()
{
	static char input[1];

	kbcbuf.aio_fildes     = 0;

	kbcbuf.aio_buf        = input;

	kbcbuf.aio_nbytes     = 1;

	kbcbuf.aio_offset     = 0;


	kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;

	kbcbuf.aio_sigevent.sigev_signo  = SIGIO;
}
