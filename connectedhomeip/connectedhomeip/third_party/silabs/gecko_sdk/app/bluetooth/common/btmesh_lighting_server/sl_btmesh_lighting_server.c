/***************************************************************************//**
 * @file
 * @brief Lighting Server module
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

// C Standard Library headers
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

// Bluetooth stack headers
#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_lib.h"

#include "sl_btmesh_lighting_server.h"
#include "sl_btmesh_lighting_server_config.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup Lighting Server
 * @{
 ******************************************************************************/

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
#define scene_server_reset_register(elem_index) \
  scene_server_reset_register_impl(elem_index)
#else
#define scene_server_reset_register(elem_index)
#endif

/// No flags used for message
#define NO_FLAGS              0
/// Immediate transition time is 0 seconds
#define IMMEDIATE             0
/// Callback has no parameters
#define NO_CALLBACK_DATA      (void *)NULL
/// High Priority
#define HIGH_PRIORITY         0
/// Values greater than max 37200000 are treated as unknown remaining time
#define UNKNOWN_REMAINING_TIME      40000000

/**
 * @brief Default value of Lightness Last
 *
 * Stores the last known non-zero value of lightness actual.
 */
#define LIGHTNESS_LAST_DEFAULT      0xFFFF
/**
 * @brief Default value of Lightness Default
 *
 * Representing a default lightness level for lightness actual.
 */
#define LIGHTNESS_DEFAULT_DEFAULT   0x0000

/// Lightbulb state
static PACKSTRUCT(struct lightbulb_state {
  // On/Off Server state
  uint8_t onoff_current;          /**< Current generic on/off value */
  uint8_t onoff_target;           /**< Target generic on/off value */

  // Transition Time Server state
  uint8_t transtime;              /**< Transition time */

  // On Power Up Server state
  uint8_t onpowerup;              /**< On Power Up value */

  // Lightness server
  uint16_t lightness_current;     /**< Current lightness value */
  uint16_t lightness_target;      /**< Target lightness value */
  uint16_t lightness_last;        /**< Last lightness value */
  uint16_t lightness_default;     /**< Default lightness value */
  uint16_t lightness_min;         /**< Minimum lightness value */
  uint16_t lightness_max;         /**< Maximum lightness value */

  // Primary Generic Level
  int16_t pri_level_current;      /**< Current primary generic level value */
  int16_t pri_level_target;       /**< Target primary generic level value */
}) lightbulb_state;

/// copy of transition delay parameter, needed for delayed on/off request
static uint32_t delayed_onoff_trans = 0;
/// copy of transition delay parameter, needed for delayed lightness request
static uint32_t delayed_lightness_trans = 0;
/// copy of lightness request kind, needed for delayed lightness request
static mesh_generic_state_t lightness_kind = mesh_generic_state_last;
/// copy of transition delay parameter, needed for
/// delayed primary generic level request
static uint32_t delayed_pri_level_trans = 0;
/// copy of generic request kind, needed for delayed primary generic request
static mesh_generic_request_t pri_level_request_kind = mesh_generic_request_level;
/// copy of move transition parameter for primary generic request
static uint32_t move_pri_level_trans = 0;
/// copy of move delta parameter for primary generic request
static int16_t move_pri_level_delta = 0;

static void lightbulb_state_changed(void);
static void lightbulb_state_validate_and_correct(void);

static sl_status_t generic_server_respond(uint16_t model_id,
                                          uint16_t element_index,
                                          uint16_t client_addr,
                                          uint16_t appkey_index,
                                          const struct mesh_generic_state *current,
                                          const struct mesh_generic_state *target,
                                          uint32_t remaining_ms,
                                          uint8_t response_flags);

static sl_status_t generic_server_update(uint16_t model_id,
                                         uint16_t element_index,
                                         const struct mesh_generic_state *current,
                                         const struct mesh_generic_state *target,
                                         uint32_t remaining_ms);

static sl_status_t generic_server_publish(uint16_t model_id,
                                          uint16_t element_index,
                                          mesh_generic_state_t kind);

static void generic_server_register_handler(uint16_t model_id,
                                            uint16_t elem_index,
                                            mesh_lib_generic_server_client_request_cb cb,
                                            mesh_lib_generic_server_change_cb ch,
                                            mesh_lib_generic_server_recall_cb recall);

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
static void scene_server_reset_register_impl(uint16_t elem_index);
#endif

// Timer handles
static sl_simple_timer_t lighting_pri_level_move_timer;
static sl_simple_timer_t lighting_transition_complete_timer;
static sl_simple_timer_t lighting_level_transition_complete_timer;
static sl_simple_timer_t lighting_onoff_transition_complete_timer;
static sl_simple_timer_t lighting_delayed_pri_level_timer;
static sl_simple_timer_t lighting_delayed_lightness_request_timer;
static sl_simple_timer_t lighting_delayed_onoff_request_timer;
static sl_simple_timer_t lighting_state_store_timer;

// Timer callbacks
static void lighting_pri_level_move_timer_cb(sl_simple_timer_t *handle,
                                             void *data);
static void lighting_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                  void *data);
static void lighting_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data);
static void lighting_onoff_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data);
static void lighting_delayed_pri_level_timer_cb(sl_simple_timer_t *handle,
                                                void *data);
static void lighting_delayed_lightness_request_timer_cb(sl_simple_timer_t *handle,
                                                        void *data);
static void lighting_delayed_onoff_request_timer_cb(sl_simple_timer_t *handle,
                                                    void *data);
static void lighting_state_store_timer_cb(sl_simple_timer_t *handle,
                                          void *data);

/*******************************************************************************
 * Get current lightness value
 *
 * @return Current lightness
 ******************************************************************************/
uint16_t sl_btmesh_get_lightness_current(void)
{
  return lightbulb_state.lightness_current;
}

/*******************************************************************************
 * Set current lightness value
 *
 * @param[in] lightness  Current lightness
 ******************************************************************************/
void sl_btmesh_set_lightness_current(uint16_t lightness)
{
  lightbulb_state.lightness_current = lightness;
}

/*******************************************************************************
 * Get target lightness value
 *
 * @return  Target lightness
 ******************************************************************************/
uint16_t sl_btmesh_get_lightness_target(void)
{
  return lightbulb_state.lightness_target;
}

/*******************************************************************************
 * Set target lightness value
 *
 * @param[in] lightness  Target lightness
 ******************************************************************************/
void sl_btmesh_set_lightness_target(uint16_t lightness)
{
  lightbulb_state.lightness_target = lightness;
}

/*******************************************************************************
 * Get default lightness value
 *
 * @return  Default lightness
 ******************************************************************************/
uint16_t sl_btmesh_get_lightness_default(void)
{
  return lightbulb_state.lightness_default;
}

/*******************************************************************************
 * Set default lightness value
 *
 * @param[in] lightness  Default lightness
 ******************************************************************************/
void sl_btmesh_set_lightness_default(uint16_t lightness)
{
  lightbulb_state.lightness_default = lightness;
}

/*******************************************************************************
 * Set last lightness value
 *
 * @param[in] lightness  Last lightness
 ******************************************************************************/
void sl_btmesh_set_lightness_last(uint16_t lightness)
{
  lightbulb_state.lightness_last = lightness;
}

/*******************************************************************************
 * Gets default lightness value on power up
 *
 * @return  Default lightness on power up
 ******************************************************************************/
uint16_t sl_btmesh_get_lightness_onpowerup(void)
{
  return lightbulb_state.onpowerup;
}

#if defined(SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL) \
  && SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL
/***************************************************************************//**
 * This function prints debug information for mesh server state change event.
 *
 * @param[in] evt  Pointer to mesh_lib_generic_server_state_changed event.
 ******************************************************************************/
static void server_state_changed(sl_btmesh_evt_generic_server_state_changed_t *evt)
{
  int i;

  log("State changed: ");
  log("Model ID %4.4x, type %2.2x ", evt->model_id, evt->type);
  for (i = 0; i < evt->parameters.len; i++) {
    log("%2.2x ", evt->parameters.data[i]);
  }
  log("\r\n");
}
#endif // LOG_ENABLE

/*******************************************************************************
 * Handle ligthing server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt  Pointer to incoming event.
 ******************************************************************************/
void sl_btmesh_lighting_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_lighting_server_init();
      break;
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_lighting_server_init();
      }
      break;
    case sl_btmesh_evt_node_reset_id:
      sl_bt_nvm_erase(SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL);
      break;
    case sl_btmesh_evt_generic_server_state_changed_id:
#if defined(SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL) \
      && SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL
      server_state_changed(&(evt->data.evt_generic_server_state_changed));
#endif // LOG_ENABLE
      break;
  }
}

/***************************************************************************//**
 * This function convert mesh format of default transition time to milliseconds.
 *
 * @return Default transition time in milliseconds.
 ******************************************************************************/
uint32_t sl_btmesh_get_default_transition_time(void)
{
  return mesh_lib_transition_time_to_ms(lightbulb_state.transtime);
}

/***************************************************************************//**
 * \defgroup GenericOnOff
 * \brief Generic OnOff Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup GenericOnOff
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic on/off request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t onoff_response(uint16_t element_index,
                                  uint16_t client_addr,
                                  uint16_t appkey_index,
                                  uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lightbulb_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lightbulb_state.onoff_target;

  return generic_server_respond(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                0x00);
}

/***************************************************************************//**
 * Update generic on/off state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t onoff_update(uint16_t element_index, uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lightbulb_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lightbulb_state.onoff_target;

  return generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update generic on/off state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t onoff_update_and_publish(uint16_t element_index,
                                            uint32_t remaining_ms)
{
  sl_status_t e;

  e = onoff_update(element_index, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_on_off);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the generic on/off model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;

  log("ON/OFF request: requested state=<%s>, transition=%lu, delay=%u\r\n",
      request->on_off ? "ON" : "OFF", transition_ms, delay_ms);

  if (lightbulb_state.onoff_current == request->on_off) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Turning light bulb <%s>\r\n", request->on_off ? "ON" : "OFF");
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.onoff_current = request->on_off;
      lightbulb_state.onoff_target = request->on_off;
      if (lightbulb_state.onoff_current == MESH_GENERIC_ON_OFF_STATE_OFF) {
        lightbulb_state.lightness_target = 0;
      } else {
        // restore last brightness
        lightbulb_state.lightness_target = lightbulb_state.lightness_last;
      }
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_target, IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.onoff_target = request->on_off;
      sl_status_t sc = sl_simple_timer_start(&lighting_delayed_onoff_request_timer,
                                             delay_ms,
                                             lighting_delayed_onoff_request_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed ON/OFF Request timer\n");
      // store transition parameter for later use
      delayed_onoff_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.onoff_target = request->on_off;
      if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_ON) {
        lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      }

      onoff_update(element_index, transition_ms);

      if (request->on_off == MESH_GENERIC_ON_OFF_STATE_OFF) {
        lightbulb_state.lightness_target = 0;
      } else {
        // restore last brightness
        lightbulb_state.lightness_target = lightbulb_state.lightness_last;
      }
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                   transition_ms);
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lighting_onoff_transition_complete_timer,
                                             transition_ms,
                                             lighting_onoff_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start ON/OFF Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(element_index);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    onoff_response(element_index, client_addr, appkey_index, remaining_ms);
  }
  onoff_update_and_publish(element_index, remaining_ms);

  // publish to bound states
  generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_lightness_actual);
}

/***************************************************************************//**
 * This function is a handler for generic on/off change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (current->on_off.on != lightbulb_state.onoff_current) {
    log("ON/OFF state changed %u to %u\r\n",
        lightbulb_state.onoff_current,
        current->on_off.on);

    lightbulb_state.onoff_current = current->on_off.on;
    lightbulb_state_changed();
  } else {
    log("Dummy ON/OFF change - same state as before\r\n");
  }
}

/***************************************************************************//**
 * This function is a handler for generic on/off recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void onoff_recall(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t transition_ms)
{
  (void)model_id;

  log("Generic ON/OFF recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.onoff_target = current->on_off.on;
  } else {
    lightbulb_state.onoff_target = target->on_off.on;
  }

  if (lightbulb_state.onoff_current == lightbulb_state.onoff_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall ON/OFF state <%s> with transition=%lu ms\r\n",
        lightbulb_state.onoff_target ? "ON" : "OFF",
        transition_ms);

    if (transition_ms == IMMEDIATE) {
      lightbulb_state.onoff_current = current->on_off.on;
    } else {
      if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_ON) {
        lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      }
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lighting_onoff_transition_complete_timer,
                                             transition_ms,
                                             lighting_onoff_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start ON/OFF Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  onoff_update_and_publish(element_index, transition_ms);
}

/***************************************************************************//**
 * This function is called when a light on/off request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void onoff_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.onoff_current = lightbulb_state.onoff_target;

  log("Transition complete. New state is %s\r\n",
      lightbulb_state.onoff_current ? "ON" : "OFF");

  lightbulb_state_changed();
  onoff_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light on/off request has completed.
 ******************************************************************************/
static void delayed_onoff_request(void)
{
  log("Starting delayed ON/OFF request: %u -> %u, %lu ms\r\n",
      lightbulb_state.onoff_current,
      lightbulb_state.onoff_target,
      delayed_onoff_trans
      );

  if (delayed_onoff_trans == 0) {
    // no transition delay, update state immediately

    lightbulb_state.onoff_current = lightbulb_state.onoff_target;
    if (lightbulb_state.onoff_current == MESH_GENERIC_ON_OFF_STATE_OFF) {
      sl_btmesh_set_state(LED_STATE_OFF);
    } else {
      // restore last brightness level
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_last, IMMEDIATE);
      lightbulb_state.lightness_current = lightbulb_state.lightness_last;
    }

    lightbulb_state_changed();

    onoff_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                             delayed_onoff_trans);
  } else {
    if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_OFF) {
      lightbulb_state.lightness_target = 0;
    } else {
      // restore last brightness level, with transition delay
      lightbulb_state.lightness_target = lightbulb_state.lightness_last;
      lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;

      onoff_update(BTMESH_LIGHTING_SERVER_MAIN, delayed_onoff_trans);
    }
    sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                 delayed_onoff_trans);

    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&lighting_onoff_transition_complete_timer,
                                           delayed_onoff_trans,
                                           lighting_onoff_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start ON/OFF Transition Complete timer\n");
  }
}

/** @} (end addtogroup GenericOnOff) */

/***************************************************************************//**
 * \defgroup GenericPowerOnOff
 * \brief Generic Power OnOff Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup GenericPowerOnOff
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic power on/off request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t power_onoff_response(uint16_t element_index,
                                        uint16_t client_addr,
                                        uint16_t appkey_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_on_power_up;
  current.on_power_up.on_power_up = lightbulb_state.onpowerup;

  return generic_server_respond(MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                NULL,
                                0,
                                0x00);
}

/***************************************************************************//**
 * Update generic power on/off state.
 *
 * @param[in] element_index  Server model element index.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t power_onoff_update(uint16_t element_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_on_power_up;
  current.on_power_up.on_power_up = lightbulb_state.onpowerup;

  return generic_server_update(MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               NULL,
                               0);
}

/***************************************************************************//**
 * Update generic power on/off state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t power_onoff_update_and_publish(uint16_t element_index)
{
  sl_status_t e;

  e = power_onoff_update(element_index);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_on_power_up);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the generic power on/off model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void power_onoff_request(uint16_t model_id,
                                uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t server_addr,
                                uint16_t appkey_index,
                                const struct mesh_generic_request *request,
                                uint32_t transition_ms,
                                uint16_t delay_ms,
                                uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;
  (void)transition_ms;
  (void)delay_ms;

  log("ON POWER UP request received; state=<%s>\r\n",
      lightbulb_state.onpowerup == 0 ? "OFF"
      : lightbulb_state.onpowerup == 1 ? "ON"
      : "RESTORE");

  if (lightbulb_state.onpowerup == request->on_power_up) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Setting onpowerup to <%s>\r\n",
        request->on_power_up == 0 ? "OFF"
        : request->on_power_up == 1 ? "ON"
        : "RESTORE");
    lightbulb_state.onpowerup = request->on_power_up;
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    power_onoff_response(element_index, client_addr, appkey_index);
  }
  power_onoff_update_and_publish(element_index);
}

/***************************************************************************//**
 * This function is a handler for generic power on/off change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void power_onoff_change(uint16_t model_id,
                               uint16_t element_index,
                               const struct mesh_generic_state *current,
                               const struct mesh_generic_state *target,
                               uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)current;
  (void)target;
  (void)remaining_ms;
  // TODO
}

/** @} (end addtogroup GenericPowerOnOff) */

/***************************************************************************//**
 * \defgroup GenericTransitionTime
 * \brief Generic Default Transition Time Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup GenericTransitionTime
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic default transition time request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t transtime_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_transition_time;
  current.transition_time.time = lightbulb_state.transtime;

  return generic_server_respond(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                NULL,
                                0,
                                0x00);
}

/***************************************************************************//**
 * Update generic default transition time state.
 *
 * @param[in] element_index  Server model element index.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t transtime_update(uint16_t element_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_transition_time;
  current.transition_time.time = lightbulb_state.transtime;

  return generic_server_update(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               NULL,
                               0);
}

/***************************************************************************//**
 * Update generic default transition time state and publish model state
 * to the network.
 *
 * @param[in] element_index  Server model element index.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t transtime_update_and_publish(uint16_t element_index)
{
  sl_status_t e;

  e = transtime_update(element_index);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_transition_time);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the generic default transition time
 * model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void transtime_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;
  (void)transition_ms;
  (void)delay_ms;

  log("Transition time request received; state=<0x%x>\r\n",
      lightbulb_state.transtime);

  if (lightbulb_state.transtime == request->transition_time) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Setting transtime to <0x%x>\r\n", request->transition_time);
    lightbulb_state.transtime = request->transition_time;
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    transtime_response(element_index, client_addr, appkey_index);
  }
  transtime_update_and_publish(element_index);
}

/***************************************************************************//**
 * This function is a handler for generic default transition time change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void transtime_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)current;
  (void)target;
  (void)remaining_ms;
  // TODO
}

/** @} (end addtogroup GenericTransitionTime) */

/****************************************************************************//**
 * \defgroup LightLightness
 * \brief Light Lightness Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightLightness
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Convert from lightness actual to lightness linear value.
 *
 * @param[in] actual  Actual value that is converted.
 *
 * @return Linear value.
 ******************************************************************************/
static uint16_t actual2linear(uint16_t actual)
{
  uint32_t linear = ((uint32_t)actual * actual + 65534) / 65535;
  return (uint16_t)linear;
}

/***************************************************************************//**
 * Convert from lightness linear to lightness actual value.
 *
 * @param[in] linear  Linear value that is converted.
 *
 * @return Actual value.
 ******************************************************************************/
static uint16_t linear2actual(uint16_t linear)
{
  uint32_t actual = (uint32_t)sqrt(65535 * (uint32_t)linear);
  return (uint16_t)actual;
}

/***************************************************************************//**
 * Response to light lightness request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 * @param[in] kind           Type of state used in light lightness response.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightness_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      uint32_t remaining_ms,
                                      mesh_generic_state_t kind)
{
  struct mesh_generic_state current, target;

  current.kind = kind;
  if (kind == mesh_lighting_state_lightness_actual) {
    current.lightness.level = lightbulb_state.lightness_current;
  } else {
    current.lightness.level = actual2linear(lightbulb_state.lightness_current);
  }

  target.kind = kind;
  if (kind == mesh_lighting_state_lightness_actual) {
    target.lightness.level = lightbulb_state.lightness_target;
  } else {
    target.lightness.level = actual2linear(lightbulb_state.lightness_target);
  }

  return generic_server_respond(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                0x00);
}

/***************************************************************************//**
 * Update light lightness state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 * @param[in] kind           Type of state used in light lightness update.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightness_update(uint16_t element_index,
                                    uint32_t remaining_ms,
                                    mesh_generic_state_t kind)
{
  struct mesh_generic_state current, target;

  current.kind = kind;
  if (kind == mesh_lighting_state_lightness_actual) {
    current.lightness.level = lightbulb_state.lightness_current;
  } else {
    current.lightness.level = actual2linear(lightbulb_state.lightness_current);
  }

  target.kind = kind;
  if (kind == mesh_lighting_state_lightness_actual) {
    target.lightness.level = lightbulb_state.lightness_target;
  } else {
    target.lightness.level = actual2linear(lightbulb_state.lightness_target);
  }

  return generic_server_update(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light lightness state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 * @param[in] kind           Type of state used in light lightness update and publish.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightness_update_and_publish(uint16_t element_index,
                                                uint32_t remaining_ms,
                                                mesh_generic_state_t kind)
{
  sl_status_t e;

  e = lightness_update(element_index, remaining_ms, kind);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                               element_index,
                               kind);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light lightness model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void lightness_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;

  uint16_t actual_request = 0;

  switch (request->kind) {
    case mesh_lighting_request_lightness_actual:
      lightness_kind = mesh_lighting_state_lightness_actual;
      actual_request = request->lightness;
      break;

    case mesh_lighting_request_lightness_linear:
      lightness_kind = mesh_lighting_state_lightness_linear;
      actual_request = linear2actual(request->lightness);
      break;

    default:
      break;
  }

  log("lightness_request: level=%u, transition=%lu, delay=%u\r\n",
      actual_request, transition_ms, delay_ms);

  if (lightbulb_state.lightness_current == actual_request) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Setting lightness to <%u>\r\n", actual_request);
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.lightness_current = actual_request;
      lightbulb_state.lightness_target = actual_request;
      if (actual_request != 0) {
        lightbulb_state.lightness_last = actual_request;
      }

      // update LED PWM duty cycle
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                   IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.lightness_target = actual_request;
      sl_status_t sc = sl_simple_timer_start(&lighting_delayed_lightness_request_timer,
                                             delay_ms,
                                             lighting_delayed_lightness_request_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed Lightness Request timer\n");
      // store transition parameter for later use
      delayed_lightness_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.lightness_target = actual_request;
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                   transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lighting_transition_complete_timer,
                                             transition_ms,
                                             lighting_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Lighting Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(element_index);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    lightness_response(element_index,
                       client_addr,
                       appkey_index,
                       remaining_ms,
                       lightness_kind);
  }

  lightness_update_and_publish(element_index, remaining_ms, lightness_kind);

  // publish to bound states
  if (lightness_kind == mesh_lighting_state_lightness_actual) {
    generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                           element_index,
                           mesh_lighting_state_lightness_linear);
  } else {
    generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                           element_index,
                           mesh_lighting_state_lightness_actual);
  }
  generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                         element_index,
                         mesh_generic_state_on_off);

  generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                         element_index,
                         mesh_generic_state_level);

#ifdef SL_CATALOG_BTMESH_CTL_SERVER_PRESENT
  generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_ctl);
#endif

#ifdef SL_CATALOG_BTMESH_HSL_SERVER_PRESENT
  generic_server_publish(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_hsl);
#endif
}

/***************************************************************************//**
 * This function is a handler for light lightness change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void lightness_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (current->kind != mesh_lighting_state_lightness_actual) {
    // if kind is not 'actual' then just report the change here, no change to light state
    log("lightness_change, kind %u, value %u\r\n",
        current->kind,
        current->lightness.level);
    return;
  }

  if (lightbulb_state.lightness_current != current->lightness.level) {
    log("Lightness update: from %u to %u\r\n",
        lightbulb_state.lightness_current,
        current->lightness.level);
    lightbulb_state.lightness_current = current->lightness.level;
    lightbulb_state_changed();
  } else {
    log("Lightness update -same value (%d)\r\n",
        lightbulb_state.lightness_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light lightness recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void lightness_recall(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t transition_ms)
{
  (void)model_id;

  log("Light Lightness recall\r\n");
  if (current->kind != mesh_lighting_state_lightness_actual) {
    return;
  }

  if (transition_ms == IMMEDIATE) {
    lightbulb_state.lightness_target = current->lightness.level;
  } else {
    lightbulb_state.lightness_target = target->lightness.level;
  }

  if (lightbulb_state.lightness_current == lightbulb_state.lightness_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall lightness to %u with transition=%lu ms\r\n",
        lightbulb_state.lightness_target,
        transition_ms);
    sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                 transition_ms);

    if (transition_ms == IMMEDIATE) {
      lightbulb_state.lightness_current = current->lightness.level;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lighting_transition_complete_timer,
                                             transition_ms,
                                             lighting_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Lighting Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  lightness_update_and_publish(element_index,
                               transition_ms,
                               mesh_lighting_state_lightness_actual);
}

/***************************************************************************//**
 * This function is called when a light lightness request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void lightness_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.lightness_current = lightbulb_state.lightness_target;
  if (lightbulb_state.lightness_target != 0) {
    lightbulb_state.lightness_last = lightbulb_state.lightness_target;
  }

  log("Transition complete. New level is %u\r\n",
      lightbulb_state.lightness_current);

  lightbulb_state_changed();
  lightness_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                               IMMEDIATE,
                               lightness_kind);
}

/***************************************************************************//**
 * This function is called when delay for light lightness request has completed.
 ******************************************************************************/
static void delayed_lightness_request(void)
{
  log("Starting delayed lightness request: level %u -> %u, %lu ms\r\n",
      lightbulb_state.lightness_current,
      lightbulb_state.lightness_target,
      delayed_lightness_trans
      );

  sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                               delayed_lightness_trans);

  if (delayed_lightness_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.lightness_current = lightbulb_state.lightness_target;
    if (lightbulb_state.lightness_target != 0) {
      lightbulb_state.lightness_last = lightbulb_state.lightness_target;
    }

    lightbulb_state_changed();
    lightness_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                                 delayed_lightness_trans,
                                 lightness_kind);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&lighting_transition_complete_timer,
                                           delayed_lightness_trans,
                                           lighting_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start Lighting Transition Complete timer\n");
  }
}

/** @} (end addtogroup LightLightness) */

/***************************************************************************//**
 * \defgroup LightLightnessSetup
 * \brief Light Lightness Setup Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightLightnessSetup
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light lightness setup request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] kind           Type of state used in light lightness setup response.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightness_setup_response(uint16_t element_index,
                                            uint16_t client_addr,
                                            uint16_t appkey_index,
                                            mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_lightness_default:
      current.lightness.level = lightbulb_state.lightness_default;
      break;

    case mesh_lighting_state_lightness_range:
      current.lightness_range.min = lightbulb_state.lightness_min;
      current.lightness_range.max = lightbulb_state.lightness_max;
      break;

    default:
      break;
  }

  return generic_server_respond(MESH_LIGHTING_LIGHTNESS_SETUP_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                NULL,
                                0,
                                0x00);
}

/***************************************************************************//**
 * Update light lightness setup state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] kind           Type of state used in light lightness setup update.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightness_setup_update(uint16_t element_index,
                                          mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_lightness_default:
      current.lightness.level = lightbulb_state.lightness_default;
      break;

    case mesh_lighting_state_lightness_range:
      current.lightness_range.min = lightbulb_state.lightness_min;
      current.lightness_range.max = lightbulb_state.lightness_max;
      break;

    default:
      break;
  }

  return generic_server_update(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               NULL,
                               0);
}

/***************************************************************************//**
 * This function process the requests for the light lightness setup model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void lightness_setup_request(uint16_t model_id,
                                    uint16_t element_index,
                                    uint16_t client_addr,
                                    uint16_t server_addr,
                                    uint16_t appkey_index,
                                    const struct mesh_generic_request *request,
                                    uint32_t transition_ms,
                                    uint16_t delay_ms,
                                    uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;
  (void)transition_ms;
  (void)delay_ms;

  mesh_generic_state_t kind = mesh_generic_state_last;
  switch (request->kind) {
    case mesh_lighting_request_lightness_default:
      kind = mesh_lighting_state_lightness_default;
      log("lightness_setup_request: state=lightness_default, default_lightness=%u\r\n",
          request->lightness);

      if (lightbulb_state.lightness_default == request->lightness) {
        log("Request for current state received; no op\r\n");
      } else {
        log("Setting default lightness to <%u>\r\n", request->lightness);
        lightbulb_state.lightness_default = request->lightness;
        lightbulb_state_changed();
      }
      break;

    case mesh_lighting_request_lightness_range:
      kind = mesh_lighting_state_lightness_range;
      log("lightness_setup_request: state=lightness_range, min_lightness=%u, max_lightness=%u\r\n",
          request->lightness_range.min, request->lightness_range.max);

      if ((lightbulb_state.lightness_min == request->lightness_range.min)
          && (lightbulb_state.lightness_max == request->lightness_range.max)) {
        log("Request for current state received; no op\r\n");
      } else {
        if (lightbulb_state.lightness_min != request->lightness_range.min) {
          log("Setting min lightness to <%u>\r\n",
              request->lightness_range.min);
          lightbulb_state.lightness_min = request->lightness_range.min;
          if (lightbulb_state.lightness_current < request->lightness_range.min
              && lightbulb_state.lightness_current != 0) {
            lightbulb_state.lightness_current = request->lightness_range.min;
            sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                         IMMEDIATE);
          }
        }
        if (lightbulb_state.lightness_max != request->lightness_range.max) {
          log("Setting max lightness to <%u>\r\n",
              request->lightness_range.max);
          lightbulb_state.lightness_max = request->lightness_range.max;
          if (lightbulb_state.lightness_current > request->lightness_range.max) {
            lightbulb_state.lightness_current = request->lightness_range.max;
            sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                         IMMEDIATE);
          }
        }
        lightbulb_state_changed();
      }
      break;

    default:
      break;
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    lightness_setup_response(element_index, client_addr, appkey_index, kind);
  } else {
    lightness_setup_update(element_index, kind);
  }
}

/***************************************************************************//**
 * This function is a handler for light lightness setup change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void lightness_setup_change(uint16_t model_id,
                                   uint16_t element_index,
                                   const struct mesh_generic_state *current,
                                   const struct mesh_generic_state *target,
                                   uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  switch (current->kind) {
    case mesh_lighting_state_lightness_default:
      if (lightbulb_state.lightness_default != current->lightness.level) {
        log("Default lightness update: from %u to %u\r\n",
            lightbulb_state.lightness_default,
            current->lightness.level);
        lightbulb_state.lightness_default = current->lightness.level;
        lightbulb_state_changed();
      } else {
        log("Default lightness update -same value (%u)\r\n",
            lightbulb_state.lightness_default);
      }
      break;

    case mesh_lighting_state_lightness_range:
      if (lightbulb_state.lightness_min != current->lightness_range.min) {
        log("Min lightness update: from %u to %u\r\n",
            lightbulb_state.lightness_min,
            current->lightness_range.min);
        lightbulb_state.lightness_min = current->lightness_range.min;
        lightbulb_state_changed();
      } else {
        log("Min lightness update -same value (%u)\r\n",
            lightbulb_state.lightness_min);
      }

      if (lightbulb_state.lightness_max != current->lightness_range.max) {
        log("Max lightness update: from %u to %u\r\n",
            lightbulb_state.lightness_max,
            current->lightness_range.max);
        lightbulb_state.lightness_max = current->lightness_range.max;
        lightbulb_state_changed();
      } else {
        log("Max lightness update -same value (%u)\r\n",
            lightbulb_state.lightness_max);
      }

      break;

    default:
      break;
  }
}

/** @} (end addtogroup LightLightnessSetup) */

/***************************************************************************//**
 * \defgroup PriGenericLevel
 * \brief Generic Level Server model on primary element.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PriGenericLevel
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic level request on primary element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t pri_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return generic_server_respond(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                0x00);
}

/***************************************************************************//**
 * Update generic level state on primary element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t pri_level_update(uint16_t element_index,
                                    uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update generic level state on primary element
 * and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t pri_level_update_and_publish(uint16_t element_index,
                                                uint32_t remaining_ms)
{
  sl_status_t e;

  e = pri_level_update(element_index, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_level);
  }

  return e;
}

/***************************************************************************//**
 * Schedule next generic level move request on primary element.
 *
 * @param[in] remaining_delta   The remaining level delta to the target state.
 ******************************************************************************/
static void pri_level_move_schedule_next_request(int32_t remaining_delta)
{
  uint32_t transition_ms = 0;
  if (abs(remaining_delta) < abs(move_pri_level_delta)) {
    transition_ms = (uint32_t)(((int64_t)move_pri_level_trans * remaining_delta)
                               / move_pri_level_delta);
    sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                 transition_ms);
  } else {
    transition_ms = move_pri_level_trans;
    sl_btmesh_lighting_set_level(lightbulb_state.lightness_current
                                 + move_pri_level_delta,
                                 move_pri_level_trans);
  }
  sl_status_t sc = sl_simple_timer_start(&lighting_pri_level_move_timer,
                                         transition_ms,
                                         lighting_pri_level_move_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start Pri Level timer\n");
}

/***************************************************************************//**
 * Handle generic level move request on primary element.
 ******************************************************************************/
static void pri_level_move_request(void)
{
  log("Primary level move: level %d -> %d, delta %d in %lu ms\r\n",
      lightbulb_state.pri_level_current,
      lightbulb_state.pri_level_target,
      move_pri_level_delta,
      move_pri_level_trans);

  int32_t remaining_delta = (int32_t)lightbulb_state.pri_level_target
                            - lightbulb_state.pri_level_current;

  if (abs(remaining_delta) < abs(move_pri_level_delta)) {
    // end of move level as it reached target state
    lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;
    lightbulb_state.lightness_current = lightbulb_state.lightness_target;
  } else {
    lightbulb_state.pri_level_current += move_pri_level_delta;
    lightbulb_state.lightness_current += move_pri_level_delta;
  }
  lightbulb_state_changed();
  pri_level_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                               UNKNOWN_REMAINING_TIME);

  remaining_delta = (int32_t)lightbulb_state.pri_level_target
                    - lightbulb_state.pri_level_current;
  if (remaining_delta != 0) {
    pri_level_move_schedule_next_request(remaining_delta);
  }
}

/***************************************************************************//**
 * Stop generic level move on primary element.
 ******************************************************************************/
static void pri_level_move_stop(void)
{
  // Cancel timers
  sl_status_t sc = sl_simple_timer_stop(&lighting_delayed_pri_level_timer);
  app_assert_status_f(sc, "Failed to stop Delayed Primary Level timer\n");
  sc = sl_simple_timer_stop(&lighting_pri_level_move_timer);
  app_assert_status_f(sc, "Failed to stop Primary Level Move timer\n");
  //Reset move parameters
  move_pri_level_delta = 0;
  move_pri_level_trans = 0;
}

/***************************************************************************//**
 * This function process the requests for the generic level model
 * on primary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void pri_level_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;

  uint16_t lightness;
  uint32_t remaining_ms = UNKNOWN_REMAINING_TIME;

  switch (request->kind) {
    case mesh_generic_request_level:
      log("pri_level_request (generic): level=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);

      pri_level_move_stop();
      if (lightbulb_state.pri_level_current == request->level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.pri_level_target = request->level;
      } else {
        log("Setting pri_level to <%d>\r\n", request->level);

        lightness = request->level + 32768;

        if (transition_ms == 0 && delay_ms == 0) { // Immediate change
          lightbulb_state.pri_level_current = request->level;
          lightbulb_state.pri_level_target = request->level;
          lightbulb_state.lightness_current = lightness;
          lightbulb_state.lightness_target = lightness;

          // update LED Level
          sl_btmesh_lighting_set_level(lightness, IMMEDIATE);
        } else if (delay_ms > 0) {
          // a delay has been specified for the change. Start a soft timer
          // that will trigger the change after the given delay
          // Current state remains as is for now
          lightbulb_state.pri_level_target = request->level;
          lightbulb_state.lightness_target = lightness;
          pri_level_request_kind = mesh_generic_request_level;
          sl_status_t sc = sl_simple_timer_start(&lighting_delayed_pri_level_timer,
                                                 delay_ms,
                                                 lighting_delayed_pri_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Primary Level timer\n");

          // store transition parameter for later use
          delayed_pri_level_trans = transition_ms;
        } else {
          // no delay but transition time has been set.
          lightbulb_state.pri_level_target = request->level;
          lightbulb_state.lightness_target = lightness;
          sl_btmesh_lighting_set_level(lightness, transition_ms);

          // lightbulb current state will be updated when transition is complete
          sl_status_t sc = sl_simple_timer_start(&lighting_level_transition_complete_timer,
                                                 transition_ms,
                                                 lighting_level_transition_complete_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Primary Level Transition Complete timer\n");
        }

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }

      remaining_ms = delay_ms + transition_ms;
      break;

    case mesh_generic_request_level_move: {
      log("pri_level_request (move): delta=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);
      // Store move parameters
      move_pri_level_delta = request->level;
      move_pri_level_trans = transition_ms;

      int16_t requested_level = 0;
      if (move_pri_level_delta > 0) {
        requested_level = 0x7FFF; // Max level value
      } else if (move_pri_level_delta < 0) {
        requested_level = 0x8000; // Min level value
      }

      if (lightbulb_state.pri_level_current == requested_level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.pri_level_target = requested_level;
        remaining_ms = IMMEDIATE;
      } else {
        log("Setting pri_level to <%d>\r\n", requested_level);

        lightness = requested_level + 32768;

        if (delay_ms > 0) {
          // a delay has been specified for the move. Start a soft timer
          // that will trigger the move after the given delay
          // Current state remains as is for now
          lightbulb_state.pri_level_target = requested_level;
          lightbulb_state.lightness_target = lightness;
          pri_level_request_kind = mesh_generic_request_level_move;
          sl_status_t sc = sl_simple_timer_start(&lighting_delayed_pri_level_timer,
                                                 delay_ms,
                                                 lighting_delayed_pri_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Primary Level timer\n");
        } else {
          // no delay so start move
          lightbulb_state.pri_level_target = requested_level;
          lightbulb_state.lightness_target = lightness;

          int32_t remaining_delta = (int32_t)lightbulb_state.pri_level_target
                                    - lightbulb_state.pri_level_current;
          pri_level_move_schedule_next_request(remaining_delta);
        }
        remaining_ms = UNKNOWN_REMAINING_TIME;

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }
      break;
    }

    case mesh_generic_request_level_halt:
      log("pri_level_request (halt)\r\n");

      // Set current state
      lightbulb_state.lightness_current = sl_btmesh_get_level();
      lightbulb_state.lightness_target = lightbulb_state.lightness_current;
      lightbulb_state.pri_level_current = lightbulb_state.lightness_current
                                          - 32768;
      lightbulb_state.pri_level_target = lightbulb_state.pri_level_current;
      if (delay_ms > 0) {
        // a delay has been specified for the move halt. Start a soft timer
        // that will trigger the move halt after the given delay
        // Current state remains as is for now
        remaining_ms = delay_ms;
        pri_level_request_kind = mesh_generic_request_level_halt;
        sl_status_t sc = sl_simple_timer_start(&lighting_delayed_pri_level_timer,
                                               delay_ms,
                                               lighting_delayed_pri_level_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Delayed Primary Level timer\n");
      } else {
        pri_level_move_stop();
        sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                     IMMEDIATE);
        remaining_ms = IMMEDIATE;
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    pri_level_response(element_index, client_addr, appkey_index, remaining_ms);
  }
  pri_level_update_and_publish(element_index, remaining_ms);

  // publish to bound states
  generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_lightness_actual);
}

/***************************************************************************//**
 * This function is a handler for generic level change event
 * on primary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void pri_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.pri_level_current != current->level.level) {
    log("Primary level update: from %d to %d\r\n",
        lightbulb_state.pri_level_current,
        current->level.level);
    lightbulb_state.pri_level_current = current->level.level;
    lightbulb_state_changed();
    pri_level_move_stop();
  } else {
    log("Primary level update -same value (%d)\r\n",
        lightbulb_state.pri_level_current);
  }
}

/***************************************************************************//**
 * This function is a handler for generic level recall event on primary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void pri_level_recall(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t transition_ms)
{
  (void)model_id;

  log("Primary Generic Level recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.pri_level_target = current->level.level;
  } else {
    lightbulb_state.pri_level_target = target->level.level;
  }

  if (lightbulb_state.pri_level_current == lightbulb_state.pri_level_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall pri_level to %d with transition=%lu ms\r\n",
        lightbulb_state.pri_level_target,
        transition_ms);
    if (transition_ms == IMMEDIATE) {
      lightbulb_state.pri_level_current = current->level.level;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lighting_level_transition_complete_timer,
                                             transition_ms,
                                             lighting_level_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Primary Level Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  pri_level_update_and_publish(element_index, transition_ms);
}

/***************************************************************************//**
 * This function is called when a generic level request on primary element
 * with non-zero transition time has completed.
 ******************************************************************************/
static void pri_level_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;
  lightbulb_state.lightness_current = lightbulb_state.lightness_target;

  log("Transition complete. New pri_level is %d\r\n",
      lightbulb_state.pri_level_current);

  lightbulb_state_changed();
  pri_level_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for generic level request
 * on primary element has completed.
 ******************************************************************************/
static void delayed_pri_level_request(void)
{
  log("Starting delayed primary level request: level %d -> %d, %lu ms\r\n",
      lightbulb_state.pri_level_current,
      lightbulb_state.pri_level_target,
      delayed_pri_level_trans);

  switch (pri_level_request_kind) {
    case mesh_generic_request_level:
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                   delayed_pri_level_trans);

      if (delayed_pri_level_trans == 0) {
        // no transition delay, update state immediately
        lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;
        lightbulb_state.lightness_current = lightbulb_state.lightness_target;

        lightbulb_state_changed();
        pri_level_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                                     delayed_pri_level_trans);
      } else {
        // state is updated when transition is complete
        sl_status_t sc = sl_simple_timer_start(&lighting_level_transition_complete_timer,
                                               delayed_pri_level_trans,
                                               lighting_level_transition_complete_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Primary Level Transition Complete timer\n");
      }
      break;

    case mesh_generic_request_level_move:
      pri_level_move_schedule_next_request((int32_t)lightbulb_state.pri_level_target
                                           - lightbulb_state.pri_level_current);
      pri_level_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                                   UNKNOWN_REMAINING_TIME);
      break;

    case mesh_generic_request_level_halt:
      // Set current state
      lightbulb_state.lightness_current = sl_btmesh_get_level();
      lightbulb_state.lightness_target = lightbulb_state.lightness_current;
      lightbulb_state.pri_level_current = lightbulb_state.lightness_current
                                          - 32768;
      lightbulb_state.pri_level_target = lightbulb_state.pri_level_current;
      pri_level_move_stop();
      sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                   IMMEDIATE);
      pri_level_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN, IMMEDIATE);
      break;

    default:
      break;
  }
}

/** @} (end addtogroup PriGenericLevel) */

/***************************************************************************//**
 * Initialization of the models supported by this node.
 * This function registers callbacks for each of the supported models.
 ******************************************************************************/
static void init_models(void)
{
  generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  onoff_request,
                                  onoff_change,
                                  onoff_recall);

  generic_server_register_handler(MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  power_onoff_request,
                                  power_onoff_change,
                                  NULL);

  generic_server_register_handler(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  transtime_request,
                                  transtime_change,
                                  NULL);

  generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  lightness_request,
                                  lightness_change,
                                  lightness_recall);

  generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SETUP_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  lightness_setup_request,
                                  lightness_setup_change,
                                  NULL);

  generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                  BTMESH_LIGHTING_SERVER_MAIN,
                                  pri_level_request,
                                  pri_level_change,
                                  pri_level_recall);
}

/***************************************************************************//**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state.
 * If PS key with ID SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL does not exist or loading failed,
 * lightbulb_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_load(void)
{
  sl_status_t sc;
  size_t ps_len = 0;
  struct lightbulb_state ps_data;

  sc = sl_bt_nvm_load(SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL,
                      sizeof(ps_data),
                      &ps_len,
                      (uint8_t *)&ps_data);

  // Set default values if ps_load fail or size of lightbulb_state has changed
  if ((sc != SL_STATUS_OK) || (ps_len != sizeof(struct lightbulb_state))) {
    memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
    lightbulb_state.lightness_last    = LIGHTNESS_LAST_DEFAULT;
    lightbulb_state.lightness_default = LIGHTNESS_DEFAULT_DEFAULT;
    lightbulb_state.lightness_min     = SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MIN_CFG_VAL;
    lightbulb_state.lightness_max     = SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MAX_CFG_VAL;

    // Check if default values are valid and correct them if needed
    lightbulb_state_validate_and_correct();

    if (sc == SL_STATUS_OK) {
      // The sl_bt_nvm_load call was successful but the size of the loaded data
      // differs from the expected size therefore error code shall be set
      sc = SL_STATUS_INVALID_STATE;
      log("Lighting server lightbulb state loaded from PS with invalid size, "
          "use defaults. (expected=%zd,actual=%zd)\r\n",
          sizeof(struct lightbulb_state),
          ps_len);
    } else {
      log_status_f(sc,
                   "Lighting server lightbulb state load from PS failed "
                   "or nvm is empty, use defaults.\r\n");
    }
  } else {
    memcpy(&lightbulb_state, &ps_data, ps_len);
  }

  return sc;
}

/***************************************************************************//**
 * This function validates the lighbulb_state and change it if it is against
 * the specification.
 ******************************************************************************/
static void lightbulb_state_validate_and_correct(void)
{
  if (lightbulb_state.lightness_min > lightbulb_state.lightness_max) {
    lightbulb_state.lightness_min = lightbulb_state.lightness_max;
  }
  if (lightbulb_state.lightness_default) {
    if (lightbulb_state.lightness_default < lightbulb_state.lightness_min) {
      lightbulb_state.lightness_default = lightbulb_state.lightness_min;
    }
    if (lightbulb_state.lightness_default > lightbulb_state.lightness_max) {
      lightbulb_state.lightness_default = lightbulb_state.lightness_max;
    }
  }
  if (lightbulb_state.lightness_current < lightbulb_state.lightness_min) {
    lightbulb_state.lightness_current = lightbulb_state.lightness_min;
  }
  if (lightbulb_state.lightness_current > lightbulb_state.lightness_max) {
    lightbulb_state.lightness_current = lightbulb_state.lightness_max;
  }
  if (lightbulb_state.lightness_target < lightbulb_state.lightness_min) {
    lightbulb_state.lightness_target = lightbulb_state.lightness_min;
  }
  if (lightbulb_state.lightness_target > lightbulb_state.lightness_max) {
    lightbulb_state.lightness_target = lightbulb_state.lightness_max;
  }
}

/***************************************************************************//**
 * This function saves the current light state in Persistent Storage so that
 * the data is preserved over reboots and power cycles.
 * The light state is hold in a global variable lightbulb_state.
 * A PS key with ID SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL is used to store the whole struct.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_store(void)
{
  sl_status_t sc;

  sc = sl_bt_nvm_save(SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL,
                      sizeof(struct lightbulb_state),
                      (const uint8_t*)&lightbulb_state);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "Lighting server lightbulb state store in PS failed.\r\n");

  return sc;
}

/***************************************************************************//**
 * This function is called each time the lightbulb state in RAM is changed.
 * It sets up a soft timer that will save the state in flash after small delay.
 * The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lightbulb_state_changed(void)
{
  sl_status_t sc = sl_simple_timer_start(&lighting_state_store_timer,
                                         SL_BTMESH_LIGHTING_SERVER_NVM_SAVE_TIME_CFG_VAL,
                                         lighting_state_store_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start State Store timer\n");
}

/*******************************************************************************
 * Lightbulb state initialization.
 * This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 ******************************************************************************/
void sl_btmesh_lighting_server_init(void)
{
  memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));

  lightbulb_state_load();

  // Handle on power up behavior
  uint32_t transition_ms = sl_btmesh_get_default_transition_time();
  switch (lightbulb_state.onpowerup) {
    case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
      log("On power up state is OFF\r\n");
      lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_OFF;
      lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_OFF;
      lightbulb_state.lightness_current = 0;
      lightbulb_state.lightness_target = 0;
      sl_btmesh_set_state(LED_STATE_OFF);
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_ON:
      log("On power up state is ON\r\n");
      lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_ON;
      if (lightbulb_state.lightness_default == 0) {
        lightbulb_state.lightness_current = lightbulb_state.lightness_last;
        lightbulb_state.lightness_target = lightbulb_state.lightness_last;
      } else {
        lightbulb_state.lightness_current = lightbulb_state.lightness_default;
        lightbulb_state.lightness_target = lightbulb_state.lightness_default;
      }
      if (transition_ms > 0) {
        lightbulb_state.lightness_current = 0;
        sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                     IMMEDIATE);
        sl_status_t sc =
          sl_simple_timer_start(&lighting_transition_complete_timer,
                                transition_ms,
                                lighting_transition_complete_timer_cb,
                                NO_CALLBACK_DATA,
                                false);
        app_assert_status_f(sc, "Failed to start Lighting Transition Complete timer\n");
        sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                     transition_ms);
      } else {
        sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                     IMMEDIATE);
      }
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
      log("On power up state is RESTORE\r\n");
#ifdef SL_CATALOG_BTMESH_LC_SERVER_PRESENT
      if (lc_get_mode() == 0)
#endif
      {
        if (transition_ms > 0 && lightbulb_state.lightness_target > 0) {
          lightbulb_state.lightness_current = 0;
          sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                       IMMEDIATE);
          sl_status_t sc =
            sl_simple_timer_start(&lighting_transition_complete_timer,
                                  transition_ms,
                                  lighting_transition_complete_timer_cb,
                                  NO_CALLBACK_DATA,
                                  false);
          app_assert_status_f(sc, "Failed to start Lighting Transition Complete timer\n");
          sl_btmesh_lighting_set_level(lightbulb_state.lightness_target,
                                       transition_ms);
        } else {
          lightbulb_state.lightness_current = lightbulb_state.lightness_target;
          sl_btmesh_lighting_set_level(lightbulb_state.lightness_current,
                                       IMMEDIATE);
        }

        if (lightbulb_state.lightness_current) {
          lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
        } else {
          lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_OFF;
        }

        if (lightbulb_state.lightness_target) {
          lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_ON;
        } else {
          lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_OFF;
        }
      }

      break;

    default:
      break;
  }

  lightbulb_state_changed();
  init_models();
  transtime_update(BTMESH_LIGHTING_SERVER_MAIN);
  lightness_setup_update(BTMESH_LIGHTING_SERVER_MAIN,
                         mesh_lighting_state_lightness_default);

  lightness_setup_update(BTMESH_LIGHTING_SERVER_MAIN,
                         mesh_lighting_state_lightness_range);

  power_onoff_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN);

#ifdef SL_CATALOG_BTMESH_LC_PRESENT
  if (lc_get_mode() == 0)
#endif
  {
    onoff_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                             IMMEDIATE);

    lightness_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN,
                                 IMMEDIATE,
                                 mesh_lighting_state_lightness_actual);
  }
}

/***************************************************************************//**
 * @addtogroup BtmeshWrappers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_respond to log if the Btmesh API call
 * results in error. The parameters and the return value of the wrapper and
 * the wrapped functions are the same.
 ******************************************************************************/
static sl_status_t generic_server_respond(uint16_t model_id,
                                          uint16_t element_index,
                                          uint16_t client_addr,
                                          uint16_t appkey_index,
                                          const struct mesh_generic_state *current,
                                          const struct mesh_generic_state *target,
                                          uint32_t remaining_ms,
                                          uint8_t response_flags)
{
  sl_status_t sc = mesh_lib_generic_server_respond(model_id,
                                                   element_index,
                                                   client_addr,
                                                   appkey_index,
                                                   current,
                                                   target,
                                                   remaining_ms,
                                                   response_flags);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "Lighting server respond failed "
                     "(claddr=0x%04x,mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                     client_addr,
                     model_id,
                     element_index,
                     current->kind);
  return sc;
}

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_update to log if the Btmesh API call
 * results in error. The parameters and the return value of the wrapper and
 * the wrapped functions are the same.
 ******************************************************************************/
static sl_status_t generic_server_update(uint16_t model_id,
                                         uint16_t element_index,
                                         const struct mesh_generic_state *current,
                                         const struct mesh_generic_state *target,
                                         uint32_t remaining_ms)
{
  sl_status_t sc = mesh_lib_generic_server_update(model_id,
                                                  element_index,
                                                  current,
                                                  target,
                                                  remaining_ms);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "Lighting server state update failed "
                     "(mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                     model_id,
                     element_index,
                     current->kind);
  return sc;
}

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_publish to log if the Btmesh API call
 * results in error. The parameters and the return value of the two functions
 * are the same.
 ******************************************************************************/
static sl_status_t generic_server_publish(uint16_t model_id,
                                          uint16_t element_index,
                                          mesh_generic_state_t kind)
{
  sl_status_t sc;

  sc = mesh_lib_generic_server_publish(model_id, element_index, kind);

  log_btmesh_status_f(sc,
                      "Lighting server state publish failed "
                      "(mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                      model_id,
                      element_index,
                      kind);
  return sc;
}

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_register_handler with an assert which
 * detects if the Btmesh API call results in error. The parameters of the two
 * functions are the same but the wrapper does not have return value.
 ******************************************************************************/
static void generic_server_register_handler(uint16_t model_id,
                                            uint16_t elem_index,
                                            mesh_lib_generic_server_client_request_cb cb,
                                            mesh_lib_generic_server_change_cb ch,
                                            mesh_lib_generic_server_recall_cb recall)
{
  sl_status_t sc =
    mesh_lib_generic_server_register_handler(model_id, elem_index, cb, ch, recall);

  app_assert_status_f(sc,
                      "Lighting server failed to register handlers "
                      "(mdl=0x%04x,elem=%d)\n",
                      model_id,
                      elem_index);
}

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
/***************************************************************************//**
 * Wrapper for sl_btmesh_scene_server_reset_register with an assert which
 * detects if the Btmesh API call results in error. The parameters of the two
 * functions are the same but the wrapper does not have return value.
 * The scene server register shall be reset if the state of the model changes in
 * order to clear the current scene.
 * This function is available only if the btmesh_scene_server component is added
 * to the project.
 ******************************************************************************/
static void scene_server_reset_register_impl(uint16_t elem_index)
{
  sl_status_t sc = sl_btmesh_scene_server_reset_register(elem_index);

  // The function can fail if there is no scene server model in the element or
  // the btmesh_feature_scene_server component is not present. Both of these
  // are configuration issues so assert can be used.
  app_assert_status_f(sc, "Lighting server failed to reset scene register.\n");
}
#endif

/** @} (end addtogroup BtmeshWrappers) */

// -----------------------------------------------------------------------------
// Timer Callbacks

/***************************************************************************//**
 * Callback for the timer handling generic level move
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_pri_level_move_timer_cb(sl_simple_timer_t *handle,
                                             void *data)
{
  (void)data;
  (void)handle;
  // handling of generic level move, update the lightbulb state
  pri_level_move_request();
}

/***************************************************************************//**
 * Callback for the timer handling lightness request transition
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                  void *data)
{
  (void)data;
  (void)handle;
  // transition for a lightness request has completed
  // update the lightbulb state
  lightness_transition_complete();
}

/***************************************************************************//**
 * Callback for the timer handling primary generic level request transition
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data)
{
  (void)data;
  (void)handle;
  // transition for a primary generic level request has completed,
  // update the lightbulb state
  pri_level_transition_complete();
}

/***************************************************************************//**
 * Callback for the timer handling on/off request transition
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_onoff_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data)
{
  (void)data;
  (void)handle;
  // transition for an on/off request has completed,
  // update the lightbulb state
  onoff_transition_complete();
}

/***************************************************************************//**
 * Callback for the timer handling delayed primary generic level request
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_delayed_pri_level_timer_cb(sl_simple_timer_t *handle,
                                                void *data)
{
  (void)data;
  (void)handle;
  // delay for a primary generic level request has passed,
  // now process the request
  delayed_pri_level_request();
}

/***************************************************************************//**
 * Callback for the timer handling delayed lightness request
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_delayed_lightness_request_timer_cb(sl_simple_timer_t *handle,
                                                        void *data)
{
  (void)data;
  (void)handle;
  // delay for a lightness request has passed, now process the request
  delayed_lightness_request();
}

/***************************************************************************//**
 * Callback for the timer handling delayed on/off request
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_delayed_onoff_request_timer_cb(sl_simple_timer_t *handle,
                                                    void *data)
{
  (void)data;
  (void)handle;
  // delay for an on/off request has passed, now process the request
  delayed_onoff_request();
}

/***************************************************************************//**
 * Callback for the timer handling storage state change
 *
 * @param[in] handle Pointer to the timer handle
 * @param[in] data   Pointer to the timer data
 ******************************************************************************/
static void lighting_state_store_timer_cb(sl_simple_timer_t *handle,
                                          void *data)
{
  (void)data;
  (void)handle;
  // save the lightbulb state
  lightbulb_state_store();
}

/** @} (end addtogroup Lighting Server) */
