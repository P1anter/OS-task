#include "schedule.h"

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

void processing(Process* process) {
    for (int i = 1; i <= process->rt.end - process->rt.start; i++) {
        printf("P%d: %d X %d = %d\n", process->no, i, process->no, i * process->no);
    }
}

void HRN(Node* head, int numProc, Node** executionOrder) {
    int currentTime = 0;
    int completedProc = 0;
    Node* temp;

    while (completedProc < numProc) {
        double maxPriority = -1.0;
        Node* selectedProc = NULL;

        temp = head;
        while (temp != NULL) {
            if (temp->process.arrive <= currentTime && temp->process.rt.end == 0) {
                int waitingTime = currentTime - temp->process.arrive;
                double priority = (double)(waitingTime + temp->process.runtime) / temp->process.runtime;
                if (priority > maxPriority) {
                    maxPriority = priority;
                    selectedProc = temp;
                }
            }
            temp = temp->next;
        }

        if (selectedProc != NULL) {
            selectedProc->process.rt.start = currentTime;
            selectedProc->process.rt.end = currentTime + selectedProc->process.runtime;
            currentTime += selectedProc->process.runtime;
            executionOrder[completedProc] = selectedProc;
            completedProc++;
        } else {
            currentTime++;
        }
    }
}

int main() {
    int N = 5;
    Node* Q = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    InsertNewHead(&Q, head);

    printf("준비 큐 입력\n");
    printf("번호 | 도착 시간 | 실행 시간\n");
    for (int i = 0; i < N; i++) {
        Node* node = (Node*)malloc(sizeof(Node));
        scanf("%d %d %d", &node->process.no, &node->process.arrive, &node->process.runtime);
        node->next = NULL;
        Node* before = GetNodeAt(Q, i);
        InsertAfter(before, node);
    }

    Node* executionOrder[N];
    HRN(Q->next, N, executionOrder);

    printf("===================\n");
    printf("간트 차트\n");

    int returnTime[N];
    int waitingTime[N];
    double avgRetTime = 0.0;
    double avgwaitTime = 0.0;
    for (int i = 0; i < N; i++) {
        Node* Current = executionOrder[i];
        printf("P%d (%d-%d)\n", Current->process.no, Current->process.rt.start, Current->process.rt.end);
        returnTime[Current->process.no - 1] = Current->process.rt.end - Current->process.arrive;
        waitingTime[Current->process.no - 1] = Current->process.rt.start - Current->process.arrive;
    }

    printf("===================\n");
    printf("프로세스 번호 | 반환 시간 | 대기 시간\n");

    for (int i = 0; i < N; i++) {
        printf("P%d            %2d        | %2d\n", i + 1, returnTime[i], waitingTime[i]);
        avgRetTime += returnTime[i];
        avgwaitTime += waitingTime[i];
    }

    avgRetTime = avgRetTime / N;
    avgwaitTime = avgwaitTime / N;

    printf("전체 평균 반환시간: %.2lf\n", avgRetTime);
    printf("전체 평균 대기시간: %.2lf\n", avgwaitTime);
    printf("===================\n\n");

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            processing(&executionOrder[i]->process);
            exit(0);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        } else {
            fprintf(stderr, "Fork 실패\n");
            return 1;
        }
    }

    return 0;
}
