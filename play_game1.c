#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

extern int life;
int coin_input();

int play_game1()
{
	int newpid;
	int exitstatus;
	int result;	
	int coin_status;
	
//	printf("play game 1입니다.\n");
		
	if((newpid=fork())==-1)
		perror("fork");

	//자식 프로세스 game module 실행
	else if(newpid == 0)
	{
//		printf("자식입니다.\n");
		execlp("./level1game", "./level1game",(char*)NULL);
		// exec에 실패하면 아래의 에러 코드가 실행
		perror("execvp failed");
		exit(-1);
	}
	
	//부모 프로세스 경우
	else
	{
		while(wait(&exitstatus) != newpid)
			;
	//	printf("부모입니다.\n");
		// level 1 성공
		if((exitstatus>>8) == 0)
		{
		//	printf("level2 시작>>\n");
			sleep(2);
		//	play_game2();
			return 0;
		}

		else	// level1 실패
		{
			if(life == 0)
			{
				printf("남은 목숨이 없습니다.\n");
				sleep(1);
				printf("코인을 넣으십시오(제한시간 : 10초) (입력:y /  취소:n)\n");
				sleep(2);
				coin_status = coin_input();
				//coin_status = 1;
				//coin_status = getchar();
				if(coin_status == 0)
				{
					printf("코인을 넣으셨습니다.\n");
					sleep(2);
					life = 3;
					printf("현재 남은 목숨은 %d개 입니다.\n", life);
//					printf("level1 시작>> 게임 도중 게임을 포기하시려면 'Q'를 입력하세요)\n");
					sleep(2);
					//play_game1();
					return 1;
				}
				else
				{
					//printf("프로그램이 종료됩니다.!!\n");
					//sleep(2);
					return -1;
				}
				
			}
		
			else
			{
				life--;
				printf("현재 남은  목숨은 %d개 입니다.\n", life);
				sleep(2);
				//printf("level1 시작>>(게임포기는 'Q'를 입력하세요)\n");
				//play_game1();
				return 1;
			}
		}
	}
}		
