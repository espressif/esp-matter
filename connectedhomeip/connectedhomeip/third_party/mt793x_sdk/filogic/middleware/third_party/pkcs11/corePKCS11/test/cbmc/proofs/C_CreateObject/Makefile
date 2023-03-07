# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

HARNESS_ENTRY = harness
HARNESS_FILE = C_CreateObject_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = C_CreateObject

# TEMPLATE_SIZE is bounded to reduce proof run time. A user can have an
# arbitrarily large template size, but our library generally only cares about
# some attributes. Memory safety can be proven within a reasonable bound
# past what our stack cares about. It adds not value to the proof to input the
# largest possible template of the maximum CK_ULONG size.
#
# Not recommended to increase this size, will lead to long proof times, with no
# more increase in coverage.
TEMPLATE_SIZE=6

# Defined in `core_pkcs11_config.h`
MAX_LABEL_SIZE=32

# Defined in `core_pkcs11_config.h`
# Should be one more than the total number of objects in the PKCS stack.
MAX_OBJECT_NUM=2

DEFINES += -DTEMPLATE_SIZE=$(TEMPLATE_SIZE)
DEFINES += -DTEMPLATE_ATTRIBUTE_MAX_SIZE=$(TEMPLATE_ATTRIBUTE_MAX_SIZE)

INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls/include
INCLUDES += -I$(SRCDIR)/source/dependency/3rdparty/mbedtls_utils

REMOVE_FUNCTION_BODY += C_Initialize
REMOVE_FUNCTION_BODY += C_Finalize
REMOVE_FUNCTION_BODY += C_GetFunctionList
REMOVE_FUNCTION_BODY += PKCS11_PAL_Initialize
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_free
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_init
REMOVE_FUNCTION_BODY += mbedtls_ctr_drbg_seed
REMOVE_FUNCTION_BODY += mbedtls_entropy_free
REMOVE_FUNCTION_BODY +=	mbedtls_entropy_init
REMOVE_FUNCTION_BODY +=	threading_mutex_free
REMOVE_FUNCTION_BODY +=	threading_mutex_init
REMOVE_FUNCTION_BODY += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvMbedTLS_Initialize

UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvCreateCertificate.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvGetKeyType.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvGetObjectClass.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvCreateECKey.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvGetLabel.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvCreateRsaKey.0:$(TEMPLATE_SIZE)
UNWINDSET += __CPROVER_file_local_core_pkcs11_mbedtls_c_prvAddObjectToList.0:$(MAX_OBJECT_NUM)
UNWINDSET += harness.0:$(TEMPLATE_SIZE)
UNWINDSET += memcmp.0:$(MAX_LABEL_SIZE)
UNWINDSET += strncmp.0:$(MAX_LABEL_SIZE)

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/mbedtls_stubs.c
PROOF_SOURCES += $(SRCDIR)/test/cbmc/stubs/core_pkcs11_pal_stubs.c
PROJECT_SOURCES += $(SRCDIR)/source/portable/mbedtls/core_pkcs11_mbedtls.c

include ../Makefile.common
