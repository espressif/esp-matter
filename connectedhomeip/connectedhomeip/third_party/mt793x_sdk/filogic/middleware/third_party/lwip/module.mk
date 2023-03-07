
###################################################
# Sources
LWIP_SRC = middleware/third_party/lwip

LWIPDIR = middleware/third_party/lwip/src
include $(SOURCE_DIR)/middleware/third_party/lwip/src/Filelists.mk

ETHERNET_NETIF_FILE = $(LWIPDIR)/netif/ethernet.c

C_FILES  += $(COREFILES)           \
            $(CORE4FILES)          \
            $(CORE6FILES)          \
            $(APIFILES)            \
            $(ETHERNET_NETIF_FILE)

ifeq ($(MTK_TCP_PORT_MONITOR_ENABLE), y)
CFLAGS += -DMTK_TCP_PORT_MONITOR_ENABLE
C_FILES  += $(PORTMONITORFILES)
endif

ifeq ($(MTK_NB_MODEM_ENABLE), y)
C_FILES     += $(LWIP_SRC)/ports/nbnetif.c
CFLAGS += -DMTK_TCPIP_FOR_NB_MODULE_ENABLE
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/mux_ap/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ril/inc
endif
ifeq ($(MTK_MODEM_ENABLE), y)
C_FILES     += $(LWIP_SRC)/ports/psnetif.c
CFLAGS += -DMTK_TCPIP_FOR_EXTERNAL_MODULE_ENABLE
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/cmux
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/sio
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/sio_uart
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/app
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/cmux
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/sio
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/sio_uart
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/app
endif
ifeq ($(MTK_WIFI_CHIP_USE_MT5931),y)
C_FILES     += $(LWIP_SRC)/ports/mt5931_netif.c
CFLAGS += -DTCPIP_FOR_MT5931_ENABLE
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/wifi/mt5931/inc
endif
ifeq ($(MTK_WIFI_CHIP_USE_MT5932),y)
C_FILES     += $(LWIP_SRC)/ports/mt5932_netif.c
endif
ifneq ($(MTK_MT7933_CONSYS_ENABLE), y)
ifneq ($(MTK_WIFI_CHIP_USE_MT5931), y)
ifneq ($(MTK_WIFI_CHIP_USE_MT5932), y)
ifneq ($(MTK_MODEM_ENABLE), y)
ifneq ($(MTK_NB_MODEM_ENABLE), y)
#C_FILES     += $(LWIP_SRC)/ports/ethernetif.c
endif
endif
endif
endif
else
C_FILES     += $(LWIP_SRC)/ports/tcpip_wrapper.c
endif
C_FILES     += $(LWIP_SRC)/ports/sys_arch.c
C_FILES     += $(LWIP_SRC)/ports/lwip_log.c


ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES     += $(LWIP_SRC)/ports/lwip_cli.c
endif


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
