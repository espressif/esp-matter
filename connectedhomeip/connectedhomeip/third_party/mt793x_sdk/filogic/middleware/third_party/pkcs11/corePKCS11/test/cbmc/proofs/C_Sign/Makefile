# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_Sign_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_Sign

# Defined in `core_pkcs11_config.h`
# Should be one more than the total number of objects in the PKCS stack.
MAX_OBJECT_NUM=2

DEFINES += -DMAX_OBJECT_NUM=$(MAX_OBJECT_NUM)
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Initialize
REMOVE_FUNCTION_BODY += C_OpenSession
REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList
REMOVE_FUNCTION_BODY += PKCS11_PAL_Initialize
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_init
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_seed
REMOVE_FUNCTION_BODY += mbedtls_entropy_free
REMOVE_FUNCTION_BODY += mbedtls_entropy_init
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_free
REMOVE_FUNCTION_BODY += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvMbedTLS_Initialize
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pki_utils_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
