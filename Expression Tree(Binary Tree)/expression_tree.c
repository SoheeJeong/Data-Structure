#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <string.h>
#include <stdbool.h>

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

//float stack을 생성
float stack[MAX_STACK_SIZE]; //후위식 계산에 사용하는 스택
int top=-1; 

void push(float data){
	if(top<MAX_STACK_SIZE-1)	
		stack[++top]=data;
}
 float pop(){
	if(top>=0)
		return stack[top--];
}

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
bool isOperator(char c); //operator인지 검사하는 함수
static void _destroy(NODE* root);
static NODE* _makeNode(char ch);
static void _traverse(NODE* root);
static void _infix_print(NODE* root, int level);
void pushT(NODE* data);
NODE* popT(void);
/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *createTree( void)
{
	TREE* newTree=(TREE*)malloc(sizeof(TREE));
	if(newTree)
	{
		newTree->root=NULL;
	}
	return newTree;	
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *destroyTree( TREE *pTree)
{
	if(pTree)
		_destroy(pTree->root);
	free(pTree);
	return NULL;	
}

static void _destroy( NODE *root)
{
	if(root)
	{
	if(root->left)
		_destroy(root->left);
	if(root->right)
		_destroy(root->right);
	free(root);
	}
	return;
}
/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode( char ch)
{
	NODE* newNode=(NODE*)malloc(sizeof(NODE));
	if(newNode)
	{
		newNode->data=ch;
		newNode->left=NULL;
		newNode->right=NULL;
	}
	return newNode;
}
/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/

int postfix2tree( char *expr, TREE *pTree)  //postfix를 배열에 tree로 저장
{
	NODE* treestack[MAX_STACK_SIZE]={NULL};
	int treetop=-1;

	int i=0;
	int count=0;
	NODE* treeNode;
		
	for(int i=0;i<strlen(expr);i++)
	{
		treeNode=_makeNode(expr[i]);
		
		if(isdigit(treeNode->data)) //data가 operator가 아니면
		{
			if(treetop==-1)
			{
				if(treetop<MAX_STACK_SIZE-1)
					treestack[++treetop]=treeNode;
				count++;
				continue;
			}
			if(treetop<MAX_STACK_SIZE-1)
				treestack[++treetop]=treeNode;
			count++;
			continue;
		}
		else if(isOperator(treeNode->data)) //operator을 data로 가지는 node를 만나면
		{
			if(treetop==-1)
			{
				free(treeNode);
				return 0;
			}
			NODE* pop=NULL;
			if(treetop>=0)
			{
				pop=treestack[treetop--];
				treestack[treetop+1]=NULL; //이거안해서문제였음.....
			}
				treeNode->right=pop;
				
				count--;
			
			NODE* pop1=NULL;
			if(treetop>=0)
			{
				pop1=treestack[treetop--];
				treestack[treetop+1]=NULL;
			}
			treeNode->left=pop1;
				count--;
			
			if(treetop<MAX_STACK_SIZE-1)
				treestack[++treetop]=treeNode;	
			count++;
			continue;
		}
	}
	
	if(count==1)
	{
		pTree->root=treestack[treetop];
		return 1;
	}
	//_destroy(treestack[treetop]);
	int k=0;
	while(treestack[k] != NULL)
	{
		_destroy(treestack[k]);
		k++;
	}
	return 0;
}

/* Print node in tree using inorder traversal
*/
void traverseTree( TREE *pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse( NODE *root) //infix표현(괄호사용)
{
	if(root)
	{
		if((root->left)!=NULL || (root->right)!=NULL)
			printf("(");
		_traverse(root->left); 
		printf("%c",root->data);
		_traverse(root->right);
		if((root->left)!=NULL || (root->right)!=NULL)
			printf(")");
	}
	
}

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level) //그 root의 level이뭔지, level에 따라 indenting 다르게
{						//RNL로 tree print				
	if(root)
	{
		level++;
		_infix_print(root->right,level);
		for(int i=0;i<level-1;i++)
			printf("\t");
		printf("%c\n",root->data);
		_infix_print(root->left,level);
	}
	
}
/* evaluate postfix expression
	return	value of expression
*/
float operation(char c,float oper1, float oper2) 
{
	float res;
	
	switch(c)
	{
		case '+' :  
			res=(float)oper1+(float)oper2;
			break;
		case '-' :
			res=(float)oper1-(float)oper2;
			break;
		case '*' :
			res=(float)oper1*(float)oper2;
			break;
		case '/' :
			res=(float)oper1/(float)oper2;
			break;
	}
	return res;
}

bool isOperator(char c) //operator인지 검사하는 함수
{
	if(c=='+'||c=='-'||c=='*'||c=='/')
	{
		return true;
	}
	return false;
}	

float evalPostfix( char *expr) //후위식 float값 계산
{
	int i;
	
	for(i=0;i<strlen(expr);i++)
	{
		float operand1, operand2,res;
		
		if(top==-1)
		{
			float data=(float)(expr[i]-'0'); //이걸 여기 안에다가 넣었어야지.......
			push(data);
			continue;
		}
		else if(!(isOperator(expr[i])))
		{
			float data=(float)(expr[i]-'0');
			push(data);
			continue;
		}
		
		else
		{
			float operand2=pop();
			float operand1=pop();
			res=operation(expr[i],operand1,operand2);
			push(res);
			continue;
		}
	} //for	
	return stack[top];
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse (pTree->root);
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
	
		int ret = postfix2tree(expr, tree);
		
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			free(tree);
			continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);
		
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		
		// destroy tree
		destroyTree( tree);
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
