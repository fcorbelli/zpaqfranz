### [61.5] - 2025-07-10
There are many new features in this build, so particular attention should be paid to the possibility of new bugs being introduced.

## Main Change
The primary change is the overhaul of the interface with CURL and the management of SFTP commands, which now (mostly) support the `-ssd` switch for parallel operations.
### Supported commands for sftp
- **upload**: Uploads a single file to SFTP.
- **verify**: Quickly compares a local file to a remote file.
- **quick**: Retrieves the QUICK hash of a remote file.
- **ls**: Lists the contents of a remote folder.
- **delete**: Deletes a remote file.
- **size**: Retrieves the size of a remote file.
- **rsync**: Performs an rsync-like operation to sync local files to a remote folder (`-ssd` supported).
  - `-force`: Prevents appending.
- **1on1**: Quickly compares local files to a remote folder (`-ssd` supported).

## New Switches
- **-appendoutput**: Appends data to the `-out` file instead of recreating it each time.
- **-writeonconsole**: Writes output to stderr, allowing data to be displayed on the console even when redirected.
- **-last**: Operates on the last file in a selection, typically used for the last part of a multipart archive.
- **-home**: Now works with the `l` (list) command, showing the sizes of virtual folders inside an archive at one level deep.

## Other Additions
- Introduced `work devart` for highly visible on-screen text.
- In the `utf` command, the `-fix255` switch checks the maximum length of specified file names with `maxsize`.
- New `drive` command on Windows: Displays the list of connected physical disks with their respective numbers.
- The `-all` switch (with `-image`) on Windows operates on an entire disk image, similar to `dd`, rather than a single partition.
- New commands: `work datebig` and `work datetimebig`.

## Additional Features
It is now possible to extract only the files added in a specific version, marked with a textual comment, using the following example format:
```
c:\zpaqfranz\zpaqfranz x z:\2.zpaq -to z:\wherever -comment "something" -range
```

## Miscellaneous Changes
- Improved OpenBSD support.
- On Windows, `decodewinerror` is no longer hardcoded (now respects the local language).
- Fixed the `test` command to address occasional false positives.
- Improved alignment of help text lines.
- Removed comments from the CURL library and unused defines.

## Additional Notes
- Reduced the size of the source code.
  
### [61.4] - 2025-06-16

# - Many features in this release (e.g., `-image`, `-ntfs`, `ntfs` command, `work resetacl`) are experimental and not fully tested. Use with caution and report issues.

#### Added

##### For *nix (Linux, etc.)
- **`-image` Switch**: Added to the `a` (add) command to create a sector-by-sector copy of a device, similar to the `dd` command.
  - Restored images can be mounted on Linux using a snippet like:
    ```bash
    fdisk -l image.img
    losetup -fP _dev_sda.img
    losetup -a
    mkdir -p /ripristinato
    mount /dev/loop0p1 /ripristinato
    (...)
    umount /ripristinato
    losetup -d /dev/loop0
    ```
  - Experimental feature; not thoroughly tested.
- **`-tar` Switch**: Available during archive creation (`a`) and extraction (`x`) to preserve file access rights, group, and user metadata.
  - When used with the `l` (list) command, displays the added metadata.
  - Simplifies metadata restoration for *nix systems.
- **Improved ZFS Backup Handling**: Enhanced automatic integration with `pv` for better user feedback on backup progress during ZFS operations.

##### For Windows
- **`-ntfs` Switch**: When used with `-image`, stores only the used sectors of an NTFS partition in the zpaq archive.
  - Format is experimental and not yet optimized.
  - Intended for emergency image-based backups of Windows systems.
- **New `ntfs` Command**: Regenerates the original file from a zpaqfranz-created image, filling unused sectors with zeros.
  - Experimental and under active development.
- **`-ntfs` Switch (without `-image`)**: Scans an NTFS drive by reading and decoding its NTFS data directly, bypassing file-by-file enumeration.
  - Similar to the behavior of the "Everything" utility.
  - Significantly speeds up file enumeration on large, slow servers with magnetic disks.
- **New `work resetacl` Command**: Generates a batch file to reset folder permissions to administrators.
  - Useful for normalizing access after restoring NTFS folders with restricted permissions.
  - Experimental; intended to address post-restore access issues.

#### Changed
- **Code Refactoring**: Reduced compilation warnings for both Windows and *nix platforms.
- **Dropbox Cache Handling**: Skips `.dropbox.cache` folders during operations to avoid unnecessary processing.
- **Command Path Detection (*nix)**: Adopted a smarter strategy to locate *nix commands in likely directories, improving reliability.

#### Notes
- The `-ntfs` switch is designed for specific use cases like large server enumeration or emergency backups but may evolve in future releases.
- The `-tar` switch enhances metadata handling for *nix, making it easier to restore complex file permissions.
- The `pv` integration for ZFS backups improves user experience but requires `pv` to be installed.
- Feedback and bug reports are welcome via GitHub issues.

  

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

---

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

---

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

## [Unreleased]
- Planned SFTP key file support.
- Multi-monitor testing for future releases.
- Enhanced `tui` and `ls` functionality (e.g., TAB support for `ls`).

---

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

## [Unreleased]
- Planned switch to convert "normal" `.zpaq` archives into backups.

---

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

## [Unreleased]
- Planned `forcejit` switch for overriding JIT detection.
- Support for `-backupzeta` with encrypted multipart archives.
- Potential removal of `-nomore` LargePages experiment.

---


## [60.8] - 2024-10-21

