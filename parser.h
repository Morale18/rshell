#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char **argv;
    char *infile;    /* < */
    char *outfile;   /* > or >> */
    int append;      /* 1 for >>, 0 for > */
    char *errfile;   /* 2> */
} command_t;

typedef struct {
    command_t *commands;
    int num_commands;
} pipeline_t;

pipeline_t *parse_pipeline(char **tokens);
void free_pipeline(pipeline_t *pl);

#endif
