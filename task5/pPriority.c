#include "schedule.h"

#define N 5

Node* cal_time(Node* Head); // 실제 실행 시간 등을 계산하는 함수
void processing(Process* process); // 각 프로세스를 실행하는 함수

int main(void) {
    // set head
    Node* Q = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    head->process.runtime = 999;
    InsertNewHead(&Q, head);

    // 입력부
    printf("준비 큐 입력\n");
    printf("번호 | 도착 시간 | 실행 시간 | 우선순위\n");
    for (int i = 0; i < N; i++) {
        Node* node = (Node*)malloc(sizeof(Node));
        scanf("%d %d %d %d", &node->process.no, &node->process.arrive, &node->process.runtime, &node->process.priority);
        node->process.rt.end = node->process.runtime;
        node->next = NULL;
        Node* before = GetNodeAt(Q, i);
        InsertAfter(before, node);
    }
    printf("\n간트 차트\n");
    Node* NewQ = cal_time(Q); // 새 큐(실행 큐)에 스케줄링 결과 저장

    int count = GetNodeCount(NewQ->next);
    int returnTime[N]; // 반환 시간
    int waitingTime[N]; // 대기 시간
    int processNum[N]; // 프로세스 번호
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
        processNum[i-1] = Current->process.no; // 프로세스 번호 저장
        avgRetTime += returnTime[i-1];
        avgwaitTime += waitingTime[i-1];
    }

    avgRetTime = avgRetTime / N;
    avgwaitTime = avgwaitTime / N;

    printf("\n프로세스 번호 | 반환 시간 | 대기 시간\n");

    for (int i = 0; i < N; i++) {
        printf("P%d            %2d        | %2d\n", processNum[i], returnTime[i], waitingTime[i]);
    }

    printf("\n전체 평균 반환시간: %.2lf\n", avgRetTime);
    printf("전체 평균 대기시간: %.2lf\n", avgwaitTime);

    printf("\n스케줄 실행\n");

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
Node* cal_time(Node* Head) {
    // 새 큐 생성
    Node* NewQ = NULL;
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    InsertNewHead(&NewQ, head);
    Node* Before = head; // 새 큐에서 이전 노드를 가리키는 포인터
    
    int t = 0;
    int i = 0;
    while (Head->next->next != NULL) {
        Node* Current = Head->next;
        Node* Min = (Node*)malloc(sizeof(Node)); // 기존 큐에서 가장 실행시간이 작은 노드를 가리킴
        Min->process.runtime = 999; // 초기값 설정
        Min->process.priority = 0;
        Min->next = NULL;
        Node* C = NULL;

        int change = 0; // 프로세스를 교환할 지 여부를 가리키는 flag

        // 실행 시간이 가장 작은 노드를 찾아서 Min의 process에 복사
        while (Current != NULL) {
            if (Min->process.priority < Current->process.priority && t >= Current->process.arrive && Current->process.rt.end > 0) {
                Min->process = Current->process; // 프로세스 정보만 복사
                C = Current; // 가장 작은 노드를 가리키도록 설정
            }
            Current = Current->next;
        }

        // 실행시간이 가장 작은 노드가 변경된 경우에 플래그 on
        if(Before->process.no != Min->process.no) change = 1; 

        // 교환 플래그가 켜진 경우
        if(change == 1) {
            Node* NewNode = (Node*)malloc(sizeof(Node)); // 새 노드를 생성하고 Min의 프로세스 정보를 복사
            NewNode->next = NULL;
            NewNode->process = Min->process;
            Before->process.rt.end = t; // 새 큐에 있는 이전 노드의 종료 시간을 현재 시간으로 지정
            NewNode->process.rt.start = Before->process.rt.end; // 새 노드의 시작 시간은 이전 노드의 종료 시간과 같음

            // t가 0인 경우에는 바로 삽입
            // t가 0이 아닌 경우에는 새 큐의 이전 노드와 같은 노드를 기존 큐에서 찾고 검사 진행
            if(t != 0) {
                Node* cur = Head->next;
                while (cur != NULL) {
                    if (cur->process.no == Before->process.no) break;
                    cur = cur->next;
                }
                if (cur->process.rt.end <= 0) { // 실행할 시간이 남아 있지 않은 경우
                    RemoveNode((&Head), cur); // 기존 큐에서 삭제
                }
            }

            InsertAfter(Before, NewNode); // 노드 삽입
            i++;
            Before = Before->next;
        }

        Node* lastNode = GetNodeAt(NewQ, i); // 마지막 노드인 경우
        lastNode->process.rt.end = lastNode->process.rt.start + lastNode->process.runtime; // 시작 시간 + 실행 시간
        C->process.rt.end -= 1; // Min의 원본 노드에서 남은 실행 시간을 1씩 뺌.

        t += 1;
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
}
