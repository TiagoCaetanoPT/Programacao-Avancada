#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "args.h"
#include "debug.h"
#include "memory.h"

// ------------------------CONSTANTES
#define MAX_LINE_CHARS 256

// ------------------------PROTOTIPOS DE FUNCOES
void trata_sinal(int signal);

struct gengetopt_args_info args;

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    struct sigaction act;
    pid_t pid;

    // gengetopt parser: deve ser a primeira linha de código no main
    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    act.sa_handler = trata_sinal; //Definir a rotina de resposta a sinais
    sigemptyset(&act.sa_mask); // mascara sem sinais -- nao bloqueia os sinais
    act.sa_flags = 0; //fidedigno
    act.sa_flags |= SA_RESTART; //recupera chamadas bloqueantes


    pid = fork();
    if (pid == 0) { //processo filho
        pid_t parent_pid = getppid();
        int result = 0;
        printf("[PID:%d] Comando a executar: %s\n", getpid(), args.execute_arg);
        printf("[INFO] kill -s SIGUSR1 %d (para executar)\n", getpid());
        printf("[INFO] kill -s SIGUSR1 %d (para terminar)\n", getpid());
        if (result != 0) {
            result = kill(parent_pid,SIGINT);
        }
        if (result == 0) {
            result = kill(parent_pid,SIGUSR1);
        }
    } else if (pid < 0) {
        ERROR(1, "Erro na criação do fork().\n");
    }else{ //processo pai
        /* Captura do sinal SIGUSR1 */
        if(sigaction(SIGUSR1, &act, NULL) < 0){
            ERROR(1, "sigaction - SIGUSR1");
        }
        /* Captura do sinal SIGINT */
        if(sigaction(SIGINT, &act, NULL) < 0){
            ERROR(1, "sigaction - SIGINT");
        }
        wait(NULL); //espera pelo processo filho
    }
    return 0;
}

///////////////////     FUNCOES      ///////////////////
void trata_sinal(int signal)
{
    if (signal == SIGINT) {
        exit(0);
    }
    if(signal == SIGUSR1) {
        printf("[INFO] SIGUSR1 recebido. A executar: %s\n", args.execute_arg);
        system(args.execute_arg);
    }
}
