/******************************************************************************

 @file  hall_profile.c

 This file contains the profile implementation for the Hall sensor
 and battery service.

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

/*********************************************************************
 * INCLUDES
 */

#include "ti_drivers_config.h"
#include "string.h"
#include <ti/drivers/ADC.h>
#include <ti/common/sail/drv5055/drv5055.h>
#include "util.h"
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "hall_service.h"
#include "hall_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * CONSTANTS
 */

#define SENSITIVITY DRV5055A4_3_3V
#define OFFSET 1650.0f

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for internal periodic event
static Clock_Struct clkPeriodic;

static bool profile_initialized = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t profile_readSensor(uint8_t *params);
static bStatus_t profile_processValueChange(uint8_t *pParam);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Service callback function implementation
static sensorProcessCBs_t processCBs =
{
  .pfnProcessVal = profile_processValueChange, // Characteristic value change callback handler
  .pfnProcessCfg = NULL, // No CCCD change handler implemented
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== HallProfile_init ========
 */
bStatus_t HallProfile_init(void)
{
  bStatus_t status = SUCCESS;

  // Enable sensor
  status  = HallProfile_enable();

  if (status == SUCCESS)
  {
    // Register with common sensors module
    status = Sensors_registerCbs(HALL_SERV_UUID, processCBs);
  }

  if (status == SUCCESS)
  {
    // Add service
    status = Hall_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = Hall_RegisterProfileCBs(&sensorServiceCBs);
  }

  if (status == SUCCESS)
  {
    // Create periodic clock
    uint8_t period = 0;
    Hall_GetParameter(HALL_PERIOD_ID, &period );
    Util_constructClock(&clkPeriodic, Sensors_clockHandler, 0,
                        period * SENSOR_PERIOD_RESOLUTION,
                        false, (UArg)profile_readSensor);

    profile_initialized = TRUE;

    Log_info0("Hall Profile Initialized");
  }
  else
  {
    Log_error1("Hall Profile failed to initialize with status 0x%x", status);
  }

  return status;
}

/*
 *  ======== HallProfile_enable ========
 */
bStatus_t HallProfile_enable(void)
{
  bStatus_t status = SUCCESS;

  // If not already opened
  if (io.adc == NULL)
  {
    // Open ADC
    io.adc = ADC_open(CONFIG_ADC0, NULL);
  }

  if (io.adc == NULL)
  {
    Log_error0("ADC failed to open");

    status = FAILURE;
  }
  else
  {
    Log_info0("ADC enabled");
    Log_info0("Hall sensor enabled");
    if (profile_initialized == TRUE)
    {
      // Start periodic readings if unless this is the initialization
      Log_info0("Hall Readings enabled");
      Util_startClock(&clkPeriodic);
    }
  }

  return status;
}

/*
 *  ======== HallProfile_disable ========
 */
bStatus_t HallProfile_disable(void)
{
  bStatus_t status = SUCCESS;

  // Only disable ADC if it is enabled
  if (io.adc != NULL)
  {
    // Stop periodic readings
    Log_info0("Hall readings disabled");
    Util_stopClock(&clkPeriodic);

    // Close sensor
    ADC_close(io.adc);

    // Clear handle
    Log_info0("ADC closed");
    io.adc = NULL;
  }

  return status;
}

/*********************************************************************
* Private functions
*/

/**
 * Read magnetic flux from DRV5055 sensor and send over-the-air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  if (io.adc != NULL)
  {
    // Read magnetic flux
    float magneticFlux = DRV5055_getMagneticFlux(io.adc, SENSITIVITY, OFFSET,
                                                 DRV5055_3_3V);
    Log_info1("Magnetic Flux: %d", (uint32_t)magneticFlux);

    // Set profile value (and notify if notifications are enabled)
    Hall_SetParameter(HALL_DATA_ID, sizeof(float), &magneticFlux);
  }

  return SUCCESS;
}

/**
 * Handle the changing of a characteristic value
 *
 * @param characteristic data: should be cast to char_data_t
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_processValueChange(uint8_t *pParam)
{
  char_data_t *pCharData = (char_data_t *) pParam;

  switch (pCharData->paramID)
  {
    case SENSOR_CONF:
    {
      if (*(pCharData->data) == ST_CFG_SENSOR_DISABLE)
      {
        HallProfile_disable();
      }
      else if (*(pCharData->data) == ST_CFG_SENSOR_ENABLE)
      {
        Util_startClock(&clkPeriodic);

        HallProfile_enable();
      }
    }
    break;

    case SENSOR_PERI:
    {
      uint32_t new_period = *(pCharData->data) * SENSOR_PERIOD_RESOLUTION;
      Util_restartClock(&clkPeriodic, new_period);

      Log_info1("Hall reading period updated to %d ms", new_period);
    }
    break;
  }

  return SUCCESS;
}
