# Universal Makefile for zpaqfranz
# Build 1.7 2026-10-28
# By Franco Corbelli (MIT License)
# Tested (kind of) on Linux, macOS, FreeBSD, OpenBSD, NetBSD, Solaris, illumos,
# sparc64, ppc, mips, ARM, aarch64, Apple Silicon, RISC-V, etc.
# 1.7: Oracle Linux / CentOS / RHEL 7 (gcc 4.8.5, glibc 2.17): -std=c++11 when the
#      compiler is older than C++11 by default, -ldl on Linux always (not only with SFTP)

# NOTE: This Makefile requires GNU make
# On FreeBSD/OpenBSD/NetBSD/DragonFly/Solaris use 'gmake' instead of 'make'
# On macOS this is fine (Apple ships GNU make)

# Usage:
#
# make                     Compile zpaqfranz (auto-NOJIT on non-x86)
# make install             Install to /usr/local/bin (or PREFIX)
# make install-clean       Compile, install, then remove the local binary
# make static              Static build without SFTP and mount (for NAS, containers, rescue)
# make nointel             Force build without JIT (even on Intel)
# make debug               Debug build (no optimization)
# make clean               Remove local binary
# make uninstall           Remove from system
# make check               Show build configuration
# make test                Run all automatic tests
#
# Options:
# make ENABLE_SFTP=no      Build without SFTP (dynamic, no -static)
# make ENABLE_ZPAQMOUNT=yes   Build with the mount command (needs FUSE)
# make CROSS_COMPILE=aarch64-linux-gnu-   Cross-compile for another arch
#
# Download:
# make nightly             Nightly (risky)
# make download            Latest official release
# make downloads           Stable from GitHub

# OS / Architecture detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Compiler: prefer clang++, fallback g++
# GNU make gives CXX a built-in default (g++, or c++ on the BSDs), so "CXX ?= clang++" never
# assigns: only a CXX that nobody set explicitly (environment, command line, this file) is replaced
ifeq ($(origin CXX),default)
  CXX := clang++
endif
ifeq ($(shell command -v $(CXX) >/dev/null 2>&1 || echo "no"),no)
  CXX = g++
endif

# On Solaris, clang++ is often absent; fall back to g++ then CC (Solaris Studio)
ifeq ($(UNAME_S),SunOS)
  ifeq ($(shell command -v $(CXX) >/dev/null 2>&1 || echo "no"),no)
    ifeq ($(shell command -v g++ >/dev/null 2>&1 || echo "no"),no)
      CXX = CC
    else
      CXX = g++
    endif
  endif
endif

# Cross-compilation support (e.g. make CROSS_COMPILE=aarch64-linux-gnu-)
CROSS_COMPILE ?=
CXX := $(CROSS_COMPILE)$(CXX)

# Default tools
RM      ?= rm -f
MKDIR   ?= mkdir -p
LN      ?= ln -sf
STRIP   ?= $(CROSS_COMPILE)strip

# Program details
PROG    := zpaqfranz
ALTNAME := dir
SOURCE  := zpaqfranz.cpp

# Download URLs
SOURCE_URL_NIGHTLY  := http://www.francocorbelli.it/zpaqfranz.cpp
SOURCE_URL_DOWNLOAD := http://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz.cpp
SOURCE_URL_GITHUB   := https://raw.githubusercontent.com/fcorbelli/zpaqfranz/main/zpaqfranz.cpp

# User-overridable flags (safe to pass from command line or packaging systems)
CXXFLAGS ?= -O3
CPPFLAGS ?=

# SFTP support (disable with make ENABLE_SFTP=no)
ENABLE_SFTP ?= yes

# Mount command support (enable with make ENABLE_ZPAQMOUNT=yes): off by
# default, because it needs FUSE (libfuse3; on OpenBSD the FUSE of the base system)
ENABLE_ZPAQMOUNT ?= no
PKG_CONFIG ?= pkg-config

# Project-required flags (always present; user flags are appended)
# The source needs C++11: Apple clang (up to 15) still defaults to C++98
ZPAQ_STD :=
ifeq ($(UNAME_S),Darwin)
  ZPAQ_STD := -std=c++11
endif
# gcc 4.8 and 5 (Oracle Linux, CentOS, RHEL 7: 4.8.5) build C++98 by default too: asked to the compiler itself
# (__cplusplus), so the ones that are C++11 or later already are left as they are
ifeq ($(ZPAQ_STD),)
  CXX_STD_DEFAULT := $(shell echo | $(CXX) -dM -E -x c++ - 2>/dev/null | sed -n 's/^\#define __cplusplus \([0-9]*\).*/\1/p')
  ifneq ($(CXX_STD_DEFAULT),)
    ifeq ($(shell [ $(CXX_STD_DEFAULT) -lt 201103 ] && echo old),old)
      ZPAQ_STD := -std=c++11
    endif
  endif
