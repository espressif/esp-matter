/******************************************************************************

 @file  zaesccm_osal.c

 @brief Support for CCM authentication using osal.

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

#include "osal.h"
#include "hal_aes.h"
#include "hal_ccm.h"
#include "hal_assert.h"
#include "zaesccm_api.h"
#include "hal_mcu.h"

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

  unsigned char status;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );

 /* Make sure AES is not already in-use */
  if(aesInUse)
  {
    HAL_EXIT_CRITICAL_SECTION( intState );
    return AES_NO_RESOURCES;
  }

  aesInUse = TRUE;

  /* Initialize AES key */
  ssp_HW_KeyInit( AesKey );

  /* This is required for the MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  status = SSP_CCM_Auth_Encrypt (encrypt, Mval, Nonce, M, len_m, A,
                        len_a, AesKey, MAC, ccmLVal);

  aesInUse = FALSE;
  HAL_EXIT_CRITICAL_SECTION( intState );
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
  unsigned char status;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );

  /* Make sure AES is not already in-use */
  if(aesInUse)
  {
    HAL_EXIT_CRITICAL_SECTION( intState );
    return AES_NO_RESOURCES;
  }
  aesInUse = TRUE;

  /* Initialize AES key */
  ssp_HW_KeyInit( AesKey );

  /* This is required for the MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  status = SSP_CCM_InvAuth_Decrypt(decrypt, Mval, Nonce, M, len_m, A, len_a,
                          AesKey, MAC, ccmLVal);
  aesInUse = FALSE;
  HAL_EXIT_CRITICAL_SECTION( intState );
  return (status);
}








