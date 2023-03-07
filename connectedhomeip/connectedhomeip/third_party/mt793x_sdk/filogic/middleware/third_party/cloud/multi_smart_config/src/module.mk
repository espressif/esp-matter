MSC_SRC := middleware/third_party/cloud/multi_smart_config/src

C_FILES  += $(MSC_SRC)/msc_ctl.c\
            $(MSC_SRC)/msc_api.c\
            $(MSC_SRC)/msc_config.c
ifeq ($(MTK_AIRKISS_ENABLE),y)
C_FILES  += $(MSC_SRC)/msc_ak_adapter.c
endif

ifeq ($(MTK_SMTCN_V4_ENABLE),y)
C_FILES  += $(MSC_SRC)/msc_smnt_adapter.c
endif

ifeq ($(MTK_SMTCN_V5_ENABLE),y)
C_FILES  += $(MSC_SRC)/bsmtcn_adapter.c
endif

#################################################################################
#include path
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/joylink_smnt/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/multi_smart_config/inc
