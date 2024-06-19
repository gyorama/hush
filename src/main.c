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

int main(int argc, char *argv[]) {
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, exit);
    signal(SIGTERM, exit);

    setenv("SHELL", "hush", 1);
    char *name = getenv("USER");
    char *host = getenv("HOST");
    char dirBuf[2048];
    char *directory = getcwd(dirBuf, 2048);

    if (argc > 1) {
        if (dup2(open(argv[1], O_RDONLY), STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
    }

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
        int inputStatus = fgets(buffer, sizeof(buffer) - 1, stdin) != NULL;

        if (!inputStatus) {
            exit(0);
        }

        char *token;
        int i = 0;
        token = strtok(buffer, " \t\n");
        while (token != NULL && i < 128 - 1) {
            argv[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[i] = NULL;

        if (argv[0] != NULL) {
            translateSymbols((const char **)argv);
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
                checkForEnvironmentVariables((const char **)argv);
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