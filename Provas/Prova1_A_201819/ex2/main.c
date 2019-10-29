/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @Victor Boarato
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include "debug.h"
#include "memory.h"
#include "args.h"

void *task(void *arg);

typedef struct
{
	int id;
    pthread_mutex_t *ptr_mutex;
    pthread_cond_t *ptr_cond;
    int contentores_camiao;
    int total_km;
}thread_params_t;

int main(int argc, char *argv[]){

    struct gengetopt_args_info args;
    /* Disable warnings */
	(void)argc; (void)argv;
     if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

    int camioes= args.trucks_arg;
    int contentores= args.containers_arg;
    int total=0;
        /*
    printf("%d\n",camioes);
    printf("%d\n",contentores);
    */

    pthread_t tids[camioes];
    thread_params_t thread_params[camioes];

    if(camioes<=0){
        printf("[ERROR] Invalid number of trucks (%d)\n",camioes);
        exit(0);
    }
    if(contentores<=0){
        printf("[ERROR] Invalid number of containers (%d)\n",contentores);
        exit(0);
    }

    for (int i = 0; i < camioes; i++){
		thread_params[i].id = i + 1;
        thread_params[i].contentores_camiao=0;
        thread_params[i].total_km=0;
	}

	for (int i = 0; i < camioes; i++){
		if ((errno = pthread_create(&tids[i], NULL, task, &thread_params[i])) != 0){
			ERROR(10, "Erro no pthread_create()!");
        }else{

            printf("[INFO] TruckID '%d' starting service\n",i+1);
        }

	}

	for (int i = 0; i < camioes; i++){
		if ((errno = pthread_join(tids[i], NULL)) != 0)
			ERROR(11, "Erro no pthread_join()!\n");
	}
	printf("------------------------\n");

    for (int i = 0; i < camioes; i++){
		printf("Truck %d: %d container(s); %d kms\n",i+1,thread_params[i].contentores_camiao,thread_params[i].total_km);
        printf("------------------------\n");
        total= total+(thread_params[i].total_km);
	}

	printf("Total kms: %d\n",total);
    //printf("Media de km: %d", total/contentores);
    printf("Media de km: %d\n", total/(camioes*2));

    //printf("%d\n",thread_params->n_contentores);
    cmdline_parser_free(&args);

	return 0;
}

void *task(void *arg)
{
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
    int km =0;
    int tempo;
    int total = params->total_km;

    for(int i=0; i<2;i++){
        do{

            km = rand()%1000 + 100;

        }while(km%100!=0);


        tempo= km/10;
        total = total+km;
        sleep(tempo);
        params->total_km =total;
        (params->contentores_camiao)++;
        printf("Teste: id = %d | KM :%d \n", params->id,km);
    }
	// para debug :: (apagar se não for necessário)
	//printf("Teste: id = %d | TID = %lu\n", params->id, pthread_self());

	return NULL;
}
