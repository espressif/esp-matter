/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
@addtogroup cc_sbrt
@{
 */

/*!
 @file
 @brief This file contains CryptoCell Secure Boot certificate-chain processing APIs.
 */

#ifndef  _MBEDTLS_CC_SBRT_H
#define  _MBEDTLS_CC_SBRT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_defs.h"
#include "secureboot_gen_defs.h"
/*!
  @brief This function initializes the Secure Boot certificate-chain
  processing.

  It initializes the internal data fields of the certificate package.

  @note This function must be the first API called when processing a Secure
  Boot certificate chain.

  @return \c CC_OK on success.
  @return A non-zero value from bsv_error.h on failure.
 */

CCError_t mbedtls_sb_cert_chain_cerification_init(
    /*! [in/out] A pointer to the information about the certificate package. */
    CCSbCertInfo_t *certPkgInfo
    );

/*!
  @brief This function verifies a single certificate package containing
  either a key or content certificate.

  It verifies the following:
  <ul><li>The public key as saved in the certificate, against its hash.
  Its hash is found in either the OTP memory (HBK) or in \p certPkgInfo.</li>
  <li>The RSA signature of the certificate.</li>
  <li>The SW version in the certificate is higher than or equal to the minimal
  SW version, as recorded on the device and passed in \p certPkgInfo.</li>
  <li>For content certificates: Each SW module against its hash in the
  certificate.</li></ul>

  @note The certificates may reside in the memory or in the flash. The
  flashReadFunc() must be implemented accordingly. \par
  @note The certificates and images must both be placed either in the
  memory or in the flash. \par

  @return \c CC_OK on success.
  @return A non-zero value from bsv_error.h on failure.
 */
CCError_t mbedtls_sb_cert_verify_single(
    /*! [in] A pointer to the flash-read function. */
    CCSbFlashReadFunc flashReadFunc,
        /*! [in] An additional pointer for flashRead() usage. May be NULL. */
        void *userContext,
        /*! [in] The address where the certificate is located. This address is
        provided to \p flashReadFunc. */
        CCAddr_t certStoreAddress,
        /*! [in/out] A pointer to the certificate-package information. */
        CCSbCertInfo_t *pCertPkgInfo,
        /*! [in/out] A pointer to a buffer used for extracting the X.509 TBS
        Headers. Note: Must be NULL for proprietary certificates. */
        uint32_t *pHeader,
        /*! [in] The size of \p pHeader in Bytes. Note: Must be 0 for
        proprietary certificates. */
        uint32_t  headerSize,
        /*! [in] A buffer for the internal use of the function. */
        uint32_t *pWorkspace,
        /*! [in] The size of the workspace in bytes. Note: Must be at least
        #CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES. */
        uint32_t workspaceSize
    );

/*!
  @brief This function changes the storage address of a specific SW image in
  the content certificate.

  @note The certificate must be loaded to the RAM before calling this
  function. \par
  @note The function does not verify the certificate before the address change. \par

  @return \c CC_OK on success.
  @return A non-zero value from bsv_error.h on failure.
 */
CCError_t mbedtls_sb_sw_image_store_address_change(
    /*! [in] The certificate address after it has been loaded to memory. */
    uint32_t *pCert,
    /*! [in] The maximal memory size allocated for the certificate in words
    (certificate boundaries). */
    uint32_t maxCertSizeWords,
    /*! [in] The new storage address to change to. */
    CCAddr_t address,
    /*! [in] The index of the SW image in the content certificate, starting
    from 0. */
    uint32_t indexOfAddress
    );

#ifdef __cplusplus
}

#endif /*_MBEDTLS_SBRT_H*/

/*!
 @}
 */
#endif

