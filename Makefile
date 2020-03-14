CC=g++
CXXFLAGS=-Wall -std=c++11

INC = -I./include
LIBS = -L./lib -lpthread

all: udp2

udp2 : build/main.o
	$(CC) $(CXXFLAGS) -o tools/udp2server build/main.o build/UDPServer.o $(LIBS)

build/main.o : build/UDPServer.o tools/main.cpp 
	$(CC) $(CXXFLAGS) $(INC) -c tools/main.cpp build/UDPServer.o -o build/main.o  
	
build/UDPServer.o: src/UDPServer.cpp
	$(CC) $(CXXFLAGS) $(INC) -c src/UDPServer.cpp -o build/UDPServer.o

clean:
	rm -rf tools/mpnext build/* lib/*
