prefix = /usr/local
CFLAGS+=        -Dunix
LDADD=          -pthread -lstdc++ -lm

all: src/zpaqfranz

src/zpaqfranz: src/zpaqfranz.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDCFLAGS) -o $@ $^ ${LDADD}

install: src/zpaqfranz
	install -D src/zpaqfranz  $(DESTDIR)$(prefix)/bin/zpaqfranz 

clean:
	-rm -f src/zpaqfranz

distclean: clean

uninstall:
	-rm -f $(DESTDIR)$(prefix)/bin/zpaqfranz

.PHONY: all install clean distclean uninstall
