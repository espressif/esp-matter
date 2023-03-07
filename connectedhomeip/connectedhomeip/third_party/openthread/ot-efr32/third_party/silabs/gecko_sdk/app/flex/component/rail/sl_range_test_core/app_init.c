/***************************************************************************//**
 * @file
 * @brief app_init.c
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
#include "em_common.h"
#include "sl_sleeptimer.h"
#include "app_init.h"
#include "app_process.h"
#include "app_menu.h"
#include "app_log.h"
#include "app_measurement.h"
#if defined(SL_CATALOG_GLIB_PRESENT)
#include "app_graphics.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
#include "rail_config.h"
#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM)
#include "sl_rail_util_pa_config.h"
#include "rail_chip_specific.h"
#endif
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/// Timer expiration callback for the delay function.
static void init_screen_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Timer for the delay of the showing init screen
static sl_sleeptimer_timer_handle_t init_screen_timer;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Print sample app name
 *****************************************************************************/
SL_WEAK void print_sample_app_name(const char* app_name)
{
  app_log_info("%s\n", app_name);
}

/*******************************************************************************
 * The function is used for application initialization.
 *
 * @param None
 * @returns RAIL_Handle_t Null pointer to keep stlye with other sample apps
 ******************************************************************************/
RAIL_Handle_t app_init(void)
{
  sl_status_t sleep_timer_status = 0;
#if defined(SL_CATALOG_GLIB_PRESENT)
  graphics_init();
#endif
  init_range_test_phys();
  menu_init();

  set_next_state(INFO_SCREEN);
  request_refresh_screen();

  sleep_timer_status = sl_sleeptimer_start_timer(&init_screen_timer,
                                                 sl_sleeptimer_ms_to_tick(3000), init_screen_timer_callback,
                                                 NULL, 0, 0);

  if (sleep_timer_status != 0) {
    app_log_error("Sleeptimer start failed with code %d",
                  sleep_timer_status);
  }

  // CLI info message
#ifdef SL_CATALOG_RANGE_TEST_STD_COMPONENT_PRESENT
#ifdef SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT
  print_sample_app_name("Range test STD DMP");
#else
  print_sample_app_name("Range test STD");
#endif
#else
#ifdef SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT
  print_sample_app_name("Range test DMP");
#else
  print_sample_app_name("Range test");
#endif
#endif

#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM) && !defined(HARDWARE_BOARD_HAS_EFF)
  _Static_assert(SL_RAIL_UTIL_PA_SELECTION_SUBGHZ == RAIL_TX_POWER_MODE_OFDM_PA,
                 "Please use the OFDM PA settings in the sl_rail_util_pa_config.h for OFDM phys.");
#endif
#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM) && RAIL_SUPPORTS_EFF && defined(HARDWARE_BOARD_HAS_EFF)
  _Static_assert(SL_RAIL_UTIL_PA_SELECTION_SUBGHZ >= RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM,
                 "Please use the OFDM PA for EFF settings in the sl_rail_util_pa_config.h for OFDM phys.");
#endif

  return (RAIL_Handle_t) 0x01;
}

/*******************************************************************************
 * The function is used for stopping the timer in the init block.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void end_init_timer(void)
{
  bool is_running = false;
  sl_status_t sleep_timer_status = 0;
  sleep_timer_status = sl_sleeptimer_is_timer_running(&init_screen_timer,
                                                      &is_running);
  if (sleep_timer_status != 0) {
    app_log_error("Sleeptimer state read failed with code %d", sleep_timer_status);
  }
  if (is_running) {
    sleep_timer_status = sl_sleeptimer_stop_timer(&init_screen_timer);
    if (sleep_timer_status != 0) {
      app_log_error("Sleeptimer stop failed with code %d", sleep_timer_status);
    }
  }
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
static void init_screen_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)data;  // Unused parameter.
  (void)handle;  // Unused parameter.

  set_next_state(MENU_SCREEN);
  request_refresh_screen();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}
