/***************************************************************************//**
 * @file
 * @brief Implements coexistence CLI for host and SoC applications
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifdef SIMULATION_DEVICE
#include "coexistence/hal/simulator/coexistence-hal.h"
#include "coexistence/protocol/ieee802154/coexistence-802154.h"
#else //!SIMULATION_DEVICE
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#if !defined(EZSP_HOST)
#include "stack/include/ember.h"
#else
#include "stack/include/ember-types.h"
#endif

#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#ifndef EMBER_STACK_IP
#include "app/util/common/common.h"
#endif //EMBER_STACK_IP

#ifdef EZSP_HOST
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/framework/include/af-types.h"
#include "app/framework/util/af-main.h"
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
#include "platform/radio/rail_lib/plugin/coexistence/protocol/ieee802154/coexistence-802154.h"
#endif //EZSP_HOST
#ifdef EMBER_AF_GENERATE_CLI
#include "app/framework/plugin/counters/counters.h"
#else //!EMBER_AF_GENERATE_CLI
#include "app/util/counters/counters.h"
#endif //EMBER_AF_GENERATE_CLI
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif
#endif //SIMULATION_DEVICE

//-----------------------------------------------------------------------------
// Get PTA state (enabled/disabled)
// Console Command : "plugin coexistence get-pta-state"
// Console Response: "PTA is <ENABLED|DISABLED>"
void emberAfPluginCoexistenceGetPtaState(void)
{
  uint8_t ptaState;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(ptaState);

  ezspGetValue(EZSP_VALUE_ENABLE_PTA, &valueLength, &ptaState);
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  ptaState = (uint8_t)halPtaIsEnabled();
#endif //EZSP_HOST

  emberSerialPrintfLine(APP_SERIAL, "PTA is %s", ((ptaState != 0u) ? "ENABLED" : "DISABLED"));
}

//-----------------------------------------------------------------------------
// Set PTA state (disabled=0/enabled=1)
// Console Command : "plugin coexistence set-pta-state <0|1>"
// Console Response: none
void emberAfPluginCoexistenceSetPtaState(void)
{
  uint8_t ptaState = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(ptaState);

  emberAfSetEzspValue(EZSP_VALUE_ENABLE_PTA,
                      valueLength,
                      &ptaState,
                      "enable pta");
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  halPtaSetEnable(ptaState != 0u);
#endif //EZSP_HOST
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
void emberAfPluginCoexistenceGetPtaOptions(void)
{
  uint32_t ptaOptions;
  uint8_t i;
  uint32_t value;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(ptaOptions);

  ezspGetValue(EZSP_VALUE_PTA_OPTIONS,
               &valueLength,
               (uint8_t*)&ptaOptions);
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  ptaOptions = (uint32_t)halPtaGetOptions();
#endif //EZSP_HOST

  emberSerialPrintfLine(APP_SERIAL, "PTA Configuration Option: 0x%4x", ptaOptions);

  emberSerialPrintfLine(APP_SERIAL, "%s", ptaHelp[0]);
  for (i = 0; i < PTA_OPTION_FIELDS; i++) {
    value = (ptaOptions >> ptaBitShift[i]) & ptaBitMask[i];
    emberSerialPrintfLine(APP_SERIAL, "%p %02d            %d          %d",
                          ptaHelp[i + 1],
                          ptaBitShift[i],
                          (ptaBitShift[i + 1] - ptaBitShift[i]),
                          value);
  }
  emberSerialPrintfLine(APP_SERIAL, "");
}

//-----------------------------------------------------------------------------
// Set ptaOptions from console
// Console Command : "plugin coexistence set-pta-options 0x<ptaOptions>"
// Console Response: none
void emberAfPluginCoexistenceSetPtaOptions(void)
{
  uint32_t ptaOptions = (uint32_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(ptaOptions);

  emberAfSetEzspValue(EZSP_VALUE_PTA_OPTIONS,
                      valueLength,
                      (uint8_t*)&ptaOptions,
                      "pta options");
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  halPtaSetOptions(ptaOptions);
#endif //EZSP_HOST
}

//-----------------------------------------------------------------------------
// Get PWM state from console
// Console Command : "plugin coexistence get-pta-state"
// Console Response: "PTA PWM (ENABLE|DISABLED): <period> (PERIOD in 0.5ms),
//                             <duty-cycle> (%DC), <0|1> (<LOW|HIGH> PRIORITY)"
void emberAfPluginCoexistenceGetPwmState(void)
{
  uint8_t pwmPeriodHalfMs;
  uint8_t pwmPulseDc;
  bool pwmPriority;
  uint8_t getPwmStateVariable[3];

#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(getPwmStateVariable);

  ezspGetValue(EZSP_VALUE_PTA_PWM_OPTIONS,
               &valueLength,
               getPwmStateVariable);
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  getPwmStateVariable[0] = (uint8_t)halPtaGetRequestPwmArgs()->req;
  getPwmStateVariable[1] = halPtaGetRequestPwmArgs()->dutyCycle;
  getPwmStateVariable[2] = halPtaGetRequestPwmArgs()->periodHalfMs;
#endif //EZSP_HOST

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
      emberSerialPrintfLine(APP_SERIAL, "Invalid PWM state variable: %u\n", getPwmStateVariable[0]);
      return;
  }
  pwmPeriodHalfMs = getPwmStateVariable[2];

  emberSerialPrintfLine(APP_SERIAL, "PTA PWM (%s): %u (PERIOD in 0.5ms), %u (%%DC), %u (%s PRIORITY)",
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
// Console Response: none
void emberAfPluginCoexistenceSetPwmState(void)
{
  uint8_t pwmPeriodHalfMs = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t pwmPulseDc = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t pwmPriority = (bool)emberUnsignedCommandArgument(2);
  uint8_t pwmComposite = (pwmPulseDc > 0u) ? (pwmPriority ? 0x82 : 0x80) : 0x00;

#ifdef EZSP_HOST
  uint8_t setPwmStateVariable[3];
  uint8_t valueLength = sizeof(setPwmStateVariable);

  setPwmStateVariable[0] = pwmComposite;
  setPwmStateVariable[1] = pwmPulseDc;
  setPwmStateVariable[2] = pwmPeriodHalfMs;

  emberAfSetEzspValue(EZSP_VALUE_PTA_PWM_OPTIONS,
                      valueLength,
                      setPwmStateVariable,
                      "configure PWM options");
#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
  halPtaSetRequestPwm(pwmComposite,
                      0,
                      pwmPulseDc,
                      pwmPeriodHalfMs);
#endif //EZSP_HOST
}

//------------------------------------------------------------------------------
// Get Directional PRIORITY state from console
// Console Command : "plugin coexistence get-dp-state"
// Console Response: "Directional PRIORITY: <ENABLED|DISABLED>, <pulseWidth>(us)"
void emberAfPluginCoexistenceGetDpState(void)
{
// if Directional PRIORITY compiled in:
// 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
// 2. Pulse-width is adjustable in us resolution (1-255)
  uint8_t dpPulse = 0U;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(dpPulse);
  EzspStatus status;

  status = ezspGetValue(EZSP_VALUE_PTA_DIRECTIONAL_PRIORITY_PULSE_WIDTH,
                        &valueLength,
                        &dpPulse);
  if (status == EZSP_SUCCESS) {
    emberSerialPrintfLine(APP_SERIAL, "Directional PRIORITY: %s, %u (us)",
                          (dpPulse > 0u) ? "ENABLED" : "DISABLED",
                          dpPulse);
  } else {
    emberSerialPrintfLine(APP_SERIAL, "Error code: %x", status);
    emberSerialPrintfLine(APP_SERIAL, "Error getting Directional PRIORITY information from NCP!");
  }

#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
#if HAL_COEX_DP_ENABLED
  dpPulse = halPtaGetDirectionalPriorityPulseWidth();

  emberSerialPrintfLine(APP_SERIAL, "Directional PRIORITY: %s, %u (us)",
                        (dpPulse > 0u) ? "ENABLED" : "DISABLED",
                        dpPulse);
#else //!HAL_COEX_DP_ENABLED
  (void)dpPulse;
  emberSerialPrintfLine(APP_SERIAL, "Directional PRIORITY not included in build!");
#endif //HAL_COEX_DP_ENABLED
#endif //EZSP_HOST
}

//------------------------------------------------------------------------------
// Set PWM state from console
// Console Command : "plugin coexistence set-dp-state <pulseWidth>"
// Console Response: none
void emberAfPluginCoexistenceSetDpState(void)
{
// if Directional PRIORITY compiled in:
// 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
// 2. Pulse-width is adjustable in us resolution (1-255)
  uint8_t dpPulse = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(dpPulse);

  EzspStatus status;

  status = emberAfSetEzspValue(EZSP_VALUE_PTA_DIRECTIONAL_PRIORITY_PULSE_WIDTH,
                               valueLength,
                               &dpPulse,
                               "configure Directional Priority pulse width");

  if (status != EZSP_SUCCESS) {
    emberSerialPrintfLine(APP_SERIAL, "Error code: %x", status);
    emberSerialPrintfLine(APP_SERIAL, "Error getting Directional PRIORITY information from NCP!");
  }

#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
#if HAL_COEX_DP_ENABLED
  halPtaSetDirectionalPriorityPulseWidth(dpPulse);
#else //!HAL_COEX_DP_ENABLED
  (void)dpPulse;
  emberSerialPrintfLine(APP_SERIAL, "Directional PRIORITY not included in build!");
#endif //HAL_COEX_DP_ENABLED
#endif //EZSP_HOST
}

//------------------------------------------------------------------------------
// clear all counters
void emberAfPluginCoexistenceClearCounters(void)
{
  emberSerialPrintfLine(APP_SERIAL, "Clearing counters");
#ifdef EMBER_AF_GENERATE_CLI
  emberAfPluginCountersClear();
#elif !defined(EMBER_STACK_IP) || defined(QA_THREAD_TEST) //!EMBER_AF_GENERATE_CLI
  emberClearCounters();
#else
  //counters are unsupported
#endif //EMBER_AF_GENERATE_CLI
}

//------------------------------------------------------------------------------
// Get PHY select state from console
// Console Command : "plugin coexistence get-phy-state"
// Console Response: "PHY Select: <ENABLED|DISABLED>, <timeout> (ms)"
void emberAfPluginCoexistenceGetPhyState(void)
{
// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
  uint8_t timeout = 0U;
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(timeout);
  EzspStatus status;

  status = ezspGetValue(EZSP_VALUE_PTA_PHY_SELECT_TIMEOUT,
                        &valueLength,
                        &timeout);
  if (status == EZSP_SUCCESS) {
    emberSerialPrintfLine(APP_SERIAL, "PHY Select: %s, %u (ms)",
                          (timeout > 0u) ? "ENABLED" : "DISABLED",
                          timeout);
  } else {
    emberSerialPrintfLine(APP_SERIAL, "Error code: %x", status);
    emberSerialPrintfLine(APP_SERIAL, "Error getting PHY Select information from NCP!");
  }

#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
#if HAL_COEX_RUNTIME_PHY_SELECT
  timeout = halPtaGetPhySelectTimeout();

  emberSerialPrintfLine(APP_SERIAL, "PHY Select: %s, %u (ms)",
                        (timeout > 0u) ? "ENABLED" : "DISABLED",
                        timeout);
#else //!HAL_COEX_RUNTIME_PHY_SELECT
  (void)timeout;
  emberSerialPrintfLine(APP_SERIAL, "PHY Select not included in build!");
#endif //HAL_COEX_RUNTIME_PHY_SELECT
#endif //EZSP_HOST
}

//------------------------------------------------------------------------------
// Set PHY select timeout from console
// Console Command : "plugin coexistence set-phy-state <timeout>"
// Console Response: none
void emberAfPluginCoexistenceSetPhyState(void)
{
// Set PHY select timeout in milliseconds
// case 1. timeoutMs == 0 -> disable COEX optimized PHY
// case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
//   if there is no WiFi activity for timeoutMs
// case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY
  uint8_t timeout = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  uint8_t valueLength = sizeof(timeout);

  EzspStatus status;

  status = emberAfSetEzspValue(EZSP_VALUE_PTA_PHY_SELECT_TIMEOUT,
                               valueLength,
                               &timeout,
                               "configure PHY select timeout");

  if (status != EZSP_SUCCESS) {
    emberSerialPrintfLine(APP_SERIAL, "Error code: %x", status);
    emberSerialPrintfLine(APP_SERIAL, "Error getting PHY Select information from NCP!");
  }

#elif !defined(EMBER_STACK_IP) //!EZSP_HOST
#if HAL_COEX_RUNTIME_PHY_SELECT
  halPtaSetPhySelectTimeout(timeout);
#else //!HAL_COEX_RUNTIME_PHY_SELECT
  (void)timeout;
  emberSerialPrintfLine(APP_SERIAL, "PHY Select not included in build!");
#endif //HAL_COEX_RUNTIME_PHY_SELECT
#endif //EZSP_HOST
}

#if HAL_COEX_OVERRIDE_GPIO_INPUT
static const char * const gpioNames[] = {
  "PTA_GPIO_INDEX_RHO",
  "PTA_GPIO_INDEX_REQ",
  "PTA_GPIO_INDEX_GNT",
  "PTA_GPIO_INDEX_PHY_SELECT"
};
#endif //HAL_COEX_OVERRIDE_GPIO_INPUT

//------------------------------------------------------------------------------
// Get GPIO Input override from console
// Console Command : "plugin coexistence get-phy-state"
// Console Response: "<GPIO NAME> GPIO: <ENABLED|DISABLED>"
void emberAfPluginCoexistenceGetGpioInputOverride(void)
{
#ifndef EZSP_HOST
#if HAL_COEX_OVERRIDE_GPIO_INPUT
  halPtaGpioIndex_t gpioIndex = (halPtaGpioIndex_t)emberUnsignedCommandArgument(0);
  bool enabled = halPtaGetGpioInputOverride(gpioIndex);

  emberSerialPrintfLine(APP_SERIAL, "%s GPIO: %s",
                        gpioNames[gpioIndex],
                        enabled ? "ENABLED" : "DISABLED");
#else //!HAL_COEX_OVERRIDE_GPIO_INPUT
  emberSerialPrintfLine(APP_SERIAL, "COEX GPIO input override not included in build!");
#endif //HAL_COEX_OVERRIDE_GPIO_INPUT
#endif //EZSP_HOST
}

//------------------------------------------------------------------------------
// Set GPIO Input override from console
// Console Command : "plugin coexistence set-gpio-input <gpioIndex>"
// Console Response: none
void emberAfPluginCoexistenceSetGpioInputOverride(void)
{
  // Set PTA GPIO input override by gpioIndex
#ifndef EZSP_HOST
#if HAL_COEX_OVERRIDE_GPIO_INPUT
  halPtaGpioIndex_t gpioIndex = (halPtaGpioIndex_t)emberUnsignedCommandArgument(0);
  bool enabled = (bool)emberUnsignedCommandArgument(1);
  halPtaSetGpioInputOverride(gpioIndex, enabled);
#else //!HAL_COEX_OVERRIDE_GPIO_INPUT
  emberSerialPrintfLine(APP_SERIAL, "PTA GPIO input override not included in build!");
#endif //HAL_COEX_OVERRIDE_GPIO_INPUT
#endif //!EZSP_HOST
}

//------------------------------------------------------------------------------

#ifdef EMBER_STACK_IP
static const uint8_t * const titleStrings[] = {
  EMBER_COUNTER_STRINGS
};
uint16_t emberCounters[EMBER_COUNTER_TYPE_COUNT] = { 0 };
#else //EMBER_STACK_IP
extern const char * titleStrings[];
#endif //EMBER_STACK_IP

//------------------------------------------------------------------------------
static void printCounter(uint8_t id)
{
  emberSerialPrintfLine(APP_SERIAL, "%p: %u", titleStrings[id], emberCounters[id]);
}

//------------------------------------------------------------------------------
// Print all counters specific to coex
void emberAfPluginCoexistencePrintCounters(void)
{
  #ifdef EZSP_HOST
  ezspReadCounters(emberCounters);
  #endif //EZSP_HOST
  emberSerialPrintfLine(APP_SERIAL, "COUNTERS");
  printCounter(EMBER_COUNTER_PTA_LO_PRI_REQUESTED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_REQUESTED);
  printCounter(EMBER_COUNTER_PTA_LO_PRI_DENIED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_DENIED);
  printCounter(EMBER_COUNTER_PTA_LO_PRI_TX_ABORTED);
  printCounter(EMBER_COUNTER_PTA_HI_PRI_TX_ABORTED);
}

//-----------------------------------------------------------------------------
// Get Active Radio PHY
// Console Command : "plugin antenna get-active-phy"
// Console Response: "Active Radio PHY:<Active Radio PHY>"
void emberAfPluginCoexistenceGetActivePhy(void)
{
  extern void emberAfPluginGetActiveRadioPhy(void);
  emberAfPluginGetActiveRadioPhy();
}
