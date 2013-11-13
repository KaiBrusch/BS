
CC := /usr/bin/gcc
CCFLAGS := -pthread -g -lpthread -Wall
LDFLAGS := -pthread

all: philosophen

# %.o: %.c main.h
#  $(CC) -c $(CCFLAGS) -o $@ $<

main.o: main.c
	$(CC) $(CCFLAGS) -c -o main.o main.c

monitor.o: monitor.c
	$(CC) $(CCFLAGS) -c -o monitor.o monitor.c

philosophen: main.o monitor.o
	$(CC) $(LDFLAGS) -o philosophen main.o monitor.o
#		$(CC) $(LDFLAGS) -o $@ $^
clean: 
	rm philosophen main.o monitor.o