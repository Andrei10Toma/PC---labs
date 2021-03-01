#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#define MAX_LENGTH 100

void fatal(char *mesaj_eroare) {
    perror(mesaj_eroare);
    exit(-1);
}

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    int citire, scriere;
    char buf;
    char buf_invers[MAX_LENGTH][MAX_LENGTH];
    int contor1 = 0, contor2 = 0;
    int read_file = open(filename, O_RDONLY);
    if (read_file < 0) {
        fatal("Nu pot deschide fisierul.");
    }

    lseek(read_file, 0, SEEK_SET);
    while ((citire = read(read_file, &buf, 1))) {
        if (citire < 0) {
            fatal("Eroare la citire");
        }
        buf_invers[contor1][contor2] = buf;
        if (buf == '\n') {
            contor1++;
            contor2 = 0;
        }
        else {
            contor2++;
        }
    }
    close(read_file);
    for (int i = contor1; i >=0; i--) {
        scriere = write(STDOUT_FILENO, buf_invers[i], MAX_LENGTH);
        if (scriere < 0) {
            fatal("Eroare la scriere");
        }
    }
    return 0;
}
