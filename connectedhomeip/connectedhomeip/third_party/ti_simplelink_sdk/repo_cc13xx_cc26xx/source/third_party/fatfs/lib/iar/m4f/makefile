SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

OBJS = diskio.obj \
	ff.obj \
	ffsystem.obj \
	ffunicode.obj \
	ramdisk.obj

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
	"-I$(IAR_ARMCOMPILER)/inc" --cpu=Cortex-M4F --debug --silent -e --aeabi \
	--thumb --diag_suppress=Pa050,Go005 --endian=little --fpu=VFPv4_sp -Ohs

vpath %.c ../../../

%.obj: %.c
	@ $(CC) $(CFLAGS) $< -o $@

all: fatfs.a

clean:
	@ $(RM) $(OBJS) fatfs.a > $(DEVNULL) 2>&1

fatfs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) $@ $^
