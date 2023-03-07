/******************************************************************************

@file  sensor_temperature8.c

@brief  simulate temperature sensor

Group: WCS, BTS
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

/*********************************************************************
* INCLUDES
*/
#include "autoconf.h"
#include "sensor_models.h"
#include "sensor_temperature8.h"

/*********************************************************************
* TYPEDEFS
*/
typedef struct
{
  int8_t currentTemperature;
} sensorTmpr8_data_t;

typedef struct
{
  sensorDescriportState_t descriptorState;
  sensorTmpr8_data_t dataState;
} sensorTmpr8_states_t;

/*********************************************************************
* LOCAL VARIABLES
*/
sensorTmpr8_states_t sensorTmpr8_states =
{
     .descriptorState =
     {
         .propertyId = TEMPERATURE_SENSOR_PROPERTY_ID,
         .PositiveTolerance = 0x10,
         .NegativeTolerance = 0x20,
         .SamplingFunction  = 0x03,
         .MeasurementPeriod = 0x40,
         .UpdateInterval    = 0x50,
     },

     .dataState =
     {
         .currentTemperature = 72,  // 36 degree Celsius
     }
};

sensorDescriportState_t *sensorTmpr8_getDescriptor();
int sensorTmpr8_getTemperature(uint8_t **pData, uint16_t *dataLen);

static sensorModels_CBs_t sensorTmpr8_sensorCBs =
{
    sensorTmpr8_getTemperature,
    sensorTmpr8_getDescriptor
};

/*********************************************************************
 * @fn      sensorTmpr8_register
 *
 * @brief   register the sensor into the sensor models
 *
 */
int sensorTmpr8_register(uint8_t elem_idx)
{
    sensorModels_registerModel(elem_idx, TEMPERATURE_SENSOR_PROPERTY_ID, &sensorTmpr8_sensorCBs, sizeof(sensorTmpr8_data_t));
    return 0;
}
/*********************************************************************
 * @fn      sensorTmpr8_setTemperature
 *
 * @brief   set temperature
 *
 */
void sensorTmpr8_setTemperature(int8_t value)
{
    sensorTmpr8_states.dataState.currentTemperature = value;
}

/*********************************************************************
 * @fn      sensorTmpr8_getTemperature
 *
 * @brief   get temperature
 *
 */
int sensorTmpr8_getTemperature(uint8_t **pData, uint16_t *dataLen)
{
    *pData = (uint8_t *)&sensorTmpr8_states.dataState.currentTemperature;
    *dataLen = sizeof(sensorTmpr8_states.dataState.currentTemperature);

    return 0;
}

/*********************************************************************
 * @fn      sensorTmpr8_getDescriptor
 *
 * @brief   get sensor descripter
 *
 */
sensorDescriportState_t *sensorTmpr8_getDescriptor()
{
    return &sensorTmpr8_states.descriptorState;
}
