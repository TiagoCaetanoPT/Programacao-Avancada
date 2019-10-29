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
#include "args.h"

#define MAX_LINE_CHARS				256
//zona protótipos
void trata_sinal(int signal);

int continua = 1;

int main(int argc, char *argv[]){
	/* Disable warnings */
	struct gengetopt_args_info args;
	struct sigaction act;
	act.sa_handler = trata_sinal;
	(void)argc; (void)argv;

	sigemptyset(&act.sa_mask);

	act.sa_flags = 0;

	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

		if(args.min_arg < 1){
			printf("[ERROR] min signal must be >= 1\n");
			exit(0);
		}
		if(args.max_arg > 64){
			printf("[ERROR] max signal must be <= 64\n");
			exit(0);
		}

		if(args.min_arg > args.max_arg){
			printf("[ERROR] signal must be between 1 and 64\n");
			exit(0);
		}

		printf("[INFO] Processing signal from [MIN:%d, MAX:%d]\n", args.min_arg,args.max_arg);
		for(int i = args.min_arg;i<=args.max_arg;i++){

			if(i==9 || i == 19 || i == 32 || i == 33){
				printf("[WARNING] Can't install handler for signal '%s' (%d)\n",strsignal(i),i );
			}else{
				if(sigaction(i, &act, NULL) < 0){
					ERROR(3, "sigaction (sa_handler) - %d",i);
				}
			}
		}

		printf("[INFO] Terminate: kill -s SIGILL %d | kill -s SIGINT %d\n",getpid(),getpid());
		printf("Waiting for a signal\n");

		while(continua !=0){
				pause();
			}
			cmdline_parser_free(&args);

			return 0;
}

//Zona da função

void trata_sinal(int signal){
	int aux;
	aux = errno; // copia da variavel global errno

	//código
	if (signal == SIGINT){
		continua = 0;
		printf("Got SIGINT (%d) from process %d\n",getppid(), getpid());
	}else{
		printf("[PID:%d] Got signal '%s' form process %d\n",getppid(), strsignal(signal), getpid());
		printf("Waiting...\n");
	}
	errno = aux;   // Restaura valor da variável global errno
	}
