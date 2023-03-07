/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _BSV_CRYPTO_DRIVER_H
#define _BSV_CRYPTO_DRIVER_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_sec_defs.h"
#include "bsv_crypto_defs.h"

/*! @file
@brief This file contains crypto driver definitions: SH256, CMAC KDF, and CCM.
*/

/************************ Defines ******************************/

/*! SHA256 digest result in words. */
#define CC_BSV_SHA256_DIGEST_SIZE_IN_WORDS  8
/*! SHA256 digest result in bytes. */
#define CC_BSV_SHA256_DIGEST_SIZE_IN_BYTES  CC_BSV_SHA256_DIGEST_SIZE_IN_WORDS*sizeof(uint32_t)

/*! SHA256 maximal data size to be hashed */
#define CC_BSV_SHA256_MAX_DATA_SIZE_IN_BYTES    0x00010000 /* 64KB */


/*! The derived key size for 128 bits. */
#define CC_BSV_128BITS_KEY_SIZE_IN_BYTES    16

/*! The derived key size for 256 bits. */
#define CC_BSV_256BITS_KEY_SIZE_IN_BYTES    32

/*! Maximal label length in bytes. */
#define CC_BSV_MAX_LABEL_LENGTH_IN_BYTES    8

/*! Maximal context length in bytes. */
#define CC_BSV_MAX_CONTEXT_LENGTH_IN_BYTES  32

/*! KDF 128 bits key fixed data size in bytes. */
#define CC_BSV_KDF_DATA_128BITS_SIZE_IN_BYTES   3 /*!< \internal 0x01, 0x00, lengt(-0x80) */
/*! KDF 256 bits key fixed data size in bytes. */
#define CC_BSV_KDF_DATA_256BITS_SIZE_IN_BYTES   4 /*!< \internal 0x02, 0x00, lengt(-0x0100) */

/*! KDF data maximal size in bytes. */
#define CC_BSV_KDF_MAX_SIZE_IN_BYTES (CC_BSV_KDF_DATA_256BITS_SIZE_IN_BYTES + CC_BSV_MAX_LABEL_LENGTH_IN_BYTES + CC_BSV_MAX_CONTEXT_LENGTH_IN_BYTES)


/*! Maximal AES CCM associated data size in bytes. */
#define CC_BSV_CCM_MAX_ASSOC_DATA_SIZE_IN_BYTES         0xff00  /* 2^16-2^8 */

/*! Maximal AES CCM text data size in bytes. */
#define CC_BSV_CCM_MAX_TEXT_DATA_SIZE_IN_BYTES      0x00010000  /* 64KB */

/*! AES block size in bytes. */
#define BSV_AES_BLOCK_SIZE_IN_BYTES 16
/*! AES IV size in bytes. */
#define BSV_AES_IV_SIZE_IN_BYTES        16
/*! AES IV size in words. */
#define BSV_AES_IV_SIZE_IN_WORDS        4

/*! HASH SHA256 control value. */
#define BSV_HASH_CTL_SHA256_VAL         0x2UL
/*! HASH SHA256 padding configuration. */
#define BSV_HASH_PAD_CFG_VAL            0x4UL

/************************ Typedefs  *****************************/
/*! Definitions of cryptographic mode. */
typedef enum bsvCryptoMode {
    /*! AES.*/
    BSV_CRYPTO_AES = 1,
    /*! AES and HASH.*/
    BSV_CRYPTO_AES_AND_HASH = 3,
    /*! HASH.*/
    BSV_CRYPTO_HASH = 7,
    /*! AES to HASH and to DOUT.*/
    BSV_CRYPTO_AES_TO_HASH_AND_DOUT = 10,
    /*! Reserved.*/
    BSV_CRYPTO_RESERVE32B = INT32_MAX
}bsvCryptoMode_t;

/*! Definitions for AES modes. */
typedef enum bsvAesMode {
    /*! AES CTR mode.*/
    BSV_AES_CIPHER_CTR = 2,
    /*! AES CBC MAC mode.*/
    BSV_AES_CIPHER_CBC_MAC = 3,
    /*! AES CMAC mode.*/
    BSV_AES_CIPHER_CMAC = 7,
    /*! AES CCM PE mode.*/
    BSV_AES_CIPHER_CCMPE = 9,
    /*! AES CCM PD mode.*/
    BSV_AES_CIPHER_CCMPD = 10,
    /*! Reserved.*/
    BSV_AES_CIPHER_RESERVE32B = INT32_MAX
}bsvAesMode_t;

/*! Definitions for AES directions. */
typedef enum bsvAesDirection {
    /*! Encrypt.*/
    BSV_AES_DIRECTION_ENCRYPT = 0,
    /*! Decrypt.*/
    BSV_AES_DIRECTION_DECRYPT = 1,
    /*! Reserved.*/
    BSV_AES_DIRECTION_RESERVE32B = INT32_MAX
}bsvAesDirection_t;

/*! Defintions for AES key sizes. */
typedef enum bsvAesKeySize {
    /*! 128 bits AES key. */
    BSV_AES_KEY_SIZE_128BITS = 0,
    /*! 256 bits AES key. */
    BSV_AES_KEY_SIZE_256BITS = 2,
    /*! Reserved.*/
    BSV_AES_KEY_SIZE_RESERVE32B = INT32_MAX
}bsvAesKeySize_t;

/***************************** function declaration **************************/


CCError_t BsvAes(unsigned long          hwBaseAddress,
            bsvAesMode_t        mode,
            CCBsvKeyType_t          keyType,
            uint32_t            *pUserKey,
            size_t              userKeySize,
            uint32_t        *pIvBuf,
            uint8_t         *pDataIn,
            uint8_t         *pDataOut,
            size_t                  dataSize,
            CCBsvCmacResult_t   cmacResBuf);

CCError_t BsvCryptoImageInit( unsigned long     hwBaseAddress,
            bsvCryptoMode_t     mode,
            CCBsvKeyType_t      keyType);

CCError_t BsvCryptoImageUpdate( unsigned long   hwBaseAddress,
            bsvCryptoMode_t     mode,
            CCBsvKeyType_t          keyType,
            uint32_t        *pCtrStateBuf,
            uint8_t         *pDataIn,
            uint8_t         *pDataOut,
            size_t                  dataSize,
            CCHashResult_t          hashBuff,
            uint8_t         isLoadIV);

CCError_t BsvCryptoImageFinish( unsigned long   hwBaseAddress,
            bsvCryptoMode_t     mode,
            CCHashResult_t          hashBuff);


/* SHA256 */
void InitBsvHash(unsigned long hwBaseAddress);
void FreeBsvHash(unsigned long hwBaseAddress);
CCError_t ProcessBsvHash(unsigned long hwBaseAddress, uint32_t inputDataAddr, uint32_t dataInSize);
void FinishBsvHash(unsigned long hwBaseAddress, CCHashResult_t  HashBuff);


/* AES (CTR, CMAC ) */

void InitBsvAes(unsigned long hwBaseAddress);
void FreeBsvAes(unsigned long hwBaseAddress);
CCError_t ProcessBsvAes(unsigned long   hwBaseAddress,
        bsvAesMode_t        mode,
        CCBsvKeyType_t      keyType,
        uint32_t            *pUserKey,
        size_t              userKeySize,
        uint32_t        *pCtrStateBuf,
        uint32_t        inputDataAddr,
        uint32_t        outputDataAddr,
        uint32_t        blockSize,
        uint8_t         isLoadIv);

void FinishBsvAes(unsigned long hwBaseAddress,
    bsvAesMode_t        mode,
    CCBsvCmacResult_t   cmacResBuf);

/* AES-CCM */

CCError_t ProcessBsvAesCcm(unsigned long    hwBaseAddress,
            bsvAesMode_t    mode,
            uint32_t    *pKeyBuf,
            uint32_t    *pIvBuf,
            uint32_t    *pCtrStateBuf,
            uint32_t    inputDataAddr,
            uint32_t    outputDataAddr,
            uint32_t    blockSize);

void FinishBsvAesCcm(unsigned long hwBaseAddress,
        bsvAesMode_t    mode,
        uint32_t    *pIvBuf,
        uint32_t    *pCtrStateBuf);

#ifdef __cplusplus
}
#endif

#endif


