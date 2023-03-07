/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_RSA_BUILD_H
#define _CC_RSA_BUILD_H

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_error.h"
#include "cc_rsa_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file defines some utility functions for working with RSA cryptography.
@defgroup cc_rsa_build CryptoCell RSA Utility APIs
@{
@ingroup cc_rsa
*/

/******************************************************************************************/
/*!
@brief Builds a ::CCRsaUserPubKey_t public key structure with the provided modulus and exponent.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaPubKeyBuild(
                                    CCRsaUserPubKey_t *UserPubKey_ptr,   /*!< [out] Pointer to the public key structure. */
                                    uint8_t *Exponent_ptr,                  /*!< [in]  Pointer to the exponent stream of bytes (Big-Endian format). */
                                    size_t   ExponentSize,                  /*!< [in]  The size of the exponent (in bytes). */
                                    uint8_t *Modulus_ptr,                   /*!< [in]  Pointer to the modulus stream of bytes (Big-Endian format).
                                               The most significant bit (MSB) must be set to '1'. */
                                    size_t   ModulusSize                    /*!< [in]  The modulus size in bytes. Supported sizes are 256, 384 and 512 bytes. */
);


/******************************************************************************************/
/*!
@brief Builds a ::CCRsaUserPrivKey_t private-key structure with the provided modulus and exponent, marking the key as a non-CRT key.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaPrivKeyBuild(
                                    CCRsaUserPrivKey_t   *UserPrivKey_ptr,     /*!< [out] Pointer to the public key structure.*/
                                    uint8_t                 *PrivExponent_ptr,    /*!< [in]  Pointer to the private exponent stream of bytes (Big-Endian format). */
                                    size_t                   PrivExponentSize,    /*!< [in]  The size of the private exponent (in bytes). */
                                    uint8_t                 *PubExponent_ptr,     /*!< [in]  Pointer to the public exponent stream of bytes (Big-Endian format). */
                                    size_t                   PubExponentSize,     /*!< [in]  The size of the public exponent (in bytes). */
                                    uint8_t                 *Modulus_ptr,         /*!< [in]  Pointer to the modulus stream of bytes (Big-Endian format).
                                               The most significant bit must be set to '1'. */
                                    size_t                   ModulusSize          /*!< [in]  The modulus size in bytes. Supported sizes are 256, 384 and 512. */
);

/******************************************************************************************/
/*!
@brief Builds a ::CCRsaUserPrivKey_t private-key structure with the provided parameters, marking the key as a CRT key.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaPrivKeyCrtBuild(
                                      CCRsaUserPrivKey_t *UserPrivKey_ptr,   /*!< [out] Pointer to the public key structure. */
                                      uint8_t *P_ptr,                           /*!< [in]  Pointer to the first factor stream of bytes (Big-Endian format). */
                                      size_t   PSize,                           /*!< [in]  The size of the first factor (in bytes). */
                                      uint8_t *Q_ptr,                           /*!< [in]  Pointer to the second factor stream of bytes (Big-Endian format). */
                                      size_t   QSize,                           /*!< [in]  The size of the second factor (in bytes). */
                                      uint8_t *dP_ptr,                          /*!< [in]  Pointer to the first factor's CRT exponent stream of bytes
                                               (Big-Endian format). */
                                      size_t   dPSize,                          /*!< [in]  The size of the first factor's CRT exponent (in bytes). */
                                      uint8_t *dQ_ptr,                          /*!< [in]  Pointer to the second factor's CRT exponent stream of bytes
                                               (Big-Endian format). */
                                      size_t   dQSize,                          /*!< [in]  The size of the second factor's CRT exponent (in bytes). */
                                      uint8_t *qInv_ptr,                        /*!< [in]  Pointer to the first CRT coefficient stream of bytes (Big-Endian format). */
                                      size_t   qInvSize                         /*!< [in]  The size of the first CRT coefficient (in bytes). */
);


