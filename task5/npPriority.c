#include "schedule.h"

#define N 5

Node* calculateRealtime(Node* Head);
void processing(Process* process);

int main(void) {

    // 헤드 설정
    Node* Q = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    InsertNewHead(&Q, head);

    // 입력부
    printf("준비 큐 입력\n");
    printf("번호 | 도착 시간 | 실행 시간 | 우선순위\n");
    for (int i = 0; i < N; i++) {
        Node* node = (Node*)malloc(sizeof(Node));
        scanf("%d %d %d %d", &node->process.no, &node->process.arrive, &node->process.runtime, &node->process.priority);

        node->next = NULL;
        Node* before = GetNodeAt(Q, i);
        InsertAfter(before, node);
    }
    printf("===================\n");
    printf("간트 차트\n");
    Node* NewQ = calculateRealtime(Q);

    int returnTime[N];
    int waitingTime[N];
    int processNo[N];
    double avgRetTime = 0.0;
    double avgwaitTime = 0.0;
    for (int i = 0; i < N; i++) {
        Node* Current = GetNodeAt(NewQ, i+1);
        printf("P%d (%d-%d)\n", Current->process.no, Current->process.rt.start, Current->process.rt.end);
        returnTime[i] = Current->process.rt.end - Current->process.arrive;
        waitingTime[i] = Current->process.rt.start - Current->process.arrive;
        processNo[i] = Current->process.no;
        avgRetTime += returnTime[i];
        avgwaitTime += waitingTime[i];
    }

    avgRetTime = avgRetTime / N;
    avgwaitTime = avgwaitTime / N;

    printf("===================\n");
    printf("프로세스 번호 | 반환 시간 | 대기 시간\n");

    for (int i = 0; i < N; i++) {
        printf("P%d            %2d        | %2d\n", processNo[i], returnTime[i], waitingTime[i]);
    }

    printf("전체 평균 반환시간: %.2lf\n", avgRetTime);
    printf("전체 평균 대기시간: %.2lf\n", avgwaitTime);
    printf("===================\n\n");
    printf("스케줄 실행\n");
    printf("===================\n");

    Node* Current = NewQ->next->next;
    while (Current != NULL) {
        pid_t pid = fork();
        if (pid == 0) { // 자식 프로세스
            processing(&Current->process);
            exit(0); // 실행 후 자식 프로세스 종료
        } else if (pid > 0) { // 부모 프로세스
            waitpid(pid, NULL, 0); // 자식 프로세스가 완료될 때까지 대기
        } else {
            fprintf(stderr, "Fork 실패\n");
            return 1;
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
    Node* Before = head;

    int t = 0;
    while (Head->next != NULL) {
        Node* Current = Head->next;
        Node* Min = NULL;
        Node* Remove = NULL; // 삭제 할 노드

        // 제일 높은 우선순위 노드 찾기
        while (Current != NULL) {
            if (Current->process.arrive <= t && (Min == NULL || 
                Current->process.priority > Min->process.priority || 
                (Current->process.priority == Min->process.priority && Current->process.arrive < Min->process.arrive))) {
                Min = Current; // 가장 높은 우선순위 프로세스를 설정
                Remove = Current; // 삭제할 것을 설정해둠
            }

            Current = Current->next;
        }

        if (Min == NULL) {
            t++;
            continue;
        }

        Min->process.rt.start = (Before->process.rt.end > Min->process.arrive) ? Before->process.rt.end : Min->process.arrive;
        Min->process.rt.end = Min->process.rt.start + Min->process.runtime;
        t = Min->process.rt.end;

        Node* MinCopy = (Node*)malloc(sizeof(Node));
        MinCopy->process = Min->process;
        MinCopy->next = NULL;

        InsertAfter(Before, MinCopy); // 새 큐에 삽입하고
        RemoveNode(&Head, Remove); // 기존 큐에서 삭제

        Before = Before->next;
    }

    return NewQ;
}

// 프로세스 함수
void processing(Process* process) {
    for (int i = 1; i <= process->rt.end - process->rt.start; i++) {
        printf("P%d: %d X %d = %d\n", process->no, i, process->no, i * process->no);
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