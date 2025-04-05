## [61.3] - 2025-04-05

### Added
- **Power-Saving Features**: Introduced new switches and functions to reduce energy consumption during operations.
  - **`-slow` Switch**: Disables TurboBoost on modern CPUs (tested on AMD, untested on Intel) to limit maximum frequency.
    - Reduces power consumption by up to 30% during deduplication-heavy tasks (e.g., SHA1 calculation) with minimal impact on execution time.
    - Decreases noise on systems with variable cooling (fans, pumps).
    - Reliable on Windows; experimental on Linux (hardware interaction varies).
  - **`-monitor` Switch (Windows only)**: Puts the monitor into standby mode to save power during long sessions.
    - Not tested on multi-monitor setups (planned for future testing).
    - Not implemented for non-Windows systems due to complexity (X, non-X, consoles, etc.).
  - **`-shutdown` Switch**: Performs a "merciless" system shutdown after completing an `add` command.
    - Windows: Attempts to terminate all processes (success not guaranteed).
    - Non-Windows: Uses heuristic methods to handle `sudo` availability (not universally present).
  - **New `work` Commands**:
    - `zpaqfranz work shutdown`: Triggers a merciless system shutdown.
    - `zpaqfranz work big turbo`: Activates CPU turbo mode.
    - `zpaqfranz work big noturbo`: Deactivates CPU turbo mode (same as `-slow`).
    - `zpaqfranz work monitoroff`: Turns off the monitor (Windows only).
    - `zpaqfranz work monitoron`: Turns on the monitor (Windows only).
- **Example Usage**: `zpaqfranz a z:\1.zpaq c:\pippo -slow -monitor -shutdown`

### Changed
- **Shutdown Logic**: Improved system shutdown mechanism with platform-specific heuristics (e.g., `sudo` detection on non-Windows systems).

### Notes
- The `-slow` switch is most effective when deduplication dominates over compression, offering power savings with negligible performance impact.
- The `-monitor` feature is Windows-only due to the complexity of non-Windows display systems; no plans to extend it currently.
- The `-shutdown` feature may not always succeed on Windows due to process termination challenges.
- Feedback or suggestions are welcome via GitHub issues or direct contact.

## [61.2] - 2025-04-05

### Added
- **New `mysqldump` Command**: Introduced a new command to automatically generate backup scripts for MySQL/MariaDB databases, saving each database as a separate file within a single `.zpaq` archive.
  - Default behavior: Dumps all databases (excluding system databases like `information_schema`, `performance_schema`, and `sys`) as the root user.
  - Filtering options:
    - `-only <pattern>`: Include only databases matching the specified pattern (e.g., `-only 2015` matches `db2015`, `test2015prod`).
    - `-not <pattern>`: Exclude databases matching the specified pattern (e.g., `-not temp` excludes `temporary`, `temp_db`).
  - Compression and encryption support via `-mX` (compression level 0-5) and `-key <pwd>` (archive encryption).
  - Heuristic executable detection:
    - **Windows**: Searches `c:\program files` for `mysql.exe` and `mysqldump.exe`. Use `-space` to download 64-bit versions from [www.francocorbelli.it](http://www.francocorbelli.it/) or `-bin <path>` to specify a custom location.
    - **Non-Windows**: Searches typical directories (`/bin`, `/usr/local/bin`, etc.) or allows manual specification with `-bin <path>`.
  - Connection options: `-u <user>`, `-p <password>`, `-h <host>`, `-P <port>`.
  - Verbose mode with `-verbose`.
  - Example usage:
    - Windows: `mysqldump z:\1.zpaq -u root -p pluto -h 127.0.0.1 -P 3306 -key pippo -m2`
    - Linux: `mysqldump /tmp/test.zpaq -u root -p pluto -bin "/bin"`
    - Filtered: `mysqldump test.zpaq -u root -p pluto -only prod -not backup`
- **Solaris Compatibility**: Added support for Solaris systems.
- **ESXi Support**: Included compatibility improvements for ESXi environments.

### Changed
- **Deduplication Optimization**: Emphasized that deduplication occurs before compression, significantly speeding up subsequent backups of unchanged databases, especially with high compression levels like `-m4`.

### Fixed
- Minor bug fixes (details not specified in the release notes).

### Notes
- The `mysqldump` command is still under development but already provides significant utility.
- The `-fragment` switch is not compatible with this command due to its piping mode.
- Parallel dumping was considered but not implemented to maintain a single `.zpaq` file per RDBMS for convenience.
- Suggestions and issues can be reported via GitHub or direct contact.


## [61.1] - 2025-02-14

### Added
- **SFTP Support with libcurl**: Enabled SFTP functionality by compiling with `-DSFTP` to dynamically use the libcurl library.
  - **Windows**: Automatically downloads `libcurl-x64.dll`/`libcurl.dll` from the author's website (`zpaqfranz sftp`) if not found.
  - **Non-Windows**: Requires manual installation of `libcurl.so` (e.g., `apt install libcurl` on Debian, `pkg install curl` on FreeBSD). Searches heuristically in common paths (`/usr/lib/`, `/usr/local/lib/`, etc.).
  - **Use Case**: Direct uploads to SFTP servers (username/password only; key file support planned), reducing ransomware risks compared to Samba shares.
  - **Note**: Do not use `-static` with `-DSFTP` on *nix systems due to inconsistent behavior across platforms.
- **TUI Command**: Added a minimal text-based user interface (`tui`) to list, select, and extract files from archives.
  - Replaces the previous ncurses-based GUI with a simpler, DOS-like interface.
  - Works on most *nix systems (not very old ones). Use `h` or `?` for help.
  - Development status: ~50% complete, with many edge cases still needing debugging.
- **LS Command**: Introduced the `ls` command to navigate `.zpaq` archives like a filesystem.
  - Supports `ls (/dir)` to list directories, `cd` to change directories, and `get` to extract files.
  - Development status: ~30% complete, very immature, lacks TAB support and requires significant work.
  - Use `help` or `?` for command list.
- **New Switches**:
  - `-noonedrive`: Disables Windows OneDrive placeholders to prevent automatic downloads to the local drive.
  - `-norecursion` with `-only` in `list`: Prevents recursion into folders when listing with `-only` (fixes [issue #156](https://github.com/fcorbelli/zpaqfranz/issues/156)).
  - `-DNOLM` (experimental): Uses a software implementation for numeric functions, bypassing the `lm` library for compatibility with unusual systems.

### Removed
- **Server Code**: Dropped `zpaqfranz-over-TCP` functionality, replaced by SFTP.
- **Windows GUI with ncurses**: Replaced by the new `tui` command.

### Fixed
- **Linuxsettime Issues**: Addressed some unspecified bugs in `linuxsettime` functionality.

### Changed
- **Branch Introduction**: This release marks the start of branch 61 with significant new features and potential instability.

### Notes
- **Development Status**:
  - `sftp`: ~70% complete and tested.
  - `tui`: ~50% complete, needs extensive debugging.
  - `ls`: ~30% complete, highly experimental.
- **User Feedback**: As this is the first release of branch 61, bugs are expected. Please report issues on GitHub to help improve stability and functionality.
- **SFTP Installation Examples**:
  - Debian: `apt install libcurl`
  - Fedora: `dnf install libcurl`
  - FreeBSD: `pkg install curl`
  - macOS: `brew install curl`
  - See documentation for full list of package manager commands.

### Download
- Available at [SourceForge](https://sourceforge.net/projects/zpaqfranz/files/61.1/zpaqfranz.cpp/download)


## [Unreleased]
- Planned SFTP key file support.
- Multi-monitor testing for future releases.
- Enhanced `tui` and `ls` functionality (e.g., TAB support for `ls`).

## [60.10] - 2024-12-20

### Added
- **`-tmp` Switch**: Now enabled by default for backups. Creates archives with a `.tmp` extension during compression, renaming them to `.zpaq` only upon successful completion.
  - Mitigates corruption risks from unexpected shutdowns or crashes by ensuring incomplete archives remain as `.tmp`.
  - On restart, existing `.tmp` files are "parked," allowing the process to resume and complete.
- **`-notrim` Flag**: Disables automatic correction of incomplete transactions in the last transaction, restoring zpaq 7.15 behavior.
- **`-destination` Switch in `consolidate` Command**: Allows renaming of `.zpaq` backup files (e.g., from `pippo` to `pluto`).
  - Complements the existing `-to` switch, which merges multipart files into a single file (labeled 01).
  - **Warning**: Always use full paths (e.g., `c:\zpaqfranz\pippo.zpaq`) with `consolidate`.
- **Enhanced `i` (info) Command**:
  - Now displays totals by default.
  - Added `-n` switch to show the last few lines of info output.
- **`-nopid` Switch**: Disables creation of `.pid` files during backups to prevent multiple executions.
- **Windows Progress Display**: Shows download progress in the calling console during updates from the author's website.
- **`-big` Switch Enhancement**: During backups, displays the last day of the backup in a larger format for easier log checking.

### Changed
- **Default Backup Behavior**: Backup command now uses `.tmp` files by default to protect against corruption from interruptions.
- **Incomplete Transaction Handling**: 
  - zpaqfranz now issues a prominent warning for incomplete transactions.
  - Automatically attempts to correct the archive if the interrupted transaction is the last one (unless `-notrim` is used).
  - For severe cases, users can use `consolidate` (multipart) or `trim` (single file) to remove corrupted parts.

### Fixed
- **`-stdin` Bug**: Resolved an issue that disabled the deduplicator when using `-stdin`.
- **Windows XP Support**: Restored compatibility for the 32-bit version on Windows XP.
- **Minor Source Code Fixes**: Addressed various unspecified issues in the codebase.

### Notes
- The `.tmp` feature addresses zpaq's historical fragility with corrupted archives due to shutdowns or crashes, improving reliability for both single and multipart backups.
- For further details or to report issues, refer to the GitHub issues section.
- Future plans include a switch to convert "normal" `.zpaq` archives directly into backups.

---

## [Unreleased]
- Planned switch to convert "normal" `.zpaq` archives into backups.

## [60.9] - 2024-11-13

### Added
- **`-nojit` Switch**: Replaces the `-DNOJIT` compilation flag. Automatically detects JIT support at both CPU and OS levels (e.g., NetBSD may block `PROT_EXEC`).
  - JIT accelerates data extraction (compression speed unaffected).
  - Not fully tested on virtualized systems with "fake" CPUs; a `forcejit` switch is planned for the future.
- **`-tmp` Switch for Multipart Files**: Names multipart files as `.tmp` during creation, renaming them to `.zpaq` upon completion.
  - Enables parallel testing/updates and compatibility with file-sync tools like Syncthing.
- **Improved Password Handling**: Enhanced `-key` switch with support for delete key and cursor movement.
  - Prompts for password twice during archive creation to ensure consistency (e.g., `zpaqfranz a z:\1.zpaq *.cpp -key`).
- **1980 Timestamp**: Sets file dates to 1/1/1980 during creation, updated only when the `jidac` header is written, aiding identification of incomplete files.
- **Windows Placeholders**: Added filename placeholders `$pcname`, `$computername`, and `$username` (e.g., `zpaqfranz a z:\pippo_$username c:\nz`).
- **ZETA Hasher with `-backupzeta`**: New pseudo XXHASH64 hash calculation during backup generation.
  - Avoids re-reading large multipart files (e.g., virtual disks) for integrity checks, also calculates CRC-32.
  - Not yet supported for encrypted multipart archives (planned for future).
- **`-nomore` Switch**: Disables the internal `more` command for faster external text processing (e.g., `zpaqfranz h h -nomore | less`).
  - On 64-bit Windows, enables experimental LargePages support (no noticeable improvement; may be removed).
- **Common Switches List**: Accessible via `zpaqfranz h common`.
- **P7M Signature Check**: Windows option to verify FEQ digital signatures for hash files during updates (see [wiki](https://github.com/fcorbelli/zpaqfranz/wiki/Windows-update)).
- **IPv6 Support**: Experimental support with `-DIVP6` compilation flag for the upgrade command (untested due to lack of IPv6 environment).

### Changed
- **JIT Handling**: Moved from compile-time `-DNOJIT` to runtime `-nojit` switch for broader compatibility.
- **Command Rename**: `consolidatebackup` renamed to `consolidate`.

### Fixed
- **Old Compiler Compatibility**: Minor fixes for very old compilers (e.g., Slackware) and 32-bit systems.
- **HPPA CPU Support**: Resolved a potential CRC-32 alignment issue on strict-memory CPUs (e.g., 32-bit HP RISC), slightly slower but more reliable.

### Notes
- **Compatibility Efforts**: Ongoing support for old systems and compilers remains a challenge but is maintained with minimal divergence from modern versions.
- **ZETA Hasher Details**: See GitHub issues for a full explanation of `-backupzeta` functionality.
- **IPv6**: Untested due to lack of test environment; feedback welcome.
- **Hints**: Additional context for changes can be found in [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues?q=is%3Aissue).
- **Request**: Testing on Apple Silicon (Mx) systems is desired; contact the author if you can provide access.

---

## [Unreleased]
- Planned `forcejit` switch for overriding JIT detection.
- Support for `-backupzeta` with encrypted multipart archives.
- Potential removal of `-nomore` LargePages experiment.

  
## [58.12.s] - 2023-12-08
### faster, redup

## [58.11.w] - 2023-11-01
### dump command
- Undocumented -collision -kill, to be implemented

## [58.11.q] - 2023-10-08
### collision command and -collision switch
- Quickly check and (sometimes) recovery from SHA-1 collision

## [58.10.k] - 2023-09-21
### Fix
- Warning-hunting on 15+ years compiler

## [58.10.j] - 2023-09-19
### Fix
- Removed a fake compiler warning
  
## [58.10.i] - 2023-09-14
### Fix
- 32 bit gcc fix, or better workaroud https://bugzilla.redhat.com/show_bug.cgi?id=2238781
- disclaimer after help for USE THE DOUBLE QUOTES, LUKE!
- changed $day, $hour... to %day, %hour ( Linux does not like '$' very much )

## [58.10.g] - 2023-09-12
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
