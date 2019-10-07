#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "debug.h"

int main (int argc, char *argv[])
{	
	char comando[100];
	int result;
	/* Silencia os warnings */
	(void)argc;
	(void)argv;


	printf("Nome do programa: %s\n", argv[0]);
	printf("PID do processo atual: %d\n",getpid());
	
	sprintf(comando,"ps -eLF |  grep %s | grep -v grep | tr -s ' ' | cut -f6 -d' ' | tail -n1", argv[0]);
	
	printf("Numero de threads do processo atual: ");
	fflush(stdout);
	result = system(comando);

	if(result < 0){
		ERROR(1, "Erro na execução do comando \n");
	}
	
	exit(0);
}
