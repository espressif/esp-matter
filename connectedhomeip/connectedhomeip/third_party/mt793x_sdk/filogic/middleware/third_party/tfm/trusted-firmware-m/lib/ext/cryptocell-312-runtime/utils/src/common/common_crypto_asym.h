/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_CRYPTO_ASYM_H
#define _COMMON_CRYPTO_ASYM_H


#include <stdint.h>

#define RSA_USE_PKCS_21_VERSION  0x01
#define RSA_USE_PKCS_15_VERSION  0x02

#define RSA_SALT_LEN             32
/**
 * @brief Verifies RSA signature.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[in] pPemEncryptedFileName - the private key file
 * @param[in] pKeyPwd - the passphrase string
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t CC_CommonRsaVerify(int32_t  pkcsVersion,
             int8_t *pPubKey,
             int8_t *pDataIn,
             int32_t  dataInSize,
             int8_t *pSignature);


/**
 * @brief The CC_CommonRsaSign generates RSA signature and returns it.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] DataIn_ptr - the data to sign on
 * @param[in] DataInSize - the data size
 * @param[in] PemEncryptedFileName_ptr - the private key file
 * @param[in] Key_ptr - the passphrase string
 * @param[out] Signature_ptr - the RSA signature
 *
 */
/*********************************************************/
int32_t CC_CommonRsaSign(int32_t pkcsVersion,
               int8_t *DataIn_ptr,
               uint32_t  DataInSize,
               int8_t *PemEncryptedFileName_ptr,
               int8_t *Key_ptr,
               int8_t *Signature_ptr);



/**
 * @brief Encrypts data using RSA.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pPemEncryptedFileName - the private key file
 * @param[in] pKeyPwd - the passphrase string
 * @param[in] pDataIn - the data to encrypt
 * @param[in] dataInSize - the data size
 * @param[out] pEncData - the encrypted data
 *
 */
/*********************************************************/
int32_t CC_CommonRsaEncrypt(int32_t pkcsVersion,
              int8_t *pPubKey,
              int8_t *pDataIn,
              int32_t  dataInSize,
              int8_t *pEncData);


/**
 * @brief Decrypts data using RSA.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pPemEncryptedFileName - the private key file
 * @param[in] pKeyPwd - the passphrase string
 * @param[in] pEnDataIn - the data to decrypt
 * @param[in] enDataInSize - the encrypted data size
 * @param[out] pData - the decrypted data
 *
 */
/*********************************************************/
int32_t CC_CommonRsaDecrypt(int32_t pkcsVersion,
              int8_t *pPemEncryptedFileName,
              int8_t *pKeyPwd,
              int8_t *pEnDataIn,
              int32_t  enDataInSize,
              int8_t *pData);


/**
* @brief The function CC_CommonRsaCalculateH calculates the H it returns it as binary string
*
* @param[in] N_ptr - public key N, represented as array of ascii's (0xbc is translated
*                    to 0x62 0x63)
* @param[out] H_ptr - The H result. H size is N_SIZE_IN_BYTES*2 + 1
*
*/
/*********************************************************/
int32_t CC_CommonRsaCalculateH(const int8_t *N_ptr, int8_t *H_ptr);




/**
* @brief The CC_CommonRsaLoadKey reads RSA key from the file using passphrase
*        and returns its decrypted value.
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] Key_ptr - passphrase
*/
/*********************************************************/
int32_t CC_CommonRsaLoadKey(int8_t *PemEncryptedFileName_ptr, int8_t *Key_ptr, int8_t *PemDecryted);


/**
* @brief The CC_CommonRandBytes reads RSA key from the file using passphrase
*        and returns its decrypted value.
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] Key_ptr - passphrase
*/
/*********************************************************/
int32_t CC_CommonRandBytes(int32_t numBytes, int8_t *buf);


#endif
