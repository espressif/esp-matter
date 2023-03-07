/******************************************************************************

 @file  sensor_gatt_profile.h

 @brief This file contains the 15.4 Sensor GATT profile definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2010-2020, Texas Instruments Incorporated
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

#ifndef SENSORPROFILEGATTPROFILE_H
#define SENSORPROFILEGATTPROFILE_H

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
// Sensor Profile Services bit fields
#define SENSORPROFILE_SERVICE               0x00000001

// Maximum user description length in bytes for characteristics
#define SENSORPROFILE_MAX_DESCRIPTION_LEN (23)

// Profile Parameters
#define SENSORPROFILE_TEMPERATURE_CHAR                  (0)  // RN uint8[2]- Target Addr characteristic
#define SENSORPROFILE_REPORT_INTERVAL_CHAR                  (1)  // RW uint8[4]- Target Addr characteristic

// Sensor Profile Service UUID
#define SENSORPROFILE_SERV_UUID               0x11A0
#define SENSORPROFILE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// Sensor UUID
#define SENSORPROFILE_CHAR_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

#define SENSORPROFILE_TEMPERATURE_CHAR_UUID               0x11A1
#define SENSORPROFILE_REPORT_INTERVAL_CHAR_UUID               0x11A2

// Length of Characteristics in bytes
#define SENSORPROFILE_TEMPERATURE_LEN                    (2)
#define SENSORPROFILE_REPORT_INTERVAL_LEN                (4)
#define SENSORPROFILE_GENERIC_LEN                        (1)

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
typedef void (*SensorProfileChange_t)( uint8 paramID );

typedef struct
{
  SensorProfileChange_t        pfnSensorProfileChange;  // Called when characteristic value changes
} SensorProfileCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * SensorProfile_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t SensorProfile_AddService( uint32 services );

/*
 * SensorProfile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t SensorProfile_RegisterAppCBs( SensorProfileCBs_t *appCallbacks );

/*
 * Sensorprofile_SetParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t SensorProfile_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * SensorProfile_GetParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t SensorProfile_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORPROFILEGATTPROFILE_H */
