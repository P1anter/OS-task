#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#define THREAD_NUM 2
#define MAX 100

int v = 1;
int flag[THREAD_NUM] = {0, };
int turn = 0;


void *func(void *arg) {
	int i = (intptr_t)arg;
	int next = (i + 1) % THREAD_NUM;
	int task = MAX / THREAD_NUM;
	
	int count = 0;
	
	while (1) {
		
		flag[i] = 1;
		turn = next;
		while ((flag[next] == 1) && turn == next) { }
		
		while(count < task && v <= MAX) {
			printf("THREAD %d  value = %d\n", i+1, v * 3);
			
			v++;
			count++;
		}
		
		flag[i] = 0;
		
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
		if(pthread_create(&tid[i], NULL, func, (void *) i) != 0) 
		{ 
			fprintf(stderr, "thread create error\n");
			exit(1);
		}
	}
	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;

}
