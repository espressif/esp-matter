/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CRYPTO_DRIVER_DEFS_H
#define _CRYPTO_DRIVER_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types_plat.h"
#include "cc_sec_defs.h"
#include "cc_crypto_defs.h"
/******************************************************************************
*                           DEFINITIONS
******************************************************************************/
#define SB_SET_CLK_ENABLE_VAL       0x1UL
#define SB_SET_CLK_DISABLE_VAL      0x0UL

#define SB_CRYPTO_CTL_AES_MODE      0x1UL
#define SB_CRYPTO_CTL_AES_TO_HASH_MODE  0xAUL
#define SB_CRYPTO_CTL_HASH_MODE     0x7UL

#define SB_AES_ENCRYPT          0x0UL
#define SB_AES_DECRYPT          0x1UL

#define SB_HASH_CTL_SHA256_VAL      0x2UL
#define SB_AES_CTRL_MODE_BIT_SHIFT      2

#define SB_HASH_BLOCK_SIZE_BYTES    64

typedef enum {
    CC_SB_ECB_MODE      = 0,
    CC_SB_CBC_MODE      = 1,
    CC_SB_CTR_MODE      = 2,
    CC_SB_CBC_MAC_MODE  = 3,
    CC_SB_CMAC_MODE     = 7,
    CC_SB_MAX_MODES     = 0x7FFFFFFFUL

}CCSbAesModes;


/* Poll on the DOUT DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_DOUT_DMA_BUSY(hwBaseAddress)                      \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, DOUT_MEM_DMA_BUSY), regVal);   \
        }while( regVal );                               \
    }while(0)

/* Poll on the DIN DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_DIN_DMA_BUSY(hwBaseAddress)                       \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, DIN_MEM_DMA_BUSY), regVal);    \
        }while( regVal );                               \
    }while(0)

/* Poll on the DOUT DMA busy till it is = 0 */
#define CC_SB_WAIT_ON_CRYPTO_BUSY(hwBaseAddress)                        \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, CRYPTO_BUSY), regVal);     \
        }while( regVal );                               \
    }while(0)

/* Poll on the AES busy till it is = 0 */
#define CC_SB_WAIT_ON_AES_BUSY()\
    do {\
        uint32_t regVal=1;\
            do {\
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, AES_BUSY), regVal);    \
                }while( regVal );\
        }while(0)

/* Poll on the HASH busy till it is = 0 */
#define CC_SB_WAIT_ON_HASH_BUSY()\
    do {\
        uint32_t regVal=1;\
            do {\
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, HASH_BUSY), regVal);   \
                }while( regVal );\
        }while(0)

/* Use constant counter ID and AXI ID */
#define SB_COUNTER_ID   0


/* The AES block size in words and in bytes */
#define AES_BLOCK_SIZE_IN_WORDS 4

/* The size of the IV or counter buffer */
#define AES_IV_COUNTER_SIZE_IN_WORDS   AES_BLOCK_SIZE_IN_WORDS
#define AES_IV_COUNTER_SIZE_IN_BYTES  (AES_IV_COUNTER_SIZE_IN_WORDS * sizeof(uint32_t))

/* The size of the AES KEY in words and bytes */
#define AES_KEY_SIZE_IN_WORDS AES_BLOCK_SIZE_IN_WORDS
#define AES_KEY_SIZE_IN_BYTES (AES_KEY_SIZE_IN_WORDS * sizeof(uint32_t))

#define AES_Key128Bits_SIZE_IN_WORDS    AES_BLOCK_SIZE_IN_WORDS
#define AES_Key128Bits_SIZE_IN_BYTES    AES_BLOCK_SIZE_IN_BYTES
#define AES_Key256Bits_SIZE_IN_WORDS    8
#define AES_Key256Bits_SIZE_IN_BYTES    (AES_Key256Bits_SIZE_IN_WORDS * sizeof(uint32_t))


/* Hash IV+Length */
#define HASH_DIGEST_SIZE_IN_WORDS   8
#define HASH_DIGEST_SIZE_IN_BYTES   (HASH_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))
#define HASH_LENGTH_SIZE_IN_WORDS   4
#define HASH_LENGTH_SIZE_IN_BYTES   (HASH_LENGTH_SIZE_IN_WORDS * sizeof(uint32_t))


/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/


#define NVM_HASH_Result_t CCHashResult_t

/* Defines the IV counter buffer  - 16 bytes array */
typedef uint32_t AES_Iv_t[AES_IV_COUNTER_SIZE_IN_WORDS];

/* Defines the AES key buffer */
typedef uint32_t AES_Key_t[AES_KEY_SIZE_IN_WORDS];

/* Defines the AES CMAC output result */
typedef uint8_t AES_CMAC_RESULT_t[AES_BLOCK_SIZE_IN_BYTES];


typedef enum {
    CRYPTO_DRIVER_HASH_MODE     = 0,
    CRYPTO_DRIVER_AES_CTR_MODE  = 1,
    CRYPTO_DRIVER_AES_CTR_TO_HASH_MODE = 2,
    CRYPTO_DRIVER_AES_CMAC_MODE = 3
}CryptoDriverMode_t;


/* enum definitons for crypto operation completion mode */
typedef enum
{
    CC_SB_CRYPTO_COMPLETION_NO_WAIT = 0,
    CC_SB_CRYPTO_COMPLETION_NO_WAIT_ASK_ACK = 1,
    CC_SB_CRYPTO_COMPLETION_WAIT_UPON_START = 2,
    CC_SB_CRYPTO_COMPLETION_WAIT_UPON_END = 3
} CCSbCryptoCompletionMode_t;



/******************************************************************************
*                HW engines related definitions
******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif



