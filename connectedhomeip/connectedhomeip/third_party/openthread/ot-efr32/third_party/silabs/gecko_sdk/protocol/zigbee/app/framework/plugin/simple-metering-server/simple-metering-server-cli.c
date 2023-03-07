/***************************************************************************//**
 * @file
 * @brief CLI for the Simple Metering Server plugin.
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

#include "app/framework/include/af.h"
#include "simple-metering-server.h"
#include "simple-metering-test.h"

#ifdef UC_BUILD
#include  "sl_cli.h"

void emAfPluginSimpleMeteringServerCliConfigureMirror(sl_cli_command_arg_t *arguments)
{
  EmberNodeId dstAddr = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint =  sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint =  sl_cli_get_argument_uint8(arguments, 2);
  uint32_t issuerId = sl_cli_get_argument_uint32(arguments, 3);
  uint32_t reportingInterval = sl_cli_get_argument_uint32(arguments, 4);
  uint8_t mirrorNotificationReporting = sl_cli_get_argument_uint8(arguments, 5);
  uint8_t notificationScheme = sl_cli_get_argument_uint8(arguments, 6);

  emberAfFillCommandSimpleMeteringClusterConfigureMirror(issuerId,
                                                         reportingInterval,
                                                         mirrorNotificationReporting,
                                                         notificationScheme);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringCliDisableFastPolling(sl_cli_command_arg_t * arguments)
{
  uint8_t toggleFastPolling = sl_cli_get_argument_uint8(arguments, 0);
  emAfToggleFastPolling(toggleFastPolling);
}

#else // !UC_BUILD

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#if !defined(EMBER_AF_GENERATE_CLI)

void emAfPluginSimpleMeteringServerCliPrint(void);
void emAfPluginSimpleMeteringServerCliRate(void);
void emAfPluginSimpleMeteringServerCliVariance(void);
void emAfPluginSimpleMeteringServerCliAdjust(void);
void emAfPluginSimpleMeteringServerCliOff(void);

void emAfPluginSimpleMeteringServerCliElectric(void);
void emAfPluginSimpleMeteringServerCliGas(void);

#ifdef TEST_METER_ERRORS
void emAfPluginSimpleMeteringServerCliRandomError(void);
void emAfPluginSimpleMeteringServerCliSetError(void);
#endif // TEST_METER_ERRORS

void emAfPluginSimpleMeteringServerCliProfiles(void);

EmberCommandEntry emberAfPluginSimpleMeteringServerCommands[] = {
#ifdef TEST_METER_ENABLE
  emberCommandEntryAction("print", emAfPluginSimpleMeteringServerCliPrint, "", ""),
  emberCommandEntryAction("rate", emAfPluginSimpleMeteringServerCliRate, "v", ""),
  emberCommandEntryAction("variance", emAfPluginSimpleMeteringServerCliVariance, "v", ""),
  emberCommandEntryAction("adjust", emAfPluginSimpleMeteringServerCliAdjust, "u", ""),
  emberCommandEntryAction("off", emAfPluginSimpleMeteringServerCliOff, "u", ""),
  emberCommandEntryAction("electric", emAfPluginSimpleMeteringServerCliElectric, "u", ""),
  emberCommandEntryAction("gas", emAfPluginSimpleMeteringServerCliGas, "u", ""),
#ifdef TEST_METER_ERRORS
  emberCommandEntryAction("rnd_error", emAfPluginSimpleMeteringServerCliRandomError, "u", ""),
  emberCommandEntryAction("set_error", emAfPluginSimpleMeteringServerCliSetError, "uu", ""),
#endif
  emberCommandEntryAction("profiles", emAfPluginSimpleMeteringServerCliProfiles, "u", ""),
#endif // TEST_METER_ENABLE
  emberCommandEntryTerminator(),
};

#endif // EMBER_AF_GENERATE_CLI

#ifdef TEST_METER_ENABLE
static uint8_t getEndpointArgument(uint8_t index)
{
  uint8_t endpoint = (uint8_t) emberUnsignedCommandArgument(index);
  return (endpoint == 0
          ? emberAfPrimaryEndpointForCurrentNetworkIndex()
          : endpoint);
}

// plugin simple-metering-server print
void emAfPluginSimpleMeteringServerCliPrint(void)
{
  afTestMeterPrint();
}

// plugin simple-metering-server rate <int:2>
void emAfPluginSimpleMeteringServerCliRate(void)
{
  afTestMeterSetConsumptionRate((uint16_t)emberUnsignedCommandArgument(0),
                                (uint8_t)emberUnsignedCommandArgument(1));
}

// plugin simple-metering-server variance <int:2>
void emAfPluginSimpleMeteringServerCliVariance(void)
{
  afTestMeterSetConsumptionVariance((uint16_t)emberUnsignedCommandArgument(0));
}

// plugin simple-metering-server adjust <endpoint:1>
void emAfPluginSimpleMeteringServerCliAdjust(void)
{
  afTestMeterAdjust(getEndpointArgument(0));
}

// plugin simple-metering-server off <endpoint: 1>
void emAfPluginSimpleMeteringServerCliOff(void)
{
  afTestMeterMode(getEndpointArgument(0), 0);
}

// plugin simple-metering-server electric <endpoint:1>
void emAfPluginSimpleMeteringServerCliElectric(void)
{
#if (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_TYPE == EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_ELECTRIC_METER)
  afTestMeterMode(getEndpointArgument(0), 1);
#else
  emberAfCorePrintln("Not applicable for a non Electric Meter.");
#endif
}

// plugin simple-metering-server gas <endpoint:1>
void emAfPluginSimpleMeteringServerCliGas(void)
{
//#if (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_TYPE == EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_GAS_METER)
  afTestMeterMode(getEndpointArgument(0), 2);
// #else
//   emberAfCorePrintln("Not applicable for a non Gas Meter.");
// #endif
}

#ifdef TEST_METER_ERRORS
// plugin simple-metering-server rnd_error <data:1>
void emAfPluginSimpleMeteringServerCliRandomError(void)
{
  // enables random error setting at each tick
  afTestMeterRandomError((uint8_t)emberUnsignedCommandArgument(0));
}

// plugin simple-metering-server set_error <data:1> <endpoint:1>
void emAfPluginSimpleMeteringServerCliSetError(void)
{
  // sets error, in the process overriding random_error
  afTestMeterSetError(getEndpointArgument(1),
                      (uint8_t)emberUnsignedCommandArgument(0));
}
#endif // TEST_METER_ERRORS

// plugin simple-metering-server profiles <data:1>
void emAfPluginSimpleMeteringServerCliProfiles(void)
{
#if (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_PROFILES != 0)
  afTestMeterEnableProfiles((uint8_t)emberUnsignedCommandArgument(0));
#else
  emberAfCorePrintln("Not applicable for 0 configured profiles.");
#endif
}
#endif // TEST_METER_ENABLE

void emAfPluginSimpleMeteringServerCliConfigureMirror(void)
{
  EmberNodeId dstAddr = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint =  (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint =  (uint8_t)emberUnsignedCommandArgument(2);
  uint32_t issuerId = (uint32_t)emberUnsignedCommandArgument(3);
  uint32_t reportingInterval = (uint32_t)emberUnsignedCommandArgument(4);
  uint8_t mirrorNotificationReporting = (uint8_t)emberUnsignedCommandArgument(5);
  uint8_t notificationScheme = (uint8_t)emberUnsignedCommandArgument(6);

  emberAfFillCommandSimpleMeteringClusterConfigureMirror(issuerId,
                                                         reportingInterval,
                                                         mirrorNotificationReporting,
                                                         notificationScheme);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

void emAfPluginSimpleMeteringCliDisableFastPolling(void)
{
  uint8_t toggleFastPolling = (uint8_t)emberUnsignedCommandArgument(0);
  emAfToggleFastPolling(toggleFastPolling);
}

#endif // UC_BUILD
