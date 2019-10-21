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
#define PING 1
#define PONG 0

/* Estrutura a 'passar' às threads */
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;		/* variável de condição que vai ser 'partilhada' por todas as threads */
    int next;
}thread_params_t;

void *task_ping(void *arg);
void *task_pong(void *arg);

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    pthread_t t_ping, t_pong;
    thread_params_t thread_params;
    thread_params.next = PING;

    /* Inicialização do mutex */
    if ((errno = pthread_mutex_init(&thread_params.mutex, NULL)) != 0)
    ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

    /* Inicialização da variável de condição */
    if ((errno = pthread_cond_init(&thread_params.cond, NULL)) != 0)
    ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

    /* Criação das threads + passagem de parametro */
    if ((errno = pthread_create(&t_ping, NULL, task_ping, &thread_params)) != 0)
    ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");

    if ((errno = pthread_create(&t_pong, NULL, task_pong, &thread_params)) != 0)
    ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");

    /* Espera que todas as threads terminem */
    if ((errno = pthread_join(t_ping, NULL)) != 0) //espera pelas 200 threads
    ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");

    if ((errno = pthread_join(t_pong, NULL)) != 0) //espera pelas 200 threads
    ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");

    /* Destroi o mutex */
    if ((errno = pthread_mutex_destroy(&thread_params.mutex)) != 0)
    ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

    /* Destroi a variável de condição */
    if ((errno = pthread_cond_destroy(&thread_params.cond)) != 0)
    ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");

    return 0;
}


void *task_ping(void *arg){
    thread_params_t *params = arg;
    int i;

    for (i = 0; i < LOOP; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }

        while (params->next != PING) {
            if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)){
                WARNING("pthread_cond_wait() failed");
                return NULL;
            }
        }
        sleep(1);
        printf("Ping");
        fflush(stdout);
        params->next = PONG; //secção critica

        /* Notifica threads à espera na variável de condição */
        if ((errno = pthread_cond_signal(&params->cond)) != 0){
            WARNING("pthread_cond_signal() failed");
            return NULL;
        }

        /*MUTEX: Sai da secçao crítica */
        if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_unlock() failed");
            return NULL;
        }
    }

    return NULL;
}

void *task_pong(void *arg){
    thread_params_t *params = arg;
    int i;

    for (i = 0; i < LOOP; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }

        while (params->next != PONG) {
            if ((errno = pthread_cond_wait(&params->cond, &params->mutex)!=0)){
                WARNING("pthread_cond_wait() failed");
                return NULL;
            }
        }
        sleep(1);
        printf(" <=> Pong\n");
        fflush(stdout);
        params->next = PING; //secção critica

        /* Notifica threads à espera na variável de condição */
        if ((errno = pthread_cond_signal(&params->cond)) != 0){
            WARNING("pthread_cond_signal() failed");
            return NULL;
        }

        /*MUTEX: Sai da secçao crítica */
        if ((errno = pthread_mutex_unlock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_unlock() failed");
            return NULL;
        }
    }

    return NULL;
}
