/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _BSV_CRYPTO_API_H
#define _BSV_CRYPTO_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains cryptographic ROM APIs : SHA256, CMAC KDF, and CCM.
*/

#include "cc_pal_types.h"
#include "bsv_crypto_defs.h"
#include "cc_sec_defs.h"

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
@brief This function calculates SHA256 digest over contiguous memory in an integrated operation.

@return CC_OK on success.
@return A non-zero value from bsv_error.h on failure.
*/
CCError_t CC_BsvSHA256(
    unsigned long   hwBaseAddress,      /*!< [in] CryptoCell HW registers' base address. */
    uint8_t     *pDataIn,       /*!< [in] Pointer to the input data to be HASHed. Buffer must be contiguous. */
    size_t      dataSize,       /*!< [in] The size of the data to be hashed in Bytes. Limited to 64KB. */
    CCHashResult_t  hashBuff        /*!< [out] Pointer to a word-aligned 32 Byte buffer. */
    );

/*!
@brief  The key derivation function is as specified in the "KDF in Counter Mode" section of
    NIST Special Publication 800-108: Recommendation for Key Derivation Using Pseudorandom Functions.
    Key derivation is based on length l, label L, context C and derivation key Ki.
        AES-CMAC is used as the pseudorandom function (PRF).
\note   When using this API the label and context for each use-case must be well defined.
\note   We recommend to derive only 256-bit keys from HUK or 256-bit user keys.

@return CC_OK on success.
@return A non-zero value from bsv_error.h on failure.
*/

/*  A key derivation functions can iterates n times until l bits of keying material are generated.
        For each of the iteration of the PRF, i=1 to n, do:
        result(0) = 0;
        K(i) = PRF (Ki, [i] || Label || 0x00 || Context || length);
        results(i) = result(i-1) || K(i);

        concisely, result(i) = K(i) || k(i-1) || .... || k(0)*/
CCError_t CC_BsvKeyDerivation(
    unsigned long       hwBaseAddress,  /*!< [in] CryptoCell HW registers' base address. */
    CCBsvKeyType_t      keyType,    /*!< [in] One of the following key types used as an input to a key derivation function:
                              <ul><li>HUK</li>
                              <ul><li>Krtl</li>
                              <ul><li>KCP</li>
                              <ul><li>KPICV</li>
                              <ul><li>128-bit User key</li>
                              <ul><li>256-bit User Key.</li></ul> */
    uint32_t            *pUserKey,  /*!< [in] A pointer to the user's key buffer. */
    size_t              userKeySize,    /*!< [in] The user key size in Bytes (limited to 16Bytes or 32Bytes). */
    const uint8_t       *pLabel,    /*!< [in] A string that identifies the purpose for the derived keying material.*/
    size_t              labelSize,  /*!< [in] The label size. Must be in range of 1 to 8 Bytes in length. */
    const uint8_t       *pContextData,  /*!< [in] A binary string containing the information related to the derived keying material. */
    size_t              contextSize,    /*!< [in] The context size should be in range of 1 to 32 Bytes in length. */
    uint8_t         *pDerivedKey,   /*!< [out] Keying material output. Must be at least the size of derivedKeySize. */
    size_t          derivedKeySize  /*!< [in] Size of the derived keying material in Bytes. Limited to 128bits or 256bits. */
    );


/*!
@brief This API allows a limited AES-CCM decrypt and verify operation, needed for AES-CCM verification during boot.
AES-CCM combines counter mode encryption with CBC-MAC authentication.
Input to CCM includes the following elements:
<ul><li> Payload - text data that is both decrypted and verified.</li>
<li> Associated data (Adata) - data that is authenticated but not encrypted, e.g., a header.</li>
<li> Nonce - A unique value that is assigned to the payload and the associated data.</li></ul>

@return CC_OK on success.
@return A non-zero value on failure as defined bsv_error.h.
*/
CCError_t CC_BsvAesCcm(
    unsigned long       hwBaseAddress,      /*!< [in] CryptoCell HW registers' base address. */
    CCBsvCcmKey_t       keyBuf,             /*!< [in] Pointer to the 128bit AES-CCM key. */
    CCBsvCcmNonce_t     nonceBuf,               /*!< [in] Pointer to the 12 Byte Nonce. */
    uint8_t                 *pAssocData,        /*!< [in] Pointer to the associated data. The buffer must be contiguous. */
    size_t                  assocDataSize,          /*!< [in] Byte size of the associated data limited to (2^16-2^8) bytes. */
    uint8_t                 *pTextDataIn,       /*!< [in] Pointer to the cipher-text data for decryption. The buffer must be contiguous. */
    size_t                  textDataSize,       /*!< [in] Byte size of the full text data limited to 64KB. */
    uint8_t                 *pTextDataOut,      /*!< [out] Pointer to the output (plain text data). The buffer must be contiguous. */
    CCBsvCcmMacRes_t        macBuf              /*!< [in] Pointer to the MAC result buffer. */
);


#ifdef __cplusplus
}
#endif

#endif



