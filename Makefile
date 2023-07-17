CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra
LDFLAGS=-pthread

.PHONY: all
all: encoder

encoder: encoder.c mytaskqueue.o myresultqueue.o

mytaskqueue.o: mytaskqueue.c mytaskqueue.h

myresultqueue.o: mytaskqueue.c mytaskqueue.h

.PHONY: clean
clean:
	rm -f *.o encoder
