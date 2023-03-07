/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PROD_CRYPTO_DRIVER_H
#define _PROD_CRYPTO_DRIVER_H

#include "cc_cmpu.h"
#include "cc_otp_defs.h"
#include "driver_defs.h"
#include "aes_driver.h"

/* The AES block size in words and in bytes */
#define CC_PROD_AES_BLOCK_SIZE_IN_WORDS     4
#define CC_PROD_AES_BLOCK_SIZE_IN_BYTES  (CC_PROD_AES_BLOCK_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)

/* The size of the IV or counter buffer */
#define CC_PROD_AES_IV_COUNTER_SIZE_IN_WORDS   CC_PROD_AES_BLOCK_SIZE_IN_WORDS
#define CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES  (CC_PROD_AES_IV_COUNTER_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)

#define CC_PROD_AES_Key256Bits_SIZE_IN_WORDS    8
#define CC_PROD_AES_Key256Bits_SIZE_IN_BYTES    (CC_PROD_AES_Key256Bits_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE)


uint32_t CC_PROD_AesCcmDecrypt(uint8_t *pKey,
                     uint32_t  keySizeInBytes,
                     uint8_t *pNonce,
                     uint32_t  nonceSizeInBytes,
                        uint8_t *pAddData,
                        uint32_t  addDataSizeInBytes,
                     uint8_t *pCipherData,
                     uint32_t  dataSize,
                     uint8_t *pPlainBuff,
                               uint32_t  tagSize,
                            uint8_t *pTagBuff,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize);

uint32_t CC_PROD_Aes(aesMode_t cipherMode,
               cryptoDirection_t encDecDir,
               cryptoKeyType_t keyType,
               uint8_t *pKey,
               uint32_t keySize,
               uint8_t *pIv,
               uint32_t ivSize,
               uint32_t *pDataIn,
               uint32_t  dataInSize,
               uint32_t *pOutbuff);

uint32_t CC_PROD_AesInit(AesContext_t *pAesCtx,
                         aesMode_t cipherMode,
               cryptoDirection_t encDecDir,
               cryptoKeyType_t keyType,
               uint8_t *pKey,
               uint32_t keySize,
               uint8_t *pIv,
               uint32_t ivSize);

uint32_t CC_PROD_AesProcess(AesContext_t *pAesCtx,
                  uint32_t *pDataIn,
                  uint32_t  dataInSize,
                  uint32_t *pOutbuff);

uint32_t  CC_PROD_KeyDerivation( cryptoKeyType_t             keyType,
                                    uint8_t                 *pUserKey,
                                    const uint8_t               *pLabel,
                                    size_t                      labelSize,
                                    const uint8_t               *pContextData,
                                    size_t                      contextSize,
                                    uint8_t                     *pDerivedKey);

#endif  //_PROD_CRYPTO_DRIVER_H

