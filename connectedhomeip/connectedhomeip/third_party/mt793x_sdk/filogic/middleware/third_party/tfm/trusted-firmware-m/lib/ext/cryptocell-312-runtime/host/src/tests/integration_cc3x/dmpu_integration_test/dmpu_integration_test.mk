#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include ../proj_integration_tests.cfg
include $(HOST_PROJ_ROOT)/Makefile.test_suite
include $(HOST_PROJ_ROOT)/../proj.ext.cfg

ifneq ($(TEE_OS),linux)
TARGET_LIBS = dmpu_integration_test
else
TARGET_EXES = dmpu_integration_test
DEPLIBS = dmpu
DEPLIBS += $(TEST_AL_LITE_LIBS)

endif

# Unit test dependencies
SOURCES_dmpu_integration_test = dmpu_integration_test.c
SOURCES_dmpu_integration_test += dmpu_integration_test_arm.c
SOURCES_dmpu_integration_test += $(filter-out test_proj_cclib.c,$(PROJ_SOURCES))

ifeq ($(TEST_DEBUG),1)
CFLAGS_EXTRA += -DTEST_DEBUG=1# use to toggle debug
endif

CFLAGS_EXTRA+=-DCC_TEE

TEST_INCLUDES += $(CURDIR)/dmpu_integration_test.h
INCDIRS_EXTRA += $(CURDIR)/pal/include

INCDIRS_EXTRA += $(SHARED_INCDIR)/pal
INCDIRS_EXTRA += $(SHARED_INCDIR)/pal/$(TEE_OS)
INCDIRS_EXTRA += $(SHARED_INCDIR)/pal/$(TEE_OS)/include
INCDIRS_EXTRA += $(SHARED_DIR)/include/proj/cc3x
INCDIRS_EXTRA += $(PROJ_INCLUDE)
INCDIRS_EXTRA += $(TEST_AL_INCLUDE)

INCDIRS_EXTRA += $(HOST_PROJ_ROOT)/include
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal/$(TEST_PRODUCT)
INCDIRS_EXTRA += $(HOST_SRCDIR)/cc3x_productionlib/dmpu

INCDIRS_EXTRA += $(SHARED_DIR)/hw/include

ifeq ($(TEE_OS), freertos)
INCDIRS_EXTRA += $(KERNEL_DIR)/OS/FreeRTOS/Source/include
INCDIRS_EXTRA += $(KERNEL_DIR)/OS/FreeRTOS-Plus-CLI
INCDIRS_EXTRA += $(KERNEL_DIR)/board/MPS2+
INCDIRS_EXTRA += $(KERNEL_DIR)/lib/main
INCDIRS_EXTRA += $(KERNEL_DIR)/OS/FreeRTOS/Source/portable/RVDS/ARM_CM3
endif

VPATH += $(CURDIR)/pal/$(TEE_OS)
VPATH += $(CURDIR)/hal/$(TEST_PRODUCT)
VPATH += $(PROJ_VPATH)