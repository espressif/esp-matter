/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _BSV_CRYPTO_DEFS_H
#define _BSV_CRYPTO_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains cryptographic ROM APIs definitions.
*/

/*! AES CMAC result size in words. */
#define CC_BSV_CMAC_RESULT_SIZE_IN_WORDS    4  /* 128b */
/*! AES CMAC result size in Bytes. */
#define CC_BSV_CMAC_RESULT_SIZE_IN_BYTES    16 /* 128b */
/*! AES CCM 128bit key size in Bytes. */
#define CC_BSV_CCM_KEY_SIZE_BYTES               16
/*! AES CCM 128bit key size in words. */
#define CC_BSV_CCM_KEY_SIZE_WORDS               4
/*! AES CCM NONCE size in Bytes. */
#define CC_BSV_CCM_NONCE_SIZE_BYTES     12


/*! Definitions for AES key types. */
typedef enum  {
        /*! Root key (HUK).*/
        CC_BSV_HUK_KEY = 0,
        /*! RTL key (Krtl).*/
        CC_BSV_RTL_KEY = 1,
        /*! OEM Provision key (Kcp).*/
        CC_BSV_PROV_KEY = 2,
        /*! OEM Code encryption key (Kce).*/
        CC_BSV_CE_KEY = 3,
        /*! ICV Provision key (Kpicv).*/
        CC_BSV_ICV_PROV_KEY = 4,
        /*! ICV Code encryption key (Kceicv).*/
        CC_BSV_ICV_CE_KEY = 5,
        /*! User's key.*/
        CC_BSV_USER_KEY = 6,
        /*! Reserved.*/
        CC_BSV_END_OF_KEY_TYPE = 0x7FFFFFFF
}CCBsvKeyType_t;


/*! CMAC result buffer. */
typedef uint32_t CCBsvCmacResult_t[CC_BSV_CMAC_RESULT_SIZE_IN_WORDS];

/*! AES_CCM key buffer definition.*/
typedef uint32_t CCBsvCcmKey_t[CC_BSV_CCM_KEY_SIZE_WORDS];

/*! AES_CCM nonce buffer definition.*/
typedef uint8_t CCBsvCcmNonce_t[CC_BSV_CCM_NONCE_SIZE_BYTES];

/*! AES_CCM MAC buffer definition.*/
typedef uint8_t CCBsvCcmMacRes_t[CC_BSV_CMAC_RESULT_SIZE_IN_BYTES];



#ifdef __cplusplus
}
#endif

#endif
