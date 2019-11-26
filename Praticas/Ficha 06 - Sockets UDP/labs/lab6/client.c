#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"


int main(int argc, char *argv[]){

    /* Disable warnings */
    (void)argc; (void)argv;

    uint16_t number;

    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    memset(&serv_addr, 0, sizeof(serv_addr));

    /* Criar e registar socket UDP IPv4 */
    int socket_udp= socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp == -1){
        ERROR(ERROR_SOCKET, "Can't create udp_server_socket (IPv4).\n");
    }

    /* Registo local - bind */
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));

    client_addr.sin_family = AF_INET; //AF_INET -> IPv4
    client_addr.sin_port = htons(PORT); //htons -> coloca em bigendian ... define o porto à escuta
    client_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    //gerar numero a enviar
    srand(time(NULL));
    number = rand() %100 +1; //para gerar de 1 a 100
    printf("O numero gerado foi: %d\n", number);

    number = htons(number); //converter para rede

    //Envia a pedido
    if (sendto(socket_udp, &number, sizeof(number) +1, 0, (struct sockaddr *) &serv_addr, serv_addr_len) == -1) //enviar resposta
    ERROR(ERROR_SENDTO, "Erro ao enviar dados.\n");

    char response[MAX_STR];
    //receber resposta
    if (recvfrom(socket_udp, response, MAX_STR, 0, (struct sockaddr *) &serv_addr, &serv_addr_len) == -1) //receber resposta
    ERROR(ERROR_RECVFROM, "Can't recvfrom client\n");

    printf("O valor da raiz quadrada: %s\n", response);

    //fechar socket
    close(socket_udp);

    return 0;
}
