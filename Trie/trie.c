#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	char 			*entry;
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
TRIE *trieCreateNode(void);
void trieDestroy( TRIE *root);
int trieInsert( TRIE *root, char *str);
int trieSearch( TRIE *root, char *str);
void trieList( TRIE *root);
void triePrefixList( TRIE *root, char *str);
int make_permuterms( char *str, char *permuterms[]);
void clear_permuterms( char *permuterms[], int size);
void trieSearchWildcard( TRIE *root, char *str);
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void)
{
	TRIE *root=(TRIE*)malloc(sizeof(TRIE));
		
	if(root)
	{
		root->entry=NULL;
		for(int i=0;i<MAX_DEGREE;i++)
		{
			root->subtrees[i]=NULL;//TRIE* 의 배열
		}
			
	}
	return root;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root)
{
	if(root)
	{	
		if(root->entry)
			free(root->entry);
		
		for(int i=0;i<MAX_DEGREE;i++)
		{
			if(root->subtrees[i])
				trieDestroy(root->subtrees[i]);	
			
			continue;
		}
		free(root);
	}
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
// EOW 만나면..??/?//

int trieInsert( TRIE *root, char *str)
{
	int length=strlen(str);
	
	if(root)
	{
		int index=0;
		
		for(int i=0;i<length;i++)
		{			
			index=getIndex(str[i]);
			
			if(!(0<=index) || !(index<=26)) //알파벡 소문자가 아님
			{
				if((-32<=index)&&(index<=-6)) //근데 알파벳 대문자임
				{
					index+=32;
					str[i]=str[i]+32;
				}
				else
					return 0; //알파벳이 아님
			}
			if(!(root->subtrees[index]))
				root->subtrees[index]=trieCreateNode(); //
			
			root=root->subtrees[index];	
		}
		if(!(root->entry))
		{
			str[length]='\0';
			root->entry=strdup(str); //나중에 strdup free해주기
		}
		return 1;	
	}
	return 0;
}

/* Retrieve trie for the requested key
	return	1 key found
			0 key not found
*/
int trieSearch( TRIE *root, char *str)
{	
	if(root)
	{
		int index=0;
		
		for(int i=0;i<strlen(str);i++)
		{			
			index=getIndex(str[i]);
			
			if(!(0<=index) || !(index<=26)) //알파벡 소문자가 아님
			{
				if((-32<=index)&&(index<=-6)) //근데 알파벳 대문자임
				{
					index+=32;
					str[i]=str[i]+32;
				}
				else
					return 0; //알파벳이 아님
			}
			
			if((root->subtrees[index])==NULL)
				return 0;
			else
				root=root->subtrees[index];	
		}
		
		if(root->entry)
		{
			if(!strcmp((root->entry),str))
				return 1;	
		}
	}
	return 0;
}
/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root)
{
	if(root)
	{
		if((root->entry)!=NULL)
			printf("%s\n",(root->entry));
		
		for(int i=0;i<MAX_DEGREE;i++)
		{
			if(root->subtrees[i])
				trieList(root->subtrees[i]);
			continue;
		}
	}
}

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
void triePrefixList( TRIE *root, char *str)
{
	int index=0;
	
	if(root)
	{
		for(int i=0;i<strlen(str);i++)
		{
			int index=getIndex(str[i]);
			if(!(0<=index) || !(index<=26)) //알파벡 소문자가 아님
			{
				if((-32<=index)&&(index<=-6)) //근데 알파벳 대문자임
					index+=32;
				else
					return; //알파벳이 아님
			}
			if(root->subtrees[index])
				root=root->subtrees[index];
			else
				return;
		}
		trieList(root);		
	}
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[])
{
	int length=strlen(str);
	char* temp=strcat(str,"$");
	
	for(int i=0;i<length+1;i++) //i는 시작하는 index
	{
			char tmp=temp[0];
			for(int j=0;j<length;j++)
				temp[j]=temp[j+1]; //하나씩 땡기기
		
			temp[length]=tmp;
			permuterms[i]=strdup(temp);
	}
	int result=length+1;
	return result;
}

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size)
{
	for(int i=0;i<size;i++)
	{
		if(permuterms[i])
			free(permuterms[i]);
	}
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
//$가 맨뒤로 오게 계속 돌려서 찾기//찾아지면 found
void trieSearchWildcard( TRIE *root, char *str)
{ 
	int length=strlen(str);
	char *newstr=strcat(str,"$");
	
	while(newstr[length]!='*') //맨 뒤가 '*'가 아니면 돌리기 반복
	{
		char tmp=newstr[0];
		 //앞에서 뒤로 한개 보내기
		
		for(int j=0;j<length;j++)
			newstr[j]=newstr[j+1]; //하나씩 땡기기
		
		newstr[length]=tmp;
	}
	
	strtok(newstr,"*");
	triePrefixList(root,newstr);
}

///////////////////////////////////main////////////////////////////////////

int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	int ret=0;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p=0;
	
	for(int i=0;i<100;i++)
	{
		str[i]='\0';
		permuterms[i]=NULL;
	}

	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		ret = trieInsert( trie, str);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
			{
				//int len=strlen(permuterms[i]);
				trieInsert( permute_trie, permuterms[i]);
			}
			clear_permuterms( permuterms, num_p);
		}
	}
	printf( "[done]\n"); // Inserting to trie
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) == 1)
	{
		if (strchr( str, '*')) // wildcard search term
		{
			trieSearchWildcard( permute_trie, str);
		}
		else // search term
		{
			ret = trieSearch( trie, str);
			printf( "[%s]%s found!\n", str, ret ? "": " not");
		}
		printf( "\nQuery: ");
	}

	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}
