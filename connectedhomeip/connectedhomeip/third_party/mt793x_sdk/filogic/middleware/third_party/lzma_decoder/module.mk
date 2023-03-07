
LZMA_SRC = middleware/third_party/lzma_decoder

C_FILES  += $(LZMA_SRC)/src/lzma_decode_interface.c
C_FILES  += $(LZMA_SRC)/src/lzma_decoder.c

			 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/$(LZMA_SRC)/inc

