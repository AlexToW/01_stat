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
#include <limits.h>


#define NGROUPS 20
#define DEBUG


int main(void) {
    /*
        1. Process ID (getpid(2))
        2. Parent process ID (getppid(2))
        3. Process group ID and session ID
        4. User and group identifiers.  A process can obtain its session ID using  getsid(2), 
        and its process group ID using getpgrp(2).
            4.1  Real  user ID and real group ID (getgid(2))
            4.2 Effective user ID and effective group ID (getegid(2))
    */

    // pid, ppid
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("PID: %d,\t\t PPID: %d\n", pid, ppid);
    // Process group ID and session ID
    pid_t session_id = getsid(pid);
    pid_t proc_group_id = getpgrp();
    printf("session ID: %d,\t process group ID: %d\n", session_id, proc_group_id);
    // real/effective user id
    uid_t real_user_id = getuid();
    uid_t effective_user_id = geteuid();
    printf("Real user ID: %d,\t effective user ID: %d\n", real_user_id, effective_user_id);
    // real/effective group id
    gid_t real_group_id = getgid();
    gid_t effective_group_id = getegid();
    printf("Real group ID: %d,\t effective group ID: %d\n", real_group_id, effective_group_id);

    // groups
    gid_t groups[NGROUPS_MAX + 1];
    int groups_size = NGROUPS_MAX; 
    int num_groups = getgroups(groups_size, groups);
    if (num_groups < 0) {
        perror("getgroups");
        return 2;
    }
    struct group* gr_info_p;
    printf("groups=");
    for (unsigned i = 0; i < (unsigned)num_groups; i++) {
        printf("%d", groups[i]);
        gr_info_p = getgrgid(groups[i]);
        if (gr_info_p == NULL) {
            printf("-(?)");
        }
        else {
            printf("(%s),", gr_info_p->gr_name);
        }
    }
    putchar('\n');
    // umask
    mode_t u_mask = umask(ALLPERMS);
    printf("umask: %o\n", u_mask);
    u_mask = umask(u_mask); // хотим оставить всё как было

    #if 0
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
    #endif
    return 0;
}