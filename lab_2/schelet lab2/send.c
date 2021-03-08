#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

void check_ACK(msg *t) {
	if (recv_message(t) < 0) {
		perror("Receive error ... ");
		exit(-1);
	}
	else {
		printf("[send] Got reply with payload: %s\n", t->payload);
	}
}

int main(int argc,char** argv) {
  init(HOST,PORT);
  msg t;

  int number_files = argc - 1;
  sprintf(t.payload, "%d", number_files);
  t.len = strlen(t.payload) + 1;
  send_message(&t);
  check_ACK(&t);

  /* Send dummy message:
  printf("[send] Sending dummy...\n");
  sprintf(t.payload,"%s", "This is a dummy.");
  t.len = strlen(t.payload) + 1;
  send_message(&t); */
  for (int i = 0; i < number_files; i++) {
	char buf[MAX_LEN];
	int fd = open(argv[i + 1], O_RDONLY);
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	// Send name of the file
	sprintf(t.payload, "%s", argv[i + 1]);
	t.len = strlen(t.payload) + 1;
	send_message(&t);
	check_ACK(&t);

	// Send size of the file
	sprintf(t.payload, "%d", size);
	t.len = strlen(t.payload) + 1;
	send_message(&t);
	check_ACK(&t);

	// Send content of the file
	int check_read;
	memset(buf, 0, MAX_LEN);
	while ((check_read = read(fd, buf, MAX_LEN))) {
		if (check_read < 0) {
			perror("Reading error");
			return -1;
		}
		memset(t.payload, 0, check_read);
		memcpy(t.payload, buf, check_read);
		memset(buf, 0, MAX_LEN);
		t.len = check_read;
		send_message(&t);
		check_ACK(&t);
	}
	close(fd);
  }
  return 0;
}