### Added
- **`-backupzeta` Switch**: Generates checksums (almost XXHASH64 and CRC-32) on-the-fly during `.zpaq` file creation in the backup command.
  - Saves time by avoiding post-creation reads, especially on slow HDDs.
  - Future support planned for encrypted volumes ([issue #139](https://github.com/fcorbelli/zpaqfranz/issues/139)).
- **Creation Date Set to 1/1/1980**: `.zpaq` archives are now stamped with 1/1/1980 to easily identify incomplete files ([issue #138](https://github.com/fcorbelli/zpaqfranz/issues/138)).
- **New Hash Algorithms**: Added ZETA and ZETAENC, selectable with `-zeta` and `-zetaenc`.
  - Details in [issue #139, comment](https://github.com/fcorbelli/zpaqfranz/issues/139#issuecomment-2425010093).
- **`-destination` Switch**: Allows loading multiple `-to` options from a text file for batch processing.
  - Explanation in [issue #136, comment](https://github.com/fcorbelli/zpaqfranz/issues/136#issuecomment-2422947782).
- **`-nodelete` Switch**: Prevents marking files as deleted if not found during path scanning, useful for bulk file list manipulation.
  - Details in [issue #136, comment](https://github.com/fcorbelli/zpaqfranz/issues/136#issuecomment-2416220823).
- **`-salt` Switch**: Forces an empty salt (32 zero bytes) for development purposes (not recommended for general use).
- **`-hdd` Switch**: Uses RAM (including virtual memory/swap) to buffer extracted data before sequential writing to HDD.
  - Reduces seek times on HDDs, halving extraction time for medium-sized files (not suitable for very large files).
  - Details in [issue #135](https://github.com/fcorbelli/zpaqfranz/issues/135).
- **`-ramdisk`**: Internal support for `-hdd` functionality.

### Changed
- **Build Compatibility**: Improved support for BSD operating systems:
  - OpenBSD
  - NetBSD
  - DragonFly BSD
- **Warnings Display**: Warnings are now highlighted in yellow for better visibility.

### Fixed
- **`-input` Bug**: Minor fix for Windows-specific issue with the `-input` switch.

### Notes
- The `-backupzeta` switch significantly improves performance on slow drives by eliminating the need to re-read files for checksums.
- The `-hdd` switch leverages RAM/SSD speed for faster HDD writes but is limited by available memory for large files.
- The `-salt` switch is intended for development and debugging, not end-user scenarios.
- Additional context and explanations for many features can be found in the linked GitHub issues.

## [Unreleased]
- Planned support for `-backupzeta` with encrypted volumes.

---

## [60.7] - 2024-10-08

### Added
- **`-errorlog` Switch**: Creates a file listing errors to reduce log clutter.
- **`-nocaptcha` Switch**: Bypasses captchas during operations.
- **`-ht` Switch**: Overrides the default use of physical CPU cores (no Hyperthreading) to revert to the previous Hyperthreading-enabled method.
- **`-input` Switch**: Loads a list of files to be added from a specified input file.
- **`-715` Switch in `l` (list) Command**: Restores an output nearly identical (binary-wise) to zpaq 7.15.
- **`-to` Switch in `sfx` Command (Windows)**: Extracts the `.zpaq` file from a self-extracting executable.
- **`-home` Switch in `sum` Command**: Replaces the previous `-checksum` switch.
- **`-fixreserved` Switch (Windows)**: Removes the `:` character from filenames during extraction to handle reserved character issues.
- **Memory Usage Tracking**: Displays approximate memory usage in the final output line.
- **UTF8 Output Work Command**: Added a new `work` command for improved UTF8 file output handling on Windows.

### Changed
- **License Update**: Modified the license of one component to comply with Fedora policies.
- **CPU Detection**:
  - Now defaults to using physical CPU cores only (no Hyperthreading); use `-ht` to revert.
  - Improved CPU count detection on Solaris (untested).
- **Error Messages**: 
  - Displayed in red by default for better visibility.
  - More descriptive messages for out-of-memory errors caused by overly small fragments.
- **UTF8 File Output**: Completely rewritten for Windows to enhance compatibility and functionality.
- **Output Lines**: Renumbered for clarity.
- **Backup Naming**: Heuristically adopts the name of an existing backup in some cases.

### Notes
- This release introduces numerous features; expect potential bugs as testing continues.
- The wiki is being updated with more details; for now, this changelog provides a high-level overview.
- Feedback and bug reports are welcome via [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues).

---

## [60.6] - 2024-08-25

### Added
- **Improved `-stdin` Management**: Files added via `-stdin` are now deduplicated as efficiently as manually added files.
- **`comparehex` Command**: Compares hexadecimal hash codes between two files, exiting with "OK" if they match.
  - Ignores non-hex characters; optional hash length specification.
  - Example: `zpaqfranz comparehex z:\1.txt z:\2.txt "GLOBAL SHA256:" 64`
- **`count` Command**: Counts occurrences of a string across multiple files, returning "OK" if the count matches the expected value.
  - Defaults to counting "OK" with `-big` if no string is specified.
  - Example: `zpaqfranz count z:\*.txt 3 "all OK"`
- **`work` Command Verbs**: Added utility functions for log automation.
  - Examples: `work big "count the ok"`, `work pad 123 -n 4`, `work date "%year_%month_%day" -terse`.
- **`-crc32` Switch for `t` (test) Command**: Performs a triple CRC-32 check against the filesystem.
  - Compares original, recomputed, and re-read CRC-32 values for integrity verification.
  - Supports `-find`/`-replace` for path adjustments and `-ssd` for multithreading.
  - Example: `t z:\\1.zpaq -crc32 -find "x:/memme/" -replace "c:/nz/"`
- **`-terse` Switch**: Reduces output verbosity across commands for easier redirection.
- **`-csv` and `-csvhf` Switches for `l` (list) Command**: Outputs file lists in a CSV-like format.
  - `-csvhf` adds header/footer strings; uses `\t` for TABs.
  - Example: `l z:\1.zpaq -terse -csv "\",\"" -csvhf "\""`
- **`-external` Switch in `a` (add) Command**: Executes an external command before adding files, saving its output to a virtual file (e.g., `VFILE-l-external.txt`).
  - Useful for snapshots or independent hash checks (e.g., with `hashdeep`).
  - Example: `zpaqfranz a z:\2.zpaq c:\nz -external "c:\nz\hashdeep64 -r -c sha256 %files"`
- **`-external` Switch in `x` (extract) Command**: Extracts the virtual external file directly.
  - Example: `x z:\2.zpaq -external -silent > mygoodoutput.txt`
- **`-symlink` Switch in `a` (add) Command (Windows)**: Ignores NTFS symlinks during addition.
- **`-touch X` Switch**: Forces a specific timestamp (date or date+time) on files added during the `a` command, including `-stdin`.
- **Execution Dates in Backups**: Stores execution dates in backups; `testbackup` shows the latest date.
- **Franzomips CPU Support**: Added new CPUs to the `franzomips` list.

### Changed
- **Control-C Handling**: Improved cleanup of `-chunk` files and potential `.zpaq` rollback on termination (portability unconfirmed).
- **`gettempdirectory`**: Creates temporary files in timestamped subfolders to avoid collisions with multiple `zpaqfranz` instances.
- **Output with `-big`**: Ensures the final "OK" output remains visible even with reduced verbosity switches.
- **Maximum Versions in `i` Command**: Increased the limit of displayed versions.

### Fixed
- Various minor bugs (unspecified).

### Notes
- This release adds many features tailored to the developer's needs, potentially replicable with tools like `awk` or `grep`, but integrated for convenience in environments lacking such utilities (e.g., ESXi, NAS).
- New features may introduce bugs; users should verify archive integrity after use.
- Report issues or suggestions at [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues).

## [Unreleased]
- Potential `.zpaq` rollback on Control-C termination.
- Future evolution of `-symlink` handling.

---


## [60.5] - 2024-07-20

### Added
- **Faster Windows 64-bit Binary**: Improved performance on AMD CPUs, with average speed gains of 5% and up to 20% in best cases.
  - Example: Compression time reduced from 21.938s (v60.1k) to 18.875s (v60.5d) for identical tasks.
- **`-stat` Switch in `a` (add) Command**: Displays statistics on files added, removed, or updated.
  - Example: `zpaqfranz a z:\test.zpaq c:\zpaqfranz\*.cpp -stat`
- **`-stat` Switch in `i` (info) Command**: Shows uncompressed data size (slower operation).
  - Example: `zpaqfranz i z:\test.zpaq -stat`
- **`-quick` Switch in `t` (test) Command with Paths**: Performs a quick test using only file size and date, skipping hash computation.
  - Example: `zpaqfranz t z:\test.zpaq c:\zpaqfranz\*.cpp -quick`
- **`testbackup` Command Enhancement**: Now displays a global SHA256 hash of backup hashes during verification.
  - Facilitates quick comparison between local and remote backups (e.g., Synology vs. FreeBSD server).
  - Example output shows identical SHA256 (`EDBEE1D3...`) for consistency checks.
  - Full rehashing available with `-verify` for thorough validation.

### Changed
- **`fclose()` De-overloading**: Modified to assist with debugging, reducing potential conflicts.

### Fixed
- **Chunked Add Bug**: Resolved a possible double file close issue in chunked addition operations.

### Notes
- Performance improvements are most notable on Windows 64-bit systems, particularly with AMD CPUs.
- The global SHA256 in `testbackup` provides a fast integrity check; use `-paranoid` or `-verify` for deeper verification, especially if backup paths differ.
- Report bugs or feedback at [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues).

---

## [60.4] - 2024-07-14

### Added
- **`-nosynology` Switch**: Excludes hidden Synology system folders during the `a` (add) command.
  - Ignores paths like `*/@recycle/*`, `*/#snapshot/*`, `*/@SynologyDrive/*`, etc. (full list in documentation).
- **`isjitable` in `b` (benchmark) Command**: Issues a warning if compiled without `-DNOJIT` but the CPU does not appear to be Intel/AMD (e.g., virtual or ARM CPUs).
  - Enhanced with `-debug` for additional CPU information (e.g., vendor ID, endianness).
  - Examples:
    - `zpaqfranz b` (normal Intel/AMD CPU detection).
    - `zpaqfranz b -debug` (detailed output with warning for non-Intel/AMD CPUs).
- **Celeron J4125 Benchmark**: Added `franzomips` support for Synology DS224+ NAS with Celeron J4125 CPU.

### Fixed
- **Compatibility Fixes**: Resolved issues for "strange" platforms, ensuring compilation and execution on:
  - Haiku
  - Macintosh
  - Solaris

### Notes
- The `-nosynology` switch improves usability on Synology NAS systems by skipping system-specific folders.
- CPU detection in `isjitable` is not fully reliable across all platforms due to portability challenges; future improvements are planned.
- Report bugs or suggestions at [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues).

---

## [60.3] - 2024-07-07

### Fixed
- **Microfixes for Compilability**: Applied small corrections to ensure compatibility across various systems.
  - Supported builds:
    - `zpaqfranz.exe`: Windows 64-bit
    - `zpaqfranz32.exe`: Windows 32-bit
    - `zpaqfranzhw.exe`: Windows 64-bit with SHA-1 assembly optimizations (AMD)
    - `zpaqfranz_armv8`: ARM-NAS compatible build (e.g., QNAP, Synology)
    - `zpaqfranz_esxi`: ESXi build
    - `zpaqfranz_freebsd`: FreeBSD 64-bit
    - `zpaqfranz_linux32`: Generic Linux 32-bit
    - `zpaqfranz_linux64`: Generic Linux 64-bit
    - `zpaqfranz_nas`: Generic Linux 64-bit for Intel-powered NAS
    - `zpaqfranz_openbsd`: OpenBSD 64-bit
    - `zpaqfranz_haiku`: Haiku 64-bit

### Notes
- No macOS build included in this release ("Sorry, no Mac today 😄").
- These microfixes enhance portability; specific changes are minor and focused on compilation stability.
- Report issues or feedback at [GitHub issues](https://github.com/fcorbelli/zpaqfranz/issues).


---

## [60.2] - 2024-07-03

### Added
- **Improved `dump` Command**: Enhanced to display archive technical details in a more readable format.
  - Shows archive format type and compatibility level:
    - `60+`: zpaqfranz v60 and later (circa July 2024).
    - `<60`: zpaqfranz up to v59.x.
    - `715`: Standard zpaq 7.15 or zpaqfranz with `-715`.
  - Switches:
    - **`-summary`**: Brief output.
    - **`-verbose`**: Detailed useful information.
    - **`-all`**: Deeper technical details.
  - Examples:
    - `zpaqfranz dump z:\715.zpaq -summary`: Identifies as `715 archive (original zpaq format)`.
    - `zpaqfranz dump z:\v59.zpaq -summary`: Shows `<60 archive (older zpaqfranz v59)` with `XXHASH64`.
    - `zpaqfranz dump z:\v60.zpaq -summary`: Shows `60+ archive (newer zpaqfranz v60)` with `XXHASH64B`.
    - `zpaqfranz dump z:\v60_mixed.zpaq -summary`: Detects mixed block sizes (`050`, `190`) and hash types (`WHIRLPOOL`, `XXHASH64B`), warns against `-frugal`.

### Fixed
- **Backup Command Bug**: Prevented creation of unnecessarily large backup files when no files were added.

### Notes
- The `dump` command is a debugging tool, not designed for large (multi-gigabyte) archives; it may crash due to its naive implementation. Future improvements are possible but not prioritized.
- Mixed block sizes/hashes (e.g., `v60_mixed.zpaq`) can cause crashes with `-frugal` unless explicitly managed. Use `-frugal` only if you understand the implications; automatic controls may be added later.

---

## [60.1] - 2024-07-01

### Added
- **New Binary FRANZBLOCK Format**: Smaller storage format for FRANZBLOCKs.
  - Slightly reduces archive size; default is `-xxhash64b`.
  - New switches with `b` suffix (e.g., `-blake3b`); old formats (e.g., `-blake3`) retained.
  - Warning: `-xxhash64b` is default; explicitly specify old formats (e.g., `-xxhash64`) for pre-v60 compatibility.
- **`-frugal` Switch**: Reduces memory usage for large archives (10M–100M files).
  - Risk of crashes if mixing FRANZBLOCK sizes/hashes (e.g., `blake3`, `whirlpool`, `sha3`).
  - Safe with consistent hashes; unnecessary for smaller archives.
- **`-date` Switch**: Stores file creation dates in new format.
  - Default on Windows; optional on *nix with `-date` (slower processing).
  - Uses heuristics for *nix birth dates (accuracy not guaranteed).
- **File Change Tracking**: Stores counts of added, modified, and deleted files per version.
- **Enhanced `l` (list) Command**: Rewritten with richer output.
  - Displays compression ratio, file counts (`+` added, `#` modified, `-` deleted).
  - Auto-resizes columns; uses colors (disabled with `NO_COLOR` or `-nocolor`).
  - Supports `-attr`, `-checksum` (e.g., `-crc32`), `-date`, `-terse`.
  - Slower due to added features; faster version may be considered later.
  - Details: [GitHub #111](https://github.com/fcorbelli/zpaqfranz/issues/111).
- **Backup with `-index`**: Detachable index files for Worm systems.
  - Indexes storable separately (e.g., `-index <path>`); fragile if mismatched.
  - Details: [GitHub #109](https://github.com/fcorbelli/zpaqfranz/issues/109).
- **Enhanced `t` (test) Command**: Supports `-test` with `-find`, `-replace`, `-to`.
  - Enables string substitution during testing; see [GitHub #112](https://github.com/fcorbelli/zpaqfranz/issues/112).
- **`-DNAS` Compilation Switch**: Optimizes memory usage for NAS (e.g., Synology, QNAP).
- **Extended `autotest`**: Enhanced `autotest -all -to <path>`.
  - More tests, including expected failures; requires ~15GB.
  - Tested on Windows, Debian, FreeBSD, QNAP ARM; untested on macOS, Solaris.
  - May produce false positives; runtime varies (minutes to hours on slow NAS).

### Changed
- **Backward Compatibility**: New format readable by zpaq and zpaqfranz ≤ v59 for listing, extraction, and addition.
  - Pre-v60 cannot test CRC32 or read hashes from v60 archives.
  - Use non-`b` switches (e.g., `-xxhash64`) for full v59 compatibility.
- **`l` (list) Command**: Slower due to enhanced features and operations.

### Notes
- This branch introduces new features and potential bugs; use with caution.
- `-frugal` is for advanced users with consistent hashes and massive file counts; avoid mixing hash types.
- ADS management not yet updated for new format; recalculation can be forced.
- Autotest reliability depends on system; feedback encouraged for untested platforms.

---

## [59.9] - 2024-06-22

### Added
- **`-index` for `backup` Command**: Allows storing backup indexes in a separate folder for WORM storage.
  - User must ensure correct file pairing.
  - Example: `zpaqfranz backup z:\pippo *.cpp -index c:\temp`.
- **`-thunderbird` Flag (Windows)**: Automatically includes Thunderbird folders from `AppData\Local` and `AppData\Roaming`.
  - Optionally terminates `thunderbird.exe` before backup with `-kill`.
  - Example: `zpaqfranz a z:\email.zpaq c:\users\utente -thunderbird -kill`.
- **NO_COLOR Support on *nix**: Disables output coloring via shell variable `NO_COLOR`.

### Fixed
- Resolved an error when listing non-standard hashes.

### Notes
- Use `-index` with caution to avoid mismatching indexes and `.zpaq` files.

 ---

## [59.8] - 2024-06-19

### Added
- **`-ifexist <X>` Switch in `a` (add)**: Prevents adding files if folder `X` does not exist.
  - Useful for *nix systems to avoid backups to local disk when external mounts (e.g., NFS) fail.
  - Example: `zpaqfranz a /monta/mygoodnas/thebackup.zpaq /home -ifexist /monta/mygoodnas/rar`.
- **Console Color Support on *nix**: Adds color output to console; intended to work across most *nix systems.
- **`-nocolors` on Redirect**: Automatically disables colors when output is redirected to a file.

### Changed
- **New `l` (list) Format**: Updated display format.
  - Autosizes file size column.
  - Shows estimated compression ratio.
  - Provides enhanced details with `-all`; hides attributes by default.
  - Reverts to old format with `-attr`.
- **Win32/Win64hw Updates**: `zpaqfranz32.exe` (32-bit) and `zpaqfranzhw.exe` (64-bit SHA-1 ASM-accelerated) now support automatic updates via the `upgrade` command.

### Notes
- `-ifexist` helps prevent disk saturation on *nix by checking for sentinel folders (e.g., `/monta/mygoodnas/rar`) on mounted filesystems.
- Visual examples of the new `l` format and `-attr` behavior available in GitHub assets: [Image 1](https://github.com/fcorbelli/zpaqfranz/assets/77727889/df8e60c6-a6a3-4071-93c8-31c9982fb467), [Image 2](https://github.com/fcorbelli/zpaqfranz/assets/77727889/242b07ec-941f-462d-877d-ecd44c17fd11), [Image 3](https://github.com/fcorbelli/zpaqfranz/assets/77727889/3164b036-f779-4be9-be60-cf80b543434c).

---

## [59.7] - 2024-06-06

### Added
- **`zfssize` Command**: Displays the size of ZFS snapshots quickly.
- **`-pause` Switch in `a` (add)**: Pauses the archiver, waiting for a keystroke before proceeding.
- **Runhigh with VSS (Windows)**: Automatically elevates privileges for `-vss` add operations from a non-elevated command line.
- **AMD 3950X Benchmark**: Added performance benchmark for AMD Ryzen 3950X.

### Changed
- **Reduced RAM Usage for File Enumeration**: Now uses ~400 bytes per file on average.
  - Example: ~4GB RAM for 10 million files.
  - Details: [GitHub #104](https://github.com/fcorbelli/zpaqfranz/issues/104).
- **VSS Info Update**: Less alarming messages displayed during Volume Shadow Copy Service (VSS) operations.
- **Compiler Compatibility**: Refactored code for better support with modern compilers and fortification layers.


---

## [59.6] - 2024-05-17

### Fixed
- Minor bugs resolved, including warnings encountered with some cross-compilers.
- Improved Debian compatibility for better integration with Debian-based systems.

### Notes
- Released on a Friday the 17th.

---

## [59.5] - 2024-05-14

### Added
- **`hash` Command**: New simplified command for file hashing.
  - Outputs hashes as computed with alphabetical sorting (unlike `sum` which processes all files first).
  - Supports: `-ssd` (multithread), `-xxh3`, `-sha256`, `-stdout`, `-out <file>`.
  - Examples:
    - `hash z:\knb`: SHA1 of all files.
    - `hash z:\knb -ssd -xxh3`: XXH3 multithreaded.
    - `hash z:\knb -ssd -sha256 -stdout -out 1.txt`: SHA256 to file.
  - Without `-ssd`, hashes are written immediately.
- **`-home` Switch in `s` Command**: Calculates total folder size from depth 1.
  - Useful for sizing `/home`, `/users`, or VM stores.
  - Example: `zpaqfranz s c:\users -home -ignore`.
- **`-orderby` in `hash` Command**: Sorts output (e.g., by size with `-desc`).
  - Example: `zpaqfranz sum * -xxh3 -orderby size -desc`.
- **`-ignore` Switch**: Suppresses error messages (e.g., permission issues) during scanning.
  - Example: `zpaqfranz hash c:\users -ignore` vs. verbose errors with `-ssd`.

### Changed
- **Time Format**: Simplified `timetohuman` output (e.g., `00:00:00` instead of `0:00:00:00`).
- **Speed Units**: Replaced `/sec` with `/s` in speed information.

### Fixed
- Minor issues in `-noeta` switch.
- Improved update command on *nix systems.
  - Provides clearer update availability info (e.g., `Your 59.5h (2024-05-14) is not older...`).

### Notes
- Released version: `zpaqfranz v59.5h-JIT-GUI-L,HW BLAKE3,SHA1/2,SFX64 v55.1`.
- `-ignore` is risky as it hides errors; use cautiously.

---

## [59.4] - 2024-05-11

### Added
- **`crop` Command**: Deletes the latest version(s) from a non-multipart archive.
  - Dry run by default (test only).
  - Switches:
    - **`-kill`**: Performs an effective (wet) run.
    - **`-to <file>`**: Outputs to a new file (e.g., `tiny.zpaq`).
    - **`-until X`**: Discards versions beyond `X`.
    - **`-maxsize X`**: Cuts at size `X` (risky).
    - **`-force`**: Crops in-place without backup (very risky).
  - Examples:
    - `crop z:\1.zpaq`: Dry run info.
    - `crop z:\1.zpaq -to d:\2.zpaq -until 100 -kill`: Reduce to version 100.
    - `crop z:\1.zpaq -to d:\2.zpaq -maxsize 100k -kill`: Reduce to 100,000 bytes.
    - `crop z:\1.zpaq -until 2 -kill -force`: In-place crop to version 2.
- **Range in `l` (list) Command**: Filters files by version range.
  - Syntax:
    - `-range X:Y`: Versions X to Y.
    - `-range X:`: Versions X to last.
    - `-range :X`: Versions 1 to X.
    - `-range X`: Single version X.
    - `-range ::X`: Last X versions.
  - Examples:
    - `l z:\1.zpaq -range 2:3`: Files from versions 2–3.
    - `l z:\1.zpaq -range ::1`: Files from last version.
- **`-sfx` Flag (Win32)**: Creates a self-extracting archive directly.
  - Example: `zpaqfranz a z:\test.zpaq *.cpp -sfx`.
- **`-nomac` Flag**: Skips macOS `.DS_Store`, `Thumbs.db`, and similar files.
- **Cortex `franzomips` Benchmark**: Added benchmark for QNAP low-cost NAS CPUs.
- **`-verbose` in `dump` Command**: Now displays block offsets from file start.

### Changed
- **`c` Command**: Renamed `-verify` switch to `-checksum` to avoid collisions.

### Fixed
- **Backup Command on *nix**: Improved `./` auto-add functionality.
- **VSS Filename Handling (Win32)**: Automatically renames Volume Shadow Copy Service files.

### Notes
- Use `-force` and `-maxsize` in `crop` with caution due to risk of data loss.
- `-nomac` addresses clutter from macOS files on Samba NAS shares.

---

## [59.3] - 2024-04-19

### Added
- **`update`/`upgrade` Command**: Checks for newer zpaqfranz versions across platforms; downloads and updates on Win64.
  - Default source: `http://www.francocorbelli.it/zpaqfranz`.
  - Examples:
    - `zpaqfranz update`: Check for updates (all platforms).
    - `zpaqfranz update -force`: Update if newer (Win64).
    - `zpaqfranz update -force -kill`: Force download (Win64).
    - `zpaqfranz update <hash_url> <exe_url>`: Custom source (e.g., `https://www.pippo.com/ugo.sha256 http://www.pluto.com/zpaqnew.exe`).
- **`download` Command (Win64)**: Downloads files with optional hash verification.
  - Supports MD5/SHA-1/SHA-256; detects hash type by length (32=MD5, 40=SHA-1, 64=SHA-256).
  - Defaults: No overwrite (use `-force`), checks output path (use `-space` to bypass).
  - Examples:
    - `zpaqfranz download https://www.1.it/2.cpp ./2.cpp`: Download to `./2.cpp`.
    - `zpaqfranz download http://www.1.it/3.cpp z:\3.cpp -checktxt http://www.1.it/3.sha256`: Download with SHA-256 check.
    - `zpaqfranz download http://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz.exe ./thenewfile.exe -checktxt http://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz.md5`: Download with MD5 check.
- **Enhanced `ads` Command (Windows)**: Manages Alternate Data Streams (ADS).
  - Options: List (`ads z:\1.zpaq`), remove all (`-kill`), remove specific (`-only <name> -kill`), rebuild (`-force`).
  - Example: `zpaqfranz ads z:\*.zpaq -kill`.
- **`-fasttxt` with ADS**: Stores updated CRC-32 in ADS for archive integrity checks without decryption.
  - Example: `zpaqfranz a z:\pippo.zpaq c:\dropbox -fasttxt -ads -key pippo`, then `zpaqfranz versum z:\pippo.zpaq`.
- **`pause` Command Enhancement**: Waits for a specific keypress.
  - Example: `zpaqfranz pause -find z` (waits for 'z').
- **Updated `franzomips` Benchmark**: Added AMD Ryzen 7950X3D CPU results.

### Fixed
- Improved compatibility and stability for Windows 7 64-bit.

### Notes
- **Security Warning**: Use trusted sources for `update`/`download` (e.g., GitHub, SourceForge, or `https://www.francocorbelli.it/zpaqfranz/win64/`).
- `ads` command is under development; expect further refinements.
- `-fasttxt` enables fast integrity checks (e.g., >2GB/s on NVMe) without needing encryption keys; Samba/PAKKA integration in progress.
- `franzomips` disables HW-acceleration by default; use specific flags (e.g., `-sha256`) for HW benchmarks.

---

## [59.2] - 2024-02-23

### Added
- **`pakka` Command**: Integrates with PAKKA, a Windows GUI for zpaqfranz.
  - Supports newer PAKKA versions without requiring `zpaqlist`.
  - Features in development: file addition, testing, and full functionality beyond extraction.
  - Examples:
    - `zpaqfranz pakka h:\zarc\1.zpaq -out z:\default.txt`: Lists to file.
    - `zpaqfranz pakka h:\zarc\1.zpaq -all -distinct -out z:\default.txt`: Lists without deduplication.
    - `zpaqfranz pakka h:\zarc\1.zpaq -until 10 -out z:\10.txt`: Lists up to version 10.

### Fixed
- Minor unspecified issues.

### Notes
- PAKKA is a freeware Windows GUI available at [https://www.francocorbelli.it/pakka/build/latest/pakka_latest.zip](https://www.francocorbelli.it/pakka/build/latest/pakka_latest.zip) with built-in auto-update functionality.
- Ongoing development includes online help and ADS (Alternate Data Stream) support for small NAS systems like TrueNAS.
- PAKKA, written in Delphi, evolves faster than zpaqfranz core.

---
## [59.1] - 2024-01-16

### Added
- **`-chunk` Switch in `a` (add)**: Creates fixed-size multipart archives.
  - Supports sizes: raw numbers (e.g., `2000000`), `K`/`M`/`G`, `KB`/`MB`/`GB` (e.g., `-chunk 1G`, `-chunk 500MB`).
  - Chunks approximate multiples of 64KB; not fully integrated (e.g., unsupported in `backup`).
  - Examples:
    - `zpaqfranz a z:\ronko_?? whatever-you-like -chunk 1G`.
    - `zpaqfranz a z:\ronko_?? who-knows -chunk 500m`.
- **ADS Filelists (Windows, NTFS)**: Stores file lists in Alternate Data Streams for unencrypted archives.
  - Uses LZ4 compression for speed and chunked storage.
  - Example: `zpaqfranz a z:\1.zpaq *.cpp -ads`, then `zpaqfranz l z:\1.zpaq`.
  - Force standard listing: `zpaqfranz l z:\1.zpaq -ads`.
- **`ads` Command (Windows)**: Manipulates ADS filelists.
  - Show: `ads z:\1.zpaq`.
  - Rebuild: `ads z:\1.zpaq -force`.
  - Remove: `ads z:\*.zpaq -kill`.
- **`-fast` Switch in `a` (add)**: Experimental feature to store a partial file list in the archive.
  - Aims for faster extraction; maintains compatibility with zpaq 7.15.
  - Example: `zpaqfranz a z:\1.zpaq c:\dropbox -fast`, then `zpaqfranz l z:\1.zpaq` (auto-detects `-fast`, else `-fast` to force).
- **Console Colors (Windows)**: Limited color support for black-background consoles.
  - Disabled with `NO_COLOR` env variable or `-nocolor` switch.
- **Debug Switches**: Enhanced debugging options.
  - `-debug`, `-debug2`, `-debug3`: Increasing verbosity.
  - `-debug4`: Writes debug files to `z:\` if available.
- **`-longpath` for Files**: Rudimentary support for individual files >255 characters on Windows.
  - Uses `GetFinalPathNameByHandleW` from `KERNEL32.DLL`.
  - Details: [GitHub #90](https://github.com/fcorbelli/zpaqfranz/issues/90).

### Changed
- **Password Prompt**: Unified at the file-handling class level.
  - Applies to all commands (e.g., `dump`); prompts if `-key` omitted, avoiding command history exposure.

### Notes
- This is a new branch with untested features; bugs are expected—report them at [GitHub Issues](https://github.com/fcorbelli/zpaqfranz/issues).
- `-chunk` aims for optical media compatibility (e.g., Blu-ray); Ctrl+C handling and part estimation are incomplete.
- ADS with LZ4 enables potential filesystem mounting in future; encryption support untested.
- `-fast` is experimental and currently provides minimal utility; full implementation is complex.
- Color support on *nix is under consideration but challenging due to interoperability.
- Additional discussions: [Color Support](https://encode.su/threads/4182-Color-or-not), [Format Hacking](https://encode.su/threads/4178-hacking-zpaq-file-format-(!)), [Data Storage](https://encode.su/threads/4168-Virus-like-data-storage-(!)).

---
 
## [58.12.s] - 2023-12-08

### Changed
- Improved execution speed and deduplication (`redup`).
- Utilizes point-in-time copy mechanisms (e.g., hourly snapshots), eliminating the need to scan the entire filesystem.
- Previously, ZFS backup support operated at the block level, requiring full restoration to recover individual files. The new `-dataset` switch enables efficient file-level updates by leveraging ZFS snapshots.
- Ideal for large fileservers or systems with magnetic disks where filesystem scans are slow (e.g., tar, 7z, srep, etc.).

### Performance Example
- For a mid-sized file server with 1M files:
- Spinning drives: ~500 files/sec → ~30 minutes just to enumerate files.
- SSDs: ~5K files/sec.
- NVMe: ~30K files/sec.
- Traditional tools require full enumeration before processing, making frequent backups (e.g., every 10 minutes) impractical.
- With `-dataset`, `zpaqfranz` uses ZFS snapshots to identify changes instantly, enabling rapid updates.

### How It Works
- On first run: Creates a base snapshot (e.g., `tank/d@franco_base`).
- Subsequent runs: Generates a temporary snapshot (e.g., `tank/d@franco_diff`), compares it with the base, and processes only changed files.
- Example output:
---


## [58.11.s] - 2023-11-10

## Big "news": Developing (underway) to handle SHA-1 collisions
- Work in progress to address SHA-1 collision handling in `zpaqfranz`.

### Disclaimer: Is this a real issue? Can my backups become broken?
- SHA-1 collisions have been demonstrated in controlled lab environments, but in real-world scenarios, they are extremely unlikely (bordering on impossible).
- Backups made with `zpaqfranz` are considered safe. The `t` command has included collision detection for years, ensuring archive integrity.
- The new `collision` command offers a faster collision-specific test compared to the comprehensive `t` command, which also checks file integrity.
- Paranoid-level commands and switches are available for extra caution.
- Maintaining backward compatibility with `zpaq 7.15` remains a significant challenge.

## New switch `-collision` in add
- `zpaqfranz` can now recover from SHA-1 collisions within the current archive version, ensuring correct file extraction for paranoid users.

### Collision-aware `zpaqfranz` in action: Detecting and fixing
#### Older `zpaqfranz` (default behavior):
- No collision detection by default:

---


## [58.10] - 2023-09-21

## New `-home` switch for add
- Enables archiving different folders into separate `.zpaq` files, useful for splitting individual users (e.g., within `/home` or `c:\users`) into distinct archives.
- Examples:
```
  zpaqfranz a z:\utenti.zpaq c:\users -home
  zpaqfranz a /temp/test1 /home -home -not franco
  zpaqfranz a /temp/test2 /home -home -only "*SARA" -only "*NERI"
```

## Support for selections in `r` (robocopy) command
- File selection now mirrors the `add` command functionality.
- Examples:
```
  zpaqfranz r c:\d0 z:\dest -kill -minsize 10GB
  zpaqfranz r c:\d0 z:\dest -kill -only *.e01 -only *.zip
```

## Fixes and Improvements
### Fix for Mac PowerPC
- Added support for compiling `zpaqfranz` on PowerPC Macs.

### Improved compatibility with ancient compilers on Slackware
- Enhanced support for very old GCC versions commonly used in Slackware.

### Workaround for buggy GCC versions
- Addressed issues in newer GCC releases.

### Refactoring
- Codebase cleaned up for better maintainability, though slightly slower.

## Variable Replacements
### Replaced `$` with `%` in variables
- Linux scripts do not handle `$` well, so variables now use `%`.
- Supported variables:
- `%hour`
- `%min`
- `%sec`
- `%weekday`
- `%year`
- `%month`
- `%day`
- `%week`
- `%timestamp`
- `%datetime`
- `%date`
- `%time`
- Example:
```
  zpaqfranz r c:\d0 z:\backup_%day -kill
```

## Enhanced `-orderby` switch in `add`
- Examples:
```
  zpaqfranz a z:\test.txt c:\dropbox -orderby ext;name
  zpaqfranz a z:\test.txt c:\dropbox -orderby size -desc
```

## Filecopy with variable buffer size (`-buffer`)
- Added for testing across different platforms.

## Updated `versum` command
- Now processes only files starting with `|`.

## New Disclaimer After Help
- Added reminder to use double quotes for arguments:
```
  ************ REMEMBER TO USE DOUBLE QUOTES! ************
  *** -not .cpp    is bad,    -not ".cpp"    is good ***
  *** test_???.zpaq is bad,    "test_???.zpaq" is good ***
```



## [55.6] - 2022-07-26

### Added
- **HW-Accelerated SHA1 (Windows 64)**: New executable `zpaqfranzhw.exe` with `-hw` switch.
  - Utilizes CPU SHA1 instructions (common on AMD Ryzen since 2017; limited Intel support).
  - Modest performance gain; requires `-hw` on supported CPUs.
- **Advanced Error Reporting (Windows)**: Filesystem errors logged after `add()`.
  - Brief output with `-verbose`; detailed with `-debug`.
  - Example:
    - `-verbose`: `Error 00000032 #1 |sharing violation|`
    - `-debug`: Includes file attributes (e.g., `c:/pagefile.sys>> ARCHIVE;HIDDEN;SYSTEM;`).
- **Refactored Help**: Improved readability and conciseness.
  - "No command" output tightened.
  - Help accessible via `/?`, `h`, or `-h`.
- **C: Drive Backup Commands (`g` and `q`)**: Refined with default exclusions:
  - `c:\windows`, `RECYCLE BIN`, `%TEMP%`, `ADS`, `.zfs`, `pagefile.sys`, `swapfile.sys`, `System Volume Information`, `WindowsApps`.
  - Switches:
    - `-frugal`: Excludes `Program Files` and `Program Files (x86)`.
    - `-forcewindows`: Includes `C:\WINDOWS`, `$RECYCLE.BIN`, `%TEMP%`, `ADS`, `.zfs`.
    - `-all`: Includes all except `pagefile.sys`, `swapfile.sys`, `System Volume Information`, `WindowsApps`.
- **Reparse Point Support**: Initial implementation for handling Windows special files (e.g., in `WindowsApps`).

---

## [55.4] - 2022-07-20

### Added
- **`q` (paQQa) Command (Windows)**: Archives C: drive with exclusions (`swap files`, `System Volume Information`, `Windows`).
  - Requires admin rights; deletes pre-existing VSS; creates `C:\FRANZSNAP`.
  - Example: `zpaqfranz q z:\pippo.zpaq -only *.cpp -key mightypassword`.
  - Supports most `add()` switches except `-to`.
  - `-forcewindows`: Adds `c:\windows`, `%TEMP%`, `ADS`.

### Notes
- Not a bare-metal backup; optimized for fast snapshot archiving (~2 minutes for 200GB on test PC).
- Some folders (e.g., Windows Defender) inaccessible; under investigation.

---

## [55.2] - 2022-07-16

### Changed
- **`r` (robocopy) Command**: Tentative `-longpath` support on Windows (use with caution).
- Source code converted to Unix text format (no CR/LF) for compatibility with old `make` systems.

### Fixed
- Minor help text corrections.

---

## [55.2] - 2022-07-15

### Added
- **`-touch` Switch in `a` (add)**: Forces timestamp changes to convert zpaq 7.15 archives to zpaqfranz format in-place.
  - Example: `zpaqfranz a z:\1.zpaq c:\nz\ -touch` then `zpaqfranz a z:\1.zpaq c:\nz\`.

### Changed
- Improved ETA computation accuracy.
- Enhanced `-verify` handling for legacy zpaq 7.15 archives.
- **`-verbose`**: Shows progress for files >100MB (10%) or >1GB (1%).

### Notes
- Addresses lack of feedback during large file updates (e.g., virtual disks).

---

## [55.1] - 2022-07-09

### Added
- **OpenBSD 6.6+ and OmniOS Support**: Compilation support added.
- **SFX Module Update (Windows)**: Prompts for extraction location if `-to` omitted.
- **`-flagflat`**: Uses mime64-encoded filenames for NTFS reserved word issues.
- **`-fixcase` and `-fixreserved` (Windows)**: Handles case collisions and reserved filenames.
- **`-ssd`**: Replaces `-all` for multithread computation.
- **`d` Command**: Supports various hashes (e.g., `-blake3`).
  - Example: `zpaqfranz d c:\dropbox\ -ssd -blake3`.
- **`dir` Command**: Hash-based duplicate detection.
  - Example: `zpaqfranz dir c:\dropbox\ /s -checksum -blake3`.
- **`a` (add) Debug Switches**:
  - `-debug -zero`: Adds zero-filled files.
  - `-debug -zero -kill`: Adds 0-byte files for debugging.
- **`i` (info)**: ~30% faster; `-stat` shows collision stats.
- **`rd` Command (Windows)**: Deletes stubborn folders.
  - Switches: `-force`, `-kill`, `-space`.
- **`w` Command**: Chunked extraction/testing to disk or RAM.
  - Scenarios:
    1. HDD extraction: `zpaqfranz w z:\1.zpaq -to p:\muz7\ -ramdisk -longpath`.
    2. Hash checking: `zpaqfranz w z:\1.zpaq -ramdisk -test -checksum -ssd -frugal -verbose`.
    3. Large archive check: `zpaqfranz w z:\1.zpaq -to z:\muz7\ -paranoid -verify -verbose -longpath`.
    4. SSD/RAM check: `zpaqfranz w z:\1.zpaq -to z:\kajo -ramdisk -paranoid -verify -checksum -longpath -ssd`.
  - Switches: `-maxsize`, `-ramdisk`, `-frugal`, `-ssd`, `-test`, `-verbose`, `-checksum`, `-verify`, `-paranoid`.

### Changed
- Reduced execution output; controlled with `-noeta`, `-pakka`, `-summary` (less) or `-verbose`, `-debug` (more).
- **`dir` Command**: Defaults to European date format (e.g., `25/12/2022`).
- Disabled slow checks; re-enabled with `-stat`.

### Notes
- Primarily tested on Windows; BSD/Linux focus planned for 55.2+.
- `w` command designed for large archives and HDDs; requires empty `-to` folder.

---

## [54.10] - 2021-12-20

### Added
- Media full check.
- `franzomips` support.
- **`-checksum` in `t` (test)**.

---

## [54.9] - 2021-11-04

