#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, ret;
	socklen_t clilen;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");

	ret = listen(sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(sockfd, &read_fds);
	fdmax = sockfd;
	int clients[MAX_CLIENTS], j;
	for (j = 0; j < MAX_CLIENTS; j++) {
		clients[j] = -1;
	}
	int contor_client = 0;

	while (1) {
		tmp_fds = read_fds; 
		
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == sockfd) {
					struct msg *mesaj = (struct msg*)calloc(1, sizeof(struct msg));
					// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					DIE(newsockfd < 0, "accept");
					clients[contor_client] = newsockfd;
					contor_client++;
					mesaj->destination = newsockfd;
					// construct buffer with information
					int k = 0;
					for (j = 0; j < contor_client - 1; j++) {
						mesaj->buffer[k] = clients[j] + '0';
						mesaj->buffer[k + 1] = ' ';
						k += 2;
					}
					strcat(mesaj->buffer, " sunt clientii conectati.\n\0");
					// trimite mesajul la clientul tocmai creat cu toti clientii conectat
					send(newsockfd, mesaj, sizeof(struct msg), 0);
					memset(mesaj->buffer, 0, BUFLEN);
					mesaj->destination = 0;
					// trimite mesaj la toti clienti din retea
					for (j = 0; j < contor_client - 1; j++) {
						mesaj->destination = clients[j];
						sprintf(mesaj->buffer, "Clientul %d s-a conectat\n", newsockfd);
						send(clients[j], mesaj, sizeof(struct msg), 0);
					}
					// se adauga noul socket intors de accept() la multimea descriptorilor de citire
					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax) {
						fdmax = newsockfd;
					}
					printf("Noua conexiune de la %s, port %d, socket client %d\n",
							inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
				} else {
					// s-au primit date pe unul din socketii de client,
					// asa ca serverul trebuie sa le receptioneze
					struct msg *mesaj = (struct msg *)calloc(1, sizeof(struct msg));

					memset(mesaj->buffer, 0, BUFLEN);
					n = recv(i, mesaj, sizeof(struct msg), 0);
					DIE(n < 0, "recv");

					if (n == 0) {
						// conexiunea s-a inchis
						printf("Socket-ul client %d a inchis conexiunea\n", i);
						close(i);
						// se scoate din multimea de citire socketul inchis
						for (j = 0; j < contor_client; j++) {
							if (clients[j] != i) {
								sprintf(mesaj->buffer, "Clientul %d s-a deconectat\n", i);
								send(clients[j], mesaj, sizeof(struct msg), 0);
							}
						}
						for (j = 0; j < contor_client; j++) {
							if (clients[j] == i) {
								break;
							}
						}
						for (int k = j; k < contor_client; k++) {
							clients[k] = clients[k + 1];
						}
						contor_client--;
						FD_CLR(i, &read_fds);
					} else {
						printf ("S-a primit de la clientul de pe socketul %d mesajul: %s\n", i, mesaj->buffer);
						// char recv_msg[] = "Mesaj primit\0";
						// n = send(i, recv_msg, strlen(recv_msg) + 1, 0);
						// DIE(n < 0, "send server");
						// if (FD_ISSET(clients[0], &tmp_fds)) {
							// n = send(clients[1], mesaj, sizeof(struct msg), 0);
							// DIE(n < 0, "send client0 to client1");
						// } else {
							// n =  send(clients[0], mesaj, sizeof(struct msg), 0);
							// DIE(n < 0, "send client1 to client0");
						// }
						for (j = 0; j < contor_client; j++) {
							if (FD_ISSET(clients[j], &tmp_fds)) {
								n = send(mesaj->destination, mesaj, sizeof(struct msg), 0);
								DIE(n < 0, "send server to client");
							}
						}
					}
					free(mesaj);
				}
			}
		}
	}

	close(sockfd);

	return 0;
}
