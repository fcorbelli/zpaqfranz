### generic Makefile. Look at BINDIR for the prefix (/usr/bin instead of /usr/local/bin)
### Generally, but not always, /usr/local/bin is for "BSD-style" systems, while /usr/bin is for older Linux systems  
### In reality, each platform has its own custom:cannot know "a priori"  

### Usually a straight
### make install clean
### is good enough

CXX?=           cc
INSTALL?=       install
RM?=            rm
PROG=           zpaqfranz
SOURCE=         zpaqfranz.cpp
CPPFLAGS+=	-Dunix
CXXFLAGS+=	-O3 -pthread
LDLIBS=		-lm
BINDIR=         /usr/local/bin
BSD_INSTALL_PROGRAM?=   ${INSTALL} -m 0755

all:    build

build:  ${PROG}

install:        ${PROG}
	${BSD_INSTALL_PROGRAM} -d ${DESTDIR}${BINDIR}
	${BSD_INSTALL_PROGRAM} $^ ${DESTDIR}${BINDIR}

${PROG}:        ${SOURCE}
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${LDFLAGS} $^ ${LDLIBS} -o $@
clean:
	${RM} -f ${PROG}
