#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 20    /* Queue 최대용량 */

int t = 0;

typedef struct Node { // 노드 정의
    int data;
    struct Node *next;
} Node;

typedef struct Queue { // Queue 구조체 정의
    Node *front; // 맨 앞(꺼낼 위치)
    Node *rear; // 맨 뒤(보관할 위치)
    int count; // 보관 개수
} Queue;

void InitQueue(Queue *queue); // 큐 초기화
int IsEmpty(Queue *queue); // 큐가 비었는지 확인
void EnQueue(Queue *queue, int data); // 큐에 보관
int DeQueue(Queue *queue); // 큐에서 꺼냄
int Search(Queue *queue, int data); // 큐에서 데이터 검색
void PrintFrame(Queue *queue); // 큐 내용 출력
void PrintQueue(Queue *queue); // 큐 인덱스 출력

void InitQueue(Queue *queue) {
    queue->front = queue->rear = NULL; // front와 rear를 NULL로 설정
    queue->count = 0; // 보관 개수를 0으로 설정
}

int IsEmpty(Queue *queue) {
    return queue->count == 0; // 보관 개수가 0이면 빈 상태
}

void EnQueue(Queue *queue, int data) {
    Node *newNode = (Node *)malloc(sizeof(Node)); // 노드 생성
    newNode->data = data; // 데이터 설정
    newNode->next = NULL;

    if (IsEmpty(queue)) { // 큐가 비어있을 때
        queue->front = newNode; // 맨 앞을 newNode로 설정
    } else { // 비어있지 않을 때
        queue->rear->next = newNode; // 맨 뒤의 다음을 newNode로 설정
    }
    queue->rear = newNode; // 맨 뒤를 newNode로 설정
    queue->count++; // 보관 개수를 1 증가
}

int DeQueue(Queue *queue) {
    int data = 0;
    Node *temp;
    if (IsEmpty(queue)) { // 큐가 비었을 때
        return data;
    }
    temp = queue->front; // 맨 앞의 노드를 temp에 기억
    data = temp->data; // 반환할 값은 temp의 data로 설정
    queue->front = temp->next; // 맨 앞은 temp의 다음 노드로 설정
    free(temp); // temp 소멸
    queue->count--; // 보관 개수를 1 감소
    return data;
}

int Search(Queue *queue, int data) { // 찾고자 하는 data의 큐에서의 index return, 없으면 0
    Node *current = queue->front;
    while (current != NULL) {
        if (current->data == data)
            return 1; // 찾았을 경우 1 반환
        current = current->next;
    }
    return 0; // 없을 경우 0 반환
}

void PrintFrame(Queue *queue) {
    Node *current = queue->front;
    printf("Frame at t = %d\n", t);
    while (current != NULL) {
        printf("%d\n", current->data);
        current = current->next;
    }
    printf("\n");
}

void PrintQueue(Queue *queue) {
    Node *current = queue->front;
    printf("페이지 부재 인덱스 출력:\n");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

Queue fifo_page_replacement(int reference_string[], int num_frames) {
    Queue pageQueue;
    InitQueue(&pageQueue);

    Queue indexQueue;
    InitQueue(&indexQueue);

    int frameCount = 0;
    while (t < MAX_SIZE) {
        if (frameCount == 0) {
            EnQueue(&pageQueue, reference_string[t]);
            EnQueue(&indexQueue, t);
            frameCount++;
        } else {
            if (Search(&pageQueue, reference_string[t]) == 0) {
                if (frameCount == num_frames) {
                    EnQueue(&indexQueue, t);
                    DeQueue(&pageQueue);
                    EnQueue(&pageQueue, reference_string[t]);
                } else {
                    EnQueue(&indexQueue, t);
                    EnQueue(&pageQueue, reference_string[t]);
                    frameCount++;
                }
            }
        }
        t++;
        PrintFrame(&pageQueue);
    }

    return indexQueue;
}

int main() {
    int reference_string[MAX_SIZE] = {1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3};
    int num_frames = 3;

    Queue resultQueue = fifo_page_replacement(reference_string, num_frames);
    PrintQueue(&resultQueue);

    return 0;
}
