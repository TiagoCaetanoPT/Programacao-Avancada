
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"

int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args) != 0){
        exit(ERR_ARGS);
    }

    /* criar socket */
    int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_udp == -1)
        ERROR(ERROR_SOCKET, "Erro na criação do socket\n");

    /* criar estrutura com os dados do servidor */
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);

    memset(&serv_addr, 0, sizeof(serv_addr));//inicializacao da estr.
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(args.port_arg);

    switch (inet_pton(AF_INET, args.ip_arg, &serv_addr.sin_addr.s_addr)) {
        case 0:
            ERROR(22, "Cannot convert IP address (IPv4)");
            break;
        case -1:
            ERROR(22, "Cannot convert IP address (IPv4)");
            break;
    }
    cmdline_parser_free(&args);

    char request[MAX_STR], response[MAX_STR];
    ssize_t udp_read_bytes, udp_sent_bytes;

    while(1){
        /* obtem string a enviar */
        printf("String a enviar: ");
        if(fgets(request, MAX_STR-1, stdin) == NULL){
            ERROR(ERROR_FGETS, "Erro no fgets()\n");
        }

        if(strlen(request) > 0){
            request[strlen(request)-1] = '\0';
        }

        if(strcmp(request, "fim") == 0){
            break;
        }

        /* enviar pedido */
        if ((udp_sent_bytes = sendto(socket_udp, request, strlen(request) + 1, 0, (struct sockaddr *) &serv_addr, serv_addr_len)) == -1)
            WARNING("Erro no sendto()\n");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
        printf("String enviada para o servidor: %s\n\n", request);


        /* receber resposta */
        printf("à espera da resposta do servidor... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(socket_udp, response, MAX_STR, 0, (struct sockaddr *) &serv_addr, &serv_addr_len)) == -1){
            WARNING("Erro no recv()\n");
        }else{
            printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);
            printf("Resposta do servidor: %s\n\n", response);
        }
    }
    close(socket_udp);

    return 0;
}
