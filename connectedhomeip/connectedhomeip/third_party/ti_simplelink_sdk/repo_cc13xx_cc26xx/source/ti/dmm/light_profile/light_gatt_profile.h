/******************************************************************************

 @file  light_gatt_profile.h

 @brief This file contains the 15.4 Light GATT profile definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2010-2021, Texas Instruments Incorporated
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

#ifndef LIGHTPROFILEGATTPROFILE_H
#define LIGHTPROFILEGATTPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Light Profile Services bit fields
#define LIGHTPROFILE_SERVICE               0x00000001

// Maximum user description length in bytes for characteristics
#define LIGHTPROFILE_MAX_DESCRIPTION_LEN (23)

// Profile Parameters
#define LIGHTPROFILE_LIGHT_ONOFF_CHAR                  (0)  // RW uint8 - Light On/Off characteristic
#define LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR             (1)  // RW uint8 - Target Addr Type characteristic
#define LIGHTPROFILE_TARGET_ADDR_CHAR                  (2)  // RW uint8[2]- Target Addr characteristic
#define LIGHTPROFILE_BAT_LEVEL_CHAR                    (3)  // R uint8 - Switch Battery Level characteristic
#define LIGHTPROFILE_TARGET_ENDPOINT_CHAR              (4)  // RW uint8 - Target Endpoint characteristic

// Light Profile Service UUID
#define LIGHTPROFILE_SERV_UUID               0x11A0
#define LIGHTPROFILE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// Light UUID
#define LIGHTPROFILE_CHAR_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

#define LIGHTPROFILE_LIGHT_ONOFF_CHAR_UUID               0x11A1
#define LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR_UUID          0x11A2
#define LIGHTPROFILE_TARGET_ADDR_CHAR_UUID               0x11A3
#define LIGHTPROFILE_BAT_LEVEL_CHAR_UUID                 0x11A4
#define LIGHTPROFILE_TARGET_ENDPOINT_CHAR_UUID           0x11A5

// Length of Characteristics in bytes
#define LIGHTPROFILE_LIGHT_ONOFF_LEN                    (1)
#define LIGHTPROFILE_TARGET_ADDR_TYPE_LEN               (1)
#define LIGHTPROFILE_TARGET_ADDR_LEN                    (2)
#define LIGHTPROFILE_BAT_LEVEL_LEN                      (1)
#define LIGHTPROFILE_TARGET_ENDPOINT_LEN                (1)
#define LIGHTPROFILE_GENERIC_LEN                        (1)

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
typedef void (*lightProfileChange_t)( uint8 paramID );

typedef struct
{
  lightProfileChange_t        pfnLightProfileChange;  // Called when characteristic value changes
} lightProfileCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * LightProfile_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t LightProfile_AddService( uint32 services );

/*
 * LightProfile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t LightProfile_RegisterAppCBs( lightProfileCBs_t *appCallbacks );

/*
 * LightProfile_SetParameter - Set a Light GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t LightProfile_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * LightProfile_GetParameter - Get a Light GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t LightProfile_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* LIGHTPROFILEGATTPROFILE_H */
