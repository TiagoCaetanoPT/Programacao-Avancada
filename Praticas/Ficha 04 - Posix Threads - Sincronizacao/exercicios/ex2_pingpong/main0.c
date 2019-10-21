#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "debug.h"

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6

#define NUM_THREADS 2
#define LOOP 10


void *task(void *arg);

/* Estrutura a 'passar' às threads */
typedef struct
{
    int id; 				/* identificador da thread */
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;		/* variável de condição que vai ser 'partilhada' por todas as threads */
}thread_params_t;

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;
    int i;

    pthread_t tids[NUM_THREADS]; //vetor das threads
    thread_params_t thread_params[NUM_THREADS];
    pthread_mutex_t mutex;		/* mutex que vai ser 'partilhado' por todas as threads */
    pthread_cond_t cond;		/* variável de condição que vai ser 'partilhada' por todas as threads */


    /* Inicialização do mutex */
    if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
    ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

    /* Inicialização da variável de condição */
    if ((errno = pthread_cond_init(&cond, NULL)) != 0)
    ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

    /* Inicialização das estruturas - para cada thread*/
	for (int i = 0; i < NUM_THREADS; i++){
		thread_params[i].id = i + 1;
		thread_params[i].mutex = &mutex;
		thread_params[i].cond = &cond;
	}

    /* Criação das threads + passagem de parametro */
    for (i = 0; i < NUM_THREADS; i++) {
        if ((errno = pthread_create(&tids[i], NULL, task, &thread_params[i])) != 0) //criar as 2threads
        ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
    }

    /* Espera que todas as threads terminem */
    for (i = 0; i < NUM_THREADS; i++) {
        if ((errno = pthread_join(tids[i], NULL)) != 0) //espera pelas 200 threads
        ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
    }

    /* Destroi o mutex */
    if ((errno = pthread_mutex_destroy(&mutex)) != 0)
    ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

    /* Destroi a variável de condição */
	if ((errno = pthread_cond_destroy(&cond)) != 0)
		ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");


    return 0;
}



/* Todas as threads executam esta função */
void *task(void *arg)
{
    thread_params_t *args = (thread_params_t *) arg;
    int i;

    for (i = 0; i < LOOP; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(args->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }

        // if (args->id == 1){
            printf("Ping");
            sleep(2);
            /* Notifica threads à espera na variável de condição */
    		// if ((errno = pthread_cond_broadcast(args->cond)) != 0){
    			// WARNING("pthread_cond_broadcast() failed");
    			// return NULL;
    		// }
    	// } else {
            printf("\tPong\n");
            sleep(2);
            /* Notifica threads à espera na variável de condição */
    		// if ((errno = pthread_cond_broadcast(args->cond)) != 0){
    			// WARNING("pthread_cond_broadcast() failed");
    			// return NULL;
    		// }
        // }

        /*MUTEX: Sai da secçao crítica */
        if ((errno = pthread_mutex_unlock(args->mutex)) != 0) {
            WARNING("pthread_mutex_unlock() failed");
            return NULL;
        }
    }
    return NULL; //deve-se utilizar sempre return null nas threads
}
