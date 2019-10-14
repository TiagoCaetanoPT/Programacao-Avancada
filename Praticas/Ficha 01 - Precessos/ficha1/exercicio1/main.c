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

int main(int argc, char *argv[]){
	
	pid_t pid;

	int NUM_PROCESSOS;

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	//vai buscar o numero que se passa ao args.ggo
    NUM_PROCESSOS = args.num_arg;




		
	// processo pai cria N processos
	for(int i = 0; i < NUM_PROCESSOS; i++){
		pid = fork();
		if (pid == 0) {			// Processo filho 
			printf("Filho %d PID : %d \n", i + 1, getpid());
			
			exit(0);			// Termina processo filho (para este não criar novos processos)
		} else if (pid > 0) {	// Processo pai
			// usar preferencialmente a zona a seguir ao for
		} else					// < 0 - erro
			ERROR(2, "Erro na execucao do fork()");
	}
			
	// Apenas processo pai
	printf("Pai PID : %d \n", getpid());
	
	// Espera pelos processos filhos
	for(int i = 0; i < NUM_PROCESSOS; i++){
		wait(NULL);
	}
	
	printf("Pai terminou \n");

	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	return 0;
}

