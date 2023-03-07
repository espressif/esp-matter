# Component Makefile
#

ifeq ($(CONFIG_SYS_VFS_ENABLE),1)
CPPFLAGS += -DSYS_VFS_ENABLE
endif

ifeq ($(CONFIG_SYS_TRACE_MEM_ENABLE),1)
CONFIG_SYS_TRACE_MEM_ENTRY_NUM ?= 1000
CPPFLAGS += -DSYS_TRACE_MEM_ENABLE -DSYS_TRACE_MEM_ENTRY_NUM=$(CONFIG_SYS_TRACE_MEM_ENTRY_NUM)
endif

ifeq ($(CONFIG_USE_PSRAM), 1)
CFLAGS   += -DCFG_USE_PSRAM
CPPFLAGS += -DCFG_USE_PSRAM
endif

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ./

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := ./syscalls.c ./assert.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := ./
