################################################################################
# Makefile
#
# CPC_DIR must be set to the location wrom where cpc daemon is compiled.
#
################################################################################

.SUFFIXES:				# ignore builtin rules
.PHONY: all debug release clean

################################################################################
# General project settings                                                     #
################################################################################

################################################################################
# Set this variable to the location of your CPC directory cloned from Github.  #
# Follow the instructions at https://github.com/SiliconLabs/cpc-daemon.        #
################################################################################
CPC_DIR ?= cpc

# uniq is a function which removes duplicate elements from a list
uniq = $(strip $(if $1,$(firstword $1) \
       $(call uniq,$(filter-out $(firstword $1),$1))))


PROJECTNAME = bt_host_cpc_hci_bridge

OBJ_DIR = obj
EXE_DIR = exe
LST_DIR = lst

################################################################################
# Definitions of toolchain.                                                    #
# You might need to do changes to match your system setup                      #
################################################################################

RMDIRS     := rm -rf
RMFILES    := rm -rf
ALLFILES   := /*.*
NULLDEVICE := /dev/null

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

################################################################################
# Include paths                                                                #
################################################################################

INCLUDEPATHS ?= -I${CPC_DIR}/daemon/lib

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


override CFLAGS += \
-D_DEFAULT_SOURCE \
-D_BSD_SOURCE

# NOTE: The -Wl,--gc-sections flag may interfere with debugging using gdb.
override LDFLAGS +=

################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
bridge.c

LIBS = $(CPC_DIR)/daemon/build/libcpc.so

override LDFLAGS += -lutil

################################################################################
# Rules                                                                        #
################################################################################

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
