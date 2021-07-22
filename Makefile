CXX=g++
CPPFLAGS+=-Dunix
CXXFLAGS=-O3 -march=native
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

all: zpaqfranz

zpaqfranz: 
        $(CXX) $(CPPFLAGS) $(CXXFLAGS) zpaqfranz.cpp -o $@ -pthread -static 

install: zpaqfranz
        install -m 0755 -d $(DESTDIR)$(BINDIR)
        install -m 0755 zpaqfranz $(DESTDIR)$(BINDIR)
        
clean:
        rm -f zpaqfranz

check: zpaqfranz
        ./zpaqfranz a ./archive.zpaq *  -xxh3 -test -verify
        rm ./archive.zpaq 
