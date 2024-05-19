#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 5

// 프로세스를 나타내는 구조체
typedef struct Process {
    int pid;             // 프로세스 ID
    int arrival_time;    // 도착 시간
    int burst_time;      // 실행 시간
    int completion_time; // 완료 시간
    int waiting_time;    // 대기 시간
    int turnaround_time; // 반환 시간
    int execution_order; // 실행 순서
    struct Process *next; // 다음 프로세스를 가리키는 포인터
} Process;

// 프로세스를 초기화하는 함수
Process *createProcess(int pid, int arrival_time, int burst_time) {
    Process *new_process = (Process *)malloc(sizeof(Process));
    new_process->pid = pid;
    new_process->arrival_time = arrival_time;
    new_process->burst_time = burst_time;
    new_process->completion_time = 0;
    new_process->waiting_time = 0;
    new_process->turnaround_time = 0;
    new_process->execution_order = 0;
    new_process->next = NULL;
    return new_process;
}

// 링크드 리스트에 새로운 프로세스를 추가하는 함수
Process *addProcess(Process *head, Process *new_process) {
    if (head == NULL) {
        return new_process;
    }
    Process *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_process;
    return head;
}

// 각 프로세스의 반환 시간과 대기 시간을 계산하고 출력하는 함수
void printTurnaroundAndWaitingTime(Process *head) {
    printf("\n각 프로세스의 반환 시간 및 대기 시간:\n");
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int num_processes = 0;
    Process *temp = head;
    while (temp != NULL) {
        total_turnaround_time += temp->turnaround_time;
        total_waiting_time += temp->waiting_time;
        num_processes++;
        printf("프로세스 %d: 반환 시간=%d, 대기 시간=%d\n", temp->pid, temp->turnaround_time, temp->waiting_time);
        temp = temp->next;
    }

    double avg_turnaround_time = (double)total_turnaround_time / num_processes;
    double avg_waiting_time = (double)total_waiting_time / num_processes;

    printf("\n전체 평균 반환 시간: %.2f\n", avg_turnaround_time);
    printf("전체 평균 대기 시간: %.2f\n", avg_waiting_time);
}

// 간트 차트를 출력하는 함수
void printGanttChart(int gantt_chart[], int num_ticks) {
    printf("\n간트 차트:\n");
    int current_process = -1;
    int start_time = 0;

    for (int i = 0; i <= num_ticks; ++i) {
        if (i == num_ticks || gantt_chart[i] != current_process) {
            if (current_process != -1) {
                printf(" P%d (%d-%d) \n", current_process, start_time, i);
            }

            if (i < num_ticks) {
                start_time = i;
                current_process = gantt_chart[i];
            }
        }
    }
    printf("\n");
}

// SJF 알고리즘을 구현한 함수
void SJF(Process *head, int num_processes, int gantt_chart[]) {
    int current_time = 0;
    int completed_processes = 0;
    int remaining_burst_time[MAX_PROCESSES] = {0}; // 각 프로세스의 남은 실행 시간

    while (completed_processes < num_processes) {
        Process *temp = head;
        int shortest_job = -1;
        int min_burst_time = __INT_MAX__;

        while (temp != NULL) {
            if (temp->arrival_time <= current_time && remaining_burst_time[temp->pid - 1] == 0 && temp->burst_time < min_burst_time) {
                shortest_job = temp->pid - 1;
                min_burst_time = temp->burst_time;
            }
            temp = temp->next;
        }

        if (shortest_job == -1) {
            current_time++;
            continue;
        }

        temp = head;
        while (temp != NULL && temp->pid != shortest_job + 1) {
            temp = temp->next;
        }

        remaining_burst_time[shortest_job] = temp->burst_time;
        temp->completion_time = current_time + temp->burst_time;
        temp->turnaround_time = temp->completion_time - temp->arrival_time;
        temp->waiting_time = temp->turnaround_time - temp->burst_time;
        completed_processes++;

        for (int i = 0; i < temp->burst_time; i++) {
            gantt_chart[current_time + i] = temp->pid;
        }

        current_time += temp->burst_time;
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

    int total_burst_time = 0;
    Process *temp = head;
    while (temp != NULL) {
        total_burst_time += temp->burst_time;
        temp = temp->next;
    }

    int gantt_chart[total_burst_time + 1]; // 프로세스가 실행되는 시간을 기록하는 배열

    // SJF 알고리즘 실행
    SJF(head, MAX_PROCESSES, gantt_chart);

    // 간트 차트 출력
    printGanttChart(gantt_chart, total_burst_time);

    // 프로세스의 실행 결과 출력
    printf("\n프로세스의 실행 결과:\n");
    for (int i = 0; i < total_burst_time; ++i) {
        int pid = gantt_chart[i];
        temp = head;
        while (temp != NULL && temp->pid != pid) {
            temp = temp->next;
        }
        if (temp != NULL) {
            printf("P%d: %d × %d = %d\n", pid, temp->execution_order + 1, pid, (temp->execution_order + 1) * pid);
            temp->execution_order++;
        }
    }

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