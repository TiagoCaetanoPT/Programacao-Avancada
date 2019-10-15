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

int main(int argc, char *argv[]){
    pid_t pid;

    struct gengetopt_args_info args;

    // gengetopt parser: deve ser a primeira linha de código no main
    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    printf("Processo Pai (PID: %d)\n", getpid());
    // processo pai cria N processos
    for(int i = 0; i < args.num_procs_arg; i++){
        pid = fork();
        if (pid == 0) { // processo filho
            printf("%d-Filho (PID: %d)\n", i+1, getpid());
            exit(0);
        } else if (pid > 0){
            // Executado após o for()
        } else
        ERROR(1, "Erro na criação do fork();\n");
    }

    // Espera pelos processos filhos
    for(int i = 0; i < args.num_procs_arg; i++){
        wait(NULL);
    }

    // gengetopt: libertar recurso (assim que possível)
    cmdline_parser_free(&args);

    return 0;
}
