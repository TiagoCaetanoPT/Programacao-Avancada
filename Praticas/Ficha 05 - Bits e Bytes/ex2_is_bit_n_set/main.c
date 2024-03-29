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

int is_bit_n_set(int input, size_t bit_n);

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    struct gengetopt_args_info args;

	if(cmdline_parser(argc, argv, &args)){
		ERROR(ERROR_CMDLINE, "Erro no cmdline_parser() \n");
	}

    int ret = is_bit_n_set(args.input_arg, args.bit_n_arg);
    printf("bit #%zu tem valor %d (%d)\n",args.bit_n_arg, ret ,args.input_arg);

    cmdline_parser_free(&args);
    return 0;
}

int is_bit_n_set(int input, size_t bit_n){
    //int = 4 bytes = 32 bits
    size_t num_bits_int = sizeof(int)*8;
    unsigned int mask;

    if (bit_n > num_bits_int-1) {
        ERROR(ERROR_BIT_LARGE,"Valor de bit_n fora da gama.\n");
    }

    mask = 1 << bit_n; // prepara a mascara para a verificacao (AND binario)
    if (input & mask) {
        return 1;
    }
    else{
        return 0;
    }
}
