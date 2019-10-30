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
	/* Disable warnings */
	(void)argc; (void)argv;

    struct gengetopt_args_info args;
    pid_t pid;

    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    if(args.num_procs_arg < 4 || args.num_procs_arg > 16){
        printf("ERR: num_procs must be within [4,16]:%d given\n", args.num_procs_arg);
        return 0;
    }

    for(int i = 0; i < args.num_procs_arg; i++){
        pid = fork();
        if (pid == 0) { // processo filho
            printf("[PAI:%d] Filho %d tem PID=%d\n", getppid(), i, getpid());
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
