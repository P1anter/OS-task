#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 링크드 리스트의 노드 구조체 정의
typedef struct Node {
    int data;
    struct Node* next;
} LinkedList;

// 큐의 구조체 정의
typedef struct {
    LinkedList* front;  // 큐의 시작 지점
    LinkedList* rear;   // 큐의 끝 지점
} Queue;

typedef struct {
    int end;
    int index;
} ThreadArg;

void *processThread(void *arg) {
    ThreadArg *threadArg = (ThreadArg *)arg;
    int index = threadArg->index;
    for (int s = 1; s <= threadArg->end; s++) {
        printf("P%d : %d x %d = %d\n", index + 1, s, index + 1, (index + 1) * s);
    }
    pthread_exit(NULL);
}

Queue queue; // 전역 변수로 큐 선언

// 큐 초기화 함수
void initQueue() {
    queue.front = NULL;
    queue.rear = NULL;
}

// 큐에 새로운 노드를 삽입하는 함수 (enqueue)
void enqueue(int data) {
    LinkedList* newNode = (LinkedList*)malloc(sizeof(LinkedList));
    newNode->data = data;
    newNode->next = NULL;

    if (queue.rear == NULL) {
        queue.front = newNode;
        queue.rear = newNode;
    } else {
        queue.rear->next = newNode;
        queue.rear = newNode;
    }
}

// 큐에서 노드를 제거하는 함수 (dequeue)
int dequeue() {
    if (queue.front == NULL) {
        return -1; // 큐가 비어있을 경우
    }

    LinkedList* temp = queue.front;
    int data = temp->data;

    if (queue.front == queue.rear) {
        queue.front = NULL;
        queue.rear = NULL;
    } else {
        queue.front = queue.front->next;
    }

    free(temp);
    return data;
}

int main() {
    int arrivalTime[5]; // 프로세스 도착 시간 배열
    int burstTime[5];   // 프로세스 실행 시간 배열
    int returnTime[5];  // 프로세스 반환 시간 배열
    int waitTime[5];    // 프로세스 대기 시간 배열
    double sumReturn = 0;
    double sumWait = 0;

    // 큐 초기화
    initQueue();

    pthread_t threads[5];

    // 프로세스 도착 시간과 실행 시간 입력 받기
    for (int i = 0; i < 5; i++) {
        printf("P%d의 도착 시간을 입력하세요: ", i + 1);
        scanf("%d", &arrivalTime[i]);
        printf("P%d의 실행 시간을 입력하세요: ", i + 1);
        scanf("%d", &burstTime[i]);

        // 큐에 데이터 삽입 (도착 시간)
        enqueue(arrivalTime[i]);
        // 큐에 데이터 삽입 (실행 시간)
        enqueue(burstTime[i]);
    }

    // 프로세스 처리 및 반환 시간, 대기 시간 계산
    int currentTime = 0; // 현재 시간
    for (int i = 0; i < 5; i++) {
        // 큐에서 프로세스의 도착 시간과 실행 시간 가져오기
        int arrival = dequeue();
        int burst = dequeue();

        // 현재 시간을 프로세스의 도착 시간으로 업데이트
        currentTime = (arrival > currentTime) ? arrival : currentTime;

        // 프로세스의 반환 시간 계산
        returnTime[i] = currentTime + burst;
        // 프로세스의 대기 시간 계산
        waitTime[i] = currentTime - arrival;

        // 다음 프로세스의 도착 시간이 현재 시간보다 이전일 경우 현재 시간을 업데이트
        currentTime = (queue.front != NULL && queue.front->data < currentTime + burst) ? currentTime : currentTime + burst;

        // 프로세스의 실행 시간을 현재 시간에 추가
        currentTime += burst;
    }

    printf("\n간트 차트\n");
    for(int i = 0; i < 5; i++) {
        printf("P%d (%d - %d)\n", i + 1, arrivalTime[i] + waitTime[i], returnTime[i]);
    }

    // 결과 출력
    printf("\n프로세스   반환 시간   대기 시간\n");
    for (int i = 0; i < 5; i++) {
        printf("P%d          %d           %d\n", i + 1, returnTime[i] - i, waitTime[i]);
        sumReturn += (returnTime[i] - i);
        sumWait += waitTime[i];
    }
    printf("\n평균 반환 시간: %.2f\n", sumReturn / 5.0);
    printf("평균 대기 시간: %.2f\n", sumWait / 5.0);

    ThreadArg threadArgs[5];
    for (int i = 0; i < 5; i++) {
        threadArgs[i].end = burstTime[i];
        threadArgs[i].index = i;
        if (pthread_create(&threads[i], NULL, processThread, (void *)&threadArgs[i]) != 0) {
            fprintf(stderr, "thread create error\n");
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
