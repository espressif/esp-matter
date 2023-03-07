/******************************************************************************

 @file  hal_aes.c

 @brief Support for Hardware AES encryption.

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
#include "hal_mcu.h"
#include "hal_ccm.h"
#include <inc/hw_crypto.h>
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
void (*pSspAesEncrypt)( uint8 *, uint8 * ) = (void*)NULL;

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/******************************************************************************
 * @fn      HalAesInit
 *
 * @brief   Initilize AES engine
 *
 * input parameters
 *
 * @param   None
 *
 * @return  None
 */
void HalAesInit( void )
{
  HWREG(CRYPTO_BASE + CRYPTO_O_IRQTYPE) |= 0x00000001;
  HWREG(CRYPTO_BASE + CRYPTO_O_IRQEN) |= 0x00000003;
}


/******************************************************************************
 * @fn      ssp_HW_KeyInit
 *
 * @brief   Writes the key into AES engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 *
 * @return  None
 */
void ssp_HW_KeyInit( uint8 *AesKey )
{
  /* Load the AES key
   * KeyStore has rentention after PM2
   */
  CRYPTOAesLoadKey( (uint32_t *)AesKey, 0);
}


/******************************************************************************
 * @fn      sspAesEncryptHW
 *
 * @brief   Encrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesEncryptHW( uint8 *AesKey, uint8 *Cstate )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)Cstate, (uint32_t *)Cstate, 0, true, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(CRYPTOAesEcbStatus());

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesDecryptHW
 *
 * @brief   Decrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to decrypted data.
 *
 * @return  None
 *
 */
void sspAesDecryptHW( uint8 *AesKey, uint8 *Cstate )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)Cstate, (uint32_t *)Cstate, 0, false, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(!(CRYPTOAesEcbStatus()));

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesEncryptHW_keylocation
 *
 * @brief   Encrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesEncryptHW_keylocation(uint8 *msg_in, uint8 *msg_out, uint8 key_location )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)msg_in, (uint32_t *)msg_out, key_location, true, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(!(CRYPTOAesEcbStatus()));

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesDecryptHW_keylocation
 *
 * @brief   Decrypts 16 byte block using AES decryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesDecryptHW_keylocation( uint8 *msg_in, uint8 *msg_out, uint8 key_location )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)msg_in, (uint32_t *)msg_out, key_location, false, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(CRYPTOAesEcbStatus());

  CRYPTOAesEcbFinish();
}


