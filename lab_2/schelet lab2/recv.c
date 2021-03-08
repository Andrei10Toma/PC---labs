#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

void check_receive(msg *r) {
	if (recv_message(r) < 0) {
		perror("Receive message");
		exit(-1);
	}
	printf("[recv] Got msg with payload: <%s>, sending ACK...\n", r->payload);	
}

void send_reply_to_sender(msg *r) {
	sprintf(r->payload, "%s", "ACK");
	r->len = strlen(r->payload) + 1;
	send_message(r);
	printf("[recv] ACK sent\n");
}

int main(int argc, char **argv) {
	msg r;
	init(HOST, PORT);

	// Receive number of files
	check_receive(&r);
	int number_files = atoi(r.payload);
	send_reply_to_sender(&r);
	for (int i = 0; i < number_files; i++) {
		// Receive filename
		check_receive(&r);
		char filename[100] = "receiver_";
		strcat(filename, r.payload);
		int frd = open(filename, O_CREAT | O_WRONLY, 0666);
		send_reply_to_sender(&r);

		// Receive size of the file
		check_receive(&r);
		int file_size = atoi(r.payload);
		send_reply_to_sender(&r);

		// Receive content of the file
		while (file_size > 0) {
			check_receive(&r);
			int check_write = write(frd, r.payload, r.len);
			if (check_write < 0) {
				perror("Writing error");
				return -1;
			}
			file_size -= check_write;
			send_reply_to_sender(&r);
		}
		close(frd);
	}

	return 0;
}
