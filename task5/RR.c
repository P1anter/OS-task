#include "schedule.h"

#define N 5
#define TIME_QUANTUM 5

Node* calculateRealtime(Node* Head);
void processing(Process* process);

int main(void) {
    // set head
    Node* Q = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    head->process.runtime = 999;
    InsertNewHead(&Q, head);

    // 입력부
    printf("준비 큐 입력\n");
    printf("번호 | 도착 시간 | 실행 시간\n");
    for (int i = 0; i < N; i++) {
        Node* node = (Node*)malloc(sizeof(Node));
        scanf("%d %d %d", &node->process.no, &node->process.arrive, &node->process.runtime);
        node->process.rt.end = node->process.runtime;
        node->next = NULL;
        Node* before = GetNodeAt(Q, i);
        InsertAfter(before, node);
    }

    printf("===================\n");
    printf("간트 차트\n");
    Node* NewQ = calculateRealtime(Q); // 새 큐(실행 큐)에 스케줄링 결과 저장

    int count = GetNodeCount(NewQ->next);
    int returnTime[N]; // 반환 시간
    int waitingTime[N]; // 대기 시간
    int processNo[N];   // 프로세스 번호 저장
    double avgRetTime = 0.0;
    double avgwaitTime = 0.0;
    for (int i = 0; i < count; i++) {
        // 간트 차트 출력
        Node* Current = GetNodeAt(NewQ, i+1);
        printf("P%d (%d-%d)\n", Current->process.no, Current->process.rt.start, Current->process.rt.end);
    }

    for (int i = 1; i <= N; i++) {
        int processed_time[N][N] = {{0,},};
        int processed = 0;
        Node* Current = NULL;
        Node* node = GetNodeAt(NewQ, 1);
        while (node != NULL) {
            if (node->process.no == i) { // 새 큐에서 현재 번호와 같은 프로세스 번호를 가진 노드를 찾음
                Current = node;
                // 실제 실행 시간 저장
                if ((Current->process.rt.end - Current->process.rt.start) > 0)
                    processed_time[i-1][processed] = (Current->process.rt.end - Current->process.rt.start);
                processed++;
            }
            node = node->next;
        }
        for (int j = 0; j < N-1; j++) {
            if (processed_time[i-1][j+1] == 0) break; // 다음 실행 시간이 0이면 종료
            processed_time[i-1][N-1] += processed_time[i-1][j]; // 현재 실행 시간을 더함
        }

        returnTime[i-1] = Current->process.rt.end - Current->process.arrive;
        waitingTime[i-1] = Current->process.rt.start - Current->process.arrive - processed_time[i-1][N-1];
        processNo[i-1] = i; // 프로세스 번호 저장
        avgRetTime += returnTime[i-1];
        avgwaitTime += waitingTime[i-1];
    }

    avgRetTime = avgRetTime / N;
    avgwaitTime = avgwaitTime / N;

    printf("===================\n");
    printf("프로세스 | 반환 시간 | 대기 시간\n");

    for (int i = 0; i < N; i++) {
        printf("P%d        | %2d        | %2d\n", processNo[i], returnTime[i], waitingTime[i]);
    }

    printf("전체 평균 반환시간: %.2lf\n", avgRetTime);
    printf("전체 평균 대기시간: %.2lf\n", avgwaitTime);
    printf("===================\n\n");
    printf("스케줄 실행\n");
    printf("===================\n");

    Node* Current = NewQ->next;
    while (Current != NULL) {
        Current->process.runtime = 1;
        Current = Current->next;
    }

    Current = NewQ->next;
    while (Current != NULL) {
        pid_t pid = fork();
        
        if (pid == 0) { // 자식 프로세스
            processing(&Current->process);
            exit(0);
        } else if (pid > 0) { // 부모 프로세스
            wait(NULL); // 자식 프로세스가 종료될 때까지 기다림
            Node* cur = Current->next;
            while (cur != NULL) {
                // 다른 노드들 중에 현재 프로세스와 같은 프로세스 번호를 가진 노드에 실제 실행한 시간을 저장
                if(cur->process.no == Current->process.no) {
                    cur->process.runtime = Current->process.runtime;
                }

                cur = cur->next;
            }
        } else {
            perror("fork");
            exit(1);
        }
        Current = Current->next;
    }

    return 0;
}

// 실제 실행에 필요한 시간 계산
Node* calculateRealtime(Node* Head) {
    // 새 큐 생성
    Node* NewQ = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    InsertNewHead(&NewQ, head);
    Node* Before = head; // 새 큐에서 이전 노드를 가리키는 포인터
    
    int t = 0;
    while (1) {
        Node* Current = Head->next;
        int allProcessed = 1;
        while (Current != NULL) {
            if (Current->process.rt.end > 0 && t >= Current->process.arrive) {
                allProcessed = 0;
                int execTime = (Current->process.rt.end >= TIME_QUANTUM) ? TIME_QUANTUM : Current->process.rt.end;

                Node* NewNode = (Node*)malloc(sizeof(Node));
                NewNode->next = NULL;
                NewNode->process = Current->process;
                NewNode->process.rt.start = t;
                NewNode->process.rt.end = t + execTime;
                
                t += execTime;
                Current->process.rt.end -= execTime;

                InsertAfter(Before, NewNode);
                Before = NewNode;
            }
            Current = Current->next;
        }
        if (allProcessed) break;
    }
    return NewQ;
}

// 프로세스 함수
void processing(Process* process) {
    int n = 0;
    for (int i = process->runtime; n < process->rt.end - process->rt.start; i++, n++) {
        printf("P%d: %d X %d = %d\n", process->no, i, process->no, i * process->no);
        process->runtime = i + 1; // 현재 실행한 시간을 저장
    }
}

// head 부분에 새 헤드 삽입
void InsertNewHead(Node** Head, Node* NewHead) {
    if (Head == NULL) {
        (*Head) = NewHead;
    } else {
        NewHead->next = (*Head);
        (*Head) = NewHead;
    }
}

// Current 뒤에 새 노드 삽입
void InsertAfter(Node* Current, Node* NewNode) {
    NewNode->next = Current->next;
    Current->next = NewNode;
}

// Location 위치의 노드를 반환
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

        if(Current != NULL) {
            Current->next = Remove->next;
        }
    }
    //free(Remove);
}
