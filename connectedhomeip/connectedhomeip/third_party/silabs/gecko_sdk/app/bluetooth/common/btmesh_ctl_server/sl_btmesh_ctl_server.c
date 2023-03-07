/***************************************************************************//**
 * @file
 * @brief Bt Mesh Lighting Client module
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

#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_dcd.h"

#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_lib.h"

#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_ctl_server_config.h"
#include "sl_btmesh_ctl_server.h"
#include "sl_btmesh_lighting_server.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup CTL_Server
 * @{
 ******************************************************************************/

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
#define scene_server_reset_register(elem_index) \
  scene_server_reset_register_impl(elem_index)
#else
#define scene_server_reset_register(elem_index)
#endif

#define NO_FLAGS              0      ///< No flags used for message
#define IMMEDIATE             0      ///< Immediate transition time is 0 seconds
#define NO_CALLBACK_DATA      (void *)NULL   ///< Callback has no parameters
#define HIGH_PRIORITY         0              ///< High Priority
/// Values greater than max 37200000 are treated as unknown remaining time
#define UNKNOWN_REMAINING_TIME      40000000

/// Lightbulb state
static PACKSTRUCT(struct lightbulb_state {
  // Temperature server
  uint16_t temperature_current;   ///< Current temperature value
  uint16_t temperature_target;    ///< Target temperature value
  uint16_t temperature_default;   ///< Default temperature value
  uint16_t temperature_min;       ///< Minimum temperature value
  uint16_t temperature_max;       ///< Maximum temperature value

  // Delta UV
  int16_t deltauv_current;        ///< Current delta UV value
  int16_t deltauv_target;         ///< Target delta UV value
  int16_t deltauv_default;        ///< Default delta UV value

  // Secondary Generic Level
  int16_t sec_level_current;      ///< Current secondary generic level value
  int16_t sec_level_target;       ///< Target secondary generic level value
}) lightbulb_state;

static sl_status_t ctl_temperature_update(uint16_t element_index,
                                          uint32_t remaining_ms);

/// copy of transition delay parameter, needed for delayed ctl request
static uint32_t delayed_ctl_trans = 0;
/// copy of transition delay parameter, needed for delayed temperature request
static uint32_t delayed_ctl_temperature_trans = 0;
/// copy of transition delay parameter, needed for
/// delayed secondary generic level request
static uint32_t delayed_sec_level_trans = 0;
/// copy of generic request kind, needed for delayed secondary generic request
static mesh_generic_request_t sec_level_request_kind = mesh_generic_request_level;
/// copy of move transition parameter for secondary generic request
static uint32_t move_sec_level_trans = 0;
/// copy of move delta parameter for secondary generic request
static int16_t move_sec_level_delta = 0;

// Timer handles
static sl_simple_timer_t ctl_sec_level_move_timer;
static sl_simple_timer_t ctl_sec_level_transition_timer;
static sl_simple_timer_t ctl_temp_transition_timer;
static sl_simple_timer_t ctl_transition_complete_timer;
static sl_simple_timer_t ctl_delayed_sec_level_timer;
static sl_simple_timer_t ctl_delayed_ctl_temperature_timer;
static sl_simple_timer_t ctl_delayed_ctl_request_timer;
static sl_simple_timer_t ctl_state_store_timer;

// Timer callbacks
static void ctl_sec_level_move_timer_cb(sl_simple_timer_t *handle,
                                        void *data);
static void ctl_sec_level_transition_timer_cb(sl_simple_timer_t *handle,
                                              void *data);
static void ctl_temp_transition_timer_cb(sl_simple_timer_t *handle,
                                         void *data);
static void ctl_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                             void *data);
static void ctl_delayed_sec_level_timer_cb(sl_simple_timer_t *handle,
                                           void *data);
static void ctl_delayed_ctl_temperature_timer_cb(sl_simple_timer_t *handle,
                                                 void *data);
static void ctl_delayed_ctl_request_timer_cb(sl_simple_timer_t *handle,
                                             void *data);
static void ctl_state_store_timer_cb(sl_simple_timer_t *handle,
                                     void *data);

/***************************************************************************//**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state.
 * If PS key with ID SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL does not exist or loading failed,
 * lightbulb_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_load(void);

/***************************************************************************//**
 * This function validates the lighbulb_state and change it if it is against
 * the specification.
 ******************************************************************************/
static void lightbulb_state_validate_and_correct(void);

/***************************************************************************//**
 * This function is called each time the lightbulb state in RAM is changed.
 * It sets up a soft timer that will save the state in flash after small delay.
 * The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lightbulb_state_changed(void);

/***************************************************************************//**
 * This function validates the lighbulb_state and change it if it is against
 * the specification.
 ******************************************************************************/
static void lightbulb_state_validate_and_correct(void);

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
                                          uint8_t response_flags);

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_update to log if the Btmesh API call
 * results in error. The parameters and the return value of the wrapper and
 * the wrapped functions are the same.
 ******************************************************************************/
static sl_status_t generic_server_update(uint16_t model_id,
                                         uint16_t element_index,
                                         const struct mesh_generic_state *current,
                                         const struct mesh_generic_state *target,
                                         uint32_t remaining_ms);

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_publish to log if the Btmesh API call
 * results in error. The parameters and the return value of the wrapper and
 * the wrapped functions are the same.
 ******************************************************************************/
static sl_status_t generic_server_publish(uint16_t model_id,
                                          uint16_t element_index,
                                          mesh_generic_state_t kind);

/***************************************************************************//**
 * Wrapper for mesh_lib_generic_server_register_handler with an assert which
 * detects if the Btmesh API call results in error. The parameters of the two
 * functions are the same but the wrapper does not have return value.
 ******************************************************************************/
static void generic_server_register_handler(uint16_t model_id,
                                            uint16_t elem_index,
                                            mesh_lib_generic_server_client_request_cb cb,
                                            mesh_lib_generic_server_change_cb ch,
                                            mesh_lib_generic_server_recall_cb recall);

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
static void scene_server_reset_register_impl(uint16_t elem_index);
#endif

/***************************************************************************//**
 * \defgroup LightCTL
 * \brief Light CTL Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightCTL
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light CTL request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_response(uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t appkey_index,
                                uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl;
  current.ctl.lightness = sl_btmesh_get_lightness_current();
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl;
  target.ctl.lightness = sl_btmesh_get_lightness_target();
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return generic_server_respond(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                0x00);
}

/***************************************************************************//**
 * Update light CTL state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_update(uint16_t element_index, uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl;
  current.ctl.lightness = sl_btmesh_get_lightness_current();
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl;
  target.ctl.lightness = sl_btmesh_get_lightness_target();
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return generic_server_update(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light CTL state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_update_and_publish(uint16_t element_index,
                                          uint32_t remaining_ms)
{
  sl_status_t e;
  (void)element_index;

  e = ctl_update(BTMESH_LIGHTING_SERVER_MAIN, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                               BTMESH_LIGHTING_SERVER_MAIN,
                               mesh_lighting_state_ctl);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light CTL model.
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
static void ctl_request(uint16_t model_id,
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
  (void)element_index;
  (void)server_addr;

  log("ctl_request: lightness=%u, color temperature=%u, delta_uv=%d, "
      "transition=%lu, delay=%u\r\n",
      request->ctl.lightness,
      request->ctl.temperature,
      request->ctl.deltauv,
      transition_ms,
      delay_ms);

  if ((sl_btmesh_get_lightness_current() == request->ctl.lightness)
      && (lightbulb_state.temperature_current == request->ctl.temperature)
      && (lightbulb_state.deltauv_current == request->ctl.deltauv)) {
    log("Request for current state received; no op\r\n");
  } else {
    if (sl_btmesh_get_lightness_current() != request->ctl.lightness) {
      log("Setting lightness to <%u>\r\n", request->ctl.lightness);
    }
    if (lightbulb_state.temperature_current != request->ctl.temperature) {
      log("Setting color temperature to <%u>\r\n", request->ctl.temperature);
    }
    if (lightbulb_state.deltauv_current != request->ctl.deltauv) {
      log("Setting delta UV to <%d>\r\n", request->ctl.deltauv);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      sl_btmesh_set_lightness_current(request->ctl.lightness);
      sl_btmesh_set_lightness_target(request->ctl.lightness);
      if (request->ctl.lightness != 0) {
        sl_btmesh_set_lightness_last(request->ctl.lightness);
      }

      // update LED PWM duty cycle
      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_current(),
                                   IMMEDIATE);

      lightbulb_state.temperature_current = request->ctl.temperature;
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_current = request->ctl.deltauv;
      lightbulb_state.deltauv_target = request->ctl.deltauv;

      // update LED color temperature
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                  lightbulb_state.deltauv_current,
                                                  IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      sl_btmesh_set_lightness_target(request->ctl.lightness);
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_target = request->ctl.deltauv;
      sl_status_t sc = sl_simple_timer_start(&ctl_delayed_ctl_request_timer,
                                             delay_ms,
                                             ctl_delayed_ctl_request_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed CTL Request timer\n");
      // store transition parameter for later use
      delayed_ctl_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      sl_btmesh_set_lightness_target(request->ctl.lightness);
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_target = request->ctl.deltauv;

      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                                   transition_ms);
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                  lightbulb_state.deltauv_target,
                                                  transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&ctl_transition_complete_timer,
                                             transition_ms,
                                             ctl_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start CTL Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(BTMESH_LIGHTING_SERVER_MAIN);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_response(BTMESH_LIGHTING_SERVER_MAIN, client_addr, appkey_index, remaining_ms);
  }
  ctl_update_and_publish(BTMESH_LIGHTING_SERVER_MAIN, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                         BTMESH_LIGHTING_SERVER_MAIN,
                         mesh_lighting_state_lightness_actual);
  generic_server_publish(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                         BTMESH_CTL_SERVER_TEMPERATURE,
                         mesh_lighting_state_ctl_temperature);
}

/***************************************************************************//**
 * This function is a handler for light CTL change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void ctl_change(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (current->kind != mesh_lighting_state_ctl) {
    // if kind is not 'ctl' then just report the change here
    log("ctl_change, kind %u\r\n", current->kind);
    return;
  }

  if (sl_btmesh_get_lightness_current() != current->ctl.lightness) {
    log("Lightness update: from %u to %u\r\n",
        lightbulb_state.temperature_current,
        current->ctl.lightness);
    sl_btmesh_set_lightness_current(current->ctl.lightness);
    lightbulb_state_changed();
  } else {
    log("Lightness update -same value (%u)\r\n", current->ctl.lightness);
  }

  if (lightbulb_state.temperature_current != current->ctl.temperature) {
    log("Color temperature update: from %u to %u\r\n",
        lightbulb_state.temperature_current,
        current->ctl.temperature);
    lightbulb_state.temperature_current = current->ctl.temperature;
    lightbulb_state_changed();
  } else {
    log("Color temperature update -same value (%u)\r\n",
        lightbulb_state.temperature_current);
  }

  if (lightbulb_state.deltauv_current != current->ctl.deltauv) {
    log("Delta UV update: from %d to %d\r\n",
        lightbulb_state.deltauv_current, current->ctl.deltauv);
    lightbulb_state.deltauv_current = current->ctl.deltauv;
    lightbulb_state_changed();
  } else {
    log("Delta UV update -same value (%d)\r\n",
        lightbulb_state.deltauv_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light CTL recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void ctl_recall(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("Light CTL recall\r\n");
  if (transition_ms == IMMEDIATE) {
    sl_btmesh_set_lightness_target(current->ctl.lightness);
    lightbulb_state.temperature_target = current->ctl.temperature;
    lightbulb_state.deltauv_target = current->ctl.deltauv;
  } else {
    sl_btmesh_set_lightness_target(target->ctl.lightness);
    lightbulb_state.temperature_target = target->ctl.temperature;
    lightbulb_state.deltauv_target = target->ctl.deltauv;
  }

  // Lightness server is mandatory for CTL, so lightness change is handled
  // in lightness_recall function and here we handle temperature and deltauv
  if ((sl_btmesh_get_lightness_current()
       == sl_btmesh_get_lightness_target())
      && (lightbulb_state.temperature_current
          == lightbulb_state.temperature_target)
      && (lightbulb_state.deltauv_current
          == lightbulb_state.deltauv_target)) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall CTL lightness to %u, color temperature to %u, delta UV to %d "
        "with transition=%lu ms\r\n",
        sl_btmesh_get_lightness_target(),
        lightbulb_state.temperature_target,
        lightbulb_state.deltauv_target,
        transition_ms);
    if (sl_btmesh_get_lightness_current()
        != sl_btmesh_get_lightness_target()) {
      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                                   transition_ms);
    }
    if ((lightbulb_state.temperature_current
         != lightbulb_state.temperature_target)
        || (lightbulb_state.deltauv_current
            != lightbulb_state.deltauv_target)) {
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                  lightbulb_state.deltauv_target,
                                                  transition_ms);
    }
    if (transition_ms == IMMEDIATE) {
      sl_btmesh_set_lightness_current(current->ctl.lightness);
      lightbulb_state.temperature_current = current->ctl.temperature;
      lightbulb_state.deltauv_current = current->ctl.deltauv;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&ctl_transition_complete_timer,
                                             transition_ms,
                                             ctl_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start CTL Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  // Lightness substate is updated in lightness_recall, here only temperature
  // substate is updated, it is needed also for LC recall to not set LC mode
  // to zero by bindings
  sl_status_t e;
  e = ctl_temperature_update(BTMESH_CTL_SERVER_TEMPERATURE, transition_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                               BTMESH_LIGHTING_SERVER_MAIN,
                               mesh_lighting_state_ctl);
  }
}

/***************************************************************************//**
 * This function is called when a light CTL request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void ctl_transition_complete(void)
{
  // transition done -> set state, update and publish
  sl_btmesh_set_lightness_current(sl_btmesh_get_lightness_target());
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;
  lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

  log("Transition complete. New lightness is %u, "
      "new color temperature is %u and new deltauv is %d\r\n",
      sl_btmesh_get_lightness_current(),
      lightbulb_state.temperature_current,
      lightbulb_state.deltauv_current);

  lightbulb_state_changed();
  ctl_update_and_publish(BTMESH_CTL_SERVER_MAIN, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light CTL request has completed.
 ******************************************************************************/
static void delayed_ctl_request(void)
{
  log("Starting delayed CTL request: lightness %u -> %u, color temperature %u -> %u, "
      "delta UV %d -> %d, %lu ms\r\n",
      sl_btmesh_get_lightness_current(),
      sl_btmesh_get_lightness_target(),
      lightbulb_state.temperature_current,
      lightbulb_state.temperature_target,
      lightbulb_state.deltauv_current,
      lightbulb_state.deltauv_target,
      delayed_ctl_trans
      );

  sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                               delayed_ctl_trans);
  sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                              lightbulb_state.deltauv_target,
                                              delayed_ctl_trans);

  if (delayed_ctl_trans == 0) {
    // no transition delay, update state immediately
    sl_btmesh_set_lightness_current(sl_btmesh_get_lightness_target());
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;
    lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

    lightbulb_state_changed();
    ctl_update_and_publish(BTMESH_CTL_SERVER_MAIN, delayed_ctl_trans);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&ctl_transition_complete_timer,
                                           delayed_ctl_trans,
                                           ctl_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start CTL Transition Complete timer\n");
  }
}

/** @} (end addtogroup LightCTL) */

/***************************************************************************//**
 * \defgroup LightCTLSetup
 * \brief Light CTL Setup Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightCTLSetup
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light CTL setup request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] kind           Type of state used in light CTL setup response.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_setup_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_ctl_default:
      current.ctl.lightness = sl_btmesh_get_lightness_default();
      current.ctl.temperature = lightbulb_state.temperature_default;
      current.ctl.deltauv = lightbulb_state.deltauv_default;
      break;
    case mesh_lighting_state_ctl_temperature_range:
      current.ctl_temperature_range.min = lightbulb_state.temperature_min;
      current.ctl_temperature_range.max = lightbulb_state.temperature_max;
      break;
    default:
      break;
  }

  return generic_server_respond(MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                NULL,
                                0,
                                0x00);
}

/***************************************************************************//**
 * Update light CTL setup state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] kind           Type of state used in light CTL setup update.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_setup_update(uint16_t element_index,
                                    mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_ctl_default:
      current.ctl.lightness = sl_btmesh_get_lightness_default();
      current.ctl.temperature = lightbulb_state.temperature_default;
      current.ctl.deltauv = lightbulb_state.deltauv_default;
      break;
    case mesh_lighting_state_ctl_temperature_range:
      current.ctl_temperature_range.min = lightbulb_state.temperature_min;
      current.ctl_temperature_range.max = lightbulb_state.temperature_max;
      break;
    default:
      break;
  }

  return generic_server_update(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               NULL,
                               0);
}

/***************************************************************************//**
 * This function process the requests for the light CTL setup model.
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
static void ctl_setup_request(uint16_t model_id,
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
    case mesh_lighting_request_ctl_default:
      kind = mesh_lighting_state_ctl_default;
      log("ctl_setup_request: state=ctl_default, default lightness=%u, "
          "default color temperature=%u, default delta UV=%d\r\n",
          request->ctl.lightness,
          request->ctl.temperature,
          request->ctl.deltauv);

      if ((sl_btmesh_get_lightness_default() == request->ctl.lightness)
          && (lightbulb_state.temperature_default == request->ctl.temperature)
          && (lightbulb_state.deltauv_default == request->ctl.deltauv)) {
        log("Request for current state received; no op\r\n");
      } else {
        if (sl_btmesh_get_lightness_default() != request->ctl.lightness) {
          log("Setting default lightness to <%u>\r\n", request->ctl.lightness);
          sl_btmesh_set_lightness_default(request->ctl.lightness);
        }
        if (lightbulb_state.temperature_default != request->ctl.temperature) {
          log("Setting default color temperature to <%u>\r\n",
              request->ctl.temperature);
          lightbulb_state.temperature_default = request->ctl.temperature;
        }
        if (lightbulb_state.deltauv_default != request->ctl.deltauv) {
          log("Setting default delta UV to <%d>\r\n", request->ctl.deltauv);
          lightbulb_state.deltauv_default = request->ctl.deltauv;
        }
        lightbulb_state_changed();
      }
      break;

    case mesh_lighting_request_ctl_temperature_range:
      kind = mesh_lighting_state_ctl_temperature_range;
      log("ctl_setup_request: state=ctl_temperature_range, "
          "min color temperature=%u, max color temperature=%u\r\n",
          request->ctl_temperature_range.min,
          request->ctl_temperature_range.max);

      if ((lightbulb_state.temperature_min
           == request->ctl_temperature_range.min)
          && (lightbulb_state.temperature_max
              == request->ctl_temperature_range.max)) {
        log("Request for current state received; no op\r\n");
      } else {
        if (lightbulb_state.temperature_min
            != request->ctl_temperature_range.min) {
          log("Setting min color temperature to <%u>\r\n",
              request->ctl_temperature_range.min);
          lightbulb_state.temperature_min = request->ctl_temperature_range.min;
        }
        if (lightbulb_state.temperature_max
            != request->ctl_temperature_range.max) {
          log("Setting max color temperature to <%u>\r\n",
              request->ctl_temperature_range.max);
          lightbulb_state.temperature_max = request->ctl_temperature_range.max;
        }
        lightbulb_state_changed();
      }
      break;

    default:
      break;
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_setup_response(element_index, client_addr, appkey_index, kind);
  } else {
    ctl_setup_update(element_index, kind);
  }
}

/***************************************************************************//**
 * This function is a handler for light CTL setup change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void ctl_setup_change(uint16_t model_id,
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
    case mesh_lighting_state_ctl_default:
      if (sl_btmesh_get_lightness_default() != current->ctl.lightness) {
        log("Default lightness update: from %u to %u\r\n",
            sl_btmesh_get_lightness_default(),
            current->ctl.lightness);
        sl_btmesh_set_lightness_default(current->ctl.lightness);
        lightbulb_state_changed();
      } else {
        log("Default lightness update -same value (%u)\r\n",
            sl_btmesh_get_lightness_default());
      }

      if (lightbulb_state.temperature_default != current->ctl.temperature) {
        log("Default color temperature change: from %u to %u\r\n",
            lightbulb_state.temperature_default,
            current->ctl.temperature);
        lightbulb_state.temperature_default = current->ctl.temperature;
        lightbulb_state_changed();
      } else {
        log("Default color temperature update -same value (%u)\r\n",
            lightbulb_state.temperature_default);
      }

      if (lightbulb_state.deltauv_default != current->ctl.deltauv) {
        log("Default delta UV change: from %d to %d\r\n",
            lightbulb_state.deltauv_default,
            current->ctl.deltauv);
        lightbulb_state.deltauv_default = current->ctl.deltauv;
        lightbulb_state_changed();
      } else {
        log("Default delta UV update -same value (%d)\r\n",
            lightbulb_state.deltauv_default);
      }

      break;

    case mesh_lighting_state_ctl_temperature_range:
      if (lightbulb_state.temperature_min
          != current->ctl_temperature_range.min) {
        log("Min color temperature update: from %u to %u\r\n",
            lightbulb_state.temperature_min,
            current->ctl_temperature_range.min);
        lightbulb_state.temperature_min = current->ctl_temperature_range.min;
        lightbulb_state_changed();
      } else {
        log("Min color temperature update -same value (%u)\r\n",
            lightbulb_state.temperature_min);
      }

      if (lightbulb_state.temperature_max
          != current->ctl_temperature_range.max) {
        log("Max color temperature update: from %u to %u\r\n",
            lightbulb_state.temperature_max,
            current->ctl_temperature_range.max);
        lightbulb_state.temperature_max = current->ctl_temperature_range.max;
        lightbulb_state_changed();
      } else {
        log("Max color temperature update -same value (%u)\r\n",
            lightbulb_state.temperature_max);
      }

      break;

    default:
      break;
  }
}

/** @} (end addtogroup LightCTLSetup) */

/***************************************************************************//**
 * \defgroup LightCTLTemperature
 * \brief Light CTL Temperature Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightCTLTemperature
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light CTL temperature request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_temperature_response(uint16_t element_index,
                                            uint16_t client_addr,
                                            uint16_t appkey_index,
                                            uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl_temperature;
  current.ctl_temperature.temperature = lightbulb_state.temperature_current;
  current.ctl_temperature.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl_temperature;
  target.ctl_temperature.temperature = lightbulb_state.temperature_target;
  target.ctl_temperature.deltauv = lightbulb_state.deltauv_target;

  return generic_server_respond(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                0x00);
}

/***************************************************************************//**
 * Update light CTL temperature state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_temperature_update(uint16_t element_index,
                                          uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl_temperature;
  current.ctl_temperature.temperature = lightbulb_state.temperature_current;
  current.ctl_temperature.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl_temperature;
  target.ctl_temperature.temperature = lightbulb_state.temperature_target;
  target.ctl_temperature.deltauv = lightbulb_state.deltauv_target;

  return generic_server_update(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light CTL temperature state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t ctl_temperature_update_and_publish(uint16_t element_index,
                                                      uint32_t remaining_ms)
{
  sl_status_t e;
  (void)element_index;

  e = ctl_temperature_update(BTMESH_CTL_SERVER_TEMPERATURE, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                               BTMESH_CTL_SERVER_TEMPERATURE,
                               mesh_lighting_state_ctl_temperature);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light CTL temperature model.
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
static void ctl_temperature_request(uint16_t model_id,
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
  (void)element_index;
  (void)server_addr;

  log("ctl_temperature_request: color temperature=%u, delta UV=%d, "
      "transition=%lu, delay=%u\r\n",
      request->ctl_temperature.temperature,
      request->ctl_temperature.deltauv,
      transition_ms, delay_ms);

  if ((lightbulb_state.temperature_current
       == request->ctl_temperature.temperature)
      && (lightbulb_state.deltauv_current
          == request->ctl_temperature.deltauv)) {
    log("Request for current state received; no op\r\n");
  } else {
    if (lightbulb_state.temperature_current
        != request->ctl_temperature.temperature) {
      log("Setting color temperature to <%u>\r\n",
          request->ctl_temperature.temperature);
    }
    if (lightbulb_state.deltauv_current != request->ctl_temperature.deltauv) {
      log("Setting delta UV to <%d>\r\n",
          request->ctl_temperature.deltauv);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.temperature_current = request->ctl_temperature.temperature;
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_current = request->ctl_temperature.deltauv;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;

      // update LED color temperature
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                  lightbulb_state.deltauv_current,
                                                  IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the temperature change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;
      sl_status_t sc = sl_simple_timer_start(&ctl_delayed_ctl_temperature_timer,
                                             delay_ms,
                                             ctl_delayed_ctl_temperature_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed Temperature timer\n");
      // store transition parameter for later use
      delayed_ctl_temperature_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;

      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                  lightbulb_state.deltauv_target,
                                                  transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&ctl_temp_transition_timer,
                                             transition_ms,
                                             ctl_temp_transition_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Temp Transition timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(BTMESH_CTL_SERVER_TEMPERATURE);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_temperature_response(BTMESH_CTL_SERVER_TEMPERATURE,
                             client_addr,
                             appkey_index,
                             remaining_ms);
  }
  ctl_temperature_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                         BTMESH_LIGHTING_SERVER_MAIN,
                         mesh_lighting_state_ctl);
  generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                         BTMESH_CTL_SERVER_TEMPERATURE,
                         mesh_generic_state_level);
}

/***************************************************************************//**
 * This function is a handler for light CTL temperature change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void ctl_temperature_change(uint16_t model_id,
                                   uint16_t element_index,
                                   const struct mesh_generic_state *current,
                                   const struct mesh_generic_state *target,
                                   uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.temperature_current
      != current->ctl_temperature.temperature) {
    log("Color temperature update: from %u to %u\r\n",
        lightbulb_state.temperature_current,
        current->ctl_temperature.temperature);
    lightbulb_state.temperature_current = current->ctl_temperature.temperature;
    lightbulb_state_changed();
  } else {
    log("Color temperature update -same value (%u)\r\n",
        lightbulb_state.temperature_current);
  }

  if (lightbulb_state.deltauv_current != current->ctl_temperature.deltauv) {
    log("Delta UV update: from %d to %d\r\n",
        lightbulb_state.deltauv_current,
        current->ctl_temperature.deltauv);
    lightbulb_state.deltauv_current = current->ctl_temperature.deltauv;
    lightbulb_state_changed();
  } else {
    log("Delta UV update -same value (%d)\r\n", lightbulb_state.deltauv_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light CTL temperature recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void ctl_temperature_recall(uint16_t model_id,
                                   uint16_t element_index,
                                   const struct mesh_generic_state *current,
                                   const struct mesh_generic_state *target,
                                   uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("CTL color temperature recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.temperature_target = current->ctl_temperature.temperature;
    lightbulb_state.deltauv_target = current->ctl_temperature.deltauv;
  } else {
    lightbulb_state.temperature_target = target->ctl_temperature.temperature;
    lightbulb_state.deltauv_target = target->ctl_temperature.deltauv;
  }

  if ((lightbulb_state.temperature_current
       == lightbulb_state.temperature_target)
      && (lightbulb_state.deltauv_current
          == lightbulb_state.deltauv_target)) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall CTL color temperature to %u, delta UV to %d with "
        "transition=%lu ms\r\n",
        lightbulb_state.temperature_target,
        lightbulb_state.deltauv_target,
        transition_ms);
    sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                lightbulb_state.deltauv_target,
                                                transition_ms);

    if (transition_ms == IMMEDIATE) {
      lightbulb_state.temperature_current = current->ctl_temperature.temperature;
      lightbulb_state.deltauv_current = current->ctl_temperature.deltauv;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&ctl_temp_transition_timer,
                                             transition_ms,
                                             ctl_temp_transition_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Temp Transition timer\n");
    }
    lightbulb_state_changed();
  }

  ctl_temperature_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, transition_ms);
}

/***************************************************************************//**
 * This function is called when a light CTL temperature request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void ctl_temperature_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;
  lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

  log("Transition complete. New color temperature is %u "
      "and new delta UV is %d\r\n",
      lightbulb_state.temperature_current,
      lightbulb_state.deltauv_current);

  lightbulb_state_changed();
  ctl_temperature_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light CTL temperature request
 * has completed.
 ******************************************************************************/
static void delayed_ctl_temperature_request(void)
{
  log("Starting delayed CTL color temperature request: "
      "color temperature %u -> %u, delta UV %d -> %d, %lu ms\r\n",
      lightbulb_state.temperature_current,
      lightbulb_state.temperature_target,
      lightbulb_state.deltauv_current,
      lightbulb_state.deltauv_target,
      delayed_ctl_temperature_trans
      );

  sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                              lightbulb_state.deltauv_target,
                                              delayed_ctl_temperature_trans);

  if (delayed_ctl_temperature_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;
    lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

    lightbulb_state_changed();
    ctl_temperature_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE,
                                       delayed_ctl_temperature_trans);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&ctl_temp_transition_timer,
                                           delayed_ctl_temperature_trans,
                                           ctl_temp_transition_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start Temp Transition timer\n");
  }
}

/** @} (end addtogroup LightCTLTemperature) */

/***************************************************************************//**
 * \defgroup SecGenericLevel
 * \brief Generic Level Server model on secondary element.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SecGenericLevel
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Convert level to temperature.
 *
 * @param[in] level  Level to convert.
 *
 * @return Temperature converted from level.
 ******************************************************************************/
static uint16_t level_to_temperature(int16_t level)
{
  return lightbulb_state.temperature_min
         + (uint32_t)(level + (int32_t)32768)
         * (lightbulb_state.temperature_max - lightbulb_state.temperature_min)
         / 65535;
}

/***************************************************************************//**
 * Convert temperature to level.
 *
 * @param[in] temperature  Temperature to convert.
 *
 * @return Level converted from temperature.
 ******************************************************************************/
static int16_t temperature_to_level(uint16_t temperature)
{
  return (temperature - lightbulb_state.temperature_min)
         * (uint32_t)65535
         / (lightbulb_state.temperature_max - lightbulb_state.temperature_min)
         - (int32_t)32768;
}

/***************************************************************************//**
 * Response to generic level request on secondary element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t sec_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.sec_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.sec_level_target;

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
 * Update generic level state on secondary element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t sec_level_update(uint16_t element_index,
                                    uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.sec_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.sec_level_target;

  return generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update generic level state on secondary element
 * and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t sec_level_update_and_publish(uint16_t element_index,
                                                uint32_t remaining_ms)
{
  sl_status_t e;

  e = sec_level_update(element_index, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_level);
  }

  return e;
}

/***************************************************************************//**
 * Schedule next generic level move request on secondary element.
 *
 * @param[in] remaining_delta   The remaining level delta to the target state.
 ******************************************************************************/
static void sec_level_move_schedule_next_request(int32_t remaining_delta)
{
  uint32_t transition_ms = 0;
  if (abs(remaining_delta) < abs(move_sec_level_delta)) {
    transition_ms = (uint32_t)(((int64_t)move_sec_level_trans * remaining_delta)
                               / move_sec_level_delta);
    sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                lightbulb_state.deltauv_current,
                                                transition_ms);
  } else {
    int16_t next_level = lightbulb_state.sec_level_current
                         + move_sec_level_delta;
    transition_ms = move_sec_level_trans;
    sl_btmesh_ctl_set_temperature_deltauv_level(level_to_temperature(next_level),
                                                lightbulb_state.deltauv_current,
                                                move_sec_level_trans);
  }
  sl_status_t sc = sl_simple_timer_start(&ctl_sec_level_move_timer,
                                         transition_ms,
                                         ctl_sec_level_move_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start Sec Level timer\n");
}

/***************************************************************************//**
 * Handle generic level move request on secondary element.
 ******************************************************************************/
static void sec_level_move_request(void)
{
  log("Secondary level move: level %d -> %d, delta %d in %lu ms\r\n",
      lightbulb_state.sec_level_current,
      lightbulb_state.sec_level_target,
      move_sec_level_delta,
      move_sec_level_trans);

  int32_t remaining_delta = (int32_t)lightbulb_state.sec_level_target
                            - lightbulb_state.sec_level_current;

  if (abs(remaining_delta) < abs(move_sec_level_delta)) {
    // end of move level as it reached target state
    lightbulb_state.sec_level_current = lightbulb_state.sec_level_target;
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;
  } else {
    lightbulb_state.sec_level_current += move_sec_level_delta;
    uint16_t temperature = level_to_temperature(lightbulb_state.sec_level_current);
    lightbulb_state.temperature_current = temperature;
  }
  lightbulb_state_changed();
  sec_level_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE,
                               UNKNOWN_REMAINING_TIME);

  remaining_delta = (int32_t)lightbulb_state.sec_level_target
                    - lightbulb_state.sec_level_current;
  if (remaining_delta != 0) {
    sec_level_move_schedule_next_request(remaining_delta);
  }
}

/***************************************************************************//**
 * Stop generic level move on secondary element.
 ******************************************************************************/
static void sec_level_move_stop(void)
{
  // Cancel timers
  sl_status_t sc = sl_simple_timer_stop(&ctl_delayed_sec_level_timer);
  app_assert_status_f(sc, "Failed to stop Delayed Sec Level timer\n");
  sc = sl_simple_timer_stop(&ctl_sec_level_move_timer);
  app_assert_status_f(sc, "Failed to stop Sec Level Move timer\n");
  //Reset move parameters
  move_sec_level_delta = 0;
  move_sec_level_trans = 0;
}

/***************************************************************************//**
 * This function process the requests for the generic level model
 * on secondary element.
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
static void sec_level_request(uint16_t model_id,
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

  uint16_t temperature;
  uint32_t remaining_ms = UNKNOWN_REMAINING_TIME;

  switch (request->kind) {
    case mesh_generic_request_level:
      log("sec_level_request (generic): level=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);

      sec_level_move_stop();
      if (lightbulb_state.sec_level_current == request->level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.sec_level_target = request->level;
      } else {
        log("Setting secondary level to <%d>\r\n", request->level);

        temperature = level_to_temperature(request->level);

        if (transition_ms == 0 && delay_ms == 0) { // Immediate change
          lightbulb_state.sec_level_current = request->level;
          lightbulb_state.sec_level_target = request->level;
          lightbulb_state.temperature_current = temperature;
          lightbulb_state.temperature_target = temperature;

          // update LED Temperature
          sl_btmesh_ctl_set_temperature_deltauv_level(temperature,
                                                      lightbulb_state.deltauv_current,
                                                      IMMEDIATE);
        } else if (delay_ms > 0) {
          // a delay has been specified for the change. Start a soft timer
          // that will trigger the change after the given delay
          // Current state remains as is for now
          lightbulb_state.sec_level_target = request->level;
          lightbulb_state.temperature_target = temperature;
          sec_level_request_kind = mesh_generic_request_level;
          sl_status_t sc = sl_simple_timer_start(&ctl_delayed_sec_level_timer,
                                                 delay_ms,
                                                 ctl_delayed_sec_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Sec Level timer\n");
          // store transition parameter for later use
          delayed_sec_level_trans = transition_ms;
        } else {
          // no delay but transition time has been set.
          lightbulb_state.sec_level_target = request->level;
          lightbulb_state.temperature_target = temperature;
          sl_btmesh_ctl_set_temperature_deltauv_level(temperature,
                                                      lightbulb_state.deltauv_current,
                                                      transition_ms);

          // lightbulb current state will be updated when transition is complete
          sl_status_t sc = sl_simple_timer_start(&ctl_sec_level_transition_timer,
                                                 delayed_sec_level_trans,
                                                 ctl_sec_level_transition_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Sec Level Transition timer\n");
        }

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }

      remaining_ms = delay_ms + transition_ms;
      break;

    case mesh_generic_request_level_move: {
      log("sec_level_request (move): delta=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);
      // Store move parameters
      move_sec_level_delta = request->level;
      move_sec_level_trans = transition_ms;

      int16_t requested_level = 0;
      if (move_sec_level_delta > 0) {
        requested_level = 0x7FFF; // Max level value
      } else if (move_sec_level_delta < 0) {
        requested_level = 0x8000; // Min level value
      }

      if (lightbulb_state.sec_level_current == requested_level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.sec_level_target = requested_level;
        remaining_ms = IMMEDIATE;
      } else {
        log("Setting secondary level to <%d>\r\n", requested_level);

        temperature = level_to_temperature(requested_level);

        if (delay_ms > 0) {
          // a delay has been specified for the move. Start a soft timer
          // that will trigger the move after the given delay
          // Current state remains as is for now
          lightbulb_state.sec_level_target = requested_level;
          lightbulb_state.temperature_target = temperature;
          sec_level_request_kind = mesh_generic_request_level_move;
          sl_status_t sc = sl_simple_timer_start(&ctl_delayed_sec_level_timer,
                                                 delay_ms,
                                                 ctl_delayed_sec_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Sec Level timer\n");
        } else {
          // no delay so start move
          lightbulb_state.sec_level_target = requested_level;
          lightbulb_state.temperature_target = temperature;

          int32_t remaining_delta = (int32_t)lightbulb_state.sec_level_target
                                    - lightbulb_state.sec_level_current;
          sec_level_move_schedule_next_request(remaining_delta);
        }
        remaining_ms = UNKNOWN_REMAINING_TIME;

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }
      break;
    }

    case mesh_generic_request_level_halt:
      log("sec_level_request (halt)\r\n");

      // Set current state
      lightbulb_state.temperature_current = sl_btmesh_get_temperature();
      lightbulb_state.temperature_target = lightbulb_state.temperature_current;
      lightbulb_state.sec_level_current = temperature_to_level(lightbulb_state.temperature_current);
      lightbulb_state.sec_level_target = lightbulb_state.sec_level_current;
      if (delay_ms > 0) {
        // a delay has been specified for the move halt. Start a soft timer
        // that will trigger the move halt after the given delay
        // Current state remains as is for now
        remaining_ms = delay_ms;
        sec_level_request_kind = mesh_generic_request_level_halt;
        sl_status_t sc = sl_simple_timer_start(&ctl_delayed_sec_level_timer,
                                               delay_ms,
                                               ctl_delayed_sec_level_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Delayed Sec Level timer\n");
      } else {
        sec_level_move_stop();
        sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                    lightbulb_state.deltauv_current,
                                                    IMMEDIATE);
        remaining_ms = IMMEDIATE;
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    sec_level_response(element_index, client_addr, appkey_index, remaining_ms);
  }
  sec_level_update_and_publish(element_index, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_ctl_temperature);
}

/***************************************************************************//**
 * This function is a handler for generic level change event
 * on secondary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void sec_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.sec_level_current != current->level.level) {
    log("Secondary level update: from %d to %d\r\n",
        lightbulb_state.sec_level_current,
        current->level.level);
    lightbulb_state.sec_level_current = current->level.level;
    lightbulb_state_changed();
    sec_level_move_stop();
  } else {
    log("Secondary level update -same value (%d)\r\n",
        lightbulb_state.sec_level_current);
  }
}

/***************************************************************************//**
 * This function is a handler for generic level recall event
 * on secondary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void sec_level_recall(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("Secondary Generic Level recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.sec_level_target = current->level.level;
  } else {
    lightbulb_state.sec_level_target = target->level.level;
  }

  if (lightbulb_state.sec_level_current == lightbulb_state.sec_level_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall secondary level to %d with transition=%lu ms\r\n",
        lightbulb_state.sec_level_target,
        transition_ms);
    if (transition_ms == IMMEDIATE) {
      lightbulb_state.sec_level_current = current->level.level;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&ctl_sec_level_transition_timer,
                                             transition_ms,
                                             ctl_sec_level_transition_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Sec Level Transition timer\n");
    }
    lightbulb_state_changed();
  }
}

/***************************************************************************//**
 * This function is called when a generic level request on secondary element
 * with non-zero transition time has completed.
 ******************************************************************************/
static void sec_level_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.sec_level_current = lightbulb_state.sec_level_target;
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;

  log("Transition complete. New secondary level is %d\r\n",
      lightbulb_state.sec_level_current);

  lightbulb_state_changed();
  sec_level_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for generic level request
 * on secondary element has completed.
 ******************************************************************************/
static void delayed_sec_level_request(void)
{
  log("Starting delayed secondary level request: level %d -> %d, %lu ms\r\n",
      lightbulb_state.sec_level_current,
      lightbulb_state.sec_level_target,
      delayed_sec_level_trans);

  switch (sec_level_request_kind) {
    case mesh_generic_request_level:
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                  lightbulb_state.deltauv_current,
                                                  delayed_sec_level_trans);

      if (delayed_sec_level_trans == 0) {
        // no transition delay, update state immediately
        lightbulb_state.sec_level_current = lightbulb_state.sec_level_target;
        lightbulb_state.temperature_current = lightbulb_state.temperature_target;

        lightbulb_state_changed();
        sec_level_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE,
                                     delayed_sec_level_trans);
      } else {
        // state is updated when transition is complete
        sl_status_t sc = sl_simple_timer_start(&ctl_sec_level_transition_timer,
                                               delayed_sec_level_trans,
                                               ctl_sec_level_transition_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Sec Level Transition timer\n");
      }
      break;

    case mesh_generic_request_level_move:
      sec_level_move_schedule_next_request((int32_t)lightbulb_state.sec_level_target
                                           - lightbulb_state.sec_level_current);
      sec_level_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE,
                                   UNKNOWN_REMAINING_TIME);
      break;

    case mesh_generic_request_level_halt:
      // Set current state
      lightbulb_state.temperature_current = sl_btmesh_get_temperature();
      lightbulb_state.temperature_target = lightbulb_state.temperature_current;
      lightbulb_state.sec_level_current = temperature_to_level(lightbulb_state.temperature_current);
      lightbulb_state.sec_level_target = lightbulb_state.sec_level_current;
      sec_level_move_stop();
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                  lightbulb_state.deltauv_current,
                                                  IMMEDIATE);
      sec_level_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, IMMEDIATE);
      break;

    default:
      break;
  }
}

/** @} (end addtogroup SecGenericLevel) */

/***************************************************************************//**
 * Initialization of the models supported by this node.
 * This function registers callbacks for each of the supported models.
 ******************************************************************************/
static void init_ctl_models(void)
{
  generic_server_register_handler(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                  BTMESH_CTL_SERVER_MAIN,
                                  ctl_request,
                                  ctl_change,
                                  ctl_recall);

  generic_server_register_handler(MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID,
                                  BTMESH_CTL_SERVER_MAIN,
                                  ctl_setup_request,
                                  ctl_setup_change,
                                  NULL);

  generic_server_register_handler(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                  BTMESH_CTL_SERVER_TEMPERATURE,
                                  ctl_temperature_request,
                                  ctl_temperature_change,
                                  ctl_temperature_recall);

  generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                  BTMESH_CTL_SERVER_TEMPERATURE,
                                  sec_level_request,
                                  sec_level_change,
                                  sec_level_recall);
}

/***************************************************************************//**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state.
 * If PS key with ID SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL does not exist or loading failed,
 * lightbulb_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_load(void)
{
  sl_status_t sc;
  size_t ps_len = 0;
  struct lightbulb_state ps_data;

  sc = sl_bt_nvm_load(SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL,
                      sizeof(ps_data),
                      &ps_len,
                      (uint8_t *)&ps_data);

  // Set default values if ps_load fail or size of lightbulb_state has changed
  if ((sc != SL_STATUS_OK) || (ps_len != sizeof(struct lightbulb_state))) {
    memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
    lightbulb_state.temperature_default = SL_BTMESH_CTL_SERVER_DEFAULT_TEMPERATURE_CFG_VAL;
    lightbulb_state.temperature_min = SL_BTMESH_CTL_SERVER_MINIMUM_TEMPERATURE_CFG_VAL;
    lightbulb_state.temperature_max = SL_BTMESH_CTL_SERVER_MAXIMUM_TEMPERATURE_CFG_VAL;
    lightbulb_state.deltauv_default = SL_BTMESH_CTL_SERVER_DEFAULT_DELTAUV_CFG_VAL;

    // Check if default values are valid and correct them if needed
    lightbulb_state_validate_and_correct();

    if (sc == SL_STATUS_OK) {
      // The sl_bt_nvm_load call was successful but the size of the loaded data
      // differs from the expected size therefore error code shall be set
      sc = SL_STATUS_INVALID_STATE;
      log("CTL server lightbulb state loaded from PS with invalid size, "
          "use defaults. (expected=%zd,actual=%zd)\r\n",
          sizeof(struct lightbulb_state),
          ps_len);
    } else {
      log_status_f(sc,
                   "CTL server lightbulb state load from PS failed "
                   "or nvm is empty, use defaults.\r\n");
    }
  } else {
    memcpy(&lightbulb_state, &ps_data, ps_len);
  }

  return sc;
}

/***************************************************************************//**
 * This function saves the current light state in Persistent Storage so that
 * the data is preserved over reboots and power cycles.
 * The light state is hold in a global variable lightbulb_state.
 * A PS key with ID SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL is used to store the whole struct.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_store(void)
{
  sl_status_t sc;

  sc = sl_bt_nvm_save(SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL,
                      sizeof(struct lightbulb_state),
                      (const uint8_t *)&lightbulb_state);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "CTL server lightbulb state store in PS failed.\r\n");

  return sc;
}

/***************************************************************************//**
 * This function is called each time the lightbulb state in RAM is changed.
 * It sets up a soft timer that will save the state in flash after small delay.
 * The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lightbulb_state_changed(void)
{
  sl_status_t sc = sl_simple_timer_start(&ctl_state_store_timer,
                                         SL_BTMESH_CTL_SERVER_NVM_SAVE_TIME_CFG_VAL,
                                         ctl_state_store_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start State Store timer\n");
}

/***************************************************************************//**
 * This function validates the lighbulb_state and change it if it is against
 * the specification.
 ******************************************************************************/
static void lightbulb_state_validate_and_correct(void)
{
  if (lightbulb_state.temperature_min < SL_BTMESH_CTL_SERVER_MINIMUM_TEMPERATURE_CFG_VAL) {
    lightbulb_state.temperature_min = SL_BTMESH_CTL_SERVER_MINIMUM_TEMPERATURE_CFG_VAL;
  }
  if (lightbulb_state.temperature_min > SL_BTMESH_CTL_SERVER_MAXIMUM_TEMPERATURE_CFG_VAL) {
    lightbulb_state.temperature_min = SL_BTMESH_CTL_SERVER_MAXIMUM_TEMPERATURE_CFG_VAL;
  }
  if (lightbulb_state.temperature_min > lightbulb_state.temperature_max) {
    lightbulb_state.temperature_min = lightbulb_state.temperature_max;
  }
  if (lightbulb_state.temperature_default < lightbulb_state.temperature_min) {
    lightbulb_state.temperature_default = lightbulb_state.temperature_min;
  }
  if (lightbulb_state.temperature_default > lightbulb_state.temperature_max) {
    lightbulb_state.temperature_default = lightbulb_state.temperature_max;
  }
  if (lightbulb_state.temperature_current < lightbulb_state.temperature_min) {
    lightbulb_state.temperature_current = lightbulb_state.temperature_min;
  }
  if (lightbulb_state.temperature_current > lightbulb_state.temperature_max) {
    lightbulb_state.temperature_current = lightbulb_state.temperature_max;
  }
  if (lightbulb_state.temperature_target < lightbulb_state.temperature_min) {
    lightbulb_state.temperature_target = lightbulb_state.temperature_min;
  }
  if (lightbulb_state.temperature_target > lightbulb_state.temperature_max) {
    lightbulb_state.temperature_target = lightbulb_state.temperature_max;
  }
}

/*******************************************************************************
 * Lightbulb state initialization.
 * This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 ******************************************************************************/
void sl_btmesh_ctl_server_init(void)
{
  memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));

  lightbulb_state_load();

  // Handle on power up behavior
  uint32_t transition_ms = sl_btmesh_get_default_transition_time();
  switch (sl_btmesh_get_lightness_onpowerup()) {
    case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
      lightbulb_state.temperature_current = lightbulb_state.temperature_default;
      lightbulb_state.temperature_target = lightbulb_state.temperature_default;
      lightbulb_state.deltauv_current = lightbulb_state.deltauv_default;
      lightbulb_state.deltauv_target = lightbulb_state.deltauv_default;
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_default,
                                                  lightbulb_state.deltauv_default,
                                                  IMMEDIATE);
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_ON:
      lightbulb_state.temperature_current = lightbulb_state.temperature_default;
      lightbulb_state.temperature_target = lightbulb_state.temperature_default;
      lightbulb_state.deltauv_current = lightbulb_state.deltauv_default;
      lightbulb_state.deltauv_target = lightbulb_state.deltauv_default;
      sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_default,
                                                  lightbulb_state.deltauv_default,
                                                  IMMEDIATE);
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
      if (transition_ms > 0
          && ((lightbulb_state.temperature_target
               != lightbulb_state.temperature_default)
              || (lightbulb_state.deltauv_target
                  != lightbulb_state.deltauv_default))) {
        lightbulb_state.temperature_current = lightbulb_state.temperature_default;
        lightbulb_state.deltauv_current = lightbulb_state.deltauv_default;
        sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                    lightbulb_state.deltauv_current,
                                                    IMMEDIATE);
        sl_status_t sc = sl_simple_timer_start(&ctl_temp_transition_timer,
                                               transition_ms,
                                               ctl_temp_transition_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Temp Transition timer\n");
        sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_target,
                                                    lightbulb_state.deltauv_target,
                                                    transition_ms);
      } else {
        lightbulb_state.temperature_current = lightbulb_state.temperature_target;
        lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;
        sl_btmesh_ctl_set_temperature_deltauv_level(lightbulb_state.temperature_current,
                                                    lightbulb_state.deltauv_current,
                                                    IMMEDIATE);
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();
  init_ctl_models();
  ctl_setup_update(BTMESH_CTL_SERVER_MAIN, mesh_lighting_state_ctl_default);
  ctl_setup_update(BTMESH_CTL_SERVER_MAIN,
                   mesh_lighting_state_ctl_temperature_range);
  ctl_temperature_update_and_publish(BTMESH_CTL_SERVER_TEMPERATURE, IMMEDIATE);
}

/*******************************************************************************
 * Handle CTL Server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt  Pointer to incoming event.
 ******************************************************************************/
void sl_btmesh_ctl_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_ctl_server_init();
      break;

    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_ctl_server_init();
      }
      break;

    case sl_btmesh_evt_node_reset_id:
      sl_bt_nvm_erase(SL_BTMESH_CTL_SERVER_PS_KEY_CFG_VAL);
      break;
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
                     "CTL server respond failed"
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
                     "CTL server state update failed "
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
                      "CTL server state publish failed "
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
  sl_status_t sc = mesh_lib_generic_server_register_handler(model_id,
                                                            elem_index,
                                                            cb,
                                                            ch,
                                                            recall);

  app_assert_status_f(sc,
                      "CTL server failed to register handlers "
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
  app_assert_status_f(sc, "CTL server failed to reset scene register.\n");
}
#endif

/** @} (end addtogroup BtmeshWrappers) */

/**************************************************************************//**
 * Timer Callbacks
 *****************************************************************************/
static void ctl_sec_level_move_timer_cb(sl_simple_timer_t *handle,
                                        void *data)
{
  (void)data;
  (void)handle;
  // handling of generic level move, update the lightbulb state
  sec_level_move_request();
}

static void ctl_sec_level_transition_timer_cb(sl_simple_timer_t *handle,
                                              void *data)
{
  (void)data;
  (void)handle;
  // transition for a secondary generic level request has completed,
  //update the lightbulb state
  sec_level_transition_complete();
}

static void ctl_temp_transition_timer_cb(sl_simple_timer_t *handle,
                                         void *data)
{
  (void)data;
  (void)handle;
  // transition for a ctl temperature request has completed,
  // update the lightbulb state
  ctl_temperature_transition_complete();
}

static void ctl_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                             void *data)
{
  (void)data;
  (void)handle;
  // transition for a ctl request has completed, update the lightbulb state
  ctl_transition_complete();
}

static void ctl_delayed_sec_level_timer_cb(sl_simple_timer_t *handle,
                                           void *data)
{
  (void)data;
  (void)handle;
  // delay for a secondary generic level request has passed,
  // now process the request
  delayed_sec_level_request();
}

static void ctl_delayed_ctl_temperature_timer_cb(sl_simple_timer_t *handle,
                                                 void *data)
{
  (void)data;
  (void)handle;
  // delay for a ctl temperature request has passed, now process the request
  delayed_ctl_temperature_request();
}

static void ctl_delayed_ctl_request_timer_cb(sl_simple_timer_t *handle,
                                             void *data)
{
  (void)data;
  (void)handle;
  // delay for a ctl request has passed, now process the request
  delayed_ctl_request();
}

static void ctl_state_store_timer_cb(sl_simple_timer_t *handle,
                                     void *data)
{
  (void)data;
  (void)handle;
  // save the lightbulb state
  lightbulb_state_store();
}

/** @} (end addtogroup CTL_SERVER) */
