#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include ../proj_integration_tests.cfg
include $(HOST_PROJ_ROOT)/Makefile.test_suite
include $(HOST_PROJ_ROOT)/../proj.ext.cfg

MBEDTLS_ROOT = $(HOST_SRCDIR)/../../mbedtls

ifneq ($(TEE_OS),linux)
TARGET_LIBS = run_integration_test
else
TARGET_EXES = run_integration_test
LIBDIRS += $(MBEDTLS_ROOT)/library
DEPLIBS = mbedcrypto
DEPLIBS += cc_312 pal_$(TEE_OS)
DEPLIBS += $(TEST_AL_LITE_LIBS)
endif

# Unit test dependencies
SOURCES_run_integration_test = run_integration_test.c
SOURCES_run_integration_test += run_integration_helper.c
SOURCES_run_integration_test += run_integration_flash.c
SOURCES_run_integration_test += run_integration_otp.c
SOURCES_run_integration_test += run_integration_aes.c
SOURCES_run_integration_test += run_integration_asset_prov.c
SOURCES_run_integration_test += run_integration_sha.c
SOURCES_run_integration_test += run_integration_ccm.c
SOURCES_run_integration_test += run_integration_gcm.c
SOURCES_run_integration_test += run_integration_ecdsa.c
SOURCES_run_integration_test += run_integration_ecies.c
SOURCES_run_integration_test += run_integration_ecdh.c
SOURCES_run_integration_test += run_integration_rsa.c
SOURCES_run_integration_test += run_integration_drbg.c
SOURCES_run_integration_test += run_integration_key_derivation.c
SOURCES_run_integration_test += run_integration_chacha.c
SOURCES_run_integration_test += run_integration_srp.c
SOURCES_run_integration_test += run_integration_mac.c
SOURCES_run_integration_test += run_integration_dhm.c
SOURCES_run_integration_test += run_integration_ext_dma.c
SOURCES_run_integration_test += run_integration_test_arm.c
SOURCES_run_integration_test += run_integration_secure_boot.c
SOURCES_run_integration_test += $(PROJ_SOURCES)

ifeq ($(TEE_OS),freertos)
ifeq ($(ARM_INTERNAL),1)
SOURCES_run_integration_test += run_integration_profiler_arm.c
else
SOURCES_run_integration_test += run_integration_profiler.c
endif
else # linux
SOURCES_run_integration_test += run_integration_profiler.c
endif

TEST_INCLUDES += $(CURDIR)/run_integration_test.h

INCDIRS_EXTRA += $(SHARED_INCDIR)/pal
INCDIRS_EXTRA += $(SHARED_INCDIR)/pal/$(TEE_OS)
INCDIRS_EXTRA += $(SHARED_INCDIR)/pal/$(TEE_OS)/include

INCDIRS_EXTRA += $(HOST_PROJ_ROOT)/include
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal/$(TEST_PRODUCT)

INCDIRS_EXTRA += $(CURDIR)/tests
INCDIRS_EXTRA += $(CURDIR)/pal/include
INCDIRS_EXTRA += $(CURDIR)/pal/$(TEE_OS)

INCDIRS_EXTRA += $(PROJ_INCLUDE)
INCDIRS_EXTRA += $(TEST_AL_INCLUDE)

INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/common #due to usage of inverse functions in test
INCDIRS_EXTRA += $(SHARED_DIR)/hw/include
INCDIRS_EXTRA += $(HOST_PROJ_ROOT)/../mbedtls/include


ifeq ($(TEE_OS), freertos)
include  $(HOST_SRCDIR)/../Makefile.freertos
endif

ifeq ($(LIB_PERF),1)
CFLAGS += -DLIB_PERF
endif

ifeq ($(PIE),1)
CFLAGS += -DRUNIT_PIE_ENABLED
endif

ifeq ($(ARM_CPU),cortex-m33)
# needed to identify in code which FPGA we are working with.
# although the cortex should not matter the memory layout should.
# for example the unmanaged memory address is different -> the certificates are different
CFLAGS += -DCORTEX_M33_FPGA
endif

ifeq ($(TEST_DEBUG),1)
CFLAGS_EXTRA += -DTEST_DEBUG=1# use to toggle debug
endif

ifeq ($(CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES),)
CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES = 8192
endif

ifeq ($(CC_CONFIG_SB_X509_CERT_SUPPORTED),1)
CFLAGS_EXTRA += -DCC_SB_X509_CERT_SUPPORTED
endif

ifeq ($(CC_CONFIG_CC_CHACHA_POLY_SUPPORT),1)
CFLAGS_EXTRA += -DCC_CONFIG_CC_CHACHA_POLY_SUPPORT
endif

ifeq ($(CC_CONFIG_SUPPORT_SRP),1)
CFLAGS_EXTRA += -DCC_CONFIG_SUPPORT_SRP
endif


ifneq ($(RUNIT_TEST_ITER_MAX),)
ifeq ($(TEE_OS),freertos)
ifeq ($(ARM_INTERNAL),1)
CFLAGS_EXTRA+=-DRUNIT_TEST_ITER_MAX=$(RUNIT_TEST_ITER_MAX)
endif
endif
endif

ifeq ($(CC_CONFIG_SUPPORT_EXT_DMA),1)
CFLAGS_EXTRA += -DCC_CONFIG_SUPPORT_EXT_DMA=1
endif

CFLAGS_EXTRA += -DCC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES=$(CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES)
CFLAGS_EXTRA += -DCC_TEE

VPATH += $(CURDIR)/pal/$(TEE_OS)
VPATH += $(CURDIR)/tests
VPATH += $(CURDIR)/hal/$(TEST_PRODUCT)
VPATH += $(PROJ_VPATH)


