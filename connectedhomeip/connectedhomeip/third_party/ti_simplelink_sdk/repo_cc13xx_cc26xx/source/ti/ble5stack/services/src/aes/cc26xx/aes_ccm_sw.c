/******************************************************************************

 @file  aes_ccm_sw.c

 @brief AES CCM service (OS dependent) implementation

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#include "ccm.h"
#include "aes.h"

#include "aes_ccm_api.h"

#include "rom.h"

/* see baesccm_api.h */
signed char AesCcm_authEncrypt_Sw(uint8_t encrypt,
                               uint8_t Mval, uint8_t *N,
                               uint8_t *M, unsigned short len_m,
                               uint8_t *A, unsigned short len_a,
                               uint8_t *AesKey,
                               uint8_t *MAC, uint8_t ccmLVal)
{
  /* initialize AES engine with the designated key */
  MAP_ssp_KeyInit_Sw(AesKey);

  /* This is required as legacy interface for the MSP430 platform */
  pSspAesEncrypt_Sw = MAP_sspAesEncrypt_Sw;

  /* Run authentication + encryption */
  if (MAP_SSP_CCM_Auth_Encrypt_Sw(encrypt, Mval, N, M, len_m, A, len_a,
                           AesKey, MAC, ccmLVal) != 0)
  {
    return -2;
  }

  return 0;
}

/* see baesccm_api.h */
signed char AesCcm_decryptAuth_Sw(uint8_t decrypt,
                               uint8_t Mval, uint8_t *N,
                               uint8_t *M, unsigned short len_m,
                               uint8_t *A, unsigned short len_a,
                               uint8_t *AesKey,
                               uint8_t *cState, uint8_t ccmLVal)
{
  /* initialize AES engine with the designated key */
  MAP_ssp_KeyInit_Sw(AesKey);

  /* This is required as legacy interface for MSP430 platform */
  pSspAesEncrypt_Sw = MAP_sspAesEncrypt_Sw;

  /* Run decryption + inverse authentication */
  if (MAP_SSP_CCM_InvAuth_Decrypt_Sw(decrypt, Mval, N, M, len_m, A, len_a,
                              AesKey, cState, ccmLVal) != 0)
  {
    return -2;
  }

  return 0;
}

/**
 * @fn     Aes_encrypt_Sw
 *
 * @brief  encrypts data with AES128 encryption.  Encrypted data is returned
 *         back in the same pointer.
 *
 * @param  plainText - Plain-text to be encrypted.
 *                     Output will be written to this pointer.
 *
 * @param  textLen   - length in bytes of plain-text.
 *
 * @param  pAesKey    - pointer to the AES key.
 *
 * @ret    none
 */
void Aes_encrypt_Sw( uint8_t *plainText, uint16_t textLen, uint8_t *pAesKey)
{
  uint16_t numBlocks = MAP_AesCcm_getNumBlocks_Sw(textLen);

  for (uint8_t i = 0; i < numBlocks; ++i)
  {
    MAP_sspAesEncryptBasic_Sw(pAesKey, &plainText[i*16]);
  }
}


/**
 * @fn     Aes_Decrypt
 *
 * @brief  decrypts data which was encrypted with AES128 encryption.  Plain-text
 *         data is returned back in the same pointer.
 *
 * @param  cypherText - Cypher-text to be decrypted.
 *                      Output will be written to this pointer.
 *
 * @param  textLen    - length of encrypted text.
 *
 * @param  pAesKey    - poitner to the AES key.
 *
 * @ret    none
 */
void Aes_decrypt_Sw( uint8_t *cypherText, uint16_t textLen, uint8_t* pAesKey)
{
  uint16_t numBlocks = MAP_AesCcm_getNumBlocks_Sw(textLen);

  for (uint8_t i = 0; i < numBlocks; ++i)
  {
    MAP_sspAesDecrypt_Sw(pAesKey, &cypherText[i*16]);
  }
}

/**
 * @fn     AesCcm_getNumBlocks_Sw
 *
 * @brief  Pass in the size, in bytes, of the text.
 *
 * @param  sizeOfText - size of text in bytes.
 *
 * @ret    number of AES 128bit blocks in a block of data.
 *
 */
uint16_t AesCcm_getNumBlocks_Sw(uint16_t textLength)
{
  //( bytes*(8bits/byte) )/(128bits/block) =  bytes/16 = number of blocks.
  return textLength >> 4;
}
