/*
    Выводит UID, GID, PID, PPID,
    приоритет планирования (sheduling priority),
    идентификаторы дополнительных групп, 
    маску umask процесса
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>


#define NGROUPS 20
#define DEBUG


int main(void) {
    // UID
    uid_t real_user_id = getuid(); //These functions are always successful.
    uid_t effecrive_user_id = geteuid();
    printf("Real user ID: %u\nEffective user ID: %u\n", real_user_id, effecrive_user_id);

    // PID, PPID
    pid_t process_id = getpid();//These functions are always successful too.
    pid_t parent_pid = getppid();
    printf("Process ID: %d\nParent's process ID: %d\n", process_id, parent_pid);

    // GID
    const char* grp_name;
    struct group* grp_info = getgrgid(getgid());
    if(grp_info == NULL) {
        grp_name = "?";
    } else {
        grp_name = grp_info->gr_name;
    }
    printf("Group ID = %d, %s\n", getpgid(process_id), grp_name);

    // get user name
    struct passwd* pass_info;
    const char* user_name;
    pass_info = getpwuid(getuid());
    if(pass_info == NULL) {
        user_name = "?";
    } else {
        user_name = pass_info->pw_name;
    }

    #ifdef DEBUG
        printf("User name: %s\n", user_name);
    #endif

    // all groups
    int j, ngroups;
    gid_t *groups;
    struct passwd *pw;
    struct group *gr;
    ngroups = NGROUPS;
    groups = malloc(ngroups * sizeof (gid_t));
    if (groups == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    /* получаем структуру passwd (содержит ID первичной группы пользователя) */
    pw = getpwnam(user_name);
    if (pw == NULL) {
        perror("getpwnam");
        exit(EXIT_SUCCESS);
    }
    /* получаем список групп */
    if (getgrouplist(user_name, pw->pw_gid, groups, &ngroups) == -1) {
        fprintf(stderr, "getgrouplist() вернула -1; ngroups = %d\n",
                ngroups);
        exit(EXIT_FAILURE);
    }
    /* печатаем список полученных групп вместе с именами групп */
    fprintf(stderr, "ngroups = %d\n", ngroups);
    for (j = 0; j < ngroups; j++) {
        printf("Group (name): %d", groups[j]);
        gr = getgrgid(groups[j]);
        if (gr != NULL) {
            printf(" (%s)", gr->gr_name);
        }
        printf("\n");
    }

    // get umask
    mode_t u_mask = umask(ALLPERMS);
    printf("umask: %o\n", u_mask);
    u_mask = umask(u_mask); // хотим оставить всё как было

    // Sheduling priority
    int pr = getpriority(PRIO_PROCESS, 0);
    if(pr == -1) {
        perror("Failure getpriority");
        exit(EXIT_FAILURE);
    }
    printf("Sheduling priority: %d\n", pr);
    return 0;
}