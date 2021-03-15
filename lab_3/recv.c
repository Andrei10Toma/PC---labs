#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int calculate_check_sum(msg *r) {
	int i, check_sum = r->payload[0];
	for (i = 1; i < r->len; i++) {
		check_sum = check_sum ^ r->payload[i];
	}
	return check_sum;
}

void check_control_sum(msg *r) {
	int check_sum = calculate_check_sum(r);
	if (check_sum != r->check_sum) {
		perror("Corrupted.\n");
		exit(-1);
	}
}

int main(void)
{
	msg r;
	int i, res;
	char filename[20] = "receiver_";
	
	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	// Receive file name
	memset(&r, 0, sizeof(msg));
	res = recv_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Receive error. Exitting.\n");
		return -1;
	}
	check_control_sum(&r);
	printf("[RECEIVER] Message received: %s.\n", r.payload);
	strcat(filename, r.payload);
	int fd_recv = open(filename, O_WRONLY | O_CREAT | O_RDONLY, 0664);
	res = send_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Send ACK error. Exiting.\n");
		return -1;
	}

	// Receive file size
	memset(&r, 0, sizeof(msg));
	res = recv_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Receive error. Exiting.\n");
		return -1;
	}
	check_control_sum(&r);
	printf("[RECEIVE] Message received: %s.\n", r.payload);
	res = send_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Send ACK error. Exiting.\n");
		return -1;
	}

	// Receive count
	memset(&r, 0, sizeof(msg));
	res = recv_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Receive error. Exiting.\n");
		return -1;
	}
	check_control_sum(&r);
	printf("[RECEIVE] Message received: %s.\n", r.payload);
	int count = atoi(r.payload);
	res = send_message(&r);
	if (res < 0) {
		perror("[RECEIVER] Send ACK error. Exiting.\n");
		return -1;
	}

	for (i = 0; i < count; i++) {
		/* wait for message */
		memset(&r, 0, sizeof(msg));
		res = recv_message(&r);
		if (res < 0) {
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		}
		check_control_sum(&r);
		// receive message
		printf("[RECEIVER] Message received: %s\n", r.payload);
		write(fd_recv, r.payload, r.len);
		/* send dummy ACK */
		res = send_message(&r);
		if (res < 0) {
			perror("[RECEIVER] Send ACK error. Exiting.\n");
			return -1;
		}
	}

	printf("[RECEIVER] Finished receiving..\n");
	return 0;
}
