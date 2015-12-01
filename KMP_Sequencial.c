#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "in.h"
#include "App.h"

int match = 0;

void KMP_search(char * word, char * text);

void partial_match_table(char * word, int length_word, int * pmt);

int * allocate_pmt(int length_word);

string* readTextFromFile(char* path);

off_t fsize(const char *filename);


int main(int argc, char** argv)
{

	char * word = argv[1];
	char * file = argv[2];
	string * text = readTextFromFile(file);
	
	Stopwatch sw;
    srand(time(NULL));

	FREQUENCY(sw);
    START_STOPWATCH(sw);	
	KMP_search(word, text->content);
	STOP_STOPWATCH(sw);

	printf("Quantidade de Ocorrencias: %d\n", match);
	printf("Tempo total: %lf ms\n",sw.mElapsedTime);

	return 0;
}

void KMP_search(char * word, char * text)
{
	//Indice of text
	int m = 0;
	//Indice of word
	int i = 0;
	// recovering the length of text
	int len_text = strlen(text);
	//recovering the length of word
	int len_word = strlen(word);
	//Allocating pmt
	int * pmt = allocate_pmt(len_word);
	//Creating pmt
	partial_match_table(word, len_word, pmt);


	while (m + i < (len_text))
	{
		if (word[i] == text[m + i])
		{
			if(i == (len_word-1))
			{
				match++;	
			}
			i++;
		}	
		else
		{
			if(pmt[i] > -1)
			{
				m = (m + i) - pmt[i];
				i = pmt[i];
			}
			else
			{
				i = 0;
				m++;
			}
		}
	}
}

int * allocate_pmt(int length_word)
{

	register int i;
	int * pmt_table[length_word];

	for (i = 0; i < length_word; i++)
	{
		pmt_table[i] = (int*) malloc(sizeof(int)); 
	}

	return *pmt_table;
}

void partial_match_table(char * word, int length_word, int * pmt)
{

	int pos = 2;
	int cnd = 0;

	pmt[0] = -1;
	pmt[1] = 0;

	while(pos < length_word)
	{

		if(word[pos - 1] == word[cnd])
		{
			cnd = cnd + 1;
			pmt[pos] = cnd;
			pos++; 
		}

		else if(cnd > 0)
		{
			cnd = pmt[cnd];
		}

		else
		{
			pmt[pos] = 0;
			pos++;
		}
	}
}

string* readTextFromFile(char* path)
{
	FILE* file;
	string* str;
	unsigned int i;

	str = (string*) malloc (sizeof(string));
	str->len = fsize(path);
	str->content = (char*) malloc (sizeof(char) * (str->len + 1));
	
	file = fopen(path, "r");

	if(file == NULL)
	{
		fprintf(stderr, "Don't read file: '%s'", path);
		return NULL;
	}

	for(i = 0; !feof(file) && (str->content[i] = fgetc(file)) != '\0'; i++);

	fclose(file);
	
	return str;
}

off_t fsize(const char *filename)
{
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}
