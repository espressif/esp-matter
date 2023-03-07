#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

TARGET_LIBS = cmpu

SOURCES_cmpu = cmpu.c
SOURCES_cmpu += cmpu_llf_rnd.c
SOURCES_cmpu += cmpu_derivation.c
SOURCES_cmpu += prod_crypto_driver.c
SOURCES_cmpu += prod_util.c
SOURCES_cmpu += cc_hal.c
SOURCES_cmpu += aes_driver.c
SOURCES_cmpu += driver_common.c
SOURCES_cmpu += aesccm_driver.c
SOURCES_cmpu += ccm_alt.c
SOURCES_cmpu += mbedtls_ccm_internal.c
SOURCES_cmpu += mbedtls_common.c
SOURCES_cmpu += mbedtls_cc_mng_int.c
SOURCES_cmpu += cc_common_conv_endian.c
SOURCES_cmpu += cc_rng_plat.c
SOURCES_cmpu += llf_rnd.c

#pal sources:
SOURCES_cmpu += cc_pal.c
SOURCES_cmpu += cc_pal_mutex.c
SOURCES_cmpu += cc_pal_memmap.c
SOURCES_cmpu += cc_pal_dma.c
SOURCES_cmpu += cc_pal_pm.c
SOURCES_cmpu += cc_pal_interrupt_ctrl.c
SOURCES_cmpu += cc_pal_buff_attr.c
SOURCES_cmpu += cc_pal_mem.c
SOURCES_cmpu += cc_pal_abort_plat.c
SOURCES_cmpu += cc_pal_trng.c

ifeq ($(DEBUG),1)
   SOURCES_cmpu += cc_pal_log.c
endif

ifeq ($(CC_CONFIG_TRNG_MODE),0)
        # FE TRNG
        $(info FE TRNG: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
        SOURCES_cmpu += llf_rnd_fetrng.c
	CFLAGS_EXTRA += -DCC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE)
else ifeq ($(CC_CONFIG_TRNG_MODE),1)
        # TRNG90B
        $(info TRNG90B: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
        SOURCES_cmpu += llf_rnd_trng90b.c
	CFLAGS_EXTRA += -DCC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE)
else
        $(error illegal TRNG: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
endif

ifeq ($(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES),)
    #default
    CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES = 512
endif

VPATH +=  ../common
VPATH +=  $(HOST_SRCDIR)/pal
VPATH +=  $(HOST_SRCDIR)/pal/$(OS)
VPATH +=  $(HOST_SRCDIR)/hal/cc3x
VPATH +=  $(HOST_SRCDIR)/cc3x_lib
VPATH += $(CODESAFE_SRCDIR)/crypto_api/rnd_dma
VPATH += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/driver
VPATH += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api
VPATH += $(CODESAFE_SRCDIR)/crypto_api/common
VPATH += $(CODESAFE_SRCDIR)/mbedtls_api
VPATH += $(HOST_SRCDIR)/cc_mng

PUBLIC_INCLUDES += $(SHARED_DIR)/include/crypto_api/cc_error.h
PUBLIC_INCLUDES += cc_cmpu.h ../common/cc_prod.h ../common/cc_prod_error.h
PUBLIC_INCLUDES += cmpu_derivation.h cmpu_llf_rnd.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/cc_mng/mbedtls_cc_mng.h

INCDIRS_EXTRA +=  $(HOST_SRCDIR)/pal/$(OS)
INCDIRS_EXTRA += $(SHARED_DIR)/$(CC_TEE_HW_INC_DIR)
INCDIRS_EXTRA +=  ../common
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/crypto_api
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/pal
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/pal/$(OS)
INCDIRS_EXTRA += $(SHARED_DIR)/include/mbedtls
INCDIRS_EXTRA += $(SHARED_DIR)/include/proj/$(PROJ)
INCDIRS_EXTRA +=  $(SHARED_DIR)/../mbedtls/include
INCDIRS_EXTRA +=  $(SHARED_DIR)/../mbedtls/include/mbedtls
INCDIRS_EXTRA += $(HOST_SRCDIR)/cc3x_lib
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal
INCDIRS_EXTRA += $(HOST_SRCDIR)/cc_mng
INCDIRS_EXTRA += $(SHARED_INCDIR)/cc_mng
INCDIRS_EXTRA += $(SHARED_INCDIR)/trng
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/rnd_dma/local/
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/rnd_dma
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/
INCDIRS_EXTRA +=  $(CODESAFE_SRCDIR)/crypto_api/common
INCDIRS_EXTRA += $(SHARED_INCDIR)/crypto_api/$(PROJ_PRD)
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/driver
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/mbedtls_api

CFLAGS +=  -DCC_TEE -DCC_IOT# used for register defines
CFLAGS_EXTRA += -DDLLI_MAX_BUFF_SIZE=$(DLLI_MAX_BUFF_SIZE)
CFLAGS_EXTRA += -DCC_MNG_MIN_BACKUP_SIZE_IN_BYTES=$(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES)


