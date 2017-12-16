#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int life = 3;

int main()
{
	int newpid;
	int result1,result2,result3;	

	printf("Welcome!!\n");
	sleep(1);
	printf("프로그램 강제 종료는 CTRL+C 입력\n");
	sleep(1);
	printf("현재 목숨은 %d 개입니다\n", life);
	sleep(1);
	printf("Game을 실행합니다.\n");
	sleep(1);
	printf("level1 시작>> (게임 도중 게임을 포기하시려면 'Q'를 입력하십시오)\n");
	sleep(2);
	result1= play_game1();

	// game1 성공시 0반환 & game2실행
	if(result1==0)
		result2=play_game2();

	// 목숨을 다 잃었는데 코인을 넣지 않은경우 -1반환 & 프로그램종료
	else if(result1 == -1)
	{
		printf("프로그램이 종료됩니다.\n");
		return 0;
	}
	// 게임에 실패했거나 목숨을 잃은후 코인을 새로 넣었을때 1 반환 & game1재 실행
	else if(result1 == 1)
	{
		
	}

	return 0;
}

int play_game1()
{
	int newpid;
	int exitstatus;
	int result;	
	int coin_status;

	if((newpid=fork())==-1)
		perror("fork");

	//자식 프로세스 game module 실행
	else if(newpid == 0)
	{
		execlp("test", "test", "1000" ,0);
		// exec에 실패하면 아래의 에러 코드가 실행
		perror("execvp failed");
		exit(1);
	}
	
	//부모 프로세스 경우
	else
	{
		while(wait(&exitstatus) != pid)
			;

		// level 1 성공
		if((exitstatus>>8) == 0)
		{
			printf("level2 시작>>\n");
			sleep(2);
		//	play_game2();
			return 0;
		}

		else
		{
			if(life == 0)
			{
				printf("남은 목숨이 없습니다.");
				printf("코인을 넣으십시오(코인 넣기를 원하시면 'c'를 입력하십시오) : ");
				coin_status = coin_input();
				
				if(coin_status == 1)
				{
					life = 3;
					printf("현재 목숨은 %d개 입니다.\n", life);
					printf("level1 시작>> (게임포기는 'Q'를 입력하세요)\n");
					sleep(2);
					play_game1();
				}
				else
				{
					printf("프로그램이 종료됩니다.!!\n");
					sleep(3);
					return -1;
				}
				
			}
		
			else
			{
				life--;
				printf("현재 목숨은 %d개 입니다.\n", life);
				printf("level1 시작>>(게임포기는 'Q'를 입력하세요)\n");
				play_game1();
			}
		}
	}
		
}

int play_game2()
{
	
}
