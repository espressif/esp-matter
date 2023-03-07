#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for managing SBU build and installation
ifeq ($(CC_CONFIG_SB_X509_CERT_SUPPORTED),1)
	SUB_DIRS_PREFIX = x509cert
else
	SUB_DIRS_PREFIX = cert
endif

$(info $$SUB_DIRS_PREFIX is [${SUB_DIRS_PREFIX}])

# shared library to build
SBU_ROOT = $(PWD)
UTIL_ROOT = $(SBU_ROOT)/../..
SBU_CRYP_LIB_PATH = $(SUB_DIRS_PREFIX)_lib
SBU_CRYP_LIB_NAME = libsbu_crypto.so
SBU_SCRIPTS_DIRNAME = $(SUB_DIRS_PREFIX)_utils
SBU_COMMON_SCRIPTS_DIRNAME = common_utils

DEPENDENCY_ON_EXISTENCE_OF = $(filter-out $(wildcard $(1)), $(1))

INSTALL_LIST = install_lib install_sbu

all:   $(INSTALL_LIST)

install_lib: build_lib $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTIL_ROOT)/lib)
	@echo Installing sbu_crypto library
	@cp $(SBU_ROOT)/$(SBU_CRYP_LIB_PATH)/$(SBU_CRYP_LIB_NAME) $(UTIL_ROOT)/lib

build_lib:
	@make -C $(SBU_ROOT)/$(SBU_CRYP_LIB_PATH)


install_sbu: $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTIL_ROOT)/bin) $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTIL_ROOT)/bin/$(SBU_COMMON_SCRIPTS_DIRNAME))
	@echo Installing SBU scripts
	@cp $(SBU_ROOT)/$(SBU_SCRIPTS_DIRNAME)/*.py $(UTIL_ROOT)/bin
	@cp $(SBU_ROOT)/$(SBU_COMMON_SCRIPTS_DIRNAME)/*.py $(UTIL_ROOT)/bin/$(SBU_COMMON_SCRIPTS_DIRNAME)

$(UTIL_ROOT)/%:
	@echo Creating directory $@
	@mkdir $@




clean:
	@make -C $(SBU_CRYP_LIB_PATH) clean

.PHONY: install_lib install_sbu clean


