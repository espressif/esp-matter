 SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../)

include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CCS_LIB_DIRS = $(wildcard lib/ccs/*)
GCC_LIB_DIRS = $(wildcard lib/gcc/*)
IAR_LIB_DIRS = $(wildcard lib/iar/*)
TICLANG_LIB_DIRS = $(wildcard lib/ticlang/*)

LIB_DIRS =
ifneq ($(CCS_ARMCOMPILER),)
	LIB_DIRS += $(CCS_LIB_DIRS)
endif

ifneq ($(GCC_ARMCOMPILER),)
	LIB_DIRS += $(GCC_LIB_DIRS)
endif

ifneq ($(IAR_ARMCOMPILER),)
	LIB_DIRS += $(IAR_LIB_DIRS)
endif

ifneq ($(TICLANG_ARMCOMPILER),)
	LIB_DIRS += $(TICLANG_LIB_DIRS)
endif

.PHONY: build-libs $(LIB_DIRS)

# BUILD GOALS
all: build-libs

build-libs: $(LIB_DIRS)

$(LIB_DIRS):
	@ $(MAKE) -C $@


CLEAN_DIRS = $(addprefix clean-,$(LIB_DIRS))

.PHONY: clean-libs $(CLEAN_DIRS)

# CLEAN GOALS
clean: clean-libs

clean-libs: $(CLEAN_DIRS)

$(CLEAN_DIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean
