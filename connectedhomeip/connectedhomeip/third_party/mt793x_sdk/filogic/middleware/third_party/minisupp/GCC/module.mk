MINISUPP_SRC = middleware/third_party/minisupp
MINISUPP_CONFIG = src_protected/wpa_supplicant_2.10/wpa_supplicant

# LIB VER control SDK X.X.X-SHA_8_DIG
SuppTOTShaXDig := $(shell cd $(SOURCE_DIR)/$(MINISUPP_SRC) && git log -1 --date=format:'%Y_%m_%d_%H_%M_%S' | grep commit| cut -d " " -f2 | cut -c 1-8)

ifneq ($(SuppTOTShaXDig),)
$(info SuppTOTShaXDig: $(SuppTOTShaXDig))
CFLAGS += -DSuppTOTShaXDig=\"$(SuppTOTShaXDig)\"
endif
# LIB build date code
SuppBuildTime := $(shell date +%Y-%m-%d-%H:%M:%S)
ifneq ($(SuppBuildTime),)
$(info SuppBuildTime: $(SuppBuildTime))
CFLAGS += -DSuppBuildTime=\"$(SuppBuildTime)\"
endif

#################################################################################
#include path
ifdef MTK_FREERTOS_VERSION
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source$(MTK_FREERTOS_VERSION)/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source$(MTK_FREERTOS_VERSION)/portable/GCC/ARM_CM33_NTZ
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source$(MTK_FREERTOS_VERSION)/portable/GCC/ARM_CM33_NTZ/non_secure
else
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/SourceV10/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/SourceV10/portable/GCC/ARM_CM33_NTZ
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/SourceV10/portable/GCC/ARM_CM33_NTZ/non_secure
endif
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7933/inc/
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/minisupp/inc
CFLAGS  += -I$(MINISUPP_SRC)/src_protected/wpa_supplicant_2.10/wpa_supplicant
CFLAGS  += -I$(MINISUPP_SRC)/src_protected/wpa_supplicant_2.10/src
CFLAGS  += -I$(MINISUPP_SRC)/src_protected/wpa_supplicant_2.10/src/utils
CFLAGS  += -I$(MINISUPP_SRC)/src_protected/wpa_supplicant_2.10/src/driver
CFLAGS  += -I$(DIR)/src/utils
CFLAGS  += -I$(DIR)/src/drivers
CFLAGS  += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/connectivity/wlan/include
CFLAGS  += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/connectivity/wlan/include/nic
CFLAGS  += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/connectivity/wlan/os/freertos/include
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS	+= -I$(SOURCE_DIR)/middleware/MTK/dhcpd/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

$(info MTK_WIFI_AP_ENABLE: $(MTK_WIFI_AP_ENABLE))
ifeq ($(MTK_WIFI_AP_ENABLE), y)
$(shell cp $(SOURCE_DIR)/$(MINISUPP_SRC)/$(MINISUPP_CONFIG)/.config_ap_sta $(SOURCE_DIR)/$(MINISUPP_SRC)/$(MINISUPP_CONFIG)/.config)
CFLAGS  += -DMTK_WIFI_AP_ENABLE_FLAG=1
CFLAGS  += -DCONFIG_AP
else
$(shell cp $(SOURCE_DIR)/$(MINISUPP_SRC)/$(MINISUPP_CONFIG)/.config_sta $(SOURCE_DIR)/$(MINISUPP_SRC)/$(MINISUPP_CONFIG)/.config)
CFLAGS  += -DMTK_WIFI_AP_ENABLE_FLAG=0
endif

ifeq ($(MTK_WIFI_ROUTER_ENABLE), y)
CFLAGS  += -DMTK_WIFI_ROUTER_ENABLE_FLAG=1
CFLAGS  += -DCONFIG_REPEATER
else
CFLAGS  += -DMTK_WIFI_ROUTER_ENABLE_FLAG=0
endif

CFLAGS  += -DCONFIG_IEEE80211W
CFLAGS  += -DCONFIG_SME
CFLAGS  += -DCONFIG_SAE
CFLAGS  += -DCONFIG_DRIVER_GEN4M

C_FILES += $(MINISUPP_SRC)/src_protected/wifi_os_api.c
C_FILES += $(MINISUPP_SRC)/src_protected/minisupp_cli_gen4m.c

ifneq ($(MTK_RELEASE_MODE),)
CFLAGS += -DMTK_M_RELEASE=1
CFLAGS += -DMTK_M_DEBUG=2
CFLAGS += -DMTK_M_MFG=3
ifeq ($(MTK_RELEASE_MODE), release)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_RELEASE
endif
ifeq ($(MTK_RELEASE_MODE), debug)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_DEBUG
endif
ifeq ($(MTK_RELEASE_MODE),mfg)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_MFG
endif
endif

