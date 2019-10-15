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
            wait(NULL);
            printf("(PID: %d) -> Eu sou o filho %d -> (PPID: %d)\n", getpid(), i+1, getppid());
            exit(0);
        } else if (pid < 0) {
            ERROR(1, "Erro na criação do fork().\n");
        }
        wait(NULL);
    }

    printf("(PID: %d) -> Eu sou o pai\n", getpid());

    return 0;
}
