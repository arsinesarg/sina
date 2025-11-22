#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Input error\n");
        exit(EXIT_FAILURE);
    }

    const char *path = argv[1];

    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Error getting file info");
        exit(EXIT_FAILURE);
    }

    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Error while opening file");
        exit(EXIT_FAILURE);
    }

    off_t size = st.st_size;

    const size_t BUFSZ = 4096;
    char buf[BUFSZ];
    for (size_t i = 0; i < BUFSZ; ++i) buf[i] = '\0';

    off_t remaining = size;
    while (remaining > 0) {
        size_t chunk = remaining > (off_t)BUFSZ ? BUFSZ : (size_t)remaining;

        ssize_t w = write(fd, buf, chunk);
        if (w == -1) {
            perror("Error writing zeros");
            close(fd);
            exit(EXIT_FAILURE);
        }

        remaining -= w;
    }

    if (close(fd) == -1) {
        perror("Warning: close failed");
    }

    if (unlink(path) == -1) {
        perror("Error deleting file");
        exit(EXIT_FAILURE);
    }

    return 0;

