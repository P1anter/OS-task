#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct realtime {
    int start;
    int end;
} realtime;

typedef struct Process {
    int arrive; // 도착 시간
    int runtime; // 실행 시간
    int priority; // 우선순위
    realtime rt; // 실제로 실행될 시간
    int no; // 프로세스 번호
} Process;

typedef struct Node {
    Process process;
    struct Node* next;
} Node;

void InsertNewHead(Node** Head, Node* NewHead);
void InsertAfter(Node* Current, Node* NewNode);
Node* GetNodeAt(Node* Head, int Location);
int GetNodeCount(Node* Head);
void RemoveNode(Node** Head, Node* Remove);

#endif