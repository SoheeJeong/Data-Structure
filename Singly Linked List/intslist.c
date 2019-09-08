#include <stdlib.h> // malloc
#include <stdio.h>
#include <ctype.h> // toupper

#define QUIT	1
#define INSERT	2
#define DELETE	3
#define PRINT	4
#define SEARCH	5

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	int			data;
	struct node	*link;
} NODE;

typedef struct
{
	int		count; //node의 개수
	NODE	*pos; //현재 탐색중인 노드
	NODE	*head; //시작 노드
	NODE	*rear; //끝 노드
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, int argu);
static int _insert( LIST *pList, NODE *pPre, int dataIn);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, int *dataOut);
/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void){
	LIST* newList;
	newList=(LIST*)malloc(sizeof(LIST));
		newList->head=NULL;
		newList->rear=NULL;
		newList->pos=NULL;
		newList->count=0;	
	return newList;
}

/* Deletes all data in list and recycles memory
	return	NULL head pointer
*/
LIST *destroyList( LIST *pList){
	NODE* delNode;
	if(pList){
		while(pList->count>0){
			delNode=pList->head;
			pList->head=pList->head->link;
			free(delNode);
			pList->count-=1;
		}
	}
	return pList;
}
	

/* Inserts data into list
	return	-1 if overflow
			0 if successful
			1 if dupe key //중복된 key
*/
int addNode( LIST *pList, int dataIn){
	int found;
	int success;
	NODE* pPre;
	NODE* pLoc;
	//중복된 값 1 반환
	found=_search(pList,&pPre,&pLoc,dataIn);
	if(found)
		return 1;
	success=_insert(pList,pPre,dataIn);
	if(!success)
		return -1; //오버플로우
	return 0; //성공시 0 반환
	
}

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, int Key, int *dataOut){
	NODE* pPre;
	NODE* pLoc;
	int found;
	found=_search(pList,&pPre,&pLoc,Key);
	if(found)
		_delete(pList,pPre,pLoc,dataOut);
	return found;
}

/* interface to search function
	Argu	key being sought(찾고싶은거)
	dataOut	contains found data(찾아진거)
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, int Argu, int *dataOut){
	int found;
	NODE* pPre;
	NODE* pLoc;
	found=_search(pList,&pPre,&pLoc,Argu);
	if(found)
		*dataOut=pLoc->data;
	else
		dataOut=NULL;
	return found;	
}

/* returns number of nodes in list
*/
int listCount( LIST *pList){
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList){
	if(pList->count!=0)
		return 0;
	else 
		return 1;
}
//자리 있으면 0, 없으면 1(자리 다 찼으면 1)
int fullList( LIST *pList){
	NODE* Pos;
	if((Pos=(NODE*)malloc(sizeof(NODE)))){
		free(Pos);
		return 0;
	}
	return 1;
}

/* prints data from list
*/
void printList( LIST *pList){
	pList->pos=pList->head;
	while(pList->pos!=NULL){
		printf("%d->",pList->pos->data);
		pList->pos=pList->pos->link;
	}
	printf("NULL\n");	
}


/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, int dataIn){
	NODE* newNode;
	if(!(newNode=(NODE*)malloc(sizeof(NODE))))
		return 0; //overflow -> return 0
	
	newNode->data=dataIn;
	newNode->link=NULL;

	if(pPre==NULL){ //처음에 삽입
		newNode->link=pList->head;
		pList->head=newNode;
		if(pList->count==0)
			pList->rear=newNode;
	}
	else{ //중간에 삽입
		newNode->link=pPre->link;
		pPre->link=newNode;
		if(newNode->link==NULL)
			pList->rear=newNode;
	}
	pList->count+=1;
	return 1;	
}
/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, int *dataOut){
	*dataOut=pLoc->data;
	if(pPre==NULL) //첫 번째 노드 지우기
		pList->head=pLoc->link;
	else //중간 노드 지우기
		pPre->link=pLoc->link;
	//마지막 노드 지우고 rear 설정
	if(pLoc->link==NULL)
		pList->rear=pPre;
	pList->count-=1;
	free(pLoc);
}



/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, int argu){ //여기서의 pPre위치가 _insert에 넘어감
	*pPre=NULL;
	*pLoc=pList->head;
	int result;
	
	if(pList->count==0)
		return 0;
	if(argu>pList->rear->data){ //맨끝에넣음
		*pPre=pList->rear;
		*pLoc=NULL;
		return 0;
	}
	while(argu>(*pLoc)->data){
		*pPre=*pLoc;
		*pLoc=(*pLoc)->link;
	}
	if(argu==(*pLoc)->data) //중복된 값 발견
		return 1;
	else
		return 0;	
}


/* gets user's input
*/
int get_action()
{	
	char ch;
	
	scanf( "%c", &ch);
	ch = toupper( ch);
	
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return PRINT;
		case 'I':
			return INSERT;
		case 'D':
			return DELETE;
		case 'S':
			return SEARCH;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( void)
{
	int num;
	LIST *list;
	int data;
	
	// creates a null list
	list = createList();
	if (!list){
		printf( "Cannot create list\n");
		free(list);
		return 100;
	}
	fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
	
	while(1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList(list);
				free(list);
				return 0;
			
			case PRINT:
				// print function call
				printList( list);
				break;
				
			case INSERT:
				fprintf( stdout, "Enter a number to insert: ");
				fscanf( stdin, "%d", &num);
				
				// insert function call
				addNode( list, num);
				
				// print function call
				printList( list);
				break;
				
			case DELETE:
				fprintf( stdout, "Enter a number to delete: ");
				fscanf( stdin, "%d", &num);
				
				// delete function call
				removeNode( list, num, &data);
				
				// print function call
				printList( list);
				break;
			
			case SEARCH:
				fprintf( stdout, "Enter a number to retrieve: ");
				fscanf( stdin, "%d", &num);
				
				// search function call
				int found;
				found = searchList( list, num, &data);
				if (found) fprintf( stdout, "Found: %d\n", data);
				else fprintf( stdout, "Not found: %d\n", num);
				
				break;
		}
		if (action) fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
	}
	free(list);
	return 0;
}
