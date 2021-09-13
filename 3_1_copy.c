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
    int source_fd = open(argv[1], O_RDONLY, 0644); // user -- group -- other
    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(source_fd < 0) {
        perror("Failed to open file to reading");
        return 2;
    }
    if(dest_fd < 0) {
        perror("Failed to open file to writing")
        return 3;
    }
    return 0;
}