SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

OBJS = diskio.obj \
	ff.obj \
	ffcio.obj \
	ffsystem.obj \
	ffunicode.obj \
	ramdisk.obj

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
	"-I$(CCS_ARMCOMPILER)/include" --silicon_version=7M4 --code_state=16 \
	--little_endian --display_error_number --diag_warning=255 --diag_wrap=off \
	--gen_func_subsections=on --float_support=vfplib -O2 --symdebug:dwarf

ARFLAGS = -r

vpath %.c ../../../

%.obj: %.c
	@ $(CC) $(CFLAGS) $< --output_file=$@

all: fatfs.a

clean:
	@ $(RM) $(OBJS) fatfs.a > $(DEVNULL) 2>&1

fatfs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^
