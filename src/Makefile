CC = gcc
CFLAGS = -Wall -Wextra -O0 -g3 -std=c99
IFLAGS = -Iinc/

.PHONY: clean re

TGT = main

SRCS := $(shell find . -type f -name '*.c')
OBJS := $(SRCS:.c=.o)

all: $(TGT)

debug: all

debug: DEBUG=-DDEBUG

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG) $(IFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) $(IFLAGS) -c $^ -o $@

clean:
	@rm -f $(TGT) $(OBJS)
	@echo "clean"

re: clean $(TGT)
