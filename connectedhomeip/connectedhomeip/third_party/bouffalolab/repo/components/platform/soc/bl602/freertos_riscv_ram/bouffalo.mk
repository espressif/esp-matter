
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS += config portable/GCC/RISC-V portable/GCC/RISC-V/chip_specific_extensions/RV32F_float_abi_single panic

COMPONENT_OBJS := $(patsubst %.c,%.o, \
                    event_groups.c \
                    list.c \
                    queue.c \
                    stream_buffer.c \
                    tasks.c \
                    timers.c \
                    misaligned/misaligned_ldst.c \
                    misaligned/fp_asm.S \
                    panic/panic_c.c \
                    portable/GCC/RISC-V/port.c \
                    portable/GCC/RISC-V/portASM.S \
                    portable/MemMang/heap_5.c)

COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := . portable portable/GCC/RISC-V portable/MemMang misaligned panic

OPT_FLAG_G := $(findstring -Og, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_G)),-Og)
CFLAGS := $(patsubst -Og,-O2,$(CFLAGS))
endif
OPT_FLAG_S := $(findstring -Os, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_S)),-Os)
CFLAGS := $(patsubst -Os,-O2,$(CFLAGS))
endif

ASMFLAGS += -DportasmHANDLE_INTERRUPT=interrupt_entry
