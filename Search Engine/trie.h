
#define MAX_DEGREE   27 // 'a' ~ 'z' and EOW
#define EOW         '$' // end of word
#define getIndex(x)      (((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
   int          index; // 0, 1, 2, ...
   struct trieNode   *subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
   return   node pointer
         NULL if overflow
*/
TRIE *trieCreateNode(void)
{
	TRIE *trie=(TRIE*)malloc(sizeof(TRIE));
   
   if(trie==NULL) return NULL;
   //if(trie->subtrees==NULL) return NULL;
   
   for(int i=0;i<MAX_DEGREE;i++)
      trie->subtrees[i]=NULL;
   
   trie->index=-1;
   
   return trie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root)
{
   if(root)
   {   
      for(int i = 0; i < MAX_DEGREE; i++)
      {
         if(root->subtrees[i] != NULL)
            trieDestroy(root->subtrees[i]);
      }
	  free(root);
   }
   else 
	   return;
   
}

/* Inserts new entry into the trie
   return   1 success
         0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index)
{ 
	//int len=strlen(str);
   TRIE *pTrie=root;
   int index=0;
   for(int i=0;i<strlen(str);i++)
   {
	   index=getIndex(str[i]);
      if(pTrie->subtrees[index]==NULL)
         pTrie->subtrees[index]=trieCreateNode();
      
    pTrie=pTrie->subtrees[index];
     
   }
  
 // int len=strlen(str);
  
   if(pTrie->index==-1) //마지막 문자 확인
   {
	    str[strlen(str)]='\0';
      pTrie->index=dic_index;
      return 1;
   }
 
   else return 0;
}

/* Retrieve trie for the requested key
   return   index in dictionary (trie) if key found
         -1 key not found
*/
int trieSearch( TRIE *root, char *str)
{
   TRIE * pLoc = root;
   
   for(int i = 0; i < strlen(str); i++)
   {      
      if(pLoc->subtrees[getIndex(str[i])] == NULL)
      {
         return -1;
      }
      
      pLoc = pLoc->subtrees[getIndex(str[i])];
      
   }
   
      if(pLoc->index != -1)
            return pLoc->index;
   return -1;
}


/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root)
{
   if(root)
   {
      if(root->index != -1)
      {
         printf("%d\n", root->index);
      }

      for(int i = 0; i <MAX_DEGREE; i++)
      {
         if(root->subtrees[i] != NULL)
            trieList(root->subtrees[i]);
      }      
   }
   return;
}

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
   using trieList function

void triePrefixList( TRIE *root, char *str)
{
   TRIE* pLoc = root;   
   
   if(root)
   {   
      for(int i = 0; i < strlen(str); i++)
      {
         if(pLoc->subtrees[getIndex(str[i])] != NULL)
         {
            pLoc = pLoc->subtrees[getIndex(str[i])];
         }
         else
            return;

      }
      if(pLoc->index != 0)
      {
         printf("%d\n", pLoc->index);         
      }

      for(int i = 0; i < MAX_DEGREE; i++)
      {
         if(pLoc->subtrees[i] != NULL)
         {
            trieList(pLoc->subtrees[i]); //pLoc->subtree
         }
      }
   }      
   return;
}*/

/* makes permuterms for given str
   ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
   return   number of permuterms

int make_permuterms( char *str, char *permuterms[])
{   
   char* permStr = strcat(str, "$");
   char temp;
   int lastIndex = strlen(permStr) - 1;
   int count = 0;
   
   for(int j = 0; j < strlen(permStr); j++)
   {
      for(int i = 0; i < strlen(permStr); i++)
      {
         if(i == 0)
         {
            temp = permStr[0];
            permStr[i] = permStr[i+1];
         }
         else if(i == strlen(permStr) - 1)
            permStr[i] = temp;
         else
            permStr[i] =  permStr[i+1];
      }
      permuterms[j] = strdup(permStr);
      count++;
   }
   return count;
}*/

/* recycles memory for permuterms

void clear_permuterms( char *permuterms[], int size)
{
   for(int i = 0; i < size; i++)
      free(permuterms[i]);
   return;
}*/

/* wildcard search
   ex) "ab*", "*ab", "a*b", "*ab*"
   using triePrefixList function

void trieSearchWildcard( TRIE *root, char *str)
{
   char * searchStr = strcat(str, "$");
   int lastIndex = strlen(searchStr) - 1;
   char temp;
   char * prefix;
   
   while(1) //starIndex가 마지막 인덱스일때까지
   {
      for(int i = 0; i < strlen(searchStr); i++)
      {
         if(i == 0)
         {
            temp = searchStr[lastIndex];
            searchStr[lastIndex] = str[i];
         }
         else if(i == strlen(searchStr) - 1)
            searchStr[i - 1] = temp;
         else
            searchStr[i - 1] = searchStr[i];
      }
      
      if(searchStr[lastIndex] == '*')
         break;
   }
   
   prefix = strtok(searchStr, "*");
   triePrefixList(root, prefix);
   return;
}*/

/* makes a trie for given dictionary file
   return   trie head node pointer
         NULL failure
*/ 
TRIE *dic2trie( char *dicfile)
{
   TRIE *trie;
   
   int ret;
   char str[100];
   FILE *fp;
   
   int dic_index = 0;
   
   fp = fopen( dicfile, "rt");
   if (fp == NULL)
   {
      fprintf( stderr, "File open error: %s\n", dicfile);
      return NULL;
   }
   
   trie = trieCreateNode(); // original trie
   
   printf( "Inserting to trie...\t");
   
   while (fscanf( fp, "%s", str) == 1) // words file
   {   
      ret = trieInsert( trie, str, dic_index);
      dic_index++;
   }
   
   //trieList(trie);
   printf( "[done]\n"); // Inserting to trie
   fclose( fp);
  
   return trie;
}


/* makes a permuterm trie for given dictionary file
   return   trie head node pointer
         NULL failure
*/ 
//TRIE *dic2permute_trie( char *dicfile);