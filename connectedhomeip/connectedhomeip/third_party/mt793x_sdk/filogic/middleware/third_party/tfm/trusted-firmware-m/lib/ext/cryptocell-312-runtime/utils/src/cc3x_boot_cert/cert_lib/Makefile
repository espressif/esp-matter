#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for building sbu_crypto library
SH_LIB_NAME = libsbu_crypto.so

LIB_SRC_O = main.o common_rsa_keypair.o common_rsa_keypair_util.o common_crypto_sym.o common_util_files.o common_sb_ops.o

UTILS_ROOT = $(PWD)/../../..
SHARED_DIR = $(UTILS_ROOT)/../shared
UTILS_LIB_PATH = $(UTILS_ROOT)/lib
UTILS_INC_PATH = $(UTILS_ROOT)/include $(UTILS_ROOT)/src/common  $(SHARED_DIR)/include/proj/$(PROJ_PRD) $(SHARED_DIR)/include
UTILS_INC_PATH += $(CODESAFE_SRCDIR)/secure_boot_debug/secure_debug/cc3x  $(SHARED_DIR)/include/pal  $(SHARED_DIR)/include/pal/$(TEE_OS)
CFLAGS += -fPIC $(foreach incdir,$(UTILS_INC_PATH),-I$(incdir)) -c

CFLAGS += -DCC_SB_SUPPORT_IOT

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

