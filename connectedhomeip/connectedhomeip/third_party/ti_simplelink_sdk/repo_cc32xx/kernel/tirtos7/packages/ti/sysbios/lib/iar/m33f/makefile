SYSBIOS_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SYSBIOS_INSTALL_DIR)/imports.conan.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
ASM = "$(IAR_ARMCOMPILER)/bin/iasmarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

ifeq ("$(SHELL)","sh.exe")
#For Windows
    RM     = del
    DEVNULL = NUL
else
#For Linux
    RM     = rm -f
    DEVNULL = /dev/null
endif

OBJS = TaskSupport_asm_iar.obj Hwi_asm_iar.obj Hwi_asm_switch_iar.obj \
       cstartup_M.obj boot_iar.obj cmain_iar.obj

PSA_OBJS = TaskSupport_asm_iar.obj Hwi_asm_iar_tfm.obj Hwi_asm_switch_iar.obj \
       cstartup_M.obj boot_iar.obj cmain_iar.obj

CFLAGS = -I../../../../.. -I$(IAR_ARMCOMPILER)/inc \
         --cpu=Cortex-M33.fp --fpu=FPv5_sp \
         --debug --silent -e --aeabi --thumb \
         --diag_suppress=Pa050,Go005 --endian=little -Ohs

AFLAGS = -DBIOS_mpeEnabled_D=0 -DBIOS_smpEnabled_D=0 \
         -DTaskSupport_usesMonitors__D -DHwi_dispatcherSwiSupport_D \
         -S --cpu Cortex-M33.fp --endian little --thumb

vpath %.sv8M ../../../family/arm/v8m
vpath %.c ../../../family/arm/vxm/boot
vpath %.s ../../../family/arm/vxm/boot

%.obj:%.c
	@ echo cc $@ ...
	@ $(CC) $(CFLAGS) $< -o $@

%.obj:%.s
	@ echo asm $@ ...
	@ $(ASM) $(AFLAGS) $< -o $@

%.obj:%.sv8M
	@ echo asm $@ ...
	@ $(ASM) $(AFLAGS) $< -o $@

.all-files: sysbios.a sysbios_psa.a

.clean:
	@ echo cleaning ...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.a > $(DEVNULL) 2>&1

sysbios.a: $(OBJS)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) --silent -o $@ $^

sysbios_psa.a: $(PSA_OBJS)
	@ echo ar $@ ...
	@ $(RM) $@ > $(DEVNULL) 2>&1
	@ $(AR) --silent -o $@ $^
	@ $(RM) *.obj > $(DEVNULL) 2>&1
