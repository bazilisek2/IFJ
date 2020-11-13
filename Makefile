CC=gcc
CFLAGS=-std=c99 -Wall -W -Wextra -pedantic -ggdb
RM=rm -f


all: main.o scanner.o str.o
	$(CC) $(CFLAGS) scanner.o main.o str.o -o prg

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o $@

scanner.o: scanner.c 
	$(CC) $(CFLAGS) -c scanner.c -o $@

str.o: str.c
	$(CC) $(CFLAGS) -c str.c -o $@

clean:
	$(RM) *.o