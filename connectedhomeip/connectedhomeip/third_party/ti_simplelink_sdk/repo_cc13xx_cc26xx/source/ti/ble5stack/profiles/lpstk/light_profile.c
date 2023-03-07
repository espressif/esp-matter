/******************************************************************************

 @file  light_profile.c

 This file contains the profile implementation for the Light sensor
 and Light service.

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
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/common/sail/opt3001/opt3001.h>
#include <sail_syscfg_workaround.h>
#include "util.h"
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "light_service.h"
#include "light_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for internal periodic event
static Clock_Struct clkPeriodic;

// Pointer to I2C handle
static I2C_Handle *pI2cHandle = NULL;

// Parameters used to initialize OPT3001 sensor
static OPT3001_Params opt3001Params;

// Flag to indicate whether the profile has been initialized
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
 *  ======== LightProfile_init ========
 */
bStatus_t LightProfile_init(I2C_Handle *pHandle)
{
  bStatus_t status = SUCCESS;

  if ((pHandle == NULL) || (*pHandle == NULL))
  {
    return INVALIDPARAMETER;
  }

  // Store pointer to i2c handle
  pI2cHandle = pHandle;

  // Initialize params
  OPT3001_Params_init(&opt3001Params);
  // Enable sensor
  status  = LightProfile_enable();

  // Register with common sensors module
  if (status == SUCCESS)
  {
    status = Sensors_registerCbs(LIGHT_SERV_UUID, processCBs);
  }

  if (status == SUCCESS)
  {
    // Add service
    status = Light_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = Light_RegisterProfileCBs(&sensorServiceCBs);
  }

  if (status == SUCCESS)
  {
    // Create one-shot clock for periodic sensor reading event
    uint8_t period = 0;
    Light_GetParameter(LIGHT_PERIOD_ID, &period);
    Util_constructClock(&clkPeriodic, Sensors_clockHandler,
                        period * SENSOR_PERIOD_RESOLUTION, 0,
                        false, (UArg)profile_readSensor);

    profile_initialized = TRUE;
    Log_info0("Light Profile Initialized");
  }
  else
  {
    Log_error1("Light Profile failed to initialize with status 0x%x", status);
  }

  return status;
}

/*
 *  ======== LightProfile_enable ========
 */
bStatus_t LightProfile_enable()
{
  bStatus_t status = SUCCESS;

  // If not already opened
  if (io.opt == NULL)
  {
    // Open hardware
    OPT3001_init();
    // Open sensor
    io.opt = OPT3001_open(BOARD_OPT3001_LIGHT, *pI2cHandle, &opt3001Params);
  }

  if (io.opt == NULL)
  {
    Log_error0("OPT sensor failed to open");

    status = FAILURE;
  }
  else
  {
    Log_info0("OPT sensor enabled");
    if (profile_initialized == TRUE)
    {
      // Start periodic readings if unless this is the initialization
      Log_info0("Light Readings enabled");
      Util_startClock(&clkPeriodic);
    }
  }

  return status;
}

/*
 *  ======== LightProfile_disable ========
 */
bStatus_t LightProfile_disable()
{
  bStatus_t status = SUCCESS;

  // Only disable if it is enabled
  if (io.opt != NULL)
  {
    // Stop periodic readings
    Log_info0("Light readings disabled");
    Util_stopClock(&clkPeriodic);

    // Close sensor
    if (OPT3001_close(io.opt) == TRUE)
    {
      // Clear handle
      Log_info0("OPT sensor closed");
      io.opt = NULL;
    }
  }

  return status;
}

/*********************************************************************
* PRIVATE FUNCTIONS
*/

/**
 * Read Lux from OPT3001 sensor and send over-the-air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  if (io.opt != NULL)
  {
    // Read lux
    float lux = 0;
    OPT3001_getLux(io.opt, &lux);
    Log_info1("Lux: %d", (uint32_t)lux);

    // Set profile value (and notify if notifications are enabled)
    Light_SetParameter(LIGHT_DATA_ID, sizeof(float), &lux);

    // Start the next period
    Util_startClock(&clkPeriodic);
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
        LightProfile_disable();
      }
      else if (*(pCharData->data) == ST_CFG_SENSOR_ENABLE)
      {
        Util_startClock(&clkPeriodic);

        LightProfile_enable();
      }
    }
    break;

    case SENSOR_PERI:
    {
      uint32_t new_period = *(pCharData->data) * SENSOR_PERIOD_RESOLUTION;
      Util_restartClock(&clkPeriodic, new_period);

      Log_info1("Light reading period updated to %d ms", new_period);
    }
    break;
  }

  return SUCCESS;
}
