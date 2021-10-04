#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define BUFFSIZE    1024


ssize_t pwrite_all(int out_fd, const void* buf, ssize_t bytes_r, off_t* out_offset) {
    ssize_t bytes_w = 0;
    while(bytes_w < bytes_r) {
        ssize_t res = pwrite(out_fd, buf, bytes_r, *out_offset);
        if(res == -1) {
            perror("Bad writing");
            return 1;
        }
        bytes_w += res;
        *out_offset += res;
    }
    return (ssize_t)bytes_w;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s path text\n", argv[0]);
        return 1;
    }
    struct stat sb;
    if(lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return 2;
    }
    if((sb.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr, "Not a regular file %s\n", argv[1]);
        return 3;
    }


    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd == -1) {
        perror("Cannot open file to read");
        return 4;
    }

    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // хотим и писать, и читать
    if (out_fd == -1) {
        close(in_fd);
        return 5;
    }
    off_t in_offset = 0, out_offset = 0;
    char buf[BUFFSIZE];
    ssize_t bytes_r;
    
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
        if(pwrite_all(out_fd, buf, bytes_r, &out_offset) != bytes_r) {
            return 6;
        }
    }
    if(close(in_fd)) {
        perror("Bad closing");
        return 7;
    }
    if(close(out_fd)) {
        perror("Bad closing");
        return 8;
    }
    return 0;
}