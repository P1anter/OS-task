#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

int main(void)
{
    struct timeval start, end;
    long sec, microsec;
    double times;

    gettimeofday(&start, NULL); // 작업 시간 측정 시작

    for (int j = 3; j <=9; j+=2) {
        for(int i = 1; i <= 100; i++){
            printf("%d\n", i*j);
        }

    }
    gettimeofday(&end, NULL); // 작업 시간 측정 끝

    sec = end.tv_sec - start.tv_sec;
    microsec = end.tv_usec - start.tv_usec;
    times = sec + microsec / 1000000.0; // 작업시간을 초 단위로 저장

    printf("single_process_thread time: %f sec\n", times); // 작업 수행시간 출력

    return 0;
}