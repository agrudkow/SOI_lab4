# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

CC = g++                       # compiler to use
OBJS = main.o monitor.o

lab4: $(OBJS)
				$(CC) -g -std=c++11 -pthread -o lab4 $(OBJS)

main.o: monitor.h
monitor.o: monitor.h


.PHONY = clean
clean:
				rm lab4 $(OBJS)
