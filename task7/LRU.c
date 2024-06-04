#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 20 

int t = 0;
int faults[MAX_SIZE] = {0};

typedef struct Node{
	int data;
	int last_ref;
	struct Node *next;
}Node;

typedef struct{
	Node *top;
}Stack;

void init(Stack *s) {
	s->top = NULL;
}

int is_empty(Stack *s) {
	return (s->top == NULL);
}

void push(Stack *s, int item) {
	Node *temp = (Node *)malloc(sizeof(Node));
	temp->data = item;
	temp->last_ref = t;
	temp->next = s->top;
	s->top = temp;
}

int pop(Stack *s) {
	if (is_empty(s)) {
		printf("Stack is empty\n");
	} else {
		Node *temp = s->top;
		int data = temp->data;
		s->top = s->top->next;
		free(temp);
		return data;
	}
}

void print_stack(Stack *s) {
	printf("Stack at t = %d\n", t);
	Node *p;
	for (p = s->top; p != NULL; p = p->next)
		printf("%d : %d\n", p->data, p->last_ref);
	printf("\n");
}

void print_faults() {
	printf("페이지 부재 인덱스 출력:\n");
	for (int i = 0; i < MAX_SIZE; i++) {
		if (faults[i] == 1)
			printf("%d ", i);
	}
}

int search(Stack *s, int data) {
	Node *temp = s->top;
	while (temp != NULL) {
		if (temp->data == data) {
			temp->last_ref = t;
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

void replace(Stack *s, int data) {
	Node *new_node = (Node *)malloc(sizeof(Node));
	if (new_node == NULL) {
		printf("Memory allocation failed\n");
		return;
	}
	new_node->data = data;
	new_node->last_ref = t;

	Node *temp = s->top;
	Node *oldest = s->top;
	Node *prev_oldest = NULL;

	while (temp != NULL && temp->next != NULL) {
		if (temp->next->last_ref < oldest->last_ref) {
			prev_oldest = temp;
			oldest = temp->next;
		}
		temp = temp->next;
	}
	if (prev_oldest != NULL) {
		prev_oldest->next = new_node;
		new_node->next = oldest->next;
		free(oldest);
	} else {
		new_node->next = s->top->next;
		free(s->top);
		s->top = new_node;
	}
}

void LRU_page_replacement(int ref_str[], int num_frames) {
	int n = 0;
	Stack s;
	init(&s);

	while (t < MAX_SIZE) {
		if (n == 0) {
			push(&s, ref_str[t]);
			faults[t] = 1;
			n++;
		} else if (search(&s, ref_str[t]) == 0) {
			if (n == num_frames) {
				faults[t] = 1;
				replace(&s, ref_str[t]);
			} else {
				faults[t] = 1;
				push(&s, ref_str[t]);
				n++;
			}
		}
		t++;
		print_stack(&s);
	}
}

int main() {
	int ref_str[MAX_SIZE] = {1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3};
	int num_frames = 3;

	LRU_page_replacement(ref_str, num_frames);
	print_faults();
	return 0;
}
