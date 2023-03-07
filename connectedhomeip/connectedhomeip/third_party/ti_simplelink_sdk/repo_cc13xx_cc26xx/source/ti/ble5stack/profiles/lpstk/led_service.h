/******************************************************************************

 @file  led_service.h

 This file contains LEd Service header file.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

#ifndef _LED_SERVICE_H_
#define _LED_SERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <sensor_common.h>

/*********************************************************************
 * CONSTANTS
 */
// Service UUID
#define LED_SERVICE_SERV_UUID 0x1110

// LED0 Characteristic defines
#define LS_LED0_ID                 0
#define LS_LED0_UUID               0x1111
#define LS_LED0_LEN                1

// LED1 Characteristic defines
#define LS_LED1_ID                 1
#define LS_LED1_UUID               0x1112
#define LS_LED1_LEN                1

// LED2 Characteristic defines
#define LS_LED2_ID                 2
#define LS_LED2_UUID               0x1113
#define LS_LED2_LEN                1

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*LedServiceChange_t)(uint16_t connHandle, uint8_t paramID,
                                   uint16_t len, uint8_t *pValue);

typedef struct
{
    LedServiceChange_t pfnChangeCb;          // Called when characteristic value changes
    LedServiceChange_t pfnCfgChangeCb;       // Called when characteristic CCCD changes
} LedServiceCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/**
 * Initializes the LED service by registering GATT attributes with
 * the GATT server
 *
 * @return bleMemAllocError
 * @return SUCCESS
 */
extern bStatus_t LedService_AddService();

/**
 * Registers the profile callback
 *
 * For LPSTK sensor services, this should be the common sensors profile
 *
 * profileCallbacks - pointer to application callbacks.
 */
extern bStatus_t LedService_RegisterProfileCBs(sensorServiceCBs_t *profileCallbacks);

/**
 * Set an LED Service parameter.
 *
 * @param   param - service parameter ID
 * @param   len   - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *                  the parameter ID and WILL be cast to the appropriate
 *                  data type (example: data type of uint16 will be cast to
 *                  uint16 pointer).
 *
 * @return bleInvalidRange invalid length
 * @return INVALIDPARAMETER not a valid param
 * @return SUCCESS
 */
extern bStatus_t LedService_SetParameter(uint8_t param,
                                         uint16_t len,
                                         void *value);

/**
 * Get an LED Service parameter.
 *
 * @param param service parameter ID
 * @param value pointer to data to write.  This is dependent on the parameter ID
 *        and WILL be cast to the appropriate data type (example: data type
 *        of uint16 will be cast to uint16 pointer)
 *
 * @return INVALIDPARAMETER invalid param
 * @return SUCCESS
 */
extern bStatus_t LedService_GetParameter(uint8_t param,
                                         uint16_t *len,
                                         void *value);

#ifdef __cplusplus
}
#endif

#endif /* _LED_SERVICE_H_ */
