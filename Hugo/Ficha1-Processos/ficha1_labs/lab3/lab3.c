#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"

int main (int argc, char *argv[])
{
	int i;
	pid_t pid;
	
	/* Silencia os warnings */
	(void)argc;
	(void)argv;

	// printf("BFF: PID=%d (%d)\n", getpid(), getppid());
	printf("\n");
	for (i = 0; i < 3; i++) {
		pid = fork();
		if (pid == -1)
			ERROR (1, "Erro na execucao do fork");
		else
			printf ("(%d)Processo %d (%d)\n", i, getpid (), getppid ());
	}

	return 0;
}
