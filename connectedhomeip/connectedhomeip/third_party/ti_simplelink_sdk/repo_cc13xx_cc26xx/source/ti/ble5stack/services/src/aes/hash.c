/******************************************************************************

 @file  hash.c

 @brief Provides the keyed hash functions for message authentication.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2005-2022, Texas Instruments Incorporated
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

#if   ( SECURE != 0  )

/******************************************************************************
 * INCLUDES
 */

#include    "osal.h"

#if defined ( MAC_MOTO_D18 ) || defined ( MC13192 )
  #include "SecurityLib.h"
#elif defined ( CC2420DB )
   #include "CC2420DB\ccm.h"
#endif

/******************************************************************************
 * MACROS
 */

/******************************************************************************
 * CONSTANTS
 */

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

//void SSP_KeyedHash (uint8 *, uint16, uint8 *, uint8 *);
void sspMMOHash (uint8 *, uint8, uint8 *, uint16, uint8 *);

/******************************************************************************
 * @fn      SSP_KeyedHash
 *
 * @brief   Performs the Keyed-Hash Message Authentication Code (HMAC) as
 *          defined in FIPS-198.  Assumes block size = key size = 128 bits.
 *
 * input parameters
 *
 * @param   M           - Pointer to message block to hash
 * @param   bitlen      - Length of M[] in bits
 * @param   Aeskey      - Pointer to AES Key
 * @param   Cstate      - Pointer to Hash output buffer
 *
 * output parameters
 *
 * @param   Cstate[]    - HMAC output
 *
 * @return  None
 */
void SSP_KeyedHash (uint8 *M, uint16 bitlen, uint8 *AesKey, uint8 *Cstate)
{
  uint8   Kx[16], T[16], i;

  //
  // Since block_size = key_size = 128 bits, no additional key
  // processing is needed, so go directly to HMAC step 4 (XORing
  // with ipad).
  //

  for (i=0; i < 16; i++)
  {
    Kx[i] = AesKey[i] ^ 0x36;           // Kx[] = Key1 = AesKey ^ ipad
  }

  sspMMOHash (Kx, 1, M, bitlen, Cstate);  // Cstate[] = Hash1 = Hash(Key1,M)

  osal_memcpy (T, Cstate, 16);            // T = Hash1

  for (i=0; i < 16; i++)
  {
    Kx[i] = AesKey[i] ^ 0x5c;           // Kx[] = Key2 = AesKey ^ opad
  }

  sspMMOHash (Kx, 1, T, 128, Cstate);     // Cstate[] = Hash2 = Hash(Key2,Hash1)
}

/******************************************************************************
 * @fn      sspMMOHash
 *
 * @brief   Performs the Matyas-Meyer-Oseas hash function on 1 or 2 blocks
 *          of data using AES-128.
 *
 * input parameters
 *
 * @param   P       - Pointer to prefix block (if any) to hash
 * @param   prefix  - 1 if 16-byte prefix block exists, 0 otherwise
 * @param   M       - Pointer to message block to hash
 * @param   bitlen  - Length of M[] in bits
 * @param   Cstate  - Pointer to Hash output buffer
 *
 * output parameters
 *
 * @param   Cstate[]    - Hash output
 *
 * @return  None
 */
void sspMMOHash (uint8 *P, uint8 prefix, uint8 *M, uint16 bitlen, uint8 *Cstate)
{
  uint16  blocks, blength;
  uint8   T[16], i, remainder, *dptr;
  uint8   AesKey[16];

#ifdef  USE_KEY_EXPANSION
  uint8   *KeyExp;
  KeyExp = osal_mem_alloc (KEY_EXP_LENGTH);

  if ( KeyExp == NULL )
    return;                       // check...return error ?
#endif  // USE_KEY_EXPANSION

  blength = bitlen >> 3;
  blocks = blength >> 4;
  remainder = blength & 0x0f;

  osal_memset (Cstate, 0, 16);        // Cstate == Hash0

  //
  // If prefix is true, then the message to hash comes in two pieces.
  // The first piece is a 16-byte octet stored in P[], and the second
  // piece is the 'bitlen'-bit message stored in M[].
  //
  if (prefix)     // Process first 16-bytes in P[]
  {
    osal_memcpy (AesKey, Cstate, 16);   // Key = Hash0
    osal_memcpy (Cstate, P, 16);

#ifdef  USE_KEY_EXPANSION
    sspKeyExpansion (AesKey, KeyExp);
    sspAesEncrypt (KeyExp, Cstate);     // Cstate[] = E(Hash0,P)
#else
    sspAesEncrypt (AesKey, Cstate);     // Cstate[] = E(Hash0,P)
#endif

    for (i=0; i < 16; i++)
    {
      Cstate[i] ^= P[i];  // Cstate[] = Hash1 = E(Hash0,P) ^ P
    }
    bitlen += 128;      // adjust bitlen because of the extra 16-bytes of prefix
  }

  dptr = M;
  while (blocks--)
  {
    osal_memcpy (AesKey, Cstate, 16);   // Key = Hash(i)
    osal_memcpy (Cstate, dptr, 16);

#ifdef  USE_KEY_EXPANSION
    sspKeyExpansion (AesKey, KeyExp);
    sspAesEncrypt (KeyExp, Cstate);     // Cstate[] = E(Hash(i-1), M(i))
#else
    sspAesEncrypt (AesKey, Cstate);     // Cstate[] = E(Hash(i-1), M(i))
#endif

    for (i=0; i < 16; i++)
    {
      Cstate[i] ^= *dptr++;  // Cstate[] = Hash(i) = E(Hash(i-1),M(i)) ^ M(i)
    }
  }

  if (remainder < 14)  //  0 <= remainder <= 13
  {
    //
    // There is room left in the final block to fit the 3 bytes of fill data.
    // 0x80 goes in the next available byte.  The bit-length goes in the last
    // two bytes.
    //
    osal_memcpy (AesKey, Cstate, 16);   // Key = Hash(i)
    osal_memset(Cstate, 0, 14);
    osal_memcpy (Cstate, dptr, remainder);
    Cstate[remainder] = 0x80;
    Cstate[14] = (uint8) (bitlen >> 8);
    Cstate[15] = (uint8) (bitlen);
    osal_memcpy (T, Cstate, 16);

#ifdef  USE_KEY_EXPANSION
    sspKeyExpansion (AesKey, KeyExp);
    sspAesEncrypt (KeyExp, Cstate);     // Cstate[] = E(Hash(i-1), T)
#else
    sspAesEncrypt (AesKey, Cstate);     // Cstate[] = E(Hash(i-1), T)
#endif

    for (i=0; i < 16; i++)
    {
      Cstate[i] ^= T[i];  // Cstate[] = Hash(i) = E(Hash(i-1),T) ^ T
    }
  }
  else                // remainder == (14 or 15)
  {
    // Cannot fit the minimum 3 fill bytes into current block, so we need to fill out the
    // current block and then create another 16-byte fill block.
    //
    osal_memcpy (AesKey, Cstate, 16);   // Key = Hash(i)
    osal_memset (Cstate, 0, 16);
    osal_memcpy (Cstate, dptr, remainder);
    Cstate[remainder] = 0x80;
    osal_memcpy (T, Cstate, 16);

#ifdef  USE_KEY_EXPANSION
    sspKeyExpansion (AesKey, KeyExp);
    sspAesEncrypt (KeyExp, Cstate);     // Cstate[] = E(Hash(i-1), T)
#else
    sspAesEncrypt (AesKey, Cstate);     // Cstate[] = E(Hash(i-1), T)
#endif

    for (i=0; i < 16; i++)
    {
      Cstate[i] ^= T[i];  // Cstate[] = Hash(i) = E(Hash(i-1),T) ^ T
    }

    //
    // Create final 16-byte fill block.
    //
    osal_memcpy (AesKey, Cstate, 16);   // Key = Hash(i)
    osal_memset (Cstate, 0, 14);
    Cstate[14] = (uint8) (bitlen >> 8);
    Cstate[15] = (uint8) (bitlen);
    osal_memcpy (T, Cstate, 16);

#ifdef  USE_KEY_EXPANSION
    sspKeyExpansion (AesKey, KeyExp);
    sspAesEncrypt (KeyExp, Cstate);     // Cstate[] = E(Hash(i-1), T)
#else
    sspAesEncrypt (AesKey, Cstate);     // Cstate[] = E(Hash(i-1), T)
#endif

    for (i=0; i < 16; i++)
    {
      Cstate[i] ^= T[i];  // Cstate[] = Hash(i) = E(Hash(i-1),T) ^ T
    }
  }

#ifdef  USE_KEY_EXPANSION
  osal_mem_free (KeyExp);
#endif

}

/*********************************************************************
*********************************************************************/
#endif	// SECURE

