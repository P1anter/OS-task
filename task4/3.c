#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 5

// 프로세스를 나타내는 구조체
typedef struct Process {
    int pid;             // 프로세스 ID
    int arrival;         // 도착 시간
    int burst;           // 실행 시간
    int completion;      // 완료 시간
    int waiting;         // 대기 시간
    int turnaround;      // 반환 시간
    int count;           // 현재까지 곱한 횟수
    struct Process *next; // 다음 프로세스를 가리키는 포인터
} Process;

// 프로세스를 초기화하는 함수
Process *createProcess(int pid, int arrival, int burst) {
    Process *proc = (Process *)malloc(sizeof(Process));
    proc->pid = pid;
    proc->arrival = arrival;
    proc->burst = burst;
    proc->completion = 0;
    proc->waiting = 0;
    proc->turnaround = 0;
    proc->count = 0;
    proc->next = NULL;
    return proc;
}

// 링크드 리스트에 새로운 프로세스를 추가하는 함수
Process *addProcess(Process *head, Process *newProc) {
    if (head == NULL) {
        return newProc;
    }
    Process *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newProc;
    return head;
}

// 간트 차트를 출력하는 함수
void printGanttChart(int chart[], int ticks) {
    printf("\n간트 차트:\n");
    int current = -1;
    int start = 0;

    for (int i = 0; i <= ticks; ++i) {
        if (i == ticks || chart[i] != current) {
            if (current != -1) {
                printf(" (%d-%d)\n", start, i);
            }

            if (i < ticks) {
                start = i;
                printf("P%d", chart[i]);
                current = chart[i];
            }
        }
    }
}

// 각 프로세스의 반환 시간과 대기 시간을 계산하고 출력하는 함수
void printTurnaroundAndWaitingTime(Process *head) {
    printf("\n각 프로세스의 반환 시간 및 대기 시간:\n");
    int totalTurnaround = 0;
    int totalWaiting = 0;
    int numProc = 0;
    Process *temp = head;
    while (temp != NULL) {
        totalTurnaround += temp->turnaround;
        totalWaiting += temp->waiting;
        numProc++;
        printf("프로세스 %d: 반환 시간=%d, 대기 시간=%d\n", temp->pid, temp->turnaround, temp->waiting);
        temp = temp->next;
    }

    double avgTurnaround = (double)totalTurnaround / numProc;
    double avgWaiting = (double)totalWaiting / numProc;

    printf("\n전체 평균 반환 시간: %.2f\n", avgTurnaround);
    printf("전체 평균 대기 시간: %.2f\n", avgWaiting);
}

// 각 프로세스의 실행 결과를 출력하는 함수
void printProcessResults(int chart[], Process *head, int ticks) {
    printf("\n프로세스의 실행 결과:\n");

    for (int i = 0; i < ticks; ++i) {
        int pid = chart[i];
        Process *temp = head;
        while (temp != NULL && temp->pid != pid) {
            temp = temp->next;
        }
        if (temp != NULL) {
            temp->count++; // 카운트 증가
            printf("P%d: %d × %d = %d\n", pid, temp->count, pid, temp->count * pid);
        }
    }
}

// SJF 알고리즘을 구현한 함수
void SJF(Process *head, int numProc, int chart[]) {
    int currentTime = 0;
    int completedProc = 0;
    int remainingBurst[MAX_PROCESSES] = {0}; // 각 프로세스의 남은 실행 시간
    int lastProcStartTime[MAX_PROCESSES] = {0}; // 각 프로세스의 마지막 작업 시작 시간

    while (completedProc < numProc) {
        // 새로운 프로세스가 도착하면 큐에 추가하고 현재 실행중인 프로세스와 실행 시간을 비교
        Process *temp = head;
        while (temp != NULL) {
            if (temp->arrival == currentTime) {
                remainingBurst[temp->pid - 1] = temp->burst;
            }
            temp = temp->next;
        }

        int shortestJob = -1;
        int minBurst = __INT_MAX__;

        temp = head;
        while (temp != NULL) {
            if (remainingBurst[temp->pid - 1] > 0 && remainingBurst[temp->pid - 1] < minBurst) {
                shortestJob = temp->pid - 1;
                minBurst = remainingBurst[temp->pid - 1];
            }
            temp = temp->next;
        }

        if (shortestJob == -1) {
            currentTime++;
            continue;
        }

        temp = head;
        while (temp != NULL && temp->pid != shortestJob + 1) {
            temp = temp->next;
        }

        if (lastProcStartTime[shortestJob] == 0) {
            lastProcStartTime[shortestJob] = currentTime;
        }

        remainingBurst[shortestJob]--;
        chart[currentTime] = temp->pid;
        currentTime++;

        if (remainingBurst[shortestJob] == 0) {
            temp->completion = currentTime;
            temp->turnaround = temp->completion - temp->arrival;
            temp->waiting = temp->turnaround - temp->burst;
            completedProc++;
        }
    }
}

int main() {
    // 미리 정의된 프로세스와 실행 시간
    Process *head = NULL;
    head = addProcess(head, createProcess(1, 0, 10));
    head = addProcess(head, createProcess(2, 1, 28));
    head = addProcess(head, createProcess(3, 2, 6));
    head = addProcess(head, createProcess(4, 3, 4));
    head = addProcess(head, createProcess(5, 4, 14));

    int totalBurst = 0;
    Process *temp = head;
    while (temp != NULL) {
        totalBurst += temp->burst;
        temp = temp->next;
    }

    int chart[totalBurst + 1]; // 프로세스가 실행되는 시간을 기록하는 배열

    // SJF 알고리즘 실행
    SJF(head, MAX_PROCESSES, chart);

    // 간트 차트 출력
    printGanttChart(chart, totalBurst);

    // 프로세스의 실행 결과 출력
    printProcessResults(chart, head, totalBurst);

    // 각 프로세스의 반환시간과 대기시간 출력
    printTurnaroundAndWaitingTime(head);

    // 메모리 해제
    temp = head;
    while (temp != NULL) {
        Process *next = temp->next;
        free(temp);
        temp = next;
    }

    return 0;
}
