/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_RSA_SCHEMES_H
#define _CC_RSA_SCHEMES_H

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_error.h"
#include "cc_rsa_types.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file defines APIs that support Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5
and Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1 encryption and signature schemes.
@defgroup cc_rsa_schemes CryptoCell RSA encryption and signature schemes
@{
@ingroup cc_rsa
*/

/**********************************************************************************************************/
/*!
@brief This function implements the Encrypt algorithm, as defined in Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1 and Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5.

It should not be called directly. Instead, use macros ::CC_RsaOaepEncrypt or ::CC_RsaPkcs1V15Encrypt.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h, cc_rnd_error.h or cc_hash_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaSchemesEncrypt(
                        CCRndContext_t *rndContext_ptr,         /*!< [in/out] Pointer to the RND context buffer. */
                        CCRsaUserPubKey_t  *UserPubKey_ptr,     /*!< [in]  Pointer to the public key data structure. */
                        CCRsaPrimeData_t   *PrimeData_ptr,      /*!< [in]  Pointer to a temporary structure that is internally used as workspace for the
                                           Encryption operation. */
                        CCRsaHashOpMode_t hashFunc,             /*!< [in]  The HASH function to be used. One of the supported SHA-x HASH modes, as defined
                                           in ::CCRsaHashOpMode_t (MD5 is not supported).*/
                        uint8_t  *L,                            /*!< [in]  The label input pointer. Relevant for Public-Key Cryptography Standards (PKCS) #1 RSA
                                       Cryptography Specifications Version 2.1 only. NULL by default.
                                           NULL for Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5. */
                        size_t    Llen,                         /*!< [in]  The label length. Relevant for Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
                                       Specifications Version 2.1 only. Zero by default. Must be <=2048. Zero for Public-Key Cryptography
                                       Standards (PKCS) #1: RSA Encryption Standard Version 1.5. */
                        CCPkcs1Mgf_t MGF,                       /*!< [in]  The mask generation function. [PKCS1_2.1] defines MGF1, so the only value
                                       allowed here is CC_PKCS1_MGF1. */
                        uint8_t   *DataIn_ptr,                  /*!< [in]  Pointer to the data to encrypt. */
                        size_t     DataInSize,                  /*!< [in]  The size (in bytes) of the data to encrypt. The data size must be:
                                                                           <ul><li>For Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
                                           Version 2.1, DataSize <= modulus size - 2*HashLen - 2.</li>
                                                                           <li>For Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5,
                                           DataSize <= modulus size - 11.</li></ul> */
                        uint8_t   *Output_ptr,                  /*!< [out] Pointer to the encrypted data. The buffer must be at least modulus size bytes long. */
                        CCPkcs1Version_t  PKCS1_ver             /*!< [in] Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5 or
                                      Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1,
                                      according to the functionality required. */
);

/*!
   @brief CC_RsaOaepEncrypt implements the RSAES-OAEP algorithm
   as defined in section 8.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1.

   \note It is not recommended to use hash MD5 in Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
     Specifications Version 2.1, therefore it is not supported.

   This function combines the RSA encryption primitive and the
   EME-OAEP encoding method, to provide an RSA-based encryption
   method that is semantically secure against adaptive
   chosen-ciphertext attacks. For additional details, see Public-Key Cryptography Standards
   (PKCS) #1 RSA Cryptography Specifications Version 2.1.
*/
#define CC_RsaOaepEncrypt(rndContext_ptr, UserPubKey_ptr,PrimeData_ptr,HashMode,L,Llen,MGF,Data_ptr,DataSize,Output_ptr)\
    CC_RsaSchemesEncrypt(rndContext_ptr, UserPubKey_ptr,PrimeData_ptr,HashMode,L,Llen,MGF,Data_ptr,DataSize,Output_ptr,CC_PKCS1_VER21)

/*!
   @brief
   CC_RsaPkcs1V15Encrypt implements the RSAES-PKCS1v15 algorithm
   as defined in section 8.2 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1.
*/
#define CC_RsaPkcs1V15Encrypt(rndContext_ptr, UserPubKey_ptr,PrimeData_ptr,DataIn_ptr,DataInSize,Output_ptr)\
    CC_RsaSchemesEncrypt(rndContext_ptr, UserPubKey_ptr,PrimeData_ptr,CC_RSA_HASH_NO_HASH_mode,NULL,0,CC_PKCS1_NO_MGF,DataIn_ptr,DataInSize, Output_ptr,CC_PKCS1_VER15)


/**********************************************************************************************************/
/*!
@brief This function implements the Decrypt algorithm, as defined in Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1 and
Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5.

It should not be called directly. Instead, use macros ::CC_RsaOaepDecrypt or ::CC_RsaPkcs1V15Decrypt.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h or cc_hash_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaSchemesDecrypt(
                                CCRsaUserPrivKey_t  *UserPrivKey_ptr,    /*!< [in]  Pointer to the private-key data structure of the user. */
                                CCRsaPrimeData_t    *PrimeData_ptr,      /*!< [in]  Pointer to a temporary structure that is internally used as workspace
                                            for the decryption operation. */
                                CCRsaHashOpMode_t  hashFunc,             /*!< [in]  The HASH function to be used. One of the supported SHA-x HASH modes,
                                            as defined in ::CCRsaHashOpMode_t (MD5 is not supported). */
                                uint8_t                 *L,              /*!< [in]  The label input pointer. Relevant for Public-Key Cryptography Standards (PKCS) #1
                                            RSA Cryptography Specifications Version 2.1 only. NULL by default.
                                            NULL for Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard
                                            Version 1.5. */
                                size_t                  Llen,            /*!< [in]  The label length. Relevant for Public-Key Cryptography Standards (PKCS) #1 RSA
                                            Cryptography Specifications Version 2.1 only. Zero by default.
                                            Zero for Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard
                                            Version 1.5. */
                                CCPkcs1Mgf_t        MGF,                 /*!< [in]  The mask generation function. Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
                                            Specifications Version 2.1 defines MGF1, so the only
                                            value allowed here is CC_PKCS1_MGF1. */
                                uint8_t                 *DataIn_ptr,     /*!< [in]  Pointer to the data to decrypt. */
                                size_t                  DataInSize,      /*!< [in]  The size (in bytes) of the data to decrypt. DataSize must be &le;
                                            the modulus size. */
                                uint8_t                 *Output_ptr,     /*!< [in]  Pointer to the decrypted data. The buffer must be at least
                                            PrivKey_ptr->N.len bytes long (i.e. the modulus size in bytes). */
                                size_t                  *OutputSize_ptr, /*!< [in]  Pointer to the byte size of the buffer pointed to by Output_buffer.
                                            The size must be:
                                                                                    <ul><li> For Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
                                                 Version 2.1: Modulus size > OutputSize >= (modulus size - 2*HashLen - 2).</li>
                                                                                    <li> For Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5:
                                             Modulus size > OutputSize >= (modulus size - 11).
                                                                                         The value pointed by OutputSize_ptr is updated after decryption with
                                             the actual number of bytes that are loaded to Output_ptr.</li></ul> */
                                CCPkcs1Version_t      PKCS1_ver          /*!< [in] Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5 or
                                           Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1,
                                           according to the functionality required. */
);

