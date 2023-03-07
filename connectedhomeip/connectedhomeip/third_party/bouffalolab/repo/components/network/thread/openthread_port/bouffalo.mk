# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ../openthread/include ../openthread/src/core ../openthread/src ../openthread/examples/platforms

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := ot_alarm.c \
                  ot_diag.c \
                  ot_entropy.c \
                  ot_settings.c \
                  ot_logging.c \
                  ot_misc.c \
                  ot_radio.c \
                  ot_uart.c \
                  ot_system.c \
                  ot_freertos.c \
                  ot_memory.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := .


ifeq ($(CONFIG_SYS_AOS_LOOP_ENABLE),1)
CPPFLAGS += -DSYS_AOS_LOOP_ENABLE
endif

ifeq ($(CONFIG_SYS_AOS_CLI_ENABLE),1)
CPPFLAGS += -DSYS_AOS_CLI_ENABLE
endif

ifeq ($(CONFIG_USB_CDC),1)
CPPFLAGS += -DCFG_USB_CDC_ENABLE
endif