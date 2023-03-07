/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*!
 @addtogroup cc_aes_defs
 @{
 */

/*!
 @file
 @brief This file contains the type definitions that are used by the CryptoCell
 AES APIs.
 */


#ifndef CC_AES_DEFS_H
#define CC_AES_DEFS_H

#include "cc_pal_types.h"
#include "cc_aes_defs_proj.h"


#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines  ******************************/
/*! The size of the AES block in words. */
#define CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS 4
/*! The size of the AES block in bytes. */
#define CC_AES_BLOCK_SIZE_IN_BYTES  (CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t))

/*! The size of the IV buffer in words. */
#define CC_AES_IV_SIZE_IN_WORDS   CC_AES_CRYPTO_BLOCK_SIZE_IN_WORDS
/*! The size of the IV buffer in bytes. */
#define CC_AES_IV_SIZE_IN_BYTES  (CC_AES_IV_SIZE_IN_WORDS * sizeof(uint32_t))


/************************ Enums ********************************/
/*! The AES operation:<ul><li>Encrypt</li><li>Decrypt</li></ul>. */
typedef enum {
    /*! An AES encrypt operation. */
    CC_AES_ENCRYPT = 0,
    /*! An AES decrypt operation. */
    CC_AES_DECRYPT = 1,
    /*! The maximal number of operations. */
    CC_AES_NUM_OF_ENCRYPT_MODES,
    /*! Reserved. */
    CC_AES_ENCRYPT_MODE_LAST = 0x7FFFFFFF
}CCAesEncryptMode_t;

/*! The AES operation mode. */
typedef enum {
    /*! ECB mode. */
    CC_AES_MODE_ECB          = 0,
    /*! CBC mode. */
    CC_AES_MODE_CBC          = 1,
    /*! CBC-MAC mode. */
    CC_AES_MODE_CBC_MAC      = 2,
    /*! CTR mode. */
    CC_AES_MODE_CTR          = 3,
    /*! XCBC-MAC mode. */
    CC_AES_MODE_XCBC_MAC     = 4,
    /*! CMAC mode. */
    CC_AES_MODE_CMAC         = 5,
    /*! XTS mode. */
    CC_AES_MODE_XTS          = 6,
    /*! CBC-CTS mode. */
    CC_AES_MODE_CBC_CTS      = 7,
    /*! OFB mode. */
    CC_AES_MODE_OFB          = 8,

    /*! The maximal number of AES modes. */
    CC_AES_NUM_OF_OPERATION_MODES,
    /*! Reserved. */
    CC_AES_OPERATION_MODE_LAST = 0x7FFFFFFF
}CCAesOperationMode_t;

/*! The AES padding type. */
typedef enum {
       /*! No padding. */
       CC_AES_PADDING_NONE  = 0,
       /*! PKCS7 padding. */
       CC_AES_PADDING_PKCS7 = 1,
       /*! The maximal number of AES padding modes. */
       CC_AES_NUM_OF_PADDING_TYPES,
       /*! Reserved. */
       CC_AES_PADDING_TYPE_LAST = 0x7FFFFFFF
}CCAesPaddingType_t;

/*! The AES key type. */
typedef enum {
    /*! The user key. */
    CC_AES_USER_KEY          = 0,
    /*! The Kplt hardware key. */
    CC_AES_PLATFORM_KEY      = 1,
    /*! The Kcst hardware key. */
    CC_AES_CUSTOMER_KEY      = 2,
    /*! The maximal number of AES key types. */
    CC_AES_NUM_OF_KEY_TYPES,
    /*! Reserved. */
    CC_AES_KEY_TYPE_LAST = 0x7FFFFFFF
}CCAesKeyType_t;

/************************ Typedefs  ****************************/

/*! Defines the IV buffer. A 16-byte array. */
typedef uint8_t CCAesIv_t[CC_AES_IV_SIZE_IN_BYTES];

/*! Defines the AES key data buffer. */
typedef uint8_t CCAesKeyBuffer_t[CC_AES_KEY_MAX_SIZE_IN_BYTES];

/************************ Structs  ******************************/

/*!
 The context prototype of the user.

 The argument type that is passed by the user to the AES APIs. The context
 saves the state of the operation, and must be saved by the user until
 the end of the API flow.
 */
typedef struct CCAesUserContext_t {
    /*! The context buffer for internal usage. */
    uint32_t buff[CC_AES_USER_CTX_SIZE_IN_WORDS] ;
}CCAesUserContext_t;


/*! The AES key data of the user. */
typedef struct CCAesUserKeyData_t {
    /*! A pointer to the key. */
    uint8_t * pKey;
    /*! The size of the key in bytes. Valid values for XTS mode, if supported:
    32 bytes or 64 bytes, indicating the full size of the double key (2x128 or
    2x256 bit). Valid values for XCBC-MAC mode: 16 bytes, as limited by the
    standard. Valid values for all other modes: 16 bytes, 24 bytes, or
    32 bytes. */
    size_t    keySize;
}CCAesUserKeyData_t;

/*! The AES HW key Data. */
typedef struct CCAesHwKeyData_t {
    /*! Slot number. */
    size_t slotNumber;
}CCAesHwKeyData_t;

#endif /* CC_AES_DEFS_H */

#ifdef __cplusplus
}

#endif

/*!
 @}
*/

