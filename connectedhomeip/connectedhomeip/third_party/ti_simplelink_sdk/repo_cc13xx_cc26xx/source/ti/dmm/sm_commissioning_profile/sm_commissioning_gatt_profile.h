/**********************************************************************************************
 * Filename:       sm_commissioning_gatt_profile.h
 *
 * Description:    This file contains the sm_commissioning_gatt_profile service definitions and
 *                 prototypes.
 *
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


#ifndef _SM_COMMISSIONING_GATT_PROFILE_H_
#define _SM_COMMISSIONING_GATT_PROFILE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <bcomdef.h>

/*********************************************************************
* CONSTANTS
*/
// Provisioning Profile Services bit fields
#define SMCOMMPROFILE_SERVICE               0x00000001

// Service UUID
#define SMCOMMISSIONING_PROFILE_SERV_UUID 0xBAAA
#define SMCOMMISSIONING_PROFILE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// Maximum user description length in bytes for characteristics
#define SMCOMMISSIONING_MAX_DESCRIPTION_LEN (23)

// Provisioning characteristic UUID mapping
#define SMCOMMISSIONING_PROFILE_CHAR_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

//  Characteristic defines
#define SMCOMMISSIONING_PROFILE_STATECHAR_ID   0
#define SMCOMMISSIONING_PROFILE_STATECHAR_UUID 0xBAAB
#define SMCOMMISSIONING_PROFILE_STATECHAR_LEN  1

#ifdef DMM_SENSOR
//  Characteristic defines
#define SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID   1
#define SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID 0xBAAC
#define SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN  1
#endif

//  Characteristic defines
#define SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_ID   3
#define SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID 0xBAAD
#define SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_LEN  4

// SM State Machine Values
static uint8_t SMCOMMISSIONSTATE_IDLE             = 0;
static uint8_t SMCOMMISSIONSTATE_STARTING         = 1;
static uint8_t SMCOMMISSIONSTATE_PASSKEY_REQUEST  = 2;
static uint8_t SMCOMMISSIONSTATE_PASSKEY_TIMEOUT  = 3;
static uint8_t SMCOMMISSIONSTATE_COMMISSIONING    = 4;
static uint8_t SMCOMMISSIONSTATE_SUCCESS          = 5;
static uint8_t SMCOMMISSIONSTATE_FAIL             = 6;

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*sm_commissioning_profileChange_t)(uint8_t paramID);

typedef struct
{
    sm_commissioning_profileChange_t        pfnSmCommissioningProfileChangeCb;  // Called when characteristic value changes
} sm_commissioning_profileCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * SmCommissioning_AddService- Initializes the Sm_commissioning_gatt_profile service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t SmCommissioning_AddService( uint32 services );

/*
 * SmCommissioning_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t SmCommissioning_RegisterAppCBs( sm_commissioning_profileCBs_t *appCallbacks );

/*
 * SmCommissioning_SetParameter - Set a Sm_commissioning_gatt_profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t SmCommissioning_SetParameter(uint8_t param, uint16_t len, void *value);

/*
 * SmCommissioning_GetParameter - Get a Sm_commissioning_gatt_profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t SmCommissioning_GetParameter(uint8_t param, void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SM_COMMISSIONING_GATT_PROFILE_H_ */
