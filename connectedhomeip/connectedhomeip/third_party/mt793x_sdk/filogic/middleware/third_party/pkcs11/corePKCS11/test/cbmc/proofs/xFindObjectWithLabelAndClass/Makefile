# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = xFindObjectWithLabelAndClass_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = xFindObjectWithLabelAndClass

# Setting found in `core_pkcs11_config.h`
MAX_LABEL_SIZE=32

DEFINES += -DMAX_LABEL_SIZE=$(MAX_LABEL_SIZE)
INCLUDES +=

REMOVE_FUNCTION_BODY += C_Initialize
REMOVE_FUNCTION_BODY += C_Login
REMOVE_FUNCTION_BODY += C_OpenSession
UNWINDSET += strlen.0:$(MAX_LABEL_SIZE)

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/pkcs11_interface_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/core_pkcs11.c

include ../Makefile.common
