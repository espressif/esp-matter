SYSBIOS_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SYSBIOS_INSTALL_DIR)/imports.conan.mak

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

ifeq ("$(SHELL)","sh.exe")
#For Windows
    RM     = del
    DEVNULL = NUL
else
#For Linux
    RM     = rm -f
    DEVNULL = /dev/null
endif

OBJS = TaskSupport_asm.obj Hwi_asm.obj Hwi_asm_switch.obj \
       boot.obj auto_init.obj

CFLAGS = -DBIOS_mpeEnabled_D=0 -DBIOS_smpEnabled_D=0 \
         -DTaskSupport_usesMonitors__D -DHwi_dispatcherSwiSupport_D \
         -I../../../../.. -I$(CCS_ARMCOMPILER)/include \
         -c -qq --silicon_version=6M0 --code_state=16 \
         --little_endian --display_error_number --diag_warning=255 \
         --diag_wrap=off --gen_func_subsections=on --float_support=vfplib \
         -O2 --symdebug:dwarf

ARFLAGS = -r

vpath %.sv6M ../../../family/arm/v6m
vpath %.c ../../../family/arm/vxm/boot
vpath %.asm ../../../family/arm/vxm/boot

%.obj:%.c
	@ echo cc $@ ...
	@ $(CC) $(CFLAGS) $< --output_file=$@

%.obj:%.asm
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) $< --output_file=$@

%.obj:%.sv6M
	@ echo asm $@ ...
	@ $(CC) $(CFLAGS) $< --output_file=$@

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
