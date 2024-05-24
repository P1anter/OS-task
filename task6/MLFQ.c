#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define TIME_QUANTUM_Q1 1
#define TIME_QUANTUM_Q2 2
#define TIME_QUANTUM_Q3 4

typedef struct {
    int no;
    int arrive;
    int runtime;
    struct {
        int start;
        int end;
    } rt;
    int accumulated_runtime;
    int current_multiplier;
} Process;

typedef struct Node {
    Process process;
    struct Node* next;
} Node;

Node* calculateRealtime(Node* Head);
void processing(Process* process);
void InsertNewHead(Node** Head, Node* NewHead);
void InsertAfter(Node* Current, Node* NewNode);
Node* GetNodeAt(Node* Head, int Location);
int GetNodeCount(Node* Head);
void RemoveNode(Node** Head, Node* Remove);

int main(void) {
    int numProcesses;
    printf("프로세스 개수를 입력하세요: ");
    scanf("%d", &numProcesses);

    Node* Q = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    head->process.runtime = 999;
    InsertNewHead(&Q, head);

    printf("준비 큐 입력\n");
    printf("번호 | 실행 시간\n");

    int input[numProcesses][2];
    for (int i = 0; i < numProcesses; i++) {
        scanf("%d %d", &input[i][0], &input[i][1]); // 프로세스 번호와 실행시간 입력받기

        Node* node = (Node*)malloc(sizeof(Node));
        node->process.no = input[i][0];
        node->process.arrive = 0;
        node->process.runtime = input[i][1];
        node->process.rt.end = node->process.runtime;
        node->process.accumulated_runtime = 0; // 누적 실행 시간 초기화
        node->process.current_multiplier = 1; // 초기 곱셈 값 설정
        node->next = NULL;
        Node* before = GetNodeAt(Q, i);
        InsertAfter(before, node);
    }

    printf("===================\n");
    printf("간트 차트\n");
    Node* NewQ = calculateRealtime(Q); // 새 큐(실행 큐)에 스케줄링 결과 저장

    int count = GetNodeCount(NewQ->next);
    int returnTime[numProcesses]; // 반환 시간
    int waitingTime[numProcesses]; // 대기 시간
    int processNo[numProcesses];   // 프로세스 번호 저장
    double avgRetTime = 0.0;
    double avgwaitTime = 0.0;

    int currentQuantum = -1;
    Node* current = NewQ->next;
    while (current != NULL) {
        int execTime = current->process.rt.end - current->process.rt.start;
        int quantum;
        if (execTime == TIME_QUANTUM_Q1) {
            quantum = 1;
        } else if (execTime == TIME_QUANTUM_Q2) {
            quantum = 2;
        } else {
            quantum = 3;
        }

        if (currentQuantum != quantum) {
            if (currentQuantum != -1) {
                printf("\n");
            }
            currentQuantum = quantum;
            printf("Q%d: ", currentQuantum);
        } else {
            printf(", ");
        }

        Node* temp = current;
        while (temp->next != NULL && temp->next->process.no == current->process.no &&
               temp->next->process.rt.start == temp->process.rt.end) {
            temp = temp->next;
        }

        printf("P%d (%d-%d)", current->process.no, current->process.rt.start, temp->process.rt.end);
        current = temp->next;
    }
    printf("\n");


    for (int i = 1; i <= numProcesses; i++) {
        int processed_time[numProcesses][numProcesses];
        for (int j = 0; j < numProcesses; j++) {
            processed_time[i - 1][j] = 0;
        }

        int processed = 0;
        Node* Current = NULL;
        Node* node = GetNodeAt(NewQ, 1);
        while (node != NULL) {
            if (node->process.no == i) {
                Current = node;
                if ((Current->process.rt.end - Current->process.rt.start) > 0)
                    processed_time[i - 1][processed] = (Current->process.rt.end - Current->process.rt.start);
                processed++;
            }
            node = node->next;
        }
        for (int j = 0; j < numProcesses - 1; j++) {
            if (processed_time[i - 1][j + 1] == 0) break;
            processed_time[i - 1][numProcesses - 1] += processed_time[i - 1][j];
        }

        returnTime[i - 1] = Current->process.rt.end - Current->process.arrive;
        waitingTime[i - 1] = returnTime[i - 1] - input[i - 1][1];
        processNo[i - 1] = i;
        avgRetTime += returnTime[i - 1];
        avgwaitTime += waitingTime[i - 1];
    }

    avgRetTime = avgRetTime / numProcesses;
    avgwaitTime = avgwaitTime / numProcesses;

    printf("===================\n");
    printf("프로세스 | 반환 시간 | 대기 시간\n");

    for (int i = 0; i < numProcesses; i++) {
        printf("P%d        | %2d        | %2d\n", processNo[i], returnTime[i], waitingTime[i]);
    }

    printf("전체 평균 반환시간: %.2lf\n", avgRetTime);
    printf("전체 평균 대기시간: %.2lf\n", avgwaitTime);
    printf("===================\n\n");

    return 0;
}

