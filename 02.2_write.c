#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s paht text\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // user -- group -- other
    if(fd < 0) {
        perror("Failed to open file to writing");
        return 2;
    }

    if((size_t)dprintf(fd, "%s", argv[2]) != strlen(argv[2])) {
        perror("Failed to write");
        close(fd);
        return 3;
    }

    if(close(fd) < 0) {
        perror("Failure during close");
        return 4;
    }
    
    return 0;
}