/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 *  The file gpEncryption.h contains the encryption API definitions.
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

#ifndef _ENCRYPTION_GP_H_
#define _ENCRYPTION_GP_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpEncryption_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/**
 * @file gpEncryption.h
 *
 * @defgroup AES AES Encryption
 * @defgroup CCM CCM Encryption and Decryption
 * @defgroup INIT Initialization
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/


 /** @name gpEncryption_AESKeyLen_t */
//@{
/** @brief 128 bits key len */
#define gpEncryption_AESKeyLen128    (128>>3)
/** @brief 192 bits key len */
#define gpEncryption_AESKeyLen192    (192>>3)
/** @brief 256 bits key len */
#define gpEncryption_AESKeyLen256    (256>>3)
/** @brief Identifier for invalid value */
#define gpEncryption_AESKeyLenInv    0xFF
/** @typedef gpEncryption_AESKeyLen_t
 *  @brief gpEncryption_AESKeyLen_t possible values are: 16, 24 or 32 bytes.
 *
 */
typedef UInt8 gpEncryption_AESKeyLen_t;


/** @struct gpEncryption_AESOptions_t
 *  @param gpEncryption_AESKeyLen_t
 *  @param This parameter is an 8bit bitmask specifying the options: bits[6:0] specify the keyid to be used (see gpEncryption_API_Manual); bit[7] indicates additional hardening
*/
typedef struct{
    gpEncryption_AESKeyLen_t    keylen;
    UInt8                       options;
} gpEncryption_AESOptions_t;


#define GP_ENCRYPTION_OPTIONS_IS_HARDENED(id) (((id) & gpEncryption_Hardened) != 0)
#define GP_ENCRYPTION_OPTIONS_GET_KEYID(id)   (id & ~gpEncryption_Hardened)

#define GP_ENCRYPTION_KEYID_IS_USER(id)       (id <= gpEncryption_KeyIdUserKey7)
#define GP_ENCRYPTION_KEYID_IS_PRODUCT(id)    (id == gpEncryption_KeyIdProductKey0 || id == gpEncryption_KeyIdProductKey1)
#define GP_ENCRYPTION_KEYID_IS_KEYPTR(id)     (id == gpEncryption_KeyIdKeyPtr)

/** @name gpEncryption_SecLevel_t */
//@{
/** @brief Mode 0 No encryption, no MIC added. */
#define gpEncryption_SecLevelNothing       0
/** @brief Mode 1 No encryption, 32 bit MIC added. */
#define gpEncryption_SecLevelMIC32         1
/** @brief Mode 2 No encryption, 64 bit MIC added. */
#define gpEncryption_SecLevelMIC64         2
/** @brief Mode 3 No encryption, 128 bit MIC added. */
#define gpEncryption_SecLevelMIC128        3
/** @brief Mode 4 Encryption of payload, no MIC added. */
#define gpEncryption_SecLevelENC           4
/** @brief Mode 5 Encryption of payload, 32 bit MIC added. */
#define gpEncryption_SecLevelENC_MIC32     5
/** @brief Mode 6 Encryption of payload, 64 bit MIC added. */
#define gpEncryption_SecLevelENC_MIC64     6
/** @brief Mode 7 Encryption of payload, 128 bit MIC added. */
#define gpEncryption_SecLevelENC_MIC128    7
/** @brief The gpEncryption_SecLevel_t type defines the IEEE Std 802.15.4 (2006) security level.
*/
typedef UInt8 gpEncryption_SecLevel_t;

/** @brief Convert security level into mic length */
#define GP_ENCRYPTION_SECLEVEL2MICLENGTH(secLevel)  (((secLevel&0x03)*4)==12?16:((secLevel&0x03)*4))

//@}

/** @name gpEncryption_CCMOptions_t */
//@{
/** @brief The gpEncryption_CCMOptions structure contains all the parameters for the CCM operations.
*/
typedef struct gpEncryption_CCMOptions {
/** @brief This field contains the pd (packet descriptor) identifier where the encryption/decryption will take place */
    gpPd_Handle_t pdHandle;
/** @brief This field contains the offset in the pd indicating the start of the data (m-data in CCM*). */
    gpPd_Offset_t dataOffset;
/** @brief This field contains the data length. This is the length of the data where the security operation will be performed (m-data in CCM*).*/
    UInt8 dataLength;
/** @brief This field contains the offset in the pd indicating the start of the auxiliary data (a-data in CCM*). */
    gpPd_Offset_t auxOffset;
/** @brief This field contains the length of the auxiliary data (a-data in CCM*). */
    UInt8  auxLength;
/** @brief This field contains the expected MIC length. */
    UInt8  micLength;
/** @brief This field contains the pointer to the encryption key. The key size is fixed to 16 bytes. */
    UInt8* pKey;
/** @brief This field contains the pointer to the nonce used for operation. The nonce length is fixed to 13 bytes.  */
    UInt8* pNonce;
} gpEncryption_CCMOptions_t;
//@}

/** @name gpEncryption_Result_t */
//@{
/** @brief The function returned successful. */
#define gpEncryption_ResultSuccess             0x0
/** @brief An invalid parameter was given as a parameter to this function. */
#define gpEncryption_ResultInvalidParameter    0x5
/** @brief The GP chip is busy. */
#define gpEncryption_ResultBusy                0x7
/** @typedef gpEncryption_Result_t
 *  @brief The gpEncryption_Result_t type defines the result of various encryption functions.
*/
typedef UInt8 gpEncryption_Result_t;
//@}

/** @name gpEncryption_Result_t */
//@{
/** @brief User key identifiers */
#define gpEncryption_KeyIdUserKey0         0x00
#define gpEncryption_KeyIdUserKey1         0x01
#define gpEncryption_KeyIdUserKey2         0x02
#define gpEncryption_KeyIdUserKey3         0x03
#define gpEncryption_KeyIdUserKey4         0x04
#define gpEncryption_KeyIdUserKey5         0x05
#define gpEncryption_KeyIdUserKey6         0x06
#define gpEncryption_KeyIdUserKey7         0x07
/** @brief Product key identifiers */
#define gpEncryption_KeyIdProductKey0      0x50
#define gpEncryption_KeyIdProductKey1      0x51
/** @brief Unspecified key identifier */
#define gpEncryption_KeyIdKeyPtr           0x7E
/** @brief Unspecified key identifier */
#define gpEncryption_KeyIdUnspecified      0x7F
typedef UInt8 gpEncryption_KeyId_t;
//@}

/** @brief Enable additional security hardening */
#define gpEncryption_Hardened         0x80


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpEncryption_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @ingroup AES
 * @brief Performs a synchronous AES Encryption.
 *
 * The function will encrypt 16 bytes with the AES algorithm and return the result in place.
 *
 * @param pInplaceBuffer   Pointer to the buffer of the 16 to be encrypted bytes. The encrypted result will be returned in the same buffer.
 * @param pAesKey          Pointer to the 16-byte key.
 *                         This parameter is only used when gpEncryption_KeyIdKeyPtr is specified in the options parameter. When NULL is specified in combination with gpEncryption_KeyIdKeyPtr, 0 will be used as key value.
 * @param options          This parameter is an 8bit bitmask specifying the options: bits[6:0] specify a keyid defined by gpEncryption_KeyId_t; bit[7] indicates additional hardening.
 * @return
 *          - gpEncryption_ResultSuccess
 *          - gpEncryption_ResultBusy
*/
GP_API gpEncryption_Result_t gpEncryption_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpEncryption_AESOptions_t AESoptions);

/**
 * @ingroup CCM
 * @brief Performs a synchronous CCM Encryption.
 *
 * The function will encrypt the bytes with the CCM algorithm according to the specified options
 * in the gpEncryption_CCMOptions structure.
 *
 * @param pCCMOptions        Pointer to the gpEncryption_CCMOptions structure.
 *
 * @return
 *          - gpEncryption_ResultSuccess
 *          - gpEncryption_ResultBusy
*/
GP_API gpEncryption_Result_t gpEncryption_CCMEncrypt(gpEncryption_CCMOptions_t * pCCMOptions);

/**
 * @ingroup CCM
 * @brief Performs a synchronous CCM Decryption.
 *
 * The function will decrypt the bytes with the CCM algorithm according to the specified options
 * in the gpEncryption_CCMOptions structure.
 *
 * @param pCCMOptions        Pointer to the gpEncryption_CCMOptions structure.
 *
 * @return
 *          - gpEncryption_ResultSuccess
 *          - gpEncryption_ResultBusy
 *          - gpEncryption_ResultInvalidParameter
*/
GP_API gpEncryption_Result_t gpEncryption_CCMDecrypt(gpEncryption_CCMOptions_t* pCCMOptions);

/**
 * @ingroup INIT
 * @brief Initializes the gpEncryption component.
 *
 * This function initializes the gpEncryption component. It should be called before calling any other function.
 *
 * This primitive is typically called via the gpBaseComps_StackInit() method of the gpBaseComps component.
*/
GP_API void gpEncryption_Init(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_ENCRYPTION_GP_H_

