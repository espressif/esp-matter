/******************************************************************************

 @file  sm_ecc.c

 @brief This file contains the SM ECC Event for P-256 and Diffie-Hellman keys
        When a network processor is used.

 Group: WCS, BTS
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

#include "bcomdef.h"
#include "osal.h"

#include "hci.h"
#include "sm.h"

#include "rom_jt.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Application Task ID.  Used to send ECC Key events when an network processor
// application task is registered to do so.
extern uint8 SM_appTaskID;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      SM_p256KeyCB
 *
 * @brief   Read P-256 Key Event Callback from the Controller.  The secret key
 *          used to generate the public key pair is also input here.
 *
 * @params  pK         - pointer to HCI BLE Read P256 Public Key Complete struct
 *                       which contain the X and Y Coordinates of the ECC Public
 *                       Keys.
 *
 * @param   privateKey - the private key used to generated the ECC Public Keys.
 *
 * @return  None
 */
void SM_p256KeyCB( hciEvt_BLEReadP256PublicKeyComplete_t *pK, uint8 *privateKey )
{
  smEccKeysEvt_t *eccKeys;

  eccKeys = (smEccKeysEvt_t *)MAP_osal_msg_allocate( sizeof (smEccKeysEvt_t) );

  if ( eccKeys )
  {
    eccKeys->hdr.event = SM_MSG_EVENT;
    eccKeys->hdr.status = pK->status;
    eccKeys->opcode = SM_ECC_KEYS_EVENT;

    // If key generation was successful
    if (pK->status == SUCCESS)
    {
      MAP_osal_memcpy( eccKeys->privateKey, privateKey, SM_ECC_KEY_LEN );
      MAP_osal_memcpy( eccKeys->publicKeyX, pK->p256Key, SM_ECC_KEY_LEN );
      MAP_osal_memcpy( eccKeys->publicKeyY, &pK->p256Key[SM_ECC_KEY_LEN],
                       SM_ECC_KEY_LEN );
    }

    // Send to app.
    MAP_osal_msg_send( SM_appTaskID, (uint8 *)eccKeys );
  }
}

/*********************************************************************
 * @fn      SM_dhKeyCB
 *
 * @brief   BLE Generate Diffie-Hellman Key Event Callback from the controller.
 *
 * @params           - pointer to HCI BLE Read P256 Public Key Complete struct
 *                       which contain the X and Y Coordinates of the ECC Public
 *                       Keys.
 *
 * @param   pDhKey - the Diffie-Hellman X and Y Coordinates output from the
 *                   ECDH operation.
 *
 * @return  None
 */
void SM_dhKeyCB( hciEvt_BLEGenDHKeyComplete_t *pDhKey )
{
  smDhKeyEvt_t *dhKey;

  dhKey = (smDhKeyEvt_t *)MAP_osal_msg_allocate( sizeof (smDhKeyEvt_t) );

  if ( dhKey )
  {
    dhKey->hdr.event = SM_MSG_EVENT;
    dhKey->hdr.status = pDhKey->status;
    dhKey->opcode = SM_DH_KEY_EVENT;

    if ( pDhKey->status == SUCCESS )
    {
      MAP_osal_memcpy( dhKey->dhKey,pDhKey->dhKey, SM_ECC_KEY_LEN );
    }
    MAP_osal_msg_send( SM_appTaskID, (uint8 *)dhKey );
  }
}


/*********************************************************************
*********************************************************************/
