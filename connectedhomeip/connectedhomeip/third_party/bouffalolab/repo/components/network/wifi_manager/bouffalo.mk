# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += bl60x_wifi_driver/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := bl60x_wifi_driver

## This component's src
COMPONENT_SRCS := bl60x_wifi_driver/ipc_host.c \
				  bl60x_wifi_driver/bl_cmds.c \
				  bl60x_wifi_driver/bl_irqs.c \
				  bl60x_wifi_driver/bl_main.c \
				  bl60x_wifi_driver/bl_mod_params.c \
				  bl60x_wifi_driver/bl_msg_rx.c \
				  bl60x_wifi_driver/bl_msg_tx.c \
				  bl60x_wifi_driver/bl_platform.c \
				  bl60x_wifi_driver/bl_rx.c \
				  bl60x_wifi_driver/bl_tx.c \
				  bl60x_wifi_driver/bl_utils.c \
				  bl60x_wifi_driver/stateMachine.c \
				  bl60x_wifi_driver/wifi.c \
				  bl60x_wifi_driver/wifi_mgmr.c \
				  bl60x_wifi_driver/wifi_mgmr_api.c \
				  bl60x_wifi_driver/wifi_mgmr_cli.c \
				  bl60x_wifi_driver/wifi_mgmr_ext.c \
				  bl60x_wifi_driver/wifi_mgmr_profile.c \
				  bl60x_wifi_driver/wifi_netif.c \
				  bl60x_wifi_driver/wifi_mgmr_event.c \
				  bl60x_wifi_driver/wifi_pkt_hooks.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := bl60x_wifi_driver

ifeq ($(CONFIG_CHIP_NAME),BL602)
CPPFLAGS += -DCFG_TXDESC="2"
endif

ifeq ($(CONFIG_CHIP_NAME),BL808)
CPPFLAGS += -DCFG_TXDESC="4"
endif

ifeq ($(CONFIG_CHIP_NAME),BL606P)
CPPFLAGS += -DCFG_TXDESC="4"
endif

ifeq ($(CONFIG_BL_IOT_FW_AP),1)
CPPFLAGS += -DCFG_STA_MAX="5"
else
CPPFLAGS += -DCFG_STA_MAX="1"
endif

ifeq ($(CONFIG_BL_IOT_FW_STATISTIC),1)
CPPFLAGS += -DCFG_BL_STATISTIC
endif

ifeq ($(CONFIG_CHIP_NAME),BL808)
CPPFLAGS += -DCFG_CHIP_BL808
endif

ifeq ($(CONFIG_CHIP_NAME),BL606P)
CPPFLAGS += -DCFG_CHIP_BL606P
endif

ifeq ($(CONFIG_CHIP_NAME),BL602)
CPPFLAGS += -DCFG_CHIP_BL602
endif

ifeq ($(CONFIG_WIFI_PKT_INPUT_HOOK),1)
CPPFLAGS += -D PKT_INPUT_HOOK
endif
ifeq ($(CONFIG_WIFI_PKT_OUTPUT_HOOK),1)
CPPFLAGS += -D PKT_OUTPUT_HOOK
endif

##
#CPPFLAGS +=
