#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

extern int life;
int coin_input();

int play_game2()
{
	int newpid;
	int exitstatus;
	int result;
	int coin_status;

	if((newpid=fork())==-1)
		perror("fork");

	else if(newpid ==0)
	{
		execlp("./level2game", "./level2game", (char*)NULL);
		perror("execvp failed");
		exit(-1);
	}

	else
	{
		while(wait(&exitstatus)!=newpid)
			;

		if((exitstatus>>8)==0)
		{
			sleep(2);	// 2초 뒤에 다음단계 실행
			return 0;
		}

		else
		{
			if(life == 0)
			{
				printf("남은 목숨이 없습니다.\n");
				sleep(1);
				printf("코인을 넣으십시오(제한시간: 10초) (입력:y / cnlth : n)\n");
				sleep(2);
				coin_status = coin_input();

				if(coin_status==0)
				{
					printf("코인을 넣으셨습니다.\n");
					sleep(2);
					life = 3;
					printf("현재 남은 목숨은 %d개 입니다.\n", life);
					// 부활!!!!!!!!!!!!!!!
					sleep(2);
					return 1;
				}
				else
				{
					return -1;
				}
			}
			
			else
			{
				life--;
				printf("현재 남은 목숨은 %d개 입니다.\n", life);
				sleep(2);

				return 1;
			}
		}
	}
}
