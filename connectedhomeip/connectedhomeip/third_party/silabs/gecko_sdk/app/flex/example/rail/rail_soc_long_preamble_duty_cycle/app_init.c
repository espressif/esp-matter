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
#include <stdint.h>
#include "sl_component_catalog.h"
#include "rail.h"
#include "em_device.h"
#include "rail_types.h"
#include "rail_config.h"
#include "sl_rail_util_init.h"
#include "app_init.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "app_log.h"
#include "sl_duty_cycle_config.h"
#include "sl_duty_cycle_utility.h"
#include "sl_power_manager.h"

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
  #include "app_graphics.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Config for the correct timing of the dutycycle API
RAIL_RxDutyCycleConfig_t duty_cycle_config = {
  .delay = ((uint32_t) DUTY_CYCLE_OFF_TIME),
  .delayMode = RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC,
  .mode = RAIL_RX_CHANNEL_HOPPING_MODE_PREAMBLE_SENSE,
  .parameter = ((uint32_t) DUTY_CYCLE_ON_TIME)
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

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

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  // For handling error codes
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;

  // To calculate proper preamble
  uint32_t bit_rate = 0;

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Get current bitrate
  bit_rate = RAIL_GetBitRate(rail_handle);

  // EFR32xG23 chips support the new RadioConfigurator Fast Preamble Settings
#if (_SILICON_LABS_32B_SERIES_2_CONFIG != 3) && (_SILICON_LABS_32B_SERIES_2_CONFIG != 4)  && (_SILICON_LABS_32B_SERIES_2_CONFIG != 5)
  // preamble length in bits
  uint16_t preamble_bit_length = 0;

  // Calculate preamble length from bitrate and time
  rail_status = calculate_preamble_bit_length_from_time(bit_rate, &duty_cycle_config, &preamble_bit_length);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("Invalid preamble length parameters\n");
  }

  rail_status = RAIL_SetTxAltPreambleLength(rail_handle, preamble_bit_length);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetTxAltPreambleLength failed with %d \n", rail_status);
  }
#else
  rail_status = RAIL_GetDefaultRxDutyCycleConfig(rail_handle, &duty_cycle_config);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_GetDefaultRxDutyCycleConfig failed with %d \n", rail_status);
  }
#endif

  // Turn OFF LEDs
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
  // LCD start
  graphics_init();
#endif

  // CLI info message
  print_sample_app_name("Long Preamble Duty Cycle");
#if (_SILICON_LABS_32B_SERIES_2_CONFIG != 3) && (_SILICON_LABS_32B_SERIES_2_CONFIG != 4)  && (_SILICON_LABS_32B_SERIES_2_CONFIG != 5)
  app_log_info("Preamble length %d for bitrate %d b/s with %d us off time\n", preamble_bit_length, bit_rate, duty_cycle_config.delay);
#else
  app_log_info("Bitrate %d b/s with %d us off time\n", bit_rate, duty_cycle_config.delay);
#endif

  // Allow state machine to run without interrupt
  set_first_run(true);

  // Config DutyCycle API
  RAIL_ConfigRxDutyCycle(rail_handle, &duty_cycle_config);

  // Enable duty cycle mode
  RAIL_EnableRxDutyCycle(rail_handle, true);

  // Start RX
  RAIL_StartRx(rail_handle, CHANNEL, NULL);

#if DUTY_CYCLE_ALLOW_EM2 == 1
  // EM2 sleep level
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
