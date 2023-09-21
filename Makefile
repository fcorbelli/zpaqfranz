### generic Makefile. Look at BINDIR for the prefix (/usr/bin instead of /usr/local/bin)
### Generally, but not always, /usr/local/bin is for "BSD-style" systems, while /usr/bin is for older Linux systems  
### In reality, each platform has its own custom:cannot know "a priori"  

### Usually a straight
### make install clean
### is good enough

CC?=            cc
INSTALL?=       install
RM?=            rm
PROG=           zpaqfranz
CFLAGS+=        -O3 -Dunix
LDADD=          -pthread -lstdc++ -lm
BINDIR=         /usr/local/bin
BSD_INSTALL_PROGRAM?=   install -m 0555

all:    build

build:  ${PROG}

install:        ${PROG}
	${BSD_INSTALL_PROGRAM} ${PROG} ${DESTDIR}${BINDIR}

${PROG}:        ${OBJECTS}
	${CC}  ${CFLAGS} zpaqfranz.cpp -o ${PROG} ${LDADD}
clean:
	${RM} -f ${PROG}
