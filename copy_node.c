#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//int mknod(const char *pathname, mode_t mode, dev_t dev);
//-- копирование блочное/символьное устройство (S_IFBLK, S_IFCHR) (mknod) 


#define DEFAULT_CHUNK   262144


ssize_t write_all(int fd, const void *buf, size_t count) { // signed size_t
    /* будет вызывать write до тех пор, пока не возникнет ошибка, либо пока не запишем всё) */
    size_t bytes_written = 0;
    while(bytes_written < count) {
        ssize_t res = write(fd, buf + bytes_written, count - bytes_written);
        if(res < 0) {
            return res;
        }
        bytes_written += res;
    }
    return (ssize_t)bytes_written;
}


int copy_file(int src_fd, int dest_fd) {
    ssize_t bytes = 0;
    int err;
    char* data = (char*)malloc(DEFAULT_CHUNK);
    if(!data) {
        close(src_fd);
        close(dest_fd);
        return ENOMEM;
    }
    while(1) {
        bytes = read(src_fd, data, sizeof(data));
        if(bytes == -1) {
            err = errno;
            close(src_fd);
            close(dest_fd);
            return err;
        }
        if(bytes == 0) {
            break;
        }
        bytes = write_all(dest_fd, data, bytes);
        if(bytes < 0) {
            err = EIO;
            if(bytes == -1) {
                err = errno;
            }
            free(data);
            close(src_fd);
            close(dest_fd);
            return err;
        }
    }

    free(data);

    err = 0;
    if (close(src_fd)) {
        err = EIO;
    }
    if (close(dest_fd)) {
        err = EIO;
    }
    return err;
}




int main(int argc, char* argv[]) {
	if(argc != 3) {
		return 1;
	}
	struct stat sb;
	if(lstat(argv[1], &sb) == -1) {
		return 2;
	}
	mode_t type = sb.st_mode & S_IFMT;
	if(type == S_IFBLK || type == S_IFCHR) {
		if(lstat(argv[2], &sb) == -1 && strlen(argv[2]) != 0) {
			// то есть файл argv[2] не существует, будем создавать
			if(type == S_IFBLK) {
				if(mknod(argv[2], S_IFBLK | 0666, 0) == -1) {
					return 3;
				}
			} else {
				if(mknod(argv[2], S_IFCHR | 0666, 0) == -1) {
					return 4;
				}
			}
		}
	}
	int src_fd = open(argv[1], O_RDONLY);
	if(src_fd == -1) {
		return 5;
	}
	int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
	if(dst_fd == -1) {
		close(src_fd);
		return 6;
	}
	if(copy_file(src_fd, dst_fd) != 0) {
		close(src_fd);
		close(dst_fd);
		return 7;
	}
	return 0;
}
