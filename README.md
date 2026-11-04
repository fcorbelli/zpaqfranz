# zpaqfranz: advanced multiversioned archiver, with HW acceleration and SFX (on Windows)

<a href="https://sourceforge.net/projects/zpaqfranz/"><img align="right" width="125" alt="SourceForge Favorite" src="https://b.sf-syn.com/badge_img/3324403/oss-sf-favorite-white?achievement=oss-sf-favorite&variant_id=sf&r=https://github.com/fcorbelli/zpaqfranz"></a>

### Swiss army knife for backup and disaster recovery


[![GitHub release](https://img.shields.io/github/v/release/fcorbelli/zpaqfranz)](https://github.com/fcorbelli/zpaqfranz/releases)
[![SourceForge downloads](https://img.shields.io/sourceforge/dt/zpaqfranz?label=SourceForge%20downloads)](https://sourceforge.net/projects/zpaqfranz/)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](#license)


High-performance archiver and backup for mission-critical data.  
Like 7z or RAR on steroids, with deduplicated snapshots (versions) in a single file:
far more efficient and versatile than Time Machine or ZFS snapshots.

Built for speed on multi-core CPUs and modern SSD/NVMe drives: 1 GB/s and more, up to 3x faster than 7-Zip.

| program | time | archive |
|---|---:|---:|
| **zpaqfranz (current)** | **6.5 s** | **5.44 GB** |
| zpaqfranz (older) | 15.4 s | 5.64 GB |
| tar \| zstd `-1` | 8.1 s | 6.22 GB |
| 7-Zip `-mx1` | 21.9 s | 5.74 GB |
| WinRAR `-m1` | 40.4 s | 6.01 GB |

_Quick-and-dirty comparison against popular archivers: Windows 11, Ryzen 9 7950X3D, RAM disk, a Debian virtual machine of 11.7 GB as test set.
7-Zip 26.03, RAR 7.23, zstd 1.5.7, each in its fastest mode; median of three runs. Full details in the release notes of 65.3 and 65.5._


And that is only the first run. On the second one 7-Zip, RAR and tar rewrite everything; zpaqfranz stores only what changed.


🖥️ **[catpaq - Official GUI](https://github.com/fcorbelli/catpaq)**, open source, ready to use on many platforms.

📥 **Direct downloads (latest versions):**

[![Windows](https://img.shields.io/badge/Windows-0078D4?style=for-the-badge&logo=windows&logoColor=white)](https://www.francocorbelli.it/catpaq/latest/win64/catpaq.exe)
[![Debian](https://img.shields.io/badge/Debian-A81D33?style=for-the-badge&logo=debian&logoColor=white)](https://www.francocorbelli.it/catpaq/latest/debian/catpaq_debian_000019.zip)
[![Fedora](https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white)](https://www.francocorbelli.it/catpaq/latest/fedora/catpaq_fedora_000020.zip)
[![Arch Linux](https://img.shields.io/badge/Arch%20Linux-1793D1?style=for-the-badge&logo=archlinux&logoColor=white)](https://www.francocorbelli.it/catpaq/latest/arch/catpaq_arch_000019.zip)
[![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)](https://www.francocorbelli.it/catpaq/latest/macos/catpaq_macos_000020.zip)

**Key features**

- Deduplicated, incremental and versioned backups
- Anti-ransomware integrity checks and deep validation
- Multi-hash support: SHA-2, SHA-3, BLAKE3, XXH3 and more
- Strong encryption: AES-256 (`-key`) as standard, optional ChaCha20-based Franzen (`-franzen`), or even both!
- Low bandwidth for cloud, NAS and USB: only the new data travel
- Full deduplicated disk imaging
- Read-only mount of archives as a drive or a folder
- GUI for Windows, Linux and macOS: [catpaq](https://sourceforge.net/projects/catpaq/)

**Cross-platform and lightweight**: Windows (XP included), Linux, macOS, FreeBSD, OpenBSD, TrueNAS, Synology, QNAP, ARM and ESXi.
Without Franzen encryption, every archive can still be extracted by the original zpaq 7.15.

15+ years of active development • 100% FOSS • Forever free

---

## What's new

### ⚡ Much faster, with nothing to change

The default of `a` (add) is now faster on every side: zstd compression (`-m8`), a multithreaded deduplicator (`-turbo`),
a cached list of the files (`-fast`: `l` in a fraction of a second).
The archive is still a normal zpaq archive: zpaq 7.15 extracts it too.

**Smarter and faster**

- **Checking a backup against the disk** (`t backup.zpaq c:\data -ssd`): nothing is decompressed, the files are read in parallel. 116,725 files (6.5 GB) checked in **2.0 s** (warm cache, 32 threads).
- **Windows, whole NTFS drives**: the file list is read straight from the `$MFT`, **7 times faster** on a `C:` than the standard FindFirstFile/FindNextFile (12 times with a shadow copy).
- **Millions of files with little RAM**: 3.5 million files added with only 8 GB of RAM.

### 💽 Images of `C:` (and of any drive)

```
zpaqfranz a z:\c.zpaq c: -image
```

One switch, and it chooses by itself: on NTFS the used clusters, from a shadow copy (VSS), without pagefile and hiberfil;
FAT, FAT32 and exFAT by their used clusters too; anything else byte by byte (`-raw` for that, always).
Deduplicated like everything else: the second image of the same `C:` stores only what changed.

- About **800 MB/s** for the image of `C:` on my workstation (2026, NVMe, 16 cores)
- **Thin images**: the free clusters become zeros in the image.
- **Back out as a virtual disk**: `image z:\c.zpaq c: -to d:\c.vhd` (or `.vhdx`, `.vmdk` for VMware, VirtualBox, QEMU/Proxmox, or a raw file), any older version with `-until`, over 2 TB too
- **Back on a partition**: `image z:\c.zpaq c: -to g: -image`
- **The whole disk of Windows, for a restore that boots** (beta): `ac z:\pc.zpaq` takes `C:` plus the hidden partitions and the partition table; `xc z:\pc.zpaq -to 2: -force` puts it back (or `-to x.vmdk` / `x.vhdx`). There is a [WinPE ISO](https://sourceforge.net/projects/zpaqfranz/files/winpe/) to restore from.

On Linux, the BSDs and macOS `a /dev/sdb -image` takes the device byte by byte, and `image` gives it back as a raw file.

### 📂 Mount: browse a backup without extracting it

```
zpaqfranz mount z:\backup.zpaq              (Windows: the first free drive letter, Explorer opens on it)
zpaqfranz mount z:\backup.zpaq Z: -all      (every version, one folder each)
zpaqfranz mount /backup/nas.zpaq /mnt/zpaq  (Linux, FreeBSD, OpenBSD, macOS)
```

The archive becomes a **read-only drive or folder**: open, copy, search, play, `diff` any file, with no temporary folder.

- `-all`: one folder per version, each a full snapshot. "What did this file look like in June?" is a `copy`; "what changed between two backups?" is a `diff -r` between two folders
- `-range 5:9`: only what versions 5 to 9 added or changed
- encrypted, Franzen and multipart archives too; `-test` mounts, reads every file through the mountpoint and checks it
- **Images too**: `mount d:\c.vhd` attaches a `.vhd`, `.vhdx` or `.iso` read-only, and you look into yesterday's `C:`

Windows needs [WinFsp](https://winfsp.dev) (`mount` offers to download and install it; `zpaqfranz-full.exe` carries it inside);
Linux and the BSDs FUSE, macOS FUSE-T or macFUSE. The mount of `.vhd`/`.iso` needs no WinFsp, only the administrator (it asks).

### And more

`proxvzdump` / `zfsproxbackup` for every guest of Proxmox, with their restores ·
`mysqldump` / `mysqlrestore` ·
`repack` to keep only some versions (`7d4w12my`) ·
reports by e-mail (`-mailfull`, `-mailprivacy`) ·
`x -recover` on damaged archives ·
`cloud` upload in append ·
`f -test` surface test of a disk or a USB stick ·
SFX on Windows ·
help in English, Italian and Russian (`-it`, `-ru`)

---

## Quick start

```
zpaqfranz a backup.zpaq /home/me/data                 # first run: everything; then only the changes
zpaqfranz i backup.zpaq                               # the versions
zpaqfranz l backup.zpaq                               # the files of the last version
zpaqfranz t backup.zpaq /home/me/data -ssd            # is the backup equal to the disk?
zpaqfranz x backup.zpaq -to /tmp/restore -until 3     # restore version 3
zpaqfranz mount backup.zpaq /mnt/zpaq -all            # browse every version
```

Windows is the same (`zpaqfranz a z:\backup.zpaq c:\data`).
Run `zpaqfranz` alone for the help, `zpaqfranz h <command>` for one command,
or `zpaqfranz t` with no archive to choose it on a screen.

The help follows the language of the system (English, Italian, Russian). To choose it, add `-eng`, `-it` or `-ru`
to any line: `zpaqfranz -it`, `zpaqfranz h a -ru`.

---

## Seeing is believing

Archiving a folder 5 times, a daily run Monday to Friday, with 7z:

https://user-images.githubusercontent.com/77727889/215149589-0f2d9f91-ea5a-4f60-b587-f2a506148fe9.mp4

The same with zpaqfranz:

https://user-images.githubusercontent.com/77727889/215148702-edb8e5bb-8f4e-42bb-9637-6ee98742318a.mp4

_The five daily .7z take about 5 times the space of the .zpaq._

![compare](https://user-images.githubusercontent.com/77727889/215150599-83032cc6-06b0-432d-ba3b-b410698e3631.jpg)

Classic archivers (tar, 7z, RAR...) are the wrong tool for repeated backups, and
[even more so for SQL dumps](https://github.com/fcorbelli/zpaqfranz/wiki/Real-world:-SQL-dumps-(MySQL-MariaDB-Postgres-backup)).

<details>
<summary><b>More videos: a real file server (recorded in 2023, with older versions, so slower than today)</b></summary>

Physical Xeon machine with 8 cores, 64 GB RAM and NVMe disks, plus a Solaris-based NAS, 1 Gb ethernet.

rsync update, filesystem to filesystem (real speed):

https://user-images.githubusercontent.com/77727889/215152167-c6ce107a-6345-4060-b7a7-33ad30b269ee.mp4

rsync update to the Solaris NAS (real speed):

https://user-images.githubusercontent.com/77727889/215152259-2baa7001-d838-40de-b56c-6fe3feff9f1b.mp4

Backup update from the file system with zpaqfranz (real speed):

https://user-images.githubusercontent.com/77727889/215146670-1a11cd5d-6f00-4544-b797-9ca288ae12b1.mp4

Backup update via zfsbackup (real speed):

https://user-images.githubusercontent.com/77727889/215147310-cc760f20-08b8-4088-9d8a-f58f00eac211.mp4

Windows client, encrypted update of `C:` without the programs (`-frugal`), real speed:

https://user-images.githubusercontent.com/77727889/215269540-8e2c8641-0d3a-4f67-a243-ab617834c5de.mp4

Every-sector encrypted update of a 256 GB `C:` (accelerated):

https://user-images.githubusercontent.com/77727889/215271199-94400833-f973-41d2-a018-3f2277a648a9.mp4

Testing an archive at more than 1 GB/s (accelerated):

https://user-images.githubusercontent.com/77727889/215271989-5a77e1f1-8fba-422b-9e25-24c3f4640eb2.mp4

Update of a small server (300 GB) on a QNAP NAS: ~7 GB of Thunderbird mbox become ~6 MB, in ~4 minutes (real speed):

https://user-images.githubusercontent.com/77727889/215268613-e07e385c-0880-4534-ae35-0db8925cee6b.mp4

rsync of two remote backups, the .zpaq (file level) and zfsbackup (block level), one day of work of a small server (accelerated):

https://user-images.githubusercontent.com/77727889/215267855-22bf875c-90ee-47d1-8f8f-c2d0fa2ab201.mp4

</details>

---

## Download

### 🪟 Windows

| Variant | Download | Self-update | Notes |
|---|---|---|---|
| **Windows 64** (recommended) | [zpaqfranz.exe](https://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz.exe) | `zpaqfranz upgrade -force` | HW acceleration auto-detected (SHA-1/SHA-2) |
| Windows 64 Full | [zpaqfranz-full.exe](https://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz-full.exe) | - | Every library inside, WinFsp included: no internet needed |
| Windows 64 Open | [zpaqfranz-open.exe](https://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz-open.exe) | - | Built only from the source: no binary blobs, no network, no mount of archives |
| Windows 64 HW | [zpaqfranzhw.exe](https://www.francocorbelli.it/zpaqfranz/win64hw/zpaqfranzhw.exe) | `zpaqfranzhw upgrade -force` | Extra SHA-1 acceleration (AMD, ~+1.5%), with `-hw` |
| Windows 32 | [zpaqfranz32.exe](https://www.francocorbelli.it/zpaqfranz/win32/zpaqfranz32.exe) | `zpaqfranz32 upgrade -force` | Limited RAM and threads: slower |
| Everything | [SourceForge](https://sourceforge.net/projects/zpaqfranz/files) | | All the builds, the WinPE ISO, the older versions |

`upgrade` checks a digitally signed (`.p7m`) SHA-256 before replacing the executable.

### 🐧 Linux, BSD, macOS

| System | Version | Install | Demo |
|---|---|---|---|
| ![OpenBSD](https://img.shields.io/badge/OpenBSD-F2CA30?style=flat-square&logo=openbsd&logoColor=black) | ![version](https://repology.org/badge/version-for-repo/openbsd/zpaqfranz.svg?header=) | `pkg_add zpaqfranz` | - |
| ![FreeBSD](https://img.shields.io/badge/FreeBSD-AB2B28?style=flat-square&logo=freebsd&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/freebsd/zpaqfranz.svg?header=) | `pkg install zpaqfranz` | - |
| ![macOS](https://img.shields.io/badge/macOS-000000?style=flat-square&logo=apple&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/homebrew/zpaqfranz.svg?header=) | `brew install zpaqfranz` | - |
| ![Debian](https://img.shields.io/badge/Debian%2013+-A81D33?style=flat-square&logo=debian&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/debian_13/zpaqfranz.svg?header=) | `sudo apt install zpaqfranz` | [▶️ Desktop](http://www.francocorbelli.it/zpaqfranz/video/debian-deb.mp4) |
| ![Ubuntu](https://img.shields.io/badge/Ubuntu%2026.04+-E95420?style=flat-square&logo=ubuntu&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/ubuntu_26_04/zpaqfranz.svg?header=) | `sudo apt install zpaqfranz` | - |
| ![openSUSE](https://img.shields.io/badge/openSUSE-73BA25?style=flat-square&logo=opensuse&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/opensuse_tumbleweed/zpaqfranz.svg?header=) | `sudo zypper install zpaqfranz` | - |
| ![Arch Linux](https://img.shields.io/badge/Arch%20Linux-1793D1?style=flat-square&logo=archlinux&logoColor=white) | ![version](https://repology.org/badge/version-for-repo/aur/zpaqfranz.svg?header=) | [AUR: zpaqfranz-git](https://aur.archlinux.org/packages/zpaqfranz-git) | [▶️ Terminal](http://www.francocorbelli.it/zpaqfranz/video/arch.mp4) |
| ![Linux](https://img.shields.io/badge/Linux%2032%2F64-FCC624?style=flat-square&logo=linux&logoColor=black) | ![version](https://img.shields.io/github/v/release/fcorbelli/zpaqfranz?label=) | [static binaries](https://www.francocorbelli.it/zpaqfranz/linux) | - |

The packages of the distributions follow the releases with some delay (green: up to date, red: behind).
Every distribution that has zpaqfranz: [Repology](https://repology.org/project/zpaqfranz/versions).

### 🖥️ catpaq, the GUI

[**catpaq**](https://github.com/fcorbelli/catpaq) is the official open-source GUI: direct downloads at the [top of this page](#zpaqfranz).

In the console there is `tui` too: `zpaqfranz backup.zpaq` opens it.

---

## Real world: years of versions in one file

A file server of a mid-sized company, ~500,000 files and ~500 GB: **1042 versions in 817 GB**.

```
root@f-server:/copia1/copiepaq/spaz2020 # zpaqfranz i fserver_condivisioni.zpaq
zpaqfranz v51.27-experimental snapshot archiver, compiled May 26 2021
fserver_condivisioni.zpaq:
1042 versions, 1.538.727 files, 15.716.105 fragments, 877.457.003.477 bytes (817.20 GB)

< Ver  > <  date  > < time >  < added > <removed>    <    bytes added   >
00000001 2018-01-09 16:56:02  +00308608 -00000000 ->      229.882.913.501
00000002 2018-01-09 18:06:28  +00007039 -00000340 ->           47.356.864
00000003 2018-01-10 15:06:25  +00007731 -00000159 ->            7.314.709
(...)
00000147 2018-03-28 17:08:28  +00000422 -00000134 ->          277.237.055
(...)
00001041 2021-05-04 17:13:40  +00000605 -00000004 ->           95.909.988
00001042 2021-05-05 17:15:13  +00000579 -00000008 ->           82.487.415
```

The files as they were on 2018-03-28? That is version 147:

```
zpaqfranz x fserver_condivisioni.zpaq -to /tmp/restore -until 147
```

Another one, a ~200 GB server: **4904 versions** from August 2017 to March 2021, in 186 GB.
With tar, 7z or RAR the same 4900 copies would take 200 GB × 4900, about **980 TB**.
The same goes for virtual machines.

---

## FAQ

**Who made ZPAQ?**
One of the world's leading scientists in compression. [No, not me, but this guy](http://mattmahoney.net/)
([ZPAQ on Wikipedia](https://en.wikipedia.org/wiki/ZPAQ)), from 2009 to 2016,
on [encode.su](https://encode.su/threads/456-zpaq-updates), one of the best-known (and super-niche) compression forums.

**Why is it not as known as 7z or RAR, if it is so much better?**
Because few people use it. Try it!

**Who are you?**
A user (and a developer) who proposed and made various improvements to ZPAQ over the years.
When Dr. Mahoney retired, I made this fork to get the functions I need as a data storage manager.

**What if zpaqfranz is abandoned tomorrow?**
Your data are not locked in. A zpaqfranz archive is a standard zpaq archive: without Franzen encryption, the original
zpaq 7.15 of 2016 extracts it, the zstd methods (`-m7`, `-m8`, `-m9`) too, because their decoder is stored inside the archive.
With `a -715` zpaqfranz writes plain zpaq 7.15 archives, without any extension of its own. And zpaqfranz itself is one MIT source file that builds almost anywhere.

**Why should I trust this? It will be one of the 1000 programs that fail silently.**
As the Russians (and the Italians) say: trust, but check!
Checking is the part of the program I have worked on the most: hashes of every fragment and of every file, `t` against the disk,
`w` and `tw` that decompress in RAM, `p` for the paranoid.
In 65.15 the tests themselves were tested: 84,000 copies of archives damaged on purpose, every byte, six ways.
Result: **no test said OK with a file lost or wrong**.

**We already have borg, restic, hashbackup, bupstash...**
Great programs, often very well made. They work on a repository: a folder with many files to keep together.
zpaqfranz keeps everything in **one file that only grows at the end**: a copy is a copy of one file,
`rsync --append` sends only the new tail, and the archive of today extracts the data of eight years ago.
_If it is simple, maybe it will work._

**Too good to be true?**
It is not magic: a block deduplicator, a compressor and an archiver, chained.
There are faster compressors, better compressors, faster archivers, more efficient deduplicators.
What I never found is the four together, dead simple and reliable, with good handling of non-Latin file names, in one executable with 7z-like commands.
No `tar | srep | zstd | something` pipes, no program with dozens of dependencies.

**The archive is never modified, only appended**
So `rsync --append` (over ssh, to a NAS, to the cloud) copies only the part added by the last run:
not the 700 GB of the archive, the 2 GB of yesterday. Hundreds of GB of virtual servers can be sent over a VDSL line in minutes.

**For a developer: a "super git"**
Put a `zpaqfranz a` at the top of the makefile and every version of your software is kept, libraries and SQL dumps included, forever.

**Never delete anything?**
You can keep every version forever. When you want fewer, `repack` writes a new archive with only the ones you choose
(e.g. `repack old.zpaq 7d4w12my -to new.zpaq`: grandfather-father-son, by days, weeks, months and years; `-test` shows what would be kept).

**Defects?**
Some.
- Extraction may need many seeks (deduplicated blocks): slow on spinning disks, not on SSDs. With enough RAM `w` avoids it, and `xx` writes the extracted files sequentially (much faster on spinning disks, less fragmentation).
- Many small files: there `tar | zstd` is faster at the first run (zpaqfranz does more work per file: path, attributes, hashes for the deduplication). From the second run on, zpaqfranz stores only the changes.
- It is a tool for power users and administrators, used to the command line. [catpaq](https://github.com/fcorbelli/catpaq) and `tui` help.
- It is known and used by few.

---

## How to build

One file, `zpaqfranz.cpp`, no dependencies (link pthread):

```
g++ -O3 -Dunix zpaqfranz.cpp -o zpaqfranz -pthread                          (Linux)
clang++ -O3 -Dunix zpaqfranz.cpp -o zpaqfranz -pthread                      (FreeBSD, OpenBSD, macOS)
g++ -O3 zpaqfranz.cpp -o zpaqfranz -pthread -static -lurlmon                (Windows, MinGW)
```

With `mount`: add `-DZPAQMOUNT` and FUSE (`$(pkg-config fuse3 --cflags --libs)` on Linux); the lines for every system
are in "Building with Mount Support", at the top of the source.

Every release is built and tested on Windows, Linux (Fedora, openSUSE, Arch, Oracle Linux 7), FreeBSD, OpenBSD,
macOS, Solaris, ESXi and ARM NAS.

- [Almost "universal" (minimal) Makefile](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Makefile)
- [Quicker and dirtier!](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-quicker%E2%80%90and%E2%80%90dirtier)
- [DEFINEs at compile time](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-How-to-Build)
- [Target examples](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Target-examples)
- [Hidden gems](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Hidden-gems)
- [Strange things](https://github.com/fcorbelli/zpaqfranz/wiki/Quickstart-Strange-Things)

## Documentation

- [Integrated help](https://github.com/fcorbelli/zpaqfranz/wiki/HELP-integrated)
- [Commands (wiki)](https://github.com/fcorbelli/zpaqfranz/wiki/Command)
- [Release notes](https://github.com/fcorbelli/zpaqfranz/releases)

## License

MIT, Copyright (c) 2021-2026 Franco Corbelli. Based on ZPAQ 7.15 by Matt Mahoney; the licenses of the built-in libraries are listed in the source.
