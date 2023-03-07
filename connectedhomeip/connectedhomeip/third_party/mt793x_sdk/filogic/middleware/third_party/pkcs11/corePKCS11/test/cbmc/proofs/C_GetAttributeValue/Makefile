# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_GetAttributeValue_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_GetAttributeValue

# Size of attribute template.
TEMPLATE_SIZE=9

# Needs to be one bigger than TEMPLATE_SIZE
UNWIND_SIZE=10

# Defined in `core_pkcs11_config.h`
# Should be one more than the total number of objects in the PKCS stack.
MAX_OBJECT_NUM=2

DEFINES += -DTEMPLATE_SIZE=$(TEMPLATE_SIZE)
DEFINES += -DMAX_OBJECT_NUM=$(MAX_OBJECT_NUM)
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY +=

# In the harness we are going to assume 8 as there are only 7 cases we care about in the loop, plus 1 for the
# default in the switch statement. We will unroll 1 extra time for good measure.
# The number is arbitrary, but we bound the loop so it doesn't check forever.
# NOTE: If this proof is slow we can reduce this unwind.
UNWINDSET += C_GetAttributeValue.0:$(UNWIND_SIZE)
UNWINDSET += harness.0:$(UNWIND_SIZE)


PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
