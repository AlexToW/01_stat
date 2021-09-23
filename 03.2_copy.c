#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define BUFFSIZE        1024


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s path text\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct stat sb;
    if(lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }
    if((sb.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr, "Not a regular file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }


    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd == -1) {
        perror("Cannot open file to read");
        return errno;
    }

    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // хотим и писать, и читать
    if (out_fd == -1) {
        int err = errno;
        close(in_fd);
        return err;
    }
    off_t in_offset = 0, out_offset = 0;
    char buf[BUFFSIZE];
    ssize_t bytes_r, bytes_w;
    while(1) {
        bytes_r = pread(in_fd, buf, sizeof(buf), in_offset);
        if(bytes_r == 0) {
            break;
        }
        if(bytes_r == -1) {
            return errno;
        }
        in_offset += bytes_r;

        // непосредственно запись
        while(bytes_r) {
            bytes_w = pwrite(out_fd, buf, bytes_r, out_offset);
            if(bytes_w == -1) {
                perror("Bad writing");
                return 2;
            }
            bytes_r -= bytes_w;
            out_offset += bytes_w;
        }
    }
    if(close(in_fd)) {
        perror("Bad closing");
        return 3;
    }
    if(close(out_fd)) {
        perror("Bad closing");
        return 4;
    }
    return 0;
}