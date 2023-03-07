####################################################################
# Makefile
#
# OS variable must either be 'posix' or 'win'. E.g. 'make OS=posix'.
# Error is thrown if OS variable is not equal with any of these.
#
####################################################################

.SUFFIXES:				# ignore builtin rules
.PHONY: all debug release clean

####################################################################
# Definitions                                                      #
####################################################################

# uniq is a function which removes duplicate elements from a list
uniq = $(strip $(if $1,$(firstword $1) \
       $(call uniq,$(filter-out $(firstword $1),$1))))

PROJECTNAME = bt_host_voice
SDK_DIR = ../../../..
OBJ_DIR = obj
EXE_DIR = exe
LST_DIR = lst


####################################################################
# Definitions of toolchain.                                        #
# You might need to do changes to match your system setup          #
####################################################################

RMDIRS     := rm -rf
RMFILES    := rm -rf
ALLFILES   := /*.*
NULLDEVICE := /dev/null

# Try to detect NULL device regardless of the environment we are running on.
ifeq (,$(wildcard $(NULLDEVICE)))
  NULLDEVICE := NUL
endif

UNAME = $(shell uname 2>$(NULLDEVICE) || echo unknown)
UNAME := $(shell echo $(UNAME) | tr '[:upper:]' '[:lower:]' )

# Try autodetecting the environment
ifeq ($(UNAME),unknown)
  # If uname command is not available, the host OS is most probably Windows
  OS:=win
else ifneq (,$(filter mingw%,$(UNAME)))
  # Using MinGW for native Windows
  OS:=win
else
  # Linux, macOS, MSYS2, Cygwin or any other POSIX environment
  OS:=posix
endif

# Create directories and do a clean which is compatible with parallell make
$(shell mkdir $(OBJ_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(EXE_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(LST_DIR)>$(NULLDEVICE) 2>&1)
ifeq (clean,$(findstring clean, $(MAKECMDGOALS)))
  ifneq ($(filter $(MAKECMDGOALS),all debug release),)
    $(shell $(RMFILES) $(OBJ_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
    $(shell $(RMFILES) $(EXE_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
    $(shell $(RMFILES) $(LST_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
  endif
endif

CC = gcc
LD = ld
AR = ar


####################################################################
# Flags                                                            #
####################################################################

INCLUDEPATHS += \
-I$(SDK_DIR)/app/bluetooth/common_host/uart \
-I$(SDK_DIR)/app/bluetooth/common_host/system \
-I$(SDK_DIR)/app/common/util/app_assert \
-I$(SDK_DIR)/app/bluetooth/common_host/app_assert/config \
-I$(SDK_DIR)/app/common/util/app_log \
-I$(SDK_DIR)/app/bluetooth/common_host/iostream_mock \
-I$(SDK_DIR)/app/bluetooth/common_host/app_log/config \
-I$(SDK_DIR)/protocol/bluetooth/inc \
-I$(SDK_DIR)/platform/common/inc

# -MMD : Don't generate dependencies on system header files.
# -MP  : Add phony targets, useful when a h-file is removed from a project.
# -MF  : Specify a file to write the dependencies to.
DEPFLAGS = \
-MMD \
-MP \
-MF $(@:.o=.d)

# Add -Wa,-ahld=$(LST_DIR)/$(@F:.o=.lst) to CFLAGS to produce assembly list files
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
-D_BSD_SOURCE \
-DPOSIX
endif

# HOST_TOOLCHAIN flag indicates host side compilation,
# SL_CATALOG_APP_LOG_PRESENT is used by app_log component.
override CFLAGS += \
-DHOST_TOOLCHAIN \
-DSL_CATALOG_APP_LOG_PRESENT

# NOTE: The -Wl,--gc-sections flag may interfere with debugging using gdb.
override LDFLAGS +=


####################################################################
# Files                                                            #
####################################################################

C_SRC +=  \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host.c \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host_api.c \
$(SDK_DIR)/app/common/util/app_log/app_log.c \
$(SDK_DIR)/app/bluetooth/common_host/iostream_mock/sl_iostream_handles.c \
main.c \
app.c \
parse.c \
scan.c \
config.c

# this file should be the last added
C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/uart/uart_$(OS).c

LIBS =


####################################################################
# Rules                                                            #
####################################################################

C_FILES = $(notdir $(C_SRC) )
#make list of source paths, uniq removes duplicate paths
C_PATHS = $(call uniq, $(dir $(C_SRC) ) )

C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
C_DEPS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.d))
OBJS = $(C_OBJS)

vpath %.c $(C_PATHS)

# Default build is debug build
all:      debug

debug:    CFLAGS += -O0 -g3
debug:    $(EXE_DIR)/$(PROJECTNAME)

release:  $(EXE_DIR)/$(PROJECTNAME)


# Create objects from C SRC files
$(OBJ_DIR)/%.o: %.c
	@echo "Building file: $<"
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Link
$(EXE_DIR)/$(PROJECTNAME): $(OBJS) $(LIBS)
	@echo "Linking target: $@"
	$(CC) $^ $(LDFLAGS) -o $@


clean:
ifeq ($(filter $(MAKECMDGOALS),all debug release),)
	$(RMDIRS) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR)
endif

# include auto-generated dependency files (explicit rules)
ifneq (clean,$(findstring clean, $(MAKECMDGOALS)))
-include $(C_DEPS)
endif
