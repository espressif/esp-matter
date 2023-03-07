SYSBIOS_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SYSBIOS_INSTALL_DIR)/imports.conan.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
AR = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc-ar"

ifeq ("$(SHELL)","sh.exe")
#For Windows
    RM     = del
    DEVNULL = NUL
else
#For Linux
    RM     = rm -f
    DEVNULL = /dev/null
endif

OBJS = TaskSupport_asm_gnu.obj Hwi_asm_gnu.obj Hwi_asm_switch_gnu.obj \
       boot_gnu.obj startup_gnu.obj

CFLAGS = -DBIOS_mpeEnabled_D=0 -DBIOS_smpEnabled_D=0 \
         -DTaskSupport_usesMonitors__D -DHwi_dispatcherSwiSupport_D \
         -I../../../../.. -I$(GCC_ARMCOMPILER)/include \
         -mcpu=cortex-m4 \
         -march=armv7e-m -mthumb -std=c99 -ffunction-sections \
         -mfloat-abi=soft \
         -fdata-sections -g -O2 -gstrict-dwarf -Wall --specs=nano.specs

ARFLAGS = -r

vpath %.sv7M ../../../family/arm/m3
vpath %.c ../../../family/arm/vxm/boot
vpath %.asm ../../../family/arm/vxm/boot

%.obj:%.c
	@ echo cc $@ ...
	@ $(CC) -c $(CFLAGS) $< -o $@

%.obj:%.asm
	@ echo asm $@ ...
	@ $(CC) -c -x assembler-with-cpp $(CFLAGS) $< -o $@

%.obj:%.sv7M
	@ echo asm $@ ...
	@ $(CC) -c -x assembler-with-cpp $(CFLAGS) $< -o $@

.all-files: sysbios.a

.clean:
	@ echo cleaning ...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.a > $(DEVNULL) 2>&1

sysbios.a: $(OBJS)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) $(ARFLAGS) $@ $^
	@ $(RM) *.obj > $(DEVNULL) 2>&1
