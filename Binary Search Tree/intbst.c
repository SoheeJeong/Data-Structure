#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
static void _destroy(NODE *root);
static void _traverse( NODE *root);
static void _infix_print( NODE *root, int level);
NODE *_delete( NODE *root, int dltKey, int *success);
NODE* _insert(TREE* pTree, NODE* root, NODE* newPtr);
NODE *_retrieve( NODE *root, int key);
/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( void)
{
	TREE* pTree;
	pTree=(TREE*)malloc(sizeof(TREE));
	if(pTree)
		pTree->root=NULL;
	return pTree;
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *BST_Destroy( TREE *pTree)
{
	if(pTree){
		_destroy(pTree->root);
	}
	free(pTree);
	return pTree;
}

static void _destroy( NODE *root)
{
	if(root)
	{
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data) //같은 data 들어왔을 때 오른쪽자식으로 취급
{
	NODE* newPtr;
	newPtr=(NODE*)malloc(sizeof(NODE));
	
	if(!newPtr)
		return 0;
	
	newPtr->right=NULL;
	newPtr->left=NULL;
	newPtr->data=data;
	
	if(!pTree->root)
		pTree->root=newPtr;
	else
		_insert(pTree,pTree->root,newPtr);
	return 1;
}

NODE* _insert(TREE* pTree, NODE* root, NODE* newPtr) //같은숫자 오른쪽자식으로
{
	if(!root) //빈 node(leaf에도달)
		return newPtr;
	if((newPtr->data)<(root->data)) //new data < node data
	{
		root->left=_insert(pTree,root->left,newPtr);
		return root;
	}
	if((newPtr->data)>=(root->data)) //new data > node data
	{
		root->right=_insert(pTree,root->right,newPtr);
		return root;
	}
	return root;
}

static NODE *_makeNode( int data)
{
	NODE* newNode;
	newNode=(NODE*)malloc(sizeof(NODE));
	if(newNode)
	{
		newNode->data=0;
		newNode->left=NULL;
		newNode->right=NULL;
	}
	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/

int BST_Delete( TREE *pTree, int dltKey)
{
	int success;
	NODE* newRoot;
	newRoot=_delete(pTree->root,dltKey,&success);
	
	if(success==1)
		pTree->root=newRoot;
	return success;
}


/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
NODE *_delete( NODE *root, int dltKey, int *success)
{
	NODE* dltPtr;
	NODE* newRoot;
	NODE* exchPtr; //두 개의 subtree 존재 시 left subtree의 가장 큰 노드
	int holdPtr;
	
	if(!root)
	{
		*success=0;
		return NULL;		
	}
		
	if(dltKey<root->data)
		root->left=_delete(root->left,dltKey,success);
	
	else if(dltKey>root->data)
		root->right=_delete(root->right,dltKey,success);
	
	else
	{
		dltPtr=root;
		
		if(!(root->left))
		{
			newRoot=root->right;
			free(dltPtr);
			*success=1;
			return newRoot;
		}	
		
		else
		{
			if(!(root->right))
			{
				newRoot=root->left;
				free(dltPtr);
				*success=1;
				return newRoot;
			}
			
			else //두 개의 subtree 존재->left subtree의 가장 큰(오른쪽) node 찾기
			{
				exchPtr=root->left; 
				while(exchPtr->right)
					exchPtr=exchPtr->right;
				holdPtr=root->data;
				root->data=exchPtr->data;
				exchPtr->data=holdPtr;
				root->left=_delete(root->left,exchPtr->data,success);
			}
		}			
	}
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
NODE *BST_Retrieve( TREE *pTree, int key)
{
	if(pTree->root)
		return _retrieve(pTree->root,key);
	else
		return NULL;
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
NODE *_retrieve( NODE *root, int key)
{
	if(root){
		if((root->data)<key)
			return _retrieve(root->right,key);
		else if((root->data)>key)
			return _retrieve(root->left,key);
		else
			return root;		
	}
	else
		return NULL;
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree)
{
	_traverse(pTree->root);
}
static void _traverse( NODE *root)
{
	if(root)
	{
		_traverse(root->left);
		printf("%d ",root->data);
		_traverse(root->right);
	}
}
/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree)
{
	_infix_print(pTree->root,0);
}
/* internal traversal function
*/
static void _infix_print( NODE *root, int level)
{
	if(root)
	{
		level++;
		_infix_print(root->right,level);
		for(int i=0;i<level-1;i++)
			printf("\t");
		printf("%d\n",root->data);
		_infix_print(root->left,level);
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 1; i < 20; i++)
	{
		data = rand() % 100 + 1; // 1 ~ 100 random number
		
		fprintf( stdout, "%d ", data);
		
		// insert funtion call
		BST_Insert( tree, data);
 	}
	fprintf( stdout, "\n");
			
	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	int ret;
	do
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		ret = scanf( "%d", &num);
		if (ret != 1) break;
		//if(!(isdigit(num))) break;
		
		ret = BST_Delete( tree, num);
		if (!ret) fprintf( stdout, "%d not found\n", num);
				
		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
	} while(1);
	
	BST_Destroy( tree);

	return 0;
}

