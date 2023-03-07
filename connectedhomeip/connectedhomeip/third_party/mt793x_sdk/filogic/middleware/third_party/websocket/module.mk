
WS_SRC = middleware/third_party/websocket

C_FILES  += $(WS_SRC)/src/librws.c
C_FILES  += $(WS_SRC)/src/rws_common.c
C_FILES  += $(WS_SRC)/src/rws_error.c
C_FILES  += $(WS_SRC)/src/rws_frame.c
C_FILES  += $(WS_SRC)/src/rws_list.c
C_FILES  += $(WS_SRC)/src/rws_memory.c
C_FILES  += $(WS_SRC)/src/rws_socketpriv.c
C_FILES  += $(WS_SRC)/src/rws_socketpub.c
C_FILES  += $(WS_SRC)/src/rws_string.c
C_FILES  += $(WS_SRC)/src/rws_thread.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/websocket
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/websocket/src
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/posix
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/compat/posix
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7933/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
