#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

static int apply_redirection(command_t *cmd) {
    if (cmd->infile) {
        int fd = open(cmd->infile, O_RDONLY);
        if (fd < 0) {
            perror("rshell");
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->outfile) {
        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
        int fd = open(cmd->outfile, flags, 0644);
        if (fd < 0) {
            perror("rshell");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if (cmd->errfile) {
        int fd = open(cmd->errfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("rshell");
            return -1;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    return 0;
}

int execute_pipeline(pipeline_t *pl) {
    int num = pl->num_commands;
    int num_pipes = num - 1;
    int (*pipefds)[2] = NULL;

    if (num_pipes > 0) {
        pipefds = malloc(sizeof(int[2]) * num_pipes);
        for (int i = 0; i < num_pipes; i++) {
            if (pipe(pipefds[i]) < 0) {
                perror("rshell");
                free(pipefds);
                return 1;
            }
        }
    }

    pid_t *pids = malloc(sizeof(pid_t) * num);

    for (int i = 0; i < num; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            /* child */
            if (i > 0) {
                dup2(pipefds[i - 1][0], STDIN_FILENO);
            }
            if (i < num - 1) {
                dup2(pipefds[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < num_pipes; j++) {
                close(pipefds[j][0]);
                close(pipefds[j][1]);
            }

            if (apply_redirection(&pl->commands[i]) < 0) {
                exit(EXIT_FAILURE);
            }

            execvp(pl->commands[i].argv[0], pl->commands[i].argv);
            perror("rshell");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("rshell");
        }

        pids[i] = pid;
    }

    for (int j = 0; j < num_pipes; j++) {
        close(pipefds[j][0]);
        close(pipefds[j][1]);
    }

    int status;
    for (int i = 0; i < num; i++) {
        waitpid(pids[i], &status, 0);
    }

    free(pipefds);
    free(pids);
    return 1;
}
