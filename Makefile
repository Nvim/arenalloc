CC=gcc
# SRCS=$(wildcard src/*.c)
SRCS=src/main.c
CFLAGS=-std=c11 -pedantic -Wall -Wextra -Wvla -fvisibility=hidden -fPIC
EXTRA_FLAGS=-Werror
DEBUG_FLAGS=-ggdb

all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o main

debug: $(SRCS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SRCS) -o main

safe: $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) $(SRCS) -o main


