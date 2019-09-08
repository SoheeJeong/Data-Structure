#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define DELETE			4

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
	struct node	*llink;
	struct node	*rlink;
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
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);
tTOKEN *destroyToken( tTOKEN *pToken);
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
			pList->head=pList->head->rlink;
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
	if(found){
		pLoc->dataPtr->freq+=1;
		return 1;
	}
	success=_insert(pList,pPre,dataInPtr);
	if(!success)
		return -1;
	else
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
	if(pList->count==0)
		return 1;
	else
		return 0;
}
//int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList( LIST *pList){
	pList->pos=pList->head;
	while(pList->pos!=NULL){
		printf("%s\t",(pList->pos->dataPtr->token));
		printf("%d\n",pList->pos->dataPtr->freq);
		pList->pos=pList->pos->rlink;
	}
}

/* prints data from list (backward)
*/
void printListR( LIST *pList){
	pList->pos=pList->rear;
	while(pList->pos!=NULL){
		printf("%s\t",(pList->pos->dataPtr->token));
		printf("%d\n",pList->pos->dataPtr->freq);
		pList->pos=pList->pos->llink;
	}
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
//headnode의 llink NULL로 설정!!
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr){
	NODE* pNew;
	if(!(pNew=(NODE*)malloc(sizeof(NODE))))
		return 0;
	pNew->dataPtr=dataInPtr;
	pNew->llink=NULL;
	pNew->rlink=NULL;
	
	if(pPre==NULL){
		if(pList->count==0){
			pList->head=pNew;
			pList->rear=pNew;
			pList->count+=1;
			return 1;
		}
		pList->head->llink=pNew;
		pNew->rlink=pList->head;
		pList->head=pNew;
	}
	else{
		if(pPre->rlink==NULL){
			pNew->llink=pPre;
			pPre->rlink=pNew;
			pList->rear=pNew;
		}
		else{
			pNew->llink=pPre;
			pPre->rlink->llink=pNew;
			pNew->rlink=pPre->rlink;
			pPre->rlink=pNew;
		}		
	}
	pList->count+=1;
	return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataout->왜 save하는거임? removeNode에서 쓸라고!
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr){
	*dataOutPtr=pLoc->dataPtr;
	if(pPre==NULL&&pList->count!=1){
			pList->head=pLoc->rlink;
			pList->head->llink=NULL;
	}
	else{
		if(pLoc->rlink==NULL){
			pList->rear=pPre;
			pList->rear->rlink=NULL;
		}
		else{
			pPre->rlink=pLoc->rlink;
			pLoc->rlink->llink=pPre;
		}
	}
	pList->count-=1;
	free(pLoc);	
}
/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu){
	*pPre=NULL;
	*pLoc=pList->head;
	if(pList->count==0)
		return 0;
	if(strcmp(pArgu,pList->rear->dataPtr->token)>0){
		*pPre=pList->rear;
		*pLoc=NULL;
		return 0;
	}
	while(strcmp(pArgu,(*pLoc)->dataPtr->token)>0){
		*pPre=*pLoc;
		*pLoc=(*pLoc)->rlink;
	}
	if(strcmp(pArgu,(*pLoc)->dataPtr->token)==0){
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
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'D':
			return DELETE;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;
	char* inputst;
	
	if (argc != 2)
	{
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates a null list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", str) == 1)
	{
		inputst=strdup(str);
		pToken = createToken(inputst);
		
		// insert function call
		ret = addNode( list, pToken);
		
		if (ret == 1){ // duplicated 
			destroyToken( pToken);
			free(inputst);
		}
	}
	
	fclose( fp);
	
	fprintf( stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	
	while (1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				printList( list);
				break;
			
			case BACKWARD_PRINT:
				printListR( list);
				break;
			
			case DELETE:
				fprintf( stdout, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				int ret = removeNode( list, str, &pToken);
				if (ret)
				{
					fprintf( stdout, "%s deleted\n", pToken->token);
					free(pToken->token); //이거중요한거였음...메모리누수
					destroyToken( pToken);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
			}
		
		if (action) fprintf( stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	}
	return 0;
}

