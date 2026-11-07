# zpaqfranz on *nix: how the binaries are made

Every release has 20 *nix binaries. Eleven are for NAS: [NAS.md](NAS.md) explains them from zero,
and anyone can rebuild them to the byte. This page is about the other nine: the machine, the
compiler, the **complete command line**, and what comes out. No explanations beyond the
necessary: it is a record, so that nobody has to guess how a binary was made.

All of them come from the same source, the `zpaqfranz.cpp` of the release, the one on GitHub. Here,
65.18r: MD5 `da6d72ccfc33de4f50d0ff30f8781d66`.

Unlike the NAS ones, these are **not reproducible to the byte**: they are built with the compiler of
each system (which changes with its updates), most are not stripped, some carry debug sections with
the paths of the build. What is documented is how they are made, so that you can make your own the
same way.

Windows is not here: I build it myself, apart.

---

## In short

| Binary                | System                         | Compiler                    | Linking                | `mount` | MD5 (65.18r)                       |
|-----------------------|--------------------------------|-----------------------------|------------------------|:-------:|------------------------------------|
| `zpaqfranz_esx`       | CentOS 3.9 (for VMware ESXi)   | gcc 3.4.6                   | static, 32 bit         |         | `15ffd133a1f8ee50ca08dca7f37ea4ee` |
| `zpaqfranz_solaris`   | Oracle Solaris 11.4 x86        | gcc 7.3.0                   | dynamic                |         | `498ca8f5652989124f65d4c411552682` |
| `zpaqfranz_oracle79`  | Oracle Linux 7.9               | gcc 4.8.5                   | dynamic (glibc 2.17)   |         | `d7c64b37a06ea972e6d8bde8d5def683` |
| `zpaqfranz_opensuse`  | openSUSE Tumbleweed (2023-08)  | gcc 13.2.1                  | dynamic (glibc 2.38)   |   yes   | `14d1bcccd527332e4ba89caa1858dd17` |
| `zpaqfranz_fedora`    | Fedora 44                      | gcc 16.2.1                  | dynamic (glibc 2.43)   |   yes   | `03afa750ee549fcdfc070a820a6b58e2` |
| `zpaqfranz_arch`      | Arch Linux                     | gcc 15.2.1                  | dynamic                |   yes   | `43e912f3d37a860ace5e8b35362a634e` |
| `zpaqfranz_freebsd`   | FreeBSD 14.2                   | clang 18.1.6                | dynamic                |   yes   | `ec20e81b61ac6e0ceb01c06a4efc77e2` |
| `zpaqfranz_openbsd`   | OpenBSD 7.9                    | clang 19.1.7                | dynamic                | yes (FUSE 2) | `4c2c64d0e57e205abaa500cc668df0d1` |
| `zpaqfranz_macos`     | macOS 12.7.2 (Intel)           | Apple clang 14.0.0          | dynamic, universal     |         | `0416aa1e21424a1035dc548af6a78c52` |

Every build of 65.18r ended with `rc=0`, and its tests on its machine passed: `autotest -all`, or
`autotest` and `a`/`t`/`x` with the methods and the hashes on ESXi and Solaris. The arm64 half of
the macOS binary is not run there: the Mac is an Intel.

---

## `zpaqfranz_esx`: VMware ESXi

```
system:   CentOS release 3.9 (Final), kernel 2.4.21-50.ELsmp, glibc 2.3.2
compiler: g++ (GCC) 3.4.6, GNU ld 2.14.90.0.4 20030523
```

```
g++ -O3 -Wall -DESX zpaqfranz.cpp -o zpaqfranz_esxi -pthread -static -s
```

```
result:   ELF 32-bit LSB executable, Intel 80386, for GNU/Linux 2.2.5, statically linked, stripped
needs:    nothing
65.18r:   rc 0, 18 warnings; zpaqfranz v65.18r-ESX,4,(2026-11-07)
```

Renamed `zpaqfranz_esx`. `-DESX` means `ANCIENT` (C++98: gcc 3.4 has no C++11), no zstd (no `-m7`
`-m8` `-m9`: the levels by name are the zpaq models), no libsodium, IPv4. Static and 32 bit,
because the ESXi shell has almost nothing.

---

## `zpaqfranz_solaris`: Solaris

