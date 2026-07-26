#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "builtins.h"
#include "parser.h"
#include "executor.h"
#define TOKEN_BUFFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

static char history_path[1024];

static void shell_history_init(void) {
    const char *home = getenv("HOME");
    if (home) {
        snprintf(history_path, sizeof(history_path), "%s/.rshell_history", home);
        read_history(history_path);
    } else {
        history_path[0] = '\0';
    }
}

static void shell_history_save(void) {
    if (history_path[0] != '\0') {
        write_history(history_path);
    }
}

char **shell_split_line(char *line){
    int bufsize = TOKEN_BUFFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position>=bufsize){
            bufsize += TOKEN_BUFFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void shell_loop(void){
    char *line;
    char **tokens;
    pipeline_t *pipeline;
    int status = 1;

    shell_history_init();

    while (status) {
        line = readline("> ");
        if (line == NULL) {
            /* EOF (Ctrl-D) */
            printf("\n");
            break;
        }

        if (line[0] != '\0') {
            add_history(line);
        }

        tokens = shell_split_line(line);

        if (tokens[0] == NULL) {
            free(tokens);
            free(line);
            continue;
        }

        pipeline = parse_pipeline(tokens);

        if (pipeline == NULL) {
            status = 1; /* syntax error already reported */
        } else if (pipeline->num_commands == 1 &&
                   pipeline->commands[0].infile == NULL &&
                   pipeline->commands[0].outfile == NULL &&
                   pipeline->commands[0].errfile == NULL &&
                   is_builtin(pipeline->commands[0].argv[0])) {
            status = run_builtin(pipeline->commands[0].argv);
        } else {
            status = execute_pipeline(pipeline);
        }

        free_pipeline(pipeline);
        free(tokens);
        free(line);
    }

    shell_history_save();
}

int main(void){
    shell_loop();
    return 0;
}


