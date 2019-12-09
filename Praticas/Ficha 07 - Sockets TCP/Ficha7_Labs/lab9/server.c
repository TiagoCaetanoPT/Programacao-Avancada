#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/wait.h>

#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"

#define MAX_STR 64

int setup_TCP_server(int port, int backlog);
void handle_requests(int tcp_server_socket);
void process_option(uint8_t option, char *buffer, int buffer_len);

int main(int argc, char *argv[]) {
    struct gengetopt_args_info args;
    int tcp_server_socket;

    if( cmdline_parser(argc, argv, &args) )
    ERROR(99, "Erro: execução de cmdline_parser\n");

    int max_connections = 8; // Fila de espera
    tcp_server_socket = setup_TCP_server(args.porto_arg, max_connections);

    handle_requests(tcp_server_socket);

    close(tcp_server_socket);

    cmdline_parser_free(&args);

    exit(0);
}

void handle_requests(int tcp_server_socket){
    char tcp_client_string_ip[20];
    struct sockaddr_in tcp_client_endpoint;
    socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
    int tcp_client_socket;
    uint8_t option;
    char buffer[MAX_STR];
    ssize_t tcp_send_bytes, tcp_recv_bytes;
    pid_t pid;

    while(1){
        /* accept */
        printf("à espera da ligação do cliente... "); fflush(stdout);
        if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1)
        ERROR(54, "Can't accept client");
        printf("ok. \n");
        printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));

        pid = fork();
        if (pid == 0) { // processo filhos
            /* recv */
            printf("à espera de dados do servidor... "); fflush(stdout);
            if ((tcp_recv_bytes = recv(tcp_client_socket, &option, sizeof(option), 0)) == -1)
            ERROR(47, "Can't recv from server");
            printf("ok.  (%d bytes recebidos)\n", (int)tcp_recv_bytes);

            process_option(option, buffer, MAX_STR);

            sleep(5);

            /* send */
            if ((tcp_send_bytes = send(tcp_client_socket, buffer, strlen(buffer), 0)) == -1)
            ERROR(47, "Can't send to client");
            printf("ok.  (%d bytes enviados)\n", (int)tcp_send_bytes);

            /* close */
            if (close(tcp_client_socket) == -1)
            ERROR(55, "Can't close tcp_client_socket (IPv4)");

            exit(0);
        } else if (pid > 0) { //processo pai
            if (close(tcp_client_socket) == -1)
            ERROR(55, "Can't close tcp_client_socket (IPv4)");
        } else { // erro
            ERROR(1, "Erro no fork()!\n");
        }
    }
}

void process_option(uint8_t option, char *buffer, int buffer_len){
    time_t t = time(NULL);
    struct tm *localtime_ptr;
    localtime_ptr = localtime(&t);// obtem data do servidor

    if(localtime_ptr == NULL){
        WARNING("Erro no localtime()\n");
        return;
    }

    switch(option){
        case 0: //data+hora
        //YYYYMMDD_HHhmm:ss
        strftime(buffer, buffer_len, "%Y%m%d_%Hh%M:%S", localtime_ptr);
        break;
        case 1: //data
        strftime(buffer, buffer_len, "%Y%m%d", localtime_ptr);
        break;
        case 2: //hora
        strftime(buffer, buffer_len, "%Hh%M:%S", localtime_ptr);
        break;
        default://msg de erro
        snprintf(buffer, buffer_len, "Unknown option '%d'", option);
    }
}

int setup_TCP_server(int port, int backlog){
    /* socket servidor */
    int tcp_server_socket;
    if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    ERROR(51, "Can't create tcp_server_socket (IPv4)");

    /* bind */
    struct sockaddr_in tcp_server_endpoint;
    memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
    tcp_server_endpoint.sin_family = AF_INET;
    tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);        // Todas as interfaces de rede
    tcp_server_endpoint.sin_port = htons(port);     // Server port
    if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
    ERROR(52, "Can't bind @tcp_server_endpoint");

    /* listen */
    if (listen(tcp_server_socket, backlog)  == -1)
    ERROR(53, "Can't listen for %d clients", backlog);

    return tcp_server_socket;
}
