#include <stdio.h>
#include <pthread.h>
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>
#define THREAD_NUM 4
#define MAX 100
 
int var = 1;
int flag[THREAD_NUM] = {0, };
int turn = 0;

void *func(void *arg) {
        int i = (int)(intptr_t)arg;
        int next = (i + 1) % THREAD_NUM;
	int task = MAX / THREAD_NUM;
	
	int j;
	int sum = 0;
	int count = 0;
	
	while (1) {
		
		while (1) {		
		
			flag[i] = 1; 
			while (turn != i) {
				if (flag[turn] == 0) {
					turn = i;
				}
			}
			
			flag[i] = 2;
			j = 0;
			
			while ((j < THREAD_NUM) && (j == i || flag[j] != 2)) 
				j = j + 1;
				
			if (j >= THREAD_NUM)
				break;
		
		}
		
		while(count < task && var <= MAX) {
			printf("THREAD %d  value = %d\n", i+1, var * 3);
			
			var++;
			count++;
		}
		
		flag[i] = 0;
		
		if(var > 100 || count >= task) {
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
	printf("%d THREADS - PROGRAM DONE\n", THREAD_NUM);
	return 0;

}

