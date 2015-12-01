#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include "in.h"
#include "App.h"

char * word;
string * text;

int num_thread = 0;
int num_thread_created = 0;
int num_thread_created_d = 0;
int rest_text=0;
int offset;

int len_text;
int len_word;
int * pmt;
int match;


pthread_t *threads;
pthread_t *threads_d;

pthread_mutex_t lock;

int start = 0;
int start_d = 0;

int end = 0;
int end_d = 0;

void KMP_search(void *args);
void KMP_search_d(void *args);

void partial_match_table(char * word, int length_word, int * pmt);

int * allocate_pmt(int length_word);

string* readTextFromFile(char* path);
off_t fsize(const char *filename);



int main(int argc, char** argv)
{

	word = argv[1];
	text = readTextFromFile(argv[2]);
	num_thread = 28;

	Stopwatch sw;
    srand(time(NULL));

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Inicialização do mutex falhou!\n");
        return 1;
    }

	// recovering the length of text
	len_text = strlen(text->content);
	//recovering the length of word
	len_word = strlen(word);
	
	//Allocating pmt
	pmt = allocate_pmt(len_word);
	//Creating pmt
	partial_match_table(word, len_word, pmt);


	offset = (len_text/num_thread);
	rest_text = (len_text%len_word);

	//printf("Offset: %d e o tamanho do texto: %d\n", offset, len_text);

	threads = (pthread_t*) malloc (sizeof(pthread_t) * (num_thread));
	threads_d = (pthread_t*) malloc (sizeof(pthread_t) * (num_thread-1));

	long rc;
	long i;

	FREQUENCY(sw);
    START_STOPWATCH(sw);
	for (i = 0; i < num_thread; i++)
	{
					
		rc = pthread_create(&threads[i], NULL, KMP_search, (void*)i);
		//pthread_join(threads[i], NULL);
		if (rc)
		{
    		printf("ERROR; return code from pthread_create() is %ld\n", rc);
        	exit(-1);
        }
			
		if(i < num_thread -1)
		{
			/*end_d = ((i + 1) * offset) + (offset - 1);
			start_d = ((i + 1) * offset) - (offset - 1);*/

			rc = pthread_create(&threads_d[i], NULL, KMP_search_d, (void*)i);
			//cpthread_join(threads_d[i], NULL);		
			if (rc)
			{
    			printf("ERROR; return code from pthread_create() is %ld\n", rc);
        		exit(-1);
        	}
		}
	}
	for (i = 0; i < num_thread; ++i)
	{
		pthread_join(threads[i], NULL);
	}
	for (i = 0; i < num_thread-1; ++i)
	{
		pthread_join(threads_d[i], NULL);
	}
	STOP_STOPWATCH(sw);

	printf("Quantidade de Ocorrencias: %d\n", match);
	printf("Tempo total: %lf ms\n",sw.mElapsedTime);

	return 0;
}

void KMP_search(void *args)
{
	/*pthread_mutex_lock(&lock);
		//Indice of text
		int m = start;
		int end_k = end;
	pthread_mutex_unlock(&lock);*/

	//thread_info threa_info;	
	/*long tid;
   	tid = (long)args;	

	Stopwatch sw1;
    srand(time(NULL));*/
    
    /*FREQUENCY(sw1);
    START_STOPWATCH(sw1);
 */   	
	//Indice of word
	long i;
	i = (long)args;

	//Indice of text
	int m = start;
	int end_k = end;	


	if((i+1) == num_thread)
	{
		end_k = ((i) * offset) + offset + rest_text;
		m = i * offset;
		//printf("Thread %ld Start : %d e end: %d -- ", tid, m, end_k);
	}
	else	
	{
		end_k = ((i) * offset) + offset;
		m = i * offset;
		//printf("Thread %ld Start : %d e end: %d -- ", tid, m, end_k);
	}

	while ((m + i) < (end_k))
	{
		if (word[i] == text->content[m + i])
		{
			if(i == (len_word-1))
			{	
				
				pthread_mutex_lock(&lock);
					match++;
				pthread_mutex_unlock(&lock);
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
	/*STOP_STOPWATCH(sw1);
	printf("Tempo total da thread %ld: %lf ms\n",tid,sw1.mElapsedTime);*/
}

void KMP_search_d(void *args)
{
	/*pthread_mutex_lock(&lock);
		//Indice of text
		int m = start_d;
		int end_k = end_d;
	pthread_mutex_unlock(&lock);*/
	//thread_info threa_info;	
	/*long tid;
   	tid = (long)args;

	Stopwatch sw1;
    srand(time(NULL));
    
    FREQUENCY(sw1);
    START_STOPWATCH(sw1);
*/
	//Indice of word
	long i;
	i = (long)args;

	//Indice of text
	int m = start;
	int end_k = end;
	
	end_k = ((i + 1) * offset) + offset + (len_word - 1);
	m = ((i + 1) * offset) + offset - (len_word - 1);

	//printf("Thread_d %ld Start : %d e end: %d -- ", tid, m, end_k);
	//call_thread();

	while ((m + i) < (end_k))
	{
		if (word[i] == text->content[m + i])
		{
			if(i == (len_word-1))
			{
				//printf("%d\n", m);

				pthread_mutex_lock(&lock);
					match++;
				pthread_mutex_unlock(&lock);	
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
	/*STOP_STOPWATCH(sw1);
	printf("Tempo total da thread_d %ld: %lf ms\n",tid,sw1.mElapsedTime);*/
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
