```
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
