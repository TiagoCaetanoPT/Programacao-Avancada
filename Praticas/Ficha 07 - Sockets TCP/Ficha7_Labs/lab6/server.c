#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"
#include "common.h"

int main(int argc, char *argv[]) {
    struct gengetopt_args_info args;

    if( cmdline_parser(argc, argv, &args) )
    ERROR(99, "Erro: execução de cmdline_parser\n");


    // =========================================================
    // ================================================= SOCKETS
    // =========================================================
    /* criar socket - servidor */
    int tcp_server_socket;
    if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    ERROR(51, "Can't create tcp_server_socket (IPv4)");


    // =========================================================
    // ==================================================== BIND
    // =========================================================
    /* registar socket - servidor */
    struct sockaddr_in tcp_server_endpoint;
    memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
    tcp_server_endpoint.sin_family = AF_INET;
    tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);        // Todas as interfaces de rede
    tcp_server_endpoint.sin_port = htons(args.porto_arg);       // Server port

    if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
    ERROR(52, "Can't bind @tcp_server_endpoint");


    // =========================================================
    // ================================================== LISTEN
    // =========================================================
    /* listen */
    int tcp_max_simultaneous_clients = 1;
    if (listen(tcp_server_socket, tcp_max_simultaneous_clients)  == -1)
    ERROR(53, "Can't listen for %d clients", tcp_max_simultaneous_clients);


    // =========================================================
    // ================================================== ACCEPT
    // =========================================================
    /* accept */
    char tcp_client_string_ip[20];
    struct sockaddr_in tcp_client_endpoint;
    memset(&tcp_client_endpoint, 0, sizeof(struct sockaddr_in));
    socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
    int tcp_client_socket;
    printf("à espera da ligação do cliente... "); fflush(stdout);
    if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1)
    ERROR(54, "Can't accept client");
    printf("ok. \n");
    printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));

    // =========================================================
    // ============================================= RECV + SYNC
    // =========================================================
    /* comunicacao com o cliente */
    ssize_t recv_bytes, send_bytes;
    char response[MAX_STR];
    // unsigned short response, response_net;
    uint8_t request;

    if((recv_bytes = recv(tcp_client_socket, &request, sizeof(request), 0)) == -1){
        close(tcp_client_socket);
        ERROR(43, "Erro no recv()\n");
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    switch (request) {
        case 0: //data e hora
            sprintf(response, "%d-%d-%d_%d:%d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            /* send ok */
            if((send_bytes = send(tcp_client_socket, response, MAX_STR, 0)) == -1){
                close(tcp_client_socket);
                ERROR(43, "Erro no send()\n");
            }
        break;
        case 1: //apenas data
            sprintf(response, "%d-%d-%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
            /* send ok */
            if((send_bytes = send(tcp_client_socket, response, MAX_STR, 0)) == -1){
                close(tcp_client_socket);
                ERROR(43, "Erro no send()\n");
            }
        break;
        case 2: //apenas hora
            sprintf(response, "%d:%d:%d", t->tm_hour, t->tm_min, t->tm_sec);
            /* send ok */
            if((send_bytes = send(tcp_client_socket, response, MAX_STR, 0)) == -1){
                close(tcp_client_socket);
                ERROR(43, "Erro no send()\n");
            }
        break;
    }

    printf("Foram enviados %d bytes!\n", send_bytes);


    // =========================================================
    // ======================================== FECHA OS SOCKETS
    // =========================================================
    /* close - socket cliente */
    close(tcp_client_socket);
    /* close - socket servidor */
    close(tcp_server_socket);

    cmdline_parser_free(&args);

    exit(0);
}
