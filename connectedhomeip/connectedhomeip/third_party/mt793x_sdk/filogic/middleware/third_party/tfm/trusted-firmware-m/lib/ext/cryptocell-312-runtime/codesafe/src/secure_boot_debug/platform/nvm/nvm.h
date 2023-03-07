/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NVM__H
#define _NVM__H

#ifdef __cplusplus
extern "C"
{
#endif


/*------------------------------------
    DEFINES
-------------------------------------*/

/**
 * @brief This function reads the LCS from the SRAM/NVM
 *
 * @param[in] hwBaseAddress  -  CryptoCell base address
 *
 * @param[in/out] lcs_ptr  - pointer to memory to store the LCS
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_GetLCS(unsigned long hwBaseAddress, uint32_t *lcs_ptr);

/**
 * @brief The NVM_ReadHASHPubKey function is a NVM interface function -
 *        The function retrieves the HASH of the device Public key from the SRAM/NVM
 *
 * @param[in] hwBaseAddress -  CryptoCell base address
 *
 * @param[in] pubKeyIndex -  Index of HASH in the OTP
 *
 * @param[out] PubKeyHASH   -  the public key HASH.
 *
 * @param[in] hashSizeInWords -  hash size (valid values: 4W, 8W)
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */

CCError_t NVM_ReadHASHPubKey(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t pubKeyIndex, CCHashResult_t PubKeyHASH, uint32_t hashSizeInWords);


/**
 * @brief The NVM_ReadAESKey function is a NVM interface function -
 *        The function retrieves the AES CTR 128 bit key from the NVM
 *
 * @param[in] hwBaseAddress -  CryptoCell base address
 *
 * @param[out] AESKey   -  Kce from OTP for SW image decryption
 *
 * @return CCError_t - On success the value CC_OK is returned, and on failure   -a value from NVM_error.h
 */
CCError_t NVM_ReadAESKey(unsigned long hwBaseAddress, AES_Key_t AESKey);

#ifdef __cplusplus
}
#endif

#endif


