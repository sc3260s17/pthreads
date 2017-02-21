CC=gcc
CFLAGS=-lpthread -Wall

all : sum_lock sum_cond

sum_lock : sum_lock.c 
	$(CC) $(CFLAGS) -o sum_lock sum_lock.c -lpthread 

sum_cond : sum_cond.c barrier.c barrier.h
	$(CC) $(CFLAGS) -o sum_cond sum_cond.c barrier.c

clean :
	rm -f sum_lock sum_cond