```
system:   Oracle Solaris 11.4 X86 (SunOS 5.11 11.4.0.15.0 i86pc)
compiler: g++ (GCC) 7.3.0 (/usr/bin/g++), Solaris Link Editors 5.11-1.3159
```

```
PATH=/usr/gnu/bin:$PATH
g++ -O3 -Wall -DSOLARIS zpaqfranz.cpp -o zpaqfranz_solaris -pthread -static-libgcc -lkstat
```

```
result:   ELF 64-bit LSB executable AMD64, dynamically linked, not stripped
needs:    libkstat.so.1 libstdc++.so.6 libm.so.2 librt.so.1 libpthread.so.1 libc.so.1
65.18r:   rc 0, 11 warnings; zpaqfranz v65.18r-JIT8,-L4,(2026-11-07)
```

`-lkstat` for the number of cores and the memory. `libstdc++.so.6` is the one of the gcc runtime package of
Solaris (`/usr/lib/64`).

---

## `zpaqfranz_oracle79`: Oracle Linux 7 / RHEL 7 / CentOS 7

```
system:   Oracle Linux 7.9, glibc 2.17
compiler: g++ (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44.0.3)
```

```
g++ -O3 -Wall -Dunix -std=c++11 zpaqfranz.cpp -pthread -ldl -o zpaqfranz
```

```
result:   ELF 64-bit LSB executable, x86-64, dynamically linked, for GNU/Linux 2.6.32, not stripped
needs:    libdl.so.2 libstdc++.so.6 libm.so.6 libgcc_s.so.1 libpthread.so.0 libc.so.6
65.18r:   rc 0, 0 warnings; zpaqfranz v65.18r-JIT8,-L4,(2026-11-07)
```

Renamed `zpaqfranz_oracle79`. gcc 4.8 builds C++98 by default: `-std=c++11` is needed (without it
the source stops with an `#error` that says so). `-ldl`: before glibc 2.34 `dlopen` (libsodium,
only with `-libsodium`) is in `libdl`. No `mount`.

---

## `zpaqfranz_opensuse`: openSUSE

```
system:   openSUSE Tumbleweed (snapshot of August 2023), glibc 2.38, libfuse3-3 3.16.1
compiler: g++ (SUSE Linux) 13.2.1 20230803, GNU ld 2.40.0.20230412
```

```
printf '#ifndef LIBFUSE_CONFIG_H\n#define LIBFUSE_CONFIG_H\n#define LIBFUSE_BUILT_WITH_VERSIONED_SYMBOLS 1\n#endif\n' > $HOME/fuse3inc/fuse3/libfuse_config.h
g++ -O3 -Wall -Dunix -DHWSHA2 -DZPAQMOUNT zpaqfranz.cpp -o zf -pthread -I$HOME/fuse3inc/fuse3 /usr/lib64/libfuse3.so.3
```

```
result:   ELF 64-bit LSB executable, x86-64, dynamically linked, for GNU/Linux 3.2.0, not stripped
needs:    libfuse3.so.3 libstdc++.so.6 libm.so.6 libgcc_s.so.1 libc.so.6
65.18r:   rc 0, 0 warnings; zpaqfranz v65.18r-JIT8+M,-L,HW SHA1/2,4,(2026-11-07)
```

Renamed `zpaqfranz_opensuse`. On that machine `fuse3-devel` is not installed: the headers of libfuse
3.16.1 are in `~/fuse3inc/fuse3` (hence the `printf` of `libfuse_config.h`, that the package would
give) and the library is linked by its path. With `fuse3-devel` the line is the usual one:
`-I/usr/include/fuse3 -lfuse3`. `-DHWSHA2`: SHA-1 and SHA-256 with the CPU instructions, where
there are.

---

## `zpaqfranz_fedora`: Fedora

```
system:   Fedora release 44, kernel 7.2.7, glibc 2.43, fuse3 3.18.2
compiler: g++ (GCC) 16.2.1 20260819 (Red Hat 16.2.1-2), GNU ld 2.46.1
```

```
g++ -O3 -Wall -DZPAQMOUNT zpaqfranz.cpp -o zmz6518r_o3 -pthread $(pkg-config fuse3 --cflags --libs)
```

`pkg-config fuse3 --cflags --libs` there is `-I/usr/include/fuse3 -lfuse3 -lpthread`.

```
result:   ELF 64-bit LSB executable, x86-64, dynamically linked, for GNU/Linux 3.2.0, not stripped
needs:    libfuse3.so.4 libstdc++.so.6 libm.so.6 libgcc_s.so.1 libc.so.6
65.18r:   rc 0, 8 warnings; zpaqfranz v65.18r-JIT8+M,-L4,(2026-11-07)
```

Renamed `zpaqfranz_fedora`.

---

## `zpaqfranz_arch`: Arch Linux

```
system:   Arch Linux (rolling, as of November 2026)
compiler: g++ (GCC) 15.2.1 20260209
```

```
g++ -O3 -Wall -DZPAQMOUNT zpaqfranz.cpp -o zf_mount -pthread -I/usr/include/fuse3 -lfuse3
```

```
result:   ELF 64-bit LSB pie executable, x86-64, dynamically linked, for GNU/Linux 4.4.0, not stripped
needs:    libfuse3.so.4 libstdc++.so.6 libm.so.6 libgcc_s.so.1 libc.so.6
65.18r:   rc 0, 2 warnings (-Wformat-truncation); zpaqfranz v65.18r-JIT8+M,-L4,(2026-11-07)
```

Renamed `zpaqfranz_arch`. No `pkg-config` on that machine: the fuse3 options by hand.

---

## `zpaqfranz_freebsd`: FreeBSD

```
system:   FreeBSD 14.2
compiler: FreeBSD clang version 18.1.6 (llvmorg-18.1.6-0-g1118c2e05e67), LLD 18.1.6
```

```
clang++ -O3 -Dunix -DZPAQMOUNT zpaqfranz.cpp -o zf_mount -pthread `pkg-config fuse3 --cflags --libs` -Wall -Wextra
```

```
result:   ELF 64-bit LSB executable, x86-64, dynamically linked, for FreeBSD 14.2, not stripped
needs:    libfuse3.so.4 libthr.so.3 libc++.so.1 libcxxrt.so.1 libm.so.5 libgcc_s.so.1 libc.so.7
65.18r:   rc 0, 11 warnings (with -Wextra); zpaqfranz v65.18r-JIT8+M,-L4,(2026-11-07)
```

Renamed `zpaqfranz_freebsd`. fuse3 is the package `fusefs-libs3`.

---

## `zpaqfranz_openbsd`: OpenBSD

```
system:   OpenBSD 7.9
compiler: OpenBSD clang version 19.1.7, LLD 19.1.7
```

```
clang++ -O3 -Dunix -DZPAQMOUNT -Wall z6518r.cpp -o z6518r -pthread -lfuse
```

```
result:   ELF 64-bit LSB pie executable, x86-64, dynamically linked, for OpenBSD, not stripped
needs:    libfuse.so.4.0 libc++.so.12.0 libc++abi.so.9.0 libpthread.so.28.1 libm.so.10.1 libc.so.103.0
65.18r:   rc 0, 0 warnings; zpaqfranz v65.18r-JIT8+M,-L4,(2026-11-07)
```

Renamed `zpaqfranz_openbsd`. The FUSE of OpenBSD is FUSE 2, in the base system: `-lfuse`, nothing
to install. There the source is copied as `<name>.cpp` (here `z6518r.cpp`), and that name is the
one an `assert` keeps inside the binary: with `zpaqfranz.cpp` it is the same program.

---

## `zpaqfranz_macos`: macOS, universal (Intel + Apple Silicon)

```
system:   macOS 12.7.2 (21G1974), Intel
compiler: Apple clang version 14.0.0 (clang-1400.0.29.202), Command Line Tools, SDK 13.1
```

```
clang++ -arch x86_64 -mmacosx-version-min=10.15 -O3 -std=c++11 -Dunix -DHWSHA2 -Wall zpaqfranz.cpp -o zf_x86 -pthread
clang++ -arch arm64 -mmacosx-version-min=11.0 -O3 -std=c++11 -Dunix -DNOJIT -Wall zpaqfranz.cpp -o zf_arm -pthread
lipo -create zf_x86 zf_arm -output zf_uni
cp zf_uni zpaqfranz_macos
strip zpaqfranz_macos
```

```
result:   Mach-O universal binary with 2 architectures: [x86_64] [arm64]
          x86_64: minimum macOS 10.15; arm64: minimum macOS 11.0
needs:    /usr/lib/libc++.1.dylib /usr/lib/libSystem.B.dylib
65.18r:   rc 0, 0 warnings (both); x86_64: zpaqfranz v65.18r-JIT8,-L,HW SHA1/2,4,(2026-11-07)
```

`-DNOJIT` on arm64: the JIT of zpaq writes x86 code. No `mount` in the release binary: it would
need macFUSE or FUSE-T on every Mac (the two builds with `mount` are made at every release too, see
below).

---

## The options, one line each

| Option            | Meaning |
|-------------------|---------|
| `-Dunix`          | the *nix code. gcc and clang define `unix` by themselves in their default (GNU) mode on Linux and BSD, not with `-std=c++11` and not on macOS |
| `-std=c++11`      | zpaqfranz is C++11: needed where the compiler defaults to an older standard (gcc 4.8 and 5); on macOS, to be safe |
| `-DESX`           | ESXi: `ANCIENT` (C++98), no zstd, IPv4, no libsodium |
| `-DSOLARIS`       | Solaris: its system calls and headers |
| `-DNAS`           | the NAS builds, see [NAS.md](NAS.md) |
| `-DANCIENT`       | for compilers with no C++11: less inside (no zstd, the plain `add`, no libsodium) |
| `-DNOJIT`         | no JIT (anything that is not x86 or x86-64) |
| `-DHWSHA2`        | SHA-1 and SHA-256 with the CPU instructions (SHA-NI), used only if the CPU has them (checked at run time) |
| `-DZPAQMOUNT`     | the `mount` command, with FUSE 3 (FUSE 2 on OpenBSD) |
| `-ldl`            | glibc before 2.34: `dlopen` (libsodium, only with `-libsodium`) |
| `-lkstat`         | Solaris: the number of cores and the memory |
| `-static`         | one file, nothing needed from the system (ESXi, NAS) |
| `-s` / `strip`    | no symbols: a smaller file |

libsodium is never linked: with `-libsodium` it is loaded at run time (`dlopen`), if it is there.
Franzen does not need it: franzsodium is built in.

---

## Built at every release, not published

The same source is built in other ways too, to catch errors and warnings early:

```
# Arch, FreeBSD, openSUSE, Fedora: the build without mount
g++ -O3 -Wall zpaqfranz.cpp -o zf_plain -pthread                                    # Arch
clang++ -O3 -Dunix zpaqfranz.cpp -o zf_plain -pthread -static -Wall                 # FreeBSD
g++ -O3 -Wall -Dunix zpaqfranz.cpp -o zf_plain -pthread                             # openSUSE
g++ -O3 -Wall zpaqfranz.cpp -o zfz6518r_o3 -pthread                                 # Fedora

# Fedora: -O1, and the syntax as C++11, ANCIENT and NAS
g++ -O1 -Wall zpaqfranz.cpp -o z6518r -pthread
g++ -std=c++11 -Wall -fsyntax-only zpaqfranz.cpp
g++ -DANCIENT -Wall -fsyntax-only zpaqfranz.cpp
g++ -DNAS -Wall -fsyntax-only zpaqfranz.cpp

# Oracle Linux 7.9: the line for compilers with no C++11
g++ -DANCIENT -O3 zpaqfranz.cpp -o zpaq_anc -pthread

# macOS: mount with macFUSE, and with FUSE-T
clang++ -O3 -std=c++11 -Dunix -DZPAQMOUNT -Wall zpaqfranz.cpp -o zf_macfuse -pthread -I/usr/local/include/fuse3 -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lfuse3
clang++ -O3 -std=c++11 -Dunix -DZPAQMOUNT -Wall zpaqfranz.cpp -o zf_ft -pthread -I"/Library/Application Support/fuse-t/include" -L"/Library/Application Support/fuse-t/lib" -Wl,-rpath,"/Library/Application Support/fuse-t/lib" -lfuse3
```

---

## How they get to the release

At every release the source goes to a new folder on each machine, and each one builds and runs its
tests there (`autotest`, `a`/`t`/`x`, a few quick checks of the commands and of Ctrl+C). Then the binaries come back, each one only if its MD5 here is its
MD5 on the machine, and the MD5 of the source on the machine is the MD5 of the source of the
release, and the version inside is the right one. The 20 *nix binaries are replaced together, or
not at all.
