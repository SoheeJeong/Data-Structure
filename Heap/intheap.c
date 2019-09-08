#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <assert.h>

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	last;
	int	capacity;
} HEAP;

//////////////////////////Prototype declaration//////////////////////////
HEAP *heapCreate( int capacity);
void heapDestroy( HEAP *heap);
int heapInsert( HEAP *heap, int data);
static void _reheapUp( HEAP *heap, int index);
int heapDelete( HEAP *heap, int* data);
static void _reheapDown( HEAP *heap, int index);
void heapPrint( HEAP *heap);
//////////////////////////////////////////////////////////////////////////
HEAP *heapCreate(int capacity)
{
	HEAP* heap=(HEAP*)malloc(sizeof(HEAP));
	if(heap)
	{
		heap->heapArr=(int*)calloc(capacity,sizeof(int));
		heap->last=-1;
		heap->capacity=capacity;
	}
	return heap;
}	

void heapDestroy(HEAP *heap)
{
	if(heap)
	{
		free(heap->heapArr);
		free(heap);
	}
}

int heapInsert( HEAP *heap, int data)
{
	if(!heap)
		return 0;
	if((heap->last)>=MAX_ELEM-1)
		return 0;

	if(heap->last==-1)
	{
		(heap->last)=0;
		(heap->heapArr)[heap->last]=data;
		return 1;
	}

	++(heap->last);
	(heap->heapArr)[heap->last]=data;
	_reheapUp(heap,heap->last);

	return 1;	
}

static void _reheapUp(HEAP*heap, int childLoc)
{
	int parent;
	int* heapAry;
	int hold;
	
	if(childLoc)
	{
		heapAry=(heap->heapArr);
		parent=(childLoc-1)/2;
		if(heapAry[childLoc]>heapAry[parent])
		{
			hold=heapAry[parent];
			heapAry[parent]=heapAry[childLoc];
			heapAry[childLoc]=hold;
			_reheapUp(heap,parent);	
		}
	}
}

int heapDelete( HEAP *heap, int* data)
{
	if(heap->last==-1)
		return 0;
	
	*data=heap->heapArr[0];
	
	heap->heapArr[0]=heap->heapArr[heap->last];
	(heap->last)--;
	_reheapDown(heap,0);
	return 1;	
}

static void _reheapDown( HEAP *heap, int root)
{
	int hold;
	int leftData;
	int rightData;
	int largeLoc;
	int last;
	int check;
	
	last=heap->last;
	if(heap->last<(root*2+1))
		return;
	else if((root*2+1)<=last) //왼쪽자식존재(적어도 하나의 자식존재)
	{
		check=1;
		leftData=heap->heapArr[root*2+1];
		if((root*2+2)<=last) //오른쪽자식도존재
		{
			rightData=heap->heapArr[root*2+2];
			check=2;
		}
	}
	
	//어떤 자식이 더 큰지 결정
	if(check==1) //왼쪽자식만 있으면
		largeLoc=root*2+1;
	else if((check==2)&&(leftData)>(rightData)) //왼쪽자식>오른쪽자식
		largeLoc=root*2+1;
	else //오른쪽자식이 더 크면
		largeLoc=root*2+2;
	
	//root의 값이 위에서 결정한 더 큰 자식보다 큰지 결정
	if((heap->heapArr[root])<(heap->heapArr[largeLoc])) //큰 자식이 root보다 크면
	{
		hold=heap->heapArr[root];
		heap->heapArr[root]=heap->heapArr[largeLoc];
		heap->heapArr[largeLoc]=hold;
		_reheapDown(heap,largeLoc);	
	}	
}

void heapPrint( HEAP *heap)
{
	if(heap)
	{
		for(int i=0;i<=heap->last;i++)
			printf("%d\t",(heap->heapArr)[i]);
		printf("\n");
	}
}
/////////////////////////////////////////////////////////////////////////////
int main(void)
{
	HEAP *heap;
	int data;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (int i = 1; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		heapPrint( heap);
 	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}
