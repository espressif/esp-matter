
SRP_SRC = middleware/third_party/srp

C_FILES  +=	 $(SRP_SRC)/src/cstr.c \
             $(SRP_SRC)/src/srp.c \
             $(SRP_SRC)/src/srp6_server.c \
             $(SRP_SRC)/src/t_conv.c \
             $(SRP_SRC)/src/t_math.c \
             $(SRP_SRC)/src/t_misc.c \
             $(SRP_SRC)/src/t_sha.c
		
			 

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/srp/src
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/srp/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -D calloc=pvPortCalloc -fomit-frame-pointer -DHAVE_CONFIG_H
