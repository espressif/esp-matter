#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

HOST_PROJ_ROOT ?= ../..
include $(HOST_PROJ_ROOT)/Makefile.defs

CFLAGS_EXTRA += -DCC_TEE -DCC_SB_SUPPORT_IOT
OS =no_os
include project_sbromlib.mk

include $(HOST_PROJ_ROOT)/Makefile.rules

