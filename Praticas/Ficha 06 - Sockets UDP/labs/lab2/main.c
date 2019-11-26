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

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

    /* Verificar tamanho do tipo short */
    short a = 0x0102;

    if (sizeof(short) != 2) {
        ERROR(1,"O tamanho do short é diferente de 2 bytes!\n");
    }

    unsigned char *aPtr = (unsigned char *)&a; //aponta para o 1º byte de a

    printf("aPtr: %d\n", *aPtr);

    /* Determinar endianess da máquina local */
    if (*aPtr == 01) {
        printf("A suar arquitetura endian é: Big-endian\n");
    }else if (*aPtr == 02){
        printf("A suar arquitetura endian é: Little-endian\n");
    }
    
	return 0;
}
