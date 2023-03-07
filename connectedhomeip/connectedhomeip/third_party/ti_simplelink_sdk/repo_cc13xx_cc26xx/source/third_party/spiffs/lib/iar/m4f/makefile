SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

OBJS = spiffs_cache.obj \
	spiffs_check.obj \
	spiffs_gc.obj \
	spiffs_hydrogen.obj \
	spiffs_nucleus.obj \
	SPIFFSNVS.obj

OBJS_CC26XX = $(subst .obj,_cc26xx.obj,$(OBJS))

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
	"-I$(IAR_ARMCOMPILER)/inc" --cpu=Cortex-M4F --debug --silent -e --aeabi \
	--thumb --diag_suppress=Pa050,Go005 --endian=little --fpu=VFPv4_sp -Ohs

CFLAGS_CC26XX = $(CFLAGS) -DCC13XX_CC26XX_WRITE_LIMIT

vpath %.c ../../../

%_cc26xx.obj: %.c
	@ $(CC) $(CFLAGS_CC26XX) $< -o $@

%.obj: %.c
	@ $(CC) $(CFLAGS) $< -o $@

all: spiffs.a spiffs_cc26xx.a

clean:
	@ $(RM) $(OBJS) spiffs.a $(OBJS_CC26XX) spiffs_cc26xx.a > $(DEVNULL) 2>&1

spiffs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $@ $^

spiffs_cc26xx.a: $(OBJS_CC26XX)
	@ echo "# Archiving $@"
	@ $(AR) $@ $^
