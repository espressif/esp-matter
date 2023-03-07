/******************************************************************************

 @file  ccm.c

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2022, Texas Instruments Incorporated
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
/******************************************************************************
 * INCLUDES
 */

#include <stdint.h>
#include <string.h>

#include "rom.h"

#include "aes.h"
#include "ccm.h"

/******************************************************************************
 * MACROS
 */

/******************************************************************************
 * CONSTANTS
 */

#define CCM_SUCCESS 0
#define CCM_FAILURE 1
/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * LOCAL VARIABLES
 */

/******************************************************************************
 * GLOBAL VARIABLES
 */

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

uint8_t SSP_CCM_Auth_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *,
                                                uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_Encrypt_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_Decrypt_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *, uint8_t *, uint8_t);
uint8_t SSP_CCM_InvAuth_Sw (uint8_t, uint8_t *, uint8_t *, uint16_t, uint8_t *,
                                                uint16_t, uint8_t *, uint8_t *, uint8_t);

/******************************************************************************
 * @fn      SSP_CCM_Auth_Sw
 *
 * @brief   Generates CCM Authentication tag U.
 *
 * input parameters
 *
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   M       - Pointer to octet string 'm'
 * @param   len_m   - Length of M[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to output buffer
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain Authentication Tag T
 *
 * @return  uint8_t
 *
 */
uint8_t SSP_CCM_Auth_Sw (uint8_t Mval, uint8_t *N, uint8_t *M, uint16_t len_m, uint8_t *A,
                                    uint16_t len_a, uint8_t *AesKey, uint8_t *Cstate, uint8_t ccmLVal)
{
  uint8_t   B[16], *bptr;
  uint8_t   i, remainder;
  uint16_t  blocks;
#ifdef CCM_AUTH_DEBUG
  extern void traceBlockData(unsigned char len, unsigned char *pData);
#define CCM_AUTH_DEBUG_TRACE(_cstate) traceBlockData(16, _cstate)
#define CCM_AUTH_AESOUT_DEBUG_TRACE(_cstate)
#else
#define CCM_AUTH_DEBUG_TRACE(_cstate)
#define CCM_AUTH_AESOUT_DEBUG_TRACE(_cstate)
#define CCM_AUTH_B_DEBUG_TRACE(_cstate)
#endif /* CCM_AUTH_DEBUG */

  // Check if authentication is even requested.  If not, exit.
  // This check is actually not needed because the rest of the
  // code works fine even with Mval==0.  I added it to reduce
  // unnecessary calculations and to speed up performance when
  // Mval==0
  if (!Mval) return CCM_SUCCESS;

  //
  // Construct B0
  //
  B[0] = (ccmLVal - 1);               // L=2, L-encoding = L-1 = 1
  if (len_a)  B[0] |= 0x40;           // Adata bit
  if (Mval)  B[0] |= (Mval-2) << 2;   // M encoding

  osal_memcpy (B+1, N, 13);           // append Nonce (13-bytes)

  /* For LVal == 3 the nonce is only 12 bytes and the length is stored
   * from B[13]. Since the input length data type is uint16_t we
   * know the higher order byte is always zero.
   */
  if ( 3 == ccmLVal )
  {
    B[13] = 0;
  }

  B[14] = (uint8_t)(len_m >> 8);                // append l(m)
  B[15] = (uint8_t)(len_m);

  osal_memset (Cstate, 0, 16);                    // X0 = 0

  for (i=0; i < 16; i++)  Cstate[i] ^= B[i];
  CCM_AUTH_DEBUG_TRACE(Cstate);
  pSspAesEncrypt_Sw (AesKey, Cstate);                 // Cstate[] = X1
  CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);

  //
  // Construct B1
  //
  B[0] = (uint8_t) (len_a >> 8);
  B[1] = (uint8_t) (len_a);

  if (len_a > 14)  osal_memcpy (B+2, A, 14);
  else
  {
      osal_memset (B+2, 0, 14);
      osal_memcpy (B+2, A, len_a);
  }
  CCM_AUTH_B_DEBUG_TRACE(B);
  for (i=0; i < 16; i++)  Cstate[i] ^= B[i];
  CCM_AUTH_DEBUG_TRACE(Cstate);
  pSspAesEncrypt_Sw (AesKey, Cstate);                 // Cstate[] = X2
  CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);

  //
  // Construct B2..Ba, where Ba is the last block containing A[]
  //
  if (len_a > 14)
  {
    len_a -= 14;
    blocks = len_a >> 4;
    remainder = len_a & 0x0f;
    bptr = A+14;

    while (blocks--)
    {
      CCM_AUTH_B_DEBUG_TRACE(bptr);
      for (i=0; i < 16; i++)  Cstate[i] ^= *bptr++;
      CCM_AUTH_DEBUG_TRACE(Cstate);
      pSspAesEncrypt_Sw (AesKey, Cstate);
      CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);
    }

    if (remainder)
    {
      osal_memset (B, 0, 16);
      osal_memcpy (B, bptr, remainder);
      CCM_AUTH_B_DEBUG_TRACE(B);
      for (i=0; i < 16; i++)  Cstate[i] ^= B[i];
      CCM_AUTH_DEBUG_TRACE(Cstate);
      pSspAesEncrypt_Sw (AesKey, Cstate);
      CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);
    }
  }

  //
  // Construct Ba+1..Bm, where Bm is the last block containing M[]
  //
  blocks = len_m >> 4;
  remainder = len_m & 0x0f;
  bptr = M;

  while (blocks--)
  {
    CCM_AUTH_B_DEBUG_TRACE(bptr);
    for (i=0; i < 16; i++)  Cstate[i] ^= *bptr++;
    CCM_AUTH_DEBUG_TRACE(Cstate);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);
  }

  if (remainder)
  {
    osal_memset (B, 0, 16);
    osal_memcpy (B, bptr, remainder);
    CCM_AUTH_B_DEBUG_TRACE(B);
    for (i=0; i < 16; i++)  Cstate[i] ^= B[i];
    CCM_AUTH_DEBUG_TRACE(Cstate);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    CCM_AUTH_AESOUT_DEBUG_TRACE(Cstate);
  }

  return CCM_SUCCESS;
}

