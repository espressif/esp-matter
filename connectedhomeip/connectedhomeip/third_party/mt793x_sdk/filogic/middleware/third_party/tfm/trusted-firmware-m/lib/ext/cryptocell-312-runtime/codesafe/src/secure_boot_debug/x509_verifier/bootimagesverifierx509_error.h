/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _BOOT_IMAGES_VERIFIER_X509_ERROR_H
#define _BOOT_IMAGES_VERIFIER_X509_ERROR_H

/*! @file
@brief This file contains error code definitions used for the Secure Boot and Secure Debug X509 APIs.
*/

#include "secureboot_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! Defines error code for invalid input parameters. */
#define CC_BOOT_IMG_VERIFIER_CERT_INV_PARAM                 CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000001
/*! Defines error code for invalid token. */
#define CC_BOOT_IMG_VERIFIER_CERT_ILLEGAL_TOKEN             CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000002
/*! Defines error code for invalid certificate type. */
#define CC_BOOT_IMG_VERIFIER_CERT_ILLEGAL_CERT_TYPE         CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000003
/*! Defines error code for illegal workspace size. */
#define CC_BOOT_IMG_VERIFIER_CERT_WSP_SIZE_TOO_SMALL        CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000004
/*! Defines error code for unsupported hash algorithm. */
#define CC_BOOT_IMG_VERIFIER_CERT_UNSUPPORTED_HASH_ALG          CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000005
/*! Defines error code for unsupported certificate version. */
#define CC_BOOT_IMG_VERIFIER_CERT_ILLEGAL_VERSION           CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000006
/*! Defines error code for illegal X509 extension size. */
#define CC_BOOT_IMG_VERIFIER_CERT_ILLEGAL_EXT_SIZE              CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR + 0x00000007



#ifdef __cplusplus
}
#endif

#endif


