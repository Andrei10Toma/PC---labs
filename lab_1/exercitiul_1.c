#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

void fatal(char *mesaj_eroare) {
    perror(mesaj_eroare);
    exit(-1);
}

int main(int argc, char *argv[])
{
    char *filenmae = argv[1];
    int citire, scriere;
    char buf[1024];
    int read_file = open(filenmae, O_RDONLY);
    if (read_file < 0) {
        fatal("Nu pot deschide fisierul");
    }

    lseek(read_file, 0, SEEK_SET);
    while ((citire = read(read_file, buf, sizeof(buf)))) {
        if (citire < 0) {
            fatal("Eroare la citire");
        }
        scriere = write(STDOUT_FILENO, buf, citire);
        if (scriere < 0) {
            fatal("Eroare la scriere");
        }
    }
    close(read_file);
    return 0;
}
