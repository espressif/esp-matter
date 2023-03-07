/******************************************************************************

 @file  led_profile.c

 This file contains the profile implementation for the LED's
 and LED service.

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
#include <ti/drivers/apps/LED.h>
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "led_service.h"
#include "led_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * LOCAL FUNCTIONS
 */

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
 *  ======== LEDProfile_init ========
 */
bStatus_t LEDProfile_init()
{
  bStatus_t status = SUCCESS;

  // Open hardware
  LED_Params  ledParams;
  LED_init();

  /* Init LED params */
  LED_Params_init(&ledParams);
  ledParams.setState = LED_STATE_OFF;
  ledParams.pwmPeriod = 300;

  /* Open LED's */
  io.led.green = LED_open(GREEN_LED, &ledParams);
  io.led.red = LED_open(RED_LED, &ledParams);
  io.led.blue = LED_open(BLUE_LED, &ledParams);

  if (( io.led.green == NULL) || (io.led.red == NULL) || (io.led.blue == NULL))
  {
    Log_error0("LED(s) failed to open");
    return FAILURE;
  }
  else
  {
    Log_info0("LED's opened");
    io.led.ready = TRUE;
  }

  // Register with common sensors module
  status = Sensors_registerCbs(LED_SERVICE_SERV_UUID, processCBs);

  if (status == SUCCESS)
  {
    // Add service
    status = LedService_AddService();
  }

  if (status == SUCCESS)
  {
    // Register callbacks with service
    status = LedService_RegisterProfileCBs(&sensorServiceCBs);
  }

  if (status == SUCCESS)
  {
    Log_info0("LED Profile Initialized");
  }
  else
  {
    Log_error1("LED Profile failed to initialize with status 0x%x", status);
  }

  return status;
}

/*********************************************************************
* PRIVATE FUNCTIONS
*/

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

  LED_Handle handle = NULL;

  // This must be larger than the largest string used below
  #define LED_STRING_SIZE 10
  static uint8_t led_string[LED_STRING_SIZE];
  switch (pCharData->paramID)
  {
    case LS_LED0_ID:
    {
      osal_memcpy(led_string, "Red", LED_STRING_SIZE);
      handle = io.led.red;
    }
    break;

    case LS_LED1_ID:
    {
      osal_memcpy(led_string, "Green", LED_STRING_SIZE);
      handle = io.led.green;
    }
    break;

    case LS_LED2_ID:
    {
      osal_memcpy(led_string, "Blue", LED_STRING_SIZE);
      handle = io.led.blue;
    }
    break;
  }

  if (pCharData->data[0] == 0)
  {
    LED_setOff(handle);
    Log_info1("%s LED turned off", (uintptr_t) led_string);
  }
  else
  {
    LED_setOn(handle, 50);
    Log_info1("%s LED turned on", (uintptr_t) led_string);
  }

  return SUCCESS;
}
