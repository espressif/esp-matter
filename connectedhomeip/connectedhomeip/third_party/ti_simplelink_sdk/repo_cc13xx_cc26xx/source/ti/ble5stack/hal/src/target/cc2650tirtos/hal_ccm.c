/******************************************************************************

 @file  hal_ccm.c

 @brief Support for Hardware CCM authentication.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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
#include <driverlib/crypto.h>


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

/******************************************************************************
 * @fn      SSP_CCM_Auth_Encrypt
 *
 * @brief   Generates CCM Authentication tag U.
 *
 * input parameters
 * @param encrypt if set to 'true' then run encryption and set to 'flase' for
 * authentication only.
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
 * @return  ZStatus_t
 *
 */
uint8 SSP_CCM_Auth_Encrypt (bool encrypt, uint8 Mval, uint8 *N, uint8 *M, uint16 len_m, uint8 *A,
                    uint16 len_a, uint8 *AesKey, uint8 *Cstate, uint8 ccmLVal)
{

  unsigned char status;

  if((status = CRYPTOCcmAuthEncrypt(encrypt, Mval, (uint32_t *)N, (uint32_t *)M,  len_m, (uint32_t *)A, len_a, 0,
                                            ccmLVal, false)) != AES_SUCCESS)
  {
    return status;
  }

  do
  {
    ASM_NOP;
  }while((CRYPTOCcmAuthEncryptStatus()));

  if((status = CRYPTOCcmAuthEncryptResultGet(Mval, (uint32_t *)Cstate)) != AES_SUCCESS)
  {
    return status;
  }
  return AES_SUCCESS;
}


/******************************************************************************
 * @fn      SSP_CCM_InvAuth_Decrypt
 *
 * @brief   Verifies CCM authentication.
 *
 * input parameters
 * @param decrypt if set to 'true' then run decryption and set to 'flase' for
 * authentication only.
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
 * @return  0 = Success, 1 = Failure
 *
 */
uint8 SSP_CCM_InvAuth_Decrypt (bool decrypt, uint8 Mval, uint8 *N, uint8 *C, uint16 len_c, uint8 *A,
                       uint16 len_a, uint8 *AesKey, uint8 *Cstate, uint8 ccmLVal)
{

  unsigned char status;
  if((status = CRYPTOCcmInvAuthDecrypt(decrypt, Mval, (uint32_t *)N, (uint32_t *)C, len_c, (uint32_t *)A, len_a, 0,
                                         ccmLVal, false))!= AES_SUCCESS )
  {
    return status;
  }

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while((CRYPTOCcmInvAuthDecryptStatus()));


  if((status = CRYPTOCcmInvAuthDecryptResultGet(Mval, (uint32_t *)C, len_c, (uint32_t *)Cstate)) != AES_SUCCESS)
  {
    return status;
  }

  return AES_SUCCESS;
}







