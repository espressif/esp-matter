/***************************************************************************//**
 * @file
 * @brief Core proprietary application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "em_common.h"
#include "app_assert.h"
#include "app_proprietary.h"

/// Proprietary Task Parameters
#define APP_PROPRIETARY_TASK_NAME         "app_proprietary"
#define APP_PROPRIETARY_TASK_STACK_SIZE   200
#define APP_PROPRIETARY_TASK_PRIO         5u

StackType_t app_proprietary_task_stack[APP_PROPRIETARY_TASK_STACK_SIZE] = { 0 };
StaticTask_t app_proprietary_task_buffer;
TaskHandle_t app_proprietary_task_handle;

/// OS event group to prevent cyclic execution of the task main loop.
EventGroupHandle_t app_proprietary_event_group_handle;
StaticEventGroup_t app_proprietary_event_group_buffer;

/**************************************************************************//**
 * Proprietary application task.
 *
 * @param[in] p_arg Unused parameter required by the OS API.
 *****************************************************************************/
static void app_proprietary_task(void *p_arg);

/**************************************************************************//**
 * Proprietary application init.
 *****************************************************************************/
void app_proprietary_init()
{
  // Create the Proprietary Application task.
  app_proprietary_task_handle = xTaskCreateStatic(app_proprietary_task,
                                                  APP_PROPRIETARY_TASK_NAME,
                                                  APP_PROPRIETARY_TASK_STACK_SIZE,
                                                  NULL,
                                                  APP_PROPRIETARY_TASK_PRIO,
                                                  app_proprietary_task_stack,
                                                  &app_proprietary_task_buffer);

  app_assert(NULL != app_proprietary_task_handle,
             "Task creation failed" APP_LOG_NEW_LINE);

  // Initialize the flag group for the proprietary task.
  app_proprietary_event_group_handle =
    xEventGroupCreateStatic(&app_proprietary_event_group_buffer);

  app_assert(NULL != app_proprietary_event_group_handle,
             "Event group creation failed" APP_LOG_NEW_LINE);
}

static void app_proprietary_task(void *p_arg)
{
  (void)p_arg;
  EventBits_t event_bits;

  /////////////////////////////////////////////////////////////////////////////
  //                                                                         //
  // The following code snippet shows how to start simple receiving, as an   //
  // example, within a DMP application. However, it is commented out to      //
  // demonstrate the lowest possible power consumption as well.              //
  //                                                                         //
  // PLEASE NOTE: ENABLING CONSTANT RECIEVING HAS HEAVY IMPACT ON POWER      //
  // CONSUMPTION OF YOUR PRODUCT.                                            //
  //                                                                         //
  // For further examples on sending / recieving in a DMP application, and   //
  // also on reducing the overall power demand, the following Flex projects  //
  // could serve as a good starting point:                                   //
  //                                                                         //
  //  - Flex (Connect) - Soc Empty Example DMP                               //
  //  - Flex (RAIL) - Range Test DMP                                         //
  //  - Flex (RAIL) - Energy Mode                                            //
  //                                                                         //
  // See also: AN1134: Dynamic Multiprotocol Development with Bluetooth and  //
  //                   Proprietary Protocols on RAIL in GSDK v2.x            //
  /////////////////////////////////////////////////////////////////////////////

/*
   RAIL_Handle_t rail_handle;
   RAIL_Status_t status;

   rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
   // Start reception.
   status = RAIL_StartRx(rail_handle, 0, NULL);
   app_assert(status == RAIL_STATUS_NO_ERROR,
          "[E: 0x%04x] Failed to start RAIL reception" APP_LOG_NEW_LINE,
          (int)status);
 */

  // Start task main loop.
  while (1) {
    // Wait for the event bit to be set.
    event_bits = xEventGroupWaitBits(app_proprietary_event_group_handle,
                                     APP_PROPRIETARY_EVENT_FLAG,
                                     pdTRUE,
                                     pdFALSE,
                                     portMAX_DELAY);

    app_assert(event_bits & APP_PROPRIETARY_EVENT_FLAG,
               "Wrong event bit is set!" APP_LOG_NEW_LINE);

    ///////////////////////////////////////////////////////////////////////////
    // Put your additional application code here!                            //
    // This is called when the event flag APP_PROPRIETARY_EVENT_FLAG is set  //
    ///////////////////////////////////////////////////////////////////////////
  }
}

/**************************************************************************//**
 * This callback is called on registered RAIL events.
 * Overrides dummy weak implementation.
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle,
                           RAIL_Events_t events)
{
  (void)rail_handle;
  (void)events;

  /////////////////////////////////////////////////////////////////////////////
  // Add event handlers here as your application requires!                   //
  //                                                                         //
  // Flex (RAIL) - Simple TRX might serve as a good example on how to        //
  // implement this event handler properly.                                  //
  /////////////////////////////////////////////////////////////////////////////
}
