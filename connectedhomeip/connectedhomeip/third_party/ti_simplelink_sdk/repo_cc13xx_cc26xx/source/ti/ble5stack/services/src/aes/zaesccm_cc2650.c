/******************************************************************************

 @file  zaesccm_cc2650.c

 @brief Support for CCM authentication using ICall and TIRTOS .

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#include "zaesccm_api.h"
#include "hal_mcu.h"
#include <driverlib/crypto.h>
#include "crypto_board.h"

/******************************************************************************
 * MACROS
 */
#define AES_NO_RESOURCES              0x1A

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

/* Flag to ensure AES not reentrant */
volatile uint8 aesInUse = FALSE;

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/* see zaesccm_api.h */
signed char zaesccmAuthEncrypt(unsigned char encrypt,
                               unsigned char Mval, unsigned char *Nonce,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *MAC, unsigned char ccmLVal)
{
  char status;

  /* Initialize AES key */
  status = CryptoBoard_AesLoadKey(AesKey, AESKEY_0);

  if(status == AES_SUCCESS)
  {
    /* CCM encrypt and authenticate */
    status = CryptoBoard_CCMEncrypt(encrypt, Mval, Nonce, M, len_m, A,
                                    len_a, MAC, ccmLVal, AESKEY_0);
    CryptoBoard_AesReleaseKey(AESKEY_0);
  }

  return status;
}


/* see zaesccm_api.h */
signed char zaesccmDecryptAuth(unsigned char decrypt,
                               unsigned char Mval, unsigned char *Nonce,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *MAC, unsigned char ccmLVal)
{
  char status;
  /* Initialize AES key */
  status = CryptoBoard_AesLoadKey(AesKey, AESKEY_0);

  if(status == AES_SUCCESS)
  {
    /* CCM inverse authenticate and decrypt  */
    status = CryptoBoard_CCMDecrypt(decrypt, Mval, Nonce, M, len_m, A,
                                    len_a, MAC, ccmLVal, AESKEY_0);
    CryptoBoard_AesReleaseKey(AESKEY_0);
  }
  return status;
}








