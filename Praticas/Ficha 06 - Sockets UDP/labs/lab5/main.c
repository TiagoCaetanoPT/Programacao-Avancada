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
#include <arpa/inet.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "limits.h" // para usar o USHRT_MAX

#define ERROR_PORT          1
#define ERROR_SOCKET        2
#define ERROR_BIND          3

int main(int argc, char *argv[]){
    /* Receber porto a registar */
    struct gengetopt_args_info args;
    cmdline_parser(argc, argv, &args);

    int port = args.port_arg;

    if (port < 1 || port > USHRT_MAX) { //verificar se o porto está na gama válida
        ERROR(ERROR_PORT, "Porto fora da gama permitida!\n");
    }

    /* Criar e registar socket UDP IPv4 */
    int socket_udp= socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp == -1){
        ERROR(ERROR_SOCKET, "Can't create udp_server_socket (IPv4).\n");
    }

    /* Registo local - bind */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; //AF_INET -> IPv4
    serv_addr.sin_port = htons(port); //htons -> coloca em bigendian ... define o porque à escuta
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Fica à escuta em todas as interfaces de rede ... INADDR_ANY -> todas as interfaces

    int ret = bind(socket_udp, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret == -1){
        close(socket_udp);
        ERROR(ERROR_BIND, "Erro ao registar socket com o porto: %d\n", port);
    }

    printf("O socket registado corretamente no porto: %d\n", port);

    close(socket_udp);

    /* Liberta memória */
    cmdline_parser_free(&args);

    return 0;
}

/*
* Perguntas:
*  a)  O que sucede quando é indicado um porto entre 1 e 1023 (inclusive)?
    O sistema não permite.
*  b)  O que sucede quando se tenta registar um porto que já está registado?
*      Nota: é possível obter uma lista dos portos UDP registados no sistema através do utilitário netstat, executado da seguinte forma: netstat -u -l
    Infroma que o porto já está em uso
*/
