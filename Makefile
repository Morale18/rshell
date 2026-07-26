
CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lreadline

TARGET = shell

SRCS = shell.c builtins.c parser.c executor.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
