# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += .

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src
COMPONENT_SRCS = blprog_cmd.c blprog_ctrl.c

CONFIG_PROG_TARGET ?= BL602
ifeq ("$(CONFIG_PROG_TARGET)", "BL602")
COMPONENT_SRCS += bl602_eflash_loader_rc32m.c
else ifeq ("$(CONFIG_PROG_TARGET)", "BL702")
COMPONENT_SRCS += bl702_eflash_loader_rc32m.c
else
CPPFLAGS += -DNO_EFLASH_LOADER
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := .


##
CPPFLAGS += -DARCH_RISCV -DBFLB_CRYPT_HARDWARE -DBFLB_PKA_HARDWARE
CPPFLAGS += -D$(CONFIG_CHIP_NAME)
