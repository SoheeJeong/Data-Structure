#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time

#define MAX_ELEM 20
#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

// Prototype declarations
AVL_TREE *AVL_Create( void);
AVL_TREE *AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);
int AVL_Insert( AVL_TREE *pTree, int data);
static NODE *_insert( NODE *root, NODE *newPtr);
static NODE *_makeNode( int data);
int *AVL_Retrieve( AVL_TREE *pTree, int key);
static NODE *_retrieve( NODE *root, int key);
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);
void printTree( AVL_TREE *pTree);
static void _infix_print( NODE *root, int level);
static int getHeight( NODE *root);
static NODE *rotateRight( NODE *root);
static NODE *rotateLeft( NODE *root);
///////////////////////////////////////////////////////////////////////////////

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void)
{
	AVL_TREE *tree=(AVL_TREE*)malloc(sizeof(AVL_TREE));
	tree->count=0;
	if(tree)
		tree->root=NULL;
	return tree;
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
AVL_TREE *AVL_Destroy( AVL_TREE *pTree)
{
	if(pTree)
		_destroy(pTree->root);
	
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
int AVL_Insert( AVL_TREE *pTree, int data)
{
	NODE *newPtr=_makeNode(data);
	
	pTree->root=_insert(pTree->root,newPtr);
	(pTree->count)+=1;
		
	return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
//
static NODE *_insert( NODE *root, NODE *newPtr)
{	
	if(!root) //빈 node(leaf에도달)
		return newPtr;
	
	else
	{
		if((newPtr->data)<(root->data))//new data < node data
		{
			root->left=_insert(root->left,newPtr);
			int balance=getHeight(root->left)-getHeight(root->right);
							
				if(balance>1)
				{
					if((newPtr->data)<(root->left->data)) //left of left
						return rotateRight(root);
					else //right of left
					{
						root->left=rotateLeft(root->left);
						return rotateRight(root);	
					}
				}
				//return rotateRight(root); 
		}
		
		else //new data >= node data
		{
			root->right=_insert(root->right,newPtr);
			int balance=getHeight(root->left)-getHeight(root->right);
			
			if(balance<-1)
			{
				if((newPtr->data)>=(root->right->data)) //right of right
					return rotateLeft(root);
				else //left of right
				{
					root->right=rotateRight(root->right);
					return rotateLeft(root);	
				}
			}
		
		}
	}
	
	///height update
	root->height=1+max(getHeight(root->left),getHeight(root->right));
	return root;
}

static NODE *_makeNode( int data)
{
	NODE *newnode=(NODE*)malloc(sizeof(NODE));
	if(newnode)
	{
		newnode->data=data; 
		newnode->left=NULL;
		newnode->right=NULL;
		newnode->height=1;	
	}
	return newnode;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *AVL_Retrieve( AVL_TREE *pTree, int key)
{
	if(pTree->root){
		return &(_retrieve(pTree->root,key)->data);
	}	
	else
		return NULL;
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, int key)
{
	if(root)
	{
		if((root->data)<key)
			_retrieve(root->right,key);
		else if((root->data)>key)
			_retrieve(root->left,key);
		else
			return root;		
	}
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree)
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

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree)
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

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root)
{
	if(root)
		return root->height;
	else
		return 0;
}
/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root)
{
	NODE *temp1=root->left;
	NODE *temp2=temp1->right;
	
	//rotation
	temp1->right=root;
	root->left=temp2;
	
	//height update
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	temp1->height=max(getHeight(temp1->left),getHeight(temp1->right))+1;
	return temp1;
}
/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root)
{
	NODE *temp1=root->right;
	NODE *temp2=temp1->left;
	
	//rotation
	temp1->left=root;
	root->right=temp2;
	
	//height update
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	temp1->height=max(getHeight(temp1->left),getHeight(temp1->right))+1;
	return temp1;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	int data;
	// creates a null tree
	tree = AVL_Create();
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % (MAX_ELEM * 3) + 1; // random number
		//data = i+1; // sequential number
		
		fprintf( stdout, "%d ", data);
		// insert function call
		AVL_Insert( tree, data);
	}
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	AVL_Destroy( tree);
	return 0;
}
