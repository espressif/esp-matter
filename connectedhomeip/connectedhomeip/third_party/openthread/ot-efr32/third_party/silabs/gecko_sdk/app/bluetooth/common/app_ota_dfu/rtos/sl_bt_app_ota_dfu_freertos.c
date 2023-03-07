/***************************************************************************//**
 * @file
 * @brief Core Application OTA DFU FreeRTOS logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "semphr.h"
#include "em_common.h"
#include "app_assert.h"
#include "sl_bt_app_ota_dfu.h"
#include "sli_bt_app_ota_dfu.h"

#include "btl_interface.h"
#include "btl_interface_storage.h"

/// Task names and stacksize.
#define APP_OTA_DFU_TASK_NAME         "app_ota_dfu"
#define APP_OTA_DFU_TASK_STACK        1024u
#define APP_OTA_DFU_TASK_PRIO         5u

/// Task stack definition and identifier.
StackType_t app_ota_dfu_task_stack[APP_OTA_DFU_TASK_STACK] = { 0 };
StaticTask_t app_ota_dfu_task_buffer;
TaskHandle_t app_ota_dfu_task_handle;

/// Binary semaphore to run the task only if its really necessary.
static SemaphoreHandle_t xSemaphoreOn;

static sl_bt_app_ota_dfu_status_evt_t ota_event;

/// Task function-prototype.
static void app_ota_dfu_task(void *p_arg);

/**************************************************************************//**
 * Strong sl_bt_app_ota_dfu_init() function implementation for FreeRTOS.
 *****************************************************************************/
void sl_bt_app_ota_dfu_init(void)
{
  // Initialize binary semaphor for scheduling.
  xSemaphoreOn = xSemaphoreCreateBinary();

  app_ota_dfu_task_handle =
    xTaskCreateStatic(app_ota_dfu_task, APP_OTA_DFU_TASK_NAME,
                      APP_OTA_DFU_TASK_STACK, NULL,
                      APP_OTA_DFU_TASK_PRIO, app_ota_dfu_task_stack,
                      &app_ota_dfu_task_buffer);
  app_assert(NULL != app_ota_dfu_task_handle,
             "App OTA DFU task creation failed!");
}

/**************************************************************************//**
 * Strong sl_bt_app_ota_dfu_restart_progress() function implementation for
 * FreeRTOS.
 *****************************************************************************/
void sl_bt_app_ota_dfu_restart_progress(void)
{
  // Delete current semaphore.
  vSemaphoreDelete(xSemaphoreOn);
  // Delete current task.
  vTaskDelete(app_ota_dfu_task_handle);
  // Do the initialization again.
  sl_bt_app_ota_dfu_init();
}

/**************************************************************************//**
 * Strong implementation of sli_bt_app_ota_dfu_proceed() for FreeRTOS.
 * This function realize the semaphore-based scheduling of the
 * app_ota_dfu_task().
 * This is required to avoid unnecessary power-consumption because of unwanted
 * task cycles.
 *****************************************************************************/
void sli_bt_app_ota_dfu_proceed(void)
{
  BaseType_t ret_val;
  // Trigger app_ota_dfu_task() to proceed.
  ret_val = xSemaphoreGive(xSemaphoreOn);
  configASSERT(ret_val == pdPASS);
}

/**************************************************************************//**
 * Application OTA DFU task content.
 *****************************************************************************/
static void app_ota_dfu_task(void *p_arg)
{
  (void)p_arg;

  int32_t boot_retv = BOOTLOADER_OK;
  sl_bt_app_ota_dfu_error_t ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;
  sl_bt_app_ota_dfu_status_t ota_sts = SL_BT_APP_OTA_DFU_UNINIT;

  // Bootloader init must be called before using bootloader_* API calls!
  boot_retv = bootloader_init();
  if (boot_retv == BOOTLOADER_OK) {
    ota_sts = SL_BT_APP_OTA_DFU_INIT;
  } else {
    ota_sts = SL_BT_APP_OTA_DFU_ERROR;
    ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
  }

  // Forward state change information to application.
  sli_bt_app_ota_dfu_set_main_status(ota_sts);
  ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
  ota_event.ota_error_code = ota_error;
  ota_event.btl_api_retval = boot_retv;
  ota_event.evt_info.sts.status = ota_sts;
  ota_event.evt_info.sts.prev_status = SL_BT_APP_OTA_DFU_UNINIT;
  sl_bt_app_ota_dfu_on_status_event(&ota_event);

  // Trigger task.
  sli_bt_app_ota_dfu_proceed();

  // Start task main loop.
  while (1) {
    if ( xSemaphoreTake(xSemaphoreOn, portMAX_DELAY) == pdTRUE ) {
      // Execute cyclic process of Application OTA DFU.
      sli_bt_app_ota_dfu_step();
    }
  }
}
