CC = gcc
CFLAGS = -Wall -Wextra -O0 -g3 -std=c99
IFLAGS = -Iinc/

.PHONY: clean re

TGT = scag

SRCS := $(shell find . -name *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

$(TGT): $(OBJS)


clean:


re: clean all
