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

#define MAX_LINE_CHARS 256

void trata_sinal(int signal);

struct gengetopt_args_info args;

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    pid_t pid;
    struct sigaction act;

    // gengetopt parser: deve ser a primeira linha de código no main
    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    act.sa_handler = trata_sinal; //Definir a rotina de resposta a sinais
    sigemptyset(&act.sa_mask); // mascara sem sinais -- nao bloqueia os sinais
    act.sa_flags = 0; //fidedigno
    act.sa_flags |= SA_RESTART; //recupera chamadas bloqueantes


    pid = fork();
    if (pid == 0) { //processo filho
        int result = 0;
        pid_t parent_pid = getppid();
        while(result == 0){
            result = kill(parent_pid,SIGUSR1);
            sleep(5);
        }
    } else if (pid < 0) {
        ERROR(1, "Erro na criação do fork().\n");

    }else{ //processo pai
        /* Captura do sinal SIGUSR1 */
        if(sigaction(SIGUSR1, &act, NULL) < 0){
            ERROR(1, "sigaction - SIGUSR1");
        }
        wait(NULL); //espera pelo processo filho
    }
    return 0;
}


////////////////////////////////////////////////////////
///////////////////     FUNCOES      ///////////////////
////////////////////////////////////////////////////////
void trata_sinal(int signal)
{
    if(signal == SIGUSR1) {
        // abrir o ficheiro "_FILE_" no modo "_MODE_"
        FILE *ficheiro = NULL;

        if ((ficheiro = fopen(args.filename_arg, "r")) == NULL)
        ERROR(5, "fopen() - não foi possível abrir o ficheiro");

        char teste[50];
        fgets(teste, 50, ficheiro);
        printf("%s\n", teste);

        // fechar o ficheiro
        if (fclose(ficheiro) != 0)
        ERROR(6, "fclose() - não foi possível fechar o ficheiro");
    }
}