/******************************************************************************
 * @fn      SSP_CCM_Encrypt_Sw
 *
 * @brief   Performs CCM encryption.
 *
 * input parameters
 *
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   M       - Pointer to octet string 'm'
 * @param   len_m   - Length of M[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to Authentication Tag U
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   M[]         - Encrypted octet string 'm'
 * @param   Cstate[]    - The first Mval bytes contain Encrypted Authentication Tag U
 *
 * @return  uint8_t
 *
 */
uint8_t SSP_CCM_Encrypt_Sw (uint8_t Mval, uint8_t *N, uint8_t *M, uint16_t len_m,
                                                  uint8_t *AesKey, uint8_t *Cstate, uint8_t ccmLVal)
{
  uint8_t   A[16], T[16], *bptr;
  uint8_t   i, remainder;
  uint16_t  blocks, counter;

  osal_memcpy (T, Cstate, Mval);

  A[0] = ccmLVal - 1;               // L=2, L-encoding = L-1 = 1
  osal_memcpy (A+1, N, 13);   // append Nonce
  counter = 1;
  bptr = M;

  blocks = len_m >> 4;
  remainder = len_m & 0x0f;

  while (blocks--)
  {
    osal_memcpy (Cstate, A, 14);
    Cstate[14] = (uint8_t) (counter >> 8);
    Cstate[15] = (uint8_t) (counter);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    for (i=0; i < 16; i++) *bptr++ ^= Cstate[i];
    counter++;
  }

  if (remainder)
  {
    osal_memcpy (Cstate, A, 14);
    Cstate[14] = (uint8_t) (counter >> 8);
    Cstate[15] = (uint8_t) (counter);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    for (i=0; i < remainder; i++) *bptr++ ^= Cstate[i];
  }

  osal_memcpy (Cstate, A, 14);
  Cstate[14] = Cstate[15] = 0;        // A0
  pSspAesEncrypt_Sw (AesKey, Cstate);     // Cstate = S0

  for (i=0; i < Mval; i++)  Cstate[i] ^= T[i];

  return CCM_SUCCESS;
}


uint8_t SSP_CTR_Encrypt_Sw ( uint8_t *M, uint16_t len_m, uint8_t *AesKey, uint8_t *Nonce, uint8_t* IV)
{
  uint8_t   *bptr;
  uint8_t   i, remainder;
  uint16_t  blocks;
  uint8_t   Ctr[16]; // IV
  uint32_t  counter = 1;

  bptr = M;

  blocks = len_m >> 4;
  remainder = len_m & 0x0f;

  while (blocks--)
  {
    osal_memcpy (Ctr, Nonce, 4);
    osal_memcpy (Ctr + 4, IV, 8);
    Ctr[12] = (uint8_t) (counter >> 24);
    Ctr[13] = (uint8_t) (counter >> 16);
    Ctr[14] = (uint8_t) (counter >> 8);
    Ctr[15] = (uint8_t) (counter);

    pSspAesEncrypt_Sw (AesKey, Ctr);

    for (i=0; i < 16; i++)
    {
       *bptr++ ^= Ctr[i];
    }
    counter++;
  }

  if (remainder)
  {
    osal_memcpy (Ctr, Nonce, 4);
    osal_memcpy (Ctr + 4, IV, 8);
    Ctr[12] = (uint8_t) (counter >> 24);
    Ctr[13] = (uint8_t) (counter >> 16);
    Ctr[14] = (uint8_t) (counter >> 8);
    Ctr[15] = (uint8_t) (counter);
    pSspAesEncrypt_Sw (AesKey, Ctr);
    for (i=0; i < remainder; i++) *bptr++ ^= Ctr[i];
  }

  return CCM_SUCCESS;
}

/******************************************************************************
 * @fn      SSP_CCM_Decrypt_Sw
 *
 * @brief   Performs CCM decryption.
 *
 * input parameters
 *
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   C       - Pointer to octet string 'c', where 'c' = encrypted 'm' || encrypted auth tag U
 * @param   len_c   - Length of C[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer AES state buffer (cannot be part of C[])
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   C[]         - Decrypted octet string 'm' || auth tag T
 * @param   Cstate[]    - The first Mval bytes contain  Authentication Tag T
 *
 * @return  uint8_t
 *
 */
uint8_t SSP_CCM_Decrypt_Sw (uint8_t Mval, uint8_t *N, uint8_t *C, uint16_t len_c,
                                                     uint8_t *AesKey, uint8_t *Cstate, uint8_t ccmLVal)
{
  uint8_t   A[16], *bptr;
  uint16_t   i;
  uint8_t remainder;
  uint16_t  blocks, counter;

  A[0] = ccmLVal - 1;               // L=2, L-encoding = L-1 = 1
  osal_memcpy (A+1, N, 13);   // append Nonce
  counter = 1;
  bptr = C;

  i = len_c - Mval;
  blocks = i >> 4;
  remainder = i & 0x0f;

  while (blocks--)
  {
    osal_memcpy (Cstate, A, 14);
    Cstate[14] = (uint8_t) (counter >> 8);
    Cstate[15] = (uint8_t) (counter);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    for (i=0; i < 16; i++) *bptr++ ^= Cstate[i];
    counter++;
  }

  if (remainder)
  {
    osal_memcpy (Cstate, A, 14);
    Cstate[14] = (uint8_t) (counter >> 8);
    Cstate[15] = (uint8_t) (counter);
    pSspAesEncrypt_Sw (AesKey, Cstate);
    for (i=0; i < remainder; i++) *bptr++ ^= Cstate[i];
  }

  osal_memcpy (Cstate, A, 14);
  Cstate[14] = Cstate[15] = 0;    // A0
  pSspAesEncrypt_Sw (AesKey, Cstate); // Cstate = S0

  counter = len_c - Mval;
  for (i=0; i < Mval; i++)
  {
    Cstate[i] ^= C[counter];    // save T in Cstate[]
    C[counter++] = Cstate[i];   // replace U with T (last Mval bytes of C[])
  }

  return CCM_SUCCESS;
}


uint8_t SSP_CTR_Decrypt_Sw (uint8_t* pCipherTxt, uint16_t cipherTxtLen, uint8_t *AesKey, uint8_t* Nonce, uint8_t* IV)
{
  uint8_t   Ctr[16], *bptr;
  uint16_t   i;
  uint8_t remainder;
  uint16_t  blocks;
  uint32_t counter;

  counter = 1;
  bptr = pCipherTxt;

  blocks = cipherTxtLen >> 4;
  remainder = cipherTxtLen & 0x0f;

  while (blocks--)
  {
    osal_memcpy (Ctr, Nonce, 4);
    osal_memcpy (Ctr + 4, IV, 8);
    Ctr[12] = (uint8_t) (counter >> 24);
    Ctr[13] = (uint8_t) (counter >> 16);
    Ctr[14] = (uint8_t) (counter >> 8);
    Ctr[15] = (uint8_t) (counter);

    pSspAesEncrypt_Sw (AesKey, Ctr);

    for (i=0; i < 16; i++)
      *bptr++ ^= Ctr[i];
    counter++;
  }

  if (remainder)
  {
    osal_memcpy (Ctr, Nonce, 4);
    osal_memcpy (Ctr + 4, IV, 8);
    Ctr[12] = (uint8_t) (counter >> 24);
    Ctr[13] = (uint8_t) (counter >> 16);
    Ctr[14] = (uint8_t) (counter >> 8);
    Ctr[15] = (uint8_t) (counter);

    pSspAesEncrypt_Sw (AesKey, Ctr);
    for (i=0; i < remainder; i++) *bptr++ ^= Ctr[i];
  }

  return CCM_SUCCESS;
}

