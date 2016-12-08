#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "prioQueue.h"
#include "playback.h"

PRIOQUEUE *prioQueue_init(void)
{
	PRIOQUEUE *tmp;

	tmp = (PRIOQUEUE *)malloc(sizeof(PRIOQUEUE));

	if (!tmp) {
		errno = ENOMEM;
		return tmp;
	}
	tmp->head = NULL;
	tmp->tail = NULL;

	return tmp;
}

static PRIONODE *prionode_init(char prio)
{
	PRIONODE *tmp;

	tmp = (PRIONODE *)malloc(sizeof(PRIONODE));
	tmp->queue = queue_init();
	tmp->prio = prio;
	tmp->nextPrioQ = NULL;

	return tmp;
}

static PRIONODE *matchPrio(PRIOQUEUE *usrPQ, char prio)
{
	PRIONODE *tmp;

	if (!usrPQ) {
		errno = -EINVAL;
		return NULL;
	}

	tmp = usrPQ->head;

	while (tmp) {
		if (tmp->prio == prio)
			return tmp;

		tmp = tmp->nextPrioQ;
	}

	return NULL;
}

static PRIONODE *insertPRIONODE(PRIOQUEUE *usrPQ, char prio)
{
	PRIONODE *tmp, *tmpNext, *newNode;
	int cmp, cmpNext;

	newNode = prionode_init(prio);

	if ((usrPQ->head == NULL) && (usrPQ->tail == NULL)) {
		usrPQ->head = newNode;
		usrPQ->tail = newNode;
	} else {
		if (prio < usrPQ->head->prio) {
			newNode->nextPrioQ = usrPQ->head;
			usrPQ->head = newNode;
		} else if (prio > usrPQ->tail->prio) {
			usrPQ->tail->nextPrioQ = newNode;
			usrPQ->tail = newNode;
		} else {
			tmp = usrPQ->head;
			tmpNext = usrPQ->head->nextPrioQ;

			while (tmp->nextPrioQ) {
				if ((prio > tmp->prio) &&
					(prio < tmpNext->prio)) {
					newNode->nextPrioQ = tmp->nextPrioQ;
					tmp->nextPrioQ = newNode;
				}
				tmp = tmpNext;
				tmpNext = tmpNext->nextPrioQ;
			}
		}
	}

	return newNode;
}

int prioEnqueue(PRIOQUEUE *usrPQ, NODE *usrNode, char prio)
{
	PRIONODE *tmp;
	int rc = 0;
	if ((!usrPQ) || (!usrNode))
		return -EINVAL;

	printf("prioEnq %p\n", usrNode);
	tmp = matchPrio(usrPQ, prio);

	if (!tmp)
		tmp = insertPRIONODE(usrPQ, prio);

	enqueue(tmp->queue, usrNode);

	return rc;
}

void printPrioQueue(PRIOQUEUE *usrPQ)
{
	PRIONODE *tmp;
	if (!usrPQ) {
		errno = -EINVAL;
	}

	tmp = usrPQ->head;

	while (tmp) {
		printf("0x%02x\n", tmp->prio);
		tmp = tmp->nextPrioQ;
	}
}

PRIONODE *prioDequeue(PRIOQUEUE *usrPQ)
{
	int rc = 0;
	PRIONODE *tmp = NULL;

	if (!usrPQ) {
		errno = EINVAL;
		return tmp;
	}

	if ((!usrPQ->head) && (!usrPQ->tail)) {
		errno = -EBADMSG;
		return tmp;
	}

	tmp = usrPQ->head;

	if ((usrPQ->head == usrPQ->tail)) {
		usrPQ->head = NULL;
		usrPQ->tail = NULL;
	}
	else {
		usrPQ->head = usrPQ->head->nextPrioQ;
	}

	return tmp;
}

PRIONODE *contPeek(PRIOQUEUE *usrPQ)
{
	static PRIONODE *tmp = NULL;

	if (tmp == NULL) {
		tmp = usrPQ->head;
		return tmp;
	}

	tmp = tmp->nextPrioQ;
	return tmp;
}