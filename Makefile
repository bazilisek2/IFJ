CC=gcc
CFLAGS=-std=c99 -Wall -W -Wextra -pedantic -ggdb
RM=rm -f


all: scanner.o str.o
	$(CC) $(CFLAGS) scanner.o str.o -o prg



scanner.o: scanner.c 
	$(CC) $(CFLAGS) -c scanner.c -o $@

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c -o $@