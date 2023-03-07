/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOT_IMAGES_VERIFIER_ERROR_H
#define _BOOT_IMAGES_VERIFIER_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_error.h"

/*! @file
@brief This file contains error code definitions used for the Secure Boot and Secure Debug APIs.
*/

/*! Defines error code for invalid input parameters. */
#define CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM                            (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000001)
/*! Defines error code for invalid OTP version. */
#define CC_BOOT_IMG_VERIFIER_OTP_VERSION_FAILURE                        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000002)
/*! Defines error code for illegal certificate's magic number. */
#define CC_BOOT_IMG_VERIFIER_CERT_MAGIC_NUM_INCORRECT                   (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000003)
/*! Defines error code for illegal certificate version. */
#define CC_BOOT_IMG_VERIFIER_CERT_VERSION_NUM_INCORRECT                 (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000004)
/*! Defines error code for illegal certificate SW version, that is smaller than the version stored in the OTP. */
#define CC_BOOT_IMG_VERIFIER_SW_VER_SMALLER_THAN_MIN_VER                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000005)
/*! Defines error code for public key verification compared to the OTP value failed. */
#define CC_BOOT_IMG_VERIFIER_PUB_KEY_HASH_VALIDATION_FAILURE            (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000006)
/*! Defines error code for certificate's RSA signature verification failure. */
#define CC_BOOT_IMG_VERIFIER_RSA_SIG_VERIFICATION_FAILED                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000007)
/*! Defines error code for workspace buffer given to the API is too small. */
#define CC_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL                   (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000008)
/*! Defines error code for SW image hash verification failure. */
#define CC_BOOT_IMG_VERIFIER_SW_COMP_FAILED_VERIFICATION                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000009)
/*! Defines error code for illegal SW version or ID of SW version. */
#define CC_BOOT_IMG_VERIFIER_CERT_SW_VER_ILLEGAL                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000000D)
/*! Defines error code for illegal number of SW components (zero). */
#define CC_BOOT_IMG_VERIFIER_SW_COMP_SIZE_IS_NULL                       (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000011)
/*! Defines error code for hash of public key is not burned yet. */
#define CC_BOOT_IMG_VERIFIER_PUBLIC_KEY_HASH_EMPTY                      (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000014)
/*! Defines error code for illegal lifecycle state (LCS) for operation.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_LCS_FOR_OPERATION_ERR          (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000015)
/*! Defines error code for hash of public key is already programmed.*/
#define CC_BOOT_IMG_VERIFIER_PUB_KEY_ALREADY_PROGRAMMED_ERR     (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000016)
/*! Defines error code for OTP write failure.*/
#define CC_BOOT_IMG_VERIFIER_OTP_WRITE_FAIL_ERR             (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000017)
/*! Defines error code for incorrect certificate type.*/
#define CC_BOOT_IMG_VERIFIER_INCORRECT_CERT_TYPE            (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000018)
/*! Defines error code for illegal Hash boot key index.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_HBK_IDX                (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000019)
/*! Defines error code for hash boot key of ICV is not programmed.*/
#define CC_BOOT_IMG_VERIFIER_PUB_KEY1_NOT_PROGRAMMED_ERR        (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001A)
/*! Defines error code for illegal certificate version value.*/
#define CC_BOOT_IMG_VERIFIER_CERT_VER_VAL_ILLEGAL               (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001C)
/*! Defines error code for illegal certificate decoding value.*/
#define CC_BOOT_IMG_VERIFIER_CERT_DECODING_ILLEGAL              (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001D)
/*! Defines error code for illegal Kce in RMA LCS.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_KCE_IN_RMA_STATE               (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001E)
/*! Defines error code for illegal SOC ID value.*/
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_SOC_ID_VALUE               (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x0000001F)
/*! Defines error code for illegal number of SW images per content certificate. */
#define CC_BOOT_IMG_VERIFIER_ILLEGAL_NUM_OF_IMAGES              (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000020)
/*! Defines error code for no need to verify hashed public key. */
#define CC_BOOT_IMG_VERIFIER_SKIP_PUBLIC_KEY_VERIFY                      (CC_BOOT_IMG_VERIFIER_BASE_ERROR + 0x00000014)


#ifdef __cplusplus
}
#endif

#endif


