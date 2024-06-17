#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "hush.h"
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void runCommand(const char *argv[]) {
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGINT, exit);
        execvp(argv[0], (char * const *)argv);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else if (pid == -1) {
        perror(argv[0]);
        exit(1);
    }
}

void quit(const char *argv[]) {
    exit(atoi(argv[1]));
}