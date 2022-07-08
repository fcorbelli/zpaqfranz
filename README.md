# zpaqfranz: advanced and compatible fork of ZPAQ 7.15, with SFX (on Windows)  
### Main platforms: Windows, FreeBSD, Linux
Secondary platforms: Solaris, MacOS, OpenBSD, OmniOS, (ESXi, QNAP-based NAS for older builds)  

# New Release: 55.1 (July 2022)

### [Windows binary 32/64 bit on sourceforge](https://sourceforge.net/projects/zpaqfranz/files/) 
### [Quick start FreeBSD](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart:-FreeBSD)   
[Main site of old ZPAQ](http://mattmahoney.net/dc/zpaq.html)      [Reference decompressor](https://github.com/fcorbelli/unzpaq/tree/main) 



## Classic archivers (tar, 7z, RAR etc) are obsolete, when used for repeated backups (daily etc), compared to the ZPAQ technology, that maintain "snapshots" (versions) of the data.

[Wiki being written - be patient](https://github.com/fcorbelli/zpaqfranz/wiki)  
[Quick link to ZFS's snapshots support functions](https://github.com/fcorbelli/zpaqfranz/wiki/Command:-zfs(something))

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

Because mr. Mahoney is now retired and no longer supports it (he... run!)

**Why should I trust? It will be one of 1000 other programs that silently fail and give problems**

As the Russians say, trust me, but check.

**Archiving data requires safety. How can I be sure that I can then extract them without problems?**

It is precisely the portion of the program that I have evolved, implementing a barrage of controls up to the paranoid level, and more.
Let's say there are verification mechanisms which you have probably never seen. Do you want to use SHA-2/SHA-3 to be very confident? You can.

**Why do you say 7z, RAR etc are obsolete? How is ZPAQ so innovative?**

Let's see.
I want to make a copy of /etc

a=add
/tmp/etccopy.zpaq = the backup file
/etc = the folder to be included
```
root@aserver:~ # zpaqfranz a /tmp/etccopy.zpaq /etc
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
Creating /tmp/etccopy.zpaq at offset 0 + 0
Adding 10.496.025.943 (9.77 GB) in 396 files  at 2021-05-29 12:17:22
 99.69% 0:00:00 (  9.74 GB) -> (270.45 MB) of (  9.77 GB)  91.54 MB/secc
427 +added, 0 -removed.

0 + (10.496.025.943 -> 1.576.376.565 -> 287.332.378) = 287.332.378

126.154 seconds (all OK)
```

In this case the 10GB become 1.5 (after deduplication) and 287MB (after compression): it doesn't matter how small the file gets - there are better compressors.

OK now edit the /etc/rc.conf
```
vi /etc/rc.conf
...
```

then, again
```
root@aserver:~ # zpaqfranz a /tmp/etccopy.zpaq /etc
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
/tmp/etccopy.zpaq:
1 versions, 427 files, 21.551 fragments, 287.332.378 bytes (274.02 MB)
Updating /tmp/etccopy.zpaq at offset 287332378 + 0
Adding 469 (469.00 B) in 1 files  at 2021-05-29 12:24:19

1 +added, 0 -removed.

287.332.378 + (469 -> 469 -> 1.419) = 287.333.797

0.039 seconds (all OK)
```

Please look at the time (0.039s), and filesize.
It's now 287.333.797 bytes.

OK, let's edit rc.conf again
```
vi /etc/rc.conf
...
```
then again
```
root@aserver:~ # zpaqfranz a /tmp/etccopy.zpaq /etc
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
/tmp/etccopy.zpaq:
2 versions, 428 files, 21.552 fragments, 287.333.797 bytes (274.02 MB)
Updating /tmp/etccopy.zpaq at offset 287333797 + 0
Adding 468 (468.00 B) in 1 files  at 2021-05-29 12:25:36

1 +added, 0 -removed.

287.333.797 + (468 -> 468 -> 1.418) = 287.335.215

0.038 seconds (all OK)
```

So now we have a single archive, with the THREE versions of /etc/rc.conf
Lets see
l = list
-all = show all versions
|tail = take only last lines
```
root@aserver:~ # zpaqfranz l /tmp/etccopy.zpaq -all |tail

0.029 seconds (all OK)
- 2018-10-20 17:00:48               2.151  0644 0001|CRC32: 3EB2AD64 /etc/ttys
- 2017-08-10 14:53:46                   0  0444 0001|CRC32: 00000000 /etc/wall_cmos_clock
- 2017-08-08 16:48:55                   0 d0755 0001|/etc/zfs/
- 2017-08-08 16:48:55                   0  0644 0001|CRC32: 00000000 /etc/zfs/exports
- 2021-05-29 12:24:19                   0       0002| +1 -0 -> 1.419
- 2021-05-29 12:23:59                 469  0644 0002|CRC32: D3629FF0 /etc/rc.conf
- 2021-05-29 12:25:36                   0       0003| +1 -0 -> 1.418
- 2021-05-29 12:25:33                 468  0644 0003|CRC32: 5B47757C /etc/rc.conf

10.496.026.880 (9.77 GB) of 10.496.026.880 (9.77 GB) in 432 files shown
```

As you can see 0002 (version two) have a different /etc/rc.conf, 0003 (versione three) have a third /etc/rc.conf

```
root@aserver:~ # zpaqfranz l /tmp/etccopy.zpaq -all -comment
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
franz:use comment
/tmp/etccopy.zpaq:
3 versions, 429 files, 21.553 fragments, 287.335.215 bytes (274.02 MB)

Version comments enumerator
------------
00000001 2021-05-29 12:17:22  +00000427 -00000000 ->          287.332.378
00000002 2021-05-29 12:24:19  +00000001 -00000000 ->                1.419
00000003 2021-05-29 12:25:36  +00000001 -00000000 ->                1.418
```

So we get 3 "snapshots" (or versions).
Of course we can have 300, or 3.000 versions.
I can restore the version 2 (the "second snapshot")

x= extract
/tmp/etccopy.zpaq = archive name
/etc/rc.conf = I want to get only /etc/rc.conf
-to /tmp/restoredrc-2.conf = name the file restored
-until 2 = get the second version
```
root@aserver:~ # zpaqfranz x /tmp/etccopy.zpaq /etc/rc.conf -to /tmp/restoredrc_2.conf -until 2
zpaqfranz v51.10-experimental journaling archiver, compiled Apr  5 2021
/tmp/etccopy.zpaq -until 2:
2 versions, 428 files, 21.552 fragments, 287.333.797 bytes (274.02 MB)
Extracting 469 bytes (469.00 B) in 1 files -threads 16

0.029 seconds (all OK)
```


Now look at the restored rc.conf
```
root@aserver:~ # head /tmp/restoredrc_2.conf
#second version
hostname="aserver"
keymap="it"
ifconfig_igb0="inet 192.168.1.254 netmask 0xffffff00"
defaultrouter="192.168.1.1"
sshd_enable="YES"
ntpd_enable="YES"
ntpd_sync_on_start="YES"
# Set dumpdev to "AUTO" to enable crash dumps, "NO" to disable
dumpdev="NO"
```

**Therefore ZPAQ (zpaqfranz) allows you to NEVER delete the data that is stored and will be available forever (in reality typically you starts from scratch every 1,000 or 2,000 versions, for speed reasons), and restore the files present to each archived version, even if a month or three years ago.**

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
00000581 2019-07-12 17:13:52  +00000410 -00000031 ->          172.390.369
00000582 2019-07-13 17:13:49  +00000103 -00000093 ->           50.633.900
00000583 2019-07-14 17:13:44  +00000001 -00000000 ->           17.068.608
00000584 2019-07-15 17:14:03  +00000327 -00000005 ->          105.520.572
00000585 2019-07-16 17:13:54  +00000364 -00000018 ->          128.560.509
00000586 2019-07-17 17:14:03  +00000356 -00000032 ->          161.728.363
(...)
00000672 2019-11-17 11:51:36  +00002712 -00001401 ->        9.712.799.579
00000673 2019-11-21 18:27:07  +00001179 -00054945 ->          658.766.521
00000674 2019-11-21 19:04:12  +00000002 -00000000 ->                2.096
00000675 2019-11-28 18:17:48  +00001201 -00000253 ->          998.527.198
00000676 2019-11-29 18:13:14  +00000244 -00000014 ->          111.502.243
00000677 2019-12-02 18:14:19  +00000207 -00000075 ->           87.855.054
00000678 2019-12-03 18:12:01  +00000299 -00000078 ->          106.043.128
00000679 2019-12-04 18:12:12  +00000322 -00000005 ->          177.800.387
(...)
00000769 2020-04-15 17:12:51  +00000012 -00000000 ->              278.407
00000770 2020-04-16 17:13:00  +00000028 -00000000 ->            5.355.373
00000771 2020-04-17 17:13:00  +00000010 -00000001 ->              129.334
(...)
00000779 2020-04-25 17:13:07  +00000001 -00000000 ->              743.346
00000780 2020-04-26 17:13:03  +00000001 -00000000 ->              109.083
00000781 2020-04-27 17:12:42  +00000439 -00000366 ->           26.002.842
00000782 2020-04-28 17:13:38  +00000762 -00000627 ->          159.137.512
(...)
00000918 2020-11-10 18:16:14  +00000450 -00000059 ->        2.453.075.624
00000919 2020-11-11 18:15:25  +00000354 -00000061 ->          185.478.412
00000920 2020-11-12 18:15:33  +00000274 -00000059 ->           74.152.731
(...)
00000927 2020-11-23 18:16:24  +00002176 -00001826 ->          380.446.024
00000928 2020-11-24 18:15:51  +00000364 -00000057 ->          322.920.107
00000929 2020-11-25 18:15:53  +00000298 -00000030 ->          595.760.380
(...)
00000972 2021-02-19 18:17:52  +00000331 -00000082 ->        9.936.198.948
00000973 2021-02-20 18:12:10  +00000001 -00000001 ->                  628
00000974 2021-02-22 18:13:31  +00000324 -00000115 ->          234.014.447
00000975 2021-02-23 18:12:45  +00000357 -00000029 ->          298.551.067
00000976 2021-02-24 18:13:07  +00000609 -00000047 ->          507.154.350
00000977 2021-02-25 18:19:47  +00031694 -00000023 ->        3.673.844.953
(...)
00000983 2021-03-04 18:13:18  +00000320 -00000015 ->          261.091.829
00000984 2021-03-05 18:13:17  +00000482 -00000071 ->          596.715.880
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

You don't have to copy or synchronize let's say 700GB of tar.gz,7z or whatever, but only (say) the 2GB added in the last copy, the firs 698GB are untouched.

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
*In theory it would be possible to eliminate, but the work is huge and, frankly, I rarely use magnetic discs*

No other significant ones come to mind, except that it is known and used by few

**I do not trust you, but I am becoming curious. So?**

On **FreeBSD** [you can try to build the port (of paq, inside archivers)](https://www.freshports.org/archivers/paq) but it is very, very, very old (v 6.57 of 2014)  
On **Debian** [there is a zpaq 7.15 package](https://packages.debian.org/sid/utils/zpaq)  
You can download the original version (7.15 of 2016) directly from the author's website, and compile it, or get the same from github.  
In this case be careful, because the source is divided into 3 source files, but nothing difficult for the compilation.  

**OK, let's assume I want to try out zpaqfranz. How?**  
From branch 51 all source code is merged in one zpaqfranz.cpp aiming to make it as easy as possible to compile on "strange" systems (NAS, vSphere etc).  
Updating, compilation and Makefile are now trivial.  

_So be patient if the source is not linear, composed by the fusion of different software from different authors, without uniform style of programming. 
I have made a number of efforts to maintain compatibility 
with unmodified version (7.15), minimizing warnings, even at the cost 
of expensive on inelegant workarounds._  

Don't be surprised if it looks like what in Italy we call "zibaldone" or in Emilia-Romagna "mappazzone".  

### [Windows binary builds (32 and 64 bit) on github/sourceforge](https://sourceforge.net/projects/zpaqfranz/files/)

## **Provided as-is, with no warranty whatsoever, by Franco Corbelli, franco@francocorbelli.com**


####      Key differences against 7.15 by zpaqfranz -diff or here 
####  https://github.com/fcorbelli/zpaqfranz/blob/main/differences715.txt 
####  [Wiki in progress...](https://github.com/fcorbelli/zpaqfranz/wiki/DIff-against-7.15)

Portions of software by other authors, mentioned later, are included.
As far as I know this is allowed by the licenses. 
**Some text comment stripped due the 1MB limit for github.**  

**I apologize if I have unintentionally violated any rule.**  
**Report it and I will fix as soon as possible.**  
**CREDITS**  

- Include mod by data man and reg2s patch from encode.su forum 
- Crc32.h Copyright (c) 2011-2019 Stephan Brumme 
- xxhash64 by Stephan Brumme https://create.stephan-brumme.com/xxhash/  
- Slicing-by-16 contributed by Bulat Ziganshin 
- xxHash Extremely Fast Hash algorithm, Copyright (C) 2012-2020 Yann Collet 
- crc32c.c Copyright (C) 2013 Mark Adler  
- Embedded Artistry https://github.com/embeddedartistry  
- wyhash (experimental) WangYi  https://github.com/wangyi-fudan/wyhash  
- https://github.com/System-Glitch/SHA256
- https://github.com/BLAKE3-team/BLAKE3 (The C code is copyright Samuel Neves and Jack O'Connor, 2019-2020, the assembly code is copyright Samuel Neves, 2019-2020)
- The Whirlpool algorithm was developed by Paulo S. L. M. Barreto and Vincent Rijmen
- Nilsimsa implementation by Sepehr Laal
- Thanks for testing on various Unixes to https://github.com/dertuxmalwieder



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

**Single help**
It is possible to call -? (better -h for Unix) and -examples with a parameter
a b c d dir f i k l m n r rsync s sfx sum t utf v x z franz main normal voodoo

```
zpaqfranz -? a 
zpaqfranz -h zfs
zpaqfranz -examples x 

```



# How to build
My main development platforms are Windows and FreeBSD. 
I rarely use Linux or MacOS, so changes may be needed.

## WARNING 22-07-2021
_I discovered two different compilation problems on the latest Ubuntu  
[The first is the 64-byte alignment of some structures (for the XXH3 hash)](https://github.com/Cyan4973/xxHash/issues/543) which can fail with some compilers "too new" or others "too old"_  
_So the software works great on Windows an FreeBSD, but can fail on Ubuntu (!)_  
_[The second is a bug (!) in g++ 10.3, the DEFAULT Ubuntu 21 compiler. Yes, a bug recognized in the compiler](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101558)_  
_Some fixes/workaroud are implemented from 52.8+, and the default hasher (in add) is now XXHASH64, and not XXH3 (the 128 bit version)_  
_I'm sorry but, as always explained, I rarely use Linux (almost always Debian) and hardly Ubuntu_  
_News: from 54+ removed the workarouds, sometimes makes side effects. Update your compiler!_  


Be careful to link the pthread library.

_On Win64 from 54+ include a SFX module_  

Targets
```
Windows 64 (g++ 7.3.0)
g++ -O3  zpaqfranz.cpp -o zpaqfranz 

Windows 64 (g++ 10.3.0) MSYS2
g++ -O3  zpaqfranz.cpp -o zpaqfranz -pthread -static

Windows 32 (g++ 7.3.0 64 bit)
c:\mingw32\bin\g++ -m32 -O3 zpaqfranz.cpp -o zpaqfranz32 -pthread -static

Windows 64 (g++, Hardware Blake3 implementation)
In this case, of course, the .S file is mandatory
g++ -O3 -DHWBLAKE3 blake3_windows_gnu.S zpaqfranz.cpp -o zpaqfranz -pthread -static

FreeBSD (11.x) gcc 7
gcc7 -O3 -march=native -Dunix zpaqfranz.cpp -static -lstdc++ -pthread -o zpaqfranz -static -lm

FreeBSD (12.1) gcc 9.3.0
g++ -O3 -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc

FreeBSD (11.4) gcc 10.2.0
g++ -O3 -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static-libstdc++ -static-libgcc -Wno-stringop-overflow

FreeBSD (11.3) clang 6.0.0
clang++ -march=native -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

OpenBSD 6.6 clang++ 8.0.1
clang++ -Dunix -O3 -march=native zpaqfranz.cpp -o zpaqfranz -pthread -static

Debian Linux (10/11) gcc 8.3.0
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -static

QNAP NAS TS-431P3 (Annapurna AL314) gcc 7.4.0
g++ -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz -Wno-psabi

Fedora 34 gcc 11.2.1
Typically you will need some library (out of a fresh Fedora box)
sudo dnf install glibc-static libstdc++-static -y;
Then you can compile, via Makefile or "by hand"
(do not forget... sudo!)

CentoOS
Please note: "Red Hat discourages the use of static linking for security reasons. 
Use static linking only when necessary, especially against libraries provided by Red Hat. "
Therefore a -static linking is often a nightmare on CentOS => change the Makefile
g++ -O3 -Dunix zpaqfranz.cpp  -pthread -o zpaqfranz

Solaris 11.4 gcc 7.3.0
OmniOS r151042 gcc 7.5.0
g++ -O3 -march=native -DSOLARIS zpaqfranz.cpp -o zpaqfranz  -pthread -static-libgcc -lkstat

MacOS 11.0 gcc (clang) 12.0.5
Please note:
The -std=c++11 is required, unless you have to change half a dozen lines. No -static here
"Apple does not support statically linked binaries on Mac OS X. 
(...)
Rather, we strive to ensure binary 
compatibility in each dynamically linked system library and framework
(AHAHAHAHAHAH, note by me)
Warning: Shipping a statically linked binary entails a significant compatibility risk. 
We strongly recommend that you not do this...
Short version: Apple does not like -static, so compile with
g++ -Dunix  -O3 -march=native zpaqfranz.cpp -o zpaqfranz -pthread  -std=c++11

Beware of #definitions
g++ -dM -E - < /dev/null
sometimes __sun, sometimes not


```
