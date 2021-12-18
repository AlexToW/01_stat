#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>


#define SIZEOF_PATH     1024 
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
    DIR *dir = opendir(name);
    struct dirent *entry;

    if (!dir) {
        return 1;
    }

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
    return 0;
}


int main(int argc, char* argv[]) {
    if(argc == 2) {
        if(listdir(argv[1], 0) != 0) {
            perror("listdir");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}