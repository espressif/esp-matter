/***************************************************************************//**
 * @file
 * @brief CLI for the Network Steering plugin.
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

#include "af.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#include "network-steering.h"
#include "network-steering-internal.h"

// -----------------------------------------------------------------------------
// Helper macros and functions

static void addOrSubtractChannel(uint8_t maskToAddTo,
                                 uint8_t channelToAdd,
                                 bool operationIsAdd)
{
  if (channelToAdd < EMBER_MIN_802_15_4_CHANNEL_NUMBER
      || channelToAdd > EMBER_MAX_802_15_4_CHANNEL_NUMBER) {
    emberAfCorePrintln("Channel not valid: %d", channelToAdd);
  } else if (maskToAddTo == 1) {
    if (operationIsAdd) {
      SETBIT(emAfPluginNetworkSteeringPrimaryChannelMask, channelToAdd);
    } else {
      CLEARBIT(emAfPluginNetworkSteeringPrimaryChannelMask, channelToAdd);
    }

    emberAfCorePrintln("%p mask now 0x%4X",
                       "Primary",
                       emAfPluginNetworkSteeringPrimaryChannelMask);
  } else if (maskToAddTo == 2) {
    if (operationIsAdd) {
      SETBIT(emAfPluginNetworkSteeringSecondaryChannelMask, channelToAdd);
    } else {
      CLEARBIT(emAfPluginNetworkSteeringSecondaryChannelMask, channelToAdd);
    }

    emberAfCorePrintln("%p mask now 0x%4X",
                       "Secondary",
                       emAfPluginNetworkSteeringSecondaryChannelMask);
  } else {
    emberAfCorePrintln("Mask not valid: %d", maskToAddTo);
  }
}

#ifdef UC_BUILD

#include "app/util/serial/sl_zigbee_command_interpreter.h"

void emberAfPluginNetworkSteeringStartCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginNetworkSteeringOptionsMask = sl_cli_get_argument_uint8(arguments, 0);
  emberAfPluginNetworkSteeringStart();
}
// // -----------------------------------------------------------------------------
// // Command definitions

// // plugin network-steering mask add <[1=primary|2=secondary]:1> <channel:1>
// // plugin network-steering mask subtract <[1=primary|2=secondary]:1> <channel:1>
void emberAfPluginNetworkSteeringChannelAddOrSubtractCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  bool operationIsAdd = memcmp(sl_cli_get_command_string(arguments, position), "add", strlen("add")) == 0;
  uint8_t maskToAddTo  = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t channelToAdd = sl_cli_get_argument_uint8(arguments, 1);

  addOrSubtractChannel(maskToAddTo,
                       channelToAdd,
                       operationIsAdd);
}

void emberAfPluginNetworkSteeringChannelSetCommand(sl_cli_command_arg_t *arguments)
{
  bool maskIsPrimary = (sl_cli_get_argument_uint8(arguments, 0) == 1);
  uint32_t value = sl_cli_get_argument_uint32(arguments, 1);
  uint32_t *mask = (maskIsPrimary
                    ? &emAfPluginNetworkSteeringPrimaryChannelMask
                    : &emAfPluginNetworkSteeringSecondaryChannelMask);

  emberAfCorePrintln("%p: Set %p mask to 0x%4X",
                     emAfNetworkSteeringPluginName,
                     (maskIsPrimary ? "primary" : "secondary"),
                     (*mask = value));
}

// // plugin network-steering status
void emberAfPluginNetworkSteeringStatusCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("%p: %p:",
                     emAfNetworkSteeringPluginName,
                     "Status");

  emberAfCorePrintln("Channel mask:");
  emberAfCorePrint("    (1) 0x%4X [",
                   emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfCorePrintln("]");
  emberAfCorePrint("    (2) 0x%4X [",
                   emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfCorePrintln("]");

  emberAfCorePrintln("State: 0x%X (%s)",
                     emAfPluginNetworkSteeringState,
                     emAfPluginNetworkSteeringStateNames[emAfPluginNetworkSteeringState]);
#ifndef OPTIMIZE_SCANS
  emberAfCorePrintln("Pan ID index: %d",
                     emAfPluginNetworkSteeringPanIdIndex);
#endif //  OPTIMIZE_SCANS
  emberAfCorePrintln("Current channel: %d",
                     emAfPluginNetworkSteeringGetCurrentChannel());
  emberAfCorePrintln("Total beacons: %d",
                     emAfPluginNetworkSteeringTotalBeacons);
  emberAfCorePrintln("Join attempts: %d",
                     emAfPluginNetworkSteeringJoinAttempts);
  emberAfCorePrintln("Network state: 0x%X",
                     emberAfNetworkState());
}

void emberAfPluginNetworkSteeringStopCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("%s: %s: 0x%X",
                     emAfNetworkSteeringPluginName,
                     "Stop",
                     emberAfPluginNetworkSteeringStop());
}

void emberAfPluginNetworkSteeringSetPreconfiguredKeyCommand(sl_cli_command_arg_t *arguments)
{
#ifndef OPTIMIZE_SCANS
  EmberKeyData keyData;
  size_t len = 16;
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 0, &len);
  MEMSET(keyData.contents, 0, EMBER_ENCRYPTION_KEY_SIZE);
  MEMMOVE(keyData.contents, ptr_string, EMBER_ENCRYPTION_KEY_SIZE); // Is the padding correct?

  emAfPluginNetworkSteeringSetConfiguredKey(keyData.contents, true);
#else // OPTIMIZE_SCANS
  emberAfCorePrintln("Unsupported feature when using optimized scans. To use"
                     " a configured key when joining, add the key to the"
                     " transient key table using the wildcard EUI (all FFs).");
#endif // OPTIMIZE_SCANS
}

#else // !UC_BUILD

#if defined(EMBER_AF_GENERATE_CLI) || defined(EMBER_AF_API_COMMAND_INTERPRETER2)

// -----------------------------------------------------------------------------
// Command definitions

// plugin network-steering mask add <[1=primary|2=secondary]:1> <channel:1>
// plugin network-steering mask subtract <[1=primary|2=secondary]:1> <channel:1>
void emberAfPluginNetworkSteeringChannelAddOrSubtractCommand(void)
{
  bool operationIsAdd = (emberStringCommandArgument(-1, NULL)[0] == 'a');
  uint8_t maskToAddTo  = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t channelToAdd = (uint8_t)emberUnsignedCommandArgument(1);

  addOrSubtractChannel(maskToAddTo,
                       channelToAdd,
                       operationIsAdd);
}

void emberAfPluginNetworkSteeringChannelSetCommand(void)
{
  bool maskIsPrimary = (((uint8_t)emberUnsignedCommandArgument(0)) == 1);
  uint32_t value = (uint32_t)emberUnsignedCommandArgument(1);
  uint32_t *mask = (maskIsPrimary
                    ? &emAfPluginNetworkSteeringPrimaryChannelMask
                    : &emAfPluginNetworkSteeringSecondaryChannelMask);

  emberAfCorePrintln("%p: Set %p mask to 0x%4X",
                     emAfNetworkSteeringPluginName,
                     (maskIsPrimary ? "primary" : "secondary"),
                     (*mask = value));
}

// plugin network-steering status
void emberAfPluginNetworkSteeringStatusCommand(void)
{
  emberAfCorePrintln("%p: %p:",
                     emAfNetworkSteeringPluginName,
                     "Status");

  emberAfCorePrintln("Channel mask:");
  emberAfCorePrint("    (1) 0x%4X [",
                   emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfCorePrintln("]");
  emberAfCorePrint("    (2) 0x%4X [",
                   emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfCorePrintln("]");

  emberAfCorePrintln("State: 0x%X (%p)",
                     emAfPluginNetworkSteeringState,
                     emAfPluginNetworkSteeringStateNames[emAfPluginNetworkSteeringState]);
#ifndef EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
  emberAfCorePrintln("Pan ID index: %d",
                     emAfPluginNetworkSteeringPanIdIndex);
#endif //  EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
  emberAfCorePrintln("Current channel: %d",
                     emAfPluginNetworkSteeringGetCurrentChannel());
  emberAfCorePrintln("Total beacons: %d",
                     emAfPluginNetworkSteeringTotalBeacons);
  emberAfCorePrintln("Join attempts: %d",
                     emAfPluginNetworkSteeringJoinAttempts);
  emberAfCorePrintln("Network state: 0x%X",
                     emberAfNetworkState());
}

void emberAfPluginNetworkSteeringStartCommand(void)
{
  emAfPluginNetworkSteeringOptionsMask = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPluginNetworkSteeringStart();
}

void emberAfPluginNetworkSteeringStopCommand(void)
{
  emberAfCorePrintln("%p: %p: 0x%X",
                     emAfNetworkSteeringPluginName,
                     "Stop",
                     emberAfPluginNetworkSteeringStop());
}

void emberAfPluginNetworkSteeringSetPreconfiguredKeyCommand(void)
{
#ifndef EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
  EmberKeyData keyData;
  emberCopyKeyArgument(0, &keyData);
  emAfPluginNetworkSteeringSetConfiguredKey(keyData.contents, true);
#else // EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
  emberAfCorePrintln("Unsupported feature when using optimized scans. To use"
                     " a configured key when joining, add the key to the"
                     " transient key table using the wildcard EUI (all FFs).");
#endif // EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
}

#endif /*
          defined(EMBER_AF_GENERATE_CLI)
        || defined(EMBER_AF_API_COMMAND_INTERPRETER2)
        */

#endif // !UC_BUILD
