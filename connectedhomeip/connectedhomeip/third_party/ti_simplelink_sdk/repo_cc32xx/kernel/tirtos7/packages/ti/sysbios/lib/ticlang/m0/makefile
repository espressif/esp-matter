SYSBIOS_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SYSBIOS_INSTALL_DIR)/imports.conan.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

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
       boot_cortex_m.obj

CFLAGS = -DBIOS_mpeEnabled_D=0 -DBIOS_smpEnabled_D=0 \
         -DTaskSupport_usesMonitors__D -DHwi_dispatcherSwiSupport_D \
         -I../../../../.. -I$(TICLANG_ARMCOMPILER)/include \
         -mcpu=cortex-m0plus -msoft-float -mfpu=none -Oz

ARFLAGS = -c -q

vpath %.sv6M ../../../family/arm/v6m
vpath %.c ../../../family/arm/vxm/boot
vpath %.asm ../../../family/arm/vxm/boot

%.obj:%.c
	@ echo cc $@ ...
	@ $(CC) $(CFLAGS) -c $< -o $@

%.obj:%.asm
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) -c -x assembler-with-cpp $< -o $@

%.obj:%.sv6M
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) -c -x assembler-with-cpp  $< -o $@

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
