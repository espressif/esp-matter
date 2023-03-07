/***************************************************************************//**
 * @file
 * @brief CLI for the Polling plugin.
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
#include "app/framework/util/common.h"
#include "end-device-support.h"

const char * pollControlStrings[] = {
  "EMBER_AF_LONG_POLL",
  "EMBER_AF_SHORT_POLL",
};

#ifdef UC_BUILD
#include "app/util/serial/sl_zigbee_command_interpreter.h"

void emberAfPluginEndDeviceSupportStatusCommand(sl_cli_command_arg_t *arguments)
{
  const char * names[] = {
    EM_AF_GENERATED_NETWORK_STRINGS
  };
  uint8_t i;

  emberAfCorePrintln("End Device Poll Information");
  emberAfCorePrintln("EMBER_END_DEVICE_TIMEOUT:       %d", EMBER_END_DEVICE_POLL_TIMEOUT);
  emberAfCorePrintln("Poll completed callback: %p",
                     (emAfEnablePollCompletedCallback
                      ? "yes"
                      : "no"));
  emberAfCoreFlush();

  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    (void) emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()
        && EMBER_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
      emberAfCorePrintln("nwk %d [%p]", i, names[i]);
      emberAfCorePrintln("  Current Poll Interval (qs):   %l",
                         emberAfGetCurrentPollIntervalQsCallback());
      emberAfCorePrintln("  Long Poll Interval (qs):      %l",
                         emberAfGetLongPollIntervalQsCallback());
      if (EMBER_SLEEPY_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
        emberAfCorePrintln("  Short Poll Interval (qs):     %l",
                           emberAfGetShortPollIntervalQsCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Wake Timeout (qs):            %l",
                           emberAfGetWakeTimeoutQsCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Wake Timeout Bitmask:         0x%4x",
                           emberAfGetWakeTimeoutBitmaskCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Current App Tasks:            0x%4x",
                           emberAfGetCurrentAppTasksCallback());
        emberAfCorePrintln("  Current Poll Control          %p",
                           pollControlStrings[emberAfGetCurrentPollControlCallback()]);
        emberAfCoreFlush();
        emberAfCorePrintln("  Default Poll Control          %p",
                           pollControlStrings[emberAfGetDefaultPollControlCallback()]);
        emberAfCoreFlush();
      }
    }
    (void) emberAfPopNetworkIndex();
  }
}

void emberAfPluginEndDeviceSupportForceShortPollCommand(sl_cli_command_arg_t *arguments)
{
  bool shortPoll = sl_cli_get_argument_uint8(arguments, 0);
  if (shortPoll) {
    emberAfCorePrintln("Forcing device to short poll");
    emberAfAddToCurrentAppTasksCallback(EMBER_AF_FORCE_SHORT_POLL);
  } else {
    emberAfCorePrintln("Allowing device to long poll");
    emberAfRemoveFromCurrentAppTasksCallback(EMBER_AF_FORCE_SHORT_POLL);
  }
}

void emberAfPluginEndDeviceSupportPollCompletedCallbackCommand(sl_cli_command_arg_t *arguments)
{
  emAfEnablePollCompletedCallback = sl_cli_get_argument_uint8(arguments, 0);
}

#else

// *****************************************************************************
// Forward declarations

void emberAfPluginEndDeviceSupportStatusCommand(void);
void emberAfPluginEndDeviceSupportPollCompletedCallbackCommand(void);
void emberAfPluginEndDeviceSupportForceShortPollCommand(void);

// *****************************************************************************
// Globals

#ifndef EMBER_AF_GENERATE_CLI
EmberCommandEntry emberAfPluginEndDeviceSupportCommands[] = {
  emberCommandEntryAction("status", emberAfPluginEndDeviceSupportStatusCommand, "",
                          "Display the status of the End Device's polling"),

  emberCommandEntryAction("poll-completed-callback", emberAfPluginEndDeviceSupportPollCompletedCallbackCommand, "u",
                          "Sets whether the device's poll completed callback function is enabled"),

  emberCommandEntryAction("force-short-poll", emberAfPluginEndDeviceSupportForceShortPollCommand, "u",
                          "Sets whether the CLI forces the device to short poll"),

  emberCommandEntryTerminator()
};
#endif // EMBER_AF_GENERATE_CLI

// *****************************************************************************
// Functions

void emberAfPluginEndDeviceSupportStatusCommand(void)
{
  const char * names[] = {
    EM_AF_GENERATED_NETWORK_STRINGS
  };
  uint8_t i;

  emberAfCorePrintln("End Device Poll Information");
  emberAfCorePrintln("EMBER_END_DEVICE_TIMEOUT:       %d", EMBER_END_DEVICE_POLL_TIMEOUT);
  emberAfCorePrintln("Poll completed callback: %p",
                     (emAfEnablePollCompletedCallback
                      ? "yes"
                      : "no"));
  emberAfCoreFlush();

  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    (void) emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()
        && EMBER_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
      emberAfCorePrintln("nwk %d [%p]", i, names[i]);
      emberAfCorePrintln("  Current Poll Interval (qs):   %l",
                         emberAfGetCurrentPollIntervalQsCallback());
      emberAfCorePrintln("  Long Poll Interval (qs):      %l",
                         emberAfGetLongPollIntervalQsCallback());
      if (EMBER_SLEEPY_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
        emberAfCorePrintln("  Short Poll Interval (qs):     %l",
                           emberAfGetShortPollIntervalQsCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Wake Timeout (qs):            %l",
                           emberAfGetWakeTimeoutQsCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Wake Timeout Bitmask:         0x%4x",
                           emberAfGetWakeTimeoutBitmaskCallback());
        emberAfCoreFlush();
        emberAfCorePrintln("  Current App Tasks:            0x%4x",
                           emberAfGetCurrentAppTasksCallback());
        emberAfCorePrintln("  Current Poll Control          %p",
                           pollControlStrings[emberAfGetCurrentPollControlCallback()]);
        emberAfCoreFlush();
        emberAfCorePrintln("  Default Poll Control          %p",
                           pollControlStrings[emberAfGetDefaultPollControlCallback()]);
        emberAfCoreFlush();
      }
    }
    (void) emberAfPopNetworkIndex();
  }
}

void emberAfPluginEndDeviceSupportForceShortPollCommand(void)
{
  bool shortPoll = (bool)emberUnsignedCommandArgument(0);
  if (shortPoll) {
    emberAfCorePrintln("Forcing device to short poll");
    emberAfAddToCurrentAppTasksCallback(EMBER_AF_FORCE_SHORT_POLL);
  } else {
    emberAfCorePrintln("Allowing device to long poll");
    emberAfRemoveFromCurrentAppTasksCallback(EMBER_AF_FORCE_SHORT_POLL);
  }
}

void emberAfPluginEndDeviceSupportPollCompletedCallbackCommand(void)
{
  emAfEnablePollCompletedCallback = (bool)emberUnsignedCommandArgument(0);
}

#endif
