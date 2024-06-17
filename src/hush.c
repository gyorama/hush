#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
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

int fileWrite(const char *argv[]) {
    int writeFile = 0;
    for (int i = 1; argv[i] != NULL; i++) {
        if (strcmp(argv[i], ">") == 0) {
            argv[i] = NULL;
            writeFile = open(argv[i + 1], O_WRONLY| O_CREAT | O_TRUNC, 0644);
            int standardOut = dup(STDOUT_FILENO);
            if (dup2(writeFile, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            return standardOut;
        }
    }

    return 0;
}