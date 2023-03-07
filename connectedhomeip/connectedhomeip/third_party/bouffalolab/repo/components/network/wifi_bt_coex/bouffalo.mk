# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include
							 
## not be exported to project level

## This component's src 
COMPONENT_SRCS := src/wifi_bt_coex.c \
				  src/wifi_bt_coex_cli.c \
				  src/wifi_bt_coex_ctx.c \

ifeq ($(CONFIG_CHIP_NAME),BL808)
COMPONENT_SRCS += src/wifi_bt_coex_impl_bl808.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL606P)
COMPONENT_SRCS += src/wifi_bt_coex_impl_bl606p.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL602)
COMPONENT_SRCS += src/wifi_bt_coex_impl_bl602.c
endif
				  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

##
#CPPFLAGS += 
