#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "../debug/debug.h"
#include "../cmdline/client_cmdline.h"

#define MAX_STR 64

int main(int argc, char *argv[])
{
    ssize_t tcp_read_bytes, tcp_sent_bytes;
    struct gengetopt_args_info args;

    // cmdline_parser: deve ser a primeira linha de código no main
    if( cmdline_parser(argc, argv, &args) )
        ERROR(99, "Erro: execução de cmdline_parser\n");

    /* socket */
    int tcp_client_socket;
    if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        ERROR(41, "Can't create tcp_client_socket (IPv4)");

    // TCP IPv4: connect ao IP/porto do servidor
    struct sockaddr_in tcp_server_endpoint;
    memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
    tcp_server_endpoint.sin_family = AF_INET;
    switch (inet_pton(AF_INET, args.ip_arg, &tcp_server_endpoint.sin_addr)) {
        case 0:
            fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",__FILE__, __LINE__);
            exit(22);
        case -1:
            ERROR(22, "Cannot convert IP address (IPv4)");
    }
    tcp_server_endpoint.sin_port = htons(args.porto_arg);

    /* connect */
    printf("a ligar ao servidor... "); fflush(stdout);
    if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
        ERROR(43, "Can't connect @tcp_server_endpoint");
    printf("ok. \n");

    /* pedido - send */
    uint8_t request = 0;
    char buffer[MAX_STR];

    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, &request, sizeof(request), 0)) == -1)
        ERROR(46, "Can't send to server");
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    /* receber a resposta - recv */

    printf("à espera de dados do servidor... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, buffer, MAX_STR, 0)) == -1)
        ERROR(47, "Can't recv from server");
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);

    buffer[tcp_read_bytes] = '\0';

    printf("Resposta do servidor: %s\n", buffer);

    /* fechar socket */
    if (close(tcp_client_socket) == -1)
        ERROR(56, "Can't close tcp_server_socket (IPv4)");
    printf("ligação fechada. ok. \n");

    // libertar recurso (cmdline_parser)
    cmdline_parser_free(&args);

    exit(0);
}
