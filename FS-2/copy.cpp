#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
	if(argc != 3) {
        perror("Wrong args");
        exit(EXIT_FAILURE);
    }


	int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }


   int fd2 = open(argv[2], O_WRONLY | O_CREAT, 0664);
    if (fd2 == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
   
    const int size = 4096;
    char buffer[size];

    size_t bytes_read;
    while ((bytes_read = read(fd1, buffer, size)) > 0) {
        ssize_t bytes_written = write(fd2, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from source file");
    }

    close(fd1);
    close(fd2);
    return 0;
}