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

PSA_OBJS = TaskSupport_asm_gnu.obj Hwi_asm_gnu_tfm.obj Hwi_asm_switch_gnu.obj \
       boot_gnu.obj startup_gnu.obj

CFLAGS = -DBIOS_mpeEnabled_D=0 -DBIOS_smpEnabled_D=0 \
         -DTaskSupport_usesMonitors__D -DHwi_dispatcherSwiSupport_D \
         -I../../../../.. -I$(GCC_ARMCOMPILER)/include \
         -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 \
		 -fdata-sections -ffunction-sections -g -O2 -gstrict-dwarf -Wall --specs=nano.specs

ARFLAGS = -r

vpath %.sv8M ../../../family/arm/v8m
vpath %.c ../../../family/arm/vxm/boot
vpath %.asm ../../../family/arm/vxm/boot

%.obj:%.c
	@ echo cc $@ ...
	@ $(CC) $(CFLAGS) -c $< -o $@

%.obj:%.asm
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) -c -x assembler-with-cpp $< -o $@

%.obj:%.sv8M
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) -c -x assembler-with-cpp  $< -o $@

.all-files: sysbios.a sysbios_psa.a

.clean:
	@ echo cleaning ...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.a > $(DEVNULL) 2>&1

sysbios.a: $(OBJS)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) $(ARFLAGS) $@ $^

sysbios_psa.a: $(PSA_OBJS)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) $(ARFLAGS) $@ $^
	@ $(RM) *.obj > $(DEVNULL) 2>&1
