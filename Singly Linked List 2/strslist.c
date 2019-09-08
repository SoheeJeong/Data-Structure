#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp

// User structure type definition
typedef struct 
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*link;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);
/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void){
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
			free(delNode->dataPtr->token);
			free(delNode->dataPtr);
			free(delNode);
			pList->count-=1;
		}
		free(pList);
	}
	return pList;
}

/* Inserts data into list
	return	-1 if overflow
			0 if successful
			1 if duplicated key
*/
int addNode( LIST *pList, tTOKEN *dataInPtr){
	int found;
	int success;
	NODE* pPre;
	NODE* pLoc;
	found=_search(pList,&pPre,&pLoc,dataInPtr->token);
	if(found){ //중복되면 1 반환 
		//printf("%s\n",pLoc->dataPtr->token);
		pLoc->dataPtr->freq+=1;
		return 1;
	}
	success=_insert(pList,pPre,dataInPtr);
	if(!success)
		return -1;
	return 0;
	
}
/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut){
	NODE* pPre;
	NODE* pLoc;
	int found;
	found=_search(pList,&pPre,&pLoc,keyPtr);
	if(found)
		_delete(pList,pPre,pLoc,dataOut);
	return found;	
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut){
	int found;
	NODE* pPre;
	NODE* pLoc;
	found=_search(pList,&pPre,&pLoc,pArgu);
	if(found)
		*pDataOut=pLoc->dataPtr;
	else
		pDataOut=NULL;
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
//자리 있으면 0, 자리 다 찼으면 1
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
		printf("%s\t",(pList->pos->dataPtr->token));
		printf("%d\n",pList->pos->dataPtr->freq);
		pList->pos=pList->pos->link;
	}
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr){
	NODE* pNew;
	if(!(pNew=(NODE*)malloc(sizeof(NODE))))
		return 0;
	pNew->dataPtr=dataInPtr;
	pNew->link=NULL;
	
	if(pPre==NULL){ //맨앞에넣기
		pNew->link=pList->head;
		pList->head=pNew;
		if(pList->count==0)
			pList->rear=pNew;
	}
	else{ //중간에 넣기
		pNew->link=pPre->link;
		pPre->link=pNew;
		if(pNew->link==NULL)
			pList->rear=pNew;
	}
	pList->count+=1;
	return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr){
	*dataOutPtr=pLoc->dataPtr;
	if(pPre==NULL)
		pList->head=pLoc->link;
	else
		pPre->link=pLoc->link;
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

static int compare(char *a, char *b){
	int end;
	if(strcmp(a,b)==0)
		return 0;
	for(int i=0;;i++){
		if(a[i]==b[i])
			continue;
		else if(a[i]>b[i])
			return 1;
		else if(a[i]<b[i])
			return -1;
	}
	return 0;	
}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu){
	*pPre=NULL;
	*pLoc=pList->head;
	if(pList->count==0) //빈 LIST일 경우
		return 0;
	if(compare(pArgu,pList->rear->dataPtr->token)==1){ //맨 끝보다 값이 크면
		*pPre=pList->rear;
		*pLoc=NULL;
		return 0;
	}
	while(compare(pArgu,(*pLoc)->dataPtr->token)==1){
		*pPre=*pLoc;
		*pLoc=(*pLoc)->link;
	}
	if(compare(pArgu,(*pLoc)->dataPtr->token)==0){
		//printf("%s,%s\n",pArgu,(*pLoc)->dataPtr->token);
		return 1;
	}
	else 
		return 0;
}

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *inputst){
	tTOKEN *tNew;
	tNew=(tTOKEN*)malloc(sizeof(tTOKEN));
	tNew->token=inputst;
	tNew->freq=1;
	return tNew;
}

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
tTOKEN *destroyToken( tTOKEN *pToken){
	pToken->token=NULL;
	free(pToken);	
	return pToken;
}

////////////////////////////////////////////////////////////////////////////////
int main( void)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	char* inputst;
	
	// creates a null list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(scanf( "%s", str) == 1)
	{
		inputst=strdup(str);
		pToken = createToken( inputst);		
		// insert function call
		ret = addNode( list, pToken);
		//printList(list);
		if (ret == 1){ // duplicated 
			destroyToken( pToken);
			free(inputst);
		}
	}
	// print function call
	printList( list);
	
	destroyList( list);
	
	return 0;
}
