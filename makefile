CC=g++
CXXFLAGS=-g -Wall

.PHONY: all

all : io.o periphrials.o scompulator.o main.o
	$(CC) $(CXXFLAGS) -pthread -o Scompulator scompulator.o main.o io.o IODevices/periphrials.o

clean : 
	rm *.o
	rm Scompulator
	rm IODevices/periphrials.o

scompulator.o : scompulator.cpp scompulator.h $(wildcard *.h) $(wildcard IODevices/*.h)
	$(CC) $(CXXFLAGS) -c scompulator.cpp scompulator.h names.h

main.o : main.cpp $(wildcard *.h)
	$(CC) $(CXXFLAGS) -pthread -c main.cpp scompulator.h

io.o : io.cpp $(wildcard *.h)
	$(CC) $(CXXFLAGS) -c io.cpp

periphrials.o : $(wildcard IODevices/*.cpp) $(wildcard IODevices/*.h)
	$(CC) $(CXXFLAGS) -o IODevices/periphrials.o -c IODevices/*.cpp