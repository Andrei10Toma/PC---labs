// Protocoale de comunicatii
// Laborator 9 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;
	// struct sockaddr_in adr;
	char *buf = (char *)calloc(1, 30);

	// TODO: set hints
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	// TODO: get addresses
	ret = getaddrinfo(name, NULL, &hints, &result);
	if (ret < 0) {
		freeaddrinfo(result);
		printf("%s\n", gai_strerror(ret));
		return;
	}

	// TODO: iterate through addresses and print them
	for (p = result; p != NULL; p = p->ai_next) {
		void *ptr;
		memset(buf, 0, 30);
		if(p->ai_family == AF_INET) {
			ptr = &((struct sockaddr_in *) p->ai_addr)->sin_addr;
			inet_ntop(AF_INET, ptr, buf, 30);
		}
        else {
			ptr = &((struct sockaddr_in6 *) p->ai_addr)->sin6_addr;
			inet_ntop(AF_INET6, ptr, buf, 30);
		}
		printf("%s\n", buf);
	}
	free(buf);
	freeaddrinfo(result);
	// TODO: free allocated data
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	char host[1024];
	char service[20];

	// TODO: fill in address data
	addr.sin_family = AF_INET;
	inet_aton(ip, &addr.sin_addr);

	// TODO: get name and service
	ret = getnameinfo((struct sockaddr*)&addr, sizeof(struct sockaddr_in) , host, sizeof(host), service, sizeof(service), 0);
	if (ret < 0) {
		printf("%s\n", gai_strerror(ret));
		return;
	}

	// TODO: print name and service
	printf("host = %s, serv = %s\n", host, service);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