endif
ZPAQ_CXXFLAGS := -Wall -pthread $(ZPAQ_STD) $(CXXFLAGS)
ZPAQ_CPPFLAGS := $(CPPFLAGS)

ifeq ($(ENABLE_SFTP),yes)
  ZPAQ_CPPFLAGS += -DSFTP
endif

# Intel-specific flags (added only on x86)
INTEL_FLAGS := -DHWSHA2

# Libraries
LDLIBS ?= -lm

# -ldl is needed for SFTP on Linux and Solaris only.
# macOS has dlopen in libSystem; *BSD and DragonFly have it in libc.
ifneq (,$(findstring -DSFTP,$(ZPAQ_CPPFLAGS)))
  NEEDS_DL := yes
  ifeq ($(UNAME_S),Darwin)
    NEEDS_DL := no
  endif
  ifneq (,$(findstring BSD,$(UNAME_S)))
    NEEDS_DL := no
  endif
  ifeq ($(UNAME_S),DragonFly)
    NEEDS_DL := no
  endif
  ifeq ($(NEEDS_DL),yes)
    LDLIBS += -ldl
  endif
endif
# dlopen() is used without SFTP too (libsodium for -franzen, libcurl for download): before glibc 2.34
# (Oracle Linux, CentOS, RHEL 7: 2.17) it is in libdl, else the link fails. Later it is an empty stub: harmless
ifneq (,$(filter Linux SunOS,$(UNAME_S)))
  ifeq (,$(findstring -ldl,$(LDLIBS)))
    LDLIBS += -ldl
  endif
endif

# FUSE for the mount command, only with ENABLE_ZPAQMOUNT=yes.
# FUSE 3 everywhere (pkg-config fuse3), except OpenBSD: FUSE 2 of the base system (-lfuse).
# Without a fuse3.pc: macOS (macFUSE, FUSE-T) and the BSDs keep it in /usr/local,
# Linux in /usr/include/fuse3 (found by the source itself). Elsewhere give it by hand:
# make ENABLE_ZPAQMOUNT=yes FUSE_CFLAGS=-I/path/include/fuse3 FUSE_LIBS="-L/path/lib -lfuse3"
ifeq ($(ENABLE_ZPAQMOUNT),yes)
  ZPAQ_CPPFLAGS += -DZPAQMOUNT
  ifeq ($(UNAME_S),OpenBSD)
    FUSE_LIBS ?= -lfuse
  else
    FUSE_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags fuse3 2>/dev/null)
    FUSE_LIBS   ?= $(shell $(PKG_CONFIG) --libs fuse3 2>/dev/null)
    ifeq ($(strip $(FUSE_LIBS)),)
      ifeq ($(UNAME_S),Darwin)
        FUSE_CFLAGS := -I/usr/local/include/fuse3
        FUSE_LIBS   := -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lfuse3
      else ifneq (,$(findstring BSD,$(UNAME_S)))
        FUSE_CFLAGS := -I/usr/local/include/fuse3
        FUSE_LIBS   := -L/usr/local/lib -lfuse3
      else
        FUSE_LIBS   := -lfuse3
      endif
    endif
  endif
  ZPAQ_CPPFLAGS += $(FUSE_CFLAGS)
  LDLIBS += $(FUSE_LIBS)
endif

# Install directories
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# OS-specific adjustments
ifeq ($(UNAME_S),SunOS)
  ZPAQ_CPPFLAGS += -DSOLARIS
  # kstat_open() & C. (RAM and disk information): without it the link fails
  LDLIBS += -lkstat
endif

# Architecture-specific adjustments
# Using findstring to cover all variants (ppc64, ppc64le, sparc64, sparcv9, mips64el, etc)
ifeq ($(findstring ppc,$(UNAME_M)),ppc)
  ZPAQ_CPPFLAGS += -DANCIENT -DBIG
else ifeq ($(findstring sparc,$(UNAME_M)),sparc)
  ZPAQ_CPPFLAGS += -DALIGNMALLOC
else ifeq ($(findstring mips,$(UNAME_M)),mips)
  ZPAQ_CPPFLAGS += -DBIG
endif

# Auto-NOJIT: enable JIT + HWSHA2 only on x86/amd64.
# Everything else (ARM, aarch64, Apple Silicon, RISC-V, ppc, sparc, mips, ...)
# gets -DNOJIT automatically. Just "make" anywhere and it works.
# BTW newer zpaqfranz "automagically" enable JIT. This is for older releases
ifneq ($(filter x86_64 amd64 i386 i686 i86pc,$(UNAME_M)),)
  ZPAQ_CPPFLAGS += $(INTEL_FLAGS)
else
  ZPAQ_CPPFLAGS += -DNOJIT
endif

# Phony targets
.PHONY: all build install uninstall clean check help debug \
        nointel install-nointel install-clean static \
        nightly download downloads \
        _do_download \
        test test-nointel

# Default target
all: build

# Build
build: $(PROG)

$(PROG): $(SOURCE)
	$(CXX) $(ZPAQ_CPPFLAGS) $(ZPAQ_CXXFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

# Debug
debug: ZPAQ_CXXFLAGS = -g -O0 -Wall -Wextra -pthread $(ZPAQ_STD)
debug: $(PROG)
	@echo "Debug build completed"

# Force NOJIT even on Intel
nointel: ZPAQ_CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(ZPAQ_CPPFLAGS)) -DNOJIT
nointel: clean build
	@echo "Built without JIT (forced)"

# Static build: no SFTP (dlopen is incompatible with -static),
# no mount (libfuse3 is a shared library only: no libfuse3.a to link).
# -ldl stays: -franzen and download still call dlopen (they fall back without it), and before
# glibc 2.34 (Oracle Linux, CentOS, RHEL 7) the static link fails without libdl.a (an empty stub later)
static: ZPAQ_CPPFLAGS := $(filter-out -DSFTP -DZPAQMOUNT $(FUSE_CFLAGS),$(ZPAQ_CPPFLAGS))
static: LDLIBS := $(filter-out $(FUSE_LIBS),$(LDLIBS))
static: LDFLAGS += -static
static: clean build
	@echo "Static build completed (no SFTP, no mount)"

# Install (cp + chmod for maximum portability, including old Solaris)
install: build
	$(MKDIR) $(DESTDIR)$(BINDIR)
	-$(RM) $(DESTDIR)$(BINDIR)/$(PROG) 2>/dev/null
	cp $(PROG) $(DESTDIR)$(BINDIR)/$(PROG)
	chmod 0755 $(DESTDIR)$(BINDIR)/$(PROG)
	-$(STRIP) $(DESTDIR)$(BINDIR)/$(PROG) 2>/dev/null || true
	@if [ ! -e $(DESTDIR)$(BINDIR)/$(ALTNAME) ] && [ ! -L $(DESTDIR)$(BINDIR)/$(ALTNAME) ]; then \
		echo "Creating symbolic link $(DESTDIR)$(BINDIR)/$(ALTNAME)"; \
		$(LN) $(PROG) $(DESTDIR)$(BINDIR)/$(ALTNAME); \
	else \
		echo "$(ALTNAME) already exists, skipping"; \
	fi

install-clean: install clean
	@echo "Installed and local binary cleaned"

install-nointel: ZPAQ_CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(ZPAQ_CPPFLAGS)) -DNOJIT
install-nointel: clean build install
	@echo "Installed without JIT (forced)"

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(PROG)
	@if [ -L $(DESTDIR)$(BINDIR)/$(ALTNAME) ]; then \
		echo "Removing symbolic link $(DESTDIR)$(BINDIR)/$(ALTNAME)"; \
		$(RM) $(DESTDIR)$(BINDIR)/$(ALTNAME); \
	else \
		echo "$(ALTNAME) not found, skipping"; \
	fi

# Download helper
_do_download:
	@echo "------------------------------------------------------------"
	@echo "Source : $(SOURCE)"
	@echo "URL    : $(FETCH_URL)"
	@echo "------------------------------------------------------------"
	@$(RM) $(PROG) $(SOURCE)
	@if command -v wget >/dev/null 2>&1; then \
		echo "Downloader : wget"; \
		wget --no-check-certificate -L --progress=bar:force:noscroll "$(FETCH_URL)" -O "$(SOURCE)" 2>/dev/null \
		|| wget --no-check-certificate -L -q "$(FETCH_URL)" -O "$(SOURCE)"; \
		_RC=$$?; \
		if [ $$_RC -ne 0 ] || [ ! -s "$(SOURCE)" ]; then \
			echo "Error: wget failed or empty file."; \
			$(RM) "$(SOURCE)"; exit 1; \
		fi; \
		echo "Download OK (wget)."; \
	elif command -v curl >/dev/null 2>&1; then \
		echo "Downloader : curl"; \
		curl -k -L --fail --progress-bar "$(FETCH_URL)" -o "$(SOURCE)" 2>/dev/null \
		|| curl -k -L --fail -s "$(FETCH_URL)" -o "$(SOURCE)"; \
		_RC=$$?; \
		if [ $$_RC -ne 0 ] || [ ! -s "$(SOURCE)" ]; then \
			echo "Error: curl failed or empty file."; \
			$(RM) "$(SOURCE)"; exit 1; \
		fi; \
		echo "Download OK (curl)."; \
	elif command -v fetch >/dev/null 2>&1; then \
		echo "Downloader : fetch (BSD)"; \
		fetch -o "$(SOURCE)" "$(FETCH_URL)"; \
		_RC=$$?; \
		if [ $$_RC -ne 0 ] || [ ! -s "$(SOURCE)" ]; then \
			echo "Error: fetch failed or empty file."; \
			$(RM) "$(SOURCE)"; exit 1; \
		fi; \
		echo "Download OK (fetch)."; \
	else \
		echo "Error: no download tool (wget/curl/fetch)."; \
		exit 1; \
	fi

# Download targets: pass FETCH_URL as a make variable to the sub-make
nightly:
	$(MAKE) --no-print-directory _do_download FETCH_URL="$(SOURCE_URL_NIGHTLY)"

download:
	$(MAKE) --no-print-directory _do_download FETCH_URL="$(SOURCE_URL_DOWNLOAD)"

downloads:
	$(MAKE) --no-print-directory _do_download FETCH_URL="$(SOURCE_URL_GITHUB)"

# Test
test: build
	@if [ -x "./$(PROG)" ]; then ./$(PROG) autotest -all; else echo "Run 'make' first"; exit 1; fi

test-nointel: nointel
	@if [ -x "./$(PROG)" ]; then ./$(PROG) autotest -all; else echo "Run 'make nointel' first"; exit 1; fi

clean:
	$(RM) $(PROG)

# Check (with JIT and SFTP status)
check:
	@echo "OS           : $(UNAME_S)"
	@echo "Architecture : $(UNAME_M)"
	@echo "Compiler     : $(CXX)"
	@$(CXX) --version 2>&1 | head -n 5 || $(CXX) -v 2>&1 | head -n 5 || echo "(version not available)"
	@echo "ZPAQ_CPPFLAGS: $(ZPAQ_CPPFLAGS)"
	@echo "ZPAQ_CXXFLAGS: $(ZPAQ_CXXFLAGS)"
	@echo "LDLIBS       : $(LDLIBS)"
	@echo "JIT enabled  : $(if $(findstring -DNOJIT,$(ZPAQ_CPPFLAGS)),no (non-x86 or forced),yes (x86))"
	@echo "SFTP enabled : $(ENABLE_SFTP)"
	@echo "Mount enabled: $(ENABLE_ZPAQMOUNT)"
	@if [ "$(ENABLE_ZPAQMOUNT)" = "yes" ]; then echo "FUSE_CFLAGS  : $(FUSE_CFLAGS)"; echo "FUSE_LIBS    : $(FUSE_LIBS)"; fi
	@echo "BINDIR       : $(BINDIR)"
	@echo ""
	@echo "Download URLs:"
	@echo "  nightly    : $(SOURCE_URL_NIGHTLY)"
	@echo "  download   : $(SOURCE_URL_DOWNLOAD)"
	@echo "  downloads  : $(SOURCE_URL_GITHUB)"

# Help
help:
	@echo ""
	@echo "zpaqfranz -- Universal Makefile (Build 1.6)"
	@echo ""
	@echo "Build:"
	@echo "  make                   Compile zpaqfranz (JIT auto on x86 only)"
	@echo "  make static            Static build, no SFTP, no mount (NAS, containers, rescue)"
	@echo "  make nointel           Force without JIT (even on Intel)"
	@echo "  make debug             Debug build (no optimization)"
	@echo ""
	@echo "Options:"
	@echo "  ENABLE_SFTP=no         Disable SFTP support"
	@echo "  ENABLE_ZPAQMOUNT=yes   Enable the mount command (needs FUSE: libfuse3,"
	@echo "                         OpenBSD the FUSE of the base system)"
	@echo "  FUSE_CFLAGS=... FUSE_LIBS=...  FUSE by hand, where pkg-config has no fuse3"
	@echo "  CROSS_COMPILE=prefix-  Cross-compile (e.g. aarch64-linux-gnu-)"
	@echo ""
	@echo "Install / Uninstall:"
	@echo "  make install           Install in $(BINDIR)"
	@echo "  make install-clean     Compile + install + clean local"
	@echo "  make install-nointel   Install without JIT"
	@echo "  make uninstall         Remove zpaqfranz (and 'dir' symlink) from system"
	@echo ""
	@echo "Download source (three choices, pick one):"
	@echo "  make nightly           Nightly (latest, risky!)"
	@echo "  make download          Latest official release from my site"
	@echo "  make downloads         Stable from GitHub with https"
	@echo ""
	@echo "Test:"
	@echo "  make test              Full tests"
	@echo "  make test-nointel      Tests on no-JIT build"
	@echo ""
	@echo "Utility:"
	@echo "  make check             Show configuration (JIT + SFTP + mount status)"
	@echo "  make clean             Delete local binary"
	@echo "  make help              This message"
	@echo ""
	@echo "Note for BSD/Solaris: use 'gmake' instead of 'make'"