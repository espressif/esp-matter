# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_CloseSession_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_CloseSession

DEFINES +=
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_free
REMOVE_FUNCTION_BODY += mbedtls_entropy_free
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
