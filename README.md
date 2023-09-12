# zpaqfranz: advanced multiversioned archiver, with HW acceleration and SFX (on Windows)      

### Swiss army knife for backup and disaster recovery, like 7z or RAR on steroids, with deduplicated "snapshots" (versions). Conceptually similar to the Mac time machine, but much more efficiently. zpaq 7.15 fork.    
     

|  Platform                                                           | OS package                    |  Version    | Video|
|  ----------                                                         | -----                         |  ---------- |  -------    |
|  [Windows 32/64bit](https://sourceforge.net/projects/zpaqfranz/files)   |                                  |      |      |      
|  [OpenBSD](http://www.francocorbelli.it/zpaqfranz/openbsd)                 |`pkg_add zpaqfranz`            |      |      |
|  [FreeBSD](http://www.francocorbelli.it/zpaqfranz/freebsd)                 |`pkg install zpaqfranz`        |      |      |
|  [MacOS](http://www.francocorbelli.it/zpaqfranz/mac)                       |`brew install zpaqfranz`       |![Badge](https://img.shields.io/homebrew/v/zpaqfranz)|      |
|  [OpenSUSE](http://www.francocorbelli.it/zpaqfranz/opensuse)               |`sudo zypper install zpaqfranz`|      |      |
|  [Debian (Ubuntu etc)](http://www.francocorbelli.it/zpaqfranz/debian)      |                               |58.10g|[Desktop](http://www.francocorbelli.it/zpaqfranz/video/debian-deb.mp4)      |
|  [Linux generic](http://www.francocorbelli.it/zpaqfranz/linux)             |                               |      |      |
|  [Solaris](http://www.francocorbelli.it/zpaqfranz/solaris)                 |                               |      |      |
|  [OmniOS](http://www.francocorbelli.it/zpaqfranz/omnios)                   |                               |      |      |
|  [QNAP (Annapurna)](http://www.francocorbelli.it/zpaqfranz/qnap)           |                               |      |      | 
|  [Haiku](http://www.francocorbelli.it/zpaqfranz/haiku)                     |                               |      |      |
|  [ESXi](http://www.francocorbelli.it/zpaqfranz/esxi)                       |                               |      |      |
| [Freeware GUI for Windows](https://github.com/fcorbelli/zpaqfranz/wiki/PAKKA-Windows-32-bit-extractor) |   |34.2  |      |

[![Wiki](https://github-production-user-asset-6210df.s3.amazonaws.com/77727889/267342908-7d4c5bb9-6ea2-4735-9226-4d8112c5d65d.jpg)](https://github.com/fcorbelli/zpaqfranz/wiki) [![Help](https://github-production-user-asset-6210df.s3.amazonaws.com/77727889/267348388-d539932d-55c6-454a-a27b-054a10ae5f35.jpg)](https://github.com/fcorbelli/zpaqfranz/wiki/HELP-integrated) [![Sourceforge](https://github-production-user-asset-6210df.s3.amazonaws.com/77727889/267350249-91c18ca6-8c74-4585-96f4-3c72fd2c6725.jpg)](https://sourceforge.net/projects/zpaqfranz/)

## Classic archivers (tar, 7z, RAR etc) are obsolete, when used for repeated backups (daily etc), compared to the ZPAQ technology, that maintain "snapshots" (versions) of the data. [This is even more true in the case of ASCII dumps of databases (e.g. MySQL/MariaDB)](https://github.com/fcorbelli/zpaqfranz/wiki/Real-world:-SQL-dumps-(MySQL-MariaDB-Postgres-backup))

Let's see.
Archiving a folder multiple times (5), simulating a daily run Monday-to-Friday, with 7z

https://user-images.githubusercontent.com/77727889/215149589-0f2d9f91-ea5a-4f60-b587-f2a506148fe9.mp4

Same, but with zpaqfranz

https://user-images.githubusercontent.com/77727889/215148702-edb8e5bb-8f4e-42bb-9637-6ee98742318a.mp4

_As you can see, the .7z "daily" 5x backups takes ~ 5x the space of the .zpaq_

![compare](https://user-images.githubusercontent.com/77727889/215150599-83032cc6-06b0-432d-ba3b-b410698e3631.jpg)

## Seeing is believing ("real world")

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


**No complex (and fragile) repository folders, with hundreds of "whatever", just only a single file!**  

## Windows client? Minimum size (without software) VSS backups

_It is often important to copy the %desktop% folder, Thunderbird's data, %download% and generally the data folders of a Windows system, leaving out the programs_

Real speed (encrypted) update of C: without software (-frugal)  

https://user-images.githubusercontent.com/77727889/215269540-8e2c8641-0d3a-4f67-a243-ab617834c5de.mp4

## Are you a really paranoid Windows user (like me)? You can get sector-level copies of C:, too.

_In this case the space used is obviously larger, as is the execution time, but even the "most difficult" folders are also taken. Deliberately the bitmap of occupied clusters is ignored: if you are paranoid, be all the way down!_  

_It is just like a dd. You can't (for now) restore with zpaqfranz. You have to extract to a temporary folder and then use other software (e.g., 7z, OSFMount) to extract the files directly from the image_

Accelerated speed (encrypted) every-sector update of a 256GB C: @ ~150MB/s

https://user-images.githubusercontent.com/77727889/215271199-94400833-f973-41d2-a018-3f2277a648a9.mp4


### To date, there is no software, free or paid, that matches this characteristics  
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

As the Russians (and Italians) say, trust me, but check.

**Archiving data requires safety. How can I be sure that I can then extract them without problems?**

It is precisely the portion of the program that I have evolved, implementing a barrage of controls up to the paranoid level, and more.
Let's say there are verification mechanisms which you have probably never seen. Do you want to use SHA-2/SHA-3 to be very confident? You can.

Accelerated speed of real world testing of archive >1GB/s

https://user-images.githubusercontent.com/77727889/215271989-5a77e1f1-8fba-422b-9e25-24c3f4640eb2.mp4


**ZPAQ (zpaqfranz) allows you to NEVER delete the data that is stored and will be available forever (in reality typically you starts from scratch every 1,000 or 2,000 versions, for speed reasons, on HDD. 10K+ on SSD), and restore the files present to each archived version, even if a month or three years ago.**


Real-speed updating (on QNAP NAS) of a small server (300GB); ~7GB of Thunderbird mbox become ~6MB (!) in ~4 minutes. 

https://user-images.githubusercontent.com/77727889/215268613-e07e385c-0880-4534-ae35-0db8925cee6b.mp4

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

This is the key: you don't have to use complex "pipe" of tar | srep | zstd | something hoping that everything will runs file, but a single ~4MB executable, with 7z-like commands.  
You don't even have to install a complex program with many dependencies that will have to read a folder (the repository) with maybe thousands of files, hoping that they are all fully functional.

There are also many great features for backup, I mention only the greatest.  
**The ZPAQ file is "in addition", it is never modified**

So rsync --append will copy only the portion actually added, for example on ssh tunnel to a remote server, or local NAS (QNAP etc) with tiny times.  
TRANSLATION  
You can pay ~$4 a month for 1TB cloud-storage-space to store just about everything

You don't have to copy or synchronize let's say 700GB of tar.gz,7z or whatever, but only (say) the 2GB added in the last copy, the first 698GB are untouched.

This opens up the concrete possibility of using VDSL connections (upload ~ 2/4MB /s) to backup even virtual servers of hundreds of gigabytes in a few minutes.

In this (accelerated) video the rsync transfer of 2 remote backups: "standard" .zpaq archive (file level) AND zfsbackup (bit-level) for a small real-world server 1 day-update of work 

https://user-images.githubusercontent.com/77727889/215267855-22bf875c-90ee-47d1-8f8f-c2d0fa2ab201.mp4


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

**Very hard to use?**  
It is a tool for power users and administrators, who are used to the command line. A text-based GUI is being developed to make data selection and complex extraction easier (!).  

In this example we want to extract all the .cpp files as .bak from the 1.zpaq archive. This is something you typically cannot do with other archives such as tar, 7z, rar etc.  
### With a "sort of" WYSIWYG 'composer' 
First **f** key (find) and entering .cpp  
Then **s** (search) every .cpp substring  
Then **r** (replace) with .bak  
Then **t** (to) for the z:\example folder  
Finally **x** to run the extraction  

https://user-images.githubusercontent.com/77727889/226925740-d62b92ae-4eee-43ac-94a9-e1a6dae684c1.mp4


**I do not trust you, but I am becoming curious. So?**

On **FreeBSD** [you can try to build the port (of paq, inside archivers)](https://www.freshports.org/archivers/paq) but it is very, very, very old (v 6.57 of 2014)  
You can get a "not too old" zpaqfranz with a `pkg install zpaqfranz`

On **OpenBSD** `pkg_add zpaqfranz` is usually rather updated

On **Debian** [there is a zpaq 7.15 package](https://packages.debian.org/sid/utils/zpaq)  
You can download the original version (7.15 of 2016) directly from the author's website, and compile it, or get the same from github.  
In this case be careful, because the source is divided into 3 source files, but nothing difficult for the compilation.  

**OK, let's assume I want to try out zpaqfranz. How?**  

From branch 51 all source code is merged in one zpaqfranz.cpp aiming to make it as easy as possible to compile on "strange" systems (NAS, vSphere etc).  
Updating, compilation and Makefile are now trivial.  

# How to build

My main development platforms are INTEL Windows (non-Intel Windows (arm) currently unsupported) and FreeBSD.

I rarely use Linux or MacOS or whatever (for compiling), so fixing may be needed.

As explained the program is single file, be careful to link the pthread library.  
You need it for ESXi too, even if it doesn't work. Don't be afraid, zpaqfranz knows!

### [Almost "universal" (minimal) Makefile](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Makefile)
### [Quicker and dirtier!](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-quicker%E2%80%90and%E2%80%90dirtier)
### [DEFINEs at compile-time](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-How-to-Build)
### [TARGET EXAMPLES](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Target-examples)
### [HIDDEN GEMS](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Hidden-gems)
### [STRANGE THINGS](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Strange-Things)


### [Integrated HELP](https://github.com/fcorbelli/zpaqfranz/wiki/HELP-integrated)
### [Wiki commands](https://github.com/fcorbelli/zpaqfranz/wiki/Command)
