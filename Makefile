CXX=g++
CPPFLAGS+=-Dunix
CXXFLAGS=-O3 -march=native
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

all: zpaqfranz

zpaqfranz:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) zpaqfranz.cpp -o $@ -pthread

install: zpaqfranz
	install -m 0755 -d $(DESTDIR)$(BINDIR)
	install -m 0755 zpaqfranz $(DESTDIR)$(BINDIR)

clean:
	rm -f zpaqfranz

check: zpaqfranz
	rm archive.zpaq
	./zpaqfranz a archive.zpaq zpaqfranz.cpp
	./zpaqfranz t archive.zpaq -verify
	rm archive.zpaq
