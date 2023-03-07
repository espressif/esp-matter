################################################################################
# Toolchain definitions                                                        #
#                                                                              #
# This module automatically detects the environment it is running on           #
# and sets the following variables:                                            #
#   - OS: either 'win' on native Windows environment (e.g. MinGW)              #
#         or 'posix' in case of Linux, macOS or emulated POSIX environments    #
#         on Windows (e.g. Cygwin, MSYS2). Can be overridden externally.       #
#         When exporting, both 'win' and 'posix' are active.                   #
#   - UNAME: the value returned by the 'uname' command in lower case.          #
#            If 'uname' command is not available, the value 'unknown' is used. #
#            Can be overridden externally.                                     #
#   - UNAME_M: the value returned by 'uname -m' command in lower case.         #
#              If 'uname' command is not available, the value 'unknown' is     #
#              used. Can be overridden externally.                             #
#   - NULLDEVICE: '/dev/nul' on POSIX environment, 'NUL' on Windows.           #
#   - CC: name of the C compiler.                                              #
#   - LD: name of the linker.                                                  #
#   - AR: name of the archive utility.                                         #
#                                                                              #
# You might need to do changes to match with your system setup.                #
################################################################################

GCC_PREFIX :=

NULLDEVICE := /dev/null

ifeq (, $(wildcard $(NULLDEVICE)))
  # Assuming Windows environment.
  NULLDEVICE := NUL
endif

# Check if UNAME has any value.
ifeq (, $(UNAME))
  # Run uname command.
  override UNAME = $(shell uname 2>$(NULLDEVICE) || echo unknown)
endif
override UNAME := $(shell echo $(UNAME) | tr '[:upper:]' '[:lower:]' )

# Check if UNAME_M has any value.
ifeq (, $(UNAME_M))
  # Run uname command.
  override UNAME_M = $(shell uname -m 2>$(NULLDEVICE) || echo unknown)
endif
override UNAME_M := $(shell echo $(UNAME_M) | tr '[:upper:]' '[:lower:]' )

override OS := $(shell echo $(OS) | tr '[:upper:]' '[:lower:]' )

# Check if OS has a valid value.
ifeq (, $(filter $(OS), win posix))
  ifneq (, $(filter $(MAKECMDGOALS), export))
    # Set all the valid OS values for exporting.
    override OS := posix win
  else ifneq (, $(filter $(UNAME), darwin linux))
    # Native POSIX environment detected.
    override OS := posix
  else
    override OS := win
    # If MinGW compiler is available in PATH, it will take precedence.
    ifneq (, $(shell x86_64-w64-mingw32-gcc --version 2>$(NULLDEVICE)))
      # MinGW GCC (64-bit) found.
      GCC_PREFIX := x86_64-w64-mingw32-
    else ifneq (, $(shell i686-w64-mingw32-gcc --version 2>$(NULLDEVICE)))
      # MinGW GCC (32-bit) found.
      GCC_PREFIX := i686-w64-mingw32-
    else
      # Windows environment is assumed but no MinGW GCC installation found in PATH.
      ifeq ($(UNAME), unknown)
        # Continue as if on a native Windows environment.
      else
        # Continue as if on an emulated POSIX environment (e.g. MSYS or Cygwin).
        override OS := posix
      endif
    endif
  endif
endif

CC = $(GCC_PREFIX)gcc
LD = $(GCC_PREFIX)ld
AR = $(GCC_PREFIX)ar

################################################################################
# Default flags                                                                #
################################################################################

# -MMD : Don't generate dependencies on system header files.
# -MP  : Add phony targets, useful when a h-file is removed from a project.
# -MF  : Specify a file to write the dependencies to.
DEPFLAGS = \
-MMD \
-MP \
-MF $(@:.o=.d)

override CFLAGS += \
-fno-short-enums \
-Wall \
-c \
-fmessage-length=0 \
-std=c99 \
$(DEPFLAGS)

# Linux platform: if _DEFAULT_SOURCE is defined, the default is to have _POSIX_SOURCE set to one
# and _POSIX_C_SOURCE set to 200809L, as well as enabling miscellaneous functions from BSD and SVID.
# See usr/include/fetures.h for more information.
#
# _BSD_SOURCE (deprecated since glibc 2.20)
# Defining this macro with any value causes header files to expose BSD-derived definitions.
# In glibc versions up to and including 2.18, defining this macro also causes BSD definitions to be
# preferred in some situations where standards conflict, unless one or more of _SVID_SOURCE,
# _POSIX_SOURCE, _POSIX_C_SOURCE, _XOPEN_SOURCE, _XOPEN_SOURCE_EXTENDED, or _GNU_SOURCE is defined,
# in which case BSD definitions are disfavored. Since glibc 2.19, _BSD_SOURCE no longer causes BSD
# definitions to be preferred in case of conflicts. Since glibc 2.20, this macro is deprecated.
# It now has the same effect as defining _DEFAULT_SOURCE, but generates a compile-time warning
# (unless _DEFAULT_SOURCE is also defined). Use _DEFAULT_SOURCE instead.
# To allow code that requires _BSD_SOURCE in glibc 2.19 and earlier and _DEFAULT_SOURCE in glibc
# 2.20 and later to compile without warnings, define both _BSD_SOURCE and _DEFAULT_SOURCE.
#
# OSX platform: _DEFAULT_SOURCE is not used, instead _DARWIN_C_SOURCE is defined by default.
ifeq ($(OS),posix)
override CFLAGS += \
-D_DEFAULT_SOURCE \
-D_BSD_SOURCE
endif

ifeq ($(UNAME), linux)
# Link math library per default on Linux to avoid missing symbols from math.h.
override LDFLAGS += -lm
endif

ifeq ($(OS),win)
# Avoid runtime errors because of missing DLLs by using static linking.
override LDFLAGS += -static
endif

# Common macros to identify toolchain in source code
override CFLAGS += -DHOST_TOOLCHAIN
ifeq ($(OS),posix)
override CFLAGS += -DPOSIX
endif
