/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "helpers.h"

void run_client(int sockfd) {
    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);

    int byte_count;

    while (read(STDIN_FILENO, buf, BUFLEN - 1) > 0 && !isspace(buf[0])) {
        byte_count = strlen(buf) + 1;

        int bytes_send;
        int bytes_remaining = byte_count;
        int bytes_received;

        // TODO 4: Trimiteti mesajul catre server prin sockfd
        send(sockfd, buf, byte_count, 0);

        memset(buf, 0, BUFLEN);
       
        // TODO 5: Receptionati un mesaj venit de la server
        bytes_received = recv(sockfd, buf, bytes_remaining, 0);

        fprintf(stderr, "Received: %s", buf);

        memset(buf, 0, BUFLEN);
    }
}

int main(int argc, char* argv[])
{
    int sockfd = -1;
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);
    memset(&serv_addr, 0, socket_len);

    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // TODO 1: Creati un socket TCP pentru conectarea la server
    sockfd = socket(AF_INET | PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    inet_aton("127.0.0.1", &(serv_addr.sin_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // TODO 2: Completati in serv_addr adresa serverului, familia de adrese si portul pentru conectare

    // TODO 3: Creati conexiunea catre server
    connect(sockfd, (struct sockaddr*)&serv_addr, socket_len); 

    run_client(sockfd);

    // TODO 6: Inchideti conexiunea si socketul creat
    close(sockfd);

    return 0;
}
