# Component Makefile

ifeq ($(CONFIG_ETHERNET), 1)
CFLAGS   += -DCFG_ETHERNET_ENABLE
CPPFLAGS += -DCFG_ETHERNET_ENABLE
endif

#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += src/include src/include/compat/posix lwip-port lwip-port/FreeRTOS lwip-port/arch

ifneq ($(CONFIG_CHIP_NAME),BL616)
# bl616 lwipopt.h define in wifi6_lwip_adapter
COMPONENT_ADD_INCLUDEDIRS += lwip-port/config
else
# bl616 need lwip errno.h
COMPONENT_ADD_INCLUDEDIRS += src/include/compat/stdc
endif

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
#COMPONENT_SRCS :=
#COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src/api src/core src/core/ipv4 src/netif lwip-port/FreeRTOS lwip-port src/apps/altcp_tls

ifeq ($(CONFIG_COMPONENT_BUGKILLER_ENABLE),1)
COMPONENT_ADD_INCLUDEDIRS += bugkiller/include
COMPONENT_SRCDIRS += bugkiller
CFLAGS += -DBUGKILLER
endif

ifeq ($(CONFIG_IPV6), 1)
COMPONENT_SRCDIRS += src/core/ipv6
endif
##
#CPPFLAGS +=
