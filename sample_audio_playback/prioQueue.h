#pragma once

#include "queue.h"

typedef struct prionode{
	QUEUE *queue;
	char prio;
	struct prionode *nextPrioQ;
}PRIONODE;

typedef struct {
	PRIONODE *head;
	PRIONODE *tail;
}PRIOQUEUE;

PRIOQUEUE *prioQueue_init(void);
int prioEnqueue(PRIOQUEUE *usrPQ, NODE *usrNode, char prio);
void printPrioQueue(PRIOQUEUE *usrPQ);
PRIONODE *prioDequeue(PRIOQUEUE *usrPQ);
