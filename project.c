#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int life = 1;

int play_game1();
int coin_input();

int main()
{
	int newpid;
	int result1,result2,result3;	

	printf("Welcome!!\n");
	sleep(1);
	printf("프로그램 강제 종료는 CTRL+C 입력\n");
	sleep(1);
	printf("현재 남은 목숨은 %d 개입니다\n", life);
	sleep(1);
	printf("Game을 실행합니다.\n");
	sleep(1);
	printf("level1 시작>> (게임 도중 게임을 포기하시려면 'Q'를 입력하십시오)\n");
	sleep(2);

	while(1)
	{
//		printf("while loop안입니다.\n");
		result1 = play_game1();
		if(result1==0)
		{
			// 성공 & 다음단계로~~
			break;
		}

		// 목숨을 다잃었는데 코인을 넣지 않으면
		if(result1 == -1)
		{
			printf("프로그램이 종료됩니다.\n");
			sleep(2);
			return 0;
		}
		// 게임에 실패했는데 목숨이 남아있으면 혹은 목숨을 다잃었는데 코인을 넣으면
		else if(result1==1)
		{
			printf("level재시작>>(게임 도중 게임을 포기하시려면 'Q'를 입력하십시오)\n");
			sleep(2);
		}
		else
		{
			fprintf(stderr, "예상치 못한 결과 입니다.\n");
			exit(EXIT_FAILURE);
		}
	}
	printf("\nlevel2가 시작됩니다.>>\n");
//	while()

	return 0;
}

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
		execlp("./test", "./test", "1000" ,(char*)NULL);
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
				printf("남은 목숨이 없습니다.");
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
					life = 1;
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

/*int play_game2()
{
	
}*/
