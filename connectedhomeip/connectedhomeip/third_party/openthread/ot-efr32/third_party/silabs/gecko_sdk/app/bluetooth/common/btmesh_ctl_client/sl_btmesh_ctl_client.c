/***************************************************************************//**
 * @file
 * @brief Bt Mesh CTL Client module
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
#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"

#include "sl_btmesh_api.h"
#include "sl_bt_api.h"
#include "sl_btmesh_dcd.h"

#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_lib.h"

#include "app_assert.h"
#include "sl_simple_timer.h"
#include "sl_btmesh_lighting_client.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_ctl_client_config.h"
#include "sl_btmesh_ctl_client.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup Lighting
 * @{
 ******************************************************************************/

/// No flags used for message
#define NO_FLAGS                  0
/// Immediate transition time is 0 seconds
#define IMMEDIATE                 0
/// Callback has not parameters
#define NO_CALLBACK_DATA          (void *)NULL
/// High Priority
#define HIGH_PRIORITY             0
/// Minimum color temperature 800K
#define TEMPERATURE_MIN           0x0320
/// Maximum color temperature 20000K
#define TEMPERATURE_MAX           0x4e20
/// Scale factor for temperature calculations
#define TEMPERATURE_SCALE_FACTOR  100
/// Delta UV is hardcoded to 0
#define DELTA_UV                  0
/// Initial temperature percentage value
#define TEMPERATURE_PCT_INIT      50
/// Maximum temperature percentage value
#define TEMPERATURE_PCT_MAX       100
/// Temperature level initial value @ref temperature_level
#define TEMPERATURE_LEVEL_INIT    0
/// Delay unit value for request for ctl messages in millisecond
#define REQ_DELAY_MS              50

/// periodic timer handle
static sl_simple_timer_t ctl_retransmission_timer;

/// periodic timer callback
static void ctl_retransmission_timer_cb(sl_simple_timer_t *handle,
                                        void *data);

/// temperature level percentage
static uint8_t temperature_percent = TEMPERATURE_PCT_INIT;
/// temperature level converted from percentage to actual value, range 0..65535
static uint16_t temperature_level = TEMPERATURE_LEVEL_INIT;
/// number of ctl requests to be sent
static uint8_t ctl_request_count = 0;
/// ctl transaction identifier
static uint8_t ctl_trid = 0;

/***************************************************************************//**
 * This function publishes one light CTL request to change the temperature level
 * of light(s) in the group. Global variable temperature_level holds the latest
 * desired light temperature level.
 * The CTL request also send lightness_level which holds the latest desired light
 * lightness level and Delta UV which is hardcoded to 0 for this application.
 *
 * param[in] retrans  Indicates if this is the first request or a retransmission,
 *                    possible values are 0 = first request, 1 = retransmission.
 *
 * @note This application sends multiple ctl requests for each
 *       medium button press to improve reliability.
 *       The transaction ID is not incremented in case of a retransmission.
 ******************************************************************************/
static void send_ctl_request(uint8_t retrans)
{
  struct mesh_generic_request req;
  sl_status_t sc;

  req.kind = mesh_lighting_request_ctl;
  req.ctl.lightness = sl_btmesh_get_lightness();
  req.ctl.temperature = temperature_level;
  req.ctl.deltauv = DELTA_UV; //hardcoded delta uv

  // Increment transaction ID for each request, unless it's a retransmission
  if (retrans == 0) {
    ctl_trid++;
  }

  // Delay for the request is calculated so that the last request will have
  // a zero delay and each of the previous request have delay that increases
  // in 50 ms steps. For example, when using three ctl requests
  // per button press the delays are set as 100, 50, 0 ms
  uint16_t delay = (ctl_request_count - 1) * REQ_DELAY_MS;

  sc = mesh_lib_generic_client_publish(MESH_LIGHTING_CTL_CLIENT_MODEL_ID,
                                       BTMESH_CTL_CLIENT_MAIN,
                                       ctl_trid,
                                       &req,
                                       IMMEDIATE,     // transition
                                       delay,
                                       NO_FLAGS       // flags
                                       );

  if (sc == SL_STATUS_OK) {
    log_info(SL_BTMESH_CTL_CLIENT_LOGGING_RECALL_SUCCESS_CFG_VAL, ctl_trid, delay);
  } else {
    log_btmesh_status_f(sc, SL_BTMESH_CTL_CLIENT_LOGGING_CLIENT_PUBLISH_FAIL_CFG_VAL);
  }

  // Keep track of how many requests has been sent
  if (ctl_request_count > 0) {
    ctl_request_count--;
  }
}

/*******************************************************************************
 * This function change the color temperature and sends it to the server.
 *
 * @param[in] change_percentage  Defines the color temperature percentage change,
 * possible values are  -100% - + 100%.
 *
 ******************************************************************************/
void sl_btmesh_change_temperature(int8_t change_percentage)
{
  // Adjust light brightness, using Light Lightness model
  if (change_percentage > 0) {
    temperature_percent += change_percentage;
    if (temperature_percent > TEMPERATURE_PCT_MAX) {
#if (SL_BTMESH_CTL_CLIENT_TEMPERATURE_WRAP_ENABLED_CFG_VAL != 0)
      temperature_percent = 0;
#else
      temperature_percent = TEMPERATURE_PCT_MAX;
#endif
    }
  } else {
    if (temperature_percent < (-change_percentage)) {
#if (SL_BTMESH_CTL_CLIENT_TEMPERATURE_WRAP_ENABLED_CFG_VAL != 0)
      temperature_percent = TEMPERATURE_PCT_MAX;
#else
      temperature_percent = 0;
#endif
    } else {
      temperature_percent += change_percentage;
    }
  }

  sl_btmesh_set_temperature(temperature_percent);
}

/*******************************************************************************
 * This function change the temperature and send it to the server.
 *
 * @param[in] new_color_temperature_percentage  Defines new color temperature
 * value as percentage.
 *    Valid values 0-100 %
 *
 ******************************************************************************/
void sl_btmesh_set_temperature(uint8_t new_color_temperature_percentage)
{
  // Adjust light temperature, using Light CTL model
  if (new_color_temperature_percentage <= TEMPERATURE_PCT_MAX) {
    temperature_percent = new_color_temperature_percentage;
  } else {
    return;
  }

  // Using square of percentage to change temperature more uniformly
  // just for demonstration
  temperature_level = TEMPERATURE_MIN                              \
                      + (temperature_percent * temperature_percent \
                         / TEMPERATURE_PCT_MAX)                    \
                      * (TEMPERATURE_MAX - TEMPERATURE_MIN)        \
                      / TEMPERATURE_SCALE_FACTOR;
  log(SL_BTMESH_CTL_CLIENT_LOGGING_NEW_TEMP_SET_CFG_VAL,
      temperature_percent * temperature_percent / TEMPERATURE_PCT_MAX,
      temperature_level);

  // Request is sent multiple times to improve reliability
  ctl_request_count = SL_BTMESH_CTL_CLIENT_RETRANSMISSION_COUNT_CFG_VAL;

  send_ctl_request(0);  //Send the first request

  // If there are more requests to send, start a repeating soft timer
  // to trigger retransmission of the request after 50 ms delay
  if (ctl_request_count > 0) {
    sl_status_t sc = sl_simple_timer_start(&ctl_retransmission_timer,
                                           SL_BTMESH_CTL_CLIENT_RETRANSMISSION_TIMEOUT_CFG_VAL,
                                           ctl_retransmission_timer_cb,
                                           NO_CALLBACK_DATA,
                                           true);
    app_assert_status_f(sc, "Failed to start periodic timer\n");
  }
}

/***************************************************************************//**
 * Timer Callbacks
 * @param[in] handle pointer to handle instance
 * @param[in] data pointer to input data
 ******************************************************************************/
static void  ctl_retransmission_timer_cb(sl_simple_timer_t *handle,
                                         void *data)
{
  (void)data;
  (void)handle;

  send_ctl_request(1);   // Retransmit ctl message
  // Stop retransmission timer if it was the last attempt
  if (ctl_request_count == 0) {
    sl_status_t sc = sl_simple_timer_stop(&ctl_retransmission_timer);
    app_assert_status_f(sc, "Failed to stop periodic timer\n");
  }
}
/** @} (end addtogroup Lighting) */
