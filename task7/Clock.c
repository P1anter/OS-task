#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 20 

int t = 0;
int fault[MAX_SIZE] = {0};

typedef struct Node {
    int data;
    int used;
    struct Node *link;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} Frame;

// 초기화 함수
void init(Frame *f) {
    f->head = NULL;
    f->tail = NULL;
}

// 프레임이 비어있는지 확인하는 함수
int is_empty(Frame *f) {
    return (f->head == NULL);
}

// 모든 노드의 used 값을 0으로 초기화하는 함수
void initializeUsed(Frame *f) {
    Node *temp = f->head;
    if (f->head != NULL && f->tail != NULL) {
        do {
            temp->used = 0;
            temp = temp->link;
        } while (temp != NULL);
    }
}

// 프레임의 현재 상태를 출력하는 함수
void print_frame(Frame *f) {
    int w = 0;
    printf("Frame at t = %d\n", t);
    Node *p;
    f->tail->link = f->head;
    for (p = f->head; p->link != f->head; p = p->link) {
        printf("%d : %d\n", p->data, p->used);
        w++;
        if (w >= 3)
            break;
    }
    printf("%d : %d\n", p->data, p->used);
}

// 프레임에 새로운 데이터를 추가하는 함수
void push(Frame *f, int item, Node *clock) {
    Node *temp = (Node *)malloc(sizeof(Node)); // 동적 노드 생성
    if (temp == NULL) { // 메모리 할당 확인
        printf("Memory allocation failed\n");
        return;
    }
    temp->data = item; // 새로운 노드에 입력할 값 복사
    temp->used = 1; // 사용 여부 설정
    temp->link = NULL;
    if (f->head == NULL) { // 프레임이 비어있는 경우
        f->head = temp;
        f->tail = temp;
        f->tail->link = f->head;
        *clock = *temp;
    } else { // 프레임이 비어있지 않은 경우
        Node *p = f->head;
        while (1) { // 모든 노드의 used 값을 0으로 설정
            p->used = 0;
            if (p->link == f->head)
                break;
            p = p->link;
        }
        f->tail->link = temp;
        f->tail = temp;
        f->tail->link = f->head;
        *clock = *temp; // 새 노드를 clock으로 설정
    }
}

// 페이지 부재 인덱스를 출력하는 함수
void print_fault() {
    int i;
    printf("페이지 부재 인덱스 출력:\n");
    for (i = 0; i < MAX_SIZE; i++) {
        if (fault[i] == 1)
            printf("%d ", i);
    }
}

// 프레임에서 데이터를 교체하는 함수
void *Replace(Frame *f, int data, Node *clock) {
    Node *new = (Node *)malloc(sizeof(Node)); // 동적 노드 생성
    if (new == NULL) { // 메모리 할당 확인
        printf("Memory allocation failed\n");
        return;
    }
    new->data = data; // 새로운 노드에 입력할 값 복사
    new->used = 1;

    Node *temp;
    temp = f->head;
    Node *p = f->head;
    while (1) { // 모든 노드의 used 값을 0으로 설정
        p->used = 0;
        if (p->link == f->head)
            break;
        p = p->link;
    }

    if (clock->link == f->head) { // clock이 첫 번째 노드인 경우
        new->link = f->head->link;
        f->head = new;
        f->tail->link = new;
    } else if (clock->link == f->tail) { // clock이 마지막 노드인 경우
        new->link = f->head;
        f->tail = new;
        f->head->link->link = new;
        f->head->link->link->link = f->head;
    } else { // clock이 중간 노드인 경우
        new->link = f->tail;
        *clock->link = *new;
    }
    *clock = *new;
    f->tail->link = f->head;
}

// 프레임에서 데이터를 찾는 함수
int Search(Frame *f, int data, Node *clock) {
    Node *temp = f->head;
    do {
        if (temp->data == data) {
            Node *p = f->head;
            while (1) { // 모든 노드의 used 값을 0으로 설정
                p->used = 0;
                if (p->link->data == f->head->data)
                    break;
                p = p->link;
            }
            temp->used = 1; // 사용 여부 업데이트
            *clock = *temp;
            return 1;
        }
        temp = temp->link;
    } while (temp->data != f->head->data);
    return 0;
}

// 시계 알고리즘을 사용하는 페이지 교체 함수
void clock_algorithm(int str[], int num_frames) {
    int n = 0;
    Frame f;
    init(&f);

    Node *clock = (Node *)malloc(sizeof(Node)); // 동적 노드 생성

    while (t < MAX_SIZE) {
        if (n < num_frames) { // 프레임이 꽉 차지 않은 경우
            push(&f, str[t], clock);
            n++;
            fault[t] = 1;
        } else if (!Search(&f, str[t], clock)) { // 프레임이 꽉 찼고, 데이터가 없는 경우
            Replace(&f, str[t], clock);
            fault[t] = 1;
        }
        t++;
        print_frame(&f);
    }
}

int main() {
    int reference_string[MAX_SIZE] = {1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3};
    int num_frames = 3;

    clock_algorithm(reference_string, num_frames);
    print_fault();
    return 0;
}
