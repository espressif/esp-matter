# KERNEL_BUILD below selects a specific kernel to build. By default,
# '*' is selected so all kernels are built.
#
# KERNEL_BUILD options supported match directory names contained within the
# builds/ (choose only one for KERNEL_BUILD):
#
KERNEL_BUILD ?= *

# Find subdirectories
RELEASE-CCS-SUBDIRS   = $(wildcard builds/$(KERNEL_BUILD)/release/ccs/.)
RELEASE-GCC-SUBDIRS   = $(wildcard builds/$(KERNEL_BUILD)/release/gcc/.)
RELEASE-IAR-SUBDIRS   = $(wildcard builds/$(KERNEL_BUILD)/release/iar/.)
RELEASE-TICLANG-SUBDIRS   = $(wildcard builds/$(KERNEL_BUILD)/release/ticlang/.)
DEBUG-CCS-SUBDIRS     = $(wildcard builds/$(KERNEL_BUILD)/debug/ccs/.)
DEBUG-GCC-SUBDIRS     = $(wildcard builds/$(KERNEL_BUILD)/debug/gcc/.)
DEBUG-IAR-SUBDIRS     = $(wildcard builds/$(KERNEL_BUILD)/debug/iar/.)
DEBUG-TICLANG-SUBDIRS     = $(wildcard builds/$(KERNEL_BUILD)/debug/ticlang/.)

include ../../imports.mak

RELEASE-SUBDIRS =
DEBUG-SUBDIRS =
ifneq ($(CCS_ARMCOMPILER),)
	RELEASE-SUBDIRS += $(RELEASE-CCS-SUBDIRS)
	DEBUG-SUBDIRS += $(DEBUG-CCS-SUBDIRS)
endif
ifneq ($(GCC_ARMCOMPILER),)
	RELEASE-SUBDIRS += $(RELEASE-GCC-SUBDIRS)
	DEBUG-SUBDIRS += $(DEBUG-GCC-SUBDIRS)
endif
ifneq ($(IAR_ARMCOMPILER),)
	RELEASE-SUBDIRS += $(RELEASE-IAR-SUBDIRS)
	DEBUG-SUBDIRS += $(DEBUG-IAR-SUBDIRS)
endif
ifneq ($(TICLANG_ARMCOMPILER),)
	RELEASE-SUBDIRS += $(RELEASE-TICLANG-SUBDIRS)
	DEBUG-SUBDIRS += $(DEBUG-TICLANG-SUBDIRS)
endif

.PHONY: release debug $(RELEASE-SUBDIRS) $(DEBUG-SUBDIRS)

# rules for building
all: release debug

release: $(RELEASE-SUBDIRS)
debug: $(DEBUG-SUBDIRS)

$(RELEASE-SUBDIRS):
	@ $(MAKE) -C $@

$(DEBUG-SUBDIRS):
	@ $(MAKE) -C $@

CLEAN-DEBUG-SUBDIRS = $(addprefix clean-,$(DEBUG-SUBDIRS))
CLEAN-RELEASE-SUBDIRS = $(addprefix clean-,$(RELEASE-SUBDIRS))

.PHONY: clean-release clean-debug $(CLEAN-RELEASE-SUBDIRS) $(CLEAN-DEBUG-SUBDIRS)

# rules for cleaning
clean: clean-release clean-debug

clean-debug: $(CLEAN-DEBUG-SUBDIRS)
clean-release: $(CLEAN-RELEASE-SUBDIRS)

$(CLEAN-DEBUG-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean

$(CLEAN-RELEASE-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean
