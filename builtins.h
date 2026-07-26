#ifndef BUILTINS_H
#define BUILTINS_H


int shell_cd(char **args);
int shell_exit(char **args);
int shell_history(char **args);


int num_builtins(void);
int is_builtin(char *cmd);
int run_builtin(char **args);


#endif 
