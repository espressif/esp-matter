SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

OBJS = spiffs_cache.obj \
	spiffs_check.obj \
	spiffs_gc.obj \
	spiffs_hydrogen.obj \
	spiffs_nucleus.obj \
	SPIFFSNVS.obj

OBJS_CC26XX = $(subst .obj,_cc26xx.obj,$(OBJS))

CFLAGS = -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source \
	"-I$(CCS_ARMCOMPILER)/include" --silicon_version=7M4 --code_state=16 \
	--little_endian --display_error_number --diag_warning=255 --diag_wrap=off \
	--gen_func_subsections=on --float_support=FPv4SPD16 -O2 --symdebug:dwarf

CFLAGS_CC26XX = $(CFLAGS) -DCC13XX_CC26XX_WRITE_LIMIT

ARFLAGS = -r

vpath %.c ../../../

%_cc26xx.obj: %.c
	@ $(CC) $(CFLAGS_CC26XX) $< --output_file=$@

%.obj: %.c
	@ $(CC) $(CFLAGS) $< --output_file=$@

all: spiffs.a spiffs_cc26xx.a

clean:
	@ $(RM) $(OBJS) spiffs.a $(OBJS_CC26XX) spiffs_cc26xx.a > $(DEVNULL) 2>&1

spiffs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^

spiffs_cc26xx.a: $(OBJS_CC26XX)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^
