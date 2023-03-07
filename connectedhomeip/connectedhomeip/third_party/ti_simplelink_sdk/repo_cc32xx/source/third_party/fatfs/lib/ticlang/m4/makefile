SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

OBJS = diskio.obj \
	ff.obj \
	ffcio.obj \
	ffsystem.obj \
	ffunicode.obj \
	ramdisk.obj

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
    -mcpu=cortex-m4 -msoft-float -mfpu=none -Oz \

ARFLAGS = -c -q

vpath %.c ../../../

%.obj: %.c
	@ $(CC) -c $(CFLAGS) $< -o $@

all: fatfs.a

clean:
	@ $(RM) $(OBJS) fatfs.a > $(DEVNULL) 2>&1

fatfs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^
