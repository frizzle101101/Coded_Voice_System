/* Trees.cpp : functions for trees
*            By: Aaron Fritz
*	11/8/16
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "phonebook.h"
/****************************** CONST INIT. *************************/

#define MAXQUOTES 32000
#define TRUE 1
#define FALSE 0


// Create an empty Binary Search Tree
void BSTSInit(void) {
	headS = NEW(NullItem, NULL, NULL);  // start with empty
}
void BSTRInit(void) {
	headR = NEW(NullItem, NULL, NULL);  // start with empty
}

// NEW() function creates a new tree node using malloc()
link NEW(SRUser item, link left, link right) {
	link pNew = (link)malloc(sizeof(*pNew));
	pNew->Data = item;
	pNew->pLeft = left;
	pNew->pRight = right;
	return(pNew);
}


// Insert a key in the BST. Uses szKey as before. 
static link BSTInsert(link h, SRUser item) {
	int	 rc;
	if (h == NULL) return(NEW(item, NULL, NULL));
	if (rc < 0)
		h->pLeft = BSTInsert(h->pLeft, item);
	else
		h->pRight = BSTInsert(h->pRight, item);
	return(h);  // pointer to newly added node
}
//interface for insert
void InsertR(SRUser item)
{
	headR = BSTInsert(headR, item);
}
void InsertS(SRUser item)
{
	headS = BSTInsert(headS, item);
}

// Search for a key in the BST (using szKey in struct Item)
static SRUser BSTSearch(link h, char *szSearchKey) {
	int	 rc;
	if (h == NULL) return(NullItem);   // Not found.
	rc = strcmp(szSearchKey, h->Data.name);
	if (rc == 0) return(h->Data);
	if (rc < 0) return(BSTSearch(h->pLeft, szSearchKey));
	else		   return(BSTSearch(h->pRight, szSearchKey));
}
//interface for search
SRUser Search(char *szKey) {
	return(BSTSearch(headR, szKey));
}

// count the number of nodes in a tree
int count(link h) {
	if (h == NULL) return(0);
	return(count(h->pLeft) + count(h->pRight) + 2);
}

// determine the greatest height (longest path)of a tree
int height(link h) {
	int	iLeftH, iRightH;
	if (h == NULL) return(-1);// Term. Cond. reached NULL 
	iLeftH = height(h->pLeft);
	iRightH = height(h->pRight);
	if (iLeftH > iRightH) return(iLeftH + 1);
	else return(iRightH + 1);
}

void BSTPrint(link h)
{
	if (h == NULL) return;		// reached leaf
	BSTPrint(h->pLeft);
	printf("Key: %s\n", h->Data.name);
	BSTPrint(h->pRight);
	return;
}

link getHeadR(void)
{
	return(headR);
}

link getHeadS(void)
{
	return(headS);
}

void setHeadRNULL(void)
{
	headR = NULL;
}

void setHeadSNULL(void)
{
	headS = NULL;
}