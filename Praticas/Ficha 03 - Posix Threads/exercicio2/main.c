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

typedef struct{
	int incremento; // valor a passar a cada thread
}thread_params_t;

void *soma(void *arg);

int G_shared_counter; // variavel glocal

int main(int argc, char *argv[]){
	struct gengetopt_args_info args;
	thread_params_t  thread_params;
	int i;
	if(cmdline_parser(argc, argv, &args)){
		ERROR(1, "Erro no cmdline_parser() \n");
	}

	G_shared_counter = 0;

	pthread_t tids[args.nthreads_arg]; // vetor de threads
	thread_params.incremento = args.incremento_arg;

	for (i = 0; i < args.nthreads_arg; i++) { // lança as n threads
		if ((errno = pthread_create(&tids[i], NULL, soma, &thread_params)) != 0){
			ERROR(2, "Erro no pthread_create()!");
		}
	}

	for (i = 0; i < args.nthreads_arg; i++) { // espera pelas n threads
		if ((errno = pthread_join(tids[i], NULL)) != 0) {
			ERROR(2, "Erro no pthread_create()!");
		}
	}

	// quando todas as threads terminarem a sua execucao mostra o valor global
	printf("G_shared_counter = %d (expeting %d)\n", G_shared_counter, args.nthreads_arg*args.incremento_arg);


	cmdline_parser_free(&args);

	return 0;
}


void *soma(void *arg){ //funcao que vai ser executada para cada thread
	thread_params_t*params = (thread_params_t *) arg;
	int num_incrementos = params->incremento, i, local;

	for (i = 0; i < num_incrementos; i++) {
		local  = G_shared_counter; // obtem o valor da var global para a var local
		sched_yield();
		local+=1; // local = local + 1;
		G_shared_counter = local;
	}
	return NULL;
}
