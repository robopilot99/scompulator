CC=g++
CXXFLAGS=-g

.PHONY: all clean

all : scompulator.o main.o
	$(CC) $(CXXFLAGS) -o Scompulator scompulator.o main.o

clean : 
	rm *.o
	rm Scompulator

scompulator.o : scompulator.cpp scompulator.h
	$(CC) $(CXXFLAGS) -c scompulator.cpp scompulator.h

main.o : main.cpp scompulator.h
	$(CC) $(CXXFLAGS) -c main.cpp scompulator.h