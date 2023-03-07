# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += posix/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/vfs.c \
        src/vfs_file.c \
        src/vfs_inode.c \
        src/vfs_register.c \
        device/vfs_uart.c \
        device/vfs_adc.c \
        device/vfs_spi.c \
        device/vfs_gpio.c \
        device/vfs_pwm.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src device


##
#CPPFLAGS +=
ifeq ($(CONFIG_USB_CDC),1)
CPPFLAGS += -DCFG_USB_CDC_ENABLE
endif
