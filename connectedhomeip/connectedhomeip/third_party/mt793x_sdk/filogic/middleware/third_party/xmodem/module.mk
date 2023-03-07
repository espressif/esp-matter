
XMODEM_SRC = middleware/third_party/xmodem

CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/xmodem/inc

#################################################################################
#include path
ifeq ($(MTK_MINICLI_ENABLE),y)
ifeq ($(MTK_XMODEM_ENABLE),y)
C_FILES += $(XMODEM_SRC)/src/crc16.c
C_FILES += $(XMODEM_SRC)/src/xmodem.c
ifeq ($(MTK_CSI_PSRAM_ENABLE),y)
CFLAGS  += -DMTK_CSI_PSRAM_ENABLE
endif
endif
endif