/******************************************************************************************/
/*!
@brief The function gets the e,n public key parameters from the input
CCRsaUserPubKey_t structure. The function can also be used to retrieve the
modulus and exponent sizes only (Exponent_ptr AND Modulus_ptr must be set to
NULL).

\note All members of input UserPubKey_ptr structure must be initialized.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaPubKeyGet(
                                CCRsaUserPubKey_t *UserPubKey_ptr,   /*!< [in] A pointer to the public key structure. */
                                uint8_t  *Exponent_ptr,                 /*!< [out] A pointer to the exponent stream of bytes (Big-Endian format). */
                                size_t   *ExponentSize_ptr,             /*!< [in/out] the size of the exponent buffer in bytes,
                                              it is updated to the actual size of the exponent, in bytes. */
                                uint8_t  *Modulus_ptr,                  /*!< [out] A pointer to the modulus stream of bytes (Big-Endian format).
                                           The MS (most significant) bit must be set to '1'. */
                                size_t   *ModulusSize_ptr               /*!< [in/out] the size of the modulus buffer in bytes, it is updated to the actual
                                              size of the modulus, in bytes. */
);

/******************************************************************************************/
/*!
@brief The function gets the d,n and e - private key parameters (non CRT mode) from the input
CCRsaUserPrivKey_t structure.

\note All members of input UserPrivKey_ptr structure must be initialized. All output pointers must be allocated.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CEXPORT_C CCError_t CC_RsaGetPrivKey(CCRsaUserPrivKey_t *UserPrivKey_ptr        /*!< [in] A pointer to the private key structure.*/,
                     uint8_t             *PrivExponent_ptr      /*!< [out] A pointer to the exponent stream of bytes (Big-Endian format).*/,
                     uint16_t            *PrivExponentSize_ptr  /*!< [in,out] The size of the private exponent buffer in bytes , it is updated to the
                                                 actual size of the private exponent, in bytes*/,
                     uint8_t             *PubExponent_ptr       /*!< [out] A pointer to the public exponent stream of bytes ( Big endian ).*/,
                     uint16_t            *PubExponentSize_ptr,  /*!< [in,out] The size of the exponent buffer in bytes , it is updated to the
                                                 actual size of the exponent, in bytes*/
                     uint8_t             *Modulus_ptr,          /*!< [out] A pointer to the modulus stream of bytes (Big-Endian format).
                                                 The MS (most significant) bit must be set to '1'.*/
                     uint16_t            *ModulusSize_ptr       /*!< [in,out] The size of the modulus buffer in bytes , it is updated to the
                                                 actual size of the modulus, in bytes*/
);


/******************************************************************************************/
/*!
@brief The function gets the P, Q, dP, dQ and QInv - private key parameters (CRT mode) from the input
CCRsaUserPrivKey_t structure.

\note All members of input UserPrivKey_ptr structure must be initialized. All output pointers must be allocated.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
*/
CEXPORT_C CCError_t CC_RsaGetPrivKeyCRT(CCRsaUserPrivKey_t *UserPrivKey_ptr /*!< [in] A pointer to the private key structure.*/,
                    uint8_t *P_ptr              /*!< [out] A pointer to the first factor stream of bytes ( Big endian ).*/,
                    uint16_t *PSize_ptr,            /*!< [in,out] The size of the first factor buffer in bytes , updated to the actual size of the
                                             first factor, in bytes.*/
                    uint8_t *Q_ptr,             /*!< [out] A pointer to the second factor stream of bytes ( Big endian ).*/
                    uint16_t *QSize_ptr,            /*!< [in,out] The size of the second factor buffer in bytes , updated to the
                                             actual size of the second factor, in bytes.*/
                    uint8_t *dP_ptr,            /*!< [out] A pointer to the first factors CRT exponent stream of bytes ( Big endian ).*/
                    uint16_t *dPSize_ptr,           /*!< [in,out] The size of the first factor exponent buffer in bytes , updated to the
                                             actual size of the first factor exponent, in bytes.*/
                    uint8_t *dQ_ptr,            /*!< [out] A pointer to the second factors CRT exponent stream of bytes ( Big endian ).*/
                    uint16_t *dQSize_ptr,           /*!< [in,out] The size of the second factors CRT exponent buffer in bytes , updated to the
                                             actual size of the second factors CRT exponent, in bytes.*/
                    uint8_t *qInv_ptr,          /*!< [out] A pointer to the first CRT coefficient stream of bytes ( Big endian ).*/
                    uint16_t *qInvSize_ptr          /*!< [in,out] The size of the first CRT coefficient buffer in bytes , updated to the
                                             actual size of the first CRT coefficient, in bytes.*/
);

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
#endif /* _CC_RSA_BUILD_H */
