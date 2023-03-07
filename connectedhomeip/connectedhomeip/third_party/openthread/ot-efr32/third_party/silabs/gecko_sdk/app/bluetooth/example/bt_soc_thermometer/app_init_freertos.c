/***************************************************************************//**
 * @file
 * @brief Application init for FreeRTOS.
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
#include "sl_status.h"
#include "sl_sensor_rht.h"
#include "app_log.h"
#include "app_assert.h"
#include "app.h"

#define APP_INIT_TASK_NAME          "app_init"
#define APP_INIT_TASK_STACK_SIZE    200
#define APP_INIT_TASK_STATIC        1

#if (APP_INIT_TASK_STATIC == 1)
StackType_t app_init_task_stack[APP_INIT_TASK_STACK_SIZE];
StaticTask_t app_init_task_handle;
#else // configSUPPORT_STATIC_ALLOCATION
TaskHandle_t app_init_task_handle = NULL;
#endif // configSUPPORT_STATIC_ALLOCATION

/**************************************************************************//**
 * FreeRTOS Task for Application Init.
 *****************************************************************************/
void app_init_task(void *p_arg)
{
  (void)p_arg;
  sl_status_t sc;
  app_log_info("health thermometer initialised\n");
  // Init temperature sensor.
  sc = sl_sensor_rht_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Relative Humidity and Temperature sensor initialization failed.");
    app_log_nl();
  }
  vTaskDelete(NULL);
}

/**************************************************************************//**
 * Application Init, overrides weak implementation
 *****************************************************************************/
void app_init(void)
{
  // Create a task for init to be started by the scheduler.
  #if (APP_INIT_TASK_STATIC == 1)
  xTaskCreateStatic(app_init_task,
                    APP_INIT_TASK_NAME,
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY,
                    app_init_task_stack,
                    &app_init_task_handle);
  #else // configSUPPORT_STATIC_ALLOCATION
  xTaskCreate(app_init_task,
              APP_INIT_TASK_NAME,
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              &app_init_task_handle);
  #endif // configSUPPORT_STATIC_ALLOCATION
}