Node* calculateRealtime(Node* Head) {
    Node* NewQ = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    InsertNewHead(&NewQ, head);
    Node* Before = head;

    int t = 0;
    int remainingProcesses = GetNodeCount(Head) - 1;

    while (1) {
        Node* Current = Head->next;
        int allProcessed = 1;
        while (Current != NULL) {
            if (Current->process.rt.end > 0 && t >= Current->process.arrive) {
                allProcessed = 0;

                int execTime;
                if (Current->process.accumulated_runtime == 0 || remainingProcesses == 1) {
                    execTime = TIME_QUANTUM_Q1;
                } else if (Current->process.accumulated_runtime == 1) {
                    execTime = TIME_QUANTUM_Q2;
                } else if (Current->process.accumulated_runtime >= 3) {
                    if (Current->process.runtime - Current->process.accumulated_runtime == 1) {
                        execTime = TIME_QUANTUM_Q1;
                    } else if (Current->process.runtime - Current->process.accumulated_runtime == 2) {
                        execTime = TIME_QUANTUM_Q2;
                    } else {
                        execTime = TIME_QUANTUM_Q3;
                    }
                } else {
                    execTime = TIME_QUANTUM_Q3;
                }

                if (Current->process.runtime < execTime) {
                    execTime = Current->process.runtime;
                }

                Node* NewNode = (Node*)malloc(sizeof(Node));
                NewNode->next = NULL;
                NewNode->process = Current->process;
                NewNode->process.rt.start = t;
                NewNode->process.rt.end = t + execTime;

                t += execTime;
                Current->process.rt.end -= execTime;
                Current->process.runtime -= execTime;
                Current->process.accumulated_runtime += execTime;

                InsertAfter(Before, NewNode);
                Before = NewNode;

                // 프로세스 실행 및 처리
                pid_t pid = fork();
                if (pid == 0) {
                    processing(&NewNode->process);
                    exit(0);
                } else if (pid > 0) {
                    wait(NULL);
                    if (Current->process.runtime == 0) {
                        remainingProcesses--;
                    }
                } else {
                    perror("fork");
                    exit(1);
                }
            }
            Current = Current->next;
        }
        if (allProcessed) break;
    }
    return NewQ;
}

void processing(Process* process) {
    int n = 0;
    for (int i = process->accumulated_runtime + 1; n < process->rt.end - process->rt.start; i++, n++) {
        printf("P%d: %d * %d = %d\n", process->no, i, process->no, i * process->no);
        process->current_multiplier = i; // 다음 실행을 위해 현재 곱셈 값을 업데이트
    }
}

void InsertNewHead(Node** Head, Node* NewHead) {
    if (Head == NULL) {
        (*Head) = NewHead;
    } else {
        NewHead->next = (*Head);
        (*Head) = NewHead;
    }
}

void InsertAfter(Node* Current, Node* NewNode) {
    NewNode->next = Current->next;
    Current->next = NewNode;
}

Node* GetNodeAt(Node* Head, int Location) {
    Node* Current = Head;
    while (Current != NULL && (--Location) >= 0) {
        Current = Current->next;
    }
    return Current;
}

int GetNodeCount(Node* Head) {
    int Count = 0;
    Node* Current = Head;
    while (Current != NULL) {
        Current = Current->next;
        Count++;
    }
    return Count;
}

void RemoveNode(Node** Head, Node* Remove) {
    if (*Head == Remove) {
        *Head = Remove->next;
    } else {
        Node* Current = *Head;
        while (Current != NULL && Current->next != Remove) {
            Current = Current->next;
        }

        if (Current != NULL) {
            Current->next = Remove->next;
        }
    }
}
