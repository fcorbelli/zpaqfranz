# makefile for zpaqfranz

CFLAGS = -Wall -O2
CXXFLAGS = -std=c++11 -Wall -O2
LDFLAGS =

CC = gcc
CXX = g++

BIN = zpaqfranz

.PHONY: all clean

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

zpaqfranz: zpaqfranz.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	-rm -f *.o $(BIN) core

