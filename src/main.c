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
#include <sys/stat.h>

int main() {
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, exit);
    signal(SIGTERM, exit);

    setenv("SHELL", "hush", 1);
    char *name = getenv("USER");
    char *host = getenv("HOST");
    char dirBuf[2048];
    char *directory = getcwd(dirBuf, 2048);

    if (name == NULL) {
        name = "?";
    } else if (host == NULL) {
        host = "localhost";
    } else if (directory == NULL) {
        directory = "?";
    }

    char buffer[2048];

    char *argv[128];

    printf("\033[0;32mNote: please don't press CTRL+D\n \033[0m\n");

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
                    setenv("PWD", getcwd(dirBuf, 2048), 1);
                    directory = getcwd(dirBuf, 2048);;
                }
            } else {
                int standardOut = fileWrite((const char **)argv);
                runCommand((const char **)argv);

                if (dup2(standardOut, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }
        }
    }

    return 0;
}