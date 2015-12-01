#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include "App.h"

#define TRUE 1
#define FALSE 0

#define CHECK_ERROR(call) do {                                                    \
   if( cudaSuccess != call) {                                                             \
      std::cerr << std::endl << "CUDA ERRO: " <<                             \
         cudaGetErrorString(call) <<  " in file: " << __FILE__                \
         << " in line: " << __LINE__ << std::endl;                               \
         exit(0);                                                                                 \
   } } while (0)


void partial_match_table(int * pmt, char * word, int len_word)
{
	int pos = 2;
	int cnd = 0;

	pmt[0] = -1;
	pmt[1] = 0;

	while(pos < len_word)
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

//__device__ int get_start(int thread, int offset){
//  return thread*offset; 
//}

__device__ int get_end(int thread, int offset){
    return thread*offset+offset;
}

__device__ void KMP_search(int cursor, int end, int len_word, char* word, char* text,int* match, int* pmt){
   
	 //Verify if the distance is bigger that word's length		
	if((end-cursor) >= len_word)
	{
        
		int i = 0;
	    
		while (cursor < end)
		{
	        while(i > -1 && word[i] != text[cursor])
			{
	            i = pmt[i];
	        }

	        cursor++;
	        i++;
	        
			if(i == len_word)
			{
	            match[cursor-i] = 1;
	            i = 0;
	        }
	    }
    }
}

__global__ void kernel_KMP (char* text, char* word, int* pmt, int* match,int len_text, int len_word, 
                            int num_threads,int debug){

	int indice = blockIdx.x * blockDim.x + threadIdx.x;
	int offset = len_text/num_threads;;
	int cursor,end;
	
	if( indice < num_threads)
	{ 
	    cursor = indice*offset;
	    end = indice*offset+offset;
	}
	else
	{ //Threads Secundárias
	    cursor = get_end(indice%num_threads,offset)-(len_word-1);
	    end = get_end(indice%num_threads,offset)+len_word-1;
	    offset = end-cursor;
	}
	
    KMP_search(cursor,end,len_word,word,text,match,pmt);
}

int main(int argc, char **argv)
{
	
	char * word = argv[1];
    int len_word = strlen(word);
    int * pmt = (int*)malloc(len_word*sizeof(int));	
    	
	FILE * file = fopen(argv[2],"r");
    fseek(file,0,SEEK_END);
    int len_text = ftell(file);
    fseek(file,0,0);
    char * text = (char*)malloc(len_text*sizeof(char));
    
	int count = 0;
    
	while(!feof(file)){
        fscanf(file,"%c",&text[count]);
        count++; 
    }



	Stopwatch sw;
    srand(time(NULL));

    int debug = TRUE;
    
  	partial_match_table(pmt, word, len_word);  

	
    int* match = (int*)malloc(len_text*sizeof(int));
    int* d_pmt = NULL;
    int* d_match = NULL;
    char* d_text = NULL;
    char* d_word = NULL;
    int threads = (len_text/len_word);
    int blocks = (threads/1024)+1;
    int threads_per_block = (threads % blocks == 0) ? threads/blocks : (threads/blocks)+1;
  
	CHECK_ERROR(cudaMalloc((void**) &d_text, len_text * sizeof(char)));
	CHECK_ERROR(cudaMalloc((void**) &d_word, len_word * sizeof(char)));
	CHECK_ERROR(cudaMalloc((void**) &d_match, len_text * sizeof(int)));
	CHECK_ERROR(cudaMalloc((void**) &d_pmt, len_word * sizeof(int)));
	CHECK_ERROR(cudaMemcpy(d_text, text,  len_text * sizeof(char), cudaMemcpyHostToDevice));
	CHECK_ERROR(cudaMemcpy(d_word, word,  len_word * sizeof(char), cudaMemcpyHostToDevice));
    CHECK_ERROR(cudaMemcpy(d_pmt, pmt,  len_word * sizeof(int), cudaMemcpyHostToDevice));
    CHECK_ERROR(cudaMemcpy(d_match, match,  len_text * sizeof(int), cudaMemcpyHostToDevice));  

	FREQUENCY(sw);
    START_STOPWATCH(sw);
	
	kernel_KMP<<<blocks, threads_per_block>>> (d_text,d_word,d_pmt,d_match,len_text, len_word, threads/2, debug);
	
	CHECK_ERROR(cudaDeviceSynchronize());
	
	STOP_STOPWATCH(sw);
	
	CHECK_ERROR(cudaMemcpy(match, d_match, len_text * sizeof(int), cudaMemcpyDeviceToHost));
    
    printf("Tamanho do texto: \n%d\n",len_text);
    puts("Numero de ocorrências: ");
    int counter = 0;
	for (int i = 0; i < len_text; i++){
	    if( match[i]){
			counter++;
		    /*printf("%d ",i); 
		    if(counter % 8 == 0)
		        printf("\n");*/
	    }
	}
	printf("\nNúmero de Ocorrências: %d\n",counter);
	printf("Tempo total: %lf ms\n",sw.mElapsedTime);


	//Desalocando as memórias
	CHECK_ERROR(cudaFree(d_text));
	CHECK_ERROR(cudaFree(d_word));
	CHECK_ERROR(cudaFree(d_pmt));
	CHECK_ERROR(cudaFree(d_match));
	
	return EXIT_SUCCESS;
};
