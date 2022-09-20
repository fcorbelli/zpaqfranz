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