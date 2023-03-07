#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

HOST_PROJ_ROOT ?= ../../../..

ifneq (,$(findstring gnu,$(CROSS_COMPILE)))
TEE_OS = linux
else
TEE_OS = freertos
endif

ifneq ($(wildcard $(HOST_PROJ_ROOT)/../configs/proj-cc312-sbrom_integration_tests.cfg),)
$(info Compiling for Internal ARM use)
ARM_INTERNAL=1
else
ARM_INTERNAL=0
endif

include $(HOST_PROJ_ROOT)/Makefile.defs

include cmpu_integration_test.mk

include $(HOST_PROJ_ROOT)/Makefile.rules