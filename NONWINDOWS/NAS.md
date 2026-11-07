# zpaqfranz on a NAS: building the binaries yourself

zpaqfranz runs on most NAS: Synology, QNAP, Asustor, TerraMaster, WD, and the home-made ones on
a Raspberry Pi.  
For them I publish eleven ready binaries, one per CPU family.

But you should not have to trust them. zpaqfranz is open source, and a backup program is the
last place where a binary from a stranger 😄 belongs. 

This page explains, from zero, how to build
the same eleven binaries on any Debian machine: which compilers, which options, why each one,
how to try them without a NAS, and how to find out which one your NAS wants.

And one more thing: with the same compilers, the same options and the same source, **you get my
binaries to the byte** (see [The same bytes as mine](#5-the-same-bytes-as-mine)). So you can
check them, instead of trusting them.


---

## In short

On a Debian machine (x86_64; a VM is fine), with the scripts of the appendices:

```
apt install curl ca-certificates file qemu-user-static
sh nas_toolchains.sh /opt/musl                     # 5 cross compilers, checked with SHA-512
bash nas_build.sh /opt/musl ./zpaqfranz.cpp ./bin  # the 11 binaries, about 8 minutes
md5sum bin/zpaqfranz_*                             # compare them with mine
sh nas_qemu.sh ./bin                               # each one on its own CPU, emulated
```

Then, from any machine that can `ssh` to the NAS:

```
ssh admin@mynas sh -s < nas_choose.sh              # which binary: nothing copied, no binary run
```

---

## 1. What a NAS binary is, and why it is made this way

### Static, with musl

A NAS is a small Linux box, usually with an old kernel, an old C library and no compiler. Every
brand, every model and every firmware has its own. A normal (dynamic) binary needs the
libraries of the system it was built on: on a NAS they are older, or different, or missing.

So the NAS binaries are **static**: one file, with everything inside, that needs nothing from the
system. Copy it, `chmod +x`, run it.

And they are static with **musl**, not glibc:

- **Old kernels.** A static glibc carries the minimum kernel it was built for: Debian 11's
  glibc stops with `FATAL: kernel too old` on anything before Linux 3.2. Many NAS still in
  service have 2.6.32 or 3.x. musl uses only the basic system calls, so its static binaries run
  on very old kernels too.
- **Truly static.** glibc never is, completely: user names, host names and `dlopen` still look
  for shared libraries at run time. musl has none of this.
- **Small and simple.** musl is a small, clean C library, made for exactly this.

### `-DNAS`: what is inside, what is not

`-DNAS` is a build of zpaqfranz "reduced" for small machines. In the source it means:

- it is an `ANCIENT` build: the old, plain `add` (no `-turbo`), and **the default method is
  `-m1`**, as zpaq 7.15. The levels by name are all there: on a NAS, write `-lean`;
- **zstd stays** (`-m8`, `-m80`, `-m9`, `-m7`): the cross compilers are modern, and zstd is
  exactly what a small NAS wants: little CPU to compress, and a native decoder to extract;
- **Franzen is there**, with franzsodium, built in: no libsodium, no `dlopen`;
- **out**: the hardware SHA (`HWSHA1`, `HWSHA2`, `HWBLAKE3`), the `mount` (FUSE), SFTP.

The banner says what you have: `zpaqfranz v65.18r-NAS8,-L4,(2026-11-07)` is a NAS build (`NAS`)
with zstd (`8`), little endian (`-L`), IPv4 (`4`).

### `-DNOJIT` on ARM

zpaq has a JIT: it turns the ZPAQL of the models (`-m1` ... `-m5`) into x86 machine code. It
exists for x86 only. On ARM the models are interpreted, with `-DNOJIT`: without it the program
would try to run x86 code on an ARM CPU. That is one more reason why `-m8`/`-m80`, with their
native zstd decoder, are the right methods on an ARM NAS. The two x86 binaries keep the JIT.

### The compile line, option by option

```
<triple>-g++ -O3 -DNAS -static -no-pie -pthread -s -Wall <CPU options> zpaqfranz.cpp -o <name>
```

| Option           | Why |
|------------------|-----|
| `-O3`            | the usual optimization of every zpaqfranz build |
| `-DNAS`          | the reduced build above |
| `-static`        | one file, nothing needed from the NAS |
| `-no-pie`        | a plain static executable (not a static-PIE one): the oldest and simplest kind, that every kernel loads |
| `-pthread`       | zpaqfranz is multithreaded |
| `-s`             | no symbols: a smaller file (6-8 MB) |
| `-Wall`          | the warnings, always on |
| `-DNOJIT`        | ARM only, see above |
| CPU options      | `-march` / `-mcpu` / `-mfpu` / `-mfloat-abi`: the next chapter |

---

## 2. Eleven binaries: the platforms, and why so many

| Binary                      | Compiler (musl.cc)        | CPU options                                       | NAS (indicative list)                                        |
|-----------------------------|---------------------------|---------------------------------------------------|--------------------------------------------------------------|
| `zpaqfranz_nas_x86_64`      | `x86_64-linux-musl`       | `-march=x86-64`                                   | Intel and AMD at 64 bit: Atom C, Celeron J/N, Pentium, Ryzen |
| `zpaqfranz_nas_i686`        | `i686-linux-musl`         | `-march=i686`                                     | Intel at 32 bit: Atom D410/D510/D525, CE5335 "Evansport"     |
| `zpaqfranz_armv5`           | `armv5l-linux-musleabi`   | `-march=armv5te -mfloat-abi=soft`                 | Marvell Kirkwood: DS109/110j/210j/211j/212j/112j, QNAP TS-x19, TS-x12, TS-x10 |
| `zpaqfranz_armv7`           | `armv7l-linux-musleabihf` | `-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard` | Marvell Armada 370/XP (DS213j, DS214se, DS414), and any ARMv7 not listed |
| `zpaqfranz_armv7_cortexa9`  | `armv7l-linux-musleabihf` | `-mcpu=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=hard`| Armada 375/385/388 (DS215j, DS216j, DS218j, DS116), Comcerto 2000, STM Monaco |
| `zpaqfranz_armv7_cortexa15` | `armv7l-linux-musleabihf` | `-mcpu=cortex-a15 -mfpu=neon-vfpv4 -mfloat-abi=hard` | Annapurna Labs Alpine AL-212/AL-314 (QNAP TS-x31P/+/X, DS1515, DS416) |
| `zpaqfranz_armv8`           | `aarch64-linux-musl`      | `-march=armv8-a`                                  | any 64-bit ARM not listed                                    |
| `zpaqfranz_cortexa53`       | `aarch64-linux-musl`      | `-mcpu=cortex-a53`                                | Realtek RTD1295/1296, Armada 3720 (DS118, DS218, DS418, DS119j, DS120j, TS-x28A) |
| `zpaqfranz_cortexa55`       | `aarch64-linux-musl`      | `-mcpu=cortex-a55`                                | Realtek RTD1619/1619B (DS124, DS223, DS223j, DS423, TS-x33)  |
| `zpaqfranz_cortexa57`       | `aarch64-linux-musl`      | `-mcpu=cortex-a57`                                | Annapurna Labs Alpine AL-324/AL-524 (QNAP TS-x32X, TS-x32PX) |
| `zpaqfranz_cortexa72`       | `aarch64-linux-musl`      | `-mcpu=cortex-a72`                                | Marvell Armada 8040, some QNAP, Raspberry Pi 4               |

The model lists are indicative: check your own CPU (chapter 7), do not trust a list.

### 32-bit ARM: three generations, three different instruction sets

Here the choice really matters: a binary for a newer CPU **does not run** on an older one. It
starts, and dies with `Illegal instruction` at the first instruction the CPU does not have.

- **`armv5`**: ARMv5TE with **no floating point unit** (`-mfloat-abi=soft`: the floating point
  is done in software). The Marvell Kirkwood of the first cheap NAS (2008-2013) is like this.
- **`armv7`**: ARMv7-A with the **VFPv3-D16** unit and the hard-float calling convention. D16
  (16 double registers, not 32) because some ARMv7 CPUs (NVIDIA Tegra 2, Marvell Armada 370/XP)
  have only those: VFPv3-D16 runs on every ARMv7 with a floating point unit. The safe choice for
  any 32-bit ARMv7.
- **`armv7_cortexa9`**: the same instructions, tuned for the Cortex-A9.
- **`armv7_cortexa15`**: **NEON + VFPv4 + integer division in hardware**. Faster on a Cortex-A15
  (and A7, A17, and the 64-bit ARM that can run 32-bit code), illegal on an A9 or older.

Hard-float or soft-float is about the CPU, not about the NAS firmware: these binaries are static,
they never call a library of the system, so a hard-float binary runs on a "soft-float" firmware
too, as long as the CPU has the VFP unit.

### 64-bit ARM: one instruction set, five tunings

With this source, all the 64-bit binaries contain ARMv8.0-A instructions only: every one runs on
every 64-bit ARM. `-mcpu` changes the tuning (the order of the instructions, the unrolling...).
`-mcpu=cortex-a55` would allow the ARMv8.2 instructions too, but gcc 11 uses none of them here:
it generates the very same code for A53 and A55, and for A57 and A72, and the pairs are identical
to the byte (same MD5). They keep their names anyway, so that nobody has to wonder. In doubt:
`armv8`.

### x86

`-march=x86-64` is the baseline of every 64-bit x86 (no SSE4, no AVX assumed): it runs on the
oldest Atom. `i686` is for the 32-bit Intel CPUs of the old NAS. Both keep the JIT.

### Why five compilers for eleven binaries

The CPU options apply to zpaqfranz, but the libraries linked into it (musl and libstdc++) come
already compiled inside the toolchain, for the base CPU of the toolchain. So the base must be the
oldest CPU of the family: a soft-float ARMv5 toolchain for `armv5`, an ARMv7 hard-float
(VFPv3-D16) one for the three `armv7`, one `aarch64` for the five 64-bit ARM, one per x86.

### What is not covered

Big-endian ARM (`armv5teb`, `armeb`: the old NSLU2 and friends), MIPS, PowerPC, i586 (the
Vortex86 SoCs). No binary for them.

---

## 3. From zero: the build machine

### Debian

Any 64-bit x86 Linux will do: the musl.cc compilers are static themselves, they need nothing.
Mine is a Debian 11 VM (see the [note](#note-how-i-build-the-release-binaries)); Debian 12 or 13
are just as good. The needs:

- 4 cores and **4 GB of RAM**: the compiler wants about 900 MB for this source, and the build
  script runs 4 of them at a time (use `-P 2` in it with 2 GB);
- **3 GB of disk**: about 1.5 GB for the five compilers, a little for the binaries, the rest
  for the temporary files of gcc.

```
apt update
apt install curl ca-certificates file qemu-user-static
```

`file` to check what you built, `qemu-user-static` to try the binaries without a NAS. That is all:
no `build-essential`, no `g++`, the compilers come from musl.cc.

### The compilers, from musl.cc

[musl.cc](https://musl.cc/) publishes ready cross compilers: gcc 11.2.1, binutils 2.37, musl
1.2.2. Five are needed:

```
https://musl.cc/x86_64-linux-musl-cross.tgz
https://musl.cc/i686-linux-musl-cross.tgz
https://musl.cc/armv5l-linux-musleabi-cross.tgz
https://musl.cc/armv7l-linux-musleabihf-cross.tgz
https://musl.cc/aarch64-linux-musl-cross.tgz
```

About 100 MB each, 280-320 MB once unpacked. `nas_toolchains.sh` (appendix A) downloads them,
checks each one against `https://musl.cc/SHA512SUMS`, unpacks it and deletes the `.tgz`:

```
sh nas_toolchains.sh /opt/musl
```

The SHA-512 of the files I use (`SHA512SUMS` of musl.cc, 7 November 2026):

```
52abd1a56e670952116e35d1a62e048a9b6160471d988e16fa0e1611923dd108a581d2e00874af5eb04e4968b1ba32e0eb449a1f15c3e4d5240ebe09caf5a9f3  x86_64-linux-musl-cross.tgz
5047afc68170a2910895db2dfa448227e71a984bfa2130a1bc946fd1015d722b80b15e4abf90c64300815aa84fe781cc8b8a72f10174f9dce96169e035911880  i686-linux-musl-cross.tgz
2104a0edb08e2e9696ef8cb8c284705e987031904ede6ed49e8303f942b35886aa656c7dbd52f8a3a67abd0a2ce2eb3fa82eb8414f09dda2ae1df300edfb62db  armv5l-linux-musleabi-cross.tgz
1bb399a61da425faac521df9b8d303e60ad101f6c7827469e0b4bc685ce1f3dedc606ac7b1e8e34d79f762a3bfe3e8ab479a97e97d9f36fbd9fc5dc9d7ed6fd1  armv7l-linux-musleabihf-cross.tgz
8695ff86979cdf30fbbcd33061711f5b1ebc3c48a87822b9ca56cde6d3a22abd4dab30fdcd1789ac27c6febbaeb9e5bde59d79d66552fae53d54cc1377a19272  aarch64-linux-musl-cross.tgz
```

The compiler of each one is `<folder>/<triple>-cross/bin/<triple>-g++`, for instance
`/opt/musl/armv7l-linux-musleabihf-cross/bin/armv7l-linux-musleabihf-g++`.

### If you do not want to trust musl.cc either

Fair enough: a prebuilt compiler is a binary from a stranger too. The way out is
[musl-cross-make](https://github.com/richfelker/musl-cross-make), the scripts of the author of
musl, that build the same kind of toolchain from the sources of gcc, binutils and musl (an hour or
so each). Set the target (and, for ARM, the base CPU: ARMv5 soft-float, ARMv7 VFPv3-D16
hard-float), `make install`, and give the folder to `nas_build.sh`.

Two warnings. It is not what I use, and I have not tried it for this page. And your binaries
will work the same, but they will not be identical to mine: another compiler, other bytes.

---

## 4. Building

```
bash nas_build.sh /opt/musl ./zpaqfranz.cpp ./bin
```

`nas_build.sh` (appendix B) runs the eleven compile lines of chapter 2, four at a time, each
with its own log (`bin/<name>.log`). On a 4-core VM each binary takes about 2-2.5 minutes, the
whole set about 8.

With 65.18r every build ends with `rc=0`. There is one warning, the same in every release, in
the x86_64 and the five aarch64 builds:

```
warning: 'void* memset(void*, int, size_t)' pointer overflow between offset 0 and size [-2305843009213693952, -1] [-Warray-bounds]
```

Then the script shows what came out:

```
bin/zpaqfranz_armv5:     ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), statically linked, stripped
bin/zpaqfranz_armv8:     ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), statically linked, stripped
bin/zpaqfranz_nas_i686:  ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), statically linked, stripped
...
```

`statically linked` on all eleven: if one says `dynamically linked`, something is wrong. And the
MD5 of all of them, with the two pairs (A53 = A55, A57 = A72) equal.

The two x86 binaries run on the build machine itself:

```
./bin/zpaqfranz_nas_x86_64 autotest -verbose
```

---

## 5. The same bytes as mine

The build is **reproducible**: no date, no time, no folder goes into the binary. Same compiler,
same options, same source: same bytes. One detail: the **name** of the source goes in (an
`assert` keeps it), so the file must be called `zpaqfranz.cpp` and given to the compiler with
that bare name. `nas_build.sh` does it: it runs the compiler in the folder of the source.

On 7 November 2026 I downloaded the ARMv7 compiler again from musl.cc, on the build machine, in a
new folder: SHA-512 OK, identical file by file to the one I installed in September. Then I built
the eleven binaries of 65.18r with `nas_build.sh`, in another folder: **all eleven identical to
the byte to the release ones**.

So, to check my binaries: take the `zpaqfranz.cpp` of the same release, build, compare the MD5.

The binaries of **65.18r** (source `zpaqfranz.cpp` MD5 `da6d72ccfc33de4f50d0ff30f8781d66`):

```
677a91cf2893f2ac6ea05d4799ab8b0d  zpaqfranz_nas_x86_64
547f19ccd269e20d84770fb8c0f8d85f  zpaqfranz_nas_i686
d0921c4640124fcb6c15a71d347d3eec  zpaqfranz_armv5
2bd94e85ae4350ad4c0fae9a1c9e68b5  zpaqfranz_armv7
0e7c32fad260cf223a4affedbe4b7493  zpaqfranz_armv7_cortexa9
14fa69e5d7dc4059e77ca009860b4959  zpaqfranz_armv7_cortexa15
14afacb9f9272760fa6fa2bf6794ebb4  zpaqfranz_armv8
1a5cc71b6d053e0eb312225f4b22db83  zpaqfranz_cortexa53
1a5cc71b6d053e0eb312225f4b22db83  zpaqfranz_cortexa55
d7b235d48afc059372f23e862b5847e9  zpaqfranz_cortexa57
d7b235d48afc059372f23e862b5847e9  zpaqfranz_cortexa72
```

A different MD5 means: another source (even one byte, even the line ends: Windows CRLF against
Unix LF), another name of the source, another compiler, other options.

---

## 6. Trying them without a NAS: qemu-user

`qemu-user` runs a Linux program of another CPU on your PC: `qemu-arm` for 32-bit ARM,
`qemu-aarch64` for 64-bit ARM, `qemu-i386`, `qemu-x86_64`. With a static binary nothing else is
needed: no ARM system, no libraries.

```
qemu-arm-static -cpu cortex-a9 -r 2.6.32 ./bin/zpaqfranz_armv7_cortexa9 autotest -verbose
```

(`qemu-arm-static` with the `qemu-user-static` package, `qemu-arm` with `qemu-user`: the script
looks for both.)

- **`-cpu` is the point.** Without it qemu emulates a CPU with every instruction it knows, and a
  wrong binary would pass. With `-cpu` the instructions the CPU does not have are illegal, as on
  the real one:

  ```
  $ qemu-arm -cpu arm926 ./bin/zpaqfranz_armv7 autotest
  qemu: uncaught target signal 4 (Illegal instruction) - core dumped
  Illegal instruction
  ```

  Not always at once: on a Cortex-A9 the A15 binary passes the light `autotest` (nothing there
  needs VFPv4), then dies at the first `a`. That is why the tests below compress, too.
- **`-r 2.6.32`** changes only what `uname` says. It is not an old kernel: the old kernels work
  because musl uses only the basic system calls.

`nas_qemu.sh` (appendix C) takes each binary to the CPU it is made for (the generic ones to the
oldest of their family: `armv5` on an ARM926, `armv7` on a Cortex-A7, `armv8` on a Cortex-A53),
and there runs `autotest -verbose`, then `a`, `t`, `x` with `-m1`, `-m8` and `-lean`, comparing
the extracted files with the originals:

```
sh nas_qemu.sh ./bin
```

The binaries of 65.18r, with qemu 9.0 (97 seconds in all):

```
zpaqfranz_nas_x86_64 [qemu64] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_nas_i686 [pentium3] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_armv5 [arm926] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_armv7 [cortex-a7] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_armv7_cortexa9 [cortex-a9] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_armv7_cortexa15 [cortex-a15] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_armv8 [cortex-a53] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_cortexa53 [cortex-a53] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_cortexa55 [cortex-a55] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_cortexa57 [cortex-a57] autotest=OK -m1=OK -m8=OK -lean=OK
zpaqfranz_cortexa72 [cortex-a72] autotest=OK -m1=OK -m8=OK -lean=OK
nas_qemu: 0 KO
```

The qemu of Debian 11 (5.2) knows only `cortex-a53`, `cortex-a57`, `cortex-a72` among the 64-bit
cores: the script says so and skips the A55
(`zpaqfranz_cortexa55 [cortex-a55]: this qemu-aarch64-static has no cortex-a55, skipped`).
A newer qemu has it (the one I use is 9.0, see the note); `qemu-aarch64-static -cpu help` lists
what yours has.

What qemu does **not** show:

- the speed (it is an emulator);
- an ARMv5 NAS whose kernel has no fixup for unaligned memory reads: there an unaligned read
  returns rotated data, in silence, and qemu does not reproduce it. The case found (the CRC-32)
  is fixed in the source; for others, the real NAS is the only judge;
- the old kernel itself (`-r` only changes the name).

So the last test is on the NAS: chapter 7.

---

## 7. Which binary for my NAS? (over ssh)

Two commands on the NAS tell almost everything:

```
uname -m             # x86_64, i686, armv5tel, armv7l, aarch64...
cat /proc/cpuinfo    # on ARM: "CPU part", "CPU implementer", "Features"
```

On ARM, `CPU implementer: 0x41` is ARM Ltd, and `CPU part` is the core:

| CPU part | Core       | Binary                                  |
|----------|------------|-----------------------------------------|
| `0xc07`  | Cortex-A7  | `armv7_cortexa15` (VFPv4 + division, and NEON almost always) |
| `0xc09`  | Cortex-A9  | `armv7_cortexa9`                        |
| `0xc0f`  | Cortex-A15 | `armv7_cortexa15`                       |
| `0xd03`  | Cortex-A53 | `cortexa53`                             |
| `0xd04`  | Cortex-A35 | `cortexa53`                             |
| `0xd05`  | Cortex-A55 | `cortexa55` (= `cortexa53`)             |
| `0xd07`  | Cortex-A57 | `cortexa57`                             |
| `0xd08`  | Cortex-A72 | `cortexa72` (= `cortexa57`)             |
| `0xd09`  | Cortex-A73 | `cortexa72`                             |

Marvell (`0x56`) and others design their own cores: there the `Features` line decides.

### `nas_choose.sh`

`nas_choose.sh` (appendix D) does the reasoning for you. It needs only `sh`, `uname`,
`/proc/cpuinfo` and the usual small tools (`sed`, `sort`, `tr`, `head`), there on any NAS, and it
does not even have to be copied there:

```
ssh admin@mynas sh -s < nas_choose.sh
```

For a NAS with an Armada 385 (two Cortex-A9; here the `/proc/cpuinfo` of one, on the bench):

```
uname -m   : armv7l
CPU part   : 0xc09   implementer: 0x41
Features   : half thumb fastmult vfp edsp neon vfpv3 tls vfpd32
Recommended: zpaqfranz_armv7_cortexa9
Fallbacks  : zpaqfranz_armv7 zpaqfranz_armv5
```

The rules, from the requirements of the binaries (what `readelf -A` says of them), not from
model lists:

- **32-bit ARM**: `armv7_cortexa15` if `Features` has `neon`, `vfpv4` and `idiva`; `armv7` if it
  has `vfpv3` or `vfpv3d16` (plus `armv7_cortexa9` first, on an A9); `armv5` always, as the last
  resort. `vfp` alone is VFPv2, the unit of ARMv6 (Raspberry Pi 1, Oxford OX820): `armv5` there.
- **64-bit ARM**: `cortexa53` if every core is a small in-order one (A53, A35, A55),
  `cortexa72` if every core is a big one (A57, A72, A73), `armv8` for the rest (big.LITTLE,
  Neoverse, Qualcomm, unknown). Then `armv8`, and the 32-bit ones as a fallback (a 64-bit ARM
  runs 32-bit code too, if the kernel allows it).
- **x86**: `nas_x86_64` on a 64-bit system, `nas_i686` on a 32-bit one (and as the fallback).
- Big-endian, MIPS, i586: none.

### Then try them, for real

A binary that starts proves nothing: `Illegal instruction` comes only when the CPU meets the
instruction, maybe in the middle of a backup. So, with a folder of binaries on the NAS:

```
scp bin/zpaqfranz_armv7* bin/zpaqfranz_armv5 admin@mynas:/volume1/zp/
ssh admin@mynas sh -s /volume1/zp < nas_choose.sh
```

the script tries them, from the best to the safest: `autotest -verbose` (and it reads the
result: the light autotest exits with 0 even when a hash is wrong), then `a` + `t` with `-m1`
and `-m8`, on a few hundred KB of data. The first one that passes is the one to use. On the
bench (the binaries run by qemu as a Cortex-A9, the `/proc/cpuinfo` above):

```
Recommended: zpaqfranz_armv7_cortexa9
Fallbacks  : zpaqfranz_armv7 zpaqfranz_armv5
Trying in /zpnas/dazero/wrap:
  zpaqfranz_armv7_cortexa9: OK
USE: /zpnas/dazero/wrap/zpaqfranz_armv7_cortexa9
```

And if the diagnosis is wrong (a `/proc/cpuinfo` that promises more than the CPU has, or a rule
of mine that does not fit your CPU), the trial catches it. Here, on the bench, the `Features` of a
Cortex-A7 on a CPU that is really a Cortex-A9 (qemu):

```
Recommended: zpaqfranz_armv7_cortexa15
Fallbacks  : zpaqfranz_armv7 zpaqfranz_armv5
Trying in /zpnas/dazero/wrap:
      qemu: uncaught target signal 4 (Illegal instruction) - core dumped
      Illegal instruction
  zpaqfranz_armv7_cortexa15: SIGILL: instructions this CPU does not have
  zpaqfranz_armv7: OK
USE: /zpnas/dazero/wrap/zpaqfranz_armv7
```

The A15 binary passed `autotest`, and died at the first `a`: a `--version`, or the light
`autotest` alone, would have said "fine".

If a binary fails, the script says how: `SIGILL` (an instruction this CPU does not have),
`SIGBUS` (an unaligned access), `cannot run` (wrong architecture, or a folder mounted `noexec`:
try another folder), `killed` (memory).

### Notes from the field

- **Synology, DSM 7**: if `scp` fails with `subsystem request failed`, the SFTP service is off
  and your `scp` (OpenSSH 9 and later) uses SFTP: `scp -O` uses the old protocol.
- **Where to keep it**: in a shared folder of a volume (`/volume1/...`, `/share/...`), not in
  `/usr/local/bin` or `/opt`: a firmware update can wipe the system folders.
- **`/tmp`** is a small RAM disk on some NAS: the test files of `nas_choose.sh` are less than
  1 MB, but `TMPDIR=/volume1/tmp sh -s ...` moves them.
- **Threads**: the 32-bit binaries use at most 2 threads by default. On a 4-core ARM, `-t4`.
- **Method**: the default of the NAS builds is `-m1`, as zpaq 7.15. On a NAS, write `-lean`
  (`-m80` with 2 threads: about 30 MB of RAM, and faster than `-m1` on a Cortex-A15); on a
  really minimal one, `-lean -t1`.

---

## Note: how I build the release binaries

- **The machine**: a Debian 11.9 (bullseye) VM, x86_64, kernel 5.10, 4 cores, 4 GB of RAM.
- **The compilers**: the five of musl.cc above, installed in September 2026 and never changed:
  `gcc (GCC) 11.2.1 20211120`, GNU binutils 2.37, musl 1.2.2. They are static 32-bit x86
  programs themselves.
- **The build**: the compile lines of `nas_build.sh` (my own script is the same thing, with the
  folders of my VM). At every release the source goes to a new folder of the VM, the eleven
  binaries are built there, and the two x86 ones run `autotest` on the spot. The binaries come
  back to Windows only when their MD5 there is the MD5 on the VM, together with the MD5 of the
  source they come from.
- **The tests**: qemu-user 9.0 (the Docker image `lazymio/qemu-full`, because the qemu of Debian
  11 has no Cortex-A55), each binary on its core with `-r 2.6.32`. When the NAS build changed
  (65.5d, September 2026): `autotest`, `a`/`t`/`x` with `-m0` `-m1` `-m2` `-m6` `-m8` (and `-m5`
  on a small file), `a`/`t`/`x`/`v` with every hash, and archives made by the `armv5` and
  `cortexa55` binaries tested on Windows and with zpaq 7.15. For this page, `nas_qemu.sh` on
  the binaries of 65.18r, and `autotest -all` of `armv7_cortexa9` on a Cortex-A9 (all OK, 3.5
  minutes in qemu). And on a real NAS (Cortex-A15).

---

## Appendix A: `nas_toolchains.sh`

```sh
#!/bin/sh
# nas_toolchains.sh DIR [triple...] - the musl cross compilers from musl.cc, checked with SHA512SUMS, into DIR
#   sh nas_toolchains.sh /opt/musl            all five
#   sh nas_toolchains.sh /opt/musl aarch64-linux-musl
# Needs curl, sha512sum, tar, gzip. DIR/<triple>-cross/bin/<triple>-g++ is the compiler
D=$1
[ -n "$D" ] || { echo "nas_toolchains.sh DIR [triple...]"; exit 2; }
shift
L="$*"
[ -n "$L" ] || L="x86_64-linux-musl i686-linux-musl armv5l-linux-musleabi armv7l-linux-musleabihf aarch64-linux-musl"
mkdir -p "$D" || exit 2
cd "$D" || exit 2
curl -fsS -o SHA512SUMS https://musl.cc/SHA512SUMS || { echo "SHA512SUMS: download KO"; exit 2; }
ko=0
for t in $L; do
	f=$t-cross.tgz
	if [ -x "$t-cross/bin/$t-g++" ]; then
		echo "$t: already there"
		continue
	fi
	echo "$t: downloading"
	curl -fsS -o "$f" "https://musl.cc/$f" || { echo "$t: download KO"; ko=$((ko+1)); continue; }
	if ! grep " $f\$" SHA512SUMS | sha512sum -c - > /dev/null 2>&1; then
		echo "$t: SHA-512 KO (not in SHA512SUMS, or a different file): removed"
		rm -f "$f"; ko=$((ko+1)); continue
	fi
	tar -xzf "$f" && rm -f "$f" || { echo "$t: tar KO"; ko=$((ko+1)); continue; }
	echo "$t: $("$t-cross/bin/$t-g++" --version | head -n 1)"
done
[ $ko = 0 ]
```

## Appendix B: `nas_build.sh`

```bash
#!/bin/bash
# nas_build.sh TC SOURCE OUT - the 11 NAS binaries of zpaqfranz: static, musl, -DNAS, one per CPU family
#   bash nas_build.sh /opt/musl ./zpaqfranz.cpp ./bin
# TC = the folder of nas_toolchains.sh. 4 compilers at a time (each one wants about 1 GB of RAM).
# The compiler is run in the folder of the source, with its bare name: the name goes into the binary
# (an assert), so zpaqfranz.cpp gives the same bytes as the release binaries, and anything else does not
TC=$(cd "$1" 2>/dev/null && pwd) || { echo "nas_build.sh TC SOURCE OUT"; exit 2; }
S=$2; O=$3
[ -f "$S" ] && [ -n "$O" ] || { echo "nas_build.sh TC SOURCE OUT"; exit 2; }
mkdir -p "$O" || exit 2
O=$(cd "$O" && pwd)
cd "$(dirname "$S")" || exit 2
S=$(basename "$S")
md5sum "$S"
grep -m 1 '#define ZPAQ_VERSION' "$S"
COMMON="-O3 -DNAS -static -no-pie -pthread -s -Wall"
# name | toolchain | flags of this CPU family
TARGETS="zpaqfranz_nas_x86_64|x86_64-linux-musl|-march=x86-64
zpaqfranz_nas_i686|i686-linux-musl|-march=i686
zpaqfranz_armv5|armv5l-linux-musleabi|-DNOJIT -march=armv5te -mfloat-abi=soft
zpaqfranz_armv7|armv7l-linux-musleabihf|-DNOJIT -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard
zpaqfranz_armv7_cortexa9|armv7l-linux-musleabihf|-DNOJIT -mcpu=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=hard
zpaqfranz_armv7_cortexa15|armv7l-linux-musleabihf|-DNOJIT -mcpu=cortex-a15 -mfpu=neon-vfpv4 -mfloat-abi=hard
zpaqfranz_armv8|aarch64-linux-musl|-DNOJIT -march=armv8-a
zpaqfranz_cortexa53|aarch64-linux-musl|-DNOJIT -mcpu=cortex-a53
zpaqfranz_cortexa55|aarch64-linux-musl|-DNOJIT -mcpu=cortex-a55
zpaqfranz_cortexa57|aarch64-linux-musl|-DNOJIT -mcpu=cortex-a57
zpaqfranz_cortexa72|aarch64-linux-musl|-DNOJIT -mcpu=cortex-a72"
build() {
	IFS='|' read -r name tc flags <<< "$1"
	t0=$(date +%s)
	rm -f "$O/$name"
	"$TC/$tc-cross/bin/$tc-g++" $COMMON $flags "$S" -o "$O/$name" > "$O/$name.log" 2>&1
	rc=$?
	echo "$name rc=$rc warnings=$(grep -c 'warning:' "$O/$name.log") $(( $(date +%s)-t0 ))s"
}
export -f build
export TC COMMON S O
echo "$TARGETS" | xargs -P 4 -I{} bash -c 'build "{}"'
cd "$O" || exit 2
file zpaqfranz_* | grep -v '\.log:' | sed 's/, BuildID[^,]*//'
md5sum zpaqfranz_* | grep -v '\.log$'
n=$(ls zpaqfranz_* | grep -vc '\.log$')
echo "$n of 11 built"
[ "$n" = 11 ]
```

## Appendix C: `nas_qemu.sh`

```sh
#!/bin/sh
# nas_qemu.sh BIN - every NAS binary of BIN on the CPU it is made for, with qemu-user (no NAS needed)
#   sh nas_qemu.sh ./bin
# Needs qemu-user (qemu-x86_64, qemu-i386, qemu-arm, qemu-aarch64, or the -static ones of qemu-user-static).
# -cpu is the point: without it qemu emulates a CPU with EVERY instruction, and a wrong binary would pass.
# -r 2.6.32 is only what uname says: the old kernels work because musl uses only basic system calls
B=$(cd "$1" 2>/dev/null && pwd) || { echo "nas_qemu.sh BIN"; exit 2; }
W=${TMPDIR:-/tmp}/nas_qemu.$$
case "$W" in */nas_qemu.*) rm -rf "$W";; *) exit 2;; esac
mkdir -p "$W/d/sub" || exit 2
# a bit of everything: text, random, duplicated data, an empty file
seq 1 200000 > "$W/d/numbers.txt"
dd if=/dev/urandom of="$W/d/random.bin" bs=1024 count=1024 2> /dev/null
cat "$W/d/random.bin" "$W/d/random.bin" > "$W/d/sub/twice.bin"
: > "$W/d/sub/empty.txt"
qemu() { for q in "qemu-$1" "qemu-$1-static"; do command -v "$q" > /dev/null 2>&1 && { echo "$q"; return; }; done; }
ko=0
# binary | qemu | cpu: the generic ones on the oldest core of their family
while IFS='|' read -r name arch cpu; do
	[ -n "$name" ] || continue
	r="$name [$cpu]"
	Q=$(qemu "$arch")
	if [ ! -f "$B/$name" ]; then echo "$r: missing"; continue; fi
	if [ -z "$Q" ]; then echo "$r: no qemu-$arch"; continue; fi
	if ! "$Q" -cpu help < /dev/null 2>&1 | grep -qiw -- "$cpu"; then echo "$r: this $Q has no $cpu, skipped"; continue; fi
	Z="$Q -cpu $cpu -r 2.6.32 $B/$name"
	T=$W/$name
	mkdir -p "$T" && cd "$T" || exit 2
	$Z autotest -verbose > autotest.txt 2>&1 < /dev/null
	rc=$?
	if [ $rc = 0 ] && grep -q 'ERROR 0 |' autotest.txt; then r="$r autotest=OK"; else r="$r autotest=KO(rc $rc)"; ko=$((ko+1)); fi
	for m in 1 8 -lean; do
		case $m in -*) s=$m;; *) s=-m$m;; esac
		$Z a "a$m.zpaq" "$W/d" $s -noeta > a.txt 2>&1 < /dev/null
		ra=$?
		$Z t "a$m.zpaq" -noeta > t.txt 2>&1 < /dev/null
		rt=$?
		$Z x "a$m.zpaq" -to "$T/x$m/" -noeta > x.txt 2>&1 < /dev/null
		rx=$?
		if [ $ra = 0 ] && [ $rt = 0 ] && [ $rx = 0 ] && diff -r "$W/d" "$T/x$m$W/d" > /dev/null 2>&1; then
			r="$r $s=OK"
		else
			r="$r $s=KO(a $ra,t $rt,x $rx)"; ko=$((ko+1))
		fi
	done
	echo "$r"
	cd "$W" || exit 2
done <<EOF
zpaqfranz_nas_x86_64|x86_64|qemu64
zpaqfranz_nas_i686|i386|pentium3
zpaqfranz_armv5|arm|arm926
zpaqfranz_armv7|arm|cortex-a7
zpaqfranz_armv7_cortexa9|arm|cortex-a9
zpaqfranz_armv7_cortexa15|arm|cortex-a15
zpaqfranz_armv8|aarch64|cortex-a53
zpaqfranz_cortexa53|aarch64|cortex-a53
zpaqfranz_cortexa55|aarch64|cortex-a55
zpaqfranz_cortexa57|aarch64|cortex-a57
zpaqfranz_cortexa72|aarch64|cortex-a72
EOF
case "$W" in */nas_qemu.*) rm -rf "$W";; esac
echo "nas_qemu: $ko KO"
[ $ko = 0 ]
```

## Appendix D: `nas_choose.sh`

```sh
#!/bin/sh
# nas_choose.sh - which zpaqfranz NAS binary for this machine
#   sh nas_choose.sh          diagnosis only: needs sh, uname, sed and /proc/cpuinfo (no binary is run)
#   sh nas_choose.sh DIR      tries the zpaqfranz_* binaries in DIR, from the best to the safest
# Over ssh, nothing copied:   ssh admin@nas sh -s < nas_choose.sh
#                             ssh admin@nas sh -s /volume1/zp < nas_choose.sh
# On the bench: CPUINFO=file UNAME_M=armv7l sh nas_choose.sh
# rc: 0 found, 2 none fits
CI=${CPUINFO:-/proc/cpuinfo}
M=${UNAME_M:-$(uname -m)}
[ -r "$CI" ] || CI=/dev/null
F=" $(sed -n 's/^[Ff]eatures[[:space:]]*:[[:space:]]*//p' "$CI" | head -n 1 | tr '\t' ' ') "
P=$(sed -n 's/^CPU part[[:space:]]*:[[:space:]]*//p' "$CI" | sort -u | tr '\n' ' ')
I=$(sed -n 's/^CPU implementer[[:space:]]*:[[:space:]]*//p' "$CI" | sort -u | tr '\n' ' ')

has() { case "$F" in *" $1 "*) return 0;; esac; return 1; }
L=""
add() { case " $L " in *" $1 "*) ;; *) L="$L $1";; esac; }

# 64 bit: the same ISA (ARMv8.0) for armv8/a53/a55/a57/a72, only the tuning changes
# (a53 = a55 and a57 = a72 to the byte). Small in-order cores -> a53, big ones -> a72,
# mixed (big.LITTLE), unknown or not ARM Ltd -> the generic one.
aarch64() {
	t=armv8
	if [ "$I" = "0x41 " ] && [ -n "$P" ]; then
		small=0; big=0; other=0
		for p in $P; do
			case "$p" in
				0xd03|0xd04|0xd05) small=1;;	# A53 A35 A55
				0xd07|0xd08|0xd09) big=1;;	# A57 A72 A73
				*) other=1;;
			esac
		done
		[ $other = 0 ] && [ $small = 1 ] && [ $big = 0 ] && t=cortexa53
		[ $other = 0 ] && [ $big = 1 ] && [ $small = 0 ] && t=cortexa72
	fi
	add $t; add armv8
}
# 32 bit: what the binaries really need (readelf -A): a15 = NEON + VFPv4 + integer divide,
# armv7 and cortexa9 = VFPv3-D16, armv5 = no FPU at all. "vfp" alone is VFPv2 (ARMv6): armv5.
arm32() {
	if has neon && has vfpv4 && has idiva; then
		add armv7_cortexa15
	fi
	if has vfpv3 || has vfpv3d16; then
		case " $P" in *" 0xc09 "*) add armv7_cortexa9;; esac
		add armv7
	fi
	add armv5
}

case "$M" in
	x86_64|amd64)		add nas_x86_64; add nas_i686;;
	i686|i786)		add nas_i686;;
	aarch64_be|arm*b|arm*eb*) ;;			# big endian: no binary
	aarch64|arm64)		aarch64; add armv7_cortexa15; add armv5;;	# AArch32 on v8 always has NEON+VFPv4+idiv
	armv8l)			add armv8; arm32;;	# a 32 bit process, maybe on a 64 bit kernel
	armv5*|armv6*|armv7*|arm)	arm32;;
esac

echo "uname -m   : $M"
echo "CPU part   : ${P:-?}  implementer: ${I:-?}"
echo "Features   :$F"
if [ -z "$L" ]; then
	echo "No zpaqfranz binary for this machine"
	exit 2
fi
first=${L# }; first=${first%% *}
rest=${L# $first}
echo "Recommended: zpaqfranz_$first"
[ -n "$rest" ] && echo "Fallbacks  :$(echo "$rest" | sed 's/ / zpaqfranz_/g')"

D=${1:-}
[ -n "$D" ] || exit 0		# diagnosis only
D=$(cd "$D" 2>/dev/null && pwd) || { echo "No folder $1"; exit 2; }	# absolute: try() does a cd

explain() {
	case $1 in
		0) echo "OK";;
		3) echo "autotest: wrong hashes";;
		126) echo "cannot run (wrong architecture?)";;
		127) echo "not found";;
		132) echo "SIGILL: instructions this CPU does not have";;
		135) echo "SIGBUS: unaligned access?";;
		137) echo "killed (memory?)";;
		139) echo "SIGSEGV";;
		*) echo "rc $1";;
	esac
}
# a real try: the light autotest (-all takes 27 s on a Ryzen, minutes on a NAS), then a + t with -m1 and -m8 (the loops
# where the compiler puts NEON, divisions and so on). A --version that starts proves NOTHING: SIGILL comes only when
# the instruction is executed. </dev/null: with "sh -s" the rest of this script is on stdin, nobody else may read it
try() {
	T=${TMPDIR:-/tmp}/zpchoose.$$
	case "$T" in */zpchoose.*) rm -rf "$T";; *) return 1;; esac
	mkdir -p "$T/d" || return 1
	for i in 1 2 3 4 5 6 7 8; do cat "$CI"; done > "$T/d/text.txt" 2>/dev/null	# not $0: with "sh -s" it is no file
	dd if=/dev/urandom of="$T/d/random.bin" bs=1024 count=256 2>/dev/null
	dd if=/dev/zero of="$T/d/zeros.bin" bs=1024 count=256 2>/dev/null
	(
		cd "$T" || exit 1
		"$1" autotest -verbose > o.txt 2>&1 < /dev/null || exit $?
		grep -q 'ERROR 0 |' o.txt || exit 3	# the 16 hashes of "ABCDE": rc 0 even if wrong, so read the text
		for m in 1 8; do
			"$1" a "p$m.zpaq" d -m$m -noeta >> o.txt 2>&1 < /dev/null || exit $?
			"$1" t "p$m.zpaq" -noeta >> o.txt 2>&1 < /dev/null || exit $?
		done
	)
	rc=$?
	[ $rc = 0 ] || tail -n 2 "$T/o.txt" 2>/dev/null | sed 's/^/      /'
	case "$T" in */zpchoose.*) rm -rf "$T";; esac
	return $rc
}

echo "Trying in $D:"
for b in $L; do
	f=$D/zpaqfranz_$b
	if [ ! -f "$f" ]; then
		echo "  zpaqfranz_$b: missing"
		continue
	fi
	[ -x "$f" ] || chmod +x "$f" 2>/dev/null
	try "$f"
	rc=$?
	echo "  zpaqfranz_$b: $(explain $rc)"
	if [ $rc = 0 ]; then
		echo "USE: $f"
		exit 0
	fi
done
echo "No binary works here"
exit 2
```
