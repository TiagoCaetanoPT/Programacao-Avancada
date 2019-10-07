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
#include <signal.h>

#include "debug.h"
#include "memory.h"
#include "args.h"

#define LINE_MAX 100

void enviaSinal(void);
void trataSinalProcessoFilho(int sinal);
void leLinha(char *filename);

/* parametros de entrada*/
struct gengetopt_args_info args;


int main(int argc, char *argv[]){
	

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args) != 0){
		ERROR(1, "Erro nos parametros\n");
	}

	/*cria novo processo */
	struct sigaction act;
	pid_t pid;

	pid = fork();
	switch(pid = fork()){
		case -1:
		ERROR(1, "Erro no fork()");
		break;
		case 0:
		printf("Processo filho (PID= %d)\n", getpid());
		enviaSinal();
		break;
		default:
		printf("Processo pai (PID= %d)\n", getpid());
		/* Definir a rotina de resposta a sinais */
		act.sa_handler = trataSinalProcessoFilho;

		/* mascara sem sinais -- nao bloqueia os sinais */
		sigemptyset(&act.sa_mask);

		act.sa_flags = 0;           /*fidedigno*/
		act.sa_flags |= SA_RESTART; /*recupera chamadas bloqueantes*/


			/* Captura do sinal SIGUSR1 */
		if(sigaction(SIGUSR1, &act, NULL) < 0){
			ERROR(1, "sigaction - SIGUSR1");
		}
		wait(NULL);
		break;
	}
	

	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);
	return 0;
}

void trataSinalProcessoFilho(int sinal){
	if(sinal == SIGUSR1){
		printf("Recebi o sinal (%d)\n", sinal);
		leLinha(args.file_arg);
	}
}

void leLinha(char *filename){
	char line[LINE_MAX];
	FILE *fptr = NULL;

	fptr = fopen(filename, "r");
	if(fptr == NULL){
		WARNING("Erro ao abrir o ficheiro");
	}else{
		while(fgets(line, LINE_MAX, fptr)){
			printf("Foi lida a linha: %s\n", line);
			break;
		}
		fclose(fptr);
	}
}

void enviaSinal(void)
{
	while(1){
		if(kill(getppid(), SIGUSR1)==0){ //envia sinal ao processo pai
			printf("Processo filho (PID=%d): enviei o sinal (%d) ao processo (PID=%d)\n", getpid(), SIGUSR1, getppid());
			sleep(5);
		}else{//erro no kill
			WARNING("Não foi possivel enviar o sinal ao processo");
		}
	}
}

