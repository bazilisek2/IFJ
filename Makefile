CC=gcc
CFLAGS=-std=c99 -Wall -W -Wextra -pedantic -ggdb
RM=rm -f


all: main.o scanner.o str.o syntax_analysis.o global.o symtable.o
	$(CC) $(CFLAGS) scanner.o main.o str.o syntax_analysis.o global.o symtable.o -o prg

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o $@

scanner.o: scanner.c 
	$(CC) $(CFLAGS) -c scanner.c -o $@

str.o: str.c
	$(CC) $(CFLAGS) -c str.c -o $@

syntax_analysis.o: syntax_analysis.c
	$(CC) $(CFLAGS) -c syntax_analysis.c -o $@

global.o: global.c 
	$(CC) $(CFLAGS) -c global.c -o $@

symtable.o: symtable.c 
	$(CC) $(CFLAGS) -c symtable.c -o $@

clean:
	$(RM) *.o