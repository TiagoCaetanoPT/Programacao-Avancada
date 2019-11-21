#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"

void processa_pedido(int socket_udp);


int main(int argc, char *argv[]){
    /* Disable warnings */
    (void)argc; (void)argv;

    /* Criar e registar socket UDP IPv4 */
    int socket_udp= socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp == -1){
        ERROR(ERROR_SOCKET, "Can't create udp_server_socket (IPv4).\n");
    }

    /* Registo local - bind */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; //AF_INET -> IPv4
    serv_addr.sin_port = htons(PORTO); //htons -> coloca em bigendian ... define o porto à escuta
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Fica à escuta em todas as interfaces de rede ... INADDR_ANY -> todas as interfaces

    int ret = bind(socket_udp, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret == -1){
        close(socket_udp);
        ERROR(ERROR_BIND, "Erro ao registar socket com o porto\n");
    }


    processa_pedido(socket_udp);

    close(socket_udp);

    return 0;
}


void processa_pedido(int socket_udp){
    uint16_t number;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    double value;
    char response[MAX_SRT];

    if (recvfrom(socket_udp, &number, sizeof(number), 0, (struct sockaddr *) &client_addr, &client_addr_len) == -1) //receber o pedido
    ERROR(ERROR_RECVFROM, "Can't recvfrom client\n");

    number = ntohs(number);
    printf("Foi recebido o valor: %d do cliente\n", number);

    //calcular raiz quadrada
    value = sqrt(number);

    sprintf(response, "%f", value);
    printf("Valor da raiz quadrada: %f\n", value);

    //devove a resposta
    if (sendto(socket_udp, response, strlen(response) +1, 0, (struct sockaddr *) &client_addr, client_addr_len) == -1) //enviar resposta
    ERROR(ERROR_SENDTO, "Erro a enviar dados.\n");
}
