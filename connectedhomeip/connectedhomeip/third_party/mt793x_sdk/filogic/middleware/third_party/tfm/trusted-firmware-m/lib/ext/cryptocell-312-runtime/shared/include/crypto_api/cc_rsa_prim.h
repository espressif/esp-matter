/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_RSA_PRIM_H
#define _CC_RSA_PRIM_H

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_rsa_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file defines the API that implements the Public-Key Cryptography Standards (PKCS) #1
RSA Cryptography Specifications Version 2.1 primitive functions.
@defgroup cc_rsa_prim CryptoCell RSA primitive APIs
@{
@ingroup cc_rsa


\note Direct use of primitive functions, rather than schemes to protect data, is strongly discouraged as primitive functions are
susceptible to well-known attacks.
*/



/**********************************************************************************/
/*!
@brief Implements the RSAEP algorithm, as defined in section 6.1.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
Specifications Version 2.1.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.
 */
CIMPORT_C CCError_t CC_RsaPrimEncrypt(
                                CCRsaUserPubKey_t *UserPubKey_ptr,       /*!< [in]  Pointer to the public-key data structure. */
                                CCRsaPrimeData_t  *PrimeData_ptr,        /*!< [in]  Pointer to a temporary structure containing internal buffers. */
                                uint8_t              *Data_ptr,          /*!< [in]  Pointer to the data to encrypt. */
                                size_t                DataSize,          /*!< [in]  The size (in bytes) of the data to encrypt. Data size must be &le; Modulus size.
                                            It can be smaller than the modulus size but it is not recommended.
                                                                                    If smaller, the data is zero-padded up to the modulus size.
                                                                                    Since the result of decryption is always the size of the modulus,
                                                                                    this causes the size of the decrypted data to be larger than the
                                            originally encrypted data. */
                                uint8_t              *Output_ptr         /*!< [out] Pointer to the encrypted data. The buffer size must be &ge; the modulus size. */
);


/**********************************************************************************/
/*!
@brief Implements the RSADP algorithm, as defined in section 6.1.2 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
Specifications Version 2.1.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h on failure.

*/
CIMPORT_C CCError_t CC_RsaPrimDecrypt(
                            CCRsaUserPrivKey_t *UserPrivKey_ptr,     /*!< [in]  Pointer to the private-key data structure.
                                                                                    The representation (pair or quintuple) and hence the algorithm (CRT or not-CRT)
                                                                                    is determined by the Private Key data structure - using
                                            ::CC_RsaPrivKeyBuild or ::CC_RsaPrivKeyCrtBuild
                                                                                    to determine which algorithm is used.*/
                            CCRsaPrimeData_t   *PrimeData_ptr,       /*!< [in]  Pointer to a temporary structure containing internal buffers required for
                                        the RSA operation. */
                            uint8_t     *Data_ptr,                   /*!< [in]  Pointer to the data to be decrypted. */
                            size_t       DataSize,                   /*!< [in]  The size (in bytes) of the data to decrypt. Must be equal to the modulus size. */
                            uint8_t     *Output_ptr                  /*!< [out] Pointer to the decrypted data. The buffer size must be &le; the modulus size. */
);


/*!
@brief Implements the RSASP1 algorithm, as defined in [PKCS1_2.1] - 6.2.1, as a call to ::CC_RsaPrimDecrypt,
since the signature primitive is identical to the decryption primitive.
*/
#define CC_RsaPrimSign CC_RsaPrimDecrypt

/*!
@brief Implements the RSAVP1 algorithm, as defined in [PKCS1_2.1] - 6.2.2, as a call to ::CC_RsaPrimEncrypt.
*/
#define CC_RsaPrimVerify CC_RsaPrimEncrypt

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
#endif /* _CC_RSA_PRIM_H */
