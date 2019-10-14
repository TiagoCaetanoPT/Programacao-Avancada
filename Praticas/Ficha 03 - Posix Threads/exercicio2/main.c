/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include <pthread.h>
#include <sched.h>

//  Protóptipos
void *task(void *arg);


// Estrutura a 'passar' às threads
typedef struct 
{
	int id;
}thread_params_t;

int varglobal = 0;

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	int NUM_THREADS = args.num_threads_arg;
	int qtsoma= args.qtsoma_arg;

	// na função main/outra  ** (não copiar este comentário) ** 
	pthread_t tids[NUM_THREADS];
	thread_params_t thread_params[NUM_THREADS];


	// Inicialização das estruturas - para cada thread
	for (int i = 0; i < NUM_THREADS; i++){
		thread_params[i].id = i + 1;

	}
	
	// Criação das threads + passagem de parâmetro
	for (int i = 0; i < NUM_THREADS; i++){
		if ((errno = pthread_create(&tids[i], NULL, task, &thread_params[i])) != 0)
			ERROR(10, "Erro no pthread_create()!");	
	}

	for (int i = 0; i < qtsoma; ++i)
	{
		varglobal++;
		sched_yield();
	}
	

    // Espera que todas as threads terminem
	for (int i = 0; i < NUM_THREADS; i++){
		if ((errno = pthread_join(tids[i], NULL)) != 0)
			ERROR(11, "Erro no pthread_join()!\n");
	}

	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);


	return 0;
}

void *task(void *arg) 
{
	// cast para o tipo de dados enviado pela 'main thread'
	//thread_params_t *params = (thread_params_t *) arg;
	
	// para debug :: (apagar se não for necessário)
	printf("contador partilhado a: %d \n", varglobal);
	
	return NULL;
}
