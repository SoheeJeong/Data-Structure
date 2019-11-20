//#define DEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "trie.h"

// 역색인 헤더 정보에 대한 구조체
typedef struct {
   int      index;   // starting position in posting.idx
   int      df;      // document frequency
} tHEADER;


static char *trim( char *);
////////////////////////////////////////////////////////////////////////////////
// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header( char *filename)
{
   FILE *header;
   int size;
   
   header = fopen(filename, "rb");
   
   if(header == NULL)
      return NULL;
   
   fseek(header, 0, SEEK_END);
   size = ftell(header);
   
   tHEADER * headers =(tHEADER*)malloc(sizeof(tHEADER) * size);
   for(int i = 0; i < size; i++)
   {
      headers[i].index = 0;
      headers[i].df = 0;
   }
   
   fseek(header, 0, SEEK_SET);
   
   fread(headers, sizeof(tHEADER), size, header);
   fclose(header);

   return headers;
}

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting( char *filename)
{
   FILE* post;
   int size;
   
   post = fopen(filename, "rb");
   if(post == NULL)
      return NULL;
   
   fseek(post, 0, SEEK_END);
   size = ftell(post);
   
   int* posting = (int*)malloc(sizeof(int) * size);
   for(int i = 0; i < size; i++)
   {
      posting[i] = 0;
   }
   
   fseek(post, 0, SEEK_SET);
   
   fread(posting, sizeof(int), size, post);
   
   fclose(post);
   
   return posting;
}

// 문서 집합을 화면에 출력한다.
void showDocuments( int *docs, int numdocs)
{
   for(int i = 0; i < numdocs; i++)
   {
      printf(" %d", docs[i]);
   }
   printf("\n\n");
}

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs)
{
   int * interarr;
   interarr = (int*)malloc(sizeof(int) * 10000);
   int Loc1 = 0;
   int Loc2 = 0;
   int Loc3 = 0;
   int count = 0;

   while((Loc1 < numdocs) && (Loc2 < numdocs2)) 
   {
      if(docs[Loc1] == docs2[Loc2])
      {         
         interarr[Loc3] = docs[Loc1];
         Loc1++; Loc2++; Loc3++;
         count++;
      }
      else
      {
         if(docs[Loc1] < docs2[Loc2])
            Loc1++;
         else if(docs[Loc1] > docs2[Loc2])
            Loc2++;
      }
   }
   
   free(docs);
   free(docs2);
   
   *newnumdocs = count;
   
   if(Loc3 == 0)
      return NULL;
      
   return interarr;
}

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs)
{
   int * unarr;
   unarr = (int*)malloc(sizeof(int) * 10000);
   int Loc1 = 0;
   int Loc2 = 0;
   int Loc3 = 0;
   
   while((Loc1 < numdocs) || (Loc2 < numdocs2)) 
   {
      if((Loc1 == numdocs) && (Loc2 != numdocs2) ) 
      {
         while(Loc2 < numdocs2)
         {         
            unarr[Loc3] = docs2[Loc2];
            Loc2++;
            Loc3++;
         }
         break;
      }
      else if((Loc1 != numdocs) && (Loc2 == numdocs2)) 
      {
         while(Loc1 < numdocs)
         {
            unarr[Loc3] = docs[Loc1];
			Loc1++;
            Loc3++;
         }
         break;
      }
      
      if(docs[Loc1] < docs2[Loc2])
      {
         unarr[Loc3] = docs[Loc1];
         Loc1++;
         Loc3++;
      }
      else if(docs[Loc1] > docs2[Loc2])
      {
         unarr[Loc3] = docs2[Loc2];
         Loc2++;
         Loc3++;
      }
      else 
      {
         unarr[Loc3] = docs[Loc1];
        Loc1++;
        Loc2++;
        Loc3++;
      }
   }

   *newnumdocs =Loc3;
   
   free(docs);
   free(docs2);
   
   if(*newnumdocs == 0)
      return NULL;
  
   return unarr;
}

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments( tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs)
{
	
   int dicnum = -1;
   int df = 0;
   int postnum = -1;
   int count = 0;
   dicnum = trieSearch(trie, term);
   
   if(dicnum == -1)
      return NULL;
   
   df = header[dicnum].df;
   postnum = header[dicnum].index;
   
   int* docArr = (int*)malloc(sizeof(int) * 10000);
   for(int i = 0; i < df; i++)
   {
      docArr[i] = posting[postnum + i];
      count++;
   }

   *numdocs = count;
   return docArr;
}


// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments( tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs)
{
	int docnum1 = 0;
   int docnum2 = 0;
   int* docary;
   char delimeter[100];
   int delcount = 0;
   int* docary1;
   int* docary2;
    char* tokens[100] = {'\0', };
  
   
   query = trim(query); 
   
   if((strchr(query, '&') == NULL) && (strchr(query, '|') == NULL))
   {
      docary = getDocuments(header, posting, trie, query, numdocs);
      if(*numdocs == 0)
         return NULL;
      return docary;
   }
   
   else 
   {

      for(int i = 0; i < strlen(query); i++) 
      {
         if(query[i] == '&')
         {
            delimeter[delcount] = '&';
            delcount++;
         }
         else if(query[i] == '|')
         {
            delimeter[delcount] = '|';
            delcount++;
         }      
      }
      
      for(int i = 0; i < strlen(query); i++)
      {
         tokens[i] = strtok(query, "|&");
         tokens[i] = trim(tokens[i]);
         while(tokens[i]!=NULL)
         {
             i++;
             tokens[i]=strtok(NULL,"|&");
             tokens[i] = trim(tokens[i]);
         }
      }
      
      for(int j = 0; j < delcount; j++)
      {
         if(j == 0)
            docary1 = getDocuments(header, posting, trie, tokens[0], &docnum1); 
         docary2 = getDocuments(header, posting, trie, tokens[j+1], &docnum2);      
         if(delimeter[j] == '&')
         {
            docary1= intersectDocuments(docary1, docnum1, docary2, docnum2, numdocs);
            docnum1 = *numdocs;
         }
         else if(delimeter[j] == '|')
         {
            docary1 = unionDocuments(docary1, docnum1, docary2,docnum2, numdocs);
            docnum1 = *numdocs;
         }
      } 
   
      if(*numdocs == 0)
         return NULL;
	 
      return docary1;
   }
}

////////////////////////////////////////////////////////////////////////////////
static char *rtrim( char *str)
{
   char *p = str + strlen( str) - 1;
   
   while (p >= str)
   {
      if (*p == '\n' || *p == ' ' || *p == '\t') *p = 0;
      else return str;
      p--;
   }
}

static char *ltrim( char *str)
{
   char *p = str;
   
   while (*p)
   {
      if (*p == '\n' || *p == ' ' || *p == '\t') p++;
      else 
      {
         return p;
      }
   }
   return NULL;
}

static char *trim( char *str)
{
   if( str == NULL || *str == 0) return str;
   
   return rtrim(ltrim(str));
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
   tHEADER *header;
   int *posting;
   TRIE *trie;
   char query[100];
   int index;
   
   header = load_header( "header.idx");
   if (header == NULL) return 1;
   
   posting = load_posting( "posting.idx");
   if (posting == NULL) return 1;
      
   trie = dic2trie( "dic.txt");
   
   printf( "\nQuery: ");
   while (fgets( query, 100, stdin) != NULL)
   {
      int numdocs;
      int *docs = searchDocuments( header, posting, trie, query, &numdocs);
      
      if (docs == NULL) printf( "not found!\n");
      else 
      {
         showDocuments( docs, numdocs);
         free( docs);
      }
      printf( "\nQuery: ");
   }
   
   free( header);
   free( posting);
   trieDestroy( trie);
   
   return 0;
}