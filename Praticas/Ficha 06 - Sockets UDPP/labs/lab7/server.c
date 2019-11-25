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
#include <math.h>
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"

void processa_pedido(int socket_udp);

int main(int argc, char *argv[]){
    (void)argc; (void)argv;

    /* criar socket */
    int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_udp == -1){
        ERROR(ERROR_SOCKET, "Erro ao criar o socket!\n");
    }

    /* bind */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; //familia de end. (IPv4)
    serv_addr.sin_port = htons(PORT); //porto de escuta
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(socket_udp, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(ret == -1){
        close(socket_udp);
        ERROR(ERROR_BIND, "Erro ao registar socket com o porto: %d\n", PORT);
    }

    processa_pedido(socket_udp);

    /* fechar o socket */
    close(socket_udp);

    return 0;
}

void processa_pedido(int socket_udp){
    uint16_t number;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    double value;
    char response[MAX_STR];

    /* receber o pedido */
    if (recvfrom(socket_udp, &number, sizeof(number), 0, (struct sockaddr *)&client_addr, &client_addr_len) == -1){
        ERROR(ERROR_RECV, "Erro ao receber dados\n");
    }

    number = ntohs(number); // converte para little endian
    printf("Foi recebido o valor '%d' do cliente\n", number);

    /* calcular a raiz quadrada */
    value = sqrt(number);
    sprintf(response, "%f", value);

    printf("Valor da raiz quadrada: %f\n", value);

    /* devolver resposta */
    if(sendto(socket_udp, response, strlen(response) + 1, 0, (struct sockaddr *)&client_addr, client_addr_len)==-1){
        ERROR(ERROR_SENDTO, "Erro ao enviar dados\n");
    }
}
