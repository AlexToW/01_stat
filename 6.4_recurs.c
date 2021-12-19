#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>


#define SIZEOF_PATH     1024*1024
#define INDENT          4
#define MAX_DEPTH       10


char dtype_letter(unsigned d_type) {
    switch(d_type) {
        case DT_BLK: return 'b';
        case DT_CHR: return 'c';
        case DT_DIR: return 'd';
        case DT_FIFO: return 'p';
        case DT_LNK: return 'l';
        case DT_REG: return '-';
        case DT_SOCK: return 's';
    }
    return '?';
}


char mode_letter(mode_t st_mode) {
    switch (st_mode & S_IFMT) {
        case S_IFBLK: return 'b'; 
        case S_IFCHR: return 'c';
        case S_IFDIR: return 'd'; 
        case S_IFIFO: return 'p'; 
        case S_IFLNK: return 'l'; 
        case S_IFREG: return '-'; 
        case S_IFSOCK: return 's';	
    }
    return '?';
}


int listdir(const char *name, int lvl) {
    #if 0
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char new_path[SIZEOF_PATH];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || lvl > MAX_DEPTH) {
                continue;
            }
            if(snprintf(new_path, SIZEOF_PATH, "%s/%s", name, entry->d_name) < 0) {
                continue;
            }
            for(int i = 0; i < lvl; i++) {
                putc(' ', stdout);
            }
            char type = '?';
            type = dtype_letter(entry->d_type);
            if(type == '?') {
                struct stat sb;
                if(lstat(entry->d_name, &sb) == 0) {
                    type = mode_letter(sb.st_mode);
                }
            }
            printf("%c %s\n", type, entry->d_name);
            listdir(new_path, lvl + INDENT);
        } else {
            printf("%*s- %s\n", lvl, "", entry->d_name);
        }
    }
    closedir(dir);
    #endif
    DIR *dir = opendir(name);
    struct dirent *entry;
    struct stat sb;
    char type;
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    int errno;
    while(1) {
        errno = 0;
        if((entry = readdir(dir)) != NULL) {
            break;
        }
        if(entry->d_type != DT_UNKNOWN) {
            type = dtype_letter(entry->d_type);
        } else {
            if(fstatat(dirfd(dir), entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
                perror("fstatat");
            }
            type = mode_letter(sb.st_mode & S_IFMT);
        }

        printf("%-20c", type);
        for(int i = 10 * lvl; i > 0; i--) {
            printf(" ");
        }
        printf("%s\n", entry->d_name);
        if(type != 'd' && strcmp(entry->d_name, ".") != 0 &&  strcmp(entry->d_name, "..") != 0) {
            char* new_dir_name;
            asprintf(&new_dir_name, "%s/%s", name, entry->d_name);
            listdir(new_dir_name, lvl+1);
        }
    }
    int res = 0;
    if(errno) {
        perror("Something went wrong");
        res = 2;
    }

    if(closedir(dir) == -1) {
        perror("closedir");
        res = 3;
    }
    return res;
}


int main(int argc, char* argv[]) {
    if(argc == 2) {
        int res = listdir(argv[1], 0);
        if(res != 0) {
            perror("listdir");
            return res;
        }
    }
    return 0;
}