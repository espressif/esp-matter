
AIRKISS_SRC = middleware/third_party/cloud/tencent_weixin

C_FILES  += $(AIRKISS_SRC)/src/airkiss.c
#C_FILES  += $(AIRKISS_SRC)/src/airkiss_cloud.c
C_FILES  += $(AIRKISS_SRC)/src/airkiss_porting_freertos.c
C_FILES  += $(AIRKISS_SRC)/src/app_start.c

#################################################################################
#include path
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/MTK/smtcn/inc/
CFLAGS+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS+= -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS+= -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/posix
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/posix/sys
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip

LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin/lib/libairkiss.a
#LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin/lib/libwxcloud.a

