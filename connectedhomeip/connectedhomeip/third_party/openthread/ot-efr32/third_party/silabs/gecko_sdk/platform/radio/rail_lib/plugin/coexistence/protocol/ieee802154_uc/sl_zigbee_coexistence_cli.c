/***************************************************************************//**
 * @file
 * @brief sl_zigbee_coexistence_cli.c
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_cli.h"
#include "sl_zigbee_debug_print.h"
#include "stack/include/ember-types.h"
#include "counters.h"
#include "coexistence-802154.h"

//-----------------------------------------------------------------------------
// Get PTA state (enabled/disabled)
// Console Command : "plugin coexistence get-pta-state"
// Console Response: "PTA is <ENABLED|DISABLED>"
void emberAfPluginCoexistenceGetPtaState(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  bool ptaState = sl_rail_util_coex_is_enabled();
  sl_zigbee_app_debug_print("PTA is %s", (ptaState ? "ENABLED" : "DISABLED"));
}

//-----------------------------------------------------------------------------
// Set PTA state (disabled=0/enabled=1)
// Console Command : "plugin coexistence set-pta-state <0|1>"
// Console Response: "PTA is <ENABLED|DISABLED>"
void emberAfPluginCoexistenceSetPtaState(sl_cli_command_arg_t *arguments)
{
  bool ptaState = (bool)sl_cli_get_argument_uint8(arguments, 0);
  sl_rail_util_coex_set_enable(ptaState);
  emberAfPluginCoexistenceGetPtaState(arguments);
}

//-----------------------------------------------------------------------------
#define PTA_OPTION_FIELDS 16
#define PTA_OPTION_LINE_MAX 80

static const char * const ptaHelp[PTA_OPTION_FIELDS + 1] = {
  "Field                             Bit Position  Size(bits)  Value",
  "RX retry timeout ms              ",
  "Disable ACK if nGRANT||RHO||nREQ ",
  "Abort mid TX if grant is lost    ",
  "TX request is high priority      ",
  "RX request is high priority      ",
  "RX retry request is high priority",
  "RX retry request is enabled      ",
  "Radio holdoff is enabled         ",
  "Reserved                         ",
  "Disable REQ (force holdoff)      ",
  "Synch MAC to GRANT (MAC holdoff) ",
  "REQ/PRI Assert (PreSync/AddrDet) ",
  "CCA/GRANT TX PRI Escalation Thres",
  "Reserved                         ",
  "MAC Fail TX PRI Escalation Thresh",
  "Reserved                         "
};

static const uint8_t ptaBitShift[PTA_OPTION_FIELDS + 1] = { 0, 8, 9, 10, 11, 12, 13,
                                                            14, 15, 16, 17, 18, 20, 23, 25, 27, 32 };
static const uint32_t ptaBitMask[PTA_OPTION_FIELDS] = { 0xFF, 0x1, 0x1, 0x1, 0x1,
                                                        0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0x7, 0x3, 0x3, 0x1F };

//-----------------------------------------------------------------------------
// Get ptaOptions and print hex value to console
// Console Command : "plugin coexistence get-pta-options"
// Console Response: "PTA Configuration Option: 0x<ptaOptions>"
void emberAfPluginCoexistenceGetPtaOptions(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  uint8_t i;
  uint32_t value;
  sl_rail_util_coex_options_t ptaOptions = sl_rail_util_coex_get_options();

  sl_zigbee_app_debug_print("PTA Configuration Option: 0x%4x", ptaOptions);

  sl_zigbee_app_debug_print("%s", ptaHelp[0]);
  for (i = 0; i < PTA_OPTION_FIELDS; i++) {
    value = (ptaOptions >> ptaBitShift[i]) & ptaBitMask[i];
    sl_zigbee_app_debug_print("%s %02d            %d          %d",
                              ptaHelp[i + 1],
                              ptaBitShift[i],
                              (ptaBitShift[i + 1] - ptaBitShift[i]),
                              value);
  }
  sl_zigbee_app_debug_print("");
}

//-----------------------------------------------------------------------------
// Set ptaOptions from console
// Console Command : "plugin coexistence set-pta-options 0x<ptaOptions>"
// Console Response: "PTA Configuration Option: 0x<ptaOptions>"
void emberAfPluginCoexistenceSetPtaOptions(sl_cli_command_arg_t *arguments)
{
  sl_rail_util_coex_options_t ptaOptions = (sl_rail_util_coex_options_t)sl_cli_get_argument_uint32(arguments, 0);

  sl_rail_util_coex_set_options(ptaOptions);
  emberAfPluginCoexistenceGetPtaOptions(arguments);
}

//-----------------------------------------------------------------------------
// Get PWM state from console
// Console Command : "plugin coexistence get-pta-state"
// Console Response: "PTA PWM (ENABLE|DISABLED): <period> (PERIOD in 0.5ms),
//                             <duty-cycle> (%DC), <0|1> (<LOW|HIGH> PRIORITY)"
void emberAfPluginCoexistenceGetPwmState(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  uint8_t pwmPeriodHalfMs;
  uint8_t pwmPulseDc;
  bool pwmPriority;
  uint8_t getPwmStateVariable[3];

  getPwmStateVariable[0] = (uint8_t)sl_rail_util_coex_get_request_pwm_args()->req;
  getPwmStateVariable[1] = sl_rail_util_coex_get_request_pwm_args()->dutyCycle;
  getPwmStateVariable[2] = sl_rail_util_coex_get_request_pwm_args()->periodHalfMs;

  switch (getPwmStateVariable[0]) {
    case 0x00:
      pwmPulseDc = 0;
      pwmPriority = false;
      break;

    case 0x80:
      pwmPulseDc = getPwmStateVariable[1];
      pwmPriority = false;
      break;

    case 0x82:
      pwmPulseDc = getPwmStateVariable[1];
      pwmPriority = true;
      break;

    default:
      sl_zigbee_app_debug_print("Invalid PWM state variable: %u\n", getPwmStateVariable[0]);
      return;
  }
  pwmPeriodHalfMs = getPwmStateVariable[2];

  sl_zigbee_app_debug_print("PTA PWM (%s): %u (PERIOD in 0.5ms), %u (%%DC), %u (%s PRIORITY)",
                            ((pwmPulseDc > 0u) ? "ENABLED" : "DISABLED"),
                            pwmPeriodHalfMs,
                            pwmPulseDc,
                            pwmPriority,
                            (pwmPriority ? "HIGH" : "LOW"));
}

//------------------------------------------------------------------------------
// Set PWM state from console
// Console Command : "plugin coexistence set-pta-state <periodHalfms>
//                                              <duty-cycle %> <priority, 0|1>"
// Console Response: "PTA PWM (ENABLE|DISABLED): <period> (PERIOD in 0.5ms),
//                             <duty-cycle> (%DC), <0|1> (<LOW|HIGH> PRIORITY)"
void emberAfPluginCoexistenceSetPwmState(sl_cli_command_arg_t *arguments)
{
  uint8_t pwmPeriodHalfMs = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t pwmPulseDc = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t pwmPriority = (bool)sl_cli_get_argument_uint8(arguments, 2);
  uint8_t pwmComposite = (pwmPulseDc > 0u) ? (pwmPriority ? 0x82 : 0x80) : 0x00;

  sl_rail_util_coex_set_request_pwm(pwmComposite,
                                    0,
                                    pwmPulseDc,
                                    pwmPeriodHalfMs);
  emberAfPluginCoexistenceGetPwmState(arguments);
}

//------------------------------------------------------------------------------
// Get Directional PRIORITY state from console
// Console Command : "plugin coexistence get-dp-state"
// Console Response: "Directional PRIORITY: <ENABLED|DISABLED>, <pulseWidth>(us)"
void emberAfPluginCoexistenceGetDpState(sl_cli_command_arg_t *arguments)
{
// if Directional PRIORITY compiled in:
// 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
// 2. Pulse-width is adjustable in us resolution (1-255)
  (void)arguments;
#if SL_RAIL_UTIL_COEX_DP_ENABLED
  uint8_t dpPulse = sl_rail_util_coex_get_directional_priority_pulse_width();

  sl_zigbee_app_debug_print("Directional PRIORITY: %s, %u (us)",
                            (dpPulse > 0u) ? "ENABLED" : "DISABLED",
                            dpPulse);
#else //!SL_RAIL_UTIL_COEX_DP_ENABLED
  sl_zigbee_app_debug_print("Directional PRIORITY not included in build!");
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED
}

//------------------------------------------------------------------------------
// Set PWM state from console
// Console Command : "plugin coexistence set-dp-state <pulseWidth>"
// Console Response: "Directional PRIORITY: <ENABLED|DISABLED>, <pulseWidth>(us)"
void emberAfPluginCoexistenceSetDpState(sl_cli_command_arg_t *arguments)
{
// if Directional PRIORITY compiled in:
// 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
// 2. Pulse-width is adjustable in us resolution (1-255)
#if SL_RAIL_UTIL_COEX_DP_ENABLED
  uint8_t dpPulse = sl_cli_get_argument_uint8(arguments, 0);
  sl_rail_util_coex_set_directional_priority_pulse_width(dpPulse);
#else //!SL_RAIL_UTIL_COEX_DP_ENABLED
  sl_zigbee_app_debug_print("Directional PRIORITY not included in build!");
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED
  emberAfPluginCoexistenceGetDpState(arguments);
}

//------------------------------------------------------------------------------
// clear all counters
void emberAfPluginCoexistenceClearCounters(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_zigbee_app_debug_print("Clearing counters");
  emberAfPluginCountersClear();
}

//------------------------------------------------------------------------------
// Get PHY select state from console
// Console Command : "plugin coexistence get-phy-state"
// Console Response: "PHY Select: <ENABLED|DISABLED>, <timeout> (ms)"
void emberAfPluginCoexistenceGetPhyState(sl_cli_command_arg_t *arguments)
{
// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
  (void)arguments;
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  uint8_t timeout = sl_rail_util_coex_get_phy_select_timeout();

  sl_zigbee_app_debug_print("PHY Select: %s, %u (ms)",
                            (timeout > 0u) ? "ENABLED" : "DISABLED",
                            timeout);
#else //!SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  sl_zigbee_app_debug_print("PHY Select not included in build!");
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
}

//------------------------------------------------------------------------------
// Set PHY select timeout from console
// Console Command : "plugin coexistence set-phy-state <timeout>"
// Console Response: none
void emberAfPluginCoexistenceSetPhyState(sl_cli_command_arg_t *arguments)
{
// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
  uint8_t timeout = sl_cli_get_argument_uint8(arguments, 0);
#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  sl_rail_util_coex_set_phy_select_timeout(timeout);
  emberAfPluginCoexistenceGetPhyState(arguments);
#else //!SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
  (void)timeout;
  sl_zigbee_app_debug_print("PHY Select not included in build!");
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
}

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
static const char * const gpioNames[] = {
  "PTA_GPIO_INDEX_RHO",
  "PTA_GPIO_INDEX_REQ",
  "PTA_GPIO_INDEX_GNT",
  "PTA_GPIO_INDEX_PHY_SELECT",
  "PTA_GPIO_INDEX_WIFI_TX",
  "PTA_GPIO_INDEX_INTERNAL_REQ"
};
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT

//------------------------------------------------------------------------------
// Get GPIO Input override from console
// Console Command : "plugin coexistence get-phy-state"
// Console Response: "<GPIO NAME> GPIO: <ENABLED|DISABLED>"
void emberAfPluginCoexistenceGetGpioInputOverride(sl_cli_command_arg_t *arguments)
{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  sl_rail_util_coex_gpio_index_t gpioIndex = (sl_rail_util_coex_gpio_index_t)sl_cli_get_argument_uint8(arguments, 0);
  if (gpioIndex < (COEX_GPIO_INDEX_COUNT - 1)) {
    bool enabled = sl_rail_util_coex_get_gpio_input_override(gpioIndex);
    sl_zigbee_app_debug_print("%s GPIO: %s",
                              gpioNames[gpioIndex],
                              enabled ? "ENABLED" : "DISABLED");
  } else {
    sl_zigbee_app_debug_print("COEX GPIO index out of bounds!");
  }
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  (void)arguments;
  sl_zigbee_app_debug_print("COEX GPIO input override not included in build!");
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
}

//------------------------------------------------------------------------------
// Set GPIO Input override from console
// Console Command : "plugin coexistence set-gpio-input <gpioIndex>"
// Console Response: none
void emberAfPluginCoexistenceSetGpioInputOverride(sl_cli_command_arg_t *arguments)
{
  // Set PTA GPIO input override by gpioIndex
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  sl_rail_util_coex_gpio_index_t gpioIndex = (sl_rail_util_coex_gpio_index_t)sl_cli_get_argument_uint8(arguments, 0);
  if (gpioIndex < (COEX_GPIO_INDEX_COUNT - 1)) {
    bool enabled = (bool)sl_cli_get_argument_uint8(arguments, 1);
    sl_rail_util_coex_set_gpio_input_override(gpioIndex, enabled);
    emberAfPluginCoexistenceGetGpioInputOverride(arguments);
  } else {
    sl_zigbee_app_debug_print("COEX GPIO index out of bounds!");
  }
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  (void)arguments;
  sl_zigbee_app_debug_print("PTA GPIO input override not included in build!");
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
}

//------------------------------------------------------------------------------
extern const char * titleStrings[];

//------------------------------------------------------------------------------
static void printCounter(uint8_t id)
{
  sl_zigbee_app_debug_print("%s: %u", titleStrings[id], emberCounters[id]);
}

//------------------------------------------------------------------------------
// Print all counters specific to coex
void emberAfPluginCoexistencePrintCounters(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_zigbee_app_debug_print("COUNTERS");
  printCounter(EMBER_COUNTER_PTA_LO_PRI_REQUESTED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_REQUESTED);
  printCounter(EMBER_COUNTER_PTA_LO_PRI_DENIED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_DENIED);
  printCounter(EMBER_COUNTER_PTA_LO_PRI_TX_ABORTED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_TX_ABORTED);
}
