/******************************************************************************

 @file  zaesccm_api.h

 @brief AES CCM service (OS dependent) interface

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
#ifndef ZAESCCM_API_H
#define ZAESCCM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Authenticates and encrypts a text using an MLE key.<br>
 * This function is thread-safe.
 *
 * @param  encrypt   set to TRUE to encrypt. FALSE to just authenticate without encryption.
 * @param  Mval      length of authentication field in octets
 * @param  N         13 byte nonce
 * @param  M         octet string 'm'
 * @param  len_m     length of M[] in octets
 * @param  A         octet string 'a'
 * @param  len_a     length of A[] in octets
 * @param  AesKey    Pointer to AES Key or Pointer to Key Expansion buffer
 * @param  MAC       MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char zaesccmAuthEncrypt(unsigned char encrypt,
                                      unsigned char Mval, unsigned char *Nonce,
                                      unsigned char *M, unsigned short len_m,
                                      unsigned char *A, unsigned short len_a,
                                      unsigned char *AesKey,
                                      unsigned char *MAC, unsigned char ccmLVal);

/**
 * Decrypts and authenticates an encrypted text using an MLE key.
 * This function is thread-safe.
 *
 * @param  decrypt   set to TRUE to decrypt. Set to FALSE to authenticate without decryption.
 * @param  Mval      length of authentication field in octets
 * @param  N         13 byte nonce
 * @param  M         octet string 'm'
 * @param  len_m     length of M[] in octets
 * @param  A         octet string 'a'
 * @param  len_a     length of A[] in octets
 * @param  AesKey    Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param  MAC       MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char zaesccmDecryptAuth(unsigned char decrypt,
                                      unsigned char Mval, unsigned char *Nonce,
                                      unsigned char *M, unsigned short len_m,
                                      unsigned char *A, unsigned short len_a,
                                      unsigned char *AesKey,
                                      unsigned char *MAC, unsigned char ccmLVal);

/**
 * Locks mutex used by AES CCM service module.
 * Note that this function is not provided by AES CCM service module but instead it has
 * to be implemented in order to link and use AES CCM service.
 */
extern void zaesccmMutexLock(void);

/**
 * Unlocks mutex used by AES CCM service module.
 * Note that this function is not provided by AES CCM service module but instead it has
 * to be implemented in order to link and use AES CCM service.
 */
extern void zaesccmMutexUnlock(void);

#ifdef __cplusplus
}
#endif

#endif /* ZAESCCM_API_H */
