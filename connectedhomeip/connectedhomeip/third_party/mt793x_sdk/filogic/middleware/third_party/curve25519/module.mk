
CURVE25519_SRC = middleware/third_party/curve25519

C_FILES  +=	$(CURVE25519_SRC)/src/curve25519-donna-test.c \
			$(CURVE25519_SRC)/src/curve25519-donna.c
		
			 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/third_party/curve25519/src
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/third_party/curve25519/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