/**********************************************************************************************************/
/**
   @brief CC_RsaOaepDecrypt implements the RSAES-OAEP algorithm
   as section 8.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1.

   \note It is not recommended to use hash MD5 in Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
   Specifications Version 2.1, therefore it is not supported.

   This function combines the RSA decryption primitive and the
   EME-OAEP encoding method, to provide an RSA-based decryption
   method that is semantically secure against adaptive
   chosen-ciphertext attacks. For more details, see Public-Key Cryptography Standards
   (PKCS) #1 RSA Cryptography Specifications Version 2.1.

*/
#define CC_RsaOaepDecrypt(UserPrivKey_ptr,PrimeData_ptr,HashMode,L,Llen,MGF,Data_ptr,DataSize,Output_ptr,OutputSize_ptr)\
    CC_RsaSchemesDecrypt(UserPrivKey_ptr,PrimeData_ptr,HashMode,L,Llen,MGF,Data_ptr,DataSize,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/**
   @brief CC_RsaPkcs1V15Decrypt implements the RSAES-PKCS1v15 algorithm as defined
   in PKCS#1 v2.1 8.2.
*/
#define CC_RsaPkcs1V15Decrypt(UserPrivKey_ptr,PrimeData_ptr,DataIn_ptr,DataInSize,Output_ptr,OutputSize_ptr)\
    CC_RsaSchemesDecrypt(UserPrivKey_ptr,PrimeData_ptr,CC_RSA_HASH_NO_HASH_mode,NULL,0,CC_PKCS1_NO_MGF,DataIn_ptr,DataInSize,Output_ptr,OutputSize_ptr,CC_PKCS1_VER15)

/**********************************************************************************************************/
/*!
@brief Implements the Signing algorithm, as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5
or Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1, using a single function.

The input data may be either a non-hashed data or a digest of a hash function.
For a non-hashed data, the input data will be hashed using the hash function indicated by ::CCRsaHashOpMode_t.
For a digest, ::CCRsaHashOpMode_t should indicate the hash function that the input data was created by, and it will not be hashed.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h, cc_rnd_error.h or cc_hash_error.h on failure.
*/
CIMPORT_C CCError_t CC_RsaSign(
                   CCRndContext_t *rndContext_ptr,               /*!< [in/out] Pointer to the RND context buffer. */
                   CCRsaPrivUserContext_t *UserContext_ptr,   /*!< [in]  Pointer to a temporary context for internal use. */
                   CCRsaUserPrivKey_t *UserPrivKey_ptr,          /*!< [in]  Pointer to the private-key data structure of the user.
                                                                            The representation (pair or quintuple) and hence the algorithm (CRT or not CRT)
                                        is determined by the Private Key build function -
                                                                            ::CC_RsaPrivKeyBuild or ::CC_RsaPrivKeyCrtBuild. */
                   CCRsaHashOpMode_t rsaHashMode,                /*!< [in]  One of the supported SHA-x HASH modes, as defined in ::CCRsaHashOpMode_t.
                                        (MD5 is not supported). */
                   CCPkcs1Mgf_t MGF,                             /*!< [in]  The mask generation function. Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
                                        Version 2.1 defines only MGF1, so the only value allowed for it is CC_PKCS1_MGF1. */
                   size_t       SaltLen,                         /*!< [in]  The Length of the Salt buffer (relevant for Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography
                                        Specifications Version 2.1 only, typically lengths is 0 or hash Len).
                                                                            FIPS Publication 186-4: Digital Signature Standard (DSS) requires, that SaltLen <= hash len.
                                        If SaltLen > KeySize - hash Len - 2, the function returns an error.  */
                   uint8_t     *DataIn_ptr,                      /*!< [in]  Pointer to the input data to be signed.
                                                                            The size of the scatter/gather list representing the data buffer is limited to 128
                                        entries, and the size of each entry is limited to 64KB (fragments larger than
                                        64KB are broken into fragments <= 64KB). */
                   size_t      DataInSize,                       /*!< [in]  The size (in bytes) of the data to sign. */
                   uint8_t     *Output_ptr,                      /*!< [out] Pointer to the signature. The buffer must be at least PrivKey_ptr->N.len bytes
                                        long (i.e. the modulus size in bytes). */
                   size_t      *OutputSize_ptr,                  /*!< [in/out] Pointer to the signature size value - the input value is the signature
                                           buffer size allocated, the output value is the signature size used.
                                                                               he buffer must be equal to PrivKey_ptr->N.len bytes long
                                           (i.e. the modulus size in bytes). */
                   CCPkcs1Version_t PKCS1_ver                    /*!< [in]  Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5 or Public-Key Cryptography
                                        Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1, according to the functionality required. */
);


/*!
@brief CC_RsaPkcs1V15Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards (PKCS) #1:
RSA Encryption Standard Version 1.5.

This function combines the RSASP1 signature primitive and the EMSA-PKCS1v15 encoding method, to provide an RSA-based signature scheme.
For more details, see Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5.
 */

#define CC_RsaPkcs1V15Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,hashFunc,DataIn_ptr,DataInSize,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(hashFunc),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),(DataInSize),(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Sha1Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards
(PKCS) #1: RSA Encryption Standard Version 1.5, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-1.

\note The data_in size is already known after the Hash.
*/
#define CC_RsaPkcs1V15Sha1Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(CC_RSA_After_SHA1_mode),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Md5Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards (PKCS) #1:
RSA Encryption Standard Version 1.5, but without performing a HASH function - it assumes that the data in has already been
hashed using MD5.

\note The data_in size is already known after the Hash.
*/

#define CC_RsaPkcs1V15Md5Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),CC_RSA_After_MD5_mode,CC_PKCS1_NO_MGF,0,(DataIn_ptr),CC_HASH_MD5_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Sha224Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-224.

\note The data_in size is already known after the Hash.
*/
#define CC_RsaPkcs1V15Sha224Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(CC_RSA_After_SHA224_mode),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Sha256Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-256.

\note The data_in size is already known after the Hash.
*/
#define CC_RsaPkcs1V15Sha256Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(CC_RSA_After_SHA256_mode),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Sha1Sign implements the RSASSA-PKCS1v15 algorithm as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-384.

\note The data_in size is already known after the Hash.
*/
#define CC_RsaPkcs1V15Sha384Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(CC_RSA_After_SHA384_mode),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Sha512Sign implements the RSASSA-PKCS1v15 algorithm  as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5, but without performing a HASH function - it assumes that the data in has already been hashed using SHA-512.

\note The data_in size is already known after the Hash.
*/
#define CC_RsaPkcs1V15Sha512Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, (UserContext_ptr),(UserPrivKey_ptr),(CC_RSA_After_SHA512_mode),(CC_PKCS1_NO_MGF),0,(DataIn_ptr),CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES,(Output_ptr),(OutputSize_ptr),CC_PKCS1_VER15)



/*!
@brief CC_RsaPssSign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1, in a single function call.

\note According to the Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1 it is not recommended to use MD5 Hash,
therefore it is not supported.

The actual macro that is used by the user is ::CC_RsaPssSign.
*/

#define CC_RsaPssSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,hashFunc,MGF,SaltLen,DataIn_ptr,DataInSize,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,hashFunc,MGF,SaltLen,DataIn_ptr,DataInSize,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha1Sign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1
RSA Cryptography Specifications Version 2.1 in a single function call, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-1.

\note The data_in size is already known after the Hash.

The actual macro that is used by the users is ::CC_RsaPssSha1Sign.
*/

#define CC_RsaPssSha1Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,MGF,SaltLen,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,CC_RSA_After_SHA1_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha224Sign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1 in a single function call, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-224.

\note The data_in size is already known after the Hash.

The actual macro that is used by the users is ::CC_RsaPssSha224Sign.
*/

#define CC_RsaPssSha224Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,MGF,SaltLen,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,CC_RSA_After_SHA224_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha256Sign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1 in a single function call, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-256.

\note The data_in size is already known after the Hash.

The actual macro that is used by the users is ::CC_RsaPssSha256Sign.
*/

#define CC_RsaPssSha256Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,MGF,SaltLen,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,CC_RSA_After_SHA256_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha384Sign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1 in a single function call, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-384.

\note The data_in size is already known after the Hash.

The actual macro that is used by the users is ::CC_RsaPssSha384Sign.
*/

#define CC_RsaPssSha384Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,MGF,SaltLen,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,CC_RSA_After_SHA384_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha512Sign implements the RSASSA-PSS algorithm as defined in section 9.1 of Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications
Version 2.1 in a single function call, but without performing a HASH function -
it assumes that the data in has already been hashed using SHA-512.

\note The data_in size is already known after the Hash.

The actual macro that is used by the users is ::CC_RsaPssSha512Sign.
*/

#define CC_RsaPssSha512Sign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,MGF,SaltLen,DataIn_ptr,Output_ptr,OutputSize_ptr)\
    CC_RsaSign(rndContext_ptr, UserContext_ptr,UserPrivKey_ptr,CC_RSA_After_SHA512_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES,Output_ptr,OutputSize_ptr,CC_PKCS1_VER21)


/**********************************************************************************************************/
/*!
@brief Implements the RSA signature verification algorithms, in a single function call, as defined in Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5 and in Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 2.1.

The input data may be either a non-hashed data or a digest of a hash function.
For a non-hashed data, the input data will be hashed using the hash function indicated by ::CCRsaHashOpMode_t.
For a digest, ::CCRsaHashOpMode_t should indicate the hash function that the input data was created by, and it will not be hashed.

@return CC_OK on success.
@return A non-zero value from cc_rsa_error.h or cc_hash_error.h on failure.
*/

CIMPORT_C CCError_t CC_RsaVerify(
                            CCRsaPubUserContext_t *UserContext_ptr,   /*!< [in]  Pointer to a temporary context for internal use. */
                            CCRsaUserPubKey_t *UserPubKey_ptr,           /*!< [in]  Pointer to the public key data structure of the user. */
                            CCRsaHashOpMode_t rsaHashMode,               /*!< [in]  One of the supported SHA-x HASH modes, as defined in ::CCRsaHashOpMode_t.
                                            (MD5 is not supported). */
                            CCPkcs1Mgf_t MGF,                            /*!< [in]  The mask generation function. Public-Key Cryptography Standards (PKCS) #1 RSA
                                                Cryptography Specifications Version 2.1 defines only MGF1, so the only
                                            value allowed for it is CC_PKCS1_MGF1. */
                            size_t      SaltLen,                         /*!< [in]  The Length of the Salt buffer. Relevant only for Public-Key Cryptography Standards
                                            (PKCS) #1 RSA Cryptography Specifications Version 2.1.
                                            Typical lengths are 0 or hash Len (20 for SHA-1).
                                                                                    The maximum length allowed is [modulus size - hash Len - 2]. */
                            uint8_t     *DataIn_ptr,                     /*!< [in]  Pointer to the input data to be verified.
                                                                                    The size of the scatter/gather list representing the data buffer is
                                            limited to 128 entries, and the size of each entry is limited to 64KB
                                                (fragments larger than 64KB are broken into fragments <= 64KB). */
                            size_t      DataInSize,                      /*!< [in]  The size (in bytes) of the data whose signature is to be verified. */
                            uint8_t     *Sig_ptr,                        /*!< [in]  Pointer to the signature to be verified.
                                                                                    The length of the signature is PubKey_ptr->N.len bytes
                                            (i.e. the modulus size in bytes). */
                            CCPkcs1Version_t PKCS1_ver                   /*!< [in]  Public-Key Cryptography Standards (PKCS) #1: RSA Encryption Standard Version 1.5 or
                                            Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version
                                            2.1, according to the functionality required. */
);
/*!
@brief CRYS_RSA_PKCS1v15_Verify implements the Public-Key Cryptography Standards (PKCS) #1: RSA Encryption
Standard Version 1.5 Verify algorithm.
*/
#define CC_RsaPkcs1V15Verify(UserContext_ptr,UserPubKey_ptr,hashFunc,DataIn_ptr,DataInSize,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,hashFunc,CC_PKCS1_NO_MGF,0,DataIn_ptr,DataInSize,Sig_ptr,CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Md5Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes the DataIn_ptr data has already been hashed using MD5.
*/

#define CC_RsaPkcs1V15Md5Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_MD5_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_MD5_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)


/*!
@brief CC_RsaPkcs1V15Sha1Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes that the DataIn_ptr data has already been hashed using SHA1.

*/
#define CC_RsaPkcs1V15Sha1Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA1_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Sha224Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes that the DataIn_ptr data has already been hashed using SHA224.

*/
#define CC_RsaPkcs1V15Sha224Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA224_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Sha256Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes that the DataIn_ptr data has already been hashed using SHA256.

*/
#define CC_RsaPkcs1V15Sha256Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA256_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Sha384Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes that the DataIn_ptr data has already been hashed using SHA384.

*/
#define CC_RsaPkcs1V15Sha384Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA384_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)

/*!
@brief CC_RsaPkcs1V15Sha512Verify implements the RSASSA-PKCS1v15 Verify algorithm as defined in PKCS#1 v1.5, but without operating the HASH function -
it assumes that the DataIn_ptr data has already been hashed using SHA512.

*/
#define CC_RsaPkcs1V15Sha512Verify(UserContext_ptr,UserPubKey_ptr,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA512_mode,CC_PKCS1_NO_MGF,0,DataIn_ptr,CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER15)

/*!
@brief CC_RsaPssVerify implements the RSASSA-PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1.
*/

#define CC_RsaPssVerify(UserContext_ptr,UserPubKey_ptr,hashFunc,MGF,SaltLen,DataIn_ptr,DataInSize,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,hashFunc,MGF,SaltLen,DataIn_ptr,DataInSize,Sig_ptr,CC_PKCS1_VER21)

/*!
@brief CC_RsaPssSha1Verify implements the PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1, but without operating the HASH function -
it assumes the DataIn_ptr has already been hashed using SHA1.
*/

#define CC_RsaPssSha1Verify(UserContext_ptr,UserPubKey_ptr,MGF,SaltLen,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA1_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER21)
/*!
@brief CC_RsaPssSha224Verify implements the PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1, but without operating the HASH function -
it assumes the DataIn_ptr has already been hashed using SHA224.
*/

#define CC_RsaPssSha224Verify(UserContext_ptr,UserPubKey_ptr,MGF,SaltLen,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA224_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER21)

/*!
@brief CC_RsaPssSha256Verify implements the PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1, but without operating the HASH function -
it assumes the DataIn_ptr has already been hashed using SHA256.
*/

#define CC_RsaPssSha256Verify(UserContext_ptr,UserPubKey_ptr,MGF,SaltLen,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA256_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha384Verify implements the PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1, but without operating the HASH function -
it assumes the DataIn_ptr has already been hashed using SHA384.

*/

#define CC_RsaPssSha384Verify(UserContext_ptr,UserPubKey_ptr,MGF,SaltLen,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA384_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER21)


/*!
@brief CC_RsaPssSha512Verify implements the PKCS1v21 Verify algorithm as defined in PKCS#1 v2.1, but without operating the HASH function -
it assumes the DataIn_ptr has already been hashed using SHA512.
*/

#define CC_RsaPssSha512Verify(UserContext_ptr,UserPubKey_ptr,MGF,SaltLen,DataIn_ptr,Sig_ptr)\
    CC_RsaVerify(UserContext_ptr,UserPubKey_ptr,CC_RSA_After_SHA512_mode,MGF,SaltLen,DataIn_ptr,CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES,Sig_ptr,CC_PKCS1_VER21)

/**********************************************************************************************************/


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
#endif /* _CC_RSA_SCHEMES_H */
