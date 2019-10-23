#include <signal.h>
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

#define MAX_LINE_CHARS				256

void trata_sinal(int signal);

int main(int argc, char *argv[]){

    struct gengetopt_args_info args;

    // gengetopt parser: deve ser a primeira linha de código no main
    if(cmdline_parser(argc, argv, &args))
    ERROR(1, "Erro: execução de cmdline_parser\n");

    char * filename = args.filename_arg;
    printf("filename: %s\n",filename);


    // abrir o ficheiro "_FILE_" no modo "_MODE_"
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    ERROR(5, "fopen() - não foi possível abrir o ficheiro");

    // início da main/função
    struct sigaction act;


    // algures... no local mais adequado
    act.sa_handler = trata_sinal; 	// Definir a rotina de resposta a sinais
    sigemptyset(&act.sa_mask);  	// mascara sem sinais -- nao bloqueia os sinais
    act.sa_flags = 0;
    //act.sa_flags |= SA_RESTART; 	// recupera chamadas bloqueantes

    // Captura do sinal ???
    if(sigaction(SIGUSR1, &act, NULL) < 0)
    ERROR(3, "sigaction (sa_handler) - SIGUSR1");

    if(sigaction(SIGINT, &act, NULL) < 0)
    ERROR(3, "sigaction (sa_handler) - SIGINT");

    printf("PID:%d\n",getpid());



    pid_t pid;

    pid = fork();

    if (pid == 0) {
        int result = 0;
        pid_t parent_pid = getppid();
        while(result == 0)
        {			// Processo filho
            sleep(5);
            result = kill(parent_pid,SIGUSR1);
        }
        printf("Child terminated:\n");
        exit(0);			// Terminar processo filho
    } else if (pid > 0) {	// Processo pai
        // usar preferencialmente a zona a seguir ao if
    } else					// < 0 - erro
    ERROR(2, "Erro na execucao do fork()");



    // pai espera pelo filho
    wait(NULL);



    // ler uma linha com uma formatação específica. Exemplo: inteiro <\t> inteiro <\n>
    int i1, i2;
    fscanf(fp, "%d\t%d\n", &i1, &i2);



    // fechar o ficheiro
    if (fclose(fp) != 0)
    ERROR(6, "fclose() - não foi possível fechar o ficheiro");

    printf("Parent terminated:\n");
    cmdline_parser_free(&args);
    return 0;
}


void trata_sinal(int signal)
{
    int aux;
    aux = errno;   // Copia da variável global errno

    // código
    printf("Recebi o sinal (%d)\n", signal);
    if(signal == SIGINT){
        
    }
    errno = aux;   // Restaura valor da variável global errno
}
