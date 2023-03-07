/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_RSA_KG_H
#define _CC_RSA_KG_H

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_rsa_types.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief Generates a RSA pair of public and private keys.
@defgroup cc_rsa_kg CryptoCell RSA key generation APIs
@{
@ingroup cc_rsa
*/

/************************ Defines ******************************/

/* Max allowed size and values of public exponent for key generation in CryptoCell*/
/*! Maximal public exponent size in bits. */
#define CC_RSA_KG_PUB_EXP_MAX_SIZE_BITS    17
/*! Definition of public exponent value. */
#define CC_RSA_KG_PUB_EXP_ALLOW_VAL_1      0x000003
/*! Definition of public exponent value. */
#define CC_RSA_KG_PUB_EXP_ALLOW_VAL_2      0x000011
/*! Definition of public exponent value. */
#define CC_RSA_KG_PUB_EXP_ALLOW_VAL_3      0x010001




/***********************************************************************************************/

/*!
@brief CC_RsaKgKeyPairGenerate generates a Pair of public and private keys on non CRT mode according to ANSI X9.31-1988: Public Key
Cryptography Using Reversible Algorithms for the Financial Services Industry (rDSA).

\note   To be FIPS Publication 186-4: Digital Signature Standard (DSS) [5.1] compliant use only the following:
    key sizes (in bits): 2048, 3072, 4096 and public exponent value 0x10001.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h or cc_rnd_error.h on failure.

*/
CIMPORT_C CCError_t CC_RsaKgKeyPairGenerate(
                                        CCRndContext_t *rndContext_ptr,          /*!< [in/out] Pointer to the RND context buffer. */
                                        uint8_t             *pubExp_ptr,            /*!< [in]  The pointer to the public exponent (public key). */
                                        size_t               pubExpSizeInBytes,     /*!< [in]  The public exponent size in bytes. */
                                        size_t               keySize,               /*!< [in]  The size of the key, in bits. Supported sizes are
                                                   2048, 3072 and 4096 bit. */
                                        CCRsaUserPrivKey_t *userPrivKey_ptr,     /*!< [out] Pointer to the private-key structure. */
                                        CCRsaUserPubKey_t  *userPubKey_ptr,      /*!< [out] Pointer to the public-key structure. */
                    CCRsaKgData_t      *keyGenData_ptr,      /*!< [in]  Pointer to a temporary structure required for the KeyGen operation. */
                                        CCRsaKgFipsContext_t    *pFipsCtx        /*!< [in]  Pointer to temporary buffer used in case FIPS certification if required
                                                (may be NULL for all other cases). */
);

/***********************************************************************************************/
/*!
@brief Generates a pair of public and private keys on CRT mode according to ANSI X9.31-1988: Public Key
Cryptography Using Reversible Algorithms for the Financial Services Industry (rDSA).

\note To be FIPS Publication 186-4: Digital Signature Standard (DSS) compliant use only the following key sizes (in bits): 2048, 3072 and 4096.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h or cc_rnd_error.h on failure.
*/

CIMPORT_C CCError_t CC_RsaKgKeyPairCrtGenerate(
                                        CCRndContext_t *rndContext_ptr,         /*!< [in/out] Pointer to the RND context buffer. */
                                        uint8_t             *pubExp_ptr,           /*!< [in]  The pointer to the public exponent (public key). */
                                        size_t               pubExpSizeInBytes,    /*!< [in]  The public exponent size in bytes. */
                                        size_t               keySize,              /*!< [in]  The size of the key, in bits. Supported sizes are
                                                                                              2048, 3072 and 4096 bit. */
                                        CCRsaUserPrivKey_t *userPrivKey_ptr,    /*!< [out] Pointer to the private-key structure. */
                                        CCRsaUserPubKey_t  *userPubKey_ptr,     /*!< [out] Pointer to the public-key structure. */
                                        CCRsaKgData_t      *keyGenData_ptr,     /*!< [in] Pointer to a temporary structure required for the KeyGen operation. */
                                        CCRsaKgFipsContext_t    *pFipsCtx       /*!< [in] Pointer to temporary buffer used in case FIPS certification if required
                                              (may be NULL for all other cases). */
);


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
#endif /* _CC_RSA_KG_H */


