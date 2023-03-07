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

/* C Standard Library headers */
#include <stdlib.h>
#include <stdio.h>

#include "em_common.h"
#include "sl_status.h"

#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include "sl_btmesh_ctl_server.h"
#include "sl_btmesh_ctl_signal_transition_handler.h"
#include "sl_btmesh_ctl_server_config.h"

#define SIGNED_INT_16BIT_MAX_POSITIVE     (0x7FFF)
#define FIXED_POINT_Q15_FRAC_BITS         (15)
#define DELTA_UV_SIGNIFICANT_DIGITS_MUL   (100)

/***************************************************************************//**
 * @addtogroup Lighting Level Transition Handler
 * @{
 ******************************************************************************/

#define NO_FLAGS              0       ///< No flags used for message
#define IMMEDIATE             0       ///< Immediate transition time is 0 seconds
#define NO_CALLBACK_DATA      (void *)NULL   // Callback has not parameters
#define HIGH_PRIORITY         0              // High Priority

/// current temperature level
static uint16_t current_temperature = SL_BTMESH_CTL_SERVER_DEFAULT_TEMPERATURE_CFG_VAL;
/// starting level of temperature transition
static uint16_t start_temperature;
/// target level of temperature transition
static uint16_t target_temperature;

/// current delta UV level
static int16_t current_deltauv;
/// starting level of delta UV transition
static int16_t start_deltauv;
/// target level of delta UV transition
static int16_t target_deltauv;

/// temperature transition time in timer ticks
static uint32_t temp_transtime_ticks;
/// time elapsed from temperature transition start
static uint32_t temp_transtime_elapsed;
/// non-zero if temperature transition is active
static uint8_t temp_transitioning;

static sl_simple_timer_t transition_timer;

// Timer callbacks
static void transition_timer_cb(sl_simple_timer_t *timer, void *data);

////////////////////////////////////////////////////////////////////////////////
//    Lighting  Callbacks                                                     //
////////////////////////////////////////////////////////////////////////////////

SL_WEAK void sl_btmesh_lighting_color_pwm_cb(uint16_t color)
{
  (void)color;
}

SL_WEAK void sl_btmesh_ctl_on_ui_update(uint16_t temperature,
                                        uint16_t deltauv)
{
  (void)temperature;
  (void)deltauv;
}

/***************************************************************************//**
 * Handler for Transition Timer, which manages LEDs transitions.
 ******************************************************************************/
static void transition_timer_cb(sl_simple_timer_t *timer, void *data)
{
  (void)data;
  (void)timer;
  // Initialize the variable to UI update period in order to trigger a UI update
  // at the beginning of the transition.
  static uint16_t time_elapsed_since_ui_update = SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL;

  if (!temp_transitioning) {
    sl_status_t sc = sl_simple_timer_stop(&transition_timer);
    app_assert_status_f(sc, "Failed to stop Periodic Level Transition Timer\n");
    return;
  } else {
    temp_transtime_elapsed++;

    if (temp_transtime_elapsed >= temp_transtime_ticks) {
      // transition complete
      temp_transitioning = 0;
      current_temperature = target_temperature;
      current_deltauv = target_deltauv;

      // Set the variable to UI update period in order to trigger a UI update
      // at the beginning of the next transition.
      time_elapsed_since_ui_update = SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL;

      // Trigger a UI update in order to provide the target values at the end
      // of the current transition
      sl_btmesh_ctl_on_ui_update(current_temperature, current_deltauv);
    } else {
      // calculate current temperature based on elapsed transition time
      if (target_temperature >= start_temperature) {
        current_temperature = start_temperature
                              + (target_temperature - start_temperature)
                              * (uint64_t)temp_transtime_elapsed
                              / temp_transtime_ticks;
      } else {
        current_temperature = start_temperature
                              - (start_temperature - target_temperature)
                              * (uint64_t)temp_transtime_elapsed
                              / temp_transtime_ticks;
      }

      if (target_deltauv >= start_deltauv) {
        current_deltauv = start_deltauv
                          + (target_deltauv - start_deltauv)
                          * (uint64_t)temp_transtime_elapsed
                          / temp_transtime_ticks;
      } else {
        current_deltauv = start_deltauv
                          - (start_deltauv - target_deltauv)
                          * (uint64_t)temp_transtime_elapsed
                          / temp_transtime_ticks;
      }

      // When transition is ongoing generate an event to application once every
      // SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL ms because the event is used to update
      // display status and therefore the rate should not be too high
      time_elapsed_since_ui_update += SL_BTMESH_CTL_SERVER_PWM_UPDATE_PERIOD_CFG_VAL;

      if (SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL <= time_elapsed_since_ui_update) {
        time_elapsed_since_ui_update -= SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL;
        sl_btmesh_ctl_on_ui_update(current_temperature, current_deltauv);
      }
    }
  }

  sl_btmesh_lighting_color_pwm_cb(current_temperature);
}

/*******************************************************************************
 * Set LED temperature and delta UV in given transition time.
 *
 * @param[in] temperature    Temperature of color.
 * @param[in] deltauv        Delta UV value.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void sl_btmesh_ctl_set_temperature_deltauv_level(uint16_t temperature,
                                                 int16_t deltauv,
                                                 uint32_t transition_ms)
{
  if (temperature < SL_BTMESH_CTL_SERVER_MINIMUM_TEMPERATURE_CFG_VAL) {
    temperature = SL_BTMESH_CTL_SERVER_MINIMUM_TEMPERATURE_CFG_VAL;
  } else if (temperature > SL_BTMESH_CTL_SERVER_MAXIMUM_TEMPERATURE_CFG_VAL) {
    temperature = SL_BTMESH_CTL_SERVER_MAXIMUM_TEMPERATURE_CFG_VAL;
  }

  if (transition_ms == 0) {
    current_temperature = temperature;
    current_deltauv = deltauv;

    sl_btmesh_lighting_color_pwm_cb(current_temperature);

    /* if a transition was in progress, cancel it */
    if (temp_transitioning) {
      temp_transitioning = 0;
      sl_status_t sc = sl_simple_timer_stop(&transition_timer);
      app_assert_status_f(sc, "Failed to stop Periodic Level Transition Timer\n");
    }
    sl_btmesh_ctl_on_ui_update(current_temperature, current_deltauv);
    return;
  }

  temp_transtime_ticks = transition_ms;

  start_temperature = current_temperature;
  target_temperature = temperature;

  start_deltauv = current_deltauv;
  target_deltauv = deltauv;

  temp_transtime_elapsed = 0;
  temp_transitioning = 1;

  // enabling timer IRQ -> the temperature is adjusted in timer interrupt
  // gradually until target temperature is reached.
  sl_status_t sc = sl_simple_timer_start(&transition_timer,
                                         SL_BTMESH_CTL_SERVER_PWM_UPDATE_PERIOD_CFG_VAL,
                                         transition_timer_cb,
                                         NO_CALLBACK_DATA,
                                         true);
  app_assert_status_f(sc, "Failed to start periodic Transition Timer\n");

  return;
}

/***************************************************************************//**
 * Utility function to print the delta UV raw value into the passed character
 * buffer in the <sign>X.XX format.
 *
 * @param[out] buffer      Character buffer where the formatted delta UV
 *                         is stored
 * @param[in] buffer_size  Size of the character buffer
 * @param[in] deltauv_raw  Raw value of the Delta UV CTL parameter
 *
 * @return same as snprintf
 ******************************************************************************/
int sl_btmesh_ctl_server_snprint_deltauv(char *buffer,
                                         uint16_t buffer_size,
                                         uint16_t deltauv_raw)
{
  // Additive variable for rounding to 0.01 precision (2^15 / 100 / 2)
  const uint32_t round_value = ((1UL << FIXED_POINT_Q15_FRAC_BITS) / 2)
                               / DELTA_UV_SIGNIFICANT_DIGITS_MUL;
  uint32_t raw = deltauv_raw;
  uint16_t integer_part, fractional_part;
  char sign_char;

  // The deltauv_raw is a 2s complement signed fixed point value 16bit (Q15)
  // which uses 15 bit to code the fractional part therefore:
  // delta UV raw: 0x8000 => delta UV: -1.0000000 -->   -1.00
  // delta UV raw: 0x7fff => delta UV: +0,9999695 --> ~ +1.00

  if (SIGNED_INT_16BIT_MAX_POSITIVE < raw) {
    // Convert the negative value to positive and round it
    raw = ( (1UL << 16) - raw) + round_value;

    // Store that the value was negative
    sign_char = '-';
  } else {
    // Round the raw value to 0.01 precision
    raw += round_value;

    // Store that the value was not negative
    sign_char = ' ';
  }

  // Division by 2^15 to calculate the integer part
  integer_part = raw >> FIXED_POINT_Q15_FRAC_BITS;

  // Calculate the significant number of fractional decimal digits
  fractional_part = DELTA_UV_SIGNIFICANT_DIGITS_MUL
                    * (raw & SIGNED_INT_16BIT_MAX_POSITIVE)
                    >> FIXED_POINT_Q15_FRAC_BITS;

  // Handle the corner case when a negative value was rounded to zero
  if ((integer_part == 0) && (fractional_part == 0)) {
    sign_char = ' ';
  }

  // Print the formatted delta UV value to the character buffer
  return snprintf(buffer, buffer_size, "%c%d.%02d",
                  sign_char,
                  integer_part,
                  fractional_part);
}

/*******************************************************************************
 * Function for retrieving actual temperature.
 *
 * @return Actual temperature level.
 ******************************************************************************/
uint16_t sl_btmesh_get_temperature(void)
{
  return(current_temperature);
}

/*******************************************************************************
 * Function for retrieving actual delta UV.
 *
 * @return Actual delta UV level.
 ******************************************************************************/
uint16_t sl_btmesh_get_deltauv(void)
{
  return(current_deltauv);
}

/** @} (end addtogroup Lighting Level Transition Handler) */
