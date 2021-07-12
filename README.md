## This is zpaqfranz, a patched  but (maybe) compatible fork of ZPAQ version 7.15 
(http://mattmahoney.net/dc/zpaq.html)

## An archiver, just like 7z or RAR style to understand, with the ability to maintain sort of "snapshots" of the data.

[Wiki being written - be patient](https://github.com/fcorbelli/zpaqfranz/wiki)

At every run only data changed since the last execution will be added, creating a new version (the "snapshot").
It is then possible to restore the data @ the single version.
It shouldn't be difficult to understand its enormous potential, especially for those used to snapshots (by zfs or virtual machines).
Keeps a forever-to-ever copy (even thousands of versions) of your files, conceptually similar to MAC's time machines, 
but much more efficiently.
Ideal for virtual machine disk storage (ex backup of vmdk), virtual disks (VHDx) and even TrueCrypt containers.
Easily handles millions of files and tens of TBs of data.
Allows rsync copies to the cloud with minimal data transfer and encryption: nightly copies with simple FTTC (1-2MB/s upload bandwith) of archives of hundreds of gigabytes.
Multiple possibilities of data verification, fast, advanced and even paranoid.
And much more.
A GUI (PAKKA) is available on Windows to make extraction easier.
## To date, there is no software, free or paid, that matches this characteristics
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
I don't think to talk about it now (too long... read the source!), but let's say there are verification mechanisms which you have probably never seen.

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
00000007 2018-01-10 18:06:42  +00007007 -00000000 ->              613.273
00000008 2018-01-10 18:16:44  +00007007 -00000000 ->              613.273
00000009 2018-01-10 18:23:07  +00007007 -00000000 ->              613.273
00000010 2018-01-10 18:26:25  +00007009 -00000000 ->              615.192
00000011 2018-01-10 19:20:30  +00007007 -00000000 ->              613.273
00000012 2018-01-11 07:00:36  +00007008 -00000000 ->              613.877
(...)
00000146 2018-03-27 17:08:39  +00001105 -00000541 ->          164.399.767
00000147 2018-03-28 17:08:28  +00000422 -00000134 ->          277.237.055
00000148 2018-03-29 17:12:02  +00011953 -00011515 ->          826.218.948
00000149 2018-03-30 17:08:28  +00000396 -00000301 ->          614.932.523
00000150 2018-03-31 17:08:04  +00000003 -00000000 ->           13.228.327
00000151 2018-04-01 17:08:06  +00000003 -00000000 ->           13.228.445
00000152 2018-04-02 17:08:07  +00000003 -00000000 ->           13.229.456
00000153 2018-04-03 17:08:15  +00000362 -00000050 ->           84.812.331
00000154 2018-04-04 17:08:22  +00000330 -00000057 ->          246.051.997
00000155 2018-04-05 17:08:21  +00000334 -00000036 ->           99.834.743
00000156 2018-04-06 17:08:37  +00003058 -00000035 ->          318.541.190
00000157 2018-04-07 17:08:18  +00000003 -00000000 ->           13.353.976
00000158 2018-04-08 17:08:17  +00000003 -00000000 ->           13.353.062
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
00000437 2019-02-12 18:39:07  +00012281 -00012069 ->           92.558.250
00000438 2019-02-13 18:10:26  +00000327 -00000027 ->          156.617.222
00000439 2019-02-14 18:10:33  +00000393 -00000047 ->          201.978.058
00000440 2019-02-15 18:10:21  +00000357 -00000078 ->          137.899.556
00000441 2019-02-16 18:10:19  +00000007 -00000000 ->           15.903.020
00000442 2019-02-17 18:10:16  +00000001 -00000000 ->           15.507.069
00000443 2019-02-18 18:11:06  +00000433 -00000128 ->          804.210.678
00000444 2019-02-19 18:10:31  +00000311 -00000047 ->          394.942.132
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
00000499 2019-04-20 17:11:03  +00000001 -00000000 ->           16.680.076
00000500 2019-04-21 17:11:22  +00000001 -00000000 ->           16.682.634
00000501 2019-04-22 17:11:03  +00000001 -00000000 ->           16.680.028
00000502 2019-04-23 17:11:30  +00000311 -00000029 ->          156.716.433
00000503 2019-04-24 17:11:05  +00000340 -00000040 ->          150.014.131
00000504 2019-04-25 17:11:15  +00000001 -00000000 ->           16.698.751
00000505 2019-04-26 17:11:12  +00000001 -00000000 ->           16.694.826
00000506 2019-04-27 17:11:18  +00000001 -00000000 ->           16.698.196
00000507 2019-04-28 17:11:55  +00000001 -00000000 ->           16.693.376
00000508 2019-04-29 17:11:04  +00000831 -00000470 ->           94.625.724
(...)
00000572 2019-07-02 17:13:53  +00000449 -00000040 ->          108.048.297
00000573 2019-07-03 17:13:55  +00000579 -00000044 ->          400.854.748
00000574 2019-07-04 17:13:52  +00000631 -00000237 ->           91.992.975
00000575 2019-07-05 17:11:37  +00000273 -00000002 ->           48.580.359
00000576 2019-07-06 17:15:10  +00000005 -00000000 ->           17.051.250
00000577 2019-07-07 17:13:41  +00000001 -00000000 ->           17.030.268
00000578 2019-07-08 17:13:39  +00000349 -00000010 ->           85.705.672
00000579 2019-07-10 17:13:47  +00000715 -00000093 ->          321.573.944
00000580 2019-07-11 17:14:08  +00000590 -00000255 ->          634.245.790
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
00000772 2020-04-18 17:13:10  +00000001 -00000000 ->              109.065
00000773 2020-04-19 17:12:49  +00000001 -00000000 ->              109.205
00000774 2020-04-20 17:12:47  +00000287 -00000015 ->           86.857.663
00000775 2020-04-21 17:13:01  +00000124 -00000015 ->           13.069.143
00000776 2020-04-22 17:13:03  +00000173 -00000003 ->           16.582.870
00000777 2020-04-23 17:13:01  +00000119 -00000005 ->            6.720.711
00000778 2020-04-24 17:13:34  +00000145 -00000001 ->           21.905.114
00000779 2020-04-25 17:13:07  +00000001 -00000000 ->              743.346
00000780 2020-04-26 17:13:03  +00000001 -00000000 ->              109.083
00000781 2020-04-27 17:12:42  +00000439 -00000366 ->           26.002.842
00000782 2020-04-28 17:13:38  +00000762 -00000627 ->          159.137.512
(...)
00000918 2020-11-10 18:16:14  +00000450 -00000059 ->        2.453.075.624
00000919 2020-11-11 18:15:25  +00000354 -00000061 ->          185.478.412
00000920 2020-11-12 18:15:33  +00000274 -00000059 ->           74.152.731
00000921 2020-11-13 18:15:25  +00000206 -00000019 ->           40.681.663
00000922 2020-11-16 18:15:12  +00000321 -00000016 ->          162.377.906
00000923 2020-11-17 18:15:22  +00000523 -00000109 ->          423.009.390
00000924 2020-11-18 18:16:03  +00000470 -00001051 ->          497.305.664
00000925 2020-11-19 18:15:32  +00000775 -00000031 ->          554.932.985
00000926 2020-11-20 18:15:09  +00000248 -00000019 ->          125.918.278
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
00000978 2021-02-26 18:14:04  +00000832 -00000529 ->          212.231.488
00000979 2021-02-27 18:12:25  +00000003 -00000000 ->            5.386.009
00000980 2021-03-01 18:13:26  +00000653 -00000293 ->          285.460.026
00000981 2021-03-02 18:13:31  +00000629 -00000287 ->          377.233.510
00000982 2021-03-03 18:13:31  +00000462 -00000096 ->        1.046.609.168
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
00004893 2021-02-18 19:32:59  +00000363 -00000244 ->          142.371.767
00004894 2021-02-19 19:33:55  +00000528 -00000305 ->           69.809.434
00004895 2021-02-20 19:30:18  +00000026 -00000000 ->            7.590.410
00004896 2021-02-21 19:30:18  +00000029 -00000005 ->           11.250.079
00004897 2021-02-22 19:34:15  +00000485 -00000192 ->          167.085.478
00004898 2021-02-23 19:32:17  +00000258 -00000144 ->           37.831.971
00004899 2021-02-24 19:32:40  +00000089 -00000003 ->           73.584.310
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

## Too good to be true? ##

Obviously this is not "magic", it is simply the "chaining" of a block deduplicator with a compressor and an archiver.
There are faster compressors.
There are better compressors.
There are faster archivers.
There are more efficient deduplicators.

But what I have never found is a combination of these that is simple to use and reliable, with excellent handling of non-Latin filenames (Chinese, Russian etc).

This is the key: you don't have to use tar | srep | zstd | something hoping that everything will runs file, but a single 1MB executable, with 7z-like commands

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
*I could get rid of it, but at the cost of breaking the backward compatibility of the file format, so I don't want to.*

It is not the fastest tool out there, with real world performance of 80-200MB/s (depending on the case and HW of course).
*Not a big deal for me (I have very powerful HW, and/or run nightly cron-tasks)*

Extraction can require a number of seeks (due to various deduplicated blocks), which can slow down extraction on magnetic disks (but not on SSDs).
*In theory it would be possible to eliminate, but the work is huge and, frankly, I rarely use magnetic discs*

No other significant ones come to mind, except that it is known and used by few

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

**Single help**
It is possible to call -? and -examples with a parameter
a c d dir i k l m r s sha1 t utf x z franz main normal voodoo

```
zpaqfranz -? a 
zpaqfranz -examples x 

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
