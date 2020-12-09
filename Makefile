CC=gcc
CFLAGS=-std=c99 -Wall -W -Wextra -pedantic -ggdb
RM=rm -f


all: main.o scanner.o str.o syntax_analysis.o  symtable.o stack.o precanal.o codegen.o
	$(CC) $(CFLAGS) scanner.o main.o str.o syntax_analysis.o  symtable.o stack.o precanal.o codegen.o -o ifj20

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o $@

scanner.o: scanner.c 
	$(CC) $(CFLAGS) -c scanner.c -o $@

str.o: str.c
	$(CC) $(CFLAGS) -c str.c -o $@

syntax_analysis.o: syntax_analysis.c
	$(CC) $(CFLAGS) -c syntax_analysis.c -o $@



symtable.o: symtable.c 
	$(CC) $(CFLAGS) -c symtable.c -o $@

stack.o: stack.c 
	$(CC) $(CFLAGS) -c stack.c -o $@

precanal.o: precanal.c 
	$(CC) $(CFLAGS) -c precanal.c -o $@


codegen.o: codegen.c 
	$(CC) $(CFLAGS) -c codegen.c -o $@



clean:
	$(RM) *.o
