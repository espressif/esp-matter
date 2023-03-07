/******************************************************************************

 @file  button_profile.c

 This file contains the profile implementation for the buttons and button service

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
#include <ti/drivers/apps/Button.h>
#include "util.h"
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "button_service.h"
#include "button_profile.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t profile_readSensor(uint8_t *params);
static void button_callback(Button_Handle buttonHandle,
                            Button_EventMask buttonEvents);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Service callback function implementation
static sensorProcessCBs_t processCBs =
{
  .pfnProcessVal = NULL, // Characteristic value change callback handler
  .pfnProcessCfg = NULL, // No CCCD change handler implemented
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== ButtonProfile_init ========
 */
bStatus_t ButtonProfile_init()
{
  bStatus_t status = FAILURE;

  // Open hardware
  Button_init();
  Button_Params buttonParams;
  Button_Params_init(&buttonParams);
  io.button.zero = Button_open(CONFIG_BUTTON_0, &buttonParams);
  io.button.one = Button_open(CONFIG_BUTTON_1, &buttonParams);
  Button_setCallback(io.button.zero, button_callback);
  Button_setCallback(io.button.one, button_callback);

  if ((io.button.zero != NULL) && (io.button.one != NULL))
  {
    Log_info0("Buttons enabled");

    // Register with common sensors module
    status = Sensors_registerCbs(BUTTON_SERVICE_SERV_UUID, processCBs);
  }

  if (status == SUCCESS)
  {
    // Add service
    status = ButtonService_AddService();
  }

  if (status == SUCCESS)
  {
    Log_info0("Button Profile Initialized");
  }
  else
  {
    Log_error1("Button Profile failed to initialize with status 0x%x", status);
  }

  return status;
}

/*********************************************************************
* PRIVATE FUNCTIONS
*/

/**
 * Callback from button driver to post event to application task for
 * processing
 *
 * @warning This is the SWI context
 */
static void button_callback(Button_Handle buttonHandle,
                            Button_EventMask buttonEvents)
{
  // Allocate container for button event
  buttonEventData_t *pData = (buttonEventData_t*)ICall_malloc(sizeof(buttonEventData_t));

  if (pData != NULL)
  {
    // Fill up Container
    pData->buttonEvents = buttonEvents;
    pData->buttonHandle = buttonHandle;

    // Defer to process in application context
    Sensors_defer(profile_readSensor, (uint8_t *)pData);
  }
}

/**
 * Read button event and send over the air
 *
 * @param params not used
 *
 * @return Always SUCCESS
 */
static bStatus_t profile_readSensor(uint8_t *params)
{
  bStatus_t status = SUCCESS;
  buttonEventData_t *pEvent = (buttonEventData_t *) params;

  uint8_t char_val;
  // This must be bigger than longest possible string
  #define ACTION_STRINGSIZE 10
  static uint8_t action_string[ACTION_STRINGSIZE];
  if (pEvent->buttonEvents & Button_EV_PRESSED)
  {
    char_val = 1;
    osal_memcpy(action_string, "Pressed", ACTION_STRINGSIZE);
  }
  else if (pEvent->buttonEvents & Button_EV_RELEASED)
  {
    char_val = 0;
   osal_memcpy(action_string, "Released", ACTION_STRINGSIZE);
  }

  if (pEvent->buttonHandle == io.button.zero)
  {
    status = ButtonService_SetParameter(BS_BUTTON0_ID, BS_BUTTON0_LEN, &char_val);
    Log_info1("Button zero %s", (uintptr_t) action_string);
  }
  else if (pEvent->buttonHandle == io.button.one)
  {
    status = ButtonService_SetParameter(BS_BUTTON1_ID, BS_BUTTON1_LEN, &char_val);
    Log_info1("Button one %s", (uintptr_t) action_string);
  }

  if (status != SUCCESS)
  {
    Log_error1("Failed to set button parameter with status 0x%x", status);
  }

  return SUCCESS;
}
