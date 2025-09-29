#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if(argc != 2) {
        perror("Wrong args");
        exit(EXIT_FAILURE);
    }

	int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

	int size = 4096;
    char bf[size];

    size_t byt_read = read(fd, bf, size);
	while (byt_read  > 0) {
        write(1, bf, byt_read);
        byt_read = read(fd, bf, size);
    }
    
    close(fd);  
    return 0;
	
}
