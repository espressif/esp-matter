# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := wifi_hosal.c

ifeq ($(CONFIG_CHIP_NAME),BL808)
COMPONENT_SRCS += port/wifi_hosal_bl808.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL606P)
COMPONENT_SRCS += port/wifi_hosal_bl606p.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL602)
COMPONENT_SRCS += port/wifi_hosal_bl602.c
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := . port

##
#CPPFLAGS +=
