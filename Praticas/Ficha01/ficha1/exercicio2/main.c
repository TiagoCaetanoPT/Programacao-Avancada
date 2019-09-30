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

int main(int argc, char *argv[]){
	
	pid_t pid, pid1, pid2;

	pid = fork();
	if(pid>0){//pai
		printf("PAI PID %d PPID %d\n", getpid(), getppid());
		waitpid(pid,NULL,0);
	}else{//filho 1
		printf("FILHO 1 PID %d PPID %d \n", getpid(), getppid());
		exit(0);
	}

	pid1=fork();
	if(pid1==0){
		//filho 2
		pid2=fork();
		if(pid2==0){ //neto
			printf("NETO PID %d PPID %d\n", getpid(), getppid());
			exit(0);
		}else{//filho2
			printf("FILHO 2 PID %d PPID %d\n", getpid(), getppid());
			waitpid(pid2,NULL,0);
			exit(0);
		}
	}else{//pai 
		wait(NULL);
	}
	return 0;
}