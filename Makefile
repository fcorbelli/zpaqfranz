# Universal Makefile for zpaqfranz
# Build 1.0a 2025-04-06
# By Franco Corbelli, under MIT License
# Tested on modern Linux (e.g., Debian 13 with g++ 14), BSD, macOS, Solaris, etc.

# Usage:
#
# make                         Compile zpaqfranz
# make install                 Install zpaqfranz in /usr/local/bin (or PREFIX)
# make install clean           Compile, install, then delete local binary
# make nointel                 Compile without Intel-specific optimizations (no HWSHA2)
# make install-nointel         Compile without Intel optimizations and install
# make debug                   Compile without optimization (for debugging)
# make clean                   Delete zpaqfranz from local folder
# make uninstall               Delete zpaqfranz from system
# make download                Download latest source code
# make check                   Show build configuration
# make test                    Run all automated tests (zpaqfranz autotest -all)
# make test-nointel            Run all automated tests on nointel build
# make help                    Show this help message

# Typically "Makefile" for zpaqfranz on newer Linux boxes is
# g++ -O3 zpaqfranz.cpp -o zpaqfranz -pthread

# I don't like Makefiles at all, I try to avoid them, they are complicated and often don't work.
# Anyway, if you're lazy, you can try this.

# Try to catch as much not-Windows as possible

# Detect OS
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Compiler: prefer clang++ if available, fallback to g++
CXX ?= clang++
ifeq ($(shell command -v $(CXX) >/dev/null 2>&1 || echo "no"),no)
  CXX = g++
endif

# Default tools
INSTALL ?= install
RM ?= rm -f
MKDIR ?= mkdir -p
LN ?= ln -sf

# Program details
PROG := zpaqfranz

# zpaqfranz behaves differently than usual when called 'dir'. 
# It operates in a way that is quite similar to the corresponding Windows command. 
# This may conflict with other programs named 'dir', BEWARE!

ALTNAME := dir
SOURCE := zpaqfranz.cpp
SOURCE_URL := http://www.francocorbelli.it/zpaqfranz/win64/zpaqfranz.cpp

# Base compiler flags (optimization, warnings, threading)
CXXFLAGS ?= -O3 -Wall -pthread -s

# Default preprocessor flags (customizable via environment or command line)
CPPFLAGS ?= -DSFTP -DHWSHA2

# Intel-specific flags (can be removed with nointel target)
INTEL_FLAGS := -DHWSHA2

# Default libraries
LDLIBS ?= -lm

# Install directories (customizable via PREFIX)
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# OS/system-specific settings 

# -DHWSHA2      Enable HW SHA2 (without assembly code to be linked), Intel CPUs
# -DSFTP	    Enable SFTP support via curl (needs the library!)
# -DSOLARIS     Solaris is similar, but not equal, to BSD Unix
# -DANCIENT     Turn off some functions for compiling in very old systems (PowerPC Mac, Synology 7.1)
# -DNAS	        Like ANCIENT, but a bit less stringent (ex. Synology 7.2)
# -DBIG	        Turn on BIG ENDIAN at compile time
# -DESX	        Yes, zpaqfranz run on ESXi too :-)
# -DALIGNMALLOC Force malloc to be aligned at something (sparc64). Use naive CRC-32

# -Dunix        Compile on "something different from Windows (autodetected, in fact obsolete)
# -DNOJIT       Compile to non-Intel CPU (autodetected, in fact obsolete)
# -DIPV6        Do not force IPv4 (experimental)
# -DNOLM        Turn off the lm library (experimental)

# OS-specific adjustments
ifeq ($(UNAME_S),Darwin)
  BINDIR := /usr/local/bin
else ifeq ($(UNAME_S),SunOS)
  CPPFLAGS += -DSOLARIS
endif

# Architecture-specific adjustments
ifeq ($(UNAME_M),ppc)
  CPPFLAGS += -DANCIENT -DBIG
  CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(CPPFLAGS))
else ifeq ($(UNAME_M),ppc64)
  CPPFLAGS += -DANCIENT -DBIG
  CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(CPPFLAGS))
else ifeq ($(UNAME_M),sparc64)
  CPPFLAGS += -DALIGNMALLOC
  CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(CPPFLAGS))
endif

# Phony targets
.PHONY: all build install uninstall clean check help debug download nointel install-nointel test test-nointel

# Default target
all: build

# Build the program
build: $(PROG)

$(PROG): $(SOURCE)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

# make debug for a quick-and-dirty: faster compiling
debug: CXXFLAGS = -g -O0 -Wall -Wextra -pthread
debug: $(PROG)
	@echo "Debug build completed"

# Non-Intel build. BTW -DNOJIT is obsolete
nointel: CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(CPPFLAGS)) -DNOJIT
nointel: clean build
	@echo "Built without Intel-specific optimizations"

# Install with Intel optimizations
install: build
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0755 $(PROG) $(DESTDIR)$(BINDIR)/$(PROG)
	@if [ ! -e $(DESTDIR)$(BINDIR)/$(ALTNAME) ] && [ ! -L $(DESTDIR)$(BINDIR)/$(ALTNAME) ]; then \
		echo "Creating symbolic link $(DESTDIR)$(BINDIR)/$(ALTNAME)"; \
		$(LN) $(PROG) $(DESTDIR)$(BINDIR)/$(ALTNAME); \
	else \
		echo "$(ALTNAME) already exists in $(DESTDIR)$(BINDIR), skipping"; \
	fi

# Install without Intel optimizations
install-nointel: CPPFLAGS := $(filter-out $(INTEL_FLAGS),$(CPPFLAGS)) -DNOJIT
install-nointel: clean build install
	@echo "Installed without Intel-specific optimizations"

# Download latest source code
download:
	@echo "Downloading latest version of zpaqfranz..."
	@$(RM) $(PROG) $(SOURCE)
	@if command -v wget >/dev/null 2>&1; then \
		echo "Using wget..."; \
		wget $(SOURCE_URL) -O $(SOURCE) && \
		echo "Download completed successfully."; \
	elif command -v curl >/dev/null 2>&1; then \
		echo "Using curl..."; \
		curl -L $(SOURCE_URL) -o $(SOURCE) && \
		echo "Download completed successfully."; \
	else \
		echo "Error: Neither wget nor curl found. Please install one of them or download $(SOURCE) manually."; \
		echo "Download URL: $(SOURCE_URL)"; \
		exit 1; \
	fi

# Run automated tests on standard build
test: build
	@echo "Running automated tests on standard build with Intel optimizations..."
	@if [ -x "./$(PROG)" ]; then \
		./$(PROG) autotest -all; \
	else \
		echo "Error: $(PROG) executable not found. Please build it first with 'make'."; \
		exit 1; \
	fi

# Run automated tests on non-Intel build
test-nointel: nointel
	@echo "Running automated tests on build without Intel optimizations..."
	@if [ -x "./$(PROG)" ]; then \
		./$(PROG) autotest -all; \
	else \
		echo "Error: $(PROG) executable not found. Please build nointel first."; \
		exit 1; \
	fi

# Uninstall the program and symlink
uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(PROG)
	@if [ -L $(DESTDIR)$(BINDIR)/$(ALTNAME) ]; then \
		echo "Removing symbolic link $(DESTDIR)$(BINDIR)/$(ALTNAME)"; \
		$(RM) $(DESTDIR)$(BINDIR)/$(ALTNAME); \
	else \
		echo "$(ALTNAME) not found in $(DESTDIR)$(BINDIR), skipping"; \
	fi

# Clean up
clean:
	$(RM) $(PROG)

# Check compiler version and configuration
check:
	@echo "OS: $(UNAME_S)"
	@echo "Architecture: $(UNAME_M)"
	@echo "Compiler: $(CXX)"
	@$(CXX) --version
	@echo "CPPFLAGS: $(CPPFLAGS)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDLIBS: $(LDLIBS)"
	@echo "BINDIR: $(BINDIR)"

# Help message
help:
	@echo "Usage:"
	@echo "  make                   Compile zpaqfranz (for Intel/AMD CPUs)"
	@echo "  make install           Install zpaqfranz in $(BINDIR)"
	@echo "  make install clean     Compile, install, then delete local binary"
	@echo "  ----------------------"
	@echo "  make nointel           Compile without Intel-specific optimizations"
	@echo "  make install-nointel   Compile without Intel optimizations and install"
	@echo "  ----------------------"
	@echo "  make debug             Compile without optimization (for debugging)"
	@echo "  make clean             Delete zpaqfranz from local folder"
	@echo "  make uninstall         Delete zpaqfranz from system"
	@echo "  make download          Download (one of the) latest source code from my site"
	@echo "  make check             Show build configuration"
	@echo "  ----------------------"
	@echo "  make test              Run all automated tests on standard build"
	@echo "  make test-nointel      Run all automated tests on build without Intel optimizations"
	@echo "  ----------------------"
	@echo "  make help              Show this help message"