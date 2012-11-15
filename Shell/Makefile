TARG = Shell

CC = gcc

OBJS = main-pipe-redirect-working.o parser.o 

SRC = main-pipe-redirect-working.c  parser.c  

INC = main.h parser.h  

CFLAGS=-c -Wall

DEBUG = -g  

CHAR_SIZE = 8

$(TARG) : $(OBJS)
	$(CC) $(DEBUG) $(OBJS) -o $(TARG)

main.o : main-pipe-redirect-working.c $(INC)
	$(CC) $(DEBUG) $(CFLAGS) main-pipe-redirect-working.c

parser.o : parser.c $(INC)
	$(CC) $(DEBUG) $(CFLAGS) parser.c
 
clean : 
	rm -f *.o $(TARG) 
# DO NOT DELETE
