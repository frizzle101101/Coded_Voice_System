#ifndef QUEUE_H
#define QUEUE_H

typedef struct node {
	struct node *next;
	void *data;
}NODE;

typedef struct {
	NODE *head;
	NODE *tail;
	int count;
}QUEUE;

QUEUE *queue_init(void);
int enqueue(QUEUE *userQ, NODE *userNode);
NODE *dequeue(QUEUE *userQ);
int print_Queue(QUEUE *userQ);
int destroy_queue(QUEUE *userQ);
void printQ_recurFW(NODE *inNode);
void printQ_recurBW(NODE *inNode);

#endif /* QUEUE_H */