/***************************************************************************//**
 * @file  sl_btmesh_hsl_server.c
 * @brief Bt Mesh HSL Server module
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

#include "sl_btmesh_hsl_server_config.h"
#include "sl_btmesh_hsl_server.h"
#include "sl_btmesh_hsl_signal_transition_handler.h"
#include "sl_btmesh_lighting_server.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup HSL_Server
 * @{
 ******************************************************************************/

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
#define scene_server_reset_register(elem_index) \
  scene_server_reset_register_impl(elem_index)
#else
#define scene_server_reset_register(elem_index)
#endif

#define NO_FLAGS                0   ///< No flags used for message
#define IMMEDIATE               0   ///< Immediate transition time is 0 seconds
#define NO_CALLBACK_DATA        (void *)NULL   ///< Callback has no parameters
#define HIGH_PRIORITY           0              ///< High Priority
/// Values greater than max 37200000 are treated as unknown remaining time
#define UNKNOWN_REMAINING_TIME  40000000

/// Lightbulb state
static PACKSTRUCT(struct lightbulb_state {
  // Hue state
  uint16_t hue_current;            ///< Current hue value
  uint16_t hue_target;             ///< Target hue value
  uint16_t hue_default;            ///< Default hue value
  uint16_t hue_min;                ///< Minimum hue value
  uint16_t hue_max;                ///< Maximum hue value

  // Generic Level bound to hue
  int16_t hue_level_current;       ///< Current hue generic level value
  int16_t hue_level_target;        ///< Target hue generic level value

  // Saturation state
  uint16_t saturation_current;     ///< Current saturation value
  uint16_t saturation_target;      ///< Target saturation value
  uint16_t saturation_default;     ///< Default saturation value
  uint16_t saturation_min;         ///< Minimum saturation value
  uint16_t saturation_max;         ///< Maximum saturation value

  // Generic Level bound to saturation
  int16_t saturation_level_current;       ///< Current saturation generic level value
  int16_t saturation_level_target;        ///< Target saturation generic level value
}) lightbulb_state;

static sl_status_t hsl_hue_update(uint16_t element_index,
                                  uint32_t remaining_ms);
static sl_status_t hsl_saturation_update(uint16_t element_index,
                                         uint32_t remaining_ms);

/// copy of transition delay parameter, needed for delayed hsl request
static uint32_t delayed_hsl_trans = 0;
/// copy of transition delay parameter, needed for delayed hue request
static uint32_t delayed_hsl_hue_trans = 0;
/// copy of transition delay parameter, needed for
///delayed hue generic level request
static uint32_t delayed_hue_level_trans = 0;
/// copy of generic request kind, needed for delayed hue generic request
static mesh_generic_request_t hue_level_request_kind = mesh_generic_request_level;
/// copy of move transition parameter for hue generic request
static uint32_t move_hue_level_trans = 0;
/// copy of move delta parameter for hue generic request
static int16_t move_hue_level_delta = 0;
/// copy of transition delay parameter, needed for delayed saturation request
static uint32_t delayed_hsl_saturation_trans = 0;
/// copy of transition delay parameter, needed for
/// delayed saturation generic level request
static uint32_t delayed_saturation_level_trans = 0;
/// copy of generic request kind, needed for delayed saturation generic request
static mesh_generic_request_t saturation_level_request_kind = mesh_generic_request_level;
/// copy of move transition parameter for saturation generic request
static uint32_t move_saturation_level_trans = 0;
/// copy of move delta parameter for saturation generic request
static int16_t move_saturation_level_delta = 0;

// Timer handles
static sl_simple_timer_t hsl_transition_complete_timer;
static sl_simple_timer_t hsl_delayed_hsl_request_timer;
static sl_simple_timer_t hsl_hue_transition_complete_timer;
static sl_simple_timer_t hsl_delayed_hsl_hue_timer;
static sl_simple_timer_t hsl_hue_level_move_timer;
static sl_simple_timer_t hsl_hue_level_transition_complete_timer;
static sl_simple_timer_t hsl_delayed_hue_level_timer;
static sl_simple_timer_t hsl_saturation_transition_complete_timer;
static sl_simple_timer_t hsl_delayed_hsl_saturation_timer;
static sl_simple_timer_t hsl_saturation_level_move_timer;
static sl_simple_timer_t hsl_saturation_level_transition_complete_timer;
static sl_simple_timer_t hsl_delayed_saturation_level_timer;
static sl_simple_timer_t hsl_state_store_timer;

// Timer callbacks
static void hsl_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                             void *data);
static void hsl_delayed_hsl_request_timer_cb(sl_simple_timer_t *handle,
                                             void *data);
static void hsl_hue_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                 void *data);
static void hsl_delayed_hsl_hue_timer_cb(sl_simple_timer_t *handle,
                                         void *data);
static void hsl_hue_level_move_timer_cb(sl_simple_timer_t *handle,
                                        void *data);
static void hsl_hue_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                       void *data);
static void hsl_delayed_hue_level_timer_cb(sl_simple_timer_t *handle,
                                           void *data);
static void hsl_saturation_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data);
static void hsl_delayed_hsl_saturation_timer_cb(sl_simple_timer_t *handle,
                                                void *data);
static void hsl_saturation_level_move_timer_cb(sl_simple_timer_t *handle,
                                               void *data);
static void hsl_saturation_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                              void *data);
static void hsl_delayed_saturation_level_timer_cb(sl_simple_timer_t *handle,
                                                  void *data);
static void hsl_state_store_timer_cb(sl_simple_timer_t *handle,
                                     void *data);

/***************************************************************************//**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state.
 * If PS key with ID SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL does not exist or loading failed,
 * lightbulb_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_load(void);

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
 * \defgroup LightHSL
 * \brief Light HSL Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightHSL
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light HSL request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_response(uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t appkey_index,
                                uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl;
  current.hsl.lightness = sl_btmesh_get_lightness_current();
  current.hsl.hue = lightbulb_state.hue_current;
  current.hsl.saturation = lightbulb_state.saturation_current;

  target.kind = mesh_lighting_state_hsl;
  target.hsl.lightness = sl_btmesh_get_lightness_target();
  target.hsl.hue = lightbulb_state.hue_target;
  target.hsl.saturation = lightbulb_state.saturation_target;

  return generic_server_respond(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update light HSL state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_update(uint16_t element_index, uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl;
  current.hsl.lightness = sl_btmesh_get_lightness_current();
  current.hsl.hue = lightbulb_state.hue_current;
  current.hsl.saturation = lightbulb_state.saturation_current;

  target.kind = mesh_lighting_state_hsl;
  target.hsl.lightness = sl_btmesh_get_lightness_target();
  target.hsl.hue = lightbulb_state.hue_target;
  target.hsl.saturation = lightbulb_state.saturation_target;

  return generic_server_update(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light HSL state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_update_and_publish(uint16_t element_index,
                                          uint32_t remaining_ms)
{
  sl_status_t e;
  (void)element_index;

  e = hsl_update(BTMESH_HSL_SERVER_MAIN, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                               BTMESH_HSL_SERVER_MAIN,
                               mesh_lighting_state_hsl);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light HSL model.
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
static void hsl_request(uint16_t model_id,
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

  log("hsl_request: lightness=%u, hue=%u, saturation=%u, "
      "transition=%lu, delay=%u\r\n",
      request->hsl.lightness,
      request->hsl.hue,
      request->hsl.saturation,
      transition_ms,
      delay_ms);

  if ((sl_btmesh_get_lightness_current() == request->hsl.lightness)
      && (lightbulb_state.hue_current == request->hsl.hue)
      && (lightbulb_state.saturation_current == request->hsl.saturation)) {
    log("Request for current state received; no op\r\n");
  } else {
    if (sl_btmesh_get_lightness_current() != request->hsl.lightness) {
      log("Setting lightness to <%u>\r\n", request->hsl.lightness);
    }
    if (lightbulb_state.hue_current != request->hsl.hue) {
      log("Setting hue to <%u>\r\n", request->hsl.hue);
    }
    if (lightbulb_state.saturation_current != request->hsl.saturation) {
      log("Setting saturation to <%u>\r\n", request->hsl.saturation);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      sl_btmesh_set_lightness_current(request->hsl.lightness);
      sl_btmesh_set_lightness_target(request->hsl.lightness);
      if (request->hsl.lightness != 0) {
        sl_btmesh_set_lightness_last(request->hsl.lightness);
      }

      // update LED PWM duty cycle
      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_current(),
                                   IMMEDIATE);

      lightbulb_state.hue_current = request->hsl.hue;
      lightbulb_state.hue_target = request->hsl.hue;
      lightbulb_state.saturation_current = request->hsl.saturation;
      lightbulb_state.saturation_target = request->hsl.saturation;

      // update LED hue
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current,
                                  IMMEDIATE);

      // update LED saturation
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                         IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      sl_btmesh_set_lightness_target(request->hsl.lightness);
      lightbulb_state.hue_target = request->hsl.hue;
      lightbulb_state.saturation_target = request->hsl.saturation;
      sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hsl_request_timer,
                                             delay_ms,
                                             hsl_delayed_hsl_request_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed HSL Request timer\n");
      // store transition parameter for later use
      delayed_hsl_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      sl_btmesh_set_lightness_target(request->hsl.lightness);
      lightbulb_state.hue_target = request->hsl.hue;
      lightbulb_state.saturation_target = request->hsl.saturation;

      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                                   transition_ms);
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                  transition_ms);
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                         transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_transition_complete_timer,
                                             transition_ms,
                                             hsl_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start HSL Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(BTMESH_HSL_SERVER_MAIN);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    hsl_response(BTMESH_HSL_SERVER_MAIN, client_addr, appkey_index, remaining_ms);
  }
  hsl_update_and_publish(BTMESH_HSL_SERVER_MAIN, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_MAIN,
                         mesh_lighting_state_lightness_actual);
  generic_server_publish(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_HUE,
                         mesh_lighting_state_hsl_hue);
  generic_server_publish(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_SATURATION,
                         mesh_lighting_state_hsl_saturation);
}

/***************************************************************************//**
 * This function is a handler for light HSL change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void hsl_change(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (current->kind != mesh_lighting_state_hsl) {
    // if kind is not 'hsl' then just report the change here
    log("hsl_change, kind %u\r\n", current->kind);
    return;
  }

  if (sl_btmesh_get_lightness_current() != current->hsl.lightness) {
    log("Lightness update: from %u to %u\r\n",
        sl_btmesh_get_lightness_current(),
        current->hsl.lightness);
    sl_btmesh_set_lightness_current(current->hsl.lightness);
    lightbulb_state_changed();
  } else {
    log("Lightness update -same value (%u)\r\n", current->hsl.lightness);
  }

  if (lightbulb_state.hue_current != current->hsl.hue) {
    log("Hue update: from %u to %u\r\n",
        lightbulb_state.hue_current,
        current->hsl.hue);
    lightbulb_state.hue_current = current->hsl.hue;
    lightbulb_state_changed();
  } else {
    log("Hue update -same value (%u)\r\n",
        lightbulb_state.hue_current);
  }

  if (lightbulb_state.saturation_current != current->hsl.saturation) {
    log("Saturation update: from %u to %u\r\n",
        lightbulb_state.saturation_current,
        current->hsl.saturation);
    lightbulb_state.saturation_current = current->hsl.saturation;
    lightbulb_state_changed();
  } else {
    log("Saturation update -same value (%u)\r\n",
        lightbulb_state.saturation_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light HSL recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void hsl_recall(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("Light HSL recall\r\n");
  if (transition_ms == IMMEDIATE) {
    sl_btmesh_set_lightness_target(current->hsl.lightness);
    lightbulb_state.hue_target = current->hsl.hue;
    lightbulb_state.saturation_target = current->hsl.saturation;
  } else {
    sl_btmesh_set_lightness_target(target->hsl.lightness);
    lightbulb_state.hue_target = target->hsl.hue;
    lightbulb_state.saturation_target = target->hsl.saturation;
  }

  if ((sl_btmesh_get_lightness_current()
       == sl_btmesh_get_lightness_target())
      && (lightbulb_state.hue_current
          == lightbulb_state.hue_target)
      && (lightbulb_state.saturation_current
          == lightbulb_state.saturation_target)) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall HSL lightness to %u, hue to %u, saturation to %d "
        "with transition=%lu ms\r\n",
        sl_btmesh_get_lightness_target(),
        lightbulb_state.hue_target,
        lightbulb_state.saturation_target,
        transition_ms);
    if (sl_btmesh_get_lightness_current()
        != sl_btmesh_get_lightness_target()) {
      sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                                   transition_ms);
    }
    if (lightbulb_state.hue_current
        != lightbulb_state.hue_target) {
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                  transition_ms);
    }
    if (lightbulb_state.saturation_current
        != lightbulb_state.saturation_target) {
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                         transition_ms);
    }
    if (transition_ms == IMMEDIATE) {
      sl_btmesh_set_lightness_current(current->hsl.lightness);
      lightbulb_state.hue_current = current->hsl.hue;
      lightbulb_state.saturation_current = current->hsl.saturation;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_transition_complete_timer,
                                             transition_ms,
                                             hsl_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start HSL Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  // Lightness substate is updated in lightness_recall, here only hue and
  // saturation substate is updated, it is needed also for LC recall
  // to not set LC mode to zero by bindings
  sl_status_t e;
  e = hsl_hue_update(BTMESH_HSL_SERVER_HUE, transition_ms);
  e = hsl_saturation_update(BTMESH_HSL_SERVER_SATURATION, transition_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                               BTMESH_HSL_SERVER_MAIN,
                               mesh_lighting_state_hsl);
  }
}

/***************************************************************************//**
 * This function is called when a light HSL request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void hsl_transition_complete(void)
{
  // transition done -> set state, update and publish
  sl_btmesh_set_lightness_current(sl_btmesh_get_lightness_target());
  lightbulb_state.hue_current = lightbulb_state.hue_target;
  lightbulb_state.saturation_current = lightbulb_state.saturation_target;

  log("Transition complete. New lightness is %u, "
      "new hue is %u and new saturation is %u\r\n",
      sl_btmesh_get_lightness_current(),
      lightbulb_state.hue_current,
      lightbulb_state.saturation_current);

  lightbulb_state_changed();
  hsl_update_and_publish(BTMESH_HSL_SERVER_MAIN, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light HSL request has completed.
 ******************************************************************************/
static void delayed_hsl_request(void)
{
  log("Starting delayed HSL request: lightness %u -> %u, hue %u -> %u, "
      "saturation %u -> %u, %lu ms\r\n",
      sl_btmesh_get_lightness_current(),
      sl_btmesh_get_lightness_target(),
      lightbulb_state.hue_current,
      lightbulb_state.hue_target,
      lightbulb_state.saturation_current,
      lightbulb_state.saturation_target,
      delayed_hsl_trans
      );

  sl_btmesh_lighting_set_level(sl_btmesh_get_lightness_target(),
                               delayed_hsl_trans);
  sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                              delayed_hsl_trans);
  sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                     delayed_hsl_trans);

  if (delayed_hsl_trans == 0) {
    // no transition delay, update state immediately
    sl_btmesh_set_lightness_current(sl_btmesh_get_lightness_target());
    lightbulb_state.hue_current = lightbulb_state.hue_target;
    lightbulb_state.saturation_current = lightbulb_state.saturation_target;

    lightbulb_state_changed();
    hsl_update_and_publish(BTMESH_HSL_SERVER_MAIN, delayed_hsl_trans);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&hsl_transition_complete_timer,
                                           delayed_hsl_trans,
                                           hsl_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start HSL Transition Complete timer\n");
  }
}

/** @} (end addtogroup LightHSL) */

/***************************************************************************//**
 * \defgroup LightHSLSetup
 * \brief Light HSL Setup Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightHSLSetup
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light HSL setup request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] kind           Type of state used in light HSL setup response.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_setup_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_hsl_default:
      current.hsl.lightness = sl_btmesh_get_lightness_default();
      current.hsl.hue = lightbulb_state.hue_default;
      current.hsl.saturation = lightbulb_state.saturation_default;
      break;
    case mesh_lighting_state_hsl_range:
      current.hsl_range.hue_min = lightbulb_state.hue_min;
      current.hsl_range.hue_max = lightbulb_state.hue_max;
      current.hsl_range.saturation_min = lightbulb_state.saturation_min;
      current.hsl_range.saturation_max = lightbulb_state.saturation_max;
      break;
    default:
      break;
  }

  return generic_server_respond(MESH_LIGHTING_HSL_SETUP_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                NULL,
                                IMMEDIATE,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update light HSL setup state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] kind           Type of state used in light HSL setup update.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_setup_update(uint16_t element_index,
                                    mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_hsl_default:
      current.hsl.lightness = sl_btmesh_get_lightness_default();
      current.hsl.hue = lightbulb_state.hue_default;
      current.hsl.saturation = lightbulb_state.saturation_default;
      break;
    case mesh_lighting_state_hsl_range:
      current.hsl_range.hue_min = lightbulb_state.hue_min;
      current.hsl_range.hue_max = lightbulb_state.hue_max;
      current.hsl_range.saturation_min = lightbulb_state.saturation_min;
      current.hsl_range.saturation_max = lightbulb_state.saturation_max;
      break;
    default:
      break;
  }

  return generic_server_update(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               NULL,
                               IMMEDIATE);
}

/***************************************************************************//**
 * This function process the requests for the light HSL setup model.
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
static void hsl_setup_request(uint16_t model_id,
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
    case mesh_lighting_request_hsl_default:
      kind = mesh_lighting_state_hsl_default;
      log("hsl_setup_request: state=hsl_default, default lightness=%u, "
          "default hue=%u, default saturation=%u\r\n",
          request->hsl.lightness,
          request->hsl.hue,
          request->hsl.saturation);

      if ((sl_btmesh_get_lightness_default() == request->hsl.lightness)
          && (lightbulb_state.hue_default == request->hsl.hue)
          && (lightbulb_state.saturation_default == request->hsl.saturation)) {
        log("Request for current state received; no op\r\n");
      } else {
        if (sl_btmesh_get_lightness_default() != request->hsl.lightness) {
          log("Setting default lightness to <%u>\r\n", request->hsl.lightness);
          sl_btmesh_set_lightness_default(request->hsl.lightness);
        }
        if (lightbulb_state.hue_default != request->hsl.hue) {
          log("Setting default hue to <%u>\r\n",
              request->hsl.hue);
          lightbulb_state.hue_default = request->hsl.hue;
        }
        if (lightbulb_state.saturation_default != request->hsl.saturation) {
          log("Setting default saturation to <%u>\r\n", request->hsl.saturation);
          lightbulb_state.saturation_default = request->hsl.saturation;
        }
        lightbulb_state_changed();
      }
      break;

    case mesh_lighting_request_hsl_range:
      kind = mesh_lighting_state_hsl_range;
      log("hsl_setup_request: state=hsl_range, min hue=%u, max hue=%u, "
          "min saturation=%u, max saturation=%u\r\n",
          request->hsl_range.hue_min,
          request->hsl_range.hue_max,
          request->hsl_range.saturation_min,
          request->hsl_range.saturation_max);

      if ((lightbulb_state.hue_min == request->hsl_range.hue_min)
          && (lightbulb_state.hue_max == request->hsl_range.hue_max)
          && (lightbulb_state.saturation_min
              == request->hsl_range.saturation_min)
          && (lightbulb_state.saturation_max
              == request->hsl_range.saturation_max)) {
        log("Request for current state received; no op\r\n");
      } else {
        if (lightbulb_state.hue_min != request->hsl_range.hue_min) {
          log("Setting min hue to <%u>\r\n", request->hsl_range.hue_min);
          lightbulb_state.hue_min = request->hsl_range.hue_min;
        }
        if (lightbulb_state.hue_max != request->hsl_range.hue_max) {
          log("Setting max hue to <%u>\r\n", request->hsl_range.hue_max);
          lightbulb_state.hue_max = request->hsl_range.hue_max;
        }
        if (lightbulb_state.saturation_min != request->hsl_range.saturation_min) {
          log("Setting min saturation to <%u>\r\n",
              request->hsl_range.saturation_min);
          lightbulb_state.saturation_min = request->hsl_range.saturation_min;
        }
        if (lightbulb_state.saturation_max != request->hsl_range.saturation_max) {
          log("Setting max saturation to <%u>\r\n",
              request->hsl_range.saturation_max);
          lightbulb_state.saturation_max = request->hsl_range.saturation_max;
        }
        lightbulb_state_changed();
      }
      break;

    default:
      break;
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    hsl_setup_response(element_index, client_addr, appkey_index, kind);
  } else {
    hsl_setup_update(element_index, kind);
  }
}

/***************************************************************************//**
 * This function is a handler for light HSL setup change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void hsl_setup_change(uint16_t model_id,
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
    case mesh_lighting_state_hsl_default:
      if (sl_btmesh_get_lightness_default() != current->hsl.lightness) {
        log("Default lightness update: from %u to %u\r\n",
            sl_btmesh_get_lightness_default(),
            current->hsl.lightness);
        sl_btmesh_set_lightness_default(current->hsl.lightness);
        lightbulb_state_changed();
      } else {
        log("Default lightness update -same value (%u)\r\n",
            sl_btmesh_get_lightness_default());
      }
      if (lightbulb_state.hue_default != current->hsl.hue) {
        log("Default hue change: from %u to %u\r\n",
            lightbulb_state.hue_default,
            current->hsl.hue);
        lightbulb_state.hue_default = current->hsl.hue;
        lightbulb_state_changed();
      } else {
        log("Default hue update -same value (%u)\r\n",
            lightbulb_state.hue_default);
      }
      if (lightbulb_state.saturation_default != current->hsl.saturation) {
        log("Default saturation change: from %u to %u\r\n",
            lightbulb_state.saturation_default,
            current->hsl.saturation);
        lightbulb_state.saturation_default = current->hsl.saturation;
        lightbulb_state_changed();
      } else {
        log("Default saturation update -same value (%u)\r\n",
            lightbulb_state.saturation_default);
      }
      break;

    case mesh_lighting_state_hsl_range:
      if (lightbulb_state.hue_min != current->hsl_range.hue_min) {
        log("Min hue update: from %u to %u\r\n",
            lightbulb_state.hue_min,
            current->hsl_range.hue_min);
        lightbulb_state.hue_min = current->hsl_range.hue_min;
        lightbulb_state_changed();
      } else {
        log("Min hue update -same value (%u)\r\n",
            lightbulb_state.hue_min);
      }
      if (lightbulb_state.hue_max != current->hsl_range.hue_max) {
        log("Max hue update: from %u to %u\r\n",
            lightbulb_state.hue_max,
            current->hsl_range.hue_max);
        lightbulb_state.hue_max = current->hsl_range.hue_max;
        lightbulb_state_changed();
      } else {
        log("Max hue update -same value (%u)\r\n",
            lightbulb_state.hue_max);
      }
      if (lightbulb_state.saturation_min != current->hsl_range.saturation_min) {
        log("Min_saturation_change: from %u to %u\r\n",
            lightbulb_state.saturation_min,
            current->hsl_range.saturation_min);
        lightbulb_state.saturation_min = current->hsl_range.saturation_min;
        lightbulb_state_changed();
      } else {
        log("Min saturation update -same value (%u)\r\n",
            lightbulb_state.saturation_min);
      }
      if (lightbulb_state.saturation_max != current->hsl_range.saturation_max) {
        log("Max_saturation_change: from %u to %u\r\n",
            lightbulb_state.saturation_max,
            current->hsl_range.saturation_max);
        lightbulb_state.saturation_max = current->hsl_range.saturation_max;
        lightbulb_state_changed();
      } else {
        log("Max saturation update -same value (%u)\r\n",
            lightbulb_state.saturation_max);
      }
      break;

    default:
      break;
  }
}

/** @} (end addtogroup LightHSLSetup) */

/***************************************************************************//**
 * \defgroup LightHSLHue
 * \brief Light HSL Hue Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightHSLHue
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light HSL hue request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_hue_response(uint16_t element_index,
                                    uint16_t client_addr,
                                    uint16_t appkey_index,
                                    uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl_hue;
  current.hsl_hue.hue = lightbulb_state.hue_current;

  target.kind = mesh_lighting_state_hsl_hue;
  target.hsl_hue.hue = lightbulb_state.hue_target;

  return generic_server_respond(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update light HSL hue state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_hue_update(uint16_t element_index,
                                  uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl_hue;
  current.hsl_hue.hue = lightbulb_state.hue_current;

  target.kind = mesh_lighting_state_hsl_hue;
  target.hsl_hue.hue = lightbulb_state.hue_target;

  return generic_server_update(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light HSL hue state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_hue_update_and_publish(uint16_t element_index,
                                              uint32_t remaining_ms)
{
  sl_status_t e;
  (void)element_index;

  e = hsl_hue_update(BTMESH_HSL_SERVER_HUE, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                               BTMESH_HSL_SERVER_HUE,
                               mesh_lighting_state_hsl_hue);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light HSL hue model.
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
static void hsl_hue_request(uint16_t model_id,
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

  log("hsl_hue_request: hue=%u, transition=%lu, delay=%u\r\n",
      request->hsl_hue,
      transition_ms, delay_ms);

  if (lightbulb_state.hue_current == request->hsl_hue) {
    log("Request for current state received; no op\r\n");
  } else {
    if (lightbulb_state.hue_current != request->hsl_hue) {
      log("Setting hue to <%u>\r\n", request->hsl_hue);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.hue_current = request->hsl_hue;
      lightbulb_state.hue_target = request->hsl_hue;

      // update LED hue
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current,
                                  IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the hue change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.hue_target = request->hsl_hue;
      sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hsl_hue_timer,
                                             delay_ms,
                                             hsl_delayed_hsl_hue_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed Hue timer\n");
      // store transition parameter for later use
      delayed_hsl_hue_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.hue_target = request->hsl_hue;

      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                  transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_hue_transition_complete_timer,
                                             transition_ms,
                                             hsl_hue_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Hue Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(BTMESH_HSL_SERVER_HUE);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    hsl_hue_response(BTMESH_HSL_SERVER_HUE,
                     client_addr,
                     appkey_index,
                     remaining_ms);
  }
  hsl_hue_update_and_publish(BTMESH_HSL_SERVER_HUE, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_MAIN,
                         mesh_lighting_state_hsl);
  generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_HUE,
                         mesh_generic_state_level);
}

/***************************************************************************//**
 * This function is a handler for light HSL hue change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void hsl_hue_change(uint16_t model_id,
                           uint16_t element_index,
                           const struct mesh_generic_state *current,
                           const struct mesh_generic_state *target,
                           uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.hue_current != current->hsl_hue.hue) {
    log("Hue update: from %u to %u\r\n",
        lightbulb_state.hue_current,
        current->hsl_hue.hue);
    lightbulb_state.hue_current = current->hsl_hue.hue;
    lightbulb_state_changed();
  } else {
    log("Hue update -same value (%u)\r\n", lightbulb_state.hue_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light HSL hue recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void hsl_hue_recall(uint16_t model_id,
                           uint16_t element_index,
                           const struct mesh_generic_state *current,
                           const struct mesh_generic_state *target,
                           uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("HSL hue recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.hue_target = current->hsl_hue.hue;
  } else {
    lightbulb_state.hue_target = target->hsl_hue.hue;
  }

  if (lightbulb_state.hue_current == lightbulb_state.hue_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall HSL hue to %u with transition=%lu ms\r\n",
        lightbulb_state.hue_target,
        transition_ms);
    sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                transition_ms);

    if (transition_ms == IMMEDIATE) {
      lightbulb_state.hue_current = current->hsl_hue.hue;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_hue_transition_complete_timer,
                                             transition_ms,
                                             hsl_hue_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Hue Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  hsl_hue_update_and_publish(BTMESH_HSL_SERVER_HUE, transition_ms);
}

/***************************************************************************//**
 * This function is called when a light HSL hue request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void hsl_hue_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.hue_current = lightbulb_state.hue_target;

  log("Transition complete. New hue is %u\r\n", lightbulb_state.hue_current);

  lightbulb_state_changed();
  hsl_hue_update_and_publish(BTMESH_HSL_SERVER_HUE, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light HSL hue request has completed.
 ******************************************************************************/
static void delayed_hsl_hue_request(void)
{
  log("Starting delayed HSL hue request: hue %u -> %u, %lu ms\r\n",
      lightbulb_state.hue_current,
      lightbulb_state.hue_target,
      delayed_hsl_hue_trans);

  sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                              delayed_hsl_hue_trans);

  if (delayed_hsl_hue_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.hue_current = lightbulb_state.hue_target;

    lightbulb_state_changed();
    hsl_hue_update_and_publish(BTMESH_HSL_SERVER_HUE,
                               delayed_hsl_hue_trans);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&hsl_hue_transition_complete_timer,
                                           delayed_hsl_hue_trans,
                                           hsl_hue_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start Hue Transition Complete timer\n");
  }
}

/** @} (end addtogroup LightHSLHue) */

/***************************************************************************//**
 * \defgroup HueGenericLevel
 * \brief Generic Level Server model on hue element.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup HueGenericLevel
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic level request on hue element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hue_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.hue_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.hue_level_target;

  return generic_server_respond(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update generic level state on hue element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hue_level_update(uint16_t element_index,
                                    uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.hue_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.hue_level_target;

  return generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update generic level state on hue element
 * and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hue_level_update_and_publish(uint16_t element_index,
                                                uint32_t remaining_ms)
{
  sl_status_t e;

  e = hue_level_update(element_index, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_level);
  }

  return e;
}

/***************************************************************************//**
 * Schedule next generic level move request on hue element.
 *
 * @param[in] remaining_delta   The remaining level delta to the target state.
 ******************************************************************************/
static void hue_level_move_schedule_next_request(int32_t remaining_delta)
{
  uint32_t transition_ms = 0;
  if (remaining_delta == 0) {
    if (move_hue_level_delta > 0) {
      lightbulb_state.hue_level_current = 0x8000; // Min level value
      lightbulb_state.hue_level_target = 0x7FFF; // Max level value
    } else if (move_hue_level_delta < 0) {
      lightbulb_state.hue_level_current = 0x7FFF; // Max level value
      lightbulb_state.hue_level_target = 0x8000; // Min level value
    }
    transition_ms = move_hue_level_trans;
    sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current + move_hue_level_delta,
                                move_hue_level_trans);
  } else if (abs(remaining_delta) < abs(move_hue_level_delta)) {
    transition_ms = (uint32_t)(((int64_t)move_hue_level_trans * remaining_delta)
                               / move_hue_level_delta);
    sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target, transition_ms);
  } else {
    transition_ms = move_hue_level_trans;
    sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current + move_hue_level_delta,
                                move_hue_level_trans);
  }
  sl_status_t sc = sl_simple_timer_start(&hsl_hue_level_move_timer,
                                         transition_ms,
                                         hsl_hue_level_move_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start Hue Generic Level Move timer\n");
}

/***************************************************************************//**
 * Handle generic level move request on hue element.
 ******************************************************************************/
static void hue_level_move_request(void)
{
  log("Hue generic level move: level %d -> %d, delta %d in %lu ms\r\n",
      lightbulb_state.hue_level_current,
      lightbulb_state.hue_level_target,
      move_hue_level_delta,
      move_hue_level_trans);

  int32_t remaining_delta = (int32_t)lightbulb_state.hue_level_target
                            - lightbulb_state.hue_level_current;

  if (abs(remaining_delta) < abs(move_hue_level_delta)) {
    // end of move level as it reached target state
    lightbulb_state.hue_level_current = lightbulb_state.hue_level_target;
    lightbulb_state.hue_current = lightbulb_state.hue_target;
  } else {
    lightbulb_state.hue_level_current += move_hue_level_delta;
    lightbulb_state.hue_current += move_hue_level_delta;
  }
  lightbulb_state_changed();
  hue_level_update_and_publish(BTMESH_HSL_SERVER_HUE, UNKNOWN_REMAINING_TIME);

  remaining_delta = (int32_t)lightbulb_state.hue_level_target
                    - lightbulb_state.hue_level_current;

  hue_level_move_schedule_next_request(remaining_delta);
}

/***************************************************************************//**
 * Stop generic level move on hue element.
 ******************************************************************************/
static void hue_level_move_stop(void)
{
  // Cancel timers
  sl_status_t sc = sl_simple_timer_stop(&hsl_delayed_hue_level_timer);
  app_assert_status_f(sc, "Failed to stop Delayed Hue Generic Level timer\n");
  sc = sl_simple_timer_stop(&hsl_hue_level_move_timer);
  app_assert_status_f(sc, "Failed to stop Hue Generic Level Move timer\n");
  //Reset move parameters
  move_hue_level_delta = 0;
  move_hue_level_trans = 0;
}

/***************************************************************************//**
 * This function process the requests for the generic level model on hue element.
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
static void hue_level_request(uint16_t model_id,
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

  uint16_t hue;
  uint32_t remaining_ms = UNKNOWN_REMAINING_TIME;

  switch (request->kind) {
    case mesh_generic_request_level:
      log("hue_level_request (generic): level=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);

      hue_level_move_stop();
      if (lightbulb_state.hue_level_current == request->level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.hue_level_target = request->level;
      } else {
        log("Setting hue generic level to <%d>\r\n", request->level);

        hue = request->level + 32768;

        if (transition_ms == 0 && delay_ms == 0) { // Immediate change
          lightbulb_state.hue_level_current = request->level;
          lightbulb_state.hue_level_target = request->level;
          lightbulb_state.hue_current = hue;
          lightbulb_state.hue_target = hue;

          // update LED Hue
          sl_btmesh_hsl_set_hue_level(hue, IMMEDIATE);
        } else if (delay_ms > 0) {
          // a delay has been specified for the change. Start a soft timer
          // that will trigger the change after the given delay
          // Current state remains as is for now
          lightbulb_state.hue_level_target = request->level;
          lightbulb_state.hue_target = hue;
          hue_level_request_kind = mesh_generic_request_level;
          sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hue_level_timer,
                                                 delay_ms,
                                                 hsl_delayed_hue_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Hue Generic Level timer\n");
          // store transition parameter for later use
          delayed_hue_level_trans = transition_ms;
        } else {
          // no delay but transition time has been set.
          lightbulb_state.hue_level_target = request->level;
          lightbulb_state.hue_target = hue;
          sl_btmesh_hsl_set_hue_level(hue, transition_ms);

          // lightbulb current state will be updated when transition is complete
          sl_status_t sc = sl_simple_timer_start(&hsl_hue_level_transition_complete_timer,
                                                 delayed_hue_level_trans,
                                                 hsl_hue_level_transition_complete_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Hue Generic Level Transition Complete timer\n");
        }

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }

      remaining_ms = delay_ms + transition_ms;
      break;

    case mesh_generic_request_level_move: {
      log("hue_level_request (move): delta=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);
      // Store move parameters
      move_hue_level_delta = request->level;
      move_hue_level_trans = transition_ms;

      int16_t requested_level = 0;
      if (move_hue_level_delta > 0) {
        requested_level = 0x7FFF; // Max level value
      } else if (move_hue_level_delta < 0) {
        requested_level = 0x8000; // Min level value
      }

      if (lightbulb_state.hue_level_current == requested_level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.hue_level_target = requested_level;
        remaining_ms = IMMEDIATE;
      } else {
        log("Setting hue level to <%d>\r\n", requested_level);

        hue = requested_level + 32768;

        if (delay_ms > 0) {
          // a delay has been specified for the move. Start a soft timer
          // that will trigger the move after the given delay
          // Current state remains as is for now
          lightbulb_state.hue_level_target = requested_level;
          lightbulb_state.hue_target = hue;
          hue_level_request_kind = mesh_generic_request_level_move;
          sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hue_level_timer,
                                                 delay_ms,
                                                 hsl_delayed_hue_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Hue Generic Level timer\n");
        } else {
          // no delay so start move
          lightbulb_state.hue_level_target = requested_level;
          lightbulb_state.hue_target = hue;

          int32_t remaining_delta = (int32_t)lightbulb_state.hue_level_target
                                    - lightbulb_state.hue_level_current;
          hue_level_move_schedule_next_request(remaining_delta);
        }
        remaining_ms = UNKNOWN_REMAINING_TIME;

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }
      break;
    }

    case mesh_generic_request_level_halt:
      log("hue_level_request (halt)\r\n");

      // Set current state
      lightbulb_state.hue_current = sl_btmesh_get_hue();
      lightbulb_state.hue_target = lightbulb_state.hue_current;
      lightbulb_state.hue_level_current = lightbulb_state.hue_current - 32768;
      lightbulb_state.hue_level_target = lightbulb_state.hue_level_current;
      if (delay_ms > 0) {
        // a delay has been specified for the move halt. Start a soft timer
        // that will trigger the move halt after the given delay
        // Current state remains as is for now
        remaining_ms = delay_ms;
        hue_level_request_kind = mesh_generic_request_level_halt;
        sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hue_level_timer,
                                               delay_ms,
                                               hsl_delayed_hue_level_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Delayed Hue Generic Level timer\n");
      } else {
        hue_level_move_stop();
        sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current, IMMEDIATE);
        remaining_ms = IMMEDIATE;
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    hue_level_response(element_index, client_addr, appkey_index, remaining_ms);
  }
  hue_level_update_and_publish(element_index, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_hsl_hue);
}

/***************************************************************************//**
 * This function is a handler for generic level change event
 * on hue element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void hue_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.hue_level_current != current->level.level) {
    log("Hue generic level update: from %d to %d\r\n",
        lightbulb_state.hue_level_current,
        current->level.level);
    lightbulb_state.hue_level_current = current->level.level;
    lightbulb_state_changed();
    hue_level_move_stop();
  } else {
    log("Hue generic level update -same value (%d)\r\n",
        lightbulb_state.hue_level_current);
  }
}

/***************************************************************************//**
 * This function is a handler for generic level recall event
 * on hue element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void hue_level_recall(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("Hue Generic Level recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.hue_level_target = current->level.level;
  } else {
    lightbulb_state.hue_level_target = target->level.level;
  }

  if (lightbulb_state.hue_level_current == lightbulb_state.hue_level_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall hue level to %d with transition=%lu ms\r\n",
        lightbulb_state.hue_level_target,
        transition_ms);
    if (transition_ms == IMMEDIATE) {
      lightbulb_state.hue_level_current = current->level.level;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_hue_level_transition_complete_timer,
                                             transition_ms,
                                             hsl_hue_level_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Hue Generic Level Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }
}

/***************************************************************************//**
 * This function is called when a generic level request on hue element
 * with non-zero transition time has completed.
 ******************************************************************************/
static void hue_level_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.hue_level_current = lightbulb_state.hue_level_target;
  lightbulb_state.hue_current = lightbulb_state.hue_target;

  log("Transition complete. New hue generic level is %d\r\n",
      lightbulb_state.hue_level_current);

  lightbulb_state_changed();
  hue_level_update_and_publish(BTMESH_HSL_SERVER_HUE, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for generic level request
 * on hue element has completed.
 ******************************************************************************/
static void delayed_hue_level_request(void)
{
  log("Starting delayed hue generic level request: level %d -> %d, %lu ms\r\n",
      lightbulb_state.hue_level_current,
      lightbulb_state.hue_level_target,
      delayed_hue_level_trans);

  switch (hue_level_request_kind) {
    case mesh_generic_request_level:
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                  delayed_hue_level_trans);

      if (delayed_hue_level_trans == 0) {
        // no transition delay, update state immediately
        lightbulb_state.hue_level_current = lightbulb_state.hue_level_target;
        lightbulb_state.hue_current = lightbulb_state.hue_target;

        lightbulb_state_changed();
        hue_level_update_and_publish(BTMESH_HSL_SERVER_HUE,
                                     delayed_hue_level_trans);
      } else {
        // state is updated when transition is complete
        sl_status_t sc = sl_simple_timer_start(&hsl_hue_level_transition_complete_timer,
                                               delayed_hue_level_trans,
                                               hsl_hue_level_transition_complete_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Hue Generic Level Transition Complete timer\n");
      }
      break;

    case mesh_generic_request_level_move:
      hue_level_move_schedule_next_request((int32_t)lightbulb_state.hue_level_target
                                           - lightbulb_state.hue_level_current);
      hue_level_update_and_publish(BTMESH_HSL_SERVER_HUE,
                                   UNKNOWN_REMAINING_TIME);
      break;

    case mesh_generic_request_level_halt:
      // Set current state
      lightbulb_state.hue_current = sl_btmesh_get_hue();
      lightbulb_state.hue_target = lightbulb_state.hue_current;
      lightbulb_state.hue_level_current = lightbulb_state.hue_current - 32768;
      lightbulb_state.hue_level_target = lightbulb_state.hue_level_current;
      hue_level_move_stop();
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current,
                                  IMMEDIATE);
      hue_level_update_and_publish(BTMESH_HSL_SERVER_HUE, IMMEDIATE);
      break;

    default:
      break;
  }
}

/** @} (end addtogroup HueGenericLevel) */

/***************************************************************************//**
 * \defgroup LightHSLSaturation
 * \brief Light HSL Saturation Server model.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LightHSLSaturation
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to light HSL saturation request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_saturation_response(uint16_t element_index,
                                           uint16_t client_addr,
                                           uint16_t appkey_index,
                                           uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl_saturation;
  current.hsl_saturation.saturation = lightbulb_state.saturation_current;

  target.kind = mesh_lighting_state_hsl_saturation;
  target.hsl_saturation.saturation = lightbulb_state.saturation_target;

  return generic_server_respond(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update light HSL saturation state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_saturation_update(uint16_t element_index,
                                         uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_hsl_saturation;
  current.hsl_saturation.saturation = lightbulb_state.saturation_current;

  target.kind = mesh_lighting_state_hsl_saturation;
  target.hsl_saturation.saturation = lightbulb_state.saturation_target;

  return generic_server_update(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update light HSL saturation state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t hsl_saturation_update_and_publish(uint16_t element_index,
                                                     uint32_t remaining_ms)
{
  sl_status_t e;
  (void)element_index;

  e = hsl_saturation_update(BTMESH_HSL_SERVER_SATURATION, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                               BTMESH_HSL_SERVER_SATURATION,
                               mesh_lighting_state_hsl_saturation);
  }

  return e;
}

/***************************************************************************//**
 * This function process the requests for the light HSL saturation model.
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
static void hsl_saturation_request(uint16_t model_id,
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

  log("hsl_saturation_request: saturation=%u, transition=%lu, delay=%u\r\n",
      request->hsl_saturation,
      transition_ms, delay_ms);

  if (lightbulb_state.saturation_current == request->hsl_saturation) {
    log("Request for current state received; no op\r\n");
  } else {
    if (lightbulb_state.saturation_current != request->hsl_saturation) {
      log("Setting saturation to <%u>\r\n", request->hsl_saturation);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.saturation_current = request->hsl_saturation;
      lightbulb_state.saturation_target = request->hsl_saturation;

      // update LED saturation
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                         IMMEDIATE);
    } else if (delay_ms > 0) {
      // a delay has been specified for the saturation change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.saturation_target = request->hsl_saturation;
      sl_status_t sc = sl_simple_timer_start(&hsl_delayed_hsl_saturation_timer,
                                             delay_ms,
                                             hsl_delayed_hsl_saturation_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Delayed Saturation timer\n");
      // store transition parameter for later use
      delayed_hsl_saturation_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.saturation_target = request->hsl_saturation;

      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                         transition_ms);

      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_saturation_transition_complete_timer,
                                             transition_ms,
                                             hsl_saturation_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Saturation Transition Complete timer\n");
    }
    lightbulb_state_changed();

    // State has changed, so the current scene number is reset
    scene_server_reset_register(BTMESH_HSL_SERVER_SATURATION);
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    hsl_saturation_response(BTMESH_HSL_SERVER_SATURATION,
                            client_addr,
                            appkey_index,
                            remaining_ms);
  }
  hsl_saturation_update_and_publish(BTMESH_HSL_SERVER_SATURATION, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_MAIN,
                         mesh_lighting_state_hsl);
  generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                         BTMESH_HSL_SERVER_SATURATION,
                         mesh_generic_state_level);
}

/***************************************************************************//**
 * This function is a handler for light HSL saturation change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void hsl_saturation_change(uint16_t model_id,
                                  uint16_t element_index,
                                  const struct mesh_generic_state *current,
                                  const struct mesh_generic_state *target,
                                  uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.saturation_current != current->hsl_saturation.saturation) {
    log("Saturation update: from %u to %u\r\n",
        lightbulb_state.saturation_current,
        current->hsl_saturation.saturation);
    lightbulb_state.saturation_current = current->hsl_saturation.saturation;
    lightbulb_state_changed();
  } else {
    log("Saturation update -same value (%u)\r\n", lightbulb_state.saturation_current);
  }
}

/***************************************************************************//**
 * This function is a handler for light HSL saturation recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void hsl_saturation_recall(uint16_t model_id,
                                  uint16_t element_index,
                                  const struct mesh_generic_state *current,
                                  const struct mesh_generic_state *target,
                                  uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("HSL saturation recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.saturation_target = current->hsl_saturation.saturation;
  } else {
    lightbulb_state.saturation_target = target->hsl_saturation.saturation;
  }

  if (lightbulb_state.saturation_current == lightbulb_state.saturation_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall HSL saturation to %u with transition=%lu ms\r\n",
        lightbulb_state.saturation_target,
        transition_ms);
    sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                       transition_ms);

    if (transition_ms == IMMEDIATE) {
      lightbulb_state.saturation_current = current->hsl_saturation.saturation;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_saturation_transition_complete_timer,
                                             transition_ms,
                                             hsl_saturation_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Saturation Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }

  hsl_saturation_update_and_publish(BTMESH_HSL_SERVER_SATURATION, transition_ms);
}

/***************************************************************************//**
 * This function is called when a light HSL saturation request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void hsl_saturation_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.saturation_current = lightbulb_state.saturation_target;

  log("Transition complete. New saturation is %u\r\n",
      lightbulb_state.saturation_current);

  lightbulb_state_changed();
  hsl_saturation_update_and_publish(BTMESH_HSL_SERVER_SATURATION, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for light HSL saturation request has completed.
 ******************************************************************************/
static void delayed_hsl_saturation_request(void)
{
  log("Starting delayed HSL saturation request: saturation %u -> %u, %lu ms\r\n",
      lightbulb_state.saturation_current,
      lightbulb_state.saturation_target,
      delayed_hsl_saturation_trans);

  sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                     delayed_hsl_saturation_trans);

  if (delayed_hsl_saturation_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.saturation_current = lightbulb_state.saturation_target;

    lightbulb_state_changed();
    hsl_saturation_update_and_publish(BTMESH_HSL_SERVER_SATURATION,
                                      delayed_hsl_saturation_trans);
  } else {
    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&hsl_saturation_transition_complete_timer,
                                           delayed_hsl_saturation_trans,
                                           hsl_saturation_transition_complete_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start Saturation Transition Complete timer\n");
  }
}

/** @} (end addtogroup LightHSLSaturation) */

/***************************************************************************//**
 * \defgroup SaturationGenericLevel
 * \brief Generic Level Server model on saturation element.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SaturationGenericLevel
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to generic level request on saturation element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t saturation_level_response(uint16_t element_index,
                                             uint16_t client_addr,
                                             uint16_t appkey_index,
                                             uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.saturation_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.saturation_level_target;

  return generic_server_respond(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                element_index,
                                client_addr,
                                appkey_index,
                                &current,
                                &target,
                                remaining_ms,
                                NO_FLAGS);
}

/***************************************************************************//**
 * Update generic level state on saturation element.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t saturation_level_update(uint16_t element_index,
                                           uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.saturation_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.saturation_level_target;

  return generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               &current,
                               &target,
                               remaining_ms);
}

/***************************************************************************//**
 * Update generic level state on saturation element
 * and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t saturation_level_update_and_publish(uint16_t element_index,
                                                       uint32_t remaining_ms)
{
  sl_status_t e;

  e = saturation_level_update(element_index, remaining_ms);
  if (e == SL_STATUS_OK) {
    e = generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                               element_index,
                               mesh_generic_state_level);
  }

  return e;
}

/***************************************************************************//**
 * Schedule next generic level move request on saturation element.
 *
 * @param[in] remaining_delta   The remaining level delta to the target state.
 ******************************************************************************/
static void saturation_level_move_schedule_next_request(int32_t remaining_delta)
{
  uint32_t transition_ms = 0;
  if (remaining_delta == 0) {
    if (move_saturation_level_delta > 0) {
      lightbulb_state.saturation_level_current = 0x8000; // Min level value
      lightbulb_state.saturation_level_target = 0x7FFF; // Max level value
    } else if (move_saturation_level_delta < 0) {
      lightbulb_state.saturation_level_current = 0x7FFF; // Max level value
      lightbulb_state.saturation_level_target = 0x8000; // Min level value
    }
    transition_ms = move_saturation_level_trans;
    sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current + move_saturation_level_delta,
                                       move_saturation_level_trans);
  } else if (abs(remaining_delta) < abs(move_saturation_level_delta)) {
    transition_ms = (uint32_t)(((int64_t)move_saturation_level_trans * remaining_delta)
                               / move_saturation_level_delta);
    sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                       transition_ms);
  } else {
    transition_ms = move_saturation_level_trans;
    sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current + move_saturation_level_delta,
                                       move_saturation_level_trans);
  }
  sl_status_t sc = sl_simple_timer_start(&hsl_saturation_level_move_timer,
                                         transition_ms,
                                         hsl_saturation_level_move_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start Saturation Generic Level Move timer\n");
}

/***************************************************************************//**
 * Handle generic level move request on saturation element.
 ******************************************************************************/
static void saturation_level_move_request(void)
{
  log("Saturation generic level move: level %d -> %d, delta %d in %lu ms\r\n",
      lightbulb_state.saturation_level_current,
      lightbulb_state.saturation_level_target,
      move_saturation_level_delta,
      move_saturation_level_trans);

  int32_t remaining_delta = (int32_t)lightbulb_state.saturation_level_target
                            - lightbulb_state.saturation_level_current;

  if (abs(remaining_delta) < abs(move_saturation_level_delta)) {
    // end of move level as it reached target state
    lightbulb_state.saturation_level_current = lightbulb_state.saturation_level_target;
    lightbulb_state.saturation_current = lightbulb_state.saturation_target;
  } else {
    lightbulb_state.saturation_level_current += move_saturation_level_delta;
    lightbulb_state.saturation_current += move_saturation_level_delta;
  }
  lightbulb_state_changed();
  saturation_level_update_and_publish(BTMESH_HSL_SERVER_SATURATION,
                                      UNKNOWN_REMAINING_TIME);

  remaining_delta = (int32_t)lightbulb_state.saturation_level_target
                    - lightbulb_state.saturation_level_current;

  saturation_level_move_schedule_next_request(remaining_delta);
}

/***************************************************************************//**
 * Stop generic level move on saturation element.
 ******************************************************************************/
static void saturation_level_move_stop(void)
{
  // Cancel timers
  sl_status_t sc = sl_simple_timer_stop(&hsl_delayed_saturation_level_timer);
  app_assert_status_f(sc, "Failed to stop Delayed Saturation Generic Level timer\n");
  sc = sl_simple_timer_stop(&hsl_saturation_level_move_timer);
  app_assert_status_f(sc, "Failed to stop Saturation Generic Level Move timer\n");
  //Reset move parameters
  move_saturation_level_delta = 0;
  move_saturation_level_trans = 0;
}

/***************************************************************************//**
 * This function process the requests for the generic level model on saturation element.
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
static void saturation_level_request(uint16_t model_id,
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

  uint16_t saturation;
  uint32_t remaining_ms = UNKNOWN_REMAINING_TIME;

  switch (request->kind) {
    case mesh_generic_request_level:
      log("saturation_level_request (generic): level=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);

      saturation_level_move_stop();
      if (lightbulb_state.saturation_level_current == request->level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.saturation_level_target = request->level;
      } else {
        log("Setting saturation generic level to <%d>\r\n", request->level);

        saturation = request->level + 32768;

        if (transition_ms == 0 && delay_ms == 0) { // Immediate change
          lightbulb_state.saturation_level_current = request->level;
          lightbulb_state.saturation_level_target = request->level;
          lightbulb_state.saturation_current = saturation;
          lightbulb_state.saturation_target = saturation;

          // update LED Saturation
          sl_btmesh_hsl_set_saturation_level(saturation, IMMEDIATE);
        } else if (delay_ms > 0) {
          // a delay has been specified for the change. Start a soft timer
          // that will trigger the change after the given delay
          // Current state remains as is for now
          lightbulb_state.saturation_level_target = request->level;
          lightbulb_state.saturation_target = saturation;
          saturation_level_request_kind = mesh_generic_request_level;
          sl_status_t sc = sl_simple_timer_start(&hsl_delayed_saturation_level_timer,
                                                 delay_ms,
                                                 hsl_delayed_saturation_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Saturation Generic Level timer\n");
          // store transition parameter for later use
          delayed_saturation_level_trans = transition_ms;
        } else {
          // no delay but transition time has been set.
          lightbulb_state.saturation_level_target = request->level;
          lightbulb_state.saturation_target = saturation;
          sl_btmesh_hsl_set_saturation_level(saturation, transition_ms);

          // lightbulb current state will be updated when transition is complete
          sl_status_t sc = sl_simple_timer_start(&hsl_saturation_level_transition_complete_timer,
                                                 delayed_saturation_level_trans,
                                                 hsl_saturation_level_transition_complete_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Saturation Generic Level Transition Complete timer\n");
        }

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }

      remaining_ms = delay_ms + transition_ms;
      break;

    case mesh_generic_request_level_move: {
      log("saturation_level_request (move): delta=%d, transition=%lu, delay=%u\r\n",
          request->level, transition_ms, delay_ms);
      // Store move parameters
      move_saturation_level_delta = request->level;
      move_saturation_level_trans = transition_ms;

      int16_t requested_level = 0;
      if (move_saturation_level_delta > 0) {
        requested_level = 0x7FFF; // Max level value
      } else if (move_saturation_level_delta < 0) {
        requested_level = 0x8000; // Min level value
      }

      if (lightbulb_state.saturation_level_current == requested_level) {
        log("Request for current state received; no op\r\n");
        lightbulb_state.saturation_level_target = requested_level;
        remaining_ms = IMMEDIATE;
      } else {
        log("Setting saturation level to <%d>\r\n", requested_level);

        saturation = requested_level + 32768;

        if (delay_ms > 0) {
          // a delay has been specified for the move. Start a soft timer
          // that will trigger the move after the given delay
          // Current state remains as is for now
          lightbulb_state.saturation_level_target = requested_level;
          lightbulb_state.saturation_target = saturation;
          saturation_level_request_kind = mesh_generic_request_level_move;
          sl_status_t sc = sl_simple_timer_start(&hsl_delayed_saturation_level_timer,
                                                 delay_ms,
                                                 hsl_delayed_saturation_level_timer_cb,
                                                 NO_CALLBACK_DATA,
                                                 false);
          app_assert_status_f(sc, "Failed to start Delayed Saturation Generic Level timer\n");
        } else {
          // no delay so start move
          lightbulb_state.saturation_level_target = requested_level;
          lightbulb_state.saturation_target = saturation;

          int32_t remaining_delta = (int32_t)lightbulb_state.saturation_level_target
                                    - lightbulb_state.saturation_level_current;
          saturation_level_move_schedule_next_request(remaining_delta);
        }
        remaining_ms = UNKNOWN_REMAINING_TIME;

        // State has changed, so the current scene number is reset
        scene_server_reset_register(element_index);
      }
      break;
    }

    case mesh_generic_request_level_halt:
      log("saturation_level_request (halt)\r\n");

      // Set current state
      lightbulb_state.saturation_current = sl_btmesh_get_saturation();
      lightbulb_state.saturation_target = lightbulb_state.saturation_current;
      lightbulb_state.saturation_level_current = lightbulb_state.saturation_current - 32768;
      lightbulb_state.saturation_level_target = lightbulb_state.saturation_level_current;
      if (delay_ms > 0) {
        // a delay has been specified for the move halt. Start a soft timer
        // that will trigger the move halt after the given delay
        // Current state remains as is for now
        remaining_ms = delay_ms;
        saturation_level_request_kind = mesh_generic_request_level_halt;
        sl_status_t sc = sl_simple_timer_start(&hsl_delayed_saturation_level_timer,
                                               delay_ms,
                                               hsl_delayed_saturation_level_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Delayed Saturation Generic Level timer\n");
      } else {
        saturation_level_move_stop();
        sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                           IMMEDIATE);
        remaining_ms = IMMEDIATE;
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    saturation_level_response(element_index,
                              client_addr,
                              appkey_index,
                              remaining_ms);
  }
  saturation_level_update_and_publish(element_index, remaining_ms);
  // publish to bound states
  generic_server_publish(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                         element_index,
                         mesh_lighting_state_hsl_saturation);
}

/***************************************************************************//**
 * This function is a handler for generic level change event
 * on saturation element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void saturation_level_change(uint16_t model_id,
                                    uint16_t element_index,
                                    const struct mesh_generic_state *current,
                                    const struct mesh_generic_state *target,
                                    uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (lightbulb_state.saturation_level_current != current->level.level) {
    log("Saturation generic level update: from %d to %d\r\n",
        lightbulb_state.saturation_level_current,
        current->level.level);
    lightbulb_state.saturation_level_current = current->level.level;
    lightbulb_state_changed();
    saturation_level_move_stop();
  } else {
    log("Saturation generic level update -same value (%d)\r\n",
        lightbulb_state.saturation_level_current);
  }
}

/***************************************************************************//**
 * This function is a handler for generic level recall event
 * on saturation element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void saturation_level_recall(uint16_t model_id,
                                    uint16_t element_index,
                                    const struct mesh_generic_state *current,
                                    const struct mesh_generic_state *target,
                                    uint32_t transition_ms)
{
  (void)model_id;
  (void)element_index;

  log("Saturation Generic Level recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lightbulb_state.saturation_level_target = current->level.level;
  } else {
    lightbulb_state.saturation_level_target = target->level.level;
  }

  if (lightbulb_state.saturation_level_current == lightbulb_state.saturation_level_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall saturation level to %d with transition=%lu ms\r\n",
        lightbulb_state.saturation_level_target,
        transition_ms);
    if (transition_ms == IMMEDIATE) {
      lightbulb_state.saturation_level_current = current->level.level;
    } else {
      // lightbulb current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&hsl_saturation_level_transition_complete_timer,
                                             transition_ms,
                                             hsl_saturation_level_transition_complete_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start Saturation Generic Level Transition Complete timer\n");
    }
    lightbulb_state_changed();
  }
}

/***************************************************************************//**
 * This function is called when a generic level request on saturation element
 * with non-zero transition time has completed.
 ******************************************************************************/
static void saturation_level_transition_complete(void)
{
  // transition done -> set state, update and publish
  lightbulb_state.saturation_level_current = lightbulb_state.saturation_level_target;
  lightbulb_state.saturation_current = lightbulb_state.saturation_target;

  log("Transition complete. New saturation generic level is %d\r\n",
      lightbulb_state.saturation_level_current);

  lightbulb_state_changed();
  saturation_level_update_and_publish(BTMESH_HSL_SERVER_SATURATION, IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for generic level request
 * on saturation element has completed.
 ******************************************************************************/
static void delayed_saturation_level_request(void)
{
  log("Starting delayed saturation generic level request: level %d -> %d, %lu ms\r\n",
      lightbulb_state.saturation_level_current,
      lightbulb_state.saturation_level_target,
      delayed_saturation_level_trans);

  switch (saturation_level_request_kind) {
    case mesh_generic_request_level:
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                         delayed_saturation_level_trans);

      if (delayed_saturation_level_trans == 0) {
        // no transition delay, update state immediately
        lightbulb_state.saturation_level_current = lightbulb_state.saturation_level_target;
        lightbulb_state.saturation_current = lightbulb_state.saturation_target;

        lightbulb_state_changed();
        saturation_level_update_and_publish(BTMESH_HSL_SERVER_SATURATION,
                                            delayed_saturation_level_trans);
      } else {
        // state is updated when transition is complete
        sl_status_t sc = sl_simple_timer_start(&hsl_saturation_level_transition_complete_timer,
                                               delayed_saturation_level_trans,
                                               hsl_saturation_level_transition_complete_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Saturation Generic Level Transition Complete timer\n");
      }
      break;

    case mesh_generic_request_level_move:
      saturation_level_move_schedule_next_request((int32_t)lightbulb_state.saturation_level_target
                                                  - lightbulb_state.saturation_level_current);
      saturation_level_update_and_publish(BTMESH_HSL_SERVER_SATURATION,
                                          UNKNOWN_REMAINING_TIME);
      break;

    case mesh_generic_request_level_halt:
      // Set current state
      lightbulb_state.saturation_current = sl_btmesh_get_saturation();
      lightbulb_state.saturation_target = lightbulb_state.saturation_current;
      lightbulb_state.saturation_level_current = lightbulb_state.saturation_current - 32768;
      lightbulb_state.saturation_level_target = lightbulb_state.saturation_level_current;
      saturation_level_move_stop();
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                         IMMEDIATE);
      saturation_level_update_and_publish(BTMESH_HSL_SERVER_SATURATION,
                                          IMMEDIATE);
      break;

    default:
      break;
  }
}

/** @} (end addtogroup SaturationGenericLevel) */

/***************************************************************************//**
 * Initialization of the models supported by this node.
 * This function registers callbacks for each of the supported models.
 ******************************************************************************/
static void init_hsl_models(void)
{
  generic_server_register_handler(MESH_LIGHTING_HSL_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_MAIN,
                                  hsl_request,
                                  hsl_change,
                                  hsl_recall);

  generic_server_register_handler(MESH_LIGHTING_HSL_SETUP_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_MAIN,
                                  hsl_setup_request,
                                  hsl_setup_change,
                                  NULL);

  generic_server_register_handler(MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_HUE,
                                  hsl_hue_request,
                                  hsl_hue_change,
                                  hsl_hue_recall);

  generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_HUE,
                                  hue_level_request,
                                  hue_level_change,
                                  hue_level_recall);

  generic_server_register_handler(MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_SATURATION,
                                  hsl_saturation_request,
                                  hsl_saturation_change,
                                  hsl_saturation_recall);

  generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                  BTMESH_HSL_SERVER_SATURATION,
                                  saturation_level_request,
                                  saturation_level_change,
                                  saturation_level_recall);
}

/***************************************************************************//**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state.
 * If PS key with ID SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL does not exist or loading failed,
 * lightbulb_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_load(void)
{
  sl_status_t sc;
  size_t ps_len = 0;
  struct lightbulb_state ps_data;

  sc = sl_bt_nvm_load(SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL,
                      sizeof(ps_data),
                      &ps_len,
                      (uint8_t *)&ps_data);

  // Set default values if ps_load fail or size of lightbulb_state has changed
  if ((sc != SL_STATUS_OK) || (ps_len != sizeof(struct lightbulb_state))) {
    memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
    lightbulb_state.hue_default = SL_BTMESH_HSL_SERVER_DEFAULT_HUE_CFG_VAL;
    lightbulb_state.hue_min = SL_BTMESH_HSL_SERVER_MINIMUM_HUE_CFG_VAL;
    lightbulb_state.hue_max = SL_BTMESH_HSL_SERVER_MAXIMUM_HUE_CFG_VAL;
    lightbulb_state.saturation_default = SL_BTMESH_HSL_SERVER_DEFAULT_SATURATION_CFG_VAL;
    lightbulb_state.saturation_min = SL_BTMESH_HSL_SERVER_MINIMUM_SATURATION_CFG_VAL;
    lightbulb_state.saturation_max = SL_BTMESH_HSL_SERVER_MAXIMUM_SATURATION_CFG_VAL;

    // Check if default values are valid and correct them if needed
    lightbulb_state_validate_and_correct();

    if (sc == SL_STATUS_OK) {
      // The sl_bt_nvm_load call was successful but the size of the loaded data
      // differs from the expected size therefore error code shall be set
      sc = SL_STATUS_INVALID_STATE;
      log("HSL server lightbulb state loaded from PS with invalid size, "
          "use defaults. (expected=%zd,actual=%zd)\r\n",
          sizeof(struct lightbulb_state),
          ps_len);
    } else {
      log_status_f(sc,
                   "HSL server lightbulb state load from PS failed "
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
 * A PS key with ID SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL is used to store the whole struct.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lightbulb_state_store(void)
{
  sl_status_t sc;

  sc = sl_bt_nvm_save(SL_BTMESH_HSL_SERVER_PS_KEY_CFG_VAL,
                      sizeof(struct lightbulb_state),
                      (const uint8_t *)&lightbulb_state);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "HSL server lightbulb state store in PS failed.\r\n");

  return sc;
}

/***************************************************************************//**
 * This function is called each time the lightbulb state in RAM is changed.
 * It sets up a soft timer that will save the state in flash after small delay.
 * The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lightbulb_state_changed(void)
{
  sl_status_t sc = sl_simple_timer_start(&hsl_state_store_timer,
                                         SL_BTMESH_HSL_SERVER_NVM_SAVE_TIME_CFG_VAL,
                                         hsl_state_store_timer_cb,
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
  if (lightbulb_state.hue_min > lightbulb_state.hue_max) {
    lightbulb_state.hue_min = lightbulb_state.hue_max;
  }
  if (lightbulb_state.hue_default < lightbulb_state.hue_min) {
    lightbulb_state.hue_default = lightbulb_state.hue_min;
  }
  if (lightbulb_state.hue_default > lightbulb_state.hue_max) {
    lightbulb_state.hue_default = lightbulb_state.hue_max;
  }
  if (lightbulb_state.hue_current < lightbulb_state.hue_min) {
    lightbulb_state.hue_current = lightbulb_state.hue_min;
  }
  if (lightbulb_state.hue_current > lightbulb_state.hue_max) {
    lightbulb_state.hue_current = lightbulb_state.hue_max;
  }
  if (lightbulb_state.hue_target < lightbulb_state.hue_min) {
    lightbulb_state.hue_target = lightbulb_state.hue_min;
  }
  if (lightbulb_state.hue_target > lightbulb_state.hue_max) {
    lightbulb_state.hue_target = lightbulb_state.hue_max;
  }

  if (lightbulb_state.saturation_min > lightbulb_state.saturation_max) {
    lightbulb_state.saturation_min = lightbulb_state.saturation_max;
  }
  if (lightbulb_state.saturation_default < lightbulb_state.saturation_min) {
    lightbulb_state.saturation_default = lightbulb_state.saturation_min;
  }
  if (lightbulb_state.saturation_default > lightbulb_state.saturation_max) {
    lightbulb_state.saturation_default = lightbulb_state.saturation_max;
  }
  if (lightbulb_state.saturation_current < lightbulb_state.saturation_min) {
    lightbulb_state.saturation_current = lightbulb_state.saturation_min;
  }
  if (lightbulb_state.saturation_current > lightbulb_state.saturation_max) {
    lightbulb_state.saturation_current = lightbulb_state.saturation_max;
  }
  if (lightbulb_state.saturation_target < lightbulb_state.saturation_min) {
    lightbulb_state.saturation_target = lightbulb_state.saturation_min;
  }
  if (lightbulb_state.saturation_target > lightbulb_state.saturation_max) {
    lightbulb_state.saturation_target = lightbulb_state.saturation_max;
  }
}

/*******************************************************************************
 * Lightbulb state initialization.
 * This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 ******************************************************************************/
void sl_btmesh_hsl_server_init(void)
{
  memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));

  lightbulb_state_load();

  // Handle on power up behavior
  uint32_t transition_ms = sl_btmesh_get_default_transition_time();
  switch (sl_btmesh_get_lightness_onpowerup()) {
    case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
    case MESH_GENERIC_ON_POWER_UP_STATE_ON:
      lightbulb_state.hue_current = lightbulb_state.hue_default;
      lightbulb_state.hue_target = lightbulb_state.hue_default;
      sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_default, IMMEDIATE);
      lightbulb_state.saturation_current = lightbulb_state.saturation_default;
      lightbulb_state.saturation_target = lightbulb_state.saturation_default;
      sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_default,
                                         IMMEDIATE);
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
      if (transition_ms > 0
          && lightbulb_state.hue_target != lightbulb_state.hue_default) {
        lightbulb_state.hue_current = lightbulb_state.hue_default;
        sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current, IMMEDIATE);
        sl_status_t sc = sl_simple_timer_start(&hsl_hue_transition_complete_timer,
                                               transition_ms,
                                               hsl_hue_transition_complete_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Hue Transition Complete timer\n");
        sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_target,
                                    transition_ms);
      } else {
        lightbulb_state.hue_current = lightbulb_state.hue_target;
        sl_btmesh_hsl_set_hue_level(lightbulb_state.hue_current, IMMEDIATE);
      }

      if (transition_ms > 0
          && lightbulb_state.saturation_target != lightbulb_state.saturation_default) {
        lightbulb_state.saturation_current = lightbulb_state.saturation_default;
        sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                           IMMEDIATE);
        sl_status_t sc = sl_simple_timer_start(&hsl_saturation_transition_complete_timer,
                                               transition_ms,
                                               hsl_saturation_transition_complete_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start Saturation Transition Complete timer\n");
        sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_target,
                                           transition_ms);
      } else {
        lightbulb_state.saturation_current = lightbulb_state.saturation_target;
        sl_btmesh_hsl_set_saturation_level(lightbulb_state.saturation_current,
                                           IMMEDIATE);
      }
      break;

    default:
      break;
  }

  lightbulb_state_changed();
  init_hsl_models();
  hsl_setup_update(BTMESH_HSL_SERVER_MAIN, mesh_lighting_state_hsl_default);
  hsl_setup_update(BTMESH_HSL_SERVER_MAIN, mesh_lighting_state_hsl_range);
  hsl_update_and_publish(BTMESH_HSL_SERVER_MAIN, IMMEDIATE);
}

/*******************************************************************************
 * Handling of mesh events in hsl component.
 * It handles:
 *  - node_provisioned
 *  - node_initialized
 *
 * @param[in] evt   Pointer to incoming time event.
 ******************************************************************************/
void sl_btmesh_hsl_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_hsl_server_init();
      break;

    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_hsl_server_init();
      }
      break;

    default:
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
                     "HSL server respond failed "
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
                     "HSL server state update failed "
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
                      "HSL server state publish failed "
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
                      "HSL server failed to register handlers "
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
  app_assert_status_f(sc, "HSL server failed to reset scene register.\n");
}
#endif

/** @} (end addtogroup BtmeshWrappers) */

/***************************************************************************//**
 * Timer Callbacks
 ******************************************************************************/
static void hsl_hue_level_move_timer_cb(sl_simple_timer_t *handle,
                                        void *data)
{
  (void)data;
  (void)handle;
  // handling of generic level move, update the lightbulb state
  hue_level_move_request();
}

static void hsl_hue_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                       void *data)
{
  (void)data;
  (void)handle;
  // transition for a hue generic level request has completed,
  //update the lightbulb state
  hue_level_transition_complete();
}

static void hsl_hue_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                 void *data)
{
  (void)data;
  (void)handle;
  // transition for a hsl hue request has completed,
  // update the lightbulb state
  hsl_hue_transition_complete();
}
static void hsl_saturation_level_move_timer_cb(sl_simple_timer_t *handle,
                                               void *data)
{
  (void)data;
  (void)handle;
  // handling of generic level move, update the lightbulb state
  saturation_level_move_request();
}

static void hsl_saturation_level_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                              void *data)
{
  (void)data;
  (void)handle;
  // transition for a saturation generic level request has completed,
  //update the lightbulb state
  saturation_level_transition_complete();
}

static void hsl_saturation_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                                        void *data)
{
  (void)data;
  (void)handle;
  // transition for a hsl saturation request has completed,
  // update the lightbulb state
  hsl_saturation_transition_complete();
}

static void hsl_transition_complete_timer_cb(sl_simple_timer_t *handle,
                                             void *data)
{
  (void)data;
  (void)handle;
  // transition for a hsl request has completed, update the lightbulb state
  hsl_transition_complete();
}

static void hsl_delayed_hue_level_timer_cb(sl_simple_timer_t *handle,
                                           void *data)
{
  (void)data;
  (void)handle;
  // delay for a hue generic level request has passed,
  // now process the request
  delayed_hue_level_request();
}

static void hsl_delayed_hsl_hue_timer_cb(sl_simple_timer_t *handle,
                                         void *data)
{
  (void)data;
  (void)handle;
  // delay for a hsl hue request has passed, now process the request
  delayed_hsl_hue_request();
}

static void hsl_delayed_saturation_level_timer_cb(sl_simple_timer_t *handle,
                                                  void *data)
{
  (void)data;
  (void)handle;
  // delay for a saturation generic level request has passed,
  // now process the request
  delayed_saturation_level_request();
}

static void hsl_delayed_hsl_saturation_timer_cb(sl_simple_timer_t *handle,
                                                void *data)
{
  (void)data;
  (void)handle;
  // delay for a hsl saturation request has passed, now process the request
  delayed_hsl_saturation_request();
}

static void hsl_delayed_hsl_request_timer_cb(sl_simple_timer_t *handle,
                                             void *data)
{
  (void)data;
  (void)handle;
  // delay for a hsl request has passed, now process the request
  delayed_hsl_request();
}

static void hsl_state_store_timer_cb(sl_simple_timer_t *handle,
                                     void *data)
{
  (void)data;
  (void)handle;
  // save the lightbulb state
  lightbulb_state_store();
}

/** @} (end addtogroup HSL_SERVER) */
