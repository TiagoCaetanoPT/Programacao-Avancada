#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include<time.h>

#include "debug.h"
#include "memory.h"

#define BREAD_SLICED 0
#define ADD_CHEESE 1
#define ADD_HAM 2
#define TOAST_END 3

void *taskSliceBread(void *arg);
void *taskAddCheese(void *arg);
void *taskAddHam(void *arg);
void *taskToast_End(void *arg);


// Estrutura a 'passar' às threads
typedef struct {
	int nextStep;
	int cheeseSlice;
	int ToastWaiting;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}thread_params_t;

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	pthread_t thread_bread, thread_cheese, thread_ham, thread_toastF;
	thread_params_t thread_params;
	thread_params.nextStep = BREAD_SLICED;
	thread_params.cheeseSlice = 0;
	thread_params.ToastWaiting = 0;

	// Mutex: inicializa o mutex antes de criar a(s) thread(s)	
	if ((errno = pthread_mutex_init(&thread_params.mutex, NULL)) != 0) {
		ERROR(12, "pthread_mutex_init() failed");
	}

		// Var.Condição: inicializa variável de condição
	if ((errno = pthread_cond_init(&thread_params.cond, NULL)) != 0) {
		ERROR(14, "pthread_cond_init() failed!");
	}

	// Criação das threads + passagem de parâmetro
	if ((errno = pthread_create(&thread_bread, NULL, taskSliceBread, &thread_params)) != 0) {
		ERROR(10, "Erro no pthread_create()!");
	}

	// Criação das threads + passagem de parâmetro
	if ((errno = pthread_create(&thread_cheese, NULL, taskAddCheese, &thread_params)) != 0) {
		ERROR(10, "Erro no pthread_create()!");
	}

	// Criação das threads + passagem de parâmetro
	if ((errno = pthread_create(&thread_ham, NULL, taskAddHam, &thread_params)) != 0) {
		ERROR(10, "Erro no pthread_create()!");
	}

	// Criação das threads + passagem de parâmetro
	if ((errno = pthread_create(&thread_toastF, NULL, taskToast_End, &thread_params)) != 0) {
		ERROR(10, "Erro no pthread_create()!");
	}

	// Espera que todas as threads terminem
	if ((errno = pthread_join(thread_bread, NULL)) != 0) {
		ERROR(11, "Erro no pthread_join()!\n");
	}

	if ((errno = pthread_join(thread_cheese, NULL)) != 0) {
		ERROR(11, "Erro no pthread_join()!\n");
	}

	if ((errno = pthread_join(thread_ham, NULL)) != 0) {
		ERROR(11, "Erro no pthread_join()!\n");
	}

	if ((errno = pthread_join(thread_toastF, NULL)) != 0) {
		ERROR(11, "Erro no pthread_join()!\n");
	}

	// Var.Condição: destroi a variável de condição 
	if ((errno = pthread_cond_destroy(&thread_params.cond)) != 0) {
		ERROR(15,"pthread_cond_destroy failed!");
	}

	// Mutex: liberta recurso
	if ((errno = pthread_mutex_destroy(&thread_params.mutex)) != 0) {
		ERROR(13, "pthread_mutex_destroy() failed");
	}

	return 0;
}

void *taskSliceBread(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	while(1) {

		// Mutex: entra na secção crítica 
		if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {		
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->nextStep != BREAD_SLICED) {		
			if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)) {   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("Bread sliced!\n");
		fflush(stdout);
		params->nextStep = ADD_CHEESE;

		if ((errno = pthread_cond_broadcast(&params->cond)) != 0) {
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// Mutex: sai da secção crítica 
		if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}

	return NULL;
}

void *taskAddCheese(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	while(1) {

		// Mutex: entra na secção crítica 
		if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {		
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->nextStep != ADD_CHEESE) {		
			if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("Putting cheese slice #1\n");
		fflush(stdout);
		params->nextStep = ADD_HAM;


		if ((errno = pthread_cond_broadcast(&params->cond)) != 0) {
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// Mutex: sai da secção crítica 
		if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}

	return NULL;;
}

void *taskAddHam(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	while(1) {

		// Mutex: entra na secção crítica 
		if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {		
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->nextStep != ADD_HAM) {		
			if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)) {   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("Putting ham slice\n");

		fflush(stdout);

		params->nextStep = TOAST_END;

		if ((errno = pthread_cond_broadcast(&params->cond)) != 0) {
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// Mutex: sai da secção crítica 
		if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}

	return NULL;;
}

void *taskToast_End(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	while(1) {

		// Mutex: entra na secção crítica 
		if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {		
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->nextStep != TOAST_END) {		
			if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)) {   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("Toasting....\n");
		fflush(stdout);
		params->nextStep = BREAD_SLICED;

		if ((errno = pthread_cond_broadcast(&params->cond)) != 0) {
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// Mutex: sai da secção crítica 
		if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

		if (sleep(rand() % 4) == 0) {
			printf("Toast Ready!!\n");
		}

	}

	return NULL;
}
