## [58.10.f] - 2023-09-12
### Fix
- PowerPC fix

## [58.10.e] - 2023-09-06
### Added
- New switch -home in a command (add)
  
## [58.10.a] - 2023-09-06
### Changed
- Stripped of about everything from zpaqfranz repository
- Moved to zpaqfranz-stuff (for license reason when 'packaged')
- the subfolders, much smaller git
- Fixed some typo in zpaqfranz.cpp source code, added some greetings

## [58.9.a] - 2023-08-16
### Changed
- This `CHANGELOG.md` file.


### 26-07-2022: 55.6
### Support for HW accelerated SHA1 instructions (Windows 64)  
_Thanks to Igor Pavlov  (7-Zip's author) I introduce a new EXE: **zpaqfranzhw.exe** with the new switch **-hw**  to use (if any) CPU's HW SHA1 instructions.  
Not very "Intel friendly" (only a few Intel models), but more common in the AMD field: the Ryzen processors implement them (AFAIK) since 2017
The difference in overall performance is modest.  
However, if you have a Ryzen CPU (like me) ... just try and DO NOT forget the **-hw** :)_

![ryzen cpu](http://www.francocorbelli.it/ryzen.jpg)

### More advanced errors (Windows)
_Filesystems error (access denied etc) are now kept (on Windows) and written briefly (-verbose) or in detail (-debug) after **add()**_ 
_In case of error zpaqfranz try to get filesystem's attributes, very helpful for debug (aka: send me!)_
_In this example it's very clear "why"_
**-verbose**
```
File errors report
Error 00000032 #        1 |sharing violation        |
```
**-debug**
```
File errors report
Error 00000032 #        1 |sharing violation        |
c:/pagefile.sys>>
00000026 ARCHIVE;HIDDEN;SYSTEM;
```

### Refactored help
_The "no command" is now this (a bit tighter). Of course Windows and Non-Windows are different_  
![ryzen cpu](http://www.francocorbelli.it/nocommand.jpg)  
_The "help on help" (can be asked by zpaqfranz /?    zpaqfranz h    zpaqfranz -h etc) is *more concise, but more readable*_  
![ryzen cpu](http://www.francocorbelli.it/command.jpg)


### Commands "kind of C: backup" g/q refined a bit
Default exclusions are:
- c:\windows
- RECYCLE BIN
- %TEMP%
- ADS
- .zfs
- pagefile.sys
- swapfile.sys
- System Volume Information
- WindowsApps (maybe)

_Switches_
**-frugal**  
_Exclude program files and program files x86_

**-forcewindows**
_Takes C:\WINDOWS, $RECYCLE.BIN, %TEMP%, ADS and .zfs_

**-all**
_All EXCEPT pagefile, swapfile, system volume information, WindowsApps_


### Starting of reparsepoint support
_Windows have a LOT of strange files (for example in WindowsApps folder), work in progress..._


### 20-07-2022: 55.4
### Command q (paQQa) on Windows: archive ("fake backup") of drive C
_The new command archive (a large part) of the C: drive, excluding, by default, the swap files, system volume information, and the Windows folder  
**Admin rights are required, and the C:\FRANZSNAP FOLDER MUST NOT EXISTS  
EVERY KIND OF PRE-EXISTING VSS WILL BE DELETED**_  

_Usage is quite easy_
```
zpaqfranz q z:\pippo.zpaq
```
_the -forcewindows add the c:\windows folder, the %TEMP% and ADS  
Almost all add() switches are enabled, with the exception of -to_  

```
zpaqfranz q z:\pippo.zpaq -only *.cpp -key migthypassword
```


**This is NOT a bare-metal restorable backup but a (blazingly fast) snapshotted/versioned archiving of precious data**  
_On my PC updating the "backup" (aka: new version) takes less than 2 minutes for ~200GB C: drive_  
_Note: some special folders cannot be accessed/archived at all (Windows's Defender, CSC etc.), because Windows does not like. I am working on it_


### 16-07-2022: 55.2
### Command r (robocopy) MAYBE works with -longpath on Windows (caution using r)
### Some minor fixes in help
### Source code in "Unix" text format (no CR/LF) for easy ancient make compatibility
---

### 15-07-2022: 55.2
### Some fixes on ETA computations
### A little bit better handling of verify on legacy 7.15 archives
### -verbose shows individual % of files bigger than 100.000.000 (10%) or 1.000.000.000 (1%)
_One of the problems with zpaq is that, while updating a large, little changed file, there is no response: the program seems to hang  
This is evident, in particular, for virtual machine disks, hundreds of GBs each, in which perhaps only a few MB need to be added to the archive.
In this case you get zero feedback maybe for an hour, then 5 seconds of ETA, then all is done.  
So now, using the -verbose switch, you get a progress indicator on big files (nothing on smaller one to reduce console output time)_
### -touch switch on command a add()
_Converting a legacy 7.15 archive to zpaqfranz (aka: saving CRC-32s and hashes for every file) IN PLACE (=without extracting/repack etc) is not trivial and potentially, it is a dangerous thing to do.  
As a workaround I implemented the -touch switch that, during the add phase, "fakely" change the timestamp of the files.  
Therefore IF you own the original files, you can switch from 7.15 to zpaqfranz in two steps  
Suppose you have a **z:\1.zpaq** file, created by zpaq 7.15, containing years of backups WITHOUT **CRC-32**. Now you want to use zpaqfranz to get better testing features. The data folder is **c:\nz** and is available_

```
zpaqfranz a z:\1.zpaq c:\nz\ -touch
```
_This will force to add ANYTHING from **c:\nz** into **z:\1.zpaq**. Lenghty, but litte space needed. Then_
```
zpaqfranz a z:\1.zpaq c:\nz\
```
_This time **without -touch**. Add, again, everything, BUT now with "true" timestamps.  
Yes, I know, it is rather weird, but the complexity (on the source code) is minimal.  
It is actually possible to carry out this operation WITHOUT having the data archive available, and in a much faster way. However, as it is normally a one-off operation, it is enough for me_

---

### 09-07-2022: 55.1
This new release introduce... new features  
# As all new code should be deeply tested, before production use    

### Compile on OpenBSD 6.6+ and OmniOS (open Solaris Unix) r151042  
_Not very common, but I like Unix more than Linux_  

### (Windows) sfx module updated
_Now, if no -to is specified, it asks from console where to extract the data_

### General behaviour
_Show less infos during execution. It is a working in progress    
Switches to REDUCE output are -noeta, -pakka and -summary  
Switches to INCREASE output are -verbose and -debug  
Advancing by 1s in progress infos_  

### The "dir" command, by default, show dates in European format
_Instead of 2022-12-25 => 25/12/2022  
It does NOT use "local" translator, because it's way too hard to support many platforms  
-utc turn off local time (just like 7.15)_  

### -flagflat use mime64 encoded filenames
_In some cases there were problems with reserved words on NTFS filesystems_  

### On Windows more extensive support for -longpath (filenames longer than 255 chars)

### On Windows -fixcase and -fixreserved  
_Handle case collisions (ex. pippo.txt and PIPPO.txt) and reserved filenames (ex lpt1)_  

### Disabled some computation. Use -stat to re-activate  
_Some checks can be slow for huge archives (reserved filenames, collisions etc)_

### The -all switch, for turning on multi-thread computation, is now -ssd
_In zpaqfranz 55- -all can means "all versions" or "all core". On 55+ (zpaqfranz's extensions) for multithread read/write use -ssd_  

### command d
_It is now possible to use different hashes  
zpaqfranz d c:\dropbox\ -ssd -blake3_  

### command dir
_It is possible now to use different hashes to find duplicates  
zpaqfranz dir c:\dropbox\ /s -checksum -blake3_  

### Main changes command a (add)
_-debug -zero       Add files but zero-filled (debugging)_  
_If you want to send me some kind of strange archive, for debug, without privacy issues_  

_-debug -zero -kill Add 0-byte long file (debugging)_  
_Store empty files, very small archive, to test and debug filesystem issues_  

_-verify       Verify hashes against filesystem_  
_-verify -ssd  Verify hashes against filesystem MULTITHREAD (do NOT use on spinning drives)_

### Command i (information)
_Is now about 30% faster_  
_-stat to calc statistics on files, like case collisions (slow)_  

### On Windows: new command rd()
_Delete hard-to-erase folders on Windows  
-force  Remove folder if not-zero files present  
-kill wet-run  
-space do not check writeability (example delete folder from a 0 bytes free drive)_  

# And now... the main thing!
### command w Chunked-extraction  
_Extract/test in chunks, on disk or 'ramdisk' (RAM)  
**The output -to folder MUST (should) BE EMPTY**  
The w command essentially works like the x (extract) command but in chunks  
It can extract the data into RAM, and to simply check (hash) it, or even write it in order_  

**PRELIMINARY NOTE AGAIN: the -to folder MUST BE EMPTY (no advanced checks-and-balance as zpaq)**    

There are various scenarios

1) **Extracting on spinning drive (HDD)**  
zpaq's extraction method is not very fit for spinning drive, because can make a lot of seeks while writing data to disk. This can slow down the process. On media with lower latency (SSD, NVMe, ramdisk) the slowdown is much less noticeable. Basically zpaq "loves" SSD  and isn't very HDD friendly.
If the largest file in the archive is smaller than the free RAM on your computer, you can use the -ramdisk switch
Extract to a spinning drive (Windows)
zpaqfranz w z:\1.zpaq -to p:\muz7\ -ramdisk -longpath
In this example the archive 1.zpaq will be extracted in RAM, then written on p:\muz7 folder (suppose an HDD drive) at the max speed possible (no seeks at all)

2) **Checking the hashes without write on disk AND MULTITHREAD AND MULTIPART and whatever**
The p (paranoid) command can verify the hash checksum of the files into the archive WITHOUT writing to disk, but it is limited to a SINGLE CORE computation, without multipart archive support
The w command (if the largest file in the archive is smaller than free RAM) does not have such limitations
zpaqfranz w z:\1.zpaq -ramdisk -test -checksum -ssd -frugal -verbose
will test (-test, no write on drive) the archive 1.zpaq, into RAM (-ramdisk), testing hashes (-checksum), in multithread way (-ssd), using as little memory as possible (-frugal) and in -verbose mode

3) **Paranoid no-limit check, for huge archives (where the largest uncompressed file is bigger than free RAM)**
zpaqfranz w z:\1.zpaq -to z:\muz7\ -paranoid -verify -verbose -longpath
will extract everything from 1.zpaq into z:\muz7, with longpath support (example for Windows), then do a -paranoid -verify
At the end into z:\muz7\zfranz the BAD files will be present. If everything is ok this folder should be empty

4) **Paranoid test-everything, when the biggest uncompressed file is smaller then RAM and using a SSD/NVMe/ramdisk as output (z:\)**
zpaqfranz w z:\1.zpaq -to z:\kajo -ramdisk -paranoid -verify -checksum -longpath -ssd


Recap of switches
+ : **-maxsize X**    Limit chunks to X bytes
+ : **-ramdisk**      Use 'RAMDISK', only if uncompressed size of the biggest file (+10%) is smaller than current free RAM (-25%)
+ : **-frugal**       Consume as litte RAM as possible (default: get 75% of free RAM)
+ : **-ssd**          Multithread writing from ramdisk to disk / Multithread hash computation (ex -all)
+ : **-test**         Do not write on media
+ : **-verbose**      Show useful infos (default: rather brief)
+ : **-checksum**     Do CRC-32 / hashes test in w command (by default: NO)
+ : **-verify**       Do a 'check-against-filesystem'
+ : **-paranoid**     Extract "real" to filesystem, then delete if hash OK (need -verify)  

### Yes, I understand, the w command can seems incomprehensible.  
In fact it is developed to avoid the limitations of zpaq in the management of very large archives with huge files (for example virtual machine disks) kept on spinning HDD or handling archives containing a very large number (millions) of relatively small files (such as for example the versioned backup of a file server full of DOC, EML, JPG etc), to be checked on a high-powered machine (with many cores, lots of RAM and SSD), without wearing the media. As is known, writing large amounts of data reduces the lifespan of SSDs and NVMes (HDDs too, but to a lesser extent). And remember: the **p** command is monothread AND cannot handle archive bigger than RAM.  
To make a "quick check" of the "spiegone" try yourself: compare the execution time on a "small" archive (=uncompressed size smaller than your RAM, say 5/10GB for example)  
zpaqfranz p **p:\1.zpaq**  
against  
zpaqfranz w **p:\1.zpaq** -test -checksum -ramdisk -ssd -verbose  


### One last thing: 55.1 is developed and tested on Windows. More developing for BSD/Linux from 55.2+. And remember: the embedded help (and examples) are just about always updated







```
20-12-2021: 54.10  
media full check  
franzomips  
-checksum in test

04-11-2021: 54.9  
-nilsimsa
-nodedup
-desc
-tar
-orderby 
-desc
-flagappend (robocopy)
-minsize (s command)

06-10-2021: 54.8
New purgesync command, exec_warn, support for "strange things", .ppt with .xls, fix, changed -kill to -zero in extract

25-09-2021: 54.7
Support (maybe!) for ReFS-deduplicated VHDX, verify now supports multithread (-all)

18-09-2021: 54.6
Refactoring, some fixes

08-09-2021: 52.3
SFX module (for Windows) now support encrypted archives

07-09-2021: 52.2
SFX module for Win32 too, LZ4-compressed

06-09-2021: 54.1
SFX module for Win64, some fixes

31-08-2021: 52.21
Some fixes, less RAM usage, Solaris port

21-08-2021: 52.19
-paranoid in test

20-08-2021: 52.18
-sha3 (SHA3-256)
-md5  (MD5)

17-08-2021: 52.17

New switch for add()
- freeze outputfolder  -maxsize something
zpaqfranz a z:\1.zpaq c:\nz\ -freeze y:\archived -maxsize 2000000000
If the archive (z:\1.zpaq) is bigger than the -maxsize limit (2GB), it will be MOVED (before the execution) to the -freeze folder (the y:\archived directory, so **y:\archived\1.zpaq**, **y:\archived\1_000001.zpaq**, **y:\archived\1_000002.zpaq** ...)   **without** overwrite.  

HW acceleration for BLAKE3 on Windows 64
zpaqfranz use 1 of 4 implementations (if supported), SSE2 SSE41, AVX2, AVX512 for the BLAKE3 hasher.
it is not a very tested version, I use it mainly to compare performance with the Rust version
Fast check
zpaqfranz b -blake3 -sha1
On SW implementation (zpaqfranz <=52.16) BLAKE3 run slower then SHA-1 (about 2/3)  
You can see here
zpaqfranz v52.17-experimental **with HW BLAKE3**, compiled Aug 17 2021
Usage: zpaqfranz command archive[.zpaq] files|directory... -switches...

12-08-2021: 52.16  
zfslist, zfspurge, zfsadd

06-08-2021: 52.14
in add()   
```
-exec_ok something.bat (launch a script with archivename as parameter)
-copy somewhere (write a second copy of the archive into somewhere)

Add 2nd copy to USB drive (U):       a "z:\f_???.zpaq" c:\nz\ -copy u:\usb
Launch pippo.bat after OK:           a "z:\g_???.zpaq" c:\nz\ -exec_ok u:\pippo.bat
```

03-08-2021: 52.13
New hasher -whirlpool
new commands zfs

31-07-2021: 52.12
In the command a (add) it is now possible to use -blake3 as file integrity checker

30-07-2021: 52.11
The CPU cooker: in the b command (benchmark) now -all (multithread) and -tX (limit to X thread)  

29-07-2021
Very rough benchmark command for hashes-checksums test.  
By default runs 5 seconds each on 390.62 (-7) KB  
To be developed in future (in fact it is a BLAKE3 mock up test)
YES I KNOW, THOSE ARE REALLY CRUDE BENCHMARKS!

zpaqfranz b
zpaqfranz -? b

25-07-2021:  52.9
A little more faster in -sha256 (2%)
TWO TIME faster in multithreaded -sha1. Up to 6.7 GB/s on my test PC
Implemented, just for fun, the -wyhash running on memory mapped files (not so fast, of course)  

21-07-2021:  52.8
Support for storing XXHASH64 as default, plus a fix for XXH3 on "strange" compilers

19-07-2021:  52.5 
Support for storing SHA-256 for each file.
New switches

zpaqfranz a z:\1.zpaq c:\nz -xxhash (the default)
zpaqfranz a z:\1.zpaq c:\nz -sha1
zpaqfranz a z:\1.zpaq c:\nz -sha256
zpaqfranz a z:\1.zpaq c:\nz -crc32
zpaqfranz a z:\1.zpaq c:\nz -nochecksum (like 7.15)



11-07-2021:  52.1
New switch -filelist
zpaqfranz a z:\1.zpaq c:\nz -filelist
zpaqfranz x z:\1.zpaq -filelist

New behavior: store by default XXH3 and CRC-32 of every file (to be completed)

New switches -checksum and -summary in list

zpaqfranz l z:\1.zpaq -checksum (show CRC and hash if any)
zpaqfranz l z:\1.zpaq -checksum -summary (compact output)

02-07-2021:  51.43 n command (decimation)

26-06-2021:  51.41 help refactoring (splitted map)

20-06-2021:  51.36 help refactoring

16-06-2021: 51.33
            commands r (robocopy) and d (deduplication)
	    
06-06-2021: 51.31
            -715 Works just about like v7.15
	    
22-05-2021: 51.23
            First "working" stage of -utf, -fix255, fixeml
	    
18-05-2021: 51.22
            A lot of work, experimental extraction of too long filename (and many more)

05-04-2021: 51.10
            New branch: single source code (zpaqfranz.cpp), to make it easier to package for FreeBSD and later Linux
	    
03-04-2021: 50.22
            With sha1 ... -all runs hash in multithread (DO NOT USE ON MAGNETIC DISKS!)
	    With sha1 ... -verify compute XXH3 of the entire tree

01-04-2021: 50.21
            Use XXH3 (128 bit version) instead of xxhash64
	    Refined -noeta -pakka in sha1()
	    
29-03-2021: 50.19
            New "hidden" command kill
	    Fixed a bug extracting files with -force over existing (but different) ones
	    
26-03-2021: 50.18
            Fixed some aesthetic detail
22-03-2021: 50.17
            Fixed the "magical" help parser
			
			+Added -minsize X and -maxsize Y on the add(), list() and sha1() functions
			       zpaqfranz a z:\1.zpaq c:\nz -maxsize 1000000
			
			+Added exec_on something / exec_error something
					execute (system()) a file after successfull (all OK) or NOT (errors/warnings)
					Typically this is for sending an error e-mail alert message
					Something like (with smtp-cli)
					
					smtp-cli 
					--missing-modules-ok 
					-verbose 
					-server=mail.mysmtp.com 
					--port 587 
					-4 
					-user=log@mysmtp.com 
					-pass=mygoodpassword
					-from=log@mysmtp.com
					-to log@mysmtp.com 
					-subject  "CHECK-ZPAQ" 
					-body-plain="Something wrong" 
					-attach=/tmp/checkoutput.txt
```
