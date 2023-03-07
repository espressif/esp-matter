/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_ECPKI_ECDSA_H
#define _CC_ECPKI_ECDSA_H

/*!
@file
@brief This file defines the APIs that support the ECDSA functions.
@defgroup cc_ecpki_ecdsa CryptoCell ECDSA APIs
@{
@ingroup cryptocell_ecpki

*/

#include "cc_error.h"
#include "cc_ecpki_types.h"
#include "cc_hash_defs.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif



/**************************************************************************
 *                CC_EcdsaSign - integrated function
 **************************************************************************/
/*!
@brief This function performs an ECDSA sign operation in integrated form.

\note Using of HASH functions with HASH size greater than EC modulus size, is not recommended!.
Algorithm according to the ANSI X9.62-2005: Public Key Cryptography for the Financial Services Industry, The Elliptic
Curve Digital Signature Algorithm (ECDSA) standard.

The message data may be either a non-hashed data or a digest of a hash function.
For a non-hashed data, the message data will be hashed using the hash function indicated by ::CCEcpkiHashOpMode_t.
For a digest, ::CCEcpkiHashOpMode_t should indicate the hash function that the message data was created by, and it will not be hashed.


@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h, cc_hash_error.h or cc_rnd_error.h.
**/
CIMPORT_C CCError_t CC_EcdsaSign(
                     CCRndContext_t         *pRndContext,            /*!< [in/out] Pointer to the RND context buffer. */
                     CCEcdsaSignUserContext_t   *pSignUserContext,   /*!< [in/out] Pointer to the user buffer for signing the database. */
                     CCEcpkiUserPrivKey_t       *pSignerPrivKey,     /*!< [in]  A pointer to a user private key structure. */
                     CCEcpkiHashOpMode_t        hashMode,            /*!< [in]  One of the supported SHA-x HASH modes, as defined in
                                                   ::CCEcpkiHashOpMode_t.
                                                   \note MD5 is not supported. */
                     uint8_t                    *pMessageDataIn,     /*!< [in] Pointer to the input data to be signed.
                                                   The size of the scatter/gather list representing the data buffer
                                                   is limited to 128 entries, and the size of each entry is limited
                                                   to 64KB (fragments larger than 64KB are broken into
                                                   fragments <= 64KB). */
                     size_t                     messageSizeInBytes,  /*!< [in]  Size of message data in bytes. */
                     uint8_t                    *pSignatureOut,      /*!< [in]  Pointer to a buffer for output of signature. */
                     size_t                     *pSignatureOutSize   /*!< [in/out] Pointer to the signature size. Used to pass the size of
                                                       the SignatureOut buffer (in), which must be >=
                                                       2 * OrderSizeInBytes. When the API returns,
                                                       it is replaced with the size of the actual signature (out). */
                     );



/**************************************************************************
 *                CC_EcdsaVerify integrated function
 **************************************************************************/
/*!
@brief This function performs an ECDSA verify operation in integrated form.
Algorithm according to the ANSI X9.62-2005: Public Key Cryptography for the Financial Services Industry,
The Elliptic Curve Digital Signature Algorithm (ECDSA) standard.

The message data may be either a non-hashed data or a digest of a hash function.
For a non-hashed data, the message data will be hashed using the hash function indicated by ::CCEcpkiHashOpMode_t.
For a digest, ::CCEcpkiHashOpMode_t should indicate the hash function that the message data was created by, and it will not be hashed.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h or cc_hash_error.h.
*/
CIMPORT_C CCError_t CC_EcdsaVerify (
                    CCEcdsaVerifyUserContext_t *pVerifyUserContext, /*!< [in] Pointer to the user buffer for signing the database. */
                    CCEcpkiUserPublKey_t       *pUserPublKey,       /*!< [in] Pointer to a user public key structure. */
                    CCEcpkiHashOpMode_t         hashMode,           /*!< [in] One of the supported SHA-x HASH modes, as defined in
                                                  ::CCEcpkiHashOpMode_t.
                                                  \note MD5 is not supported. */
                    uint8_t                     *pSignatureIn,       /*!< [in] Pointer to the signature to be verified. */
                    size_t                      SignatureSizeBytes,  /*!< [in] Size of the signature (in bytes).  */
                    uint8_t                     *pMessageDataIn,     /*!< [in] Pointer to the input data that was signed (same as given to
                                                      the signing function). The size of the scatter/gather list representing
                                                      the data buffer is limited to 128 entries, and the size of each entry is
                                                      limited to 64KB (fragments larger than 64KB are broken into fragments <= 64KB). */
                    size_t                      messageSizeInBytes   /*!< [in] Size of the input data (in bytes). */
                    );


/**********************************************************************************************************/


#ifdef __cplusplus
}
#endif
/**
@}
 */

#endif
