/***************************************************************************//**
 * @file
 * @brief Application init
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <assert.h>
#include "app_init.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_event_mgr.h"
#include "sl_wisun_coap_meter.h"
#include "app.h"
#include "app_custom_callback.h"
#include "sl_wisun_coap_resource_handler.h"
#include "sl_wisun_coap_meter_collector_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/**
 * @brief Application stack size in word size (void *)
 */
#define APP_STACK_SIZE      (320U)

/**
 * @brief Stack size in aligned bytes
 */
#define APP_STACK_SIZE_BYTES ((APP_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8U)
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**
 * @brief App task control block
 */
__ALIGNED(8) static  uint8_t app_task_cb[sizeof(osThread_t)];

/**
 * @brief Static stack allocation
 */
__ALIGNED(8) static uint8_t app_stack[APP_STACK_SIZE_BYTES];

/**
 * @brief Measurement resource definition for the temperature sensor
 */
static const sl_wisun_coap_resource_t _measurement_resource = {
  .data = {
    .uri_path          = SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH,
    .resource_type     = SL_WISUN_COAP_METER_RESOURCE_RT_TEMP,
    .interface         = SL_WISUN_COAP_METER_RESOURCE_IF_TEMP_SENSOR,
    .uri_path_len      = sizeof(SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH),
    .resource_type_len = sizeof(SL_WISUN_COAP_METER_RESOURCE_RT_TEMP),
    .interface_len     = sizeof(SL_WISUN_COAP_METER_RESOURCE_IF_TEMP_SENSOR),
  },
  .discoverable = discoverable,
};

/**
 * @brief Measurement resource definition for the LED
 */
static const sl_wisun_coap_resource_t _led_resource = {
  .data = {
    .uri_path          = SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH,
    .resource_type     = SL_WISUN_COAP_METER_RESOURCE_RT_LED,
    .interface         = SL_WISUN_COAP_METER_RESOURCE_IF_LED,
    .uri_path_len      = sizeof(SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH),
    .resource_type_len = sizeof(SL_WISUN_COAP_METER_RESOURCE_RT_LED),
    .interface_len     = sizeof(SL_WISUN_COAP_METER_RESOURCE_IF_LED),
  },
  .discoverable = discoverable,
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void app_init(void)
{
  /* Init project info */
  app_wisun_project_info_init("Wi-SUN CoAP Meter Application");

  /* init meter collector */
  sl_wisun_coap_meter_init();

  // init resources
  sl_wisun_coap_resource_init();

  (void)sl_wisun_coap_resource_register(&_measurement_resource.data,
                                        _measurement_resource.discoverable);
  (void)sl_wisun_coap_resource_register(&_led_resource.data,
                                        _led_resource.discoverable);

  /* Register callbacks */
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_CONNECTED_IND_ID,
                                        app_custom_connected_callback);
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_SOCKET_DATA_IND_ID,
                                        app_custom_socket_data_callback);
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID,
                                        app_custom_socket_data_sent_callback);

  /* Creating App main thread */
  const osThreadAttr_t app_task_attr = {
    .name        = "AppMain",
    .attr_bits   = osThreadDetached,
    .cb_mem      = app_task_cb,
    .cb_size     = sizeof(app_task_cb),
    .stack_mem   = app_stack,
    .stack_size  = APP_STACK_SIZE_BYTES,
    .priority    = osPriorityNormal,
    .tz_module   = 0
  };
  osThreadId_t app_thr_id = osThreadNew(app_task,
                                        NULL,
                                        &app_task_attr);
  assert(app_thr_id != NULL);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
