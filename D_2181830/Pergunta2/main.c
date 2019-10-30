#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include <pthread.h>

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6

#define LOOP 1000

// void *codigo_thread(void *parm);
void *colher(void *arg);
void *produzir(void *arg);


/* Estrutura a 'passar' às threads */
typedef struct{
    int id; 				/* identificador da thread */
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    int tamanho
}thread_params_t;


int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    struct gengetopt_args_info args;

    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    pthread_t tids[args.threads_arg]; //vetor das threads
    pthread_t t_colher; //vetor das threads
    thread_params_t thread_params[args.threads_arg]; //variavel do tido da estrutura
    // thread_params.contador = 0; //inicializa o contador da estrutura
    pthread_mutex_t mutex;		/* mutex que vai ser 'partilhado' por todas as threads */
    pthread_cond_t cond;		/* variável de condição que vai ser 'partilhada' por todas as threads */
    int i;

    // Verificacoes dos argumentos
    if(args.threads_arg < 3 || args.threads_arg > 20){
        printf("threads_arg must be within [3,20] (%d given)\n", args.threads_arg);
        return 0;
    }
    if(args.rounds_arg < 3 || args.rounds_arg > 10){
        printf("rounds_arg must be within [3,10] (%d given)\n", args.rounds_arg);
        return 0;
    }

    /* Inicialização do mutex */
    if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
    ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

    /* Inicialização da variável de condição */
    if ((errno = pthread_cond_init(&cond, NULL)) != 0)
    ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

    /* Inicialização das estruturas - para cada thread*/
    for (int i = 0; i < args.threads_arg; i++){
        thread_params[i].id = i + 1;
        thread_params[i].mutex = &mutex;
        thread_params[i].cond = &cond;
        thread_params[i].tamanho = args.size_arg;
    }

    /* Criação das threads + passagem de parametro */
    for (i = 0; i < args.threads_arg; i++) {
        if ((errno = pthread_create(&tids[i], NULL, produzir, &thread_params[i])) != 0) //criar as N threads produtoras
        ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
    }

    if ((errno = pthread_create(&t_colher, NULL, colher, &thread_params[i])) != 0) //criar a threads consumidora
    ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");

    if ((errno = pthread_join(t_colher, NULL)) != 0) //espera a threads consumidora
    ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");

    /* Espera que todas as threads terminem */
    for (i = 0; i < args.threads_arg; i++) {
        if ((errno = pthread_join(tids[i], NULL)) != 0) //espera pelas N threads
        ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
    }

    /* Destroi o mutex */
    if ((errno = pthread_mutex_destroy(&mutex)) != 0)
    ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

    /* Destroi a variável de condição */
    if ((errno = pthread_cond_destroy(&cond)) != 0)
    ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");

    // gengetopt: libertar recurso (assim que possível)
    cmdline_parser_free(&args);


    return 0;
}

void *produzir(void *arg){
    thread_params_t *params = (thread_params_t *) arg;
    unsigned int seedp = pthread_self();
    int i;

    /*MUTEX: Entra da secçao crítica */
    if ((errno = pthread_mutex_lock(params->mutex)) != 0) {
        WARNING("pthread_mutex_lock() failed\n");
        return NULL;
    }

        int num = (rand_r(&seedp) % 10) +1;
        /* a primeira thread sinaliza uma variável de condição, as restantes esperam nessa variável */
        /* Notifica threads à espera na variável de condição */
        if ((errno = pthread_cond_broadcast(params->cond)) != 0){
            WARNING("pthread_cond_broadcast() failed");
            return NULL;
        }

        printf("[Worker:%d] writing '%d' at position %d\n",params->id, num, i);
        usleep(10000);

        if ((errno = pthread_cond_wait(params->cond, params->mutex)) != 0) {
            WARNING("pthread_cond_wait() failed");
            return NULL;
        }

    /*MUTEX: Sai da secçao crítica */
    if ((errno = pthread_mutex_unlock(params->mutex)) != 0) {
        WARNING("pthread_mutex_unlock() failed");
        return NULL;
    }


    return NULL;
}

void *colher(void *arg){
    thread_params_t *params = (thread_params_t *) arg;

    return NULL;
}
