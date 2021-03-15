#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int file_size(int fd) {
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return size;
}

void check_ACK(msg *t) {
	if (recv_message(t) < 0) {
		perror("[SENDER] Receive error. Exiting.\n");
		exit(-1);
	}
}

int calculate_check_sum (msg t) {
	int i, check_sum = t.payload[0];
	for (i = 1; i < t.len; i++) {
		check_sum = check_sum ^ t.payload[i];
	}
	return check_sum;
}

int main(int argc, char *argv[])
{
	msg t;
	int i, check_read, res;
	int BDP = atoi(argv[1]);
	int fd = open("test_file.txt", O_RDONLY);
	if (fd < 0) {
		perror("[SENDER] Can't open file.\n");
		return -1;
	}
	char name[] = "test_file.txt";
	char buf[MSGSIZE];
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);

	// Send file name
	memset(&t, 0, sizeof(msg));
	sprintf(t.payload, "%s", name);
	t.len = strlen(t.payload) + 1;
	t.check_sum = calculate_check_sum(t);
	// printf("%d %s %d\n", t.check_sum, t.payload, t.len);
	send_message(&t);
	check_ACK(&t);

	// Send size of the file
	memset(&t, 0, sizeof(msg));
	sprintf(t.payload, "%d", file_size(fd));
	t.len = strlen(t.payload) + 1;
	t.check_sum = calculate_check_sum(t);
	send_message(&t);
	check_ACK(&t);

	printf("[SENDER]: BDP=%d\n", BDP);
	int count = file_size(fd) / MSGSIZE + 1;

	// Send count
	memset(&t, 0, sizeof(msg));
	sprintf(t.payload, "%d", count);
	t.len = strlen(t.payload) + 1;
	t.check_sum = calculate_check_sum(t);
	send_message(&t);
	check_ACK(&t);

	int WINDOW_SIZE = (BDP * 1000) / (MSGSIZE * 8);

	// printf("%d\n", WINDOW_SIZE);

	for (i = 0; i < WINDOW_SIZE; i++) {
		memset(&t, 0, sizeof(msg));
		memset(buf, 0, 1300);
		check_read = read(fd, buf, 1300);
		buf[check_read] = '\0';
		
		if (check_read < 0) {
			perror("[SENDER] Reading error.\n");
			return -1;
		}
		memcpy(t.payload, buf, check_read);
		t.payload[check_read] = '\0';
		t.len = check_read;
		t.check_sum = calculate_check_sum(t);
		printf("[SENDER] Send message (window %d): %s\n", i + 1, t.payload);
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Sending error.\n");
			return -1;
		}
	}

	for (i = WINDOW_SIZE; i < count; i++) {
		/* cleanup msg */
		check_ACK(&t);
		memset(&t, 0, sizeof(msg));
		memset(buf, 0, 1300);
		check_read = read(fd, buf, 1300);
		buf[check_read] = '\0';
		if (check_read < 0) {
			perror("[SENDER] Reading error.\n");
			return -1;
		}
		memcpy(t.payload, buf, check_read);
		t.payload[check_read] = '\0';
		t.len = check_read;
		t.check_sum = calculate_check_sum(t);
		printf("[SENDER] Send message (window %d): %s\n", i + 1, t.payload);
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Sending error.\n");
			return -1;
		}
	}

	for (i = count; i < count + WINDOW_SIZE; i++) {
		check_ACK(&t);
	}

	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
