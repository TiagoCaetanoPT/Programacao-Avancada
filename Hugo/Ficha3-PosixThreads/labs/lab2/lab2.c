#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "debug.h"

int main (int argc, char *argv[]) {	
	
	/* Silencia os warnings */
	(void)argc;
	(void)argv;

	char progExecName[100];
	int result = 0;


	printf("Nome do programa: %s\n", argv[0]);

	printf("PID do processo atual: %d\n", getpid());

	printf("Número de threads do processo atual: ");
	fflush(stdout);
	sprintf(progExecName, "ps -eLf | grep %s$ | tr -s ' ' | cut -f6 -d' '", argv[0]);
	result = system(progExecName);
	if (result < 0) {
		ERROR(1, "Error in the function");
	}

	exit(0);
}
