/******************************************************************************

 @file  oad_sap.h

 @brief This file contains OAD Profile header file.

 Group: WCS, BTS
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
#ifndef OAD_SAP_H
#define OAD_SAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include <ti/sysbios/knl/Queue.h>

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

#if defined FEATURE_OAD_SECURE
static_assert((sizeof(img_hdr_t) == 16), "Bad SBL_ADDR_AES_HDR definition.");
static_assert(((sizeof(img_hdr_t) % KEY_BLENGTH) == 0),
                      "img_hdr_t is not an even multiple of KEY_BLENGTH");

// The AES Header must be encrypted and the Signature must include the Image Header.
typedef struct {
  uint8 signature[KEY_BLENGTH];  // The AES-128 CBC-MAC signature.
  uint8 nonce12[12];             // The 12-byte Nonce for calculating the signature.
  uint8 spare[4];
} aes_hdr_t;
#endif //FEATURE_OAD_SECURE

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*oadWriteCB_t)(uint8_t event, uint16_t connHandle,
                             uint8_t *pData, uint16_t dataLen);

typedef struct
{
  oadWriteCB_t       pfnOadWrite; // Called when characteristic value changes.
} oadTargetCBs_t;

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      OAD_addService
 *
 * @brief   Initializes the OAD Service by registering GATT attributes
 *          with the GATT server. Only call this function once.
 *
 * @param   None.
 *
 * @return  Success or Failure
 */
extern uint8_t OAD_addService(void);

#ifdef IMAGE_INVALIDATE
/*********************************************************************
 * @fn      Reset_addService
 *
 * @brief   Initializes the Reset Service by registering GATT attributes
 *          with the GATT server. Only call this function once.
 *
 * @param   None.
 *
 * @return  Success or Failure
 */
extern uint8_t Reset_addService(void);
#endif //IMAGE_INVALIDATE

/*********************************************************************
 * @fn      OAD_register
 *
 * @brief   Register a callback function with the OAD Target Profile.
 *
 * @param   pfnOadCBs - struct holding function pointers to OAD application
 *                      callbacks.
 *
 * @return  None
 */
extern void OAD_register(oadTargetCBs_t *pfnOadCBs);

/*********************************************************************
 * @fn      OAD_imgIdentifyWrite
 *
 * @brief   Process the Image Identify Write.  Determine from the received OAD
 *          Image Header if the Downloaded Image should be acquired.
 *
 * @param   connHandle - connection message was received on
 * @param   pValue     - pointer to data to be written
 *
 * @return  status
 */
extern void OAD_imgIdentifyWrite(uint16 connHandle, uint8 *pValue);

/*********************************************************************
 * @fn      OAD_imgBlockWrite
 *
 * @brief   Process the Image Block Write.
 *
 * @param   connHandle - connection message was received on
 * @param   pValue     - pointer to data to be written
 *
 * @return  status
 */
extern void OAD_imgBlockWrite(uint16 connHandle, uint8 *pValue);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OAD_SAP */
