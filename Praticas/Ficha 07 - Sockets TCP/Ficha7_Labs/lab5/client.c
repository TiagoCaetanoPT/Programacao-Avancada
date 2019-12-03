#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

#include "../debug/debug.h"
#include "../cmdline/client_cmdline.h"
#include "common.h"


int main(int argc, char *argv[])
{
    struct gengetopt_args_info args;

    // cmdline_parser: deve ser a primeira linha de código no main
    if( cmdline_parser(argc, argv, &args) )
        ERROR(99, "Erro: execução de cmdline_parser\n");

    // =========================================================
    // ================================================= SOCKETS
    // =========================================================
    /* socket */
    int tcp_client_socket;
    if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        ERROR(41, "Can't create tcp_client_socket (IPv4)");


    // =========================================================
    // ================================================= CONNECT
    // =========================================================
    /* connect */
    struct sockaddr_in tcp_server_endpoint;
    memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
    tcp_server_endpoint.sin_family = AF_INET;

    /* converter ip de formato string para binário (rede) */
    switch (inet_pton(AF_INET, args.ip_arg, &tcp_server_endpoint.sin_addr)) {
        case 0:
            fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n", __FILE__, __LINE__);
            exit(22);
        case -1:
            ERROR(22, "Cannot convert IP address (IPv4)");
    }
    tcp_server_endpoint.sin_port = htons(args.porto_arg);

    printf("a ligar ao servidor... "); fflush(stdout);
    if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
        ERROR(43, "Can't connect @tcp_server_endpoint");
    printf("ok. \n");


    // =========================================================
    // ==================================================== SEND
    // =========================================================
    /* enviar string ao servidor */
    //args.numero_arg
    ssize_t send_bytes, recv_bytes;
    char buffer[MAX_STR], ok;

    /* envia 123 */
    sprintf(buffer, "123");

    if((send_bytes = send(tcp_client_socket, buffer, strlen(buffer), 0)) == -1 ){
        close(tcp_client_socket);
        ERROR(43, "Erro no send()\n");
    }
    printf("ok. (%d bytes enviados)\n", send_bytes);
    printf("String enviada: %s\n\n", buffer);

    /* recebe o ok */
    if((recv_bytes = recv(tcp_client_socket, &ok, sizeof(ok), 0)) == -1 ){
        close(tcp_client_socket);
        ERROR(43, "Erro no recv()\n");
    }
    printf("ok. (%d bytes recebidos)\n", recv_bytes);

    /* envia 45678 */
    sprintf(buffer, "45678");

    if((send_bytes = send(tcp_client_socket, buffer, strlen(buffer), 0)) == -1 ){
        close(tcp_client_socket);
        ERROR(43, "Erro no send()\n");
    }

    printf("ok. (%d bytes enviados)\n", send_bytes);
    printf("String enviada: %s\n\n", buffer);

    /* recebe o ok */
    if((recv_bytes = recv(tcp_client_socket, &ok, sizeof(ok), 0)) == -1 ){
        close(tcp_client_socket);
        ERROR(43, "Erro no recv()\n");
    }
    printf("ok. (%d bytes recebidos)\n", recv_bytes);


    // =========================================================
    // ======================================== FECHA OS SOCKETS
    // =========================================================
    /* close */
    if (close(tcp_client_socket) == -1)
        ERROR(45, "Can't close tcp_client_socket (IPv4)");
    printf("ligação fechada. ok. \n");

    // libertar recurso (cmdline_parser)
    cmdline_parser_free(&args);

    exit(0);
}
