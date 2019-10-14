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
#include <pthread.h>

#define NUM_PROCESSOS 3
#define NUM_THREADS 2



//  Protóptipos
void *task(void *arg);


// Estrutura a 'passar' às threads
typedef struct 
{
	int id;
}thread_params_t;

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	pid_t pid;
		
	// processo pai cria N processos
	for(int i = 0; i < NUM_PROCESSOS; i++){
		pid = fork();
		if (pid == 0) {			// Processo filho 


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


    // Espera que todas as threads terminem
	for (int i = 0; i < NUM_THREADS; i++){
		if ((errno = pthread_join(tids[i], NULL)) != 0)
			ERROR(11, "Erro no pthread_join()!\n");
	}
			
			exit(0);			// Termina processo filho (para este não criar novos processos)
		} else if (pid > 0) {	// Processo pai
			// usar preferencialmente a zona a seguir ao for
		} else					// < 0 - erro
			ERROR(2, "Erro na execucao do fork()");
	}
	
	// Apenas processo pai
	
	// Espera pelos processos filhos
	for(int i = 0; i < NUM_PROCESSOS; i++){
		wait(NULL);
	}
	
	return 0;
}


// Zona das funções  ** (não copiar este comentário) ** 
// Thread
void *task(void *arg) 
{
	(void)arg;
	// cast para o tipo de dados enviado pela 'main thread'
	//thread_params_t *params = (thread_params_t *) arg;
	
	// para debug :: (apagar se não for necessário)
	printf("Processo pai [%d] | ProcessoID: [%d] | TID = [%lu]\n", getppid(), getpid(), pthread_self());
	
	return NULL;
}




