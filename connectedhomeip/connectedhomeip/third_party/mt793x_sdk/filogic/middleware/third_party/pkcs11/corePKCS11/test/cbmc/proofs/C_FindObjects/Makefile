# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_FindObjects_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_FindObjects

DEFINES +=
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList

# This should be similar to the dummy data length in "core_pkcs11_pal_stubs.c" PKCS11_PAL_GetObjectValue
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvFindObjectInListByLabel.0:13
# This should align with the max object count configured in core_pkcs11_config.h
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvAddObjectToList.0:2

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
