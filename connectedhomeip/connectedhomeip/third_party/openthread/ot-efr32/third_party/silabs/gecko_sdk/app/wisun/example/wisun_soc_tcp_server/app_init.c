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
#include "app.h"
#include "app_custom_callback.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/**
 * @brief Application stack size in word size (void *)
 */
#define APP_STACK_SIZE      (256U)

/**
 * @brief Stack size in aligned bytes
 */
#define APP_STACK_SIZE_BYTES ((APP_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8u)
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

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void app_init(void)
{
  /* Init project info */
  app_wisun_project_info_init("Wi-SUN TCP Server Application");

  /* Register callbacks */
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_CONNECTED_IND_ID,
                                        app_custom_connected_callback);
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_SOCKET_DATA_IND_ID,
                                        app_custom_socket_data_callback);
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID,
                                        app_custom_socket_data_sent_callback);
  app_wisun_em_custom_callback_register(SL_WISUN_MSG_SOCKET_CLOSING_IND_ID,
                                        app_custom_socket_closing_event_handler);

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
