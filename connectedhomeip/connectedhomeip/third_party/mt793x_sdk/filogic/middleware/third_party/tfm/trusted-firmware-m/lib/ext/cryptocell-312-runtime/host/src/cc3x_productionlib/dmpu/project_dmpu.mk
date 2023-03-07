#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

TARGET_LIBS = dmpu

SOURCES_dmpu = dmpu.c
SOURCES_dmpu += prod_crypto_driver.c
SOURCES_dmpu += prod_util.c
SOURCES_dmpu += cc_hal.c
SOURCES_dmpu += aes_driver.c
SOURCES_dmpu += aesccm_driver.c
SOURCES_dmpu += driver_common.c
SOURCES_dmpu += ccm_alt.c
SOURCES_dmpu += mbedtls_common.c
SOURCES_dmpu += mbedtls_ccm_internal.c
SOURCES_dmpu += mbedtls_cc_mng.c
SOURCES_dmpu += mbedtls_cc_mng_int.c
SOURCES_dmpu += cc_common_conv_endian.c

#pal sources
SOURCES_dmpu += cc_pal.c
SOURCES_dmpu += cc_pal_mutex.c
SOURCES_dmpu += cc_pal_memmap.c
SOURCES_dmpu += cc_pal_dma.c
SOURCES_dmpu += cc_pal_pm.c
SOURCES_dmpu += cc_pal_interrupt_ctrl.c
SOURCES_dmpu += cc_pal_buff_attr.c
SOURCES_dmpu += cc_pal_mem.c
SOURCES_dmpu += cc_pal_abort_plat.c

ifeq ($(DEBUG),1)
   SOURCES_dmpu += cc_pal_log.c
endif

ifeq ($(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES),)
    #default
    CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES = 512
endif

VPATH +=  ../common
VPATH +=  $(HOST_SRCDIR)/cc3x_productionlib/cmpu
VPATH +=  $(HOST_SRCDIR)/pal/$(OS)
VPATH +=  $(HOST_SRCDIR)/hal/cc3x
VPATH += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/driver
VPATH += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api
VPATH += $(CODESAFE_SRCDIR)/crypto_api/common
VPATH += $(CODESAFE_SRCDIR)/mbedtls_api
VPATH += $(HOST_SRCDIR)/cc_mng

PUBLIC_INCLUDES += $(SHARED_DIR)/include/crypto_api/cc_error.h
PUBLIC_INCLUDES += $(HOST_SRCDIR)/cc3x_productionlib/cmpu/cc_cmpu.h
PUBLIC_INCLUDES += cc_dmpu.h ../common/cc_prod.h ../common/cc_prod_error.h

INCDIRS_EXTRA += $(SHARED_DIR)/$(CC_TEE_HW_INC_DIR)
INCDIRS_EXTRA +=  ../common
INCDIRS_EXTRA +=  ../cmpu
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/pal
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/crypto_api
INCDIRS_EXTRA += $(HOST_SRCDIR)/pal/$(OS)
INCDIRS_EXTRA +=  $(SHARED_DIR)/include/pal/$(OS)
INCDIRS_EXTRA += $(SHARED_DIR)/include/mbedtls
INCDIRS_EXTRA += $(SHARED_DIR)/include/proj/$(PROJ)
INCDIRS_EXTRA +=  $(SHARED_DIR)/../mbedtls/include
INCDIRS_EXTRA +=  $(SHARED_DIR)/../mbedtls/include/mbedtls
INCDIRS_EXTRA += $(HOST_SRCDIR)/cc3x_lib
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal
INCDIRS_EXTRA += $(HOST_SRCDIR)/cc_mng
INCDIRS_EXTRA += $(SHARED_INCDIR)/cc_mng
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/
INCDIRS_EXTRA +=  $(CODESAFE_SRCDIR)/crypto_api/common
INCDIRS_EXTRA += $(SHARED_INCDIR)/crypto_api/$(PROJ_PRD)
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/driver
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/mbedtls_api

CFLAGS +=  -DCC_TEE -DCC_IOT# used for register defines
CFLAGS_EXTRA += -DDLLI_MAX_BUFF_SIZE=$(DLLI_MAX_BUFF_SIZE)
CFLAGS_EXTRA += -DCC_MNG_MIN_BACKUP_SIZE_IN_BYTES=$(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES)


