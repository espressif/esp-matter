# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_GenerateKeyPair_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_GenerateKeyPair

TEMPLATE_SIZE=10

DEFINES += -DTEMPLATE_SIZE=$(TEMPLATE_SIZE)

INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Initialize
REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList
REMOVE_FUNCTION_BODY += PKCS11_PAL_DestroyObject
REMOVE_FUNCTION_BODY += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvMbedTLS_Initialize
REMOVE_FUNCTION_BODY += PKCS11_PAL_Initialize
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_init
REMOVE_FUNCTION_BODY +=	mbedtls_ctr_drbg_seed
REMOVE_FUNCTION_BODY +=	mbedtls_entropy_init

UNWINDSET += harness.0:10
UNWINDSET += harness.1:10
UNWINDSET += memcmp.0:32
UNWINDSET += memcpy.0:32

# The nested memcmp in this loop will exponentially increase the CBMC bounds checking.
# Be very careful increasing this. At the time of writing this, the PKCS stack was
# configured to store just one object.
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvAddObjectToList.0:2
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvDeleteObjectFromList.0:2
UNWINDSET += C_GenerateKeyPair.0:$(TEMPLATE_SIZE)
UNWINDSET += C_GenerateKeyPair.1:$(TEMPLATE_SIZE)

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
