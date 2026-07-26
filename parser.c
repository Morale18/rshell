#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 64

static command_t *parse_command(char **segment, int count) {
    command_t *cmd = malloc(sizeof(command_t));
    cmd->argv = malloc(sizeof(char *) * (count + 1));
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->append = 0;
    cmd->errfile = NULL;

    int argc = 0;
    for (int i = 0; i < count; i++) {
        char *tok = segment[i];
        char **target = NULL;

        if (strcmp(tok, "<") == 0) {
            target = &cmd->infile;
        } else if (strcmp(tok, ">>") == 0) {
            target = &cmd->outfile;
            cmd->append = 1;
        } else if (strcmp(tok, ">") == 0) {
            target = &cmd->outfile;
            cmd->append = 0;
        } else if (strcmp(tok, "2>") == 0) {
            target = &cmd->errfile;
        } else {
            cmd->argv[argc++] = tok;
            continue;
        }

        if (i + 1 >= count) {
            fprintf(stderr, "rshell: syntax error: expected filename after `%s'\n", tok);
            free(cmd->argv);
            free(cmd);
            return NULL;
        }
        *target = segment[++i];
    }
    cmd->argv[argc] = NULL;

    if (argc == 0) {
        fprintf(stderr, "rshell: syntax error near unexpected token `|'\n");
        free(cmd->argv);
        free(cmd);
        return NULL;
    }

    return cmd;
}

pipeline_t *parse_pipeline(char **tokens) {
    int num_commands = 1;
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            num_commands++;
        }
    }

    pipeline_t *pl = malloc(sizeof(pipeline_t));
    pl->commands = malloc(sizeof(command_t) * num_commands);
    pl->num_commands = 0;

    char *segment[MAX_ARGS];
    int seg_count = 0;

    for (int i = 0; ; i++) {
        int is_pipe = tokens[i] != NULL && strcmp(tokens[i], "|") == 0;

        if (tokens[i] == NULL || is_pipe) {
            command_t *cmd = parse_command(segment, seg_count);
            if (cmd == NULL) {
                free_pipeline(pl);
                return NULL;
            }
            pl->commands[pl->num_commands++] = *cmd;
            free(cmd);
            seg_count = 0;

            if (tokens[i] == NULL) {
                break;
            }
        } else {
            if (seg_count >= MAX_ARGS - 1) {
                fprintf(stderr, "rshell: too many arguments in a single command\n");
                free_pipeline(pl);
                return NULL;
            }
            segment[seg_count++] = tokens[i];
        }
    }

    return pl;
}

void free_pipeline(pipeline_t *pl) {
    if (!pl) {
        return;
    }
    for (int i = 0; i < pl->num_commands; i++) {
        free(pl->commands[i].argv);
    }
    free(pl->commands);
    free(pl);
}
