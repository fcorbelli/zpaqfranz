# zpaqfranz: advanced and compatible fork of ZPAQ 7.15, with SFX (on Windows)  
### Main platforms: Windows, FreeBSD, Linux
Secondary platforms: Solaris, MacOS, OpenBSD, OmniOS, ESXi, QNAP-based NAS, Haiku OS  

### [Binaries on sourceforge](https://sourceforge.net/projects/zpaqfranz/files/) 
### [Quick start FreeBSD](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart:-FreeBSD)   
[Main site of old ZPAQ](http://mattmahoney.net/dc/zpaq.html)      [Reference decompressor](https://github.com/fcorbelli/unzpaq/tree/main) 
### OpenBSD: `pkg_add zpaqfranz`
### FreeBSD: `pkg install zpaqfranz`

## Classic archivers (tar, 7z, RAR etc) are obsolete, when used for repeated backups (daily etc), compared to the ZPAQ technology, that maintain "snapshots" (versions) of the data.

[Wiki being written - be patient](https://github.com/fcorbelli/zpaqfranz/wiki)  
[Quick link to ZFS's snapshots support functions](https://github.com/fcorbelli/zpaqfranz/wiki/Command:-zfs(something))

**Why do you say 7z, RAR etc are obsolete? How is ZPAQ so innovative?**

Let's see.
Archiving a folder multiple times (5), simulating a daily run Monday-to-Friday, with 7z

https://user-images.githubusercontent.com/77727889/215149589-0f2d9f91-ea5a-4f60-b587-f2a506148fe9.mp4

Same, but with zpaqfranz

https://user-images.githubusercontent.com/77727889/215148702-edb8e5bb-8f4e-42bb-9637-6ee98742318a.mp4

_As you can see, the .7z "daily" 5x backups takes ~ 5x the space of the .zpaq_

![compare](https://user-images.githubusercontent.com/77727889/215150599-83032cc6-06b0-432d-ba3b-b410698e3631.jpg)

# Seeing is believing ("real world")

I thought it's best to show the difference for a more realistic example.  

Physical (small fileserver) Xeon machine with 8 cores, 64GB RAM and NVMe disks, plus Solaris-based NAS, 1Gb ethernet

Rsync update from filesystem to filesystem (real speed)  

https://user-images.githubusercontent.com/77727889/215152167-c6ce107a-6345-4060-b7a7-33ad30b269ee.mp4


Rsync update to Solaris NAS (real speed)

https://user-images.githubusercontent.com/77727889/215152259-2baa7001-d838-40de-b56c-6fe3feff9f1b.mp4


Backup update from file system with zpaqfranz (real speed)  

https://user-images.githubusercontent.com/77727889/215146670-1a11cd5d-6f00-4544-b797-9ca288ae12b1.mp4

Backup upgrade via zfsbackup (real speed)

https://user-images.githubusercontent.com/77727889/215147310-cc760f20-08b8-4088-9d8a-f58f00eac211.mp4

# What?

At every run only data changed since the last execution will be added, creating a new version (the "snapshot").
It is then possible to restore the data @ the single version, just like snapshots by zfs or virtual machines, but a single-file level.  
- Keeps a forever-to-ever copy (even thousands of versions), conceptually similar to Mac's time machine, but much more efficiently.  
- Ideal for virtual machine disk storage (ex backup of vmdk), virtual disks (VHDx) and even TrueCrypt containers.  
- Easily handles millions of files and tens of TBs of data.  
- Allows rsync (or zfs replica) copies to the cloud with minimal data transfer and encryption.    
- Multiple possibilities of data verification, fast, advanced and even paranoid.
- Some optimizations for modern hardware (aka: SSD, NVMe, multithread).
- By default triple-check with "chunked" SHA-1, XXHASH64 and CRC-32 (!).  

```
For even higher level of paranoia, it is possible to use others hash algorithms, as
```
- MD5
- SHA-1 of the full-file (NIST FIPS 180-4)
- XXH3-128
- BLAKE3 128
- SHA-2-256 (NIST FIPS 180-4)
- SHA-3-256 (NIST FIPS 202)
- WHIRLPOOL (ISO/IEC 10118-3)
- HIGHWAY (64,128,256)
...And much more.  
A freeware GUI (PAKKA) is available on Windows to make extraction easier.  

**No complex (and fragile) repository folders, with hundreds of "whatever", just only a single file!**  
## To date, there is no software, free or paid, that matches this characteristics  
_AFAIK of course_  
10+ years of developing (2009-now).

**Who did that?**

One of the world's leading scientists in compression.

[No, not me, but this guy](http://mattmahoney.net/) [ZPAQ - Wikipedia](https://en.wikipedia.org/wiki/ZPAQ)

**When?**

From 2009 to 2016.

**Where?**

On a Russian compression forum, one of the most famous, but obviously super-niche

**Why is it not known as 7z or RAR, despite being enormously superior?**

Because lack of users who ... try it!

**Who are you?**

A user (and a developer) who has proposed and made various improvements that have been implemented over the years.
When the author left the project, I made my fork to make the functions I need as a data storage manager.

**Why is it no longer developed? Why should I use your fork?**

Because Dr. Mahoney is now retired and no longer supports it (he... run!)

**Why should I trust? It will be one of 1000 other programs that silently fail and give problems**

As the Russians say, trust me, but check.

**Archiving data requires safety. How can I be sure that I can then extract them without problems?**

It is precisely the portion of the program that I have evolved, implementing a barrage of controls up to the paranoid level, and more.
Let's say there are verification mechanisms which you have probably never seen. Do you want to use SHA-2/SHA-3 to be very confident? You can.

**ZPAQ (zpaqfranz) allows you to NEVER delete the data that is stored and will be available forever (in reality typically you starts from scratch every 1,000 or 2,000 versions, for speed reasons), and restore the files present to each archived version, even if a month or three years ago.**

In this "real world" example (a ~500.000 files / ~500GB file server of a mid-sized enterprise), you will see 1042 "snapshots", stored in 877GB.

```
root@f-server:/copia1/copiepaq/spaz2020 # zpaqfranz i fserver_condivisioni.zpaq
zpaqfranz v51.27-experimental snapshot archiver, compiled May 26 2021
fserver_condivisioni.zpaq:
1042 versions, 1.538.727 files, 15.716.105 fragments, 877.457.003.477 bytes (817.20 GB)
Long filenames (>255)     4.526

Version(s) enumerator
-------------------------------------------------------------------------
< Ver  > <  date  > < time >  < added > <removed>    <    bytes added   >
-------------------------------------------------------------------------
00000001 2018-01-09 16:56:02  +00308608 -00000000 ->      229.882.913.501
00000002 2018-01-09 18:06:28  +00007039 -00000340 ->           47.356.864
00000003 2018-01-10 15:06:25  +00007731 -00000159 ->            7.314.709
00000004 2018-01-10 15:17:44  +00007006 -00000000 ->              612.584
00000005 2018-01-10 15:47:03  +00007005 -00000000 ->              611.980
00000006 2018-01-10 18:03:08  +00008135 -00000829 ->        2.698.417.427
(...)
00000011 2018-01-10 19:20:30  +00007007 -00000000 ->              613.273
00000012 2018-01-11 07:00:36  +00007008 -00000000 ->              613.877
(...)
00000146 2018-03-27 17:08:39  +00001105 -00000541 ->          164.399.767
00000147 2018-03-28 17:08:28  +00000422 -00000134 ->          277.237.055
00000148 2018-03-29 17:12:02  +00011953 -00011515 ->          826.218.948
(...)
00000159 2018-04-09 17:08:18  +00000342 -00000016 ->          161.661.586
00000160 2018-04-10 17:08:22  +00000397 -00000017 ->          129.474.045
(...)
00000315 2018-09-12 17:08:29  +00000437 -00000254 ->        1.156.119.701
00000316 2018-09-13 17:08:27  +00000278 -00000003 ->          348.365.215
00000317 2018-09-14 17:08:28  +00000247 -00000041 ->          587.163.740
00000318 2018-09-15 17:08:29  +00000003 -00000000 ->           14.610.281
00000319 2018-09-16 17:08:33  +00000003 -00000000 ->           14.612.065
00000320 2018-09-17 17:08:31  +00000226 -00000000 ->           56.830.129
00000321 2018-09-18 17:08:29  +00000276 -00000017 ->           43.014.662
00000322 2018-09-19 17:08:33  +00000247 -00000002 ->           43.257.079
(...)
00000434 2019-02-09 18:10:08  +00000009 -00000001 ->           17.491.737
00000435 2019-02-10 18:10:06  +00000001 -00000000 ->           15.472.629
00000436 2019-02-11 18:10:35  +00000354 -00000077 ->          310.408.124
(...)
00000445 2019-02-20 18:11:52  +00005711 -00005474 ->          662.296.698
00000446 2019-02-21 18:10:32  +00000325 -00004017 ->          592.816.097
00000447 2019-02-22 18:18:37  +00009206 -00000021 ->       24.236.986.132
(...)
00000492 2019-04-13 17:11:37  +00000003 -00000000 ->           16.651.589
00000493 2019-04-14 17:11:37  +00000001 -00000000 ->           16.645.997
00000494 2019-04-15 17:12:01  +00000279 -00000012 ->           78.171.480
00000495 2019-04-16 17:12:18  +00000231 -00000018 ->           70.858.973
00000496 2019-04-17 17:12:06  +00000356 -00000018 ->          119.696.255
00000497 2019-04-18 17:11:03  +00000305 -00000023 ->        2.503.628.883
00000498 2019-04-19 17:12:06  +00000298 -00000008 ->        3.007.508.933
(...)
00000508 2019-04-29 17:11:04  +00000831 -00000470 ->           94.625.724
(...)
00000572 2019-07-02 17:13:53  +00000449 -00000040 ->          108.048.297
00000573 2019-07-03 17:13:55  +00000579 -00000044 ->          400.854.748
00000574 2019-07-04 17:13:52  +00000631 -00000237 ->           91.992.975
(...)
00001039 2021-05-02 17:17:42  +00030599 -00031135 ->       12.657.155.316
00001040 2021-05-03 17:14:03  +00000960 -00000095 ->          398.358.496
00001041 2021-05-04 17:13:40  +00000605 -00000004 ->           95.909.988
00001042 2021-05-05 17:15:13  +00000579 -00000008 ->           82.487.415

54.799 seconds (all OK)
```

Do you want to restore @ 2018-03-28?
```
00000147 2018-03-28 17:08:28  +00000422 -00000134 ->          277.237.055
```
Version 147 =>
```
zpaqfranz x ... -until 147
```
Do you want 2021-03-05?
Version 984 =>
```
zpaqfranz x ... -until 984
```
Another real world example: 4900 versions, from mid-2017
```
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
franz:use comment
old_aserver.zpaq:

4904 versions, 385.830 files, 3.515.679 fragments, 199.406.200.193 bytes (185.71
GB)

Version comments enumerator
------------
00000001 2017-08-16 19:26:15  +00090863 -00000000 ->       79.321.339.869
00000002 2017-08-17 13:29:25  +00000026 -00000000 ->              629.055
00000003 2017-08-17 13:30:41  +00000005 -00000000 ->               18.103
00000004 2017-08-17 14:34:12  +00000005 -00000000 ->               18.149
00000005 2017-08-17 15:28:42  +00000008 -00000000 ->               99.062
00000006 2017-08-17 19:30:03  +00000008 -00000000 ->            1.013.616
00000007 2017-08-18 19:33:14  +00000021 -00000001 ->            2.556.335
00000008 2017-08-19 19:29:23  +00000025 -00000000 ->            1.377.082
00000009 2017-08-20 19:29:56  +00000002 -00000000 ->               24.153
00000010 2017-08-21 19:34:35  +00000031 -00000000 ->            2.554.582
(...)
00004890 2021-02-16 16:40:51  +00000190 -00000005 ->           99.051.540
00004891 2021-02-16 19:30:17  +00000065 -00000006 ->           16.467.364
00004892 2021-02-17 19:34:04  +00000381 -00000257 ->           95.354.305
(...)
00004900 2021-02-25 19:35:47  +00000755 -00000611 ->          132.241.557
00004901 2021-02-26 19:57:16  +00000406 -00000253 ->          122.669.868
00004902 2021-02-27 20:33:45  +00000029 -00000002 ->           12.677.932
00004903 2021-02-28 20:34:00  +00000027 -00000001 ->            6.978.088
00004904 2021-03-01 20:33:52  +00000174 -00000019 ->           77.113.147
```

until 2021 (4 years later)

This is a ~200GB server
```
(...)
- 2019-09-23 10:14:44       2.943.578.106  0666 /tank/mboxstorico/inviata_spazzatura__2017_2018
- 2021-02-18 10:16:25           4.119.172  0666 /tank/mboxstorico/inviata_spazzatura__2017_2018.msf
- 2019-10-25 15:39:15       1.574.715.392  0666 /tank/mboxstorico/nstmp
- 2020-11-28 20:33:22           2.038.165  0666 /tank/mboxstorico/nstmp.msf
- 2021-02-25 17:48:11               8.802  0644 /tank/mboxstorico/sha1.txt

214.379.664.412 (199.66 GB) of 214.379.664.412 (199.66 GB) in 154.975 files shown
```
so for 4900 versions you need
200GB*4900 = ~980TB with something like tar, 7z, RAR etc (yes, 980 terabytes),
versus ~200GB (yes, 200GB) with zpaq.

Same things for virtual machines (vmdks)

## Why you say uniqueness? We got (hb) hashbackup, borg, restic, bupstash etc ##

Because other software (sometimes very, very good) runs on complex "repositories", very fragile and way too hard to manage (at least for my tastes).  
It may happen that you have to worry about backing up ... the backup, because maybe some files were lost during a transfer, corrupted etc.  
_If it's simple, maybe it will work_

## Too good to be true? ##

Obviously this is not "magic", it is simply the "chaining" of a block deduplicator with a compressor and an archiver.
There are faster compressors.
There are better compressors.
There are faster archivers.
There are more efficient deduplicators.

But what I have never found is a combination of these that is so simple to use and reliable, with excellent handling of non-Latin filenames (Chinese, Russian etc).

This is the key: you don't have to use complex "chain" of tar | srep | zstd | something hoping that everything will runs file, but a single ~1MB executable, with 7z-like commands.  
You don't even have to install a complex program with many dependencies that will have to read a folder (the repository) with maybe thousands of files, hoping that they are all fully functional.

There are also many great features for backup, I mention only the greatest.
**The ZPAQ file is "in addition", it is never modified**

So rsync --append will copy only the portion actually added, for example on ssh tunnel to a remote server, or local NAS (QNAP etc) with tiny times.

You don't have to copy or synchronize let's say 700GB of tar.gz,7z or whatever, but only (say) the 2GB added in the last copy, the first 698GB are untouched.

This opens up the concrete possibility of using VDSL connections (upload ~ 2/4MB /s) to backup even virtual servers of hundreds of gigabytes in a few minutes.

**Bonus: for a developer it's just like a "super-git-versioning"**

In the makefile just put at top a zpaq-save-everything and you will keep all the versions of your software, even with libraries, SQL dump etc.
A single archive keeps everything, forever, with just one command (or two, for verify)
    
**Defects?**

Some.

The main one is that the listing of files is not very fast, when there are many versions (thousands), due to the structure of the archiver-file-format. 
*I could get rid of it, but at the cost of breaking the backward compatibility of the file format, so I don't want to. On 52+ there is a workaround (-filelist)*

It is not the fastest tool out there, with real world performance of 80-200MB/s (depending on the case and HW of course).
*Not a big deal for me (I have very powerful HW, and/or run nightly cron-tasks)*

Extraction can require a number of seeks (due to various deduplicated blocks), which can slow down extraction on magnetic disks (but not on SSDs).  
*If you have plenty of RAM now it is possible to bypass with the w command*

No other significant ones come to mind, except that it is known and used by few


**I do not trust you, but I am becoming curious. So?**

On **FreeBSD** [you can try to build the port (of paq, inside archivers)](https://www.freshports.org/archivers/paq) but it is very, very, very old (v 6.57 of 2014)  

On **Debian** [there is a zpaq 7.15 package](https://packages.debian.org/sid/utils/zpaq)  
You can download the original version (7.15 of 2016) directly from the author's website, and compile it, or get the same from github.  
In this case be careful, because the source is divided into 3 source files, but nothing difficult for the compilation.  

**OK, let's assume I want to try out zpaqfranz. How?**  

From branch 51 all source code is merged in one zpaqfranz.cpp aiming to make it as easy as possible to compile on "strange" systems (NAS, vSphere etc).  
Updating, compilation and Makefile are now trivial.  
Or (for a newer, but not the very last, build)...  
### OpenBSD: `pkg_add zpaqfranz`
### FreeBSD: `pkg install zpaqfranz`

# How to build

My main development platforms are INTEL Windows (non-Intel Windows (arm) currently unsupported) and FreeBSD.

I rarely use Linux or MacOS or whatever (for compiling), so fixing may be needed.

As explained the program is single file, be careful to link the pthread library.
You need it for ESXi too, even if it doesn't work. Don't be afraid, zpaqfranz knows!

### Almost "universal" (minimal) Makefile.  
_Beware to change /usr/local/bin to /bin on some *nix!_
```
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
```

Quick and dirtier!
```
wget https://github.com/fcorbelli/zpaqfranz/raw/main/zpaqfranz.cpp
wget https://github.com/fcorbelli/zpaqfranz/raw/main/NONWINDOWS/Makefile
make install clean
```

Dirtiest (!) no SSL certificate (very old systems), get the nightly build
```
wget http://www.francocorbelli.it/zpaqfranz.cpp -O zpaqfranz.cpp
```

then... build (aka: compile)


_Library dependencies are minimal:   libc,libc++,libcxxrt,libm,libgcc_s,libthr_

DEFINEs at compile-time
```
(nothing)                          // Compile for INTEL Windows
-DHWBLAKE3 blake3_windows_gnu.S    // On Win64 enable HW accelerated BLAKE3 (with assembly)
-DHWSHA1                           // On Win64 enable HW SHA1 (-hw)
-Dunix                             // Compile on "something different from Windows"
-DSOLARIS                          // Solaris is similar, but not equal, to BSD Unix
-DNOJIT                            // By default zpaqfranz works on Intel CPUs
                                   // (for simplicity I'll call them Intel, meaning x86-SSE2 and amd64)
                                   // On non-Intel a -NOJIT should runs fine on LITTLE ENDIANs
                                   // like Linux aarch64, Android aarch64 etc
                                   // On BIG ENDIAN or "strange things" like middle endian 
                                   // (Honeywell 316) or little word (PDP-11)
                                   // the autotest command is for you :)
-DANCIENT                          // Turn off some functions for compiling in very old systems
                                   // consuming less RAM (ex. PowerPC Mac), no auto C++
-DBIG                              // Turn on BIG ENDIAN at compile time
-DDEBUG                            // Old 7.15, almost useless. Use -debug switch instead
-DESX                              // Yes, zpaqfranz run (kind of) on ESXi too :-)
-DALIGNMALLOC                      // Force malloc to be aligned at something (sparc64)
-DSERVER                           // Enable the cloudpaq client (for Windows)
```

### HIDDEN GEMS
If the (non Windows) executable is named "dir" act (just about)... like Windows' dir
Beware of collisions with other software "dir".
_It is way better than dir_

### WARNINGS
Some strange warnings with some compilers (too old, or too new), not MY fault

### STRANGE THINGS

_NOTE1: -, not -- (into switch)_

_NOTE2: switches ARE case sensitive.   -maxsize <> -MAXSIZE_

### THE JIT (just-in-time)
zpaqfranz can translate ZPAQL opcodes into "real" Intel (amd64 or x86+SSE2) code, by default  
On other systems a **-DNOJIT** (arm CPUs for example) will enforce software interpration

### SHA-1 HARDWARE ACCELERATION
Some CPUs does have SHA instructions (typically AMD, not very widespread on Intel).  
So you can use a piece of 7-zip by Igor Pavlov (I am sure you know 7z) that is  
not really useful, but just for fun (faster BUT with higher latency).  
For performances reason, no run-time CPU compatibility checks, must be turn on   
via optional -hw switch  
On AMD 5950X runs ~1.86 GB/s vs ~951 MB/s  
The obj can be assembled from the fixed source code with asmc64  
https://github.com/nidud/asmc  
asmc64.exe sha1ugo.asm 
Then link the .obj and compile with -DHWSHA1  
Short version:  not worth the effort for the GA release  

### STATIC LINKING
I like **-static** very much, there are a thousand arguments as to whether it is good or not. 
There are strengths and weaknesses. 
Normally I prefer it, you do as you prefer.

### TO BE NATIVE OR NOT TO BE?
The **-march=native**  is a switch that asks the compiler to activate all possible 
optimizations for the CPU on which zpaqfranz is being compiled. 
This is to obtain the maximum possible performance, 
while binding the executable to the processor. 
It should not be used if you intend, for some reason, 
to transfer the object program to a different system.
If you are compiling from source you can safely use it.

### DEBIAN (and derivates)
Debian does not "like" anything embedded https://wiki.debian.org/EmbeddedCopies
zpaqfranz (on Windows) have two SFX modules (32 and 64)  
and (every platform) a testfile (sha256.zpaq) for extraction autotest  
(aka: weird CPUs)  
It is possible to make a Debian-package-compliant source code  
with some sed (or a single sed -e) (of course remove the |)
```
sed -i "/DEBIAN|START/,/\/\/\/DEBIA|NEND/d"  zpaqfranz.cpp
sed -i "s/\/\/\/char ext|ract_test1/char ext|ract_test1/g" zpaqfranz.cpp
```

### Arch Linux
I strongly advise against using zpaqfranz on this Linux distro(s).  
There is a bizarre policy on compiling executables.  
Obviously no one forbids it (**runs fine**), but just don't ask for help.  

### TARGET EXAMPLES
```
Windows 64 (g++ 7.3.0)
g++ -O3  zpaqfranz.cpp -o zpaqfranz 

Windows 64 (g++ 10.3.0) MSYS2
g++ -O3  zpaqfranz.cpp -o zpaqfranz -pthread -static

Windows 64 (g++, Hardware Blake3 implementation)
In this case, of course, linking the .S file is mandatory
g++ -O3 -DHWBLAKE3 blake3_windows_gnu.S zpaqfranz.cpp -o zpaqfranz -pthread -static

Windows 64 (g++, Hardware Blake3 implementation PLUS HW SHA1)
g++ -O3 -DHWBLAKE3 -DHWSHA1 blake3_windows_gnu.s zpaqfranz.cpp sha1ugo.obj -o zpaqfranzhw -pthread -static

Windows 32 (g++ 7.3.0 64 bit)
c:\mingw32\bin\g++ -m32 -O3 zpaqfranz.cpp -o zpaqfranz32 -pthread -static

Windows 64 (g++ 7.3.0), WITH cloud paq
g++ -O3 -DSERVER zpaqfranz.cpp -o zpaqfranz -lwsock32 -lws2_32

FreeBSD (11.x) gcc 7
gcc7 -O3 -Dunix zpaqfranz.cpp -lstdc++ -pthread -o zpaqfranz -static -lm

FreeBSD (12.1) gcc 9.3.0
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc

FreeBSD (11.4) gcc 10.2.0
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc -Wno-stringop-overflow

FreeBSD (11.3) clang 6.0.0
clang++ -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

OpenBSD 6.6 clang++ 8.0.1
OpenBSD 7.1 clang++ 13.0.0
clang++ -Dunix -O3 zpaqfranz.cpp -o zpaqfranz -pthread -static

Debian Linux (10/11) gcc 8.3.0
ubuntu 21.04 desktop-amd64 gcc  10.3.0
manjaro 21.07 gcc 11.1.0
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

QNAP NAS TS-431P3 (Annapurna AL314) gcc 7.4.0
g++ -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -Wno-psabi

Fedora 34 gcc 11.2.1
Typically you will need some library (out of a fresh Fedora box)
sudo dnf install glibc-static libstdc++-static -y;
Then you can compile, via Makefile or "by hand"
(do not forget... sudo!)

CentoOS
Please note:
"Red Hat discourages the use of static linking for security reasons. 
Use static linking only when necessary, especially against libraries provided by Red Hat. "
Therefore a -static linking is often a nightmare on CentOS => change the Makefile
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz

Solaris 11.4 gcc 7.3.0
OmniOS r151042 gcc 7.5.0
Beware: -DSOLARIS and some different linking options
g++ -O3 -DSOLARIS zpaqfranz.cpp -o zpaqfranz  -pthread -static-libgcc -lkstat

MacOS 11.0 gcc (clang) 12.0.5, INTEL
MacOS 12.6 gcc (clang) 13.1.6, INTEL
Please note:
The -std=c++11 is required, otherwise you have to change half a dozen lines (or -DANCIENT). 
No -static here
"Apple does not support statically linked binaries on Mac OS X. 
(...) Rather, we strive to ensure binary 
compatibility in each dynamically linked system library and framework
(AHAHAHAHAHAH, note by me)
Warning: Shipping a statically linked binary entails a significant compatibility risk. 
We strongly recommend that you not do this..."
Short version: Apple does not like -static, so compile with
g++ -Dunix  -O3 -march=native zpaqfranz.cpp -o zpaqfranz -pthread  -std=c++11

Mac PowerPC with gcc4.x
Look at -DBIG (for BIG ENDIAN) and -DANCIENT (old-compiler)
g++ -O3 -DBIG -DANCIENT -Dunix -DNOJIT zpaqfranz.cpp -o zpaqfranz -pthread

Apple Macintosh (M1/M2)
Untested (yet), should be
g++ -Dunix  -O3 -DNOJIT zpaqfranz.cpp -o zpaqfranz -pthread  -std=c++11

ESXi (gcc 3.4.6)
Note: not fully developed ( extract() with minimum RAM need to be implemented )
g++ -O3 -DESX zpaqfranz.cpp -o zpaqfranz6  -pthread -static -s

sparc64 (not tested)
try
-DALIGNMALLOC (+ other switches)

Haiku R1/beta4, 64 bit (gcc 11.2.0), hrev56721
Not very tested
g++ -O3 -Dunix zpaqfranz.cpp -o zpaqfranz  -pthread -static


Beware of #definitions
g++ -dM -E - < /dev/null
sometimes __sun, sometimes not
```


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


**Single help**
```
Help    "ALL IN": zpaqfranz h h         zpaqfranz /? /?
Help     on XXX : zpaqfranz h   XXX     zpaqfranz /? XXX    zpaqfranz -h XXX
Examples of XXX : zpaqfranz -he XXX
----------------------------------------------------------------------------------------------------
XXX can be a COMMAND: a autotest b c cp d dir dirsize e f find g i isopen k l m n p password pause q r
 rd rsync s sfx sum t trim utf v versum w x z zfsbackup zfsreceive zfsrestore
----------------------------------------------------------------------------------------------------
OR a set of SWITCHES: franz main normal voodoo

It is possible to call -? (better -h for Unix) and -examples with a parameter
a b c d dir f i k l m n r rsync s sfx sum t utf v x z franz main normal voodoo
```
