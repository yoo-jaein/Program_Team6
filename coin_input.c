#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define	ASK		"Enter the coin! in 10 seconds (코인 넣으시려면 'y'를 넣지않으시려면 'n'을 입력하세요)"
#define	TRIES	   4	                                   /* max tries */
#define SLEEPTIME  2                                       /* time per try */
#define	BEEP       putchar('\a')                           /* alert user */

int get_response(char*, int);
int get_ok_char();
void set_cr_noecho_mode();
void set_nodelay_mode();
void tty_mode(int);

int coin_input()
{
	int	response;

	tty_mode(0);				/* save current mode	*/

	set_cr_noecho_mode();			/* set -icanon, -echo	*/
	set_nodelay_mode();			/* noinput => EOF	*/

	response = get_response(ASK, TRIES);	/* get some answer	*/

	tty_mode(1);				/* restore orig mode	*/

	return response;

}

int get_response( char *question , int maxtries)
{
	int	input;

	printf("%s\n", question);			/* ask		*/
	fflush(stdout);					/* force output	*/

	while ( 1 ){
		sleep(SLEEPTIME);			/* wait a bit	*/

		input = tolower(get_ok_char());        	/* get next chr */
		if ( input == 'y' )
			return 0;

		if ( input == 'n' )
			return 1;

		if ( maxtries-- == 0 )			/* outatime?	*/
		{
			printf("time over\n");
			return 2;			/* sayso	*/
		}

		BEEP;
		printf("left time : %d\n", SLEEPTIME*(maxtries+1));
	}
}


int get_ok_char()
{
	int c;
	while( ( c = getchar() ) != EOF && strchr("yYnN",c) == NULL )
		;

	return c;
}



void set_cr_noecho_mode()
{
	struct	termios	ttystate;

	tcgetattr( 0, &ttystate);		/* read curr. setting	*/
	ttystate.c_lflag    	&= ~ICANON;	/* no buffering		*/
	ttystate.c_lflag    	&= ~ECHO;	/* no echo either	*/
	ttystate.c_cc[VMIN]  	=  1;		/* get 1 char at a time	*/
	tcsetattr( 0 , TCSANOW, &ttystate);	/* install settings	*/
}

void set_nodelay_mode()
{
	int	termflags;

	termflags = fcntl(0, F_GETFL);		/* read curr. settings	*/
	termflags |= O_NDELAY;			/* flip on nodelay bit	*/
	fcntl(0, F_SETFL, termflags);		/* and install 'em	*/
}




void tty_mode(int how)
{
	static struct termios original_mode;
	static int            original_flags;

	if ( how == 0 ){
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
	}

	else {
		tcsetattr(0, TCSANOW, &original_mode); 
		fcntl( 0, F_SETFL, original_flags);	
	}
}