/******************************************************************************
 * @fn      SSP_CCM_InvAuth_Sw
 *
 * @brief   Verifies CCM authentication.
 *
 * input parameters
 *
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   C       - Pointer to octet string 'c' = 'm' || auth tag T
 * @param   len_c   - Length of C[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to AES state buffer (cannot be part of C[])
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain computed Authentication Tag T
 *
 * @return  0 = CCM_SUCCESS, 1 = CCM_FAILURE
 *
 */
uint8_t SSP_CCM_InvAuth_Sw (uint8_t Mval, uint8_t *N, uint8_t *C, uint16_t len_c, uint8_t *A,
                                        uint16_t len_a, uint8_t *AesKey, uint8_t *Cstate, uint8_t ccmLVal)
{
  uint8_t   i;
  uint16_t  t;
  uint8_t status=CCM_SUCCESS;

	// Check if authentication is even requested.  If not, return
	// CCM_SUCCESS and exit.  This check is actually not needed because
	// the rest of the code works fine even with Mval==0.  I added
	// it to reduce unnecessary calculations and to speed up
	// performance when Mval==0
  if (!Mval) return 0;

  t = len_c - Mval;

  MAP_SSP_CCM_Auth_Sw (Mval, N, C, t, A, len_a, AesKey, Cstate, ccmLVal);

  for (i=0; i < Mval; i++)
  {
    if (Cstate[i] != C[t++])
    {
      status = CCM_FAILURE;
      break;
    }
  }
  return (status);
}

/******************************************************************************
 * @fn      SSP_CCM_Auth_Encrypt_Sw
 *
 * @brief   CCM Authentication and Encryption.
 *
 * input parameters
 *
 * @param   encrypt  - set to true to encrypt, false for no encryption
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   M       - Pointer to octet string 'm'
 * @param   len_m   - Length of M[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to output buffer
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain Authentication Tag T
 *
 * @return  uint8_t
 *
 */
uint8_t SSP_CCM_Auth_Encrypt_Sw (uint8_t encrypt, uint8_t Mval, uint8_t *N,
                                  uint8_t *M, uint16_t len_m, uint8_t *A,
                                  uint16_t len_a, uint8_t *AesKey, uint8_t *Cstate,
                                  uint8_t ccmLVal)
{
  uint8_t status = CCM_SUCCESS;
  status = MAP_SSP_CCM_Auth_Sw(Mval, N, M, len_m, A, len_a, AesKey, Cstate, ccmLVal);

  if (status == CCM_SUCCESS && encrypt)
  {
    status = MAP_SSP_CCM_Encrypt_Sw(Mval, N, M, len_m, AesKey, Cstate, ccmLVal);
  }
  return status;
}

/******************************************************************************
 * @fn      SSP_CCM_InvAuth_Decrypt_Sw
 *
 * @brief   CCM Inverse authentication and Decryption.
 *
 * input parameters
 *
 * @param   decrypt  - set to true to decrypt, false for no decryption
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   C       - Pointer to octet string 'c' = 'm' || auth tag T
 * @param   len_c   - Length of C[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to AES state buffer (cannot be part of C[])
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain computed Authentication Tag T
 *
 * @return  uint8_t
 *
 */
uint8_t SSP_CCM_InvAuth_Decrypt_Sw (uint8_t decrypt, uint8_t Mval, uint8_t *N,
                                     uint8_t *C, uint16_t len_c, uint8_t *A,
                                     uint16_t len_a, uint8_t *AesKey, uint8_t *Cstate,
                                     uint8_t ccmLVal)
{
  uint8_t status = CCM_SUCCESS;

  if (decrypt)
  {
     status = MAP_SSP_CCM_Decrypt_Sw(Mval, N, C, len_c, AesKey, Cstate, ccmLVal);
  }

  if (status == CCM_SUCCESS)
  {
    status = MAP_SSP_CCM_InvAuth_Sw(Mval, N, C, len_c, A, len_a, AesKey, Cstate, ccmLVal);

  }
  return status;
}

