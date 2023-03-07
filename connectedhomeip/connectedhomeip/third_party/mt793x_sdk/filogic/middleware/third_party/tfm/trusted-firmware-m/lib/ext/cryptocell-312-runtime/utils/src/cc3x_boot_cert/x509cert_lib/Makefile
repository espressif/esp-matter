#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Makefile for building sbu_crypto library
SH_LIB_NAME = libsbu_crypto.so

LIB_SRC_O = main.o common_rsa_keypair.o  common_crypto_x509.o
LIB_SRC_O += common_crypto_sym.o common_crypto_asym.o common_util_files.o common_rsa_keypair_util.o

UTILS_ROOT = $(PWD)/../../..
SHARED_DIR = $(UTILS_ROOT)/../shared
HOST_DIR = $(UTILS_ROOT)/../host
CODESAFE_DIR = $(UTILS_ROOT)/../codesafe
UTILS_LIB_PATH = $(UTILS_ROOT)/lib
UTILS_COMMON_PATH = $(UTILS_ROOT)/src/common
UTILS_INC_PATH = $(UTILS_ROOT)/include $(UTILS_COMMON_PATH) $(SHARED_DIR)/include $(SHARED_DIR)/include/sbrom  $(SHARED_DIR)/include/proj/$(PROJ_PRD)
UTILS_INC_PATH += $(HOST_DIR)/src/cc3x_sbromlib $(SHARED_DIR)/include/pal/$(TEE_OS) $(CODESAFE_DIR)/src/secure_boot_debug/cc3x_verifier $(SHARED_DIR)/include/pal
UTILS_INC_PATH += $(CODESAFE_DIR)/src/secure_boot_debug/secure_boot_gen $(SHARED_DIR)/hw/include $(CODESAFE_DIR)/src/secure_boot_debug/crypto_driver $(CODESAFE_DIR)/src/secure_boot_debug/platform/pal/cc3x
UTILS_INC_PATH += $(CODESAFE_DIR)/src/secure_boot_debug/platform/common/cc3x $(CODESAFE_DIR)/src/secure_boot_debug/platform/stage/boot/cc3x $(CODESAFE_DIR)/src/secure_boot_debug/util
UTILS_INC_PATH += $(CODESAFE_DIR)/src/secure_boot_debug/secure_debug/cc3x $(CODESAFE_DIR)/src/secure_boot_debug/platform/hal $(CODESAFE_DIR)/src/secure_boot_debug/crypto_driver/reg
UTILS_INC_PATH += $(SHARED_DIR)/include/pal  $(SHARED_DIR)/include/pal/$(TEE_OS)

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

