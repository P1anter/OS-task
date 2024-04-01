#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

int main() {

    struct timeval start, end;
    long sec, microsec;
    double times;

    gettimeofday(&start, NULL);
    
    for (int i = 1; i <= 1000; i++) {
        printf("%d\n", i * 7);
    }

        gettimeofday(&end, NULL);

        sec = end.tv_sec - start.tv_sec;
        microsec = end.tv_usec - start.tv_usec;
        times = sec + microsec / 1000000.0;

        printf("single_process time: %f sec\n", times);
    }



