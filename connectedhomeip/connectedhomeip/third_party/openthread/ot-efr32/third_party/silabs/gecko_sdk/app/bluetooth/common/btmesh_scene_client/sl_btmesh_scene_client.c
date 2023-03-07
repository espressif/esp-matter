/***************************************************************************//**
 * @file
 * @brief BT mesh scene client module
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_common.h"
#include "sl_status.h"

#include "sl_btmesh_api.h"
#include "sl_bt_api.h"
#include "sl_btmesh_dcd.h"

#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_scene_client_config.h"
#include "sl_btmesh_scene_client.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup Scene Client
 * @{
 ******************************************************************************/

/// High Priority
#define HIGH_PRIORITY                       0
/// Callback has not parameters
#define NO_CALLBACK_DATA                    (void *)NULL
/// Immediate transition time is 0 seconds
#define IMMEDIATE                           0
/// Parameter ignored for publishing
#define IGNORED                             0
/// No flags used for message
#define NO_FLAGS                            0
/// Delay unit value for request for ctl messages in millisecond
#define REQ_DELAY_MS                        50

/// Address zero is used in scene client commands to indicate that
/// message should be published
static const uint16_t PUBLISH_ADDRESS = 0x0000;

/// currently selected scene
static uint16_t scene_number = 0;
/// number of scene requests to be sent
static uint8_t scene_request_count;
/// scene transaction identifier
static uint8_t scene_trid = 0;

/// periodic timer handle
static sl_simple_timer_t app_scene_retransmission_timer;

/// periodic timer callback
static void scene_retransmission_timer_cb(sl_simple_timer_t *handle,
                                          void *data);

/***************************************************************************//**
 * This function publishes one scene recall request to recall selected scene.
 * Global variable scene_number holds the latest desired scene state.
 *
 * param[in] retrans  Indicates if this is the first request or a retransmission,
 *                    possible values are 0 = first request, 1 = retransmission.
 *
 * @note This application sends multiple scene requests for each
 *       very long button press to improve reliability.
 *       The transaction ID is not incremented in case of a retransmission.
 ******************************************************************************/
static void send_scene_recall_request(uint8_t retrans)
{
  // Increment transaction ID for each request, unless it's a retransmission
  if (retrans == 0) {
    scene_trid++;
  }

  // Delay for the request is calculated so that the last request will have
  // a zero delay and each of the previous request have delay that increases
  // in 50 ms steps. For example, when using three scene requests
  // per button press the delays are set as 100, 50, 0 ms
  uint16_t delay = (scene_request_count - 1) * REQ_DELAY_MS;

  sl_status_t sc = sl_btmesh_scene_client_recall(PUBLISH_ADDRESS,
                                                 BTMESH_SCENE_CLIENT_MAIN,
                                                 scene_number,
                                                 IGNORED,
                                                 NO_FLAGS,
                                                 scene_trid,
                                                 IMMEDIATE,
                                                 delay);

  if (SL_STATUS_OK == sc) {
    log_info(SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_SUCCESS_CFG_VAL, scene_trid, delay);
  } else {
    log_btmesh_status_f(sc, SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_FAIL_CFG_VAL);
  }

  // Keep track of how many requests has been sent
  if (scene_request_count > 0) {
    scene_request_count--;
  }
}

/*******************************************************************************
 * This function select scene and send it to the server.
 *
 * @param[in] scene_to_recall  Scene to recall, possible values 1-255.
 *
 ******************************************************************************/
void sl_btmesh_select_scene(uint8_t scene_to_recall)
{
  // Scene number 0 is prohibited
  if (scene_to_recall == 0) {
    return;
  }

  scene_number = scene_to_recall;

  // Recall scene using Scene Client model
  log(SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_CFG_VAL, scene_number);
  // Request is sent multiple times to improve reliability
  scene_request_count = SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_COUNT_CFG_VAL;

  send_scene_recall_request(0);  // Send the first request

  // If there are more requests to send, start a repeating soft timer
  // to trigger retransmission of the request after 50 ms delay
  if (scene_request_count > 0) {
    sl_status_t sc = sl_simple_timer_start(&app_scene_retransmission_timer,
                                           SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_TIMEOUT_CFG_VAL,
                                           scene_retransmission_timer_cb,
                                           NO_CALLBACK_DATA,
                                           true);
    app_assert_status_f(sc, "Failed to start periodic timer\n");
  }
}

/**************************************************************************//**
 * Handle Sensor Server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt pointer to event descriptor
 *****************************************************************************/
void sl_btmesh_handle_scene_client_on_event(sl_btmesh_msg_t *evt)
{
  sl_status_t sc;
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      sc = sl_btmesh_scene_client_init(BTMESH_SCENE_CLIENT_MAIN);
      app_assert_status_f(sc, "Failed to init scene client model\n");
      break;
  }
}

/**************************************************************************//**
 * Timer Callbacks
 * @param[in] handle pointer to handle instance
 * @param[in] data pointer to input data
 *****************************************************************************/
static void  scene_retransmission_timer_cb(sl_simple_timer_t *handle,
                                           void *data)
{
  (void)data;
  (void)handle;

  send_scene_recall_request(1);   // Retransmit scene message
  // Stop retransmission timer if it was the last attempt
  if (scene_request_count == 0) {
    sl_status_t sc = sl_simple_timer_stop(&app_scene_retransmission_timer);
    app_assert_status_f(sc, "Failed to stop periodic timer\n");
  }
}

/** @} (end addtogroup Scene Client) */
