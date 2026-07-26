#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>




char *builtins_names[] = {
    "cd",
    "leave",
    "history"
};

int (*builtins_func[]) (char **) = {
    &shell_cd,
    &shell_exit,
    &shell_history
};

int num_builtins(void) {
    return sizeof(builtins_names) / sizeof(char *);
}


int is_builtin(char *cmd) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(cmd, builtins_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int run_builtin(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtins_names[i]) == 0) {
            return (*builtins_func[i])(args);
        }
    }
    return -1; // could maybe possibly delete
}

// Builtin Commands

int shell_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "rshell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("rshell");
        }
    }

    return 1;   // continue running the shell
}

int shell_exit(char **args)
{
    (void)args; // suppress unused parameter warning
    return 0;   // tell shell.c to exit
}

int shell_history(char **args)
{
    (void)args;

    HIST_ENTRY **entries = history_list();
    if (entries) {
        for (int i = 0; entries[i] != NULL; i++) {
            printf("%d  %s\n", i + 1, entries[i]->line);
        }
    }

    return 1;
}
