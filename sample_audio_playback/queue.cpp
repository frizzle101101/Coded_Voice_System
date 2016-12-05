#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/******************************************************************************
This function returns a pointer to an initialized QUEUE Data Strcuture as a
dummy node.

    @param    NONE

    @return   tmp    A pointer to an initialized QUEUE Data Structure
	                 with NULLed head and tail.

			  NULL   malloc() failure. errno set to ENOMEM.

******************************************************************************/
QUEUE *queue_init(void)
{
	QUEUE *tmp;

	tmp = (QUEUE *)malloc(sizeof(QUEUE));

	if (!tmp) {
		errno = ENOMEM;
		return tmp;
	}
	tmp->head = NULL;
	tmp->tail = NULL;

	return tmp;
}

/******************************************************************************
This function adds a NODE to the end of the Queue.

    @param    userQ     QUEUE Data structure that holds the dummy nodes of the
	                    queue.
		      userNode  A new node with the populated data that user want to
			            insert into the queue.

	@return   0                No error.
			  -EINVAL          Invalid inputs.

******************************************************************************/
int enqueue(QUEUE *userQ, NODE *userNode)
{
	int rc = 0;
	if ((!userQ) || (!userNode))
		return -EINVAL;

	userNode->next = NULL;

	if ((userQ->head == NULL) && (userQ->tail == NULL)) {
		userQ->head = userNode;
		userQ->tail = userNode;
	} else {
		userQ->tail->next = userNode;
		userQ->tail = userNode;
	}

	return rc;
}

/******************************************************************************
This function delete a NODE at the front of the Queue.

    @param    userQ     QUEUE Data structure that holds the dummy nodes of the
                        queue.

   @return   0                No error.
             -EINVAL          Invalid inputs.
			 -EBADMSG         Empty Queue detected.

******************************************************************************/
NODE *dequeue(QUEUE *userQ)
{
	int rc = 0;
	NODE *tmp = NULL;

	if (!userQ) {
		errno = EINVAL;
		return tmp;
	}

	if ((!userQ->head) && (!userQ->tail)) {
		errno = -EBADMSG;
		return tmp;
	}

	tmp = userQ->head;

	if ((userQ->head == userQ->tail)) {
		userQ->head = NULL;
		userQ->tail = NULL;
	} else {
		userQ->head = userQ->head->next;
	}

	return tmp;
}

/******************************************************************************
This function prints all the data in the Queue.

    @param    userQ     QUEUE Data structure that holds the dummy nodes of the
                        queue.

    @return   0                No error.
              -EINVAL          Invalid inputs.
			  -EBADMSG         Empty Queue detected.

******************************************************************************/
int print_Queue(QUEUE *userQ)
{
	int rc = 0;
	int cnt = 1;
	NODE *tmp;

	if (!userQ)
		return -EINVAL;

	if (!userQ->head)
		return -EBADMSG;

	tmp = userQ->head;

	while (tmp) {
		printf("QueuePos:%d\n%s\n", cnt, tmp->data);
		tmp = tmp->next;
		cnt++;
	}
	return rc;
}

/******************************************************************************
This function prints all the data in the Queue.

    @param    userQ     QUEUE Data structure that holds the dummy nodes of the
                        queue.

    @return   0                No error.
              -EINVAL          Invalid inputs.

******************************************************************************/
int destroy_queue(QUEUE *userQ)
{
	int rc = 0;

	if (!userQ)
		return -EINVAL;

	while (dequeue(userQ) != NULL) {}

	return rc;
}

/******************************************************************************
This function prints all the data in the Queue recursively from queue's head
to queue's tail.
@param   inNode     A pointer that points to the queue's head.
@return   NONE
******************************************************************************/
void printQ_recurFW(NODE *inNode)
{
	static int i = 1;

	printf("FW %d:%s\n", i, inNode->data);
	i++;

	if (inNode->next != NULL)
		printQ_recurFW(inNode->next);
}

/******************************************************************************
This function prints all the data in the Queue recursively from queue's tail
to queue's head.
@param   inNode     A pointer that points to the queue's head.
@return   NONE
******************************************************************************/
void printQ_recurBW(NODE *inNode)
{
	static int i = 1;
	if (inNode->next != NULL)
		printQ_recurBW(inNode->next);

	printf("BW %d: %s\n", i, inNode->data);
	i++;
}