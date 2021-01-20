This is zpaqfranz, a patched  but (maybe) compatible fork of 
Matt Mahoney's ZPAQ version 7.15 
(http://mattmahoney.net/dc/zpaq.html)

Provided as-is, with no warranty whatsoever, by me
Franco Corbelli, franco@francocorbelli.com

Being an experimental software, bugs may exist that also lead to data loss.
In short, use it only if you want.

====================
Portions of software by other authors, mentioned later, are included.
As far as I know this is allowed.
I apologize if I have unintentionally violated any rule.

Report it to me and I will delete as soon as possible.
===================

The source is composed of the fusion of different software from different authors. 
Therefore there is no uniform style of programming.
I have made a number of efforts to maintain compatibility 
with unmodified versions (7.15), 
even at the cost of expensive on inelegant workarounds.

So don't be surprised if it looks like what in Italy we call 
"zibaldone" or in Emilia-Romagna "mappazzone".

COMPILING

My main development platforms are Windows and FreeBSD.
I rarely use Linux or MacOS, so changes may be needed.

Three files are required 
libzpaq.cpp (original ZPAQ 7.15)
libzpaq.h (original ZPAQ 7.15)
zpaq.cpp (my fork)

In any case I use gcc.
Some examples for compiling

Windows 64 (g++ 7.3.0 64 bit) 
g++ -O3 zpaq.cpp libzpaq.cpp -o zpaqfranz -static 

Windows 32 (g++ 7.3.0 64 bit)
c:\mingw32\bin\g++  -O3 -m32 zpaq.cpp libzpaq.cpp -o zpaqfranz32 -static -pthread

FreeBSD (11.x) gcc7
gcc7 -O3 -march=native -Dunix zpaq.cpp -static -lstdc++ libzpaq.cpp -pthread -o zpaq -static -lm

FreeBSD (12.1) gcc 9.3.0
g++ -O3 -march=native -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaq -static-libstdc++ -static-libgcc

Debian Linux (11) gcc 8.3.0
g++ -O3 -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaq -static



==========

Notes in zpaq 7.15

  This software is provided as-is, with no warranty.
  I, Matt Mahoney, release this software into
  the public domain.   This applies worldwide.
  In some countries this may not be legally possible; if so:
  I grant anyone the right to use this software for any purpose,
  without any conditions, unless such conditions are required by law.

zpaq is a journaling (append-only) archiver for incremental backups.
Files are added only when the last-modified date has changed. Both the old
and new versions are saved. You can extract from old versions of the
archive by specifying a date or version number. zpaq supports 5
compression levels, deduplication, AES-256 encryption, and multi-threading
using an open, self-describing format for backward and forward
compatibility in Windows and Linux. See zpaq.pod for usage.

TO COMPILE:

This program needs libzpaq from http://mattmahoney.net/zpaq/
Recommended compile for Windows with MinGW:

  g++ -O3 zpaq.cpp libzpaq.cpp -o zpaq

With Visual C++:

  cl /O2 /EHsc zpaq.cpp libzpaq.cpp advapi32.lib

For Linux:

  g++ -O3 -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaq

For BSD or OS/X

  g++ -O3 -Dunix -DBSD zpaq.cpp libzpaq.cpp -pthread -o zpaq

Possible options:

  -DDEBUG    Enable run time checks and help screen for undocumented options.
  -DNOJIT    Don't assume x86 with SSE2 for libzpaq. Slower (disables JIT).
  -Dunix     Not Windows. Sometimes automatic in Linux. Needed for Mac OS/X.
  -DBSD      For BSD or OS/X.
  -DPTHREAD  Use Pthreads instead of Windows threads. Requires pthreadGC2.dll
             or pthreadVC2.dll from http://sourceware.org/pthreads-win32/
  -Dunixtest To make -Dunix work in Windows with MinGW.
  -fopenmp   Parallel divsufsort (faster, implies -pthread, broken in MinGW).
  -pthread   Required in Linux, implied by -fopenmp.
  -O3 or /O2 Optimize (faster).
  -o         Name of output executable.
  /EHsc      Enable exception handing in VC++ (required).
  advapi32.lib  Required for libzpaq in VC++.


==========

Notes in unzpaq 2.06

unzpaq206.cpp - ZPAQ level 2 reference decoder version 2.06.

  This software is provided as-is, with no warranty.
  I, Matt Mahoney, release this software into
  the public domain.   This applies worldwide.
  In some countries this may not be legally possible; if so:
  I grant anyone the right to use this software for any purpose,
  without any conditions, unless such conditions are required by law.

  
==========

Codes from other authors are used.
As far as I know of free use in opensource, 
otherwise I apologize.

Include some mod by users of encode.su forum (data man and reg2s and others)

xxhash64.h Copyright (c) 2016 Stephan Brumme. All rights reserved.
Crc32.h    Copyright (c) 2011-2019 Stephan Brumme. All rights reserved, slicing-by-16 contributed by Bulat Ziganshin
crc32c.c   Copyright (C) 2013 Mark Adler
