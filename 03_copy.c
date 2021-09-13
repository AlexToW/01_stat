#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


#define  DEFAULT_CHUNK  262144  


int copy_file(const char *target, const char *source, const size_t chunk)
{
    const size_t size = (chunk > 0) ? chunk : DEFAULT_CHUNK;
    char *data, *ptr, *end;
    ssize_t bytes;
    int in_fd, out_fd, err;

    if (!target || !*target || !source || !*source) {
        return EINVAL;
    }

    in_fd = open(source, O_RDONLY);
    if (in_fd == -1) {
        return errno;
    }

    out_fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        err = errno;
        close(in_fd);
        return err;
    }

    data = malloc(size);
    if (!data) {
        close(in_fd);
        close(out_fd);
        unlink(target); // если создали новый target, а он не пригодился
        return ENOMEM;
    }

    while (1) {
        bytes = read(in_fd, data, size);
        if (bytes < 0) {
            if (bytes == -1) {
                err = errno;
            } else {
                err = EIO;
            }
            free(data);
            close(in_fd);
            close(out_fd);
            unlink(target);
            return err;
        } else if (bytes == 0) {
            break;
        }

        ptr = data;
        end = data + bytes;
        while (ptr < end) {
            bytes = write(out_fd, ptr, (size_t)(end - ptr));
            if (bytes <= 0) {
                if (bytes == -1) {
                    err = errno;
                } else {
                    err = EIO;
                }
                free(data);
                close(in_fd);
                close(out_fd);
                unlink(target);
                return err;
            } else {
                ptr += bytes;
            }
        }
    }

    free(data);

    err = 0;
    if (close(in_fd)) {
        err = EIO;
    }
    if (close(out_fd)) {
        err = EIO;
    }
    if (err) {
        unlink(target);
        return err;
    }
    return err;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s paht text\n", argv[0]);
        return EXIT_FAILURE;
    }
    if(copy_file(argv[1], argv[2], 0) != 0) {
        fprintf(stderr, "Failed to copy to %s from %s", argv[1], argv[2]);
        return 5;
    }
    return 0;
}