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

int main(void)
{
    pid_t pid, neto;

    printf("(PID: %d) -> Eu sou o pai\n", getpid());

    for (int i = 0; i < 2; i++) { //cria 2 processos
        pid = fork();
        if (i == 0 && pid == 0) { // criação do neto
            neto = fork();
            if (neto == 0) {
                printf("(PID: %d) -> Eu sou o neto %d -> (PPID: %d)\n", getpid(), i+1, getppid());
                exit(0);
            } else if (neto < 0) {
                ERROR(1, "Erro na criação do fork() neto.\n");
            }
        }
        if (pid == 0) { // 2 processos filhos
            printf("(PID: %d) -> Eu sou o filho %d -> (PPID: %d)\n", getpid(), i+1, getppid());
            exit(0);
        } else if (pid < 0) {
            ERROR(1, "Erro na criação do fork().\n");
        }
        wait(NULL);
    }
    return 0;
}







// int main(int argc, char *argv[]){

// 	pid_t pid, pid1, pid2;

// 	pid = fork();
// 	if(pid>0){//pai
// 		printf("PAI PID %d PPID %d\n", getpid(), getppid());
// 		waitpid(pid,NULL,0);
// 	}else{//filho 1
// 		printf("FILHO 1 PID %d PPID %d \n", getpid(), getppid());
// 		exit(0);
// 	}

// 	pid1=fork();
// 	if(pid1==0){
// 		//filho 2
// 		pid2=fork();
// 		if(pid2==0){ //neto
// 			printf("NETO PID %d PPID %d\n", getpid(), getppid());
// 			exit(0);
// 		}else{//filho2
// 			printf("FILHO 2 PID %d PPID %d\n", getpid(), getppid());
// 			waitpid(pid2,NULL,0);
// 			exit(0);
// 		}
// 	}else{//pai
// 		wait(NULL);
// 	}
// 	return 0;
// }
