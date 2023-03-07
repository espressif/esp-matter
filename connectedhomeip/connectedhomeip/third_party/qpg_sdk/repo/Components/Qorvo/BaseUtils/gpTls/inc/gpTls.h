/*
 * Copyright (c) 2018, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "gpTls.h"
 *
 *  Declarations of the public functions and enumerations of gpTls.
*/

#ifndef _GPTLS_H_
#define _GPTLS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpTls_Result_t */
//@{
/** @brief An invalid parameter was given as a parameter to this function. */
#define gpTls_ResultInvalidParameter                           0x5
/** @brief The GP chip is busy. */
#define gpTls_ResultBusy                                       0x7
/** @brief The operation unsupported. */
#define gpTls_ResultUnsupported                                0xB
/** @brief The function returned successful. */
#define gpTls_ResultSuccess                                    0x0
/** @typedef gpTls_Result_t
 *  @brief The gpTls_Result_t type defines the result of various encryption functions.
*/
typedef UInt8                             gpTls_Result_t;
//@}

/** @enum gpTls_AESKeyLen_t */
//@{
/** @brief An image is available */
#define gpTls_AESKeyLen128                                     (128>>3)
/** @brief 192 bits key len */
#define gpTls_AESKeyLen192                                     (192>>3)
/** @brief 256 bits key len */
#define gpTls_AESKeyLen256                                     (256>>3)
/** @brief Identifier for invalid value */
#define gpTls_AESKeyLenInv                                     0xFF
/** @typedef gpTls_AESKeyLen_t
 *  @brief gpTls_AESKeyLen_t possible values are: 16, 24 or 32 bytes.
*/
typedef UInt8                             gpTls_AESKeyLen_t;
//@}

/** @enum gpTls_SecLevel_t */
//@{
/** @brief Mode 0 No encryption, no MIC added. */
#define gpTls_SecLevelNothing                                  0
/** @brief Mode 1 No encryption, 32 bit MIC added. */
#define gpTls_SecLevelMIC32                                    1
/** @brief Mode 2 No encryption, 64 bit MIC added. */
#define gpTls_SecLevelMIC64                                    2
/** @brief Mode 3 No encryption, 128 bit MIC added. */
#define gpTls_SecLevelMIC128                                   3
/** @brief Mode 4 Encryption of payload, no MIC added. */
#define gpTls_SecLevelENC                                      4
/** @brief Mode 5 Encryption of payload, 32 bit MIC added. */
#define gpTls_SecLevelENC_MIC32                                5
/** @brief Mode 6 Encryption of payload, 64 bit MIC added. */
#define gpTls_SecLevelENC_MIC64                                6
/** @brief Mode 7 Encryption of payload, 128 bit MIC added. */
#define gpTls_SecLevelENC_MIC128                               7
/** @typedef gpTls_SecLevel_t
 *  @brief The gpTls_SecLevel_t type defines the IEEE Std 802.15.4 (2006) security level.
*/
typedef UInt8                             gpTls_SecLevel_t;
//@}

/** @enum gpTls_KeyId_t */
//@{
#define gpTls_KeyIdUserKey0                                    0x00
#define gpTls_KeyIdUserKey1                                    0x01
#define gpTls_KeyIdUserKey2                                    0x02
#define gpTls_KeyIdUserKey3                                    0x03
#define gpTls_KeyIdUserKey4                                    0x04
#define gpTls_KeyIdUserKey5                                    0x05
#define gpTls_KeyIdUserKey6                                    0x06
#define gpTls_KeyIdUserKey7                                    0x07
/** @brief Product key identifier */
#define gpTls_KeyIdProductKey0                                 0x50
/** @brief Product key identifier */
#define gpTls_KeyIdProductKey1                                 0x51
/** @brief Unspecified key identifier */
#define gpTls_KeyIdKeyPtr                                      0x7E
/** @brief Unspecified key identifier */
#define gpTls_KeyIdUnspecified                                 0x7F
/** @brief Enable additional security hardening */
#define gpTls_Hardened                                         0x80
/** @typedef gpTls_KeyId_t
 *  @brief User key identifiers
*/
typedef UInt8                             gpTls_KeyId_t;
//@}

/** @enum gpTls_HashFct_t */
//@{
/** @brief MD5 hash function */
#define gpTls_MD5                                              0x01
/** @brief SHA1 hash function */
#define gpTls_SHA1                                             0x02
/** @brief SHA224 hash function */
#define gpTls_SHA224                                           0x03
/** @brief SHA256 hash function */
#define gpTls_SHA256                                           0x04
/** @brief SHA384 hash function */
#define gpTls_SHA384                                           0x05
/** @brief SHA512 hash function */
#define gpTls_SHA512                                           0x06
/** @typedef gpTls_HashFct_t
 *  @brief gpTls_HashFct_t type of hash function.
*/
typedef UInt8                             gpTls_HashFct_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/** @macro GP_TLS_OPTIONS_IS_HARDENED(id) */
#define GP_TLS_OPTIONS_IS_HARDENED(id)               (((id) & gpTls_Hardened) != 0)
/** @macro GP_TLS_OPTIONS_GET_KEYID(id) */
#define GP_TLS_OPTIONS_GET_KEYID(id)                 (id & ~gpTls_Hardened)
/** @macro GP_TLS_KEYID_IS_USER(id) */
#define GP_TLS_KEYID_IS_USER(id)                     (id <= gpTls_KeyIdUserKey7)
/** @macro GP_TLS_KEYID_IS_PRODUCT(id) */
#define GP_TLS_KEYID_IS_PRODUCT(id)                  (id == gpTls_KeyIdProductKey0 || id == gpTls_KeyIdProductKey1)
/** @macro GP_TLS_KEYID_IS_KEYPTR(id) */
#define GP_TLS_KEYID_IS_KEYPTR(id)                   (id == gpTls_KeyIdKeyPtr)
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @struct gpTls_AESOptions_t */
typedef struct {
    gpTls_AESKeyLen_t              keylen;
    /** @brief This parameter is an 8bit bitmask specifying the options: bits[6:0] specify the keyid to be used (see API manual); bit[7] indicates additional hardening */
    UInt8                          options;
} gpTls_AESOptions_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
/** @brief Performs a synchronous CCM Encryption.  The function will encrypt the bytes with the CCM algorithm according to the specified options in the gpTls_CCMOptions structure.
*
*   @param dataLength                Length of plaintext data buffer
*   @param auxLength                 Length of aux data buffer
*   @param micLength                 Length of mic buffer
*   @param outputDataLength          Length of ciphered data buffer
*   @param pKey                      This field contains the pointer to the encryption key. The key size is fixed to 16 bytes.
*   @param pNonce                    This field contains the pointer to the nonce used for operation. The nonce length is fixed to 13 bytes.
*   @param pData                     plaintext to be encrypted
*   @param pAux                      auxiliary data (a-data in CCM*)
*   @param pDataOut                  output ciphertext
*   @param pMic                      MIC data
*   @return result                   gpTls_ResultSuccess, gpTls_ResultBusy
*/
gpTls_Result_t gpTls_CCMEncrypt(UInt16 dataLength, UInt8 auxLength, UInt8 micLength, UInt16 outputDataLength, UInt8* pKey, UInt8* pNonce, UInt8* pData, UInt8* pAux, UInt8* pDataOut, UInt8* pMic);

/** @brief Performs a synchronous CCM Decryption.  The function will decrypt the bytes with the CCM algorithm according to the specified options in the gpTls_CCMOptions structure.
*
*   @param dataLength                Length of ciphered data buffer
*   @param auxLength                 Length of aux data buffer
*   @param micLength                 Length of mic buffer
*   @param outputDataLength          Length of unciphered data buffer
*   @param pKey                      This field contains the pointer to the encryption key. The key size is fixed to 16 bytes.
*   @param pNonce                    This field contains the pointer to the nonce used for operation. The nonce length is fixed to 13 bytes.
*   @param pData                     Deciphered data buffer
*   @param pAux                      Aux data (AAD)
*   @param pDataOut                  Buffer for output ciphertext
*   @param pMic                      Buffer to MIC data
*   @return result                   gpTls_ResultSuccess, gpTls_ResultBusy, gpTls_ResultInvalidParameter
*/
gpTls_Result_t gpTls_CCMDecrypt(UInt16 dataLength, UInt8 auxLength, UInt8 micLength, UInt16 outputDataLength, UInt8* pKey, UInt8* pNonce, UInt8* pData, UInt8* pAux, UInt8* pDataOut, UInt8* pMic);

/** @brief Performs a synchronous AES Encryption. The function will encrypt 16 bytes with the AES algorithm and return the result in place.
*
*   @param pInplaceBuffer            Pointer to the buffer of the 16 to be encrypted bytes. The encrypted result will be returned in the same buffer.
*   @param pAesKey                   Pointer to the 16-byte key. This parameter is only used when gpTls_KeyIdKeyPtr is specified in the options parameter. When NULL is specified in combination with gpTls_KeyIdKeyPtr, 0 will be used as key value.
*   @param AESOptions                This parameter is an 8bit bitmask specifying the options: bits[6:0] specify a keyid defined by gpTls_KeyId_t; bit[7] indicates additional hardening.
*   @return result                   gpTls_ResultSuccess, gpTls_ResultBusy 
*/
gpTls_Result_t gpTls_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpTls_AESOptions_t AESOptions);

/** @brief Performs a synchronous AES Decryption. The function will Decryption 16 bytes with the AES algorithm and return the result in place.
*
*   @param pInplaceBuffer            Pointer to the buffer of the 16 to be decrypted bytes. The decrypted result will be returned in the same buffer.
*   @param pAesKey                   Pointer to the 16-byte key. This parameter is only used when gpTls_KeyIdKeyPtr is specified in the options parameter. When NULL is specified in combination with gpTls_KeyIdKeyPtr, 0 will be used as key value.
*   @param AESOptions                This parameter is an 8bit bitmask specifying the options: bits[6:0] specify a keyid defined by gpTls_KeyId_t; bit[7] indicates additional hardening.
*   @return result                   gpTls_ResultSuccess, gpTls_ResultBusy 
*/
gpTls_Result_t gpTls_AESDecrypt(UInt8* pInplaceBuffer, UInt8* pAesKey, gpTls_AESOptions_t AESOptions);

/** @brief Performs a  HMAC, a mechanism for message authentication using cryptographic hash functions.
*
*   @param hashFct                   gpTls_HashFct_t type of hash function.
*   @param keyLength                 Length of key data buffer
*   @param msgLength                 Length of message data buffer
*   @param resultLength              Length of output data buffer
*   @param pKey                      secret key used to authentication
*   @param pMsg                      message to be authenticated
*   @param pResult                   message to be authenticated
*   @return result                   gpTls_ResultSuccess, gpTls_ResultBusy, gpTls_ResultUnsupported 
*/
gpTls_Result_t gpTls_HMACAuth(gpTls_HashFct_t hashFct, UInt8 keyLength, UInt8 msgLength, UInt8 resultLength, UInt8* pKey, UInt8* pMsg, UInt8* pResult);

void gpTls_Init(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPTLS_H_

