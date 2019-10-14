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

#include <pthread.h>

#include "debug.h"
#include "memory.h"

#define P_MAX 	3

#define C_ERRO_PTHREAD_CREATE 	2
#define C_ERRO_PTHREAD_JOIN 	3
#define ERROR_FORK 				1

void *printThread(void *arg);

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	// Variables to use
	pid_t pid;
	pthread_t tid1, tid2;
	
	for (int i = 0; i < P_MAX; i++)	{
		pid = fork();

		if (pid == 0) {
			// Create(s) the Thread(s)
			if ( (errno = pthread_create(&tid1, NULL, printThread, NULL)) != 0) {
				ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
			}
			if ( (errno = pthread_create(&tid2, NULL, printThread, NULL)) != 0) {
				ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
			}
			// Waits for the Thread(s) to end
			if ((errno = pthread_join(tid1, NULL)) != 0) {
				ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
			}
			if ((errno = pthread_join(tid2, NULL)) != 0) {
				ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
			}
			// Terminates the process
			exit(0);
		} else if (pid > 0) {
		} else {
			ERROR(ERROR_FORK, "Process creation failed! (P%d)", i+1);
		}
	}

	for (int i = 0; i < P_MAX; ++i) {
		// Waits for the process to end
			wait(0);
	}

	return 0;
}

void *printThread(void *arg) {
	(void)arg;
	printf("PPid: %d\t", getppid());
	printf("Pid: %d\t", getpid());
	printf("Tid = [%lu]\n", (unsigned long) pthread_self());
	return NULL;
}
