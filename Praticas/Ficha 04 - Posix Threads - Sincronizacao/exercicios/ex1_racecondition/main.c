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

#define NUM_THREADS 200
#define LOOP 1000


void *task(void *arg);

/* Estrutura a 'passar' às threads */
typedef struct
{
    int contador;
    pthread_mutex_t mutex;
}thread_params_t;

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;
    int i;

    pthread_t tids[NUM_THREADS]; //vetor das threads
    thread_params_t thread_params; //variavel do tido da estrutura
    thread_params.contador = 0; //inicializa o contador da estrutura

    /* Inicialização do mutex */
    if ((errno = pthread_mutex_init(&thread_params.mutex, NULL)) != 0)
    ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");


    for (i = 0; i < NUM_THREADS; i++) {
        if ((errno = pthread_create(&tids[i], NULL, task, &thread_params)) != 0) //criar as 200 threads
        ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if ((errno = pthread_join(tids[i], NULL)) != 0) //espera pelas 200 threads
        ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
    }

    /* Destroi o mutex */
    if ((errno = pthread_mutex_destroy(&thread_params.mutex)) != 0)
    ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

    printf("Contador %d\n", thread_params.contador);

    return 0;
}



/* Todas as threads executam esta função */
void *task(void *arg)
{
    thread_params_t *args = arg;
    int i;

    for (i = 0; i < LOOP; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(&args->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }

        args->contador++; //executado por uma thread de cada vez

        /*MUTEX: Sai da secçao crítica */
        if ((errno = pthread_mutex_unlock(&args->mutex)) != 0) {
            WARNING("pthread_mutex_unlock() failed");
            return NULL;
        }
    }
    return NULL; //deve-se utilizar sempre return null nas threads
}
