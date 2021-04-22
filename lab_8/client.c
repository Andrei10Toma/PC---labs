#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_address server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;

	fd_set read_fds, tmp_fds;
	int fd_max;

	if (argc < 3) {
		usage(argv[0]);
	}

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	ret = inet_aton(argv[1], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	FD_SET(sockfd, &read_fds);
	FD_SET(STDIN_FILENO, &read_fds);
	fd_max = sockfd;

	while (1) {
		tmp_fds = read_fds;
		ret = select(fd_max + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select client");
  		// se citeste de la tastatura
		if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
			struct msg* mesaj = (struct msg*)calloc(1, sizeof(struct msg));
			memset(mesaj->buffer, 0, BUFLEN);
			scanf("%d ", &mesaj->destination);
			fgets(mesaj->buffer, BUFLEN, stdin);
			if (strncmp(mesaj->buffer, "exit", 4) == 0) {
				break;
			}
			// se trimite mesaj la server
			n = send(sockfd, mesaj, sizeof(struct msg), 0);
			DIE(n < 0, "send");
			free(mesaj);
		} else if (FD_ISSET(sockfd, &tmp_fds)) {
			// receive de la server
			struct msg *mesaj = (struct msg *)calloc(1, sizeof(struct msg));
			n = recv(sockfd, mesaj, sizeof(struct msg), 0);
			DIE(n < 0, "recv client");
			printf("%s", mesaj->buffer);
			free(mesaj);
		}
	}

	close(sockfd);

	return 0;
}
