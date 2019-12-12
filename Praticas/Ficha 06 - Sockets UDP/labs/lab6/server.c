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
#include <limits.h>
#include <unistd.h>
#include <math.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"


int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
        exit(ERR_ARGS);
    }

    int port = args_info.port_arg;

    if(port < 1 || port > USHRT_MAX){
        fprintf(stderr, "ERROR - Invalid port (%d)\n", port);
        exit(2);
    }

    printf("Port: %d\n", port);

    // UDP IPv4: cria socket
    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(31, "Can't create udp_server_socket (IPv4)");

    // UDP IPv4: bind a IPv4/porto 
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);    // Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(port);    // Server port
    if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
        ERROR(32, "Can't bind @udp_server_endpoint info");

    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;

    uint16_t r_value = 0;

    while(1){
        // UDP IPv4: "recvfrom" do cliente (bloqueante) //1º paramnetro e o socket o 2º sao os dados o 3º e o tamanho desses dados
        printf("à espera de dados do cliente... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_server_socket, &r_value, sizeof(r_value), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
            ERROR(34, "Can't recvfrom client");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);

        //inverte or cofirmar endianess
        r_value = ntohs(r_value);

        printf("Random value received: %hu \n", r_value);

        float f_value = sqrtf(r_value);

        printf("Squared value: %0.3f \n", f_value);

        char value_str[100];

        sprintf(value_str, "%0.3f", f_value);

        printf("Squared value string %s\n", value_str);

        // UDP IPv4: "sendto" para o cliente
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket, value_str, strlen(value_str), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

        printf("Buffer enviado para o cliente deve de ser o valor da raiz quadrada %s\n",value_str );

    }

    // liberta recurso: socket UDP IPv4
    if (close(udp_server_socket) == -1)
        ERROR(33, "Can't close udp_server_socket (IPv4)");

    cmdline_parser_free(&args_info);

    return 0;
}
