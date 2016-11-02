#include "queue.h"
#include "queue_demo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	PAYLOAD x, y;
	PAYLOAD *tmpP;
	QUEUE *usrQ;
	NODE *usrNode1, *usrNode2;
	NODE *tmp;

	printf("Quick Queue Demo with custom message data\n");
	strcpy_s(x.message, "PAYLOAD 1");
	x.prio = 1;
	x.rcvrID = 1;
	x.sndrID = 1;
	x.sequence = 1;

	strcpy_s(y.message, "PAYLOAD 2");
	y.prio = 2;
	y.rcvrID = 2;
	y.sndrID = 2;
	y.sequence = 2;

	usrQ = queue_init();

	usrNode1 = (NODE *)malloc(sizeof(NODE));
	usrNode2 = (NODE *)malloc(sizeof(NODE));

	usrNode1->data = &x;
	usrNode2->data = &y;

	enqueue(usrQ, usrNode1);
	enqueue(usrQ, usrNode2);

	tmp = dequeue(usrQ);
	tmpP = (PAYLOAD *)tmp->data;

	printf("First Dequeue %s\n", tmpP->message);

	tmp = dequeue(usrQ);
	tmpP = (PAYLOAD *)tmp->data;

	printf("Second Dequeue %s\n", tmpP->message);
	return 0;
}