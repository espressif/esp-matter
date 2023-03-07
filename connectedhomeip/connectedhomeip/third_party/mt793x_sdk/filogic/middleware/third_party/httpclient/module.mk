
HTTPCLIENT_SRC = middleware/third_party/httpclient

C_FILES  += $(HTTPCLIENT_SRC)/src/httpclient.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/httpclient/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
