#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "hush.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *args[]) {
    signal(SIGINT, exit);
    signal(SIGQUIT, exit);
    signal(SIGTERM, exit);
    char *name = getenv("USER");
    char *host = getenv("HOST");
    char dirBuf[2048];
    char *directory = getcwd(dirBuf, 2048);
    bool isAFile = false;

    if (argc > 1) {
        if (dup2(open(args[1], O_RDONLY), STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        isAFile = true;
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

        if (!isAFile) {// I'm sorry
            printf("\033[1m\033[34m%s\033[0m@\033[35m\033[1m%s \033[32m%s\n\033[0m& ", name, host, directory);
            fflush(stdout);
        }
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
        if (argv[0][0] == '#') {
            continue;
        }

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
                char *curDir = directory;
                checkForEnvironmentVariables((const char **)argv);
                int standardOut = fileWrite((const char **)argv);
                runCommand((const char **)argv);
                chdir(curDir);

                if (dup2(standardOut, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
                
            }
        }
    }

    return 0;
}