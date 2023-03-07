/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_SEC.h
 *
 * Contains all security functionality of the HAL
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_GP_SEC_H_
#define _HAL_GP_SEC_H_

/** @file gpHal_SEC.h
 *  This file contains all security functionality of the HAL.  Standalone AES encryption can be performed as well as CCM encryption and decryption.
 *
 *  @brief Contains all security functionality of the HAL
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpEncryption.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Performs a synchronous AES Encryption.
 *
 * The function will encrypt the number of bytes specified in keylen with the AES algorithm and return the result in place.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *          - gpHal_ResultInvalidParameter
 *
 * @param pInplaceBuffer   Pointer to the buffer of the 16 to be encrypted bytes. Encrypted result will be returned in same buffer
 * @param pAESKey          Pointer to the byte key, this key is only uses if specified by the options parameter. When used but specified as NULL, 0 will be used as key value.
 * @param AESOptions          This parameter specifies the keylen and an 8bit bitmask specifying the options: bits[6:0] specify the keyid to be used (see gpEncryption_API_Manual); bit[7] indicates additional hardening
 */
GP_API gpHal_Result_t gpHal_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAESKey, gpEncryption_AESOptions_t AESOptions);

/**
 * @brief Performs a synchronous CCM Encryption with support for larger buffers.
 *
 * The function will encrypt the bytes with the CCM algorithm according to the specified options
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidRequest
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
 */
gpHal_Result_t gpHal_CCMEncrypt_RAM(UInt16 dataLength, UInt16 auxLength, UInt8 micLength, UInt8* dataPtr, UInt8* auxPtr, UInt8* micPtr, UInt8* pKey, UInt8* pNonce, UInt8* dataOutPtr);

/**
 * @brief Performs a synchronous CCM Decryption with support for larger buffers.
 *
 * The function will decrypt the bytes with the CCM algorithm according to the specified options
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidRequest
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
 */
gpHal_Result_t gpHal_CCMDecrypt_RAM(UInt16 dataLength, UInt16 auxLength, UInt8 micLength, UInt8* dataPtr, UInt8* auxPtr, UInt8* micPtr, UInt8* pKey, UInt8* pNonce, UInt8* dataOutPtr);

/**
 * @brief Performs a synchronous CCM Encryption.
 *
 * The function will encrypt the bytes with the CCM algorithm according to the specified options
 * in the gpHal_CCMOptions structure.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
 */
GP_API gpHal_Result_t gpHal_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions);

/**
 * @brief Performs a synchronous CCM Decryption.
 *
 * The function will decrypt the bytes with the CCM algorithm according to the specified options
 * in the gpHal_CCMOptions structure.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *          - gpHal_ResultInvalidParameter
 *
 * @param pCCMOptions        Pointer to the gpHal_CCMOptions structure.
 */
GP_API gpHal_Result_t gpHal_CCMDecrypt(gpEncryption_CCMOptions_t * pCCMOptions);

/**
 * @brief Hash-based message authentication code.
 *
 * This function performs HMAC, a mechanism for message authentication using cryptographic hash functions.
 *
 * Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultBusy
 *          - gpHal_ResultUnsupported
 *
 * @param hashFct        Type of hash funtion.
 * @param pKey           Secret cryptographic Key.
 * @param pMsg           Message for authentication.
 * @param pResult        Message Authentication Codes Result, The size of the output of HMAC is the same as that of the underlying hash function.
 */
GP_API gpHal_Result_t gpHal_HMAC(UInt8 hashFct, UInt16 keyLength, UInt16 msgLength, UInt8 resultLength, UInt8* pKey, UInt8* pMsg, UInt8* pResult);

#if defined(GP_DIVERSITY_GPHAL_K8E)
/**
 * @brief Performs an AES MMO hash generation
 *
 * The function will generate a fixed length digest value for a given input message
 *
 * @param compressedDataPtr   Pointer to the message buffer in compressed address map for which digest has to be generated
 * @param compressedKeyPtr    Pointer to the key buffer in compressed address map (generated digest value is stored in this buffer)
 * @param keylen              Key length
 * @param msgLengthBytes      Input message length in bytes
 */
void gpHalSec_SspAesMMO(UInt32 compressedDataPtr,
    UInt32 compressedKeyPtr,
    gpEncryption_AESKeyLen_t keylen,
    UInt8 msgLengthBytes);
#endif



#ifdef __cplusplus
}
#endif

#endif //_HAL_GP_SEC_H_
