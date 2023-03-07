#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for integration tests
HOST_PROJ_ROOT ?= ../../..

TEST_AL_PATH = ../TestAL
TEST_AL_GIT =
TEST_AL_GIT_TAG = TAG_TESTAL_VER_3.02


include $(HOST_PROJ_ROOT)/Makefile.defs
$(info PROJ_PRD $(PROJ_PRD))
copy_integ_cfg := $(shell test -f $(HOST_PROJ_ROOT)/src/tests/integration_$(PROJ_PRD)/proj_integration_tests.cfg || cp $(HOST_PROJ_ROOT)/proj.cfg  $(HOST_PROJ_ROOT)/src/tests/integration_$(PROJ_PRD)/proj_integration_tests.cfg)
include $(HOST_PROJ_ROOT)/src/tests/integration_$(PROJ_PRD)/proj_integration_tests.cfg

ifeq ($(TEE_OS), freertos)
include  $(HOST_SRCDIR)/../Makefile.freertos
endif
export

INTEGRATION_TESTS = $(INTEG_TESTS)
RELEASE_INTEGRATION_TESTS = $(foreach module,$(INTEGRATION_TESTS),release_$(module))
CLEAN_INTEGRATION_TESTS = $(foreach module,$(INTEGRATION_TESTS),clean_$(module))
CLEAN_INTRMD_TARGETS = $(foreach module,$(INTEGRATION_TESTS),clean_intermediate_$(module))

RELEASE_INTEGRATION_SCRIPTS = $(foreach script,$(TESTER_INTEGRATION_SCRIPTS),release_$(script))
CLEAN_INTEGRATION_SCRIPTS = $(foreach script,$(TESTER_INTEGRATION_SCRIPTS),clean_$(script))

#|--------------------------TestAL_3.02---------------------------------|
#|ConfNum|    OS    |    CPU     |      Toolchain      | Device | Board |
#|----------------------------------------------------------------------|
#|   1   |  linux   | cortex-a9  |      arm-xilinx     | target | Zynq  |
#|   2   |          |            |     Deprecated      |        |       |
#|   3   |  linux   |    x86     |        native       |  host  |       |
#|   4   | freertos | cortex-m3  |    arm-compiler-5   | target | MPS2+ |
#|   5   |          |            |     Deprecated      |        |       |
#|   6   |  no_os   | cortex-m3  |    arm-compiler-5   | target | MPS2+ |
#|   7   | freertos | cortex-m33 |     arm-none-eabi   | target | MPS2+ |
#|   8   |  linux   | cortex-a9  |      arm-br-7.3     | target | Zynq  |
#|   9   | freertos | cortex-m3  |     arm-none-eabi   | target | MPS2+ |
#|   10  |  linux   |  a72 a53   |    aarch64-br-7.3   | target | Juno  |
#|   11  | freertos | cortex-m33 |    arm-compiler-6   | target | MPS2+ |
#|   12  | freertos | cortex-m3  |    arm-compiler-6   | target | MPS2+ |
#|----------------------------------------------------------------------|
# for sbrom use run_all_sbrom_integration_tests.sh otherwise use run_all_integration_tests.sh
TEST_AL_CONFIG_NUM =$(if $(findstring cortex-a9,${ARM_CPU} ), 1,)

ifeq ($(CROSS_COMPILE), armcc)
TEST_AL_CONFIG_NUM = 4
else ifeq ($(CROSS_COMPILE), armclang)
TEST_AL_CONFIG_NUM =$(if $(findstring cortex-m33,${ARM_CPU} ), 11 , 12)
else ifeq ($(CROSS_COMPILE), arm-none-eabi-)
TEST_AL_CONFIG_NUM =$(if $(findstring cortex-m33,${ARM_CPU} ), 7 , 9)
endif

default: build_test_al $(RELEASE_INTEGRATION_TESTS) $(RELEASE_INTEGRATION_SCRIPTS)
#generate_run_all_integration_tests

clean: $(CLEAN_INTEGRATION_TESTS) $(CLEAN_INTEGRATION_SCRIPTS)

clean_intermediate: $(CLEAN_INTRMD_TARGETS)


# Generic release rules recipes

build_test_al: clean_test_al
	if [ ! -d $(TEST_AL_PATH) ]; then git clone  $(TEST_AL_GIT) $(TEST_AL_PATH); cd $(TEST_AL_PATH) && git checkout $(TEST_AL_GIT_TAG) && ./TestAL-Lite/testal_lite.sh; fi
	echo [RUN] "./build_config.sh $(TEST_AL_CONFIG_NUM)"
	cd $(TEST_AL_PATH) && ./build_config.sh $(TEST_AL_CONFIG_NUM)
	mv $(TEST_AL_PATH)/libHAL* $(HOST_PROJ_ROOT)/lib/libtests_hal_lite.a
	mv $(TEST_AL_PATH)/libPAL* $(HOST_PROJ_ROOT)/lib/libtests_pal_lite.a
	@$(if $(subst freertos,,$(TEE_OS)),, $(ECHO) [CP] $(HOST_PROJ_ROOT)/lib/libtests*.a --\> $(KERNEL_DIR)/lib)
	@$(if $(subst freertos,,$(TEE_OS)),, $(CP) $(HOST_PROJ_ROOT)/lib/libtests*.a $(KERNEL_DIR)/lib)

$(RELEASE_INTEGRATION_TESTS): release_%:
	@$(ECHO) [REL] $*
	@$(MAKE) -s -C $*

$(CLEAN_INTEGRATION_TESTS): clean_%:
	@$(ECHO) [CLN] $*
	@$(MAKE) -s -C $* clean

$(CLEAN_INTRMD_TARGETS): clean_intermediate_%:
	@$(ECHO) [CLN-INT] $*
	@$(MAKE) -s -C $* clean_intermediate

clean_test_al:
	@$(ECHO) [CLN-TEST-AL]
	@$(RM) ../TestAL/*.a

.PHONY: default build_test_al clean_test_al clean clean_intermediate $(RELEASE_INTEGRATION_TESTS) $(CLEAN_INTEGRATION_TESTS) $(CLEAN_INTRMD_TARGETS) $(RELEASE_INTEGRATION_SCRIPTS) $(CLEAN_INTEGRATION_SCRIPTS) generate_run_all_integration_tests


