#pragma once

typedef struct bstnode {
	struct bstnode *right;
	struct bstnode *left;
	unsigned char data;
	int sndrIdCount;
}BSTNODE;

typedef struct {
	BSTNODE *root;
	int height;
}BST;

BST *bst_init(void);
BSTNODE *bst_node_init(void);
BSTNODE *bst_search(BST *usrBST, unsigned char key);
void bst_insert(BSTNODE **bstRoot, BSTNODE *usrBSTNODE);
int bst_inorder_traversal(BSTNODE *bstNode);
int bst_count(BSTNODE *bstNode);
int bst_height(BSTNODE *usrNode);

#define max(a, b)  (((a) > (b)) ? (a) : (b))