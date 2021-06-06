/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
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
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];
	int sockid;


	/*Deschidere socket*/
	sockid = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockid == -1) {
		perror("Eroare la deschiderea socketului");
		exit(-1);
	}
	
	
	/* Deschidere fisier pentru citire */
	DIE((fd=open(argv[3],O_RDONLY))==-1,"open file");
	
	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
	to_station.sin_family = PF_INET;
	inet_aton("127.0.0.1", &(to_station.sin_addr));
	to_station.sin_port = htons(8000);

	// char buffer[] = "Hello World!";
	// sendto(sockid, buffer, strlen(buffer), MSG_CONFIRM, &to_station, sizeof(struct sockaddr_in));

	
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	
	int citire;
	while(citire = read(fd, buf, BUFLEN)) {
		if (citire < 0) {
			perror("EROARE LA CITIRE ");
			exit(-1);
		}
		sendto(sockid, buf, citire, MSG_CONFIRM, &to_station, sizeof(struct sockaddr_in));
	}
	
	/*Inchidere socket*/
	close(sockid);
	
	/*Inchidere fisier*/
	close(fd);

	return 0;
}
