#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <string.h>

typedef struct 
{
	int count; //vertex의 개수
	short** ARRAY; //n*n 배열 (0,1)
} GRAPH ;

//새로운 graph 생성
//overflow면 return NULL
//배열을 0으로 초기화

GRAPH* graphCreate(int verNum)
{
	GRAPH* graph=(GRAPH*)malloc(sizeof(GRAPH));
	
	if(graph)
	{
		graph->count=verNum;
		graph->ARRAY=(short**)malloc(sizeof(short*)*verNum);
		for(int i=0;i<verNum;i++)
		{
			graph->ARRAY[i]=(short*)malloc(sizeof(short)*verNum);
			memset(graph->ARRAY[i],0,sizeof(short)*verNum);
		}
	}
	return graph;
}

//0,1로 채우는 함수
int graphInsert(GRAPH* graph, int a, int b)
{
	if((a>(graph->count))||b>(graph->count))
		return 0;
	
	//printf("in\n");
	(graph->ARRAY)[a-1][b-1]=1;
	(graph->ARRAY)[b-1][a-1]=1;
	//printf("out\n");	
	return 1;
	
}

//graph의 memory free하기
void graphDestroy(GRAPH* graph)
{
	if(graph)
	{
		for(int i=0;i<(graph->count);i++)
			free((graph->ARRAY)[i]);
		
		free(graph->ARRAY);
		
		free(graph);
	}
}

///////
void DFTraverse(GRAPH *graph)
{
	if(!graph)
		return;
	
	int SIZE=graph->count;
	int flag[SIZE];
	int walker=0;
	int stack[SIZE];
	int top=-1;
	int num=0;
	
	while(walker<SIZE) //flag,stack배열 0으로 초기화
	{
		flag[walker]=0;
		stack[walker]=0;
		walker++;
	}
	
	walker=0;
	while(walker<SIZE)
	{
		if(flag[walker]==0)
		{
			//push하기
			if(top<SIZE-1)
				stack[++top]=walker+1;
			flag[walker]=1;
		}
		
		while(top>-1) //stack이 비어있지 않으면
		{
			//pop하고 print하기
			num=stack[top--];
			printf("%d ",num);
			flag[num-1]=2;
			
			for(int i=0;i<SIZE;i++) //인접한거중에 flag 0인거 다 stack에 push하기
			{
				if(((graph->ARRAY)[num-1][i]==1)&&(flag[i]==0))
				{
					//push하기
					if(top<SIZE-1)
						stack[++top]=i+1;
					flag[i]=1;
				}
			}
		}
		walker++;
	}
	printf("\n");
}

////////
void BFTraverse(GRAPH *graph)
{
	if(!graph)
		return;
	
	int SIZE=graph->count;
	int flag[SIZE];
	int walker=0;
	int queue[SIZE];
	int rear=-1;
	int num=0;
	
	while(walker<SIZE) //flag,stack배열 0으로 초기화
	{
		flag[walker]=0;
		queue[walker]=0;
		walker++;
	}
	
	walker=0;
	while(walker<SIZE)
	{
		if(flag[walker]==0)
		{
			//enqueue하기
			if(rear<SIZE-1)
				queue[++rear]=walker+1;
			flag[walker]=1;
		}
		
		while(rear>=0) //queue가 비어있지 않으면
		{
			//dequeue하고 print하기
			num=queue[0];
			for(int i=0;i<rear;i++)
				queue[i]=queue[i+1]; //하나씩 땡기기
			queue[rear--]=0;
			
			printf("%d ",num);
			flag[num-1]=2;
			
			for(int i=0;i<SIZE;i++) //인접한거중에 flag 0인거 다 queue에 enqueue하기
			{
				if(((graph->ARRAY)[num-1][i]==1)&&(flag[i]==0))
				{
					//push하기
					if(rear<SIZE-1)
						queue[++rear]=i+1;
					flag[i]=1;
				}
			}
		}
		walker++;
	}
	printf("\n");	
}
/////////////////////////////////main/////////////////////
int main( int argc, char **argv)
{
	GRAPH *graph;
	char str[1000];
	FILE *pFile;
	int size=0;
	int arr[1000]; //netfile의 값이 저장되어있음
	int a, b;
	
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s FILE\n",argv[0]);
		return 1;
	}
	
	pFile=fopen(argv[1],"rt");
	if(pFile==NULL)
	{
		fprintf(stderr, "File open error: %s\n",argv[1]);
		return 1;
	}
	
	 int count=0;
	 
	 while(fscanf(pFile,"%s",str)==1)
	 {
		 if(atoi(str))
		 {
			 if(count==0)
			 {
				 size=atoi(str);
			 }
			 else
			 {
				 arr[count-1]=atoi(str);
			 }
			 count++;
		 }
	 }
	 
	graph=graphCreate(size); //create
	int edgeNum=(count-1)/2;
	
	for(int i=0;i<edgeNum*2;i+=2) //insert
	{
		if(graphInsert(graph, arr[i],arr[i+1]))
			continue;
		else
			return 1;		
	}
	
	printf("DFS : ");
	DFTraverse(graph); //depth first traversal
	printf("BFS : ");
	BFTraverse(graph); //breadth first traversal
	
	graphDestroy(graph);
	fclose(pFile);
	return 0;
}

