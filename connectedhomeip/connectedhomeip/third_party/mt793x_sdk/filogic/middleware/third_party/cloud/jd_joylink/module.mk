
JOYLINK_SRC = middleware/third_party/cloud/jd_joylink

C_FILES  += 		 		$(JOYLINK_SRC)/auth/crc.c \
		 					$(JOYLINK_SRC)/auth/joylink_crypt.c \
		 					$(JOYLINK_SRC)/auth/uECC.c \
		 					$(JOYLINK_SRC)/example/joylink_extern.c \
		 					$(JOYLINK_SRC)/example/joylink_extern_json.c \
		 					$(JOYLINK_SRC)/example/joylink_extern_sub_dev.c \
		 					$(JOYLINK_SRC)/joylink/joylink_dev_lan.c \
		 					$(JOYLINK_SRC)/joylink/joylink_dev_sdk.c \
		 					$(JOYLINK_SRC)/joylink/joylink_dev_server.c \
		 					$(JOYLINK_SRC)/joylink/joylink_join_packet.c \
		 					$(JOYLINK_SRC)/joylink/joylink_packets.c \
		 					$(JOYLINK_SRC)/joylink/joylink_security.c \
		 					$(JOYLINK_SRC)/joylink/joylink_sub_dev.c \
		 					$(JOYLINK_SRC)/joylink/joylink_utils.c \
		 					$(JOYLINK_SRC)/json/cJSON.c \
		 					$(JOYLINK_SRC)/json/joylink_json.c \
		 					$(JOYLINK_SRC)/json/joylink_json_sub_dev.c \
			 				$(JOYLINK_SRC)/list/joylink_list.c \
			 				$(JOYLINK_SRC)/src/joylink_porting_layer.c\
			 				$(JOYLINK_SRC)/joylink_smnt/src/joylink_smnt.c

#C_FILES  += $(JOYLINK_SRC)/auth/joylinkAES.c \
#C_FILES  += $(JOYLINK_SRC)/auth/md5.c \
#C_FILES  += $(JOYLINK_SRC)/auth/test.c \
#C_FILES  += $(JOYLINK_SRC)/list/test.c \
#C_FILES  += $(JOYLINK_SRC)/json/test.c \
#C_FILES  += $(JOYLINK_SRC)/joylink/test.c \
#C_FILES  += $(JOYLINK_SRC)/src/auth/aes.c \
#C_FILES  += $(JOYLINK_SRC)/src/extern/joylink_extern.c \
#C_FILES  += $(JOYLINK_SRC)/src/extern/joylink_extern_sub_dev.c \

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/joylink_smnt/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/auth
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/example
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/extern
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/joylink
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/json
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/list
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/cjson/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/fota/inc/76x7

LIBS += $(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/joylink_smnt/lib/libjoylinksdk_innet.a

