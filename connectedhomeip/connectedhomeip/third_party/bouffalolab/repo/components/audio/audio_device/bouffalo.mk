# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += audio_device/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := audio_device/include

## This component's src 
ifeq ($(CONFIG_CHIP_NAME),BL602)
COMPONENT_SRCS := bl602_dac/bl602_dac_device.c
COMPONENT_SRCDIRS := bl602_dac
endif

ifeq ($(CONFIG_CHIP_NAME),BL60X)
COMPONENT_SRCS := bl60x_i2s/bl60x_i2s_device.c 
COMPONENT_SRCDIRS := bl60x_i2s
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

##
#CPPFLAGS += 
