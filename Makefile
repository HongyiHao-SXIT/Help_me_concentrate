CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS =
SRC = src/main.c src/focus/classify.c src/focus/history.c src/proc_monitor/process_read.c
OBJ = $(SRC:.c=.o)

all: focus

focus: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) focus

.PHONY: all clean
