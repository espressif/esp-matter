/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOT_IMAGES_VERIFIER_API_H
#define _BOOT_IMAGES_VERIFIER_API_H


#ifdef __cplusplus
extern "C"
{
#endif


/*! @file
@brief This file contains the set of Secure Boot APIs.
*/

#include "secureboot_defs.h"
#include "secureboot_gen_defs.h"


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
@brief This function initializes the Secure Boot certificate chain processing, and must be the first API called when processing Secure Boot certificate chain. It initializes the internal data fields of the certificate package.

@return CC_OK on success.
@return A non-zero value from sbrom_bsv_error.h on failure.
*/
CCError_t CC_SbCertChainVerificationInit(
    CCSbCertInfo_t *certPkgInfo     /*!< [in/out] Pointer to the information about the certificate package */
    );

/*!
@brief This function verifies a single certificate package (containing either a key or content certificate). It verifies the following:
    <ul><li> The public key (as saved in the certificate) against its Hash that is either found in the OTP memory (HBK) or in certPkgInfo.</li>
    <li> The certificate's RSA signature.</li>
    <li> The SW version in the certificate must be higher than or equal to the minimum SW version, as recorded on the device and passed in certPkgInfo.</li>
    <li> Each SW module against its Hash in the certificate (for content certificates).</li></ul>

@return CC_OK   On success.
@return A non-zero value from bsv_error.h on failure.
*/
CCError_t CC_SbCertVerifySingle(
    CCSbFlashReadFunc flashReadFunc,    /*!< [in] Pointer to the flash read function. */
    void *userContext,          /*!< [in] An additional pointer for flashRead usage. May be NULL. */
    unsigned long hwBaseAddress,        /*!< [in] CryptoCell HW registers' base address. */
    CCAddr_t certStoreAddress,      /*!< [in] Flash address where the certificate is located. This address is provided to flashReadFunc. */
    CCSbCertInfo_t *certPkgInfo,        /*!< [in/out] Pointer to the information about the certificate package. */
    uint32_t  *pHeader,                 /*!< [in/out] Pointer to a buffer used for extracting the X509 TBS Headers. Must be NULL for proprietary certificates. */
    uint32_t  headerSize,           /*!< [in] The size of pHeader in bytes. Must be 0 for proprietary certificates. */
    uint32_t *pWorkspace,           /*!< [in] Buffer for the function's internal use. */
    uint32_t workspaceSize          /*!< [in] The size of the workspace in bytes. Must be at least CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES. */
    );

#ifdef __cplusplus
}
#endif

#endif


