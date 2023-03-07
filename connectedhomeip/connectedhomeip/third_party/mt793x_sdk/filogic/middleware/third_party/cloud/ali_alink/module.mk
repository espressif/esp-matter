
ALINK_SRC = middleware/third_party/cloud/ali_alink

#C_FILES  += $(ALINK_SRC)/src/demo.c
C_FILES  += $(ALINK_SRC)/src/platform_freertos.c
#C_FILES  += $(ALINK_SRC)/src/zconfig_vendor_mtk.c
C_FILES  += $(ALINK_SRC)/src/aws_platform_mtk.c
C_FILES  += $(ALINK_SRC)/src/aws_softap.c
C_FILES  += $(ALINK_SRC)/src/sample.c

#################################################################################
#include path
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/inc
CFLAGS+= -I$(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/inc/aws
CFLAGS+= -I$(SOURCE_DIR)/middleware/mtk/smtcn/inc/
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

LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/lib/libalink.a
LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/lib/libaws.a

