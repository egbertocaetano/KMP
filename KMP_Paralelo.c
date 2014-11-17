#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>


char * word = "ABCDABD";
char * text = "ABC ABCDAB ABCDABCDABDEABC ABCDAB ABCDABD CDAB ABCDABD";

int num_thread = 0;


int len_text;
int len_word;
int * pmt;

pthread_mutex_t lock;


void KMP_search(int start, int end, char * word);

void partial_match_table(char * word, int length_word, int * pmt);

int * allocate_pmt(int length_word);

void call_thread();



int main()
{

	//Initilaze
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Incialização do mutex falhou!\n");
        return 1;
    }

    printf("Digite a quantidade de thread para processar:\n");
	scanf("%d", &num_thread);

	// recovering the length of text
	len_text = strlen(text);
	//recovering the length of word
	len_word = strlen(word);
	//Allocating pmt
	*pmt = allocate_pmt(len_word);
	//Threads para 
	pthread_t threads[num_thread];

			
	return 0;
}


void call_thread()
{
	pthread_mutex_lock(&lock);

		if(num_thread > 0)
		{

			
			num_thread--;
		}

	pthread_mutex_unlock(&lock);
}

void KMP_search(int start, int end, char * word)
{
	//Indice of text
	int m = start;
	//Indice of word
	int i = 0;
		
	//Creating pmt
	partial_match_table(word, len_word, pmt);


	while ((m + i) < (end))
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
