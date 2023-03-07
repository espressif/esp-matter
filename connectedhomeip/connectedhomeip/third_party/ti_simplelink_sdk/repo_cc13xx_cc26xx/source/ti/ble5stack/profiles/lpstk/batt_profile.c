/******************************************************************************

 @file  batt_profile.c

 This file contains the profile implementation for the battery monitor
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
#include "util.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "batt_service.h"
#include "batt_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for internal periodic event
static Clock_Struct clkPeriodic;

static bool profile_enabled = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t profile_readSensor(uint8_t *params);
static bStatus_t profile_processCfgChange(uint8_t *pParam);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Service callback function implementation
static sensorProcessCBs_t processCBs =
{
  .pfnProcessVal = NULL, // No characteristic value's will change
  .pfnProcessCfg = profile_processCfgChange, // CCCD will be updated
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== BatteryProfile_init ========
 */
bStatus_t BatteryProfile_init(void)
{
  bStatus_t status = SUCCESS;

  // Register with common sensors module
  status = Sensors_registerCbs(BATT_SERV_UUID, processCBs);

  if (status == SUCCESS)
  {
    // Add service
    status = Battery_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = Battery_RegisterProfileCBs(&sensorServiceCBs);
  }

  if (status == SUCCESS)
  {
    // Create periodic clock
    Util_constructClock(&clkPeriodic, Sensors_clockHandler,
                        0, SENSOR_DEFAULT_PERIOD, false,
                        (UArg)profile_readSensor);

    // Stop clock
    // TODO. Why doesn't false flag above work?
    Util_stopClock(&clkPeriodic);

    Log_info0("Battery Profile Initialized");
  }

  return status;
}

/*
 *  ======== BatteryProfile_enable ========
 */
bStatus_t BatteryProfile_enable(void)
{
  // Start periodic readings if unless this is the initialization
  Log_info0("Battery readings enabled");
  Util_startClock(&clkPeriodic);

  profile_enabled = TRUE;

  return SUCCESS;
}

/*
 *  ======== BatteryProfile_disable ========
 */
bStatus_t BatteryProfile_disable(void)
{
  // Stop periodic readings
  Log_info0("Battery readings disabled");
  Util_stopClock(&clkPeriodic);

  profile_enabled = FALSE;

  return SUCCESS;
}

/*********************************************************************
* Private functions
*/

/**
 * Read battery data from battery monitor, convert to percentage,
 * and send update over the air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  bStatus_t status = FAILURE;

  if (profile_enabled == TRUE)
  {
    uint32_t milivolts = (AONBatMonBatteryVoltageGet()*125)>>5;
    uint8_t percent  = (milivolts * 100) / BATT_MAX_VOLTAGE;
    // Adjust for rounding error
    if (percent > 100){
      percent = 100;
    }
    Log_info1("Battery: %d", percent);

    // Set profile value (and notify if notifications are enabled)
    status = Battery_SetParameter(BATTERY_LEVEL_ID, BATTERY_LEVEL_LEN, &percent);

    if (status != SUCCESS)
    {
      Log_error1("Setting battery parameter filed with status 0x%x", status);
    }
  }

  return SUCCESS;
}

/**
 * Handle the changing of a client characteristic configuration
 *
 * This will enable / disable the battery readings
 *
 * @param characteristic data: should be cast to char_data_t
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_processCfgChange(uint8_t *pParam)
{
  char_data_t *pCharData = (char_data_t *) pParam;

  switch (pCharData->paramID)
  {
    case BATTERY_LEVEL_ID:
    {
      BatteryProfile_enable();
    }
    break;
  }

  return SUCCESS;
}

/*********************************************************************
*********************************************************************/
