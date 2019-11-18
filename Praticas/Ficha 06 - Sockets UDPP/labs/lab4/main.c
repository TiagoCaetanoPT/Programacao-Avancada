#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"
#include "memory.h"

int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    /* a) Criar um socket UDP / IPv4 */
    int socket_udp_ipv4= socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp_ipv4 == -1){
        ERROR(1, "Can't create udp_server_socket (IPv4)");
    }

    /* b) Criar um socket UDP / IPv6 */
    int socket_udp_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_udp_ipv6 == -1){
        ERROR(2, "Can't create udp_server_socket (IPv6)");
    }

    /* c) Mostrar descritores dos sockets */
    printf("Descritor do socket (IPv4): %d\n", socket_udp_ipv4);
    printf("Descritor do socket (IPv6): %d\n", socket_udp_ipv6);

    /* d) Fechar os descritores dos sockets */
    if (close(socket_udp_ipv4) == -1){
        ERROR(3, "Can't close udp_server_socket (IPv4)");
    }
    if (close(socket_udp_ipv6) == -1){
        ERROR(4, "Can't close udp_server_socket (IPv6)");
    }


    return 0;
}
