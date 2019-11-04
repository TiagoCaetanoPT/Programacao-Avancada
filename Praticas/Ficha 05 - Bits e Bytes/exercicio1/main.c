#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "math.h"

#include "debug.h"
#include "memory.h"
#include "args.h"

#define ERROR_STRING    1
#define ERROR_CMDLINE   2

int bin_to_decimal(const char *bin_s_ptr);

int main(int argc, char const *argv[]) {
    (void)argc; (void)argv;

    struct gengetopt_args_info args;
    int decimalValue;

	if(cmdline_parser(argc, argv, &args)){
		ERROR(ERROR_CMDLINE, "Erro no cmdline_parser() \n");
	}

    decimalValue = bin_to_decimal(args.string_binaria_arg);
    printf("Valor binário: %s\n", args.string_binaria_arg);
    printf("Valor decimal: %d\n", decimalValue);

    cmdline_parser_free(&args);
    return 0;
}

int bin_to_decimal(const char *bin_s_ptr){
    size_t num_chars = strlen(bin_s_ptr); //guarda o tamnaho da string
    const char *sPtr = bin_s_ptr + num_chars -1; //aponta para o ultimo caracter da string

    size_t i = 0;
    int decimalValue = 0;
    while (i<num_chars) { // percorre a string de forma inversa
        if (*sPtr == '1') {
            decimalValue += 1*pow(2,i); // pow() -> funcao para calcular a potencia matematica
        } else if (*sPtr != '0') {
            ERROR(ERROR_STRING,"Não é uma string valida em binario!");
        }
        i++;
        sPtr--;
    }
    return decimalValue;
}
