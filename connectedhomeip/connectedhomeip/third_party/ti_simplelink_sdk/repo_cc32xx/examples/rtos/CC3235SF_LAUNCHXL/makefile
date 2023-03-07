SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../..)
include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

# Find all subdirectories
TIRTOS7-CCS-SUBDIRS = $(wildcard */*/tirtos7/ccs/.)
TIRTOS7-GCC-SUBDIRS = $(wildcard */*/tirtos7/gcc/.)
TIRTOS7-IAR-SUBDIRS = $(wildcard */*/tirtos7/iar/.)
TIRTOS7-TICLANG-SUBDIRS = $(wildcard */*/tirtos7/ticlang/.)
TIRTOS-CCS-SUBDIRS = $(wildcard */*/tirtos/ccs/.)
TIRTOS-GCC-SUBDIRS = $(wildcard */*/tirtos/gcc/.)
TIRTOS-IAR-SUBDIRS = $(wildcard */*/tirtos/iar/.)
TIRTOS-TICLANG-SUBDIRS = $(wildcard */*/tirtos/ticlang/.)
FREERTOS-CCS-SUBDIRS = $(wildcard */*/freertos/ccs/.)
FREERTOS-GCC-SUBDIRS = $(wildcard */*/freertos/gcc/.)
FREERTOS-IAR-SUBDIRS = $(wildcard */*/freertos/iar/.)
FREERTOS-TICLANG-SUBDIRS = $(wildcard */*/freertos/ticlang/.)

TIRTOS7-SUBDIRS =
TIRTOS-SUBDIRS =
FREERTOS-SUBDIRS =
ifneq ($(CCS_ARMCOMPILER),)
	TIRTOS7-SUBDIRS += $(TIRTOS7-CCS-SUBDIRS)
	TIRTOS-SUBDIRS += $(TIRTOS-CCS-SUBDIRS)
	FREERTOS-SUBDIRS += $(FREERTOS-CCS-SUBDIRS)
endif
ifneq ($(GCC_ARMCOMPILER),)
	TIRTOS7-SUBDIRS += $(TIRTOS7-GCC-SUBDIRS)
	TIRTOS-SUBDIRS += $(TIRTOS-GCC-SUBDIRS)
	FREERTOS-SUBDIRS += $(FREERTOS-GCC-SUBDIRS)
endif
ifneq ($(IAR_ARMCOMPILER),)
	TIRTOS7-SUBDIRS += $(TIRTOS7-IAR-SUBDIRS)
	TIRTOS-SUBDIRS += $(TIRTOS-IAR-SUBDIRS)
	FREERTOS-SUBDIRS += $(FREERTOS-IAR-SUBDIRS)
endif
ifneq ($(TICLANG_ARMCOMPILER),)
	TIRTOS7-SUBDIRS += $(TIRTOS7-TICLANG-SUBDIRS)
	TIRTOS-SUBDIRS += $(TIRTOS-TICLANG-SUBDIRS)
	FREERTOS-SUBDIRS += $(FREERTOS-TICLANG-SUBDIRS)
endif

.PHONY: tirtos7 tirtos freertos $(TIRTOS7-SUBDIRS) $(TIRTOS-SUBDIRS) $(FREERTOS-SUBDIRS)

all: tirtos7 tirtos freertos

tirtos7: $(TIRTOS7-SUBDIRS)

tirtos: $(TIRTOS-SUBDIRS)

freertos: $(FREERTOS-SUBDIRS)

$(TIRTOS7-SUBDIRS):
	@ $(MAKE) -C $@

$(TIRTOS-SUBDIRS):
	@ $(MAKE) -C $@

$(FREERTOS-SUBDIRS):
	@ $(MAKE) -C $@

CLEAN-TIRTOS7-SUBDIRS = $(addprefix clean-,$(TIRTOS7-SUBDIRS))
CLEAN-TIRTOS-SUBDIRS = $(addprefix clean-,$(TIRTOS-SUBDIRS))
CLEAN-FREERTOS-SUBDIRS = $(addprefix clean-,$(FREERTOS-SUBDIRS))

.PHONY: clean-tirtos7 clean-tirtos clean-freertos $(CLEAN-TIRTOS7-SUBDIRS) $(CLEAN-TIRTOS-SUBDIRS) $(CLEAN-FREERTOS-SUBDIRS)

clean: clean-tirtos7 clean-tirtos clean-freertos

clean-tirtos7: $(CLEAN-TIRTOS7-SUBDIRS)
clean-tirtos: $(CLEAN-TIRTOS-SUBDIRS)
clean-freertos: $(CLEAN-FREERTOS-SUBDIRS)

$(CLEAN-TIRTOS7-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean

$(CLEAN-TIRTOS-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean

$(CLEAN-FREERTOS-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean
