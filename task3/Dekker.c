#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#define THREAD_NUM 2
#define MAX 100

int v = 1; // 전역변수 선언
int flag[THREAD_NUM] = {0, }; //플래그 초기화
int turn = 0;  // 턴 초기화


void *func(void *arg) {           //스레드 실행 함수
	int i = (int)(intptr_t)arg;
	int next = (i + 1) % THREAD_NUM;   // 다음 스레드 계산
	int task = MAX / THREAD_NUM;  // 한 스레드가 수행해야할 숫자 갯수

	int count = 0;  // 출력 갯수
	
	while (1) {
		flag[i] = 1;  //임계영역 진입희망
		
		while (flag[next] == 1) {  // 다른 스레드가 진입희망하는 동안
			if (turn == next) {  // 턴도 다른 스레드에게 있으면
				flag[i] = 0;    //내 플래그 0으로 바꿈
				while(turn == next) {}   // 턴이 넘어올때까지 대기
				flag[i] = 1;  // 다시 진입희망
			}
		}
		
		while(count < task && v <= MAX) {  // 정해진 갯수만큼 출력
			printf("THREAD %d  value = %d\n", i+1, v * 3);
			
			v++;
			count++;
		}
		
		
		turn = next;  // 작업이 끝났으므로 턴을 넘기고
		flag[i] = 0;  // 플래그도 바꿈
		
		if(v > MAX || count >= task) {
			printf("THREAD %d count = %d\n", i+1, count);
			break;
		}
		

	}
	
	return NULL;

} 


int main() {
	pthread_t tid[THREAD_NUM];
	
	for(long i = 0; i < THREAD_NUM; i++) {
		if(pthread_create(&tid[i], NULL, func, (void *) i) != 0)   // 스레드 생성
		{ 
			fprintf(stderr, "thread create error\n");
			exit(1);
		}
	}
	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tid[i], NULL);      
	}
	printf("%d THREADS - PROGRAM DONE\n", THREAD_NUM);
	return 0;

}
