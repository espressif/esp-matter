/***************************************************************************//**
 * @file
 * @brief Button Press source code
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_sleeptimer.h"
#include "app_button_press.h"
#include "app_button_press_config.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT

/***************************************************************************//**
 * @addtogroup ButtonPress
 * @{
 ******************************************************************************/

/// Index of button ID parameter from CLI
#define BUTTON_ID_PARAM_IDX         0
/// Index of button event duration parameter from CLI
#define DURATION_PARAM_IDX          1

/// State of the component, default is disabled
static bool disabled = true;

/// Structure type for button status
typedef struct {
  uint32_t timestamp;
  uint8_t state;
} button_state_t;

/// Button states
static button_state_t buttons[SL_SIMPLE_BUTTON_COUNT];

/***************************************************************************//**
 * Init function for button press
 ******************************************************************************/
void app_button_press_init(void)
{
  // Iterate over buttons and clear timestamps and states
  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    buttons[i].state = APP_BUTTON_PRESS_NONE;
    buttons[i].timestamp = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Weak implementation of Callbacks                                           //
////////////////////////////////////////////////////////////////////////////////

SL_WEAK void app_button_press_cb(uint8_t button, uint8_t duration)
{
  (void)button;
  (void)duration;
}

/***************************************************************************//**
 * This is a callback function that is invoked each time a GPIO interrupt
 * in one of the pushbutton inputs occurs.
 *
 * @param[in] handle Pointer to button instance
 *
 * @note This function is called from ISR context and therefore it is
 *       not possible to call any BGAPI functions directly. The button state
 *       of the instance is updated based on the state change. The state is
 *       updated only after button release and it depends on how long the
 *       button was pressed. The button state is handled by the main loop.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  uint32_t t_diff;
  // If disabled, do nothing
  if (disabled) {
    return;
  }
  // Iterate over buttons
  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    // If the handle is applicable
    if (SL_SIMPLE_BUTTON_INSTANCE(i) == handle) {
      // If button is pressed, update ticks
      if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
        buttons[i].timestamp = sl_sleeptimer_get_tick_count();
      } else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
        // Check time difference
        t_diff = sl_sleeptimer_get_tick_count() - buttons[i].timestamp;
        // Set state flag according to the difference
        if (t_diff < sl_sleeptimer_ms_to_tick(SHORT_BUTTON_PRESS_DURATION)) {
          buttons[i].state = APP_BUTTON_PRESS_DURATION_SHORT;
        } else if (t_diff < sl_sleeptimer_ms_to_tick(MEDIUM_BUTTON_PRESS_DURATION)) {
          buttons[i].state = APP_BUTTON_PRESS_DURATION_MEDIUM;
        } else if (t_diff < sl_sleeptimer_ms_to_tick(LONG_BUTTON_PRESS_DURATION)) {
          buttons[i].state = APP_BUTTON_PRESS_DURATION_LONG;
        } else {
          buttons[i].state = APP_BUTTON_PRESS_DURATION_VERYLONG;
        }
      } else {
        // Disabled state
        // Do nothing
      }
    }
  }
}

/***************************************************************************//**
 * Step function for button press
 ******************************************************************************/
void app_button_press_step(void)
{
  // Iterate over buttons
  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    // If the button is pressed
    if (buttons[i].state != APP_BUTTON_PRESS_NONE) {
      // Call callback
      app_button_press_cb(i, buttons[i].state);
      // Clear state
      buttons[i].state = APP_BUTTON_PRESS_NONE;
    }
  }
}

/***************************************************************************//**
 * Enable button press
 ******************************************************************************/
void app_button_press_enable(void)
{
  // Check if buttons are pressed now
  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    if (sl_button_get_state(SL_SIMPLE_BUTTON_INSTANCE(i))
        == SL_SIMPLE_BUTTON_PRESSED) {
      // Set timestamp
      buttons[i].timestamp = sl_sleeptimer_get_tick_count();
    }
  }

  // Clear disabled state
  disabled = false;
}

/***************************************************************************//**
 * Disable button press
 ******************************************************************************/
void app_button_press_disable(void)
{
  if (disabled) {
    return;
  }
  disabled = true;
  // Clear timestamps and states

  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    buttons[i].state = APP_BUTTON_PRESS_NONE;
    buttons[i].timestamp = 0;
  }
}

// -----------------------------------------------------------------------------
// Power manager related functions

/***************************************************************************//**
 * Checks if it is ok to sleep now
 ******************************************************************************/
bool app_button_press_is_ok_to_sleep(void)
{
  bool ret = true;

  // Return true if disabled or no unhandled press is present.
  if (disabled == false) {
    for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
      if (buttons[i].state != APP_BUTTON_PRESS_NONE) {
        ret = false;
        break;
      }
    }
  }
  return ret;
}
/***************************************************************************//**
 * Routine for power manager handler
 ******************************************************************************/
sl_power_manager_on_isr_exit_t app_button_press_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  // Wake if not disabled and there is an unhandled press.
  if (disabled == false) {
    for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
      if (buttons[i].state != APP_BUTTON_PRESS_NONE) {
        ret = SL_POWER_MANAGER_WAKEUP;
        break;
      }
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
// CLI related functions

#ifdef SL_CATALOG_CLI_PRESENT
/***************************************************************************//**
 * Command Line Interface callback implementation
 *
 * @param[in] arguments command line argument list
 ******************************************************************************/
void button_press_from_cli(sl_cli_command_arg_t *arguments)
{
  uint8_t button_id;
  uint8_t duration;
  button_id = sl_cli_get_argument_uint8(arguments, BUTTON_ID_PARAM_IDX);
  duration = sl_cli_get_argument_uint8(arguments, DURATION_PARAM_IDX);
  if (duration > APP_BUTTON_PRESS_DURATION_VERYLONG) {
    duration = APP_BUTTON_PRESS_DURATION_VERYLONG;
  }
  if (button_id >= SL_SIMPLE_BUTTON_COUNT) {
    button_id = SL_SIMPLE_BUTTON_COUNT - 1;
  }
  app_button_press_cb(button_id, duration);
}
#endif // SL_CATALOG_CLI_PRESENT

/** @} (end addtogroup ButtonPress) */
