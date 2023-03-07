SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

OBJS = spiffs_cache.obj \
		spiffs_check.obj \
		spiffs_gc.obj \
		spiffs_hydrogen.obj \
		spiffs_nucleus.obj \
		SPIFFSNVS.obj

OBJS_CC26XX = $(subst .obj,_cc26xx.obj,$(OBJS))

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
		"-I$(TICLANG_ARMCOMPILER)/include" \
		-mcpu=cortex-m4 -msoft-float -mfpu=none -Oz

CFLAGS_CC26XX = $(CFLAGS) -DCC13XX_CC26XX_WRITE_LIMIT

ARFLAGS = -c -q

vpath %.c ../../../

%_cc26xx.obj: %.c
	@ $(CC) -c $(CFLAGS_CC26XX) $< -o $@

%.obj: %.c
	@ $(CC) -c $(CFLAGS) $< -o $@

all: spiffs.a spiffs_cc26xx.a

clean:
	@ $(RM) $(OBJS) spiffs.a  $(OBJS_CC26XX) spiffs_cc26xx.a  > $(DEVNULL) 2>&1

spiffs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^

spiffs_cc26xx.a: $(OBJS_CC26XX)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^
