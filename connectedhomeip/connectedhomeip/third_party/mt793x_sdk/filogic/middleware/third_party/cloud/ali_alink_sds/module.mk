
ALINK_SRC = middleware/third_party/cloud/ali_alink_sds

C_FILES  += $(ALINK_SRC)/src/sample.c
C_FILES  += $(ALINK_SRC)/src/mtk_os.c
C_FILES  += $(ALINK_SRC)/src/mtk_hardware.c
C_FILES  += $(ALINK_SRC)/src/mtk_wifi.c
C_FILES  += $(ALINK_SRC)/src/mtk_network.c
C_FILES  += $(ALINK_SRC)/src/product.c
C_FILES  += $(ALINK_SRC)/src/sds_log.c

#################################################################################
#include path
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/cloud/ali_alink_sds/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/inc/aws
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include/mbedtls
CFLAGS+= -I$(SOURCE_DIR)/middleware/MTK/dhcpd/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/posix
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS+= -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS+= -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS+= -I$(SOURCE_DIR)/driver/board/mt76x7_evb/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/MTK/fota/inc/76x7

ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt7687 mt7697))
LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/ali_alink_sds/lib/libalink_87.a
else
LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/ali_alink_sds/lib/libalink.a
endif