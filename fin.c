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


struct ppball the_ball ;
struct ppball my_ball;
struct aiocb kbcbuf;


void set_up();
void wrap_up();
int set_ticker(long);
int bounce_or_lose(struct ppball*);

int done=0;
int timer_value;

int main(int argc, char* argv[])
{
	int c;

	int start_time;
	int end_time;	
	float time_diff_ms;

	if(strcmp(argv[1],"1000")==0)
		timer_value = 1000;
	else
		timer_value = 2000;

	start_time = clock();

	set_up();

	while(!done)
	{
		pause();

		if((the_ball.x_pos==my_ball.x_pos)&&(the_ball.y_pos==my_ball.y_pos))
		{
			set_ticker(0);
			break;
		}
	}

	end_time = clock();
	
	time_diff_ms = (end_time - start_time)/1000.0;

	if(done==1)// 'Q' 입력으로 게임 종료시킨경우
	{
		printf("게임을 포기하셨습니다. 목숨이 하나 줄어듭니다.\n");
		sleep(10);
		printf("\nwhat the fuck\n");
		wrap_up();
		exit(1);	// 정상종료 아님!!!
	}
	else
	{
		signal(SIGIO, SIG_IGN);
		printf("\n\nNice Catch!!!\n");
		printf("%.2lf seconds 결렸습니다.\n", time_diff_ms);
	
		if(time_diff_ms < 10.0)
		{
			printf("mission 성공!!\n");
			sleep(2);
			wrap_up();
			exit(0);
		}
		else
		{
			printf("mission 실패!!\n");
			sleep(2);
			wrap_up();
			exit(1);
		}	// complete 실패
	}

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
	

	the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;

	the_ball.y_dir = 1  ;
	the_ball.x_dir = 1  ;
	the_ball.symbol = DFL_SYMBOL ;


	initscr();
	noecho();
	crmode();
	clear();

	signal( SIGINT , SIG_IGN );
	mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol);

	//
	mvaddch(my_ball.y_pos, my_ball.x_pos, my_ball.symbol);
	move(LINES-1, COLS-1);
	//
	refresh();

	

	signal( SIGALRM, ball_move );

//	set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
	set_ticker(timer_value);
}



void wrap_up()
{
	set_ticker( 0 );
	endwin();		/* put back to normal	*/
}

void ball_move(int signum)
{
	int	y_cur, x_cur, moved;

	signal( SIGALRM , SIG_IGN );		/* dont get caught now 	*/

	y_cur = the_ball.y_pos ;		/* old spot		*/
	x_cur = the_ball.x_pos ;

	moved =1;

	if ( moved ){
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( y_cur, x_cur, BLANK );

		the_ball.x_pos += the_ball.x_dir;
		the_ball.y_pos += the_ball.y_dir;

		mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
		bounce_or_lose( &the_ball );

		move(LINES-1, COLS-1);
		
		refresh();

	}

	signal( SIGALRM, ball_move);		/* for unreliable systems */
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
