#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

int idx = 1;
typedef struct
{
    int start;
    int end;
    int mul;
} T;

T t[4]; // 각 스레드의 구간 정보를 저장할 배열

void *func(void *arg)
{
    int thread_num = *((int *)arg);
    int mul = t[thread_num].mul;

    for (int k = t[thread_num].start; k <= t[thread_num].end; k++)
    {
        int result = mul * k;
        printf("Process %d - Thread %d: %d\n", idx, thread_num+1, result);
    }
    pthread_exit(NULL);
}

int main(void)
{

    struct timeval start, end;
    long sec, microsec;
    double times;
    int status;

    pthread_t tid[4]; //pthread_t 배열 선언

    pid_t pid1 = fork();
    pid_t pid2 = fork();

    gettimeofday(&start, NULL); // 작업 시간 측정 시작

    if (pid1 == 0)
    {
        idx *= 3;
    }
    if (pid2 == 0)
    {
        idx++;
    }
    int mul = (idx * 2) + 1; // 3,5,7,9 곱하는 값 mul

    for (int i = 0; i < 4; i++)
    {
        t[i].end = (i + 1) * (100 / 4);
        t[i].start = t[i].end - (100 / 4) + 1;
        t[i].mul = mul;
        int *thread_num = malloc(sizeof(int)); // 각 스레드에 전달할 인자를 동적으로 할당
        *thread_num = i;
        pthread_create(&tid[i], NULL, func, thread_num);
    }
    for (int j = 0; j < 4; j++)
    {
        pthread_join(tid[j], NULL);
    }

    if (pid2 == 0)
    {
        exit(2);
    } // 모든 자식 프로세스가 종료될 때까지 부모 프로세스 대기
    else if (pid1 == 0)
    {
        while (wait(&status) != pid2)
            continue;
        exit(2);
    }
    else
    {
        while (wait(&status) != pid1)
            continue;
    }

    gettimeofday(&end, NULL); // 작업 시간 측정 끝

    sec = end.tv_sec - start.tv_sec;
    microsec = end.tv_usec - start.tv_usec;
    times = sec + microsec / 1000000.0; // 작업시간을 초 단위로 저장

    printf("multi_process_thread time: %f sec\n", times); // 작업 수행시간 출력

    return 0;
}
