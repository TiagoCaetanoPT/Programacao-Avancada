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

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6

#define LOOP 1

/* Estrutura a 'passar' às threads */
typedef struct{
    pthread_mutex_t mutex;
    pthread_cond_t cond;		/* variável de condição que vai ser 'partilhada' por todas as threads */
    int next;
}thread_params_t;

//------PROTOTIPOS DAS FUNÇÕES
void *task_pessoa1(void *arg);
void *task_pessoa2(void *arg);

struct gengetopt_args_info args;

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    pthread_t t_pessoa1, t_pessoa2;
    thread_params_t thread_params;
    thread_params.next = 1;

    if(cmdline_parser(argc, argv, &args)){
        ERROR(1, "Erro no cmdline_parser() \n");
    }

    if (args.duracaoTarefa_arg < 1) { //valida que o valor dado é um positivo
        printf("[ERRO] O valor tem de ser um numero positivo!\n");
        exit(0);
    }

    printf("[INFO] Cada tarefa tem a duração de %ds\n", args.duracaoTarefa_arg);

    /* Inicialização do mutex */
    if ((errno = pthread_mutex_init(&thread_params.mutex, NULL)) != 0)
    ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

    /* Inicialização da variável de condição */
    if ((errno = pthread_cond_init(&thread_params.cond, NULL)) != 0)
    ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

    /* Criação das threads + passagem de parametro */
    if ((errno = pthread_create(&t_pessoa1, NULL, task_pessoa1, &thread_params)) != 0)
    ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");

    if ((errno = pthread_create(&t_pessoa2, NULL, task_pessoa2, &thread_params)) != 0)
    ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");

    //////// CODIGO AQUI ////////

    /* Espera que todas as threads terminem */
    if ((errno = pthread_join(t_pessoa1, NULL)) != 0) //espera pelas 200 threads
    ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");

    if ((errno = pthread_join(t_pessoa2, NULL)) != 0) //espera pelas 200 threads
    ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");

    /* Destroi a variável de condição */
    if ((errno = pthread_cond_destroy(&thread_params.cond)) != 0)
    ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");

    /* Destroi o mutex */
    if ((errno = pthread_mutex_destroy(&thread_params.mutex)) != 0)
    ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

    return 0;
}



//========================================================== FUNCOES
//============================= Tarefa pessoa 1
void *task_pessoa1(void *arg){
    thread_params_t *params = arg;
    int i;

    for (i = 0; i < params->next; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }

        // secção critica
        // sleep(args.duracaoTarefa_arg);
        while (params->next) {
            switch (params->next) {
                case 1:
                sleep(args.duracaoTarefa_arg);
                printf("P1 - Soltei os parafusos\n");
                params->next++;
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
                break;

                case 3:
                sleep(args.duracaoTarefa_arg);
                printf("P1 - Retirei os parafusos\n");
                params->next++;
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
                break;

                case 4:
                sleep(args.duracaoTarefa_arg);
                printf("P1 - Retirei o pneu\n");
                params->next++;
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
                break;

                case 7:
                sleep(args.duracaoTarefa_arg);
                printf("P1 - Baixei o carro \n");
                params->next++;
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
                break;

                default:
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
        }
    }
    return NULL;
}


//============================= Tarefa pessoa 2
void *task_pessoa2(void *arg){/*MUTEX: Entra na secção crítica */
    thread_params_t *params = arg;
    int i;
    for (i = 0; i < params->next; i++) {
        /*MUTEX: Entra na secção crítica */
        if ((errno = pthread_mutex_lock(&params->mutex)) != 0) {
            WARNING("pthread_mutex_lock() failed\n");
            return NULL;
        }
        // secção critica
        while (params->next) {
            switch (params->next) {
                case 2:
                sleep(args.duracaoTarefa_arg);
                printf("P2 - Levantei o carro\n");
                params->next++;
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
                break;

                case 5:
                sleep(args.duracaoTarefa_arg);
                printf("P2 - Coloquei o pneu suplente\n");
                params->next++;
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
                break;

                case 6:
                sleep(args.duracaoTarefa_arg);
                printf("P2 - Apontei os parafusos\n");
                params->next++;
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
                break;

                case 8:
                sleep(args.duracaoTarefa_arg);
                printf("P2 - Apertei os parafusos\n");
                params->next++;
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
                exit(0);
                break;

                default:
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
        }
    }
    return NULL;
}
