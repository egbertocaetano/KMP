#include<stdio.h>
#include<string.h>
#include<stdlib.h>


void KMP_search(char * word, char * text);

void partial_match_table(char * word, int length_word, int * pmt);

int * allocate_pmt(int length_word);

//void load_text(char * text);


int main()
{

	char * word = "ABCDABD";
	char * text = "ABC ABCDAB ABCDABCDABDE";
	
	KMP_search(word, text);

	free(word);
	free(text);

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
				printf("%d\n", m);	
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
		printf("M = %d\n", m);
		printf("I = %d\n", i);
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
