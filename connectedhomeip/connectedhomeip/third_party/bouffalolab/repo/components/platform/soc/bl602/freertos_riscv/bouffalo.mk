
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS += config portable/GCC/RISC-V portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions

COMPONENT_OBJS := 

COMPONENT_SRCDIRS := 

OPT_FLAG_G := $(findstring -Og, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_G)),-Og)
CFLAGS := $(patsubst -Og,-O2,$(CFLAGS))
endif
OPT_FLAG_S := $(findstring -Os, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_S)),-Os)
CFLAGS := $(patsubst -Os,-O2,$(CFLAGS))
endif

ASMFLAGS += -DportasmHANDLE_INTERRUPT=interrupt_entry
