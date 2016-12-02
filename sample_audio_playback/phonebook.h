#pragma once

/*Trees.h : Header file for trees
* By: Aaron Fritz
*	11/8/16
*/


#ifndef TREES_H
#define TREES_H
/* ^^ The above are include guards to prevent repeats of the prototyoes */
#define STRMAX 140
//Structures for linked list
typedef struct
{
	unsigned short Id;
	char mode;
	int hashkey;
	char name[140];
}SRUser;

typedef struct node *link;
struct node
{
	link pLeft, pRight;
	SRUser Data;
};
typedef struct node BSTS;

typedef struct node *link;
struct node
{
	link pLeft, pRight;
	SRUser Data;
};
typedef struct node BSTR;

//Queue linked list
static SRUser NullItem = {};
static link headS;	// ‘headS’ points to Sender root
static link headR;	// ‘headR’ points to Receiver root

					// Function prototypes
void BSTSInit(void);
void BSTRInit(void);
link NEW(SRUser item, link left, link right);
link BSTInsert(link h, SRUser item);
void Insert(SRUser item);
static link BSTInsertH(link h, SRUser item);
void InsertH(SRUser item);
SRUser BSTSearch(link h, char *szSearchKey);
SRUser Search(char *szKey);
int count(link h);
int height(link h);
void BSTPrint(link h);
link getHead(void);
void setHeadNULL(void);
#endif
