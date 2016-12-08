#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "bst.h"

/******************************************************************************
This function returns a pointer to an initialized BST Data Strcuture as with
dummy root node.

    @param    NONE

    @return   usrBST    A pointer to an initialized BST Data Structure
                        with NULLed head and tail.

              NULL      malloc() failure. errno set to ENOMEM.

******************************************************************************/
BST *bst_init(void)
{
	BST *usrBST;
	usrBST = (BST *)malloc(sizeof(BST));

	if (!usrBST) {
		errno = EINVAL;
		return NULL;
	}
	usrBST->root = NULL;
	usrBST->height = 0;
	return usrBST;
}

/******************************************************************************
This function returns a pointer to an initialized BSTNODE Data Strcuture as with
NULL for BST Data Structure

    @param    NONE

    @return   tmp    A pointer to an initialized BSTNODE Data Structure
                     with NULLed parameters.

             NULL    malloc() failure. errno set to ENOMEM.

******************************************************************************/
BSTNODE *bst_node_init(void)
{
	BSTNODE *tmp;

	tmp = (BSTNODE *)malloc(sizeof(BSTNODE));

	if (!tmp) {
		errno = ENOMEM;
		return NULL;
	}

	tmp->left = NULL;
	tmp->right = NULL;
	tmp->data = NULL;
	tmp->sndrIdCount = 1;

	return tmp;
}

/******************************************************************************
This function returns a pointer to an BSTNODE Data Strcuture that matches the
search key. NULL is returned if nothing is matched.

    @param    usrBST    A pointer to a BST Data Strcuture.

	          key       A pointer to the data key.

    @return   tmp       A pointer to an BSTNODE Data Structure that matches the
                        search key result.

              NULL      No BSTNODE has the desired search key.

******************************************************************************/
BSTNODE *bst_search(BST *usrBST, unsigned char key)
{
	BSTNODE *tmp;
	int cmp;

	if (!usrBST) {
		errno = EINVAL;
		return NULL;
	}

	tmp = usrBST->root;
	
	while (key != tmp->data) {
		if (cmp > 0) {
			tmp = tmp->right;
		}
		else {
			tmp = tmp->left;
		}

		if (tmp == NULL) {
			return NULL;
		}
	}

	return tmp;
}

/******************************************************************************
This function inserts a BSTNODE into a BST Data Structure.

    @param    bstNode   A BSTNODE Data Structure

              key       A pointer to the data key.

    @return   N/A

******************************************************************************/
void bst_insert(BSTNODE **bstRoot, BSTNODE *usrNode)
{
	BSTNODE *tmp;
	int cmp;

	if (!*bstRoot) {
		*bstRoot = usrNode;
		return;
	}

	tmp = *bstRoot;

	while(1){
 		//cmp = stricmp(usrNode->data, tmp->data);
		
		if (usrNode->data < tmp->data) {
			if (tmp->left == NULL) {
				tmp->left = usrNode;
				break;
			}
			else
				tmp = tmp->left;
		} else if (usrNode->data > tmp->data) {
			if (tmp->right == NULL) {
				tmp->right = usrNode;
				break;
			}
			else
				tmp = tmp->right;
		} else {
			tmp->sndrIdCount++;
			break;
		}

	}
}

/******************************************************************************
This function recursively traverse from left subtree to right subtree.
(In-order traversal).

    @param    bstNode   A BSTNODE Data Structure

    @return   0         Recursion termination return.

******************************************************************************/
int bst_inorder_traversal(BSTNODE *bstNode)
{
	static int i = 1;

	if (!bstNode)
		return 0;

	bst_inorder_traversal(bstNode->left);

	printf("Station: 0x%02x Count: %d\n", bstNode->data, bstNode->sndrIdCount);
	i++;
	bst_inorder_traversal(bstNode->right);
}

/******************************************************************************
This function recursively traverse from left subtree to right subtree.
and return the number of nodes counted.

    @param    bstNode   A BSTNODE Data Structure

    @return   0         Recursion hits NULL
	          int       Self + Left Node Count + Right Node Count

******************************************************************************/
int bst_count(BSTNODE *bstNode)
{
	if (!bstNode)
		return 0;

	return 1 + (bst_count(bstNode->left) + bst_count(bstNode->right));
}

/******************************************************************************
This function recursively traverse from left subtree to right subtree.
and return the number of tree's height.

    @param    bstNode   A BSTNODE Data Structure

    @return   -1         Recursion hits NULL
              int        Larger of the left subtree's and right subtree's
			             height.

******************************************************************************/
int bst_height(BSTNODE *usrNode)
{
	int rsum, lsum;

	if (!usrNode)
		return -1;

	rsum = 1 + bst_height(usrNode->left);
	lsum = 1 + bst_height(usrNode->right);

	return (rsum > lsum)? rsum: lsum;
}