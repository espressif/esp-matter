/***************************************************************************//**
 * @file
 * @brief Lighting Level Transition Handler Module
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

#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include "sl_btmesh_lighting_level_transition_handler.h"
#include "sl_btmesh_lighting_server_config.h"

/***************************************************************************//**
 * @addtogroup Lighting Level Transition Handler
 * @{
 ******************************************************************************/

/// No flags used for message
#define NO_FLAGS              0
/// Immediate transition time is 0 seconds
#define IMMEDIATE             0
/// Callback has not parameters
#define NO_CALLBACK_DATA      (void *)NULL
/// High Priority
#define HIGH_PRIORITY         0

/// current lightness level
static uint16_t current_level;
/// starting level of lightness transition
static uint16_t start_level;
/// target level of lightness transition
static uint16_t target_level;

/// lightness transition time in timer ticks
static uint32_t level_transtime_ticks;
/// time elapsed from lightness transition start
static uint32_t level_transtime_elapsed;
/// non-zero if lightness transition is active
static uint8_t level_transitioning;

static sl_simple_timer_t transition_timer;

// Timer callbacks
static void transition_timer_cb(sl_simple_timer_t *handle,
                                void *data);

////////////////////////////////////////////////////////////////////////////////
//    Lighting  Callbacks                                                     //
////////////////////////////////////////////////////////////////////////////////

SL_WEAK void sl_btmesh_lighting_level_pwm_cb(uint16_t level)
{
  (void)level;
}

SL_WEAK void sl_btmesh_lighting_server_on_ui_update(uint16_t lightness_level)
{
  (void) lightness_level;
}

/***************************************************************************//**
 * Timer Callback for LEDs transitions.
 ******************************************************************************/
static void transition_timer_cb(sl_simple_timer_t *handle,
                                void *data)
{
  (void)data;
  (void)handle;

  // Initialize the variable to UI update period in order to trigger a UI update
  // at the beginning of the transition.
  static uint16_t time_elapsed_since_ui_update =
    SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL;

  if (!level_transitioning) {
    sl_status_t sc = sl_simple_timer_stop(&transition_timer);
    app_assert_status_f(sc, "Failed to stop Periodic Level Transition Timer\n");
    return;
  } else {
    level_transtime_elapsed++;

    if (level_transtime_elapsed >= level_transtime_ticks) {
      // transition complete
      level_transitioning = 0;
      current_level = target_level;

      // Set the variable to UI update period in order to trigger a UI update
      // at the beginning of the next transition.
      time_elapsed_since_ui_update = SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL;

      // Trigger a UI update in order to provide the target values at the end
      // of the current transition
      sl_btmesh_lighting_server_on_ui_update(current_level);
    } else {
      // calculate current PWM duty cycle based on elapsed transition time
      if (target_level >= start_level) {
        current_level = start_level
                        + (target_level - start_level)
                        * (uint64_t)level_transtime_elapsed
                        / level_transtime_ticks;
      } else {
        current_level = start_level
                        - (start_level - target_level)
                        * (uint64_t)level_transtime_elapsed
                        / level_transtime_ticks;
      }

      // When transition is ongoing generate an event to application once every
      // SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL ms because the event is used to update display
      // status and therefore the rate should not be too high
      time_elapsed_since_ui_update += SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL;

      if (SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL <= time_elapsed_since_ui_update) {
        time_elapsed_since_ui_update -= SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL;
        sl_btmesh_lighting_server_on_ui_update(current_level);
      }
    }
  }

  sl_btmesh_lighting_level_pwm_cb(current_level);
}

/*******************************************************************************
 * Set LED lightness level in given transition time.
 *
 * @param[in] level          Lightness level.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void sl_btmesh_lighting_set_level(uint16_t level, uint32_t transition_ms)
{
  if (transition_ms == 0) {
    current_level = level;

    sl_btmesh_lighting_level_pwm_cb(current_level);

    /* if a transition was in progress, cancel it */
    if (level_transitioning) {
      level_transitioning = 0;
      sl_status_t sc = sl_simple_timer_stop(&transition_timer);
      app_assert_status_f(sc, "Failed to stop Periodic Level Transition Timer\n");
    }
    sl_btmesh_lighting_server_on_ui_update(current_level);
    return;
  }

  level_transtime_ticks = transition_ms;

  start_level = current_level;
  target_level = level;

  level_transtime_elapsed = 0;
  level_transitioning = 1;

  // enabling timer IRQ -> the PWM level is adjusted in timer interrupt
  // gradually until target level is reached.
  sl_status_t sc = sl_simple_timer_start(&transition_timer,
                                         SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL,
                                         transition_timer_cb,
                                         NO_CALLBACK_DATA,
                                         true);
  app_assert_status_f(sc, "Failed to start periodic Transition Timer\n");

  return;
}

/*******************************************************************************
 * Set LEDs state. Possible states are defined in macros.
 *
 * @param[in] state  State to set.
 ******************************************************************************/
void  sl_btmesh_set_state(int state)
{
  static int toggle = 0;

  switch (state) {
    case LED_STATE_OFF:
      sl_btmesh_lighting_set_level(SL_BTMESH_LIGHTING_SERVER_PWM_MINIMUM_BRIGHTNESS_CFG_VAL, 0);
      break;
    case LED_STATE_ON:
      sl_btmesh_lighting_set_level(SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL, 0);
      break;
    case LED_STATE_PROV:
      if (++toggle % 2) {
        sl_btmesh_lighting_set_level(SL_BTMESH_LIGHTING_SERVER_PWM_MINIMUM_BRIGHTNESS_CFG_VAL, 0);
      } else {
        sl_btmesh_lighting_set_level(SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL, 0);
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Function for retrieving actual lightness level.
 *
 * @return Actual lightness level.
 ******************************************************************************/
uint16_t sl_btmesh_get_level(void)
{
  return(current_level);
}

/** @} (end addtogroup Lighting Level Transition Handler) */
