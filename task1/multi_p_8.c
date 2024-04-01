#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <math.h>

int main() {

    struct timeval start, end;
    long sec, microsec;
    double times;

    int i = 1;
    int status;
    int count = 0;
    int process_count;
    
    pid_t pid1 = fork(); // 8개의 프로세스 생성을 위해 fork() 3번 사용
    count++;             //  fork() 실행 횟수 저장
    pid_t pid2 = fork(); // 각각의 fork()후 생성되는 프로세스 구분을 위해 번호 붙힘
    count++;
    pid_t pid3 = fork();
    count++;

    process_count = pow(2, count); // fork n번에 프로세스 갯수는 2^n개가 됨으로 fork 횟수를 이용하여 프로세스 갯수 저장 

    gettimeofday(&start, NULL); // 작업 시간 측정 시작

    if (pid1 > 0) {
        i += 4;   // 프로세스를 구분하기 위해 fork수행 결과에 따라 i값 증가
        if (pid2 > 0) {
            i += 2;
        }
        if (pid3 > 0) {                
            i += 1;
        }
    }
    else {
        if (pid2 > 0) {
            i += 2;
        }
        if (pid3 > 0) {                
            i += 1;
        }
    }
    int first = 125 * (i - 1) + 1;  // 1~125, 126~250 ... 구분하기 위해 i사용
    int last = i * 125;

    for (int j = first; j <= last; j++) {      // 반복문으로 각 프로세스에 7 곱해서 출력
        printf("process_%d : %d\n", i, j * 7);
    }
    

    if(pid3== 0) {exit(2);}   // 모든 자식 프로세스가 종료될 때까지 부모 프로세스 대기 
    else if(pid2 == 0) {
        while(wait(&status) != pid3) continue;
        exit(2);
    } else if(pid1 == 0) {
        while(wait(&status) != pid2) continue;
        exit(2);
    } else {
        while(wait(&status) != pid1) continue;
    } 

    gettimeofday(&end, NULL); // 작업 시간 측정 끝

    sec = end.tv_sec - start.tv_sec;
    microsec = end.tv_usec - start.tv_usec;
    times = sec + microsec / 1000000.0; // 작업시간을 초 단위로 저장

    printf("multi_process time: %f sec\n", times); // 작업 수행시간 출력
    printf("total_process_count: %d\n", process_count);
    
}