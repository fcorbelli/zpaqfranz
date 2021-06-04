## This is zpaqfranz, a patched  but (maybe) compatible fork of ZPAQ version 7.15 
(http://mattmahoney.net/dc/zpaq.html)

Ancient version is in FreeBSD /ports/archivers/paq (v 6.57 of 2014)

From branch 51 all source code merged in one zpaqfranz.cpp
aiming to make it as easy as possible to compile on "strange" systems (NAS, vSphere etc).

So be patient if the source is not linear, 
updating and compilation are now trivial.

The source is composed of the fusion of different software 
from different authors. 

Therefore there is no uniform style of programming. 

I have made a number of efforts to maintain compatibility 
with unmodified version (7.15), even at the cost 
of expensive on inelegant workarounds.

So don't be surprised if it looks like what in Italy 
we call "zibaldone" or in Emilia-Romagna "mappazzone".

Windows binary builds (32 and 64 bit) on github/sourceforge

## **Provided as-is, with no warranty whatsoever, by Franco Corbelli, franco@francocorbelli.com**


####      Key differences against 7.15 by zpaqfranz -diff or here 
####  https://github.com/fcorbelli/zpaqfranz/blob/main/differences715.txt 

Portions of software by other authors, mentioned later, are included.
As far as I know this is allowed by the licenses. 

**I apologize if I have unintentionally violated any rule.**
**Report it and I will fix as soon as possible.**

- Include mod by data man and reg2s patch from encode.su forum 
- Crc32.h Copyright (c) 2011-2019 Stephan Brumme 
- Slicing-by-16 contributed by Bulat Ziganshin 
- xxHash Extremely Fast Hash algorithm, Copyright (C) 2012-2020 Yann Collet 
- crc32c.c Copyright (C) 2013 Mark Adler



FreeBSD port, quick and dirty to get a /usr/local/bin/zpaqfranz
```
mkdir /tmp/testme
cd /tmp/testme
wget http://www.francocorbelli.it/zpaqfranz/zpaqfranz-51.30.tar.gz
tar -xvf zpaqfranz-51.30.tar.gz
make install clean
```

NOTE1: -, not -- (into switch)
NOTE2: switches ARE case sensitive.   -maxsize <> -MAXSIZE


**Long (full) help**
```
zpaqfranz help
zpaqfranz -h
zpaqfranz -help
```

**Some examples**
```
zpaqfranz -he
zpaqfranz --helpe
zpaqfranz -examples
```

**Brief difference agains standard 7.15**
```
zpaqfranz -diff 
```

# How to build
My main development platforms are Windows and FreeBSD. 
I rarely use Linux or MacOS, so changes may be needed.

As explained the program is single file, 
be careful to link the pthread library.

Targets
```
Windows 64 (g++ 7.3.0)
g++ -O3  zpaqfranz.cpp -o zpaqfranz 

Windows 32 (g++ 7.3.0 64 bit)
c:\mingw32\bin\g++ -m32 -O3 zpaqfranz.cpp -o zpaqfranz32 -pthread -static

FreeBSD (11.x) gcc 7
gcc7 -O3 -march=native -Dunix zpaqfranz.cpp -static -lstdc++ -pthread -o zpaqfranz -static -lm

FreeBSD (12.1) gcc 9.3.0
g++ -O3 -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc

FreeBSD (11.4) gcc 10.2.0
g++ -O3 -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc -Wno-stringop-overflow

FreeBSD (11.3) clang 6.0.0
clang++ -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

Debian Linux (10/11) gcc 8.3.0
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

QNAP NAS TS-431P3 (Annapurna AL314) gcc 7.4.0
g++ -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -Wno-psabi
```
