#if 0
/* Supported events suitable for MASK parameter of INOTIFY_ADD_WATCH.  */
#define IN_ACCESS	 0x00000001	/* File was accessed.  */
#define IN_MODIFY	 0x00000002	/* File was modified.  */
#define IN_ATTRIB	 0x00000004	/* Metadata changed.  */
#define IN_CLOSE_WRITE	 0x00000008	/* Writtable file was closed.  */
#define IN_CLOSE_NOWRITE 0x00000010	/* Unwrittable file closed.  */
#define IN_CLOSE	 (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
#define IN_OPEN		 0x00000020	/* File was opened.  */
#define IN_MOVED_FROM	 0x00000040	/* File was moved from X.  */
#define IN_MOVED_TO      0x00000080	/* File was moved to Y.  */
#define IN_MOVE		 (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
#define IN_CREATE	 0x00000100	/* Subfile was created.  */
#define IN_DELETE	 0x00000200	/* Subfile was deleted.  */
#define IN_DELETE_SELF	 0x00000400	/* Self was deleted.  */
#define IN_MOVE_SELF	 0x00000800	/* Self was moved.  */
#endif

/*
    Появление новых файлов в указанном каталоге
*/
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>


#define SIZEOF_BUF  4096
#define DEBUG


void handle_event(int fd, int wd, char* pathname) {
    char buf[SIZEOF_BUF];
    const struct inotify_event *event;
    ssize_t len;
    char *ptr;
    for(;;) {
        len = read(fd, buf, sizeof(buf));
        if(len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if(len <= 0) {
            break;
        }

        for(ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event*)buf;
            if(event->cookie != 0 && (event->mask & IN_MOVE)) {
                // IN_MOVED_FROM & IN_MOVED_TO
                if(event->len && wd == event->wd)
                    printf("IN_MOVE: %s/%s", pathname, event->name);
            } else if(event->mask & IN_CREATE) {
                if(event->len && wd == event->wd)
                    printf("IN_CREATE: %s/%s", pathname, event->name);
            }
            if(event->mask & IN_ISDIR) {
                printf("\t(directory)\n");
            } else {
                printf("\t(file)\n");
            }
        }
    }
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("argc");
        exit(EXIT_FAILURE);
    }

    char buf;
    int fd, poll_num, wd;
    nfds_t nfds;
    struct pollfd fds[2];

    // создаем экземпляр inotify
    fd = inotify_init1(IN_NONBLOCK);
    if(fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    // наблюдаем за argv[1] на предмет создания новых файлов(IN_CREATE)
    wd = inotify_add_watch(fd, argv[1], IN_MOVE | IN_CREATE);
    if(wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    nfds = 2;
    /* ввод с консоли  */
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    /* ввод inotify */
    fds[1].fd = fd;
    fds[1].events = POLLIN;
    /* ждём события и/или ввода с терминала */
    printf("Ожидание событий.\n");

    while(1) {
        poll_num = poll(fds, nfds, -1);
        if(poll_num == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            exit(EXIT_FAILURE);
        }
        if(poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                /* доступен ввод с консоли: опустошаем stdin и выходим */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }
            if (fds[1].revents & POLLIN) {
                /* доступны события inotify */
                handle_event(fd, wd, argv[1]);
            }
        }
    }

    printf("Ожидание событий прекращено.\n");
    /* закрываем файловый дескриптор inotify */
    close(fd);
    return 0;
}