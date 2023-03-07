/***************************************************************************//**
 * @file
 * @brief app_cli.c
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
#include "app_process.h"
#include "app_log.h"
#include "sl_cli.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Helps to print out the variable name for printf functions
#define get_name(var)  #var

/// CLI warning for not proper value settings
#define PARAM_WARNING "%s value was out of limit, value will be set to %d\n"

/// CLI warning for setting energy mode under EM1 level
#define SLEEP_WARNING "CLI won't work under EM1 level, restart will be needed\n"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Function to set the sleep level in IDLE mode
 *
 * @param arguments: contains the sleep level for the device
 ******************************************************************************/
void cli_idle(sl_cli_command_arg_t *arguments)
{
  uint8_t energy_mode = 0;
  energy_mode = sl_cli_get_argument_uint8(arguments, 0);
  if (energy_mode > 3) {
    energy_mode = 3;
    app_log_info(PARAM_WARNING, get_name(energy_mode), energy_mode);
  }
  sleep_mode = energy_mode;
  if (sleep_mode > 1) {
    app_log_info(SLEEP_WARNING);
  }
  set_next_state(S_SET_IDLE);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the device to CW mode with specific sleep level
 *
 * @param arguments: contains the sleep level for the device
 ******************************************************************************/
void cli_cw(sl_cli_command_arg_t *arguments)
{
  uint8_t energy_mode = 0;
  energy_mode = sl_cli_get_argument_uint8(arguments, 0);
  if (energy_mode > 1) {
    energy_mode = 1;
    app_log_info(PARAM_WARNING, get_name(energy_mode), energy_mode);
  }
  sleep_mode = energy_mode;
  if (sleep_mode > 1) {
    app_log_info(SLEEP_WARNING);
  }
  set_next_state(S_CW);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the device to RX mode with specific sleep level
 *
 * @param arguments: contains the sleep level for the device
 ******************************************************************************/
void cli_rx(sl_cli_command_arg_t *arguments)
{
  uint8_t energy_mode = 0;
  energy_mode = sl_cli_get_argument_uint8(arguments, 0);
  if (energy_mode > 1) {
    energy_mode = 1;
    app_log_info(PARAM_WARNING, get_name(energy_mode), energy_mode);
  }
  sleep_mode = energy_mode;
  if (sleep_mode > 1) {
    app_log_info(SLEEP_WARNING);
  }
  set_next_state(S_RX);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the device to periodic TX mode with specific sleep level
 *
 * @param arguments:
 *                   - time in us to sleep
 *                   - sleep level
 ******************************************************************************/
void cli_periodic_tx(sl_cli_command_arg_t *arguments)
{
  uint8_t energy_mode = 0;
  energy_mode = sl_cli_get_argument_uint8(arguments, 1);
  sleep_period = sl_cli_get_argument_uint32(arguments, 0);
  if (energy_mode > 3) {
    energy_mode = 3;
    app_log_info(PARAM_WARNING, get_name(energy_mode), energy_mode);
  }
  sleep_mode = energy_mode;
  if (sleep_mode > 1) {
    app_log_info(SLEEP_WARNING);
  }

  init_needed = true;
  packet_sending = false;
  set_next_state(S_PERIODIC_TX);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the device to periodic RX mode with specific sleep level
 *
 * @param arguments:
 *                   - time in us to sleep
 *                   - time in us to be in RX mode
 *                   - sleep level
 ******************************************************************************/
void cli_periodic_rx(sl_cli_command_arg_t *arguments)
{
  uint8_t energy_mode = 0;
  sleep_period = sl_cli_get_argument_uint32(arguments, 0);
  rx_on_period = sl_cli_get_argument_uint32(arguments, 1);
  energy_mode = sl_cli_get_argument_uint8(arguments, 2);

  if (energy_mode > 3) {
    energy_mode = 3;
    app_log_info(PARAM_WARNING, get_name(energy_mode), energy_mode);
  }
  sleep_mode = energy_mode;
  if (sleep_mode > 1) {
    app_log_info(SLEEP_WARNING);
  }

  init_needed = true;
  rx_ended = true;
  set_next_state(S_PERIODIC_RX);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the TX power level of the device
 *
 * @param arguments: power level [raw]
 ******************************************************************************/
void cli_set_power_raw(sl_cli_command_arg_t *arguments)
{
  is_raw = true;
  power_raw = sl_cli_get_argument_int32(arguments, 0);
  set_next_state(S_SET_POWER_LEVEL);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to set the TX power level of the device
 *
 * @param arguments: power level [dBm]
 ******************************************************************************/
void cli_set_power(sl_cli_command_arg_t *arguments)
{
  is_raw = false;
  #ifdef SL_RAIL_UTIL_PA_CONFIG_HEADER
  power_deci_dbm = sl_cli_get_argument_int32(arguments, 0);
  #else
  app_log_info("dBm power selection requires the pa-conversions plugin");
  #endif
  set_next_state(S_SET_POWER_LEVEL);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to get the TX power level of the device in dBm units
 *
 * @param arguments: None
 ******************************************************************************/
void cli_get_power(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  is_raw = false;
  set_next_state(S_GET_POWER_LEVEL);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Function to get the TX power level of the device in raw units
 *
 * @param arguments: None
 ******************************************************************************/
void cli_get_power_raw(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  is_raw = true;
  set_next_state(S_GET_POWER_LEVEL);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
