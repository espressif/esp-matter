#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

ifeq ($(CONFIG_USB_CDC),1)
CPPFLAGS += -DCFG_USB_CDC_ENABLE
endif

ifeq ($(CONFIG_BT),1)
include $(BL60X_SDK_PATH)/components/network/ble/ble_common.mk
endif

ifeq ($(CONFIG_LMAC154_CASE),)
CPPFLAGS += -DLMAC154_CASE=0
else
CPPFLAGS += -DLMAC154_CASE=$(CONFIG_LMAC154_CASE)
endif

ifeq ($(CONFIG_LMAC154_TXRX),)
CPPFLAGS += -DLMAC154_TXRX=1
else
CPPFLAGS += -DLMAC154_TXRX=$(CONFIG_LMAC154_TXRX)
endif

ifeq ($(CONFIG_LMAC154_TX_RATE),)
CPPFLAGS += -DLMAC154_TX_RATE=0
else
CPPFLAGS += -DLMAC154_TX_RATE=$(CONFIG_LMAC154_TX_RATE)
endif
