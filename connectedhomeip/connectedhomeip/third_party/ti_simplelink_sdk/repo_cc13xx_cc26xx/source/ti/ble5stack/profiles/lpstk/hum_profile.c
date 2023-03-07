/******************************************************************************

 @file  hum_profile.c

 This file contains the profile implementation for the Humidity sensor
 and Humidity service.

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
/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/common/sail/hdc2010/hdc2010.h>
#include <sail_syscfg_workaround.h>
#include "util.h"
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "hum_service.h"
#include "hum_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for internal periodic event
static Clock_Struct clkPeriodic;

// Pointer to I2C handle
static I2C_Handle *pI2cHandle = NULL;

// Parameters to initialize the HDC sensor
static HDC2010_Params  hdc2010Params;

// Flag to indicate whether the profile is initialized
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
 *  ======== HumProfile_init ========
 */
bStatus_t HumProfile_init(I2C_Handle *pHandle)
{
  bStatus_t status = FAILURE;

  if ((pHandle == NULL) || (*pHandle == NULL))
  {
    return INVALIDPARAMETER;
  }

  // Store pointer to i2c handle
  pI2cHandle = pHandle;

  // Set sensor params
  HDC2010_Params_init(&hdc2010Params);
  hdc2010Params.humResolution = HDC2010_H14_BITS;
  hdc2010Params.tempResolution = HDC2010_T14_BITS;
  hdc2010Params.interruptPinPolarity = HDC2010_ACTIVE_LO;
  hdc2010Params.interruptEn = HDC2010_ENABLE_MODE;
  hdc2010Params.interruptMask = (HDC2010_InterruptMask) (HDC2010_TH_MASK | HDC2010_TL_MASK | HDC2010_HH_MASK | HDC2010_HL_MASK);
  hdc2010Params.interruptMode = HDC2010_COMP_MODE;
  hdc2010Params.measurementMode = HDC2010_HT_MODE;
  // Enable sensor
  status  = HumProfile_enable();

  // Register with common sensors module
  status = Sensors_registerCbs(HUMIDITY_SERV_UUID, processCBs);

  if (status == SUCCESS)
  {
    // Add service
    status = Humidity_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = Humidity_RegisterProfileCBs(&sensorServiceCBs);
  }

  if (status == SUCCESS)
  {
    // Create periodic clock
    uint8_t period = 0;
    Humidity_GetParameter(HUMIDITY_PERIOD_ID, &period );
    Util_constructClock(&clkPeriodic, Sensors_clockHandler, 0,
                        period * SENSOR_PERIOD_RESOLUTION,
                        false, (UArg)profile_readSensor);

    profile_initialized = TRUE;
    Log_info0("Humidity Profile Initialized");
  }
  else
  {
    Log_error1("Humidity Profile failed to initialize with status 0x%x", status);
  }

  return status;
}

/*
 *  ======== HumProfile_enable ========
 */
bStatus_t HumProfile_enable()
{
  bStatus_t status = SUCCESS;

  // If not already opened
  if (io.hdc.handle == NULL)
  {
    // Open hardware
    HDC2010_init();
    // Open sensor
    io.hdc.handle = HDC2010_open(BOARD_HDC2010, *pI2cHandle, &hdc2010Params);
  }

  if (io.hdc.handle == NULL)
  {
    Log_error0("HDC sensor failed to open");

    status = FAILURE;
  }
  else
  {
    io.hdc.hum_enabled = TRUE;
    Log_info0("HDC sensor enabled");
    if (profile_initialized == TRUE)
    {
      // Start periodic readings if unless this is the initialization
      Log_info0("Humidity Readings enabled");
      Util_startClock(&clkPeriodic);
    }
  }

  return status;
}

/*
 *  ======== HumProfile_disable ========
 */
bStatus_t HumProfile_disable()
{
  bStatus_t status = SUCCESS;

  // Only disable if it is enabled
  if ((io.hdc.handle != NULL) && (io.hdc.hum_enabled == TRUE))
  {
    // Clear global enabled flag
    io.hdc.hum_enabled = FALSE;

    // Stop periodic readings
    Log_info0("Humidity Readings disabled");
    Util_stopClock(&clkPeriodic);

    // Can only close if the temperature service is also disabled
    if (io.hdc.temp_enabled == FALSE)
    {
      // Close sensor
      if (HDC2010_close(io.hdc.handle) == TRUE)
      {
        // Clear handle
        Log_info0("HDC sensor closed");
        io.hdc.handle = NULL;
      }
      else
      {
        status = FAILURE;
      }
    }
  }
  return status;
}

/*********************************************************************
* Private functions
*/

/**
 * Read humidity from HD2010 sensor and send over-the-air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  if (io.hdc.hum_enabled == TRUE)
  {
    // Read humidity
    float humidity = 0;
    HDC2010_triggerMeasurement(io.hdc.handle);
    HDC2010_getHum(io.hdc.handle, &humidity);
    Log_info1("Humidity: %d", (uint32_t)humidity);

    // Set profile value (and notify if notifications are enabled)
    Humidity_SetParameter(HUMIDITY_DATA_ID, sizeof(float), &humidity);
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
        HumProfile_disable();
      }
      else if (*(pCharData->data) == ST_CFG_SENSOR_ENABLE)
      {
        HumProfile_enable();
      }
    }
    break;

    case SENSOR_PERI:
    {
      uint32_t new_period = *(pCharData->data) * SENSOR_PERIOD_RESOLUTION;
      Util_restartClock(&clkPeriodic, new_period);

      Log_info1("Humidity reading period updated to %d ms", new_period);
    }
    break;
  }

  return SUCCESS;
}
