#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for building sbu_crypto library
SH_LIB_NAME = lib_cmpuutil_crypto.so

LIB_SRC_O = main.o common_crypto_asym.o  common_crypto_sym.o common_rsa_keypair_util.o common_rsa_keypair.o common_util_files.o

UTILS_ROOT = $(PWD)/../../..
SHARED_DIR = $(UTILS_ROOT)/../shared
HOSTSRC_DIR = $(UTILS_ROOT)/../host/src
UTILS_LIB_PATH = $(UTILS_ROOT)/lib
UTILS_INC_PATH = $(SHARED_DIR)/include $(SHARED_DIR)/include/proj/$(PROJ_PRD) $(UTILS_ROOT)/include
UTILS_INC_PATH += $(UTILS_ROOT)/src/common $(HOSTSRC_DIR)/cc3x_productionlib/common $(SHARED_DIR)/include/pal  $(SHARED_DIR)/include/pal/$(TEE_OS)

CFLAGS += -fPIC $(foreach incdir,$(UTILS_INC_PATH),-I$(incdir)) -c

all: $(SH_LIB_NAME)

# Compile and link the sbu_crypto library with hard-coded library run path to utils/lib
# (to assure the private version of openssl libraries are used)
$(SH_LIB_NAME): $(LIB_SRC_O)
	gcc -shared -o $(SH_LIB_NAME) $(LIB_SRC_O) -Wl,-rpath,$(UTILS_LIB_PATH) -lcrypto -lssl

vpath %.c $(UTILS_ROOT)/src/common

%.o: %.c
	gcc $(CFLAGS) $<

clean:
	rm -f $(SH_LIB_NAME) $(LIB_SRC_O)

.PHONY: clean all

