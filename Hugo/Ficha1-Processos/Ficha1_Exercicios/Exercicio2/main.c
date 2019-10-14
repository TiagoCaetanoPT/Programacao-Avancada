/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"
#include "memory.h"

#define ERROR_FORK 1

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; 
	(void)argv;
	
	// Variables
	pid_t pid1, pid2, pid_neto;

	pid1=fork();
	if (pid1 == 0) {
		printf("Eu sou o filho 1 (PID: %d PPID: %d)\n", getpid(), getppid());
		pid_neto = fork();
		if (pid_neto == 0) {
			printf("Eu sou o neto (PID: %d PPID: %d)\n", getpid(), getppid());
		} else if(pid_neto > 0) {
			wait(0);
		} else {
			ERROR(ERROR_FORK, "Pid_Neto Fork Failed!");
		}
	} else if(pid1 > 0) {
		waitpid(pid1, NULL, 0);
		pid2 = fork();
		if (pid2 == 0) {
			printf("Eu sou o filho 2 (PID: %d PPID: %d)\n", getpid(), getppid());
		} else if(pid2 > 0) {
			waitpid(pid2, NULL, 0);
			printf("Eu sou o pai (PID: %d)\n", getpid());
		} else {
			ERROR(ERROR_FORK, "Pid2 Fork Failed!");
		}
	} else {
		ERROR(ERROR_FORK, "Pid1 Fork Failed!");
	}

	return 0;
}

