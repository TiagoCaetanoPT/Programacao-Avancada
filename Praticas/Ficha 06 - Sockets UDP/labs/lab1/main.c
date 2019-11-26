#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <arpa/inet.h>

#include "debug.h"
#include "memory.h"
#include "args.h"

#define ERROR_INET_PTON     1

int main(int argc, char *argv[]){
    /* Receber endereço IPv4 em formato texto */
    struct gengetopt_args_info args;
    cmdline_parser(argc, argv, &args);

    char *IPv4_addr_str = args.address_arg;
    uint32_t IPv4_addr_bin; //unsigned int

    int res = inet_pton(AF_INET, IPv4_addr_str, &IPv4_addr_bin);
    unsigned char *ptr;
    switch (res) {
        case 1: // consegui converter
            ptr = (unsigned char *)&IPv4_addr_bin; //ptr a apontar para o 1º

            // a cada iteração mostra 1 byte
            for (size_t i = 0; i < sizeof(IPv4_addr_bin); i++) {
                printf("Byte #%02u = %u\n", i, ptr[i]);
            }
        break;
        case 0:
            ERROR(ERROR_INET_PTON, "Não contem um edereço válido.\n");
        break;
        case -1:
            ERROR(ERROR_INET_PTON, "AF não contém uma família de endereços válida.\n");
    }

    /* Libertar memória alocada pelo gengetopt */
    cmdline_parser_free(&args);
	return 0;
}
