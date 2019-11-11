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

#define ERROR_CMDLINE   1
#define ERROR_BIT_LARGE 2

int bit_n_to_zero(int input, size_t bit_to_zero);

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    struct gengetopt_args_info args;

	if(cmdline_parser(argc, argv, &args)){
		ERROR(ERROR_CMDLINE, "Erro no cmdline_parser() \n");
	}

    int ret = bit_n_to_zero(args.input_arg, args.bit_to_zero_arg);
    printf("Valor: %d com o bit #%zu (%d)\n", args.input_arg, args.bit_to_zero_arg, ret);

    cmdline_parser_free(&args);
    return 0;
}

int bit_n_to_zero(int input, size_t bit_to_zero){
    size_t num_bits_int = sizeof(int)*8;
    unsigned int mask;

    if (bit_to_zero > num_bits_int-1) {
        ERROR(ERROR_BIT_LARGE,"Valor de bit_n fora da gama.\n");
    }

    mask = 1 << bit_to_zero; // prepara a mascara para a verificacao (AND binario)
    return input & ~mask;
}
