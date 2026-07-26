# rshell

A small Unix shell written from scratch in C. Just `fork`, `execvp`, `pipe`,built to actually understand how a shell works instead of just using one.

## What it does

rshell reads a line, splits it into tokens, figures out if you're running a single command or a pipeline of them, and hands it off to the right place — a builtin function if it's one of the shell's own commands, or a forked child process otherwise. It supports the stuff you'd expect from a basic shell:

- **Piping**: `cmd1 | cmd2 | cmd3`
- **Redirection**: `<` for input, `>` / `>>` for output (truncate or append), `2>` for stderr
- **Command history**: powered by GNU readline, persisted across sessions to `~/.rshell_history`
- **Line editing**: arrow keys, `Ctrl-R` search, etc. — anything readline gives you for free

## Building it

You need `libreadline` installed (`libreadline-dev` on Debian/Ubuntu, `readline` on Arch/macOS via Homebrew).

```sh
make
./shell
```


## Using it

Once it's running you get a `>` prompt and it behaves like a (very) minimal bash:

```
> ls -l | grep .c | wc -l
> echo hello > out.txt
> cat < out.txt
> sort < names.txt >> sorted.txt 2> errors.log
```

### Builtins

| Command   | What it does                                  |
|-----------|------------------------------------------------|
| `cd DIR`  | Change directory                                |
| `history` | Print the command history                       |
| `leave`   | Exit the shell                                   |

(Yes, it's `leave` and not `exit`, that one's on purpose, not a typo.)

## Project layout

```
shell.c     entry point, readline loop, tokenizing
parser.c/h  turns tokens into a pipeline of commands + redirection targets
executor.c/h  forks/pipes/execs the pipeline and waits on it
builtins.c/h  cd, history, leave
```


