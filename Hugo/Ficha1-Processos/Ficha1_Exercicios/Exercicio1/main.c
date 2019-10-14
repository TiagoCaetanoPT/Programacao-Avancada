#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"
#include "memory.h"

#include "args.h" // Necessário para ser possível usar o gengetopt

#define ERROR_FORK 1

struct gengetopt_args_info args;

int main(int argc, char *argv[]){
	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args)) {
		ERROR(1, "Erro: execução de cmdline_parser\n");
	}

	// Criação de variáveis
	int num_Processos;	// Contador para o número de processos
	pid_t pid;				// Retorna um inteiro

	//vai buscar o numero que se passa ao args.ggo
	num_Processos = args.num_arg;

	// Processo Pai cria N Processos Filhos
	for (int i = 0; i < num_Processos; ++i) {
		pid = fork();
		if (pid == 0) {
			printf("Processo filho %d (PID: %d)\n", i+1, getpid());
			exit(0); // Acaba a execução do processo filho (Neste caso para não criar os seus próprios processos filhos)
		}else {
			if (pid > 0) {
				waitpid(pid, NULL, 0);
			}else {
				ERROR(ERROR_FORK, "Erro na execução do fork()");
			}
		}
	}

	for (int i = 0; i < num_Processos; ++i) {
		wait(0); // O processo Pai espera por um processo filho
	}

	// Mostrar processo pai
	printf("Processo Pai (PID: %d)\n", getpid()); // Dúvida: pid do pai com getpid() porque é o único processo a correr?
	
	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);
	return 0;
}