CXX?=           c++
INSTALL?=       install
RM?=            rm
PROG=           zpaqfranz
CXXFLAGS+=      -O3 -std=c++11 -Dunix
LDADD=          -pthread -lm
BINDIR=         /usr/local/bin
BSD_INSTALL_PROGRAM?=   install -m 0555

all:    build

build:  ${PROG}

install:        ${PROG}
	${BSD_INSTALL_PROGRAM} ${PROG} ${DESTDIR}${BINDIR}

${PROG}:        ${OBJECTS}
	${CXX}  ${CXXFLAGS} zpaqfranz.cpp -o ${PROG} ${LDADD}
clean:
	${RM} -f ${PROG}