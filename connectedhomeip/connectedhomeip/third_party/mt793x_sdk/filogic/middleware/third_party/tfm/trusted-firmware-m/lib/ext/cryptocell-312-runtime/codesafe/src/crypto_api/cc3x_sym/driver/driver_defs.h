/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _DRIVER_DEFS_H_
#define _DRIVER_DEFS_H_

#include "cc_pal_types.h"

#ifdef __KERNEL__
#include <linux/types.h>
#define INT32_MAX 0x7FFFFFFFL
#else
#include <stdint.h>
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define CPU_DIN_MAX_SIZE    0xFFFFUL
/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/
typedef uint32_t drvError_t;

typedef enum aesMode {
    CIPHER_NULL_MODE = -1,
    CIPHER_ECB = 0,
    CIPHER_CBC = 1,
    CIPHER_CTR = 2,
    CIPHER_CBC_MAC = 3,
    CIPHER_OFB = 6,
    CIPHER_CMAC = 7,
    CIPHER_CCMA = 8,
    CIPHER_CCMPE = 9,
    CIPHER_CCMPD = 10,
    CIPHER_RESERVE32B = INT32_MAX
}aesMode_t;

typedef enum hashMode {
    HASH_NULL_MODE = -1,
    HASH_SHA1 = 0,
    HASH_SHA256 = 1,
    HASH_SHA224 = 2,
    HASH_SHA512 = 3,
    HASH_SHA384 = 4,
    HASH_RESERVE32B = INT32_MAX
}hashMode_t;

typedef enum DataBlockType {
    FIRST_BLOCK,
    MIDDLE_BLOCK,
    LAST_BLOCK,
    RESERVE32B_BLOCK = INT32_MAX
}DataBlockType_t;

typedef enum dataAddrType {
    SRAM_ADDR = 0,
    DLLI_ADDR = 1,
    ADDR_RESERVE32B = INT32_MAX
}dataAddrType_t;

typedef enum cryptoDirection {
    CRYPTO_DIRECTION_ENCRYPT = 0,
    CRYPTO_DIRECTION_DECRYPT = 1,
    CRYPTO_DIRECTION_NUM_OF_ENC_MODES,
    CRYPTO_DIRECTION_RESERVE32B = INT32_MAX
}cryptoDirection_t;

typedef enum keySizeId {
    KEY_SIZE_128_BIT = 0,
    KEY_SIZE_192_BIT = 1,
    KEY_SIZE_256_BIT = 2,
    KEY_SIZE_ID_RESERVE32B = INT32_MAX,
}keySizeId_t;

typedef enum cryptoKeyType {
    RKEK_KEY = 0,
    USER_KEY = 1,
    KCP_KEY = 2,
    KCE_KEY = 3,
    KPICV_KEY = 4,
    KCEICV_KEY = 5,
    RTL_KEY,
    END_OF_KEYS = INT32_MAX,
}cryptoKeyType_t;

typedef enum cryptoPaddingType {
    CRYPTO_PADDING_NONE = 0,
    CRYPTO_PADDING_PKCS7 = 1,
    CRYPTO_PADDING_RESERVE32B = INT32_MAX
}cryptoPaddingType_t;

typedef enum chachaNonceSize {
        NONCE_SIZE_64 = 0,
        NONCE_SIZE_96 = 1,
        NONCE_SIZE_RESERVE32B = INT32_MAX
}chachaNonceSize_t;

typedef enum hashSelAesMacModule {
    HASH_SEL_HASH_MOD    = 0,
    HASH_SEL_AES_MAC_MOD = 1,
    HASH_SEL_RESERVE32B  = INT32_MAX
}hashSelAesMacModule_t;

typedef enum ghashSelModule {
    GHASH_SEL_HASH_MOD   = 0,
    GHASH_SEL_GHASH_MOD  = 1,
    GHASH_SEL_RESERVE32B = INT32_MAX
}ghashSelModule_t;

/******************************************************************************
*               Buffer Information
******************************************************************************/
/*! User buffer type (input for read / output for write). */
#define INPUT_DATA_BUFFER     1
#define OUTPUT_DATA_BUFFER    0


/*! User buffer buffer information. */
typedef struct {
    uint32_t dataBuffAddr;       /*!< Address of data buffer.*/
    uint8_t  dataBuffNs;         /*!< HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
}CCBuffInfo_t;

/******************************************************************************
*               Driver's Errors base address
******************************************************************************/
#define DRV_MODULE_ERROR_BASE               0x00F00000
#define AES_DRV_MODULE_ERROR_BASE           (DRV_MODULE_ERROR_BASE + 0x10000UL)
#define AEAD_DRV_MODULE_ERROR_BASE          (DRV_MODULE_ERROR_BASE + 0x20000UL)
#define HASH_DRV_MODULE_ERROR_BASE          (DRV_MODULE_ERROR_BASE + 0x30000UL)
#define HMAC_DRV_MODULE_ERROR_BASE          (DRV_MODULE_ERROR_BASE + 0x40000UL)
#define BYPASS_DRV_MODULE_ERROR_BASE        (DRV_MODULE_ERROR_BASE + 0x50000UL)
#define CHACHA_DRV_MODULE_ERROR_BASE        (DRV_MODULE_ERROR_BASE + 0x60000UL)

/******************************************************************************
*               CRYPTOGRAPHIC FLOW DEFINITIONS
******************************************************************************/
#define CONFIG_DIN_AES_DOUT_VAL             0x1UL
#define CONFIG_DIN_AES_AND_HASH_VAL         0x3UL
#define CONFIG_HASH_MODE_VAL                0x7UL
#define CONFIG_AES_TO_HASH_AND_DOUT_VAL     0xAUL

/******************************************************************************
*              Data Buffer Attributes for secure/non-secure
******************************************************************************/
#define OUTPUT_BUFFER_HNONSEC_BIT_SHIFT     0x0UL
#define OUTPUT_BUFFER_HNONSEC_BIT_SIZE      0x1UL
#define INPUT_BUFFER_HNONSEC_BIT_SHIFT      0x1UL
#define INPUT_BUFFER_HNONSEC_BIT_SIZE       0x1UL

/******************************************************************************
*               AES DEFINITIONS
******************************************************************************/

#define AES_BLOCK_SIZE                  16
#define AES_BLOCK_SIZE_WORDS            (AES_BLOCK_SIZE >> 2)
#define AES_IV_SIZE                     16
#define AES_IV_SIZE_WORDS               (AES_IV_SIZE >> 2)
#define AES_128_BIT_KEY_SIZE            16
#define AES_128_BIT_KEY_SIZE_WORDS      (AES_128_BIT_KEY_SIZE >> 2)
#define AES_192_BIT_KEY_SIZE            24
#define AES_192_BIT_KEY_SIZE_WORDS      (AES_192_BIT_KEY_SIZE >> 2)
#define AES_256_BIT_KEY_SIZE            32
#define AES_256_BIT_KEY_SIZE_WORDS      (AES_256_BIT_KEY_SIZE >> 2)


#define SET_CLOCK_ENABLE                0x1UL
#define SET_CLOCK_DISABLE               0x0UL

/* The CC AES file errors */
#define AES_DRV_OK                                      0
#define AES_DRV_INVALID_USER_CONTEXT_POINTER_ERROR      (AES_DRV_MODULE_ERROR_BASE + 0x00UL)
#define AES_DRV_ILLEGAL_OPERATION_MODE_ERROR            (AES_DRV_MODULE_ERROR_BASE + 0x01UL)
#define AES_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR       (AES_DRV_MODULE_ERROR_BASE + 0x02UL)
#define AES_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR            (AES_DRV_MODULE_ERROR_BASE + 0x03UL)
#define AES_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR           (AES_DRV_MODULE_ERROR_BASE + 0x04UL)
#define AES_DRV_ILLEGAL_MEM_SIZE_ERROR                  (AES_DRV_MODULE_ERROR_BASE + 0x05UL)
#define AES_DRV_ILLEGAL_KEY_SIZE_ERROR                  (AES_DRV_MODULE_ERROR_BASE + 0x06UL)
#define AES_DRV_ILLEGAL_KEY_LOCK_ERROR                  (AES_DRV_MODULE_ERROR_BASE + 0x07UL)
#define AES_DRV_ILLEGAL_KEY_INTEGRITY_ERROR             (AES_DRV_MODULE_ERROR_BASE + 0x08UL)
#define AES_DRV_ILLEGAL_KEY_USE_ERROR                   (AES_DRV_MODULE_ERROR_BASE + 0x09UL)
#define AES_DRV_ILLEGAL_FATAL_ERR_BIT_ERROR             (AES_DRV_MODULE_ERROR_BASE + 0x0AUL)
#define AES_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR    (AES_DRV_MODULE_ERROR_BASE + 0x0BUL)


/******************************************************************************
*               GHASH DEFINITIONS
******************************************************************************/
#define GHASH_INIT_SET_VAL                              0x1UL

/******************************************************************************
*               HASH & HMAC DEFINITIONS
******************************************************************************/

/************************ Typedefs  ****************************/
typedef drvError_t (*llf_hash_init_operation_func)(void *);
typedef drvError_t (*llf_hash_update_operation_func)(void *, CCBuffInfo_t *pInputBuffInfo, uint32_t dataInSize);
typedef drvError_t (*llf_hash_finish_operation_func)(void *);


/* The SHA-1 digest result size */
#define SHA1_DIGEST_SIZE_IN_WORDS 5
#define SHA1_DIGEST_SIZE_IN_BYTES (SHA1_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))

/* The SHA-256 digest result size*/
#define SHA224_DIGEST_SIZE_IN_WORDS 7
#define SHA224_DIGEST_SIZE_IN_BYTES (SHA224_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))

/* The SHA-256 digest result size */
#define SHA256_DIGEST_SIZE_IN_WORDS 8
#define SHA256_DIGEST_SIZE_IN_BYTES (SHA256_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))

/* The SHA-384 digest result size*/
#define SHA384_DIGEST_SIZE_IN_WORDS 12
#define SHA384_DIGEST_SIZE_IN_BYTES (SHA384_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))

/* The SHA-512 digest result size in bytes */
#define SHA512_DIGEST_SIZE_IN_WORDS 16
#define SHA512_DIGEST_SIZE_IN_BYTES (SHA512_DIGEST_SIZE_IN_WORDS * sizeof(uint32_t))


#define MAX_DIGEST_SIZE_WORDS       SHA512_DIGEST_SIZE_IN_WORDS

#define HW_HASH_CTL_SHA1_VAL            0x0001UL
#define HW_HASH_CTL_SHA256_VAL          0x0002UL
#define HW_HASH_LE_MODE_VAL         0x0001UL
#define HW_HASH_PAD_EN_VAL          0x1UL

/* The SHA1 hash block size in words */
#define HASH_BLOCK_SIZE_IN_WORDS 16
#define HASH_BLOCK_SIZE_IN_BYTES (HASH_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t))

/* The SHA2 hash block size in words */
#define HASH_SHA512_BLOCK_SIZE_IN_WORDS 32
#define HASH_SHA512_BLOCK_SIZE_IN_BYTES (HASH_SHA512_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t))

/* the MAC key IPAD and OPAD bytes */
#define MAC_KEY_IPAD_BYTE 0x36
#define MAC_KEY_OPAD_BYTE 0x5C

#define HMAC_CONTEXT_VALIDATION_TAG 0x23456789

/* The CC HASH file errors */
#define HASH_DRV_OK                     0
#define HASH_DRV_INVALID_USER_CONTEXT_POINTER_ERROR     (HASH_DRV_MODULE_ERROR_BASE + 0x00UL)
#define HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR           (HASH_DRV_MODULE_ERROR_BASE + 0x01UL)
#define HASH_DRV_USER_CONTEXT_CORRUPTED_ERROR           (HASH_DRV_MODULE_ERROR_BASE + 0x02UL)
#define HASH_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR   (HASH_DRV_MODULE_ERROR_BASE + 0x03UL)

/* The CC HMAC file errors */
#define HMAC_DRV_OK                     0
#define HMAC_DRV_INVALID_USER_CONTEXT_POINTER_ERROR         (HMAC_DRV_MODULE_ERROR_BASE + 0x00UL)


/* SHA512 soft driver */

/* The first padding byte */
#define LLF_HASH_FIRST_PADDING_BYTE 0x80
/* The size at the end of the padding for SHA384 and SHA512 */
#define LLF_HASH_SHA2_COUNTER_SIZE_ON_END_OF_PADDING_IN_BYTES (4 * sizeof(uint32_t))
#define LLF_HASH_SHA2_COUNTER_SIZE_ON_END_OF_PADDING_IN_WORDS 4

/* the HASH user context validity TAG */
#define HASH_CONTEXT_VALIDATION_TAG 0x12345678

/* the HASH XOR data value */
#define HASH_XOR_DATA_VAL               0x0UL

/******************************************************************************
*               BYPASS DEFINITIONS
******************************************************************************/

#define CONFIG_DIN_BYPASS_DOUT_VAL                      0

/* The CC BYPASS file errors */
#define BYPASS_DRV_OK                       0
#define BYPASS_DRV_ILLEGAL_BLOCK_SIZE_ERROR             (BYPASS_DRV_MODULE_ERROR_BASE + 0x01UL)
#define BYPASS_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR         (BYPASS_DRV_MODULE_ERROR_BASE + 0x02UL)
#define BYPASS_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR        (BYPASS_DRV_MODULE_ERROR_BASE + 0x03UL)
#define BYPASS_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR (BYPASS_DRV_MODULE_ERROR_BASE + 0x04UL)

/******************************************************************************
*               CHACHA DEFINITIONS
******************************************************************************/

#define CHACHA_BLOCK_SIZE_BYTES            64
#define CHACHA_BLOCK_SIZE_WORDS            (CHACHA_BLOCK_SIZE_BYTES >> 2)
#define CHACHA_IV_64_SIZE_BYTES            8
#define CHACHA_IV_64_SIZE_WORDS            (CHACHA_IV_64_SIZE_BYTES >> 2)
#define CHACHA_IV_96_SIZE_BYTES            12
#define CHACHA_IV_96_SIZE_WORDS            (CHACHA_IV_96_SIZE_BYTES >> 2)
#define CHACHA_256_BIT_KEY_SIZE            32
#define CHACHA_256_BIT_KEY_SIZE_WORDS       (CHACHA_256_BIT_KEY_SIZE >> 2)

#define ENABLE_CHACHA_CLOCK     0x1UL
#define DISABLE_CHACHA_CLOCK        0x0UL

#define CONFIG_DIN_CHACHA_DOUT_VAL          0x10UL

/* The CC CHACHA file errors */
#define CHACHA_DRV_OK                       0
#define CHACHA_DRV_INVALID_USER_CONTEXT_POINTER_ERROR   (CHACHA_DRV_MODULE_ERROR_BASE + 0x00UL)
#define CHACHA_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR    (CHACHA_DRV_MODULE_ERROR_BASE + 0x01UL)
#define CHACHA_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR         (CHACHA_DRV_MODULE_ERROR_BASE + 0x02UL)
#define CHACHA_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR        (CHACHA_DRV_MODULE_ERROR_BASE + 0x03UL)
#define CHACHA_DRV_ILLEGAL_MEM_SIZE_ERROR               (CHACHA_DRV_MODULE_ERROR_BASE + 0x04UL)
#define CHACHA_DRV_ILLEGAL_NONCE_SIZE_ERROR             (CHACHA_DRV_MODULE_ERROR_BASE + 0x05UL)
#define CHACHA_DRV_INVALID_USER_DATA_BUFF_POINTER_ERROR (CHACHA_DRV_MODULE_ERROR_BASE + 0x06UL)


/******************************************************************************
*               MACROS
******************************************************************************/
/* This MACRO purpose is to switch from CryptoCell definitions to crypto driver definitions, the MACRO assumes that the value is legal (encrypt or decrypt only) */
#define CC_2_DRIVER_DIRECTION(ssiDirection) ((ssiDirection == CC_AES_ENCRYPT) ? (CRYPTO_DIRECTION_ENCRYPT) : (CRYPTO_DIRECTION_DECRYPT))
/* This MACRO purpose is to switch from MBEDTLS definitions to crypto driver definitions, the MACRO assumes that the value is legal (encrypt or decrypt only) */
#define MBEDTLS_2_DRIVER_DIRECTION(mbedtlsDir) ((mbedtlsDir == 1) ? (CRYPTO_DIRECTION_ENCRYPT) : (CRYPTO_DIRECTION_DECRYPT))


/* Poll on the crypto busy till it is = 0 */
#define CC_HAL_WAIT_ON_CRYPTO_BUSY()\
    do {\
        uint32_t regVal=1;\
        do {\
            regVal = CC_HAL_READ_REGISTER( CC_REG_OFFSET(HOST_RGF, CRYPTO_BUSY));\
                }while( regVal );\
        }while(0)



/* check HUK, Kcp, Kce, Kpicv, Kceicv error bit in LCS register */
#define CC_IS_KEY_ERROR(keyType, rc)\
    do {\
                uint32_t regVal = 0; \
        regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, LCS_REG));\
            rc = CC_REG_FLD_GET(0, LCS_REG, ERROR_## keyType ##_ZERO_CNT, regVal);\
                rc = (rc == 1)?CC_TRUE:CC_FALSE;\
    }while(0)


/* check if key is locked for Kpicv, Kceicv, Kcp, Kce */
#define CC_IS_KEY_LOCKED(keyType, rc)\
    do {\
            uint32_t regVal = 0;\
        regVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));\
                rc = CC_REG_FLD_GET(0, HOST_AO_LOCK_BITS, HOST_## keyType ##_LOCK, regVal);\
                rc = (rc == 1)?CC_TRUE:CC_FALSE;\
    }while(0)

/* Verifies if one of teh keys: Kpicv, Kceicv, HBK0, Kcp or Kce is in use */
#define CC_IS_OTP_KEY_IN_USE(owner, keyType, rc, isKeyInUse)\
    do {\
            uint32_t otpVal = 0;\
            uint32_t notInUse = 0;\
            rc = mbedtls_mng_otpWordRead(CC_OTP_## owner ##_FLAG_OFFSET, &otpVal);\
            if (rc == CC_OK) {\
                        notInUse = BITFIELD_GET(otpVal,  CC_OTP_## owner ##_FLAG_## keyType ##_NOT_IN_USE_BIT_SHIFT,\
                                                           CC_OTP_## owner ##_FLAG_## keyType ##_NOT_IN_USE_BIT_SIZE);\
            isKeyInUse = (notInUse ==0)?CC_TRUE:CC_FALSE;\
            }\
    }while(0)

/*!
 * This function is used to wrap the input and output data buffers to 2 info structs.
 * The function call a PAL function to verify each buffer validity memory.
 * It also get the buffer attribute (secure / non-secure) from the PAL function.
 *
 * \param pDataIn A pointer to the input data buffer.
 * \param dataInSize - number bytes of input data buffer.
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pDataOut A pointer to the output data buffer.
 * \param dataOutSize - number bytes of output data buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 *
 * \return drvError_t defined in cc_error.h.
 */
drvError_t SetDataBuffersInfo(const uint8_t *pDataIn, size_t dataInSize, CCBuffInfo_t *pInputBuffInfo,
                              const uint8_t *pDataOut, size_t dataOutSize, CCBuffInfo_t *pOutputBuffInfo);

#endif /* _DRIVER_DEFS_H_ */

