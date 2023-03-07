/***************************************************************************/
/**
 * @file
 * @brief Coexistence CLI support
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#if SL_OPENTHREAD_COEX_CLI_ENABLE

#include <openthread/cli.h>
#include "common/code_utils.hpp"
#include "coexistence-802154.h"
#include "sl_rail_util_ieee802154_phy_select.h"
#include "sl_ot_custom_cli.h"

// TO DO: Should these be left internal only?
#define SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT                 (1)

extern void efr32RadioGetCoexCounters(uint32_t (*aCoexCounters)[SL_RAIL_UTIL_COEX_EVENT_COUNT]);
extern void efr32RadioClearCoexCounters(void);
static void helpCommand(void *context, uint8_t argc, char *argv[]);

//------------------------------------------------------------------------------
// Get Directional PRIORITY state from console
// Console Command : "coexistence get-dp-state"
// Console Response: "Directional PRIORITY: <ENABLED|DISABLED>, <pulseWidth>(us)"
static void getDpStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    // if Directional PRIORITY compiled in:
    // 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
    // 2. Pulse-width is adjustable in us resolution (1-255)
    uint8_t dpPulse = 0U;
#if SL_RAIL_UTIL_COEX_DP_ENABLED
    dpPulse = sl_rail_util_coex_get_directional_priority_pulse_width();
    otCliOutputFormat("Directional PRIORITY: %s, %u (us)",
                      (dpPulse > 0u) ? "ENABLED" : "DISABLED",
                      dpPulse);
#else //!SL_RAIL_UTIL_COEX_DP_ENABLED
    (void)dpPulse;
    otCliOutputFormat("Directional PRIORITY not included in build!");
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED
    otCliOutputFormat("\r\n");
}

//------------------------------------------------------------------------------
// Set Directional PRIORITY state from console
// Console Command : "coexistence set-dp-state <pulseWidth>"
// Console Response: none
static void setDpStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    // if Directional PRIORITY compiled in:
    // 1. Enabled = pulse-width!=0, Disabled = pulse-width==0
    // 2. Pulse-width is adjustable in us resolution (1-255)
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);

    uint8_t dpPulse = (uint8_t)strtoul(argv[0], NULL, 10);

#if SL_RAIL_UTIL_COEX_DP_ENABLED
    sl_rail_util_coex_set_directional_priority_pulse_width(dpPulse);
#else //!SL_RAIL_UTIL_COEX_DP_ENABLED
    (void)dpPulse;
    otCliOutputFormat("Directional PRIORITY not included in build!");
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED
    otCliOutputFormat("\r\n");

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//------------------------------------------------------------------------------
// Get GPIO Input override from console
// Console Command : "coexistence get-phy-state"
// Console Response: "<GPIO NAME> GPIO: <ENABLED|DISABLED>"

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
static const char * const gpioNames[] = {
  "COEX_GPIO_INDEX_RHO",
  "COEX_GPIO_INDEX_REQ",
  "COEX_GPIO_INDEX_GNT",
  "COEX_GPIO_INDEX_PHY_SELECT"
};
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT

static void getGpioInputOverrideCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);

#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    COEX_GpioIndex_t gpioIndex = (COEX_GpioIndex_t)strtoul(argv[0], NULL, 10);
    bool enabled = sl_rail_util_coex_get_gpio_input_override(gpioIndex);

    otCliOutputFormat("%s GPIO: %s", gpioNames[gpioIndex], enabled ? "ENABLED" : "DISABLED");
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    otCliOutputFormat("COEX GPIO input override not included in build!");
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    otCliOutputFormat("\r\n");
exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//------------------------------------------------------------------------------
// Set GPIO Input override from console
// Console Command : "coexistence set-gpio-input <gpioIndex>"
// Console Response: none
static void setGpioInputOverrideCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);

    // Set PTA GPIO input override by gpioIndex
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    COEX_GpioIndex_t gpioIndex = (COEX_GpioIndex_t)strtoul(argv[0], NULL, 10);
    bool enabled = (bool)strtoul(argv[1], NULL, 10);
    sl_rail_util_coex_set_gpio_input_override(gpioIndex, enabled);
#else //!SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    otCliOutputFormat("PTA GPIO input override not included in build!");
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    otCliOutputFormat("\r\n");

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//------------------------------------------------------------------------------
// Get PHY select state from console
// Console Command : "coexistence get-phy-state"
// Console Response: "Active Radio PHY:<activePhy>
//                    PHY Select: <ENABLED|DISABLED>, <timeout> (ms)"

static const char * const phyNames[] = {
  "RADIO_CONFIG_154_2P4_DEFAULT",
  "RADIO_CONFIG_154_2P4_ANTDIV",
  "RADIO_CONFIG_154_2P4_COEX",
  "RADIO_CONFIG_154_2P4_ANTDIV_COEX",
  "RADIO_CONFIG_154_2P4_FEM",
  "RADIO_CONFIG_154_2P4_ANTDIV_FEM",
  "RADIO_CONFIG_154_2P4_COEX_FEM",
  "RADIO_CONFIG_154_2P4_ANTDIV_COEX_FEM",
  "INVALID_PHY_SELECTION",
};

#define PHY_COUNT ((sizeof(phyNames) / sizeof(phyNames[0])) - 1)

static void getPhyStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

    uint8_t activePhy = (uint8_t)sl_rail_util_ieee802154_get_active_radio_config();

    if (activePhy >= PHY_COUNT) {
        activePhy = PHY_COUNT;
    }
    otCliOutputFormat("Active Radio PHY:%s", phyNames[activePhy]);
    otCliOutputFormat("\r\n");

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
    uint8_t timeout = sl_rail_util_coex_get_phy_select_timeout();

    otCliOutputFormat("PHY Select: %s, %u (ms)",
                      (timeout > 0u) ? "ENABLED" : "DISABLED",
                      timeout);
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

    otCliOutputFormat("\r\n");
}

//------------------------------------------------------------------------------
// Set PHY select timeout from console
// Console Command : "coexistence set-phy-state <timeout>"
// Console Response: none
static void setPhyStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    // Set PHY select timeout in milliseconds
    // case 1. timeoutMs == 0 -> disable COEX optimized PHY
    // case 2. 0 < timeoutMs < PTA_PHY_SELECT_TIMEOUT_MAX -> disable COEX optimized PHY
    //   if there is no WiFi activity for timeoutMs
    // case 3. timeoutMs == PTA_PHY_SELECT_TIMEOUT_MAX -> enable COEX optimize PHY

    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);

    uint8_t timeout = (uint8_t)strtoul(argv[0], NULL, 10);

    if (sl_rail_util_coex_set_phy_select_timeout(timeout) != SL_STATUS_OK) {
        otCliOutputFormat("Error switching between default and coexistence PHY.");
    }
    otCliOutputFormat("\r\n");

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//-----------------------------------------------------------------------------
// Get ptaOptions and print hex value to console
// Console Command : "coexistence get-pta-options"
// Console Response: "PTA Configuration Option: 0x<ptaOptions>"

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

static void getPtaOptionsCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

    uint32_t ptaOptions;
    uint8_t i;
    uint32_t value;

    ptaOptions = (uint32_t)sl_rail_util_coex_get_options();
    otCliOutputFormat("PTA Configuration Option: 0x%04x\r\n", ptaOptions);
    otCliOutputFormat("%s\r\n", ptaHelp[0]);
    for (i = 0; i < PTA_OPTION_FIELDS; i++) {
        value = (ptaOptions >> ptaBitShift[i]) & ptaBitMask[i];
        otCliOutputFormat("%s %02d            %d          %d\r\n",
                          ptaHelp[i + 1],
                          ptaBitShift[i],
                          (ptaBitShift[i + 1] - ptaBitShift[i]),
                          value);
    }
    otCliOutputFormat("\r\n");
}

//-----------------------------------------------------------------------------
// Set ptaOptions from console
// Console Command : "coexistence set-pta-options 0x<ptaOptions>"
// Console Response: none
static void setPtaOptionsCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);

    uint32_t ptaOptions = (uint32_t)strtoul(argv[0], NULL, 16);
    sl_rail_util_coex_set_options(ptaOptions);
    otCliOutputFormat("\r\n");
exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//-----------------------------------------------------------------------------
// Get PTA state (enabled/disabled)
// Console Command : "coexistence get-pta-state"
// Console Response: "PTA is <ENABLED|DISABLED>"
static void getPtaStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

    uint8_t ptaState = (uint8_t)sl_rail_util_coex_is_enabled();
    otCliOutputFormat("PTA is %s", ((ptaState != 0u) ? "ENABLED" : "DISABLED"));
    otCliOutputFormat("\r\n");
}

//-----------------------------------------------------------------------------
// Set PTA state (disabled=0/enabled=1)
// Console Command : "coexistence set-pta-state <0|1>"
// Console Response: none
static void setPtaStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);

    uint8_t ptaState = (uint8_t)strtoul(argv[0], NULL, 10);
    sl_rail_util_coex_set_enable(ptaState != 0u);
    otCliOutputFormat("\r\n");
exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//-----------------------------------------------------------------------------
// Get PWM state from console
// Console Command : "coexistence get-pwm-state"
// Console Response: "PTA PWM (ENABLE|DISABLED): <period> (PERIOD in 0.5ms),
//                             <duty-cycle> (%DC), <0|1> (<LOW|HIGH> PRIORITY)"
static void getPwmStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
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
            otCliOutputFormat("Invalid PWM state variable: %u\n", getPwmStateVariable[0]);
        return;
    }
    pwmPeriodHalfMs = getPwmStateVariable[2];

    otCliOutputFormat("PTA PWM (%s): %u (PERIOD in 0.5ms), %u (%%DC), %u (%s PRIORITY)",
                      ((pwmPulseDc > 0u) ? "ENABLED" : "DISABLED"),
                      pwmPeriodHalfMs,
                      pwmPulseDc,
                      pwmPriority,
                      (pwmPriority ? "HIGH" : "LOW"));
    otCliOutputFormat("\r\n");
}

//------------------------------------------------------------------------------
// Set PWM state from console
// Console Command : "coexistence set-pta-state <periodHalfms>
//                                              <duty-cycle %> <priority, 0|1>"
// Console Response: none
static void setPwmStateCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    VerifyOrExit(argc == 3, error = OT_ERROR_INVALID_ARGS);

    uint8_t pwmPeriodHalfMs = (uint8_t)strtoul(argv[0], NULL, 10);
    uint8_t pwmPulseDc = (uint8_t)strtoul(argv[1], NULL, 10);
    uint8_t pwmPriority = (bool)strtoul(argv[2], NULL, 10);
    uint8_t pwmComposite = (pwmPulseDc > 0u) ? (pwmPriority ? 0x82 : 0x80) : 0x00;

    sl_rail_util_coex_set_request_pwm(pwmComposite,
                                      0,
                                      pwmPulseDc,
                                      pwmPeriodHalfMs);

    otCliOutputFormat("\r\n");
exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

//------------------------------------------------------------------------------
// Clear coex counters
// Console Command : "coexistence reset-counters
// Console Response: none
static void clearCountersCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE
    efr32RadioClearCoexCounters();
    otCliOutputFormat("coex counters cleared");
#else
    otCliOutputFormat("coex counters not enabled");
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE
    otCliOutputFormat("\r\n");
}

//------------------------------------------------------------------------------
// Print counter results
// Console Command : "coexistence get-counters
// Console Response:
// coexistence get-counters
// COEX Lo Pri Req: <count>
// COEX Hi Pri Req: <count>
// COEX Lo Pri Denied: <count>
// COEX Hi Pri Denied: <count>
// COEX Lo Pri Tx Abrt: <count>
// COEX Hi Pri Tx Abrt: <count>

static void getCountersCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE
    uint32_t coexCounters[SL_RAIL_UTIL_COEX_EVENT_COUNT] = {0};
    efr32RadioGetCoexCounters(&coexCounters);
    otCliOutputFormat("COEX %s: %lu\r\n", "Lo Pri Req",     coexCounters[SL_RAIL_UTIL_COEX_EVENT_LO_PRI_REQUESTED]);
    otCliOutputFormat("COEX %s: %lu\r\n", "Hi Pri Req",     coexCounters[SL_RAIL_UTIL_COEX_EVENT_HI_PRI_REQUESTED]);
    otCliOutputFormat("COEX %s: %lu\r\n", "Lo Pri Denied",  coexCounters[SL_RAIL_UTIL_COEX_EVENT_LO_PRI_DENIED]);
    otCliOutputFormat("COEX %s: %lu\r\n", "Hi Pri Denied",  coexCounters[SL_RAIL_UTIL_COEX_EVENT_HI_PRI_DENIED]);
    otCliOutputFormat("COEX %s: %lu\r\n", "Lo Pri Tx Abrt", coexCounters[SL_RAIL_UTIL_COEX_EVENT_LO_PRI_TX_ABORTED]);
    otCliOutputFormat("COEX %s: %lu\r\n", "Hi Pri Tx Abrt", coexCounters[SL_RAIL_UTIL_COEX_EVENT_HI_PRI_TX_ABORTED]);
#else
    otCliOutputFormat("coex counters not enabled\r\n");
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE
}

//------------------------------------------------------------------------------

static otCliCommand coexCommands[] = {
    {"help", &helpCommand},
    {"get-dp-state", &getDpStateCommand},
    {"get-gpio-input", &getGpioInputOverrideCommand},
    {"get-phy-state", &getPhyStateCommand},
    {"get-pta-options", &getPtaOptionsCommand},
    {"get-pta-state", &getPtaStateCommand},
    {"get-pwm-state", &getPwmStateCommand},
    {"set-dp-state", &setDpStateCommand},
    {"set-gpio-input", &setGpioInputOverrideCommand},
    {"set-phy-state", &setPhyStateCommand},
    {"set-pta-options", &setPtaOptionsCommand},
    {"set-pta-state", &setPtaStateCommand},
    {"set-pwm-state", &setPwmStateCommand},
    {"reset-counters", &clearCountersCommand},
    {"get-counters", &getCountersCommand},
};

void coexCommand(void *context, uint8_t argc, char *argv[])
{
    otError error = otCRPCHandleCommand(context, argc, argv, OT_ARRAY_LENGTH(coexCommands), coexCommands);

    if (error == OT_ERROR_INVALID_COMMAND)
    {
        helpCommand(NULL, 0, NULL);
    }
}

static void helpCommand(void *context, uint8_t argc, char *argv[])
{
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    printCommands(coexCommands, OT_ARRAY_LENGTH(coexCommands));
}
#endif // SL_OPENTHREAD_COEX_CLI_ENABLE