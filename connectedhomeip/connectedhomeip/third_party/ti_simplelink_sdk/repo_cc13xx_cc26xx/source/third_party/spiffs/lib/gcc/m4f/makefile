SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
AR = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc-ar"

OBJS = spiffs_cache.obj \
	spiffs_check.obj \
	spiffs_gc.obj \
	spiffs_hydrogen.obj \
	spiffs_nucleus.obj \
	SPIFFSNVS.obj

OBJS_CC26XX = $(subst .obj,_cc26xx.obj,$(OBJS))

CFLAGS= -I../../../ -I$(SIMPLELINK_SDK_INSTALL_DIR)/source -mcpu=cortex-m4 \
	-march=armv7e-m -mthumb -std=c99 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	-ffunction-sections -fdata-sections -g -O2 -gstrict-dwarf -Wall \
	--specs=nano.specs

CFLAGS_CC26XX = $(CFLAGS) -DCC13XX_CC26XX_WRITE_LIMIT

ARFLAGS = -r

vpath %.c ../../../

%_cc26xx.obj: %.c
	@ $(CC) $(CFLAGS_CC26XX) $< -c -o $@

%.obj: %.c
	@ $(CC) $(CFLAGS) $< -c -o $@

all: spiffs.a spiffs_cc26xx.a

clean:
	@ $(RM) $(OBJS) spiffs.a $(OBJS_CC26XX) spiffs_cc26xx.a > $(DEVNULL) 2>&1

spiffs.a: $(OBJS)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^

spiffs_cc26xx.a: $(OBJS_CC26XX)
	@ echo "# Archiving $@"
	@ $(AR) $(ARFLAGS) $@ $^
