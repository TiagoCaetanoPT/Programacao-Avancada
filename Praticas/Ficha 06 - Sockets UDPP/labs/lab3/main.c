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

void mostra_zona_memoria(void *zona_memoria, int tamanho);

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    /* Mostrar tamanho (em bytes) do tipo struct sockaddr_in */
    printf("Tamanho do tipo struct sockaddr_in: %d bytes\n", sizeof(struct sockaddr_in));

    /* Colocar a "zero" uma variável (my_addr_IPv4) do tipo struct sockaddr_in */
    struct sockaddr_in my_addr_IPv4;
    // void *memset(void *dest, int c, size_t n);
    memset(&my_addr_IPv4, 0, sizeof(my_addr_IPv4));

    /* Mostrar bytes da memória da variável my_addr_IPv4 (criar e chamar função)*/
    mostra_zona_memoria(&my_addr_IPv4, sizeof(my_addr_IPv4));

    /* Colocar a "4" os primeiros 6 bytes da variável my_addr_IPv4 */
    memset(&my_addr_IPv4, 4, 6);

    /* Mostrar bytes da memória da variável my_addr_IPv4 (chamar função)*/
    printf("==============================================\n");
    mostra_zona_memoria(&my_addr_IPv4, sizeof(my_addr_IPv4));
\
    return 0;
}

void mostra_zona_memoria(void *zona_memoria, int tamanho){
    // ptr aponta para o 1º byte de zona de memória
    unsigned char *ptr = (unsigned char *)zona_memoria;

    for (size_t i = 0; i < tamanho; i++) {
        printf("[%02u] 0x%x\n", i, ptr[i]);
    }
}
