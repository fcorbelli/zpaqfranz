## This is zpaqfranz, a patched  but (maybe) compatible fork of ZPAQ version 7.15 
(http://mattmahoney.net/dc/zpaq.html)

# An archiver, just like 7z or RAR style to understand, with the ability to maintain a sort of "snapshot" of the data.

At every run only data changed since the last execution will be added, creating a new version (just like a snapshot).
It is then possible to restore the data @ the single version.
It shouldn't be difficult to understand its enormous potential, especially for those used to snapshots (by zfs or virtual machines).
Keeps a forever-to-ever copy (even thousands of versions) of your files, conceptually similar to MAC's time machines, 
but much more efficiently.
Ideal for virtual machine disk storage (ex vmdk).
Easily handles millions of files and tens of TBs of data.
Allows rsync copies to the cloud with minimal data transfer and encryption: nightly copies with simple FTTC (1-2MB/s upload bandwith) of archives of hundreds of gigabytes.
Multiple possibilities of data verification, fast, advanced and even paranoid.
A GUI (PAKKA) is available on Windows to make extraction easier.
To date, there is no software, free or paid, that matches its characteristics.
10+ years of developing (2009-now).


**Who did that?**

One of the world's leading scientists in compression.

No, not me, but this guy http://mattmahoney.net/dc/

ZPAQ - Wikipedia: https://en.wikipedia.org/wiki/ZPAQ

**When?**

From 2009 to 2016.

**Where?**

On a Russian compression forum, one of the most famous, but obviously super-niche

**Why is it not known as 7z or RAR, despite being enormously superior?**

Because lack of users who ... try it!

**Who are you?**

A user (and a developer) who has proposed and made various improvements that have been implemented over the years.
When the author left the project, I took charge of it, to insert the functions I need as a data storage manager.

**Why is it no longer developed? Why should I use your fork?**

Because mr. Mahoney is now retired and no longer supports it (he... run!)

**Why should I trust? It will be one of 1000 other programs that silently fail and give problems**

As the Russians say, trust me, but check.

**Archiving data requires safety. How can I be sure that I can then extract them without problems?**

It is precisely the portion of the program that I have evolved, implementing a barrage of controls up to the paranoid level, and more.
I don't think to talk about it now (too long, I should put the link to the Russian forum), but let's say there are verification mechanisms which you have probably never seen.

**I do not trust you, but I am becoming curious. So?**

You can try to build the port (of paq, inside archivers) but it is very, very, very old (2014).
You can download the original version (7.15 of 2016) directly from the author's website, and compile it, or get the original from github.
In this case be careful, because the source is divided into 3 source files, but nothing difficult for the compilation.

**OK, let's assume I want to try it out. How?**

If you are a FreeBSD user you can find an ancient version in /ports/archivers/paq (v 6.57 of 2014)

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
