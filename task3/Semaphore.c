#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_NUM 4
#define MAX 100

int  v = 1;
sem_t semaphore;

void* func(void* arg) {
    int task = MAX / THREAD_NUM;
    int count = 0;
    int thread_id = *((int*)arg);

    sem_wait(&semaphore);
    
    while(count < task && v <= MAX) {
			printf("THREAD %d  value = %d\n", thread_id+1, v * 3);
			
			v++;
			count++;
		}
        
    sem_post(&semaphore);
    if(v > 100 || count >= task) {
			printf("THREAD %d count = %d\n", thread_id+1, count);
			pthread_exit(NULL);
		}
    

    return NULL;
}

int main() {
    pthread_t threads[THREAD_NUM];
    sem_init(&semaphore, 0, 1); // 세마포어 초기화

    printf("Semaphore test Start!\n");

    for (int i = 0; i < THREAD_NUM; i++) {
        int *thread_id = malloc(sizeof(int));
        *thread_id = i;
        pthread_create(&threads[i], NULL, func,thread_id);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("모든 스레드가 숫자 출력을 완료했습니다.\n");

    sem_destroy(&semaphore); // 세마포어 객체 소멸

    return 0;
}