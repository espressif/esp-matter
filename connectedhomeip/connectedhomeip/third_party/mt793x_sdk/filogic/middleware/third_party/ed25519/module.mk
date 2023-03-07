
ED25519_SRC = middleware/third_party/ed25519

C_FILES  +=	$(ED25519_SRC)/src/tweetnacl.c
		

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/ed25519/src
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/ed25519/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
