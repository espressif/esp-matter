/***************************************************************************//**
 * @file
 * @brief app_process.c
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
#include "app_init.h"
#include "app_process.h"
#if defined(SL_CATALOG_GLIB_PRESENT)
#include "app_graphics.h"
#endif
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
#include "app_button.h"
#endif
#include "app_measurement.h"
#include "app_menu.h"

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
#include "app_bluetooth.h"
#include "gatt_db.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
///Define to allow detailed RAIL error printout
//#define DEBUG_RAIL_EVENTS
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// State machine variable to hold the current state
state_t state = INFO_SCREEN;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// As the LCD don't need to be refresh always, use this to set when to refresh
/// the hole LCD display
static bool refresh_screen = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * State set for other part of the application.
 *
 * @param next_state What should be the next state in the state machine
 * @returns None
 ******************************************************************************/
void set_next_state(state_t next_state)
{
  state = next_state;
}

/*******************************************************************************
 * LCD screen needs update.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void request_refresh_screen(void)
{
  refresh_screen = true;
}

/*******************************************************************************
 * The function is used for Application logic.
 * @brief Application state machine, called infinitely
 *
 * @param RAIL_Handle_t Null pointer to keep stlye with other sample apps
 * @returns None
 ******************************************************************************/
void app_process_action(RAIL_Handle_t pointer)
{
  (void)pointer;
  switch (state) {
    case INFO_SCREEN:
      if (refresh_screen) {
#if defined(SL_CATALOG_GLIB_PRESENT)
        graphics_draw_init_screen();
#endif
        refresh_screen = false;
      }
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      if (get_and_clear_button_state(BUTTON_0) || get_and_clear_button_state(BUTTON_1)) {
        end_init_timer();
        set_next_state(MENU_SCREEN);
        refresh_screen = true;
      }
#endif
      break;
    case MENU_SCREEN:
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      if (get_and_clear_button_state(BUTTON_1)) {
        menu_next_item();
        refresh_screen = true;
      } else if (get_and_clear_button_state(BUTTON_0)) {
        menu_item_action();
        refresh_screen = true;
      }
#endif
      if (refresh_screen) {
        if (state == MENU_SCREEN) {
#if defined(SL_CATALOG_GLIB_PRESENT)
          graphics_draw_menu();
#endif
          refresh_screen = false;
        }
      }
      break;
    case START_MEASURMENT:
      range_test_init();
      refresh_screen = false;
      // to prevent other handlers to occupy the radio
      set_all_radio_handlers_to_idle();
      if (range_test_settings.radio_mode == RADIO_MODE_RX) {
        set_next_state(RECEIVE_MEASURMENT);
        receive_setup_radio();
        range_test_reset_values();
#if defined(SL_CATALOG_GLIB_PRESENT)
        graphics_draw_rx_screen();
#endif
      } else if (range_test_settings.radio_mode == RADIO_MODE_TX) {
        set_next_state(SEND_MEASURMENT);
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        if (!is_bluetooth_connected()) {
          deactivate_bluetooth();
        }
#endif
#if defined(SL_CATALOG_GLIB_PRESENT)
        graphics_draw_tx_screen();
#endif
      }
#if defined(SL_CATALOG_KERNEL_PRESENT)
      {
        app_task_notify();
      }
#endif
      break;
    case RECEIVE_MEASURMENT:
      refresh_screen = receive_measurment();
      if (refresh_screen) {
#if defined(SL_CATALOG_GLIB_PRESENT)
        graphics_draw_rx_screen();
#endif
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        advertise_received_data(range_test_measurement.rssi_latch_value,
                                range_test_measurement.packets_received_counter,
                                range_test_measurement.packets_received_correctly);
#endif
      }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
      manage_bluetooth_restart();
#endif
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      if (get_and_clear_button_state(BUTTON_1)) {
        stop_recive_measurement();
        set_next_state(MENU_SCREEN);
        refresh_screen = true;
      }
#endif
      break;
    case SEND_MEASURMENT:
      if (range_test_measurement.tx_is_running) {
        refresh_screen = send_measurment();
      }
      if (refresh_screen) {
#if defined(SL_CATALOG_GLIB_PRESENT)
        graphics_draw_tx_screen();
#endif
      }
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      if (get_and_clear_button_state(BUTTON_1)) {
        set_next_state(MENU_SCREEN);
        refresh_screen = true;
        range_test_measurement.tx_is_running = false;
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        add_bluetooth_indication(gattdb_isRunning);
#endif
        set_all_radio_handlers_to_idle();
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        if (!is_bluetooth_connected()) {
          activate_bluetooth();
        }
#endif
      } else if (get_and_clear_button_state(BUTTON_0)) {
        range_test_measurement.tx_is_running = !range_test_measurement.tx_is_running;
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        add_bluetooth_indication(gattdb_isRunning);
#endif
        range_test_reset_values();
        refresh_screen = true;
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
        if (range_test_measurement.tx_is_running) {
          if (!is_bluetooth_connected()) {
            deactivate_bluetooth();
          }
        } else {
          if (!is_bluetooth_connected()) {
            activate_bluetooth();
          }
        }
#endif
      }
#endif
      break;
    default:
      break;
  }
  print_log();
#if defined(DEBUG_RAIL_EVENTS)
  print_errors_from_rail_handler();
#endif
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  send_bluetooth_indications();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
