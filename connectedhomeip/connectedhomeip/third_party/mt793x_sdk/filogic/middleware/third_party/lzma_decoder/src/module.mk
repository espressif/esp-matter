
LZMA_SRC = middleware/third_party/lzma_decoder

C_FILES  += $(LZMA_SRC)/src_protected/hal_lzma_decode_interface.c \
			$(LZMA_SRC)/src_protected/lzma_decoder.c

			 

#################################################################################
#include path
CFLAGS 	+= -I./inc
CFLAGS  += -I../../../driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I../../../driver/board/mt25x3_hdk/bootloader/core/inc
