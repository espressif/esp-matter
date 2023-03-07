
CHACHA20POLY1305_SRC = middleware/third_party/chacha20poly1305

C_FILES  +=	$(CHACHA20POLY1305_SRC)/src/chacha-merged.c \
			$(CHACHA20POLY1305_SRC)/src/chacha.c \
			$(CHACHA20POLY1305_SRC)/src/e_chacha20poly1305.c \
			$(CHACHA20POLY1305_SRC)/src/evp_aead.c \
			$(CHACHA20POLY1305_SRC)/src/poly1305-donna.c \
			$(CHACHA20POLY1305_SRC)/src/poly1305.c \
			$(CHACHA20POLY1305_SRC)/src/test.c
		
			 

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/chacha20poly1305/src
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/chacha20poly1305/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -D calloc=pvPortCalloc
