
MICRO_ECC_SRC = middleware/third_party/micro_ecc

C_FILES  += $(MICRO_ECC_SRC)/uECC.c

#################################################################################
#include path
CFLAGS     += -I$(SOURCE_DIR)/middleware/third_party/micro_ecc/
