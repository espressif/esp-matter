#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

CFLAGS_EXTRA += -DCC_IOT

SOURCES_pal_$(PLAT_OS) += cc_pal.c
SOURCES_pal_$(PLAT_OS) += cc_pal_mutex.c
SOURCES_pal_$(PLAT_OS) += cc_pal_memmap.c
SOURCES_pal_$(PLAT_OS) += cc_pal_dma.c
SOURCES_pal_$(PLAT_OS) += cc_pal_pm.c
SOURCES_pal_$(PLAT_OS) += cc_pal_interrupt_ctrl.c
SOURCES_pal_$(PLAT_OS) += cc_pal_mem.c
SOURCES_pal_$(PLAT_OS) += cc_pal_buff_attr.c
SOURCES_pal_$(PLAT_OS) += cc_pal_abort_plat.c
SOURCES_pal_$(PLAT_OS) += cc_pal_trng.c
SOURCES_pal_$(PLAT_OS) += cc_pal_apbc.c

PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/cc_pal_types.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/cc_pal_log.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/cc_log_mask.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/$(PLAT_OS)/cc_pal_log_plat.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/$(PLAT_OS)/cc_pal_types_plat.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/pal/cc_pal_buff_attr.h

INCDIRS_EXTRA += $(SHARED_INCDIR)/pal
INCDIRS_EXTRA += $(SHARED_INCDIR)/pal/$(PLAT_OS)

INCDIRS_EXTRA +=  $(SHARED_DIR)/hw/include # for dx_reg_base_host.h, included in cc_pal.c
INCDIRS_EXTRA += $(HOST_SRCDIR)/hal # for cc_pal_interrupt_ctrl.c

ifeq ($(ARCH),arm)
ifeq ($(CROSS_COMPILE),arm-none-eabi-)
ifeq ($(ARM_CPU), cortex-m33)
CFLAGS += -DSSE_200
endif
endif
endif

ifeq ($(DEBUG),1)
   SOURCES_pal_$(PLAT_OS) += cc_pal_log.c
   CFLAGS += -DDEBUG
endif

$(info TRNG: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
CFLAGS_EXTRA += -DCC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE)

ifeq ($(LIB_PERF),1)
SOURCES_pal_$(PLAT_OS) += cc_pal_perf_plat.c
CFLAGS += -DLIB_PERF
endif

VPATH += $(HOST_SRCDIR)/pal $(HOST_SRCDIR)/pal/$(PLAT_OS)

