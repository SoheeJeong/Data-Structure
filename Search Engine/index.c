#define DEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 토큰-문서 구조체
typedef struct {
   char   *token;   // 토큰
   int      docid;   // 문서번호(document ID)
} tTokenDoc;

typedef struct {
   int      index;   // starting position in posting.idx
   int      df;      // 문서 빈도(document frequency)
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex( tTokenDoc *tokens, int num_tokens, 
               char *dicfilename, char *headerfilename, char *postingfilename)
{
   FILE *dic, *head, *post;
   
   int index = 0;
   int count = 0;
   
   dic = fopen(dicfilename, "wt");
   
   if (dic == NULL)
   {
      fprintf( stderr, "File open error: %s\n", dicfilename);
      return;
   }
   
   for(int index = 0; index < num_tokens; index++)
   {
	   
      if(index == 0)
      {
         fputs(tokens[0].token, dic);
         fprintf(dic, "\n");
         count++;
      }
	  
      else if(strcmp(tokens[index].token, tokens[index-1].token) != 0)
      {   
         fputs(tokens[index].token, dic);
         if(index == num_tokens - 1)
         {            
            count++;                        
            break;            
         }
         fprintf(dic, "\n");
         count++;
      }
   }
   
   fclose(dic);
   
   //////////////////////////////////////////////////////////
   head = fopen(headerfilename, "wb");
   
   if (head == NULL)
   {
      fprintf( stderr, "File open error: %s\n", headerfilename);
      return;
   }
   
   tHEADER* header = (tHEADER*)malloc(sizeof(tHEADER) * count);
   int idx = 0;
   if(header == NULL)
      return;
   for(int i = 0; i < count; i++)
   {
      header[i].index = 0;
      header[i].df = 0;      
   }
   for(int i = 0; i < num_tokens; i++)
   {
      if(i == 0)
      {
         header[idx].df++;
         continue;         
      }
      
      if(strcmp(tokens[i].token, tokens[i-1].token) == 0) 
      {
         if(tokens[i].docid != tokens[i-1].docid)
         {
            header[idx].df++;
         }
   
      }
      else
      {
         idx++;   
         header[idx].df++;      
      }
   }
   
   
   for(int i = 0; i < count; i++) 
   {
      if(i == 0)
      {
         header[i].index = 0;   
         continue;
      }
	  
      header[i].index = header[i-1].index + header[i-1].df;
   }   
   
   
   fwrite(header, sizeof(tHEADER), count, head);
   fclose(head);
   free(header);
   
  /////////////////////////////////////////////////////////
   post = fopen(postingfilename, "wb");
   int posting[450000] = {0, };
   if (post == NULL)
   {
      fprintf( stderr, "File open error: %s\n", postingfilename);
      return;
   }
   
   idx = 0;
   for(int i = 0; i < num_tokens; i++)
   {
      if(i == 0)
      {
         posting[idx] = tokens[i].docid;
         idx++;
         continue;
      }
      
      if(strcmp(tokens[i].token, tokens[i-1].token) == 0) //같은 문자열
      {
         if(tokens[i].docid == tokens[i-1].docid) //같은 docid면 skip
            continue;
         else
         {
            posting[idx] = tokens[i].docid;      
            idx++;
         }
      }
      else
      {
         posting[idx] = tokens[i].docid;
         idx++;         
      }
   }

   fwrite(posting, sizeof(int), idx, post);
   fclose(post);
   
}               

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens)
{
   FILE *fp;
   char* ptr;
   int docid = 1;
   tTokenDoc* tokenDoc;
   tokenDoc = (tTokenDoc*)malloc(sizeof(tTokenDoc) * 650000);
   if(tokenDoc == NULL)
      return NULL;
   tokenDoc->token = NULL;
   tokenDoc->docid = 0;
   
   fp = fopen( filename, "rt");
   if (fp == NULL)
   {
      fprintf( stderr, "File open error: %s\n", filename);
      return NULL;
   }
   
   char str[5000] = {'\0', };
   int index = 0;
   char* line_detector;
   while (fgets(str, 5000, fp) != NULL) 
   { 
      if((line_detector = (char*)strchr(str, '\n')) != NULL)
      {
         *line_detector = '\0';            
      }      
      ptr = strtok(str, " ");
      while(ptr != NULL)
      {
         tokenDoc[index].token = strdup(ptr);
         tokenDoc[index].docid = docid;
         ptr = strtok(NULL, " ");
         (*num_tokens)++;
         index++;
      }
      docid++;
   }
   fclose(fp);
   
   return tokenDoc;

}

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2)
{   
   tTokenDoc* ta = (tTokenDoc*)n1;
   tTokenDoc* tb = (tTokenDoc*)n2;
   int result = 0;
   result = strcmp(ta->token, tb->token);
   if(result > 0)
      return 1;   
   else if(result < 0)
      return -1;      
   else if(result == 0)
   {
      if(ta->docid > tb->docid)
         return 1;
      else if(ta->docid < tb->docid)
         return -1;
      else 
         return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
static void print_tokens( tTokenDoc *tokens, int num_tokens)
{
   int i;
   for (i = 0; i < num_tokens; i++)
   {
      printf( "%s\t%d\n", tokens[i].token, tokens[i].docid);
   }
}

////////////////////////////////////////////////////////////////////////////////
void destroy_tokens( tTokenDoc *tokens, int num_tokens)
{
   int i;
   
   for (i = 0; i < num_tokens; i++)
   {
      free( tokens[i].token);
   }
   free( tokens);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
   tTokenDoc *tokens;
   int num_tokens = 0;


   if (argc != 2)
   {
      printf( "Usage: %s FILE\n", argv[0]);
      return 2;
   }
   
   tokens = get_tokens( argv[1], &num_tokens);
   
   //printf("%d\n", num_tokens); //내가 추가해준거
   
   assert( tokens != NULL && num_tokens > 0);

#if DEBUG   
   //print_tokens( tokens, num_tokens);
#endif

   // 정렬 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
   qsort( tokens, num_tokens, sizeof( tTokenDoc), _compare);
   
   invertedIndex( tokens, num_tokens, "dic.txt", "header.idx", "posting.idx");

#if DEBUG   
   //print_tokens( tokens, num_tokens);
#endif   

   destroy_tokens( tokens, num_tokens);
   
   return 0;
}
