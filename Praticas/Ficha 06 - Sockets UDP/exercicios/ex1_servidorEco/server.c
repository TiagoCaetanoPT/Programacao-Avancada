/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"

void processa_pedido(int socket_udp);

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

    /* registar socket */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));//inicializacao da estr.

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(args.port_arg);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* registo do socket */
    int res = bind(socket_udp, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(res == -1){
        close(socket_udp);
        ERROR(ERROR_BIND, "Erro ao registar o socket com o porto: %d\n", args.port_arg);
    }

    cmdline_parser_free(&args);

    while(1){
        /* processar pedidos */
        processa_pedido(socket_udp);
    }

    /* fecho socket */
    close(socket_udp);

    return 0;
}

void processa_pedido(int socket_udp){
    ssize_t udp_read_bytes, udp_sent_bytes;
    char request[MAX_STR], response[MAX_STR];
    memset(request, 0, MAX_STR);
    memset(response, 0, MAX_STR);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    /* receber pedido do cliente */
    printf("\nà espera de dados do cliente... "); fflush(stdout);
    if ((udp_read_bytes = recvfrom(socket_udp, request, MAX_STR, 0, (struct sockaddr *) &cli_addr, &cli_addr_len)) == -1)
        ERROR(34, "Can't recvfrom client");
    printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);

    /* dados recebidos  */
    char ip[20];
    if(inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof (ip)) == NULL){
        ERROR(ERROR_INET_NTOP, "Erro no inet_ntop()\n");
    }
    printf("String recebida do cliente %s:%d: %s\n\n", ip, ntohs(cli_addr.sin_port), request);

    strcpy(response, request);

    /* devolver resposta */
    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((udp_sent_bytes = sendto(socket_udp, response, strlen(response) + 1, 0, (struct sockaddr *) &cli_addr, cli_addr_len)) == -1)
        ERROR(35, "Can't sendto client");
    printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
    printf("String enviada para o cliente: %s\n\n", request);
}
