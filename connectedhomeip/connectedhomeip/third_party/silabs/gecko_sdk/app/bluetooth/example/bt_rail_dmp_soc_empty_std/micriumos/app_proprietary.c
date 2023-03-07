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
#include "os.h"
#include "rail.h"
#include "em_common.h"
#include "app_assert.h"
#include "app_proprietary.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
#include "sl_flex_util_802154_protocol_types.h"
#include "sl_flex_util_802154_init_config.h"
#include "sl_flex_rail_ieee802154_config.h"
#include "sl_flex_ieee802154_support.h"
#include "sl_flex_util_802154_init.h"
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
#include "sl_flex_util_ble_protocol_config.h"
#include "sl_flex_util_ble_init_config.h"
#include "sl_flex_util_ble_init.h"
#else
#endif
// -----------------------------------------------------------------------------
// Constant definitions and macros
#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/// BLE channel number
#define BLE_CHANNEL ((uint8_t) 0)
#endif
// OS task parameters
#define APP_PROPRIETARY_TASK_PRIO         6u
#define APP_PROPRIETARY_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))

// OS task variables
static CPU_STK app_proprietary_task_stack[APP_PROPRIETARY_TASK_STACK_SIZE];
static OS_TCB app_proprietary_task_tcb;
static volatile bool app_rail_busy = true;

// OS event to prevent cyclic execution of the task main loop.
OS_FLAG_GRP app_proprietary_event_flags;

/**************************************************************************//**
 * Proprietary application task.
 *
 * @param[in] p_arg Unused parameter required by the OS API.
 *****************************************************************************/
static void app_proprietary_task(void *p_arg);

/******************************************************************************
 * RAIL callback, called after the RAIL's initialization finished.
 *****************************************************************************/
void sl_rail_util_on_rf_ready(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;
  app_rail_busy = false;
}

/**************************************************************************//**
 * Proprietary application init.
 *****************************************************************************/
void app_proprietary_init()
{
  RTOS_ERR err;

  while (app_rail_busy) {
  }

  // Create the Proprietary Application task.
  OSTaskCreate(&app_proprietary_task_tcb,
               "App Proprietary Task",
               app_proprietary_task,
               0u,
               APP_PROPRIETARY_TASK_PRIO,
               &app_proprietary_task_stack[0u],
               (APP_PROPRIETARY_TASK_STACK_SIZE / 10u),
               APP_PROPRIETARY_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Task creation failed" APP_LOG_NEW_LINE,
             (int)err.Code);

  // Initialize the flag group for the proprietary task.
  OSFlagCreate(&app_proprietary_event_flags, "Prop. flags", (OS_FLAGS)0, &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Event flag creation failed" APP_LOG_NEW_LINE,
             (int)err.Code);
}

static void app_proprietary_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;

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

   rail_handle = sl_flex_util_get_handle();
 #ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
   // init the selected protocol for IEEE, first
   sl_flex_ieee802154_protocol_init(rail_handle, SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
   // Start reception.
   status = RAIL_StartRx(rail_handle, sl_flex_ieee802154_get_channel(), NULL);
 #elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
   status = RAIL_StartRx(rail_handle, BLE_CHANNEL, NULL);
 #else
 #endif
   app_assert(status == RAIL_STATUS_NO_ERROR,
             "[E: 0x%04x] Failed to start RAIL reception" APP_LOG_NEW_LINE,
             (int)status);
 */

  // Start task main loop.
  while (DEF_TRUE) {
    // Wait for the event flag to be set.
    OSFlagPend(&app_proprietary_event_flags,
               APP_PROPRIETARY_EVENT_FLAG,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);
    app_assert(err.Code == RTOS_ERR_NONE,
               "[E: 0x%04x] Prop event flag pend error" APP_LOG_NEW_LINE,
               (int)err.Code);

    ///////////////////////////////////////////////////////////////////////////
    // Put your additional application code here!                            //
    // This is called when the event flag is set with OSFlagPost.            //
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
  PP_UNUSED_PARAM(rail_handle);
  PP_UNUSED_PARAM(events);

  /////////////////////////////////////////////////////////////////////////////
  // Add event handlers here as your application requires!                   //
  //                                                                         //
  // Flex (RAIL) - Simple TRX Standards might serve as a good example on how //
  // to implement this event handler properly.                               //
  /////////////////////////////////////////////////////////////////////////////
}
