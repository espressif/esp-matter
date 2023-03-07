# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_DestroyObject_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_DestroyObject

# Defined in `core_pkcs11_config.h`
MAX_OBJECT_NUM=2

# Defined in `core_pkcs11_config.h`
MAX_LABEL_SIZE=32

DEFINES += -DMAX_OBJECT_NUM=$(MAX_OBJECT_NUM)

INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList

# Same as max label size in the core_pkcs11_config.h
UNWINDSET += strncmp.0:$(MAX_LABEL_SIZE)
UNWINDSET += strlen.0:$(MAX_LABEL_SIZE)

UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvFindObjectInListByLabel.0:$(MAX_OBJECT_NUM)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvDeleteObjectFromList.0:$(MAX_OBJECT_NUM)


PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
