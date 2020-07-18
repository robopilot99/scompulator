CC=g++
CXXFLAGS=-g -Wall

.PHONY: all

all : scompulator.o main.o io.o
	$(CC) $(CXXFLAGS) -o Scompulator scompulator.o main.o io.o

clean : 
	rm *.o
	rm Scompulator

scompulator.o : scompulator.cpp scompulator.h names.h
	$(CC) $(CXXFLAGS) -c scompulator.cpp scompulator.h names.h

main.o : main.cpp scompulator.h
	$(CC) $(CXXFLAGS) -c main.cpp scompulator.h

io.o : io.cpp io.h
	$(CC) $(CXXFLAGS) -c io.cpp