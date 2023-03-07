/***************************************************************************//**
 * @file
 * @brief app_button.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_component_catalog.h"
#include "app_button.h"
#include "sl_simple_button_instances.h"
#include "sl_sleeptimer.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Time to wait before switching to long press in milliseconds
#define BUTTON_LONG_PRESS_FIRST_WAIT_MS  ((uint16_t) 2000)
/// Time between 2 long press happening in milliseconds
#define BUTTON_LONG_PRESST_WAIT_MS ((uint16_t) 500)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/// Callback to manage long press, clears button_X_wait_for_release
static void button_long_press_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Boolean flags for btn0 and btn1 to manage press in application
static bool button_0_pressed = false;
static bool button_1_pressed = false;
/// Boolean flags for blocking longer press is viewed as more presses
static bool button_0_wait_for_release = false;
static bool button_1_wait_for_release = false;
/// Timers for long press which will be managed as pressing the button
/// in every @BUTTON_LONG_PRESST_WAIT_MS millisecond
static sl_sleeptimer_timer_handle_t button_0_long_press_timer;
static sl_sleeptimer_timer_handle_t button_1_long_press_timer;

/// Delay should be added as ticks so these variables are needed to store the
/// calculated values
static uint32_t button_0_delay = 0;
static uint32_t button_1_delay = 0;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Init function for internal_app_init, initialize the button timers' times.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void init_button_delay(void)
{
  button_0_delay = sl_sleeptimer_ms_to_tick(BUTTON_LONG_PRESS_FIRST_WAIT_MS);
  button_1_delay = sl_sleeptimer_ms_to_tick(BUTTON_LONG_PRESS_FIRST_WAIT_MS);
}

/*******************************************************************************
 * Gets the state of the polled button and clears it after.
 *
 * @param button_number The selected button's number
 * @return The state of the selected button
 ******************************************************************************/
bool get_and_clear_button_state(uint8_t button_number)
{
  bool button_state = false;
  if (button_number == BUTTON_0) {
    button_state = button_0_pressed;
    button_0_pressed = false;
  } else if (button_number == BUTTON_1) {
    button_state = button_1_pressed;
    button_1_pressed = false;
  } else {
    button_state = false;
  }
  return button_state;
}

/*******************************************************************************
 * Button callback, called if any button is pressed or released.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == &sl_button_btn0) {
      if (!button_0_wait_for_release) {
        button_0_pressed = true;
        button_0_wait_for_release = true;
        sl_sleeptimer_start_timer(&button_0_long_press_timer,
                                  button_0_delay, button_long_press_callback,
                                  NULL, 0, 0);
      }
    } else if (handle == &sl_button_btn1) {
      if (!button_1_wait_for_release) {
        button_1_pressed = true;
        button_1_wait_for_release = true;
        sl_sleeptimer_start_timer(&button_1_long_press_timer,
                                  button_1_delay, button_long_press_callback,
                                  NULL, 0, 0);
      }
    }
  } else {
    if (handle == &sl_button_btn0) {
      button_0_wait_for_release = false;
      sl_sleeptimer_stop_timer(&button_0_long_press_timer);
      button_0_delay = sl_sleeptimer_ms_to_tick(
        BUTTON_LONG_PRESS_FIRST_WAIT_MS);
    } else if (handle == &sl_button_btn1) {
      button_1_wait_for_release = false;
      sl_sleeptimer_stop_timer(&button_1_long_press_timer);
      button_1_delay = sl_sleeptimer_ms_to_tick(
        BUTTON_LONG_PRESS_FIRST_WAIT_MS);
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Timer expiration callback for the delay function.
 *
 * @param handle Pointer to handle to timer.
 * @param data Pointer to delay flag.
 ******************************************************************************/
static void button_long_press_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)data;  // Unused parameter.

  if ( handle == &button_0_long_press_timer) {
    button_0_wait_for_release = false;
    button_0_delay = sl_sleeptimer_ms_to_tick(BUTTON_LONG_PRESST_WAIT_MS);
    sl_button_on_change(&sl_button_btn0);
  } else if (handle == &button_1_long_press_timer) {
    button_1_wait_for_release = false;
    button_1_delay = sl_sleeptimer_ms_to_tick(BUTTON_LONG_PRESST_WAIT_MS);
    sl_button_on_change(&sl_button_btn1);
  }
}
