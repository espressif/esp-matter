/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SECURE_BOOT_BASE_FUNC_H
#define _SECURE_BOOT_BASE_FUNC_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_parser_gen_defs.h"


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/


/**
 * @brief This function calculates the HASH over the PubKey (N, big endian) || Np (reversed - little endian).
 *
 * @param[in] hwBaseAddr -  CryptoCell base address
 * @param[in] NAndRevNp_ptr - pointer to N public key and Np in the certificate
 * @param[out] hashResult - a pointer to HASH of the public key
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from BootImagesVerifier_error.h
 */
CCError_t CCSbCalcPublicKeyHASH(unsigned long hwBaseAddress,
                uint32_t *NAndRevNp_ptr,
                uint32_t *hashResult);

/**
 * @brief This function calculates the HASH over the PubKey (N, big endian) || Np (reversed - little endian).
 *        The function gets the Public key pointer and Np (Barrett n value) from the certificate calculates hash on it and
 *    compare it to the HASH from the OTP/NVM.
 *
 * @param[in] hwBaseAddr -  CryptoCell base address
 * @param[in] NAndRevNp_ptr - pointer to N public key and Np in the certificate
 * @param[in] NHASH_ptr - a pointer to HASH of the public key
 * @param[in] HashSize - hash size (to compare)
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from BootImagesVerifier_error.h
 */
CCError_t CCSbCalcPublicKeyHASHAndCompare(unsigned long hwBaseAddress,
                         uint32_t *NAndRevNp_ptr,
                         uint32_t *NHASH_ptr,
                         uint32_t HashSize);

/**
 * @brief This function calculates the HASH over the given data and than verify
 *    RSA signature on that hashed data
 *
 * @param[in] hwBaseAddr -  CryptoCell base address
 * @param[in] pData - pointer to the data to be verified
 * @param[in] pNParams - a pointer to the public key parameters
 * @param[in] pSignature - a pointer to the signature structure
 * @param[in] sizeOfData - size of the data to calculate the HASH on (in bytes)
 * @param[in] sigAlg - signature algorithm to use
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from BootImagesVerifier_error.h
CCError_t CCSbVerifySignature(unsigned long hwBaseAddress,
                uint32_t *pData,
                CCSbNParams_t *pNParams,
                CCSbSignature_t *pSignature,
                uint32_t sizeOfData,
                CCSbSignAlg_t sigAlg);


 */
CCError_t CCSbVerifySignature(unsigned long hwBaseAddress,
                uint32_t *pData,
                CCSbNParams_t *pNParams,
                CCSbSignature_t *pSignature,
                uint32_t sizeOfData,
                CCSbSignAlg_t sigAlg);



#ifdef __cplusplus
}
#endif

#endif


