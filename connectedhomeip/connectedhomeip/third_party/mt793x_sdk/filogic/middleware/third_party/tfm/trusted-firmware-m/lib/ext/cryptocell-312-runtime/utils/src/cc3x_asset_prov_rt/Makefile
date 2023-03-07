#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for managing build and installation

# shared library to build
UTIL_ROOT = $(PWD)
UTILS_DIR_ROOT = $(UTIL_ROOT)/../..
HOST_DIR_ROOT = $(UTIL_ROOT)/../host
UTILS_LIB_PATH = ./lib
UTILS_LIB_NAME = libutil_crypto.so
UTILS_SCRIPTS_DIRNAME = $(UTIL_ROOT)


DEPENDENCY_ON_EXISTENCE_OF = $(filter-out $(wildcard $(1)), $(1))

INSTALL_LIST = install_lib install_scripts

all:   $(INSTALL_LIST)



install_lib: build_lib $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTILS_DIR_ROOT)/lib)
	@echo Installing util_crypto library
	@cp $(UTILS_LIB_PATH)/$(UTILS_LIB_NAME) $(UTILS_DIR_ROOT)/lib

build_lib:
	@make -C $(UTILS_LIB_PATH)

install_scripts: $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTILS_DIR_ROOT)/bin) $(call DEPENDENCY_ON_EXISTENCE_OF,$(UTILS_DIR_ROOT)/bin/example)
	@echo Installing scripts
	@cp $(UTIL_ROOT)/*.py $(UTILS_DIR_ROOT)/bin/

$(UTILS_DIR_ROOT)/%:
	@echo Creating directory $@
	@mkdir $@


clean:
	@make -C $(UTILS_LIB_PATH) clean

.PHONY: install_lib install_sd_scripts clean

