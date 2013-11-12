CC=/usr/bin/gcc
CCFLAGS=-pthread -g -lpthread -Wall
LDFLAGS=-pthread

all: philo

main.o: main.c main.h
	$(CC) -c $(CCFLAGS) -o main.o main.c

monitor.o: monitor.c main.h
	$(CC) -c $(CCFLAGS) -o monitor.o monitor.c

philo: main.o monitor.o
	$(CC) $(LDFLAGS) -o philo main.o monitor.o

clean:
	-rm main.o monitor.o philo