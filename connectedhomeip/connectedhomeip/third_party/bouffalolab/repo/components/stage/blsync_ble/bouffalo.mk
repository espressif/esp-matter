# Component Makefile

include $(BL60X_SDK_PATH)/components/network/ble/ble_common.mk

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += inc

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/blsync_ble.c \
				  src/encrypt_layer.c \
				  src/payload.c \
				  src/transfer.c 
					
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src


##
#CPPFLAGS +=
