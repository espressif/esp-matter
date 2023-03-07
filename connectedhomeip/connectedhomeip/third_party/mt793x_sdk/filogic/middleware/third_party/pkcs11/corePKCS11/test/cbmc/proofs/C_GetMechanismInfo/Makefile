# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_GetMechanismInfo_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_GetMechanismInfo

DEFINES +=
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY +=

# Add the time of writing this proof, 5 mechanisms are supported.
# One extra unwind to prove memory safety.
UNWINDSET +=  C_GetMechanismInfo.0:6

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
