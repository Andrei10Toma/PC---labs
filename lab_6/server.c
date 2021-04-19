/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "helpers.h"


void usage(char*file)
{
	fprintf(stderr,"Usage: %s server_port file\n",file);
	exit(0);
}

/*
*	Utilizare: ./server server_port nume_fisier
*/
int main(int argc,char**argv)
{
	int fd;

	if (argc!=3)
		usage(argv[0]);
	
	struct sockaddr_in my_sockaddr, from_station ;
	char buf[BUFLEN];


	/*Deschidere socket*/
	int socketid = socket(PF_INET, SOCK_DGRAM, 0);
	if (socketid == -1) {
		perror("EROARE LA DESCHIDEREA SOCKETULUI");
		exit(-1);
	}
	/*Setare struct sockaddr_in pentru a asculta pe portul respectiv */
	my_sockaddr.sin_port = htons(8000);
	my_sockaddr.sin_family = PF_INET;
	inet_aton("127.0.0.1", &(my_sockaddr.sin_addr));
	
	/* Legare proprietati de socket */
	int rs = bind(socketid, &my_sockaddr, sizeof(struct sockaddr_in));
	if (rs == -1) {
		perror("BIND ERROR");
		exit(-1);
	}

	// char buffer[100];
	// int byte_read = recvfrom(socketid, buffer, 13, MSG_WAITALL, &my_sockaddr, sizeof(struct sockaddr_in));
	// printf("%s\n", buffer);
	
	/* Deschidere fisier pentru scriere */
	DIE((fd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644))==-1,"open file");
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din socket
	*		pune in fisier
	*/
	int citire, length;
	while(citire = recvfrom(socketid, buf, BUFLEN, MSG_WAITALL, &from_station, &length)) {
		if (citire < 0) {
			perror("EROARE LA CITIRE aici");
			exit(-1);
		}
		int scriere = write(fd, buf, citire);
		if (scriere < 0) {
			perror("EROARE LA SCRIERE");
			exit(-1);
		}
	}


	/*Inchidere socket*/	
	close(socketid);
	
	/*Inchidere fisier*/
	close(fd);
	return 0;
}
