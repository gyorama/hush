#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <signal.h>
#include "hush.h"
#include <string.h>
#include <stdlib.h>

int main() {

    char *name = getenv("USER");
    char *host = getenv("HOST");
    char *directory = getenv("PWD");

    if (name == NULL) {
        name = "?";
    } else if (host == NULL) {
        host = "localhost";
    } else if (directory == NULL) {
        directory = "?";
    }

    char buffer[2048];

    char *argv[128];

    while (1) {
        // I'm sorry
        printf("\033[1m\033[34m%s\033[0m@\033[35m\033[1m%s \033[32m%s\n\033[0m& ", name, host, directory);
        fflush(stdout);
        fgets(buffer, sizeof(buffer) - 1, stdin);

        char *token;
        int i = 0;
        token = strtok(buffer, " \t\n");
        while (token != NULL && i < 128 - 1) {
            argv[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[i] = NULL;

        if (argv[0] != NULL) {
            if (strcmp(argv[0], "exit") == 0) {
                return (argv[1]) ? atoi(argv[1]) : 0;
            } else if (strcmp(argv[0], "cd") == 0) {
                if (argv[1]) {
                    if (chdir(argv[1])) {
                        perror(argv[1]);
                    }                    
                    setenv("PWD", argv[1], 1);
                    directory = getenv("PWD");
                }
            } else {
                runCommand((const char **)argv);
            }
        }
    }

    return 0;
}