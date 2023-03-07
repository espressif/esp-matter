/***************************************************************************//**
 * @file
 * @brief CLI for the Barrier Control Server plugin.
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

#include "barrier-control-server.h"

static bool getBarrierPositionDescription(uint8_t endpoint,
                                          const char **descriptionLocation)
{
  bool descriptionWasSet = true;
  switch (emAfPluginBarrierControlServerGetBarrierPosition(endpoint)) {
    case EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_OPEN:
      *descriptionLocation = "open";
      break;
    case EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_CLOSED:
      *descriptionLocation = "closed";
      break;
    default:
      if (emAfPluginBarrierControlServerIsPartialBarrierSupported(endpoint)) {
        descriptionWasSet = false;
      } else {
        *descriptionLocation = "unknown";
      }
  }
  return descriptionWasSet;
}

static void printSafetyStatus(uint16_t safetyStatus)
{
  emberAfAppPrint("SafetyStatus: 0x%2X (", safetyStatus);
  if (safetyStatus != 0) {
    if (READBITS(safetyStatus,
                 EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_REMOTE_LOCKOUT)) {
      emberAfAppPrint("lockout,");
    }
    if (READBITS(safetyStatus,
                 EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_TEMPER_DETECTED)) {
      emberAfAppPrint("tamper,");
    }
    if (READBITS(safetyStatus,
                 EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_FAILED_COMMUNICATION)) {
      emberAfAppPrint("communication,");
    }
    if (READBITS(safetyStatus,
                 EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_POSITION_FAILURE)) {
      emberAfAppPrint("position,");
    }
  } else {
    emberAfAppPrint("OK");
  }
  emberAfAppPrintln(")");
}
#ifdef UC_BUILD
// plugin barrier-control-server status <endpoint:1>
void emAfPluginBarrierControlServerStatusCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t barrierPosition
    = emAfPluginBarrierControlServerGetBarrierPosition(endpoint);
  const char *description;
  emberAfAppPrint("BarrierPosition: %d%%", barrierPosition);
  if (getBarrierPositionDescription(endpoint, &description)) {
    emberAfAppPrint(" (%s)", description);
  }
  emberAfAppPrintln("");
  printSafetyStatus(emAfPluginBarrierControlServerGetSafetyStatus(endpoint));
}

// plugin barrier-control-server open <endpoint:1>
// plugin barrier-control-server close <endpoint:1>
void emAfPluginBarrierControlServerOpenOrCloseCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  bool open = memcmp(arguments->argv[arguments->arg_ofs - 1], "open", strlen("open")) == 0;
  uint8_t barrierPosition
    = (open
       ? EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_OPEN
       : EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_CLOSED);
  emAfPluginBarrierControlServerSetBarrierPosition(endpoint, barrierPosition);
  emAfPluginBarrierControlServerIncrementEvents(endpoint, open, false);

  const char *description;
  assert(getBarrierPositionDescription(endpoint, &description));
  emberAfAppPrintln("Barrier is now %s", description);
}

// plugin barrier-control-server remote-lockout <endpoint:1> <setOrClear:1>
// plugin barrier-control-server tamper-detected <endpoint:1> <setOrClear:1>
// plugin barrier-control-server failed-communication <endpoint:1> <setOrClear:1>
// plugin barrier-control-server position-failure <endpoint:1> <setOrClear:1>
void emAfPluginBarrierControlServerSafetyStatusCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  bool doSet = sl_cli_get_argument_uint8(arguments, 1) == 1;
  uint8_t bit
    = (memcmp(arguments->argv[arguments->arg_ofs - 1], "remote-lockout", strlen("remote-lockout")) == 0
       ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_REMOTE_LOCKOUT
       : (memcmp(arguments->argv[arguments->arg_ofs - 1], "tamper-detected", strlen("tamper-detected")) == 0
          ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_TEMPER_DETECTED
          : (memcmp(arguments->argv[arguments->arg_ofs - 1], "failed-communication", strlen("failed-communication")) == 0
             ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_FAILED_COMMUNICATION
             : EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_POSITION_FAILURE)));

  uint16_t safetyStatus
    = emAfPluginBarrierControlServerGetSafetyStatus(endpoint);
  if (doSet) {
    SETBITS(safetyStatus, bit);
  } else {
    CLEARBITS(safetyStatus, bit);
  }

  EmberAfStatus status
    = emberAfWriteServerAttribute(endpoint,
                                  ZCL_BARRIER_CONTROL_CLUSTER_ID,
                                  ZCL_BARRIER_SAFETY_STATUS_ATTRIBUTE_ID,
                                  (uint8_t *)&safetyStatus,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
  assert(status == EMBER_ZCL_STATUS_SUCCESS);

  printSafetyStatus(safetyStatus);
}
#else
// plugin barrier-control-server status <endpoint:1>
void emAfPluginBarrierControlServerStatusCommand(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t barrierPosition
    = emAfPluginBarrierControlServerGetBarrierPosition(endpoint);
  const char *description;
  emberAfAppPrint("BarrierPosition: %d%%", barrierPosition);
  if (getBarrierPositionDescription(endpoint, &description)) {
    emberAfAppPrint(" (%s)", description);
  }
  emberAfAppPrintln("");
  printSafetyStatus(emAfPluginBarrierControlServerGetSafetyStatus(endpoint));
}

// plugin barrier-control-server open <endpoint:1>
// plugin barrier-control-server close <endpoint:1>
void emAfPluginBarrierControlServerOpenOrCloseCommand(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  bool open = emberStringCommandArgument(-1, NULL)[0] == 'o';
  uint8_t barrierPosition
    = (open
       ? EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_OPEN
       : EMBER_ZCL_BARRIER_CONTROL_BARRIER_POSITION_CLOSED);
  emAfPluginBarrierControlServerSetBarrierPosition(endpoint, barrierPosition);
  emAfPluginBarrierControlServerIncrementEvents(endpoint, open, false);

  const char *description;
  assert(getBarrierPositionDescription(endpoint, &description));
  emberAfAppPrintln("Barrier is now %s", description);
}

// plugin barrier-control-server remote-lockout <endpoint:1> <setOrClear:1>
// plugin barrier-control-server tamper-detected <endpoint:1> <setOrClear:1>
// plugin barrier-control-server failed-communication <endpoint:1> <setOrClear:1>
// plugin barrier-control-server position-failure <endpoint:1> <setOrClear:1>
void emAfPluginBarrierControlServerSafetyStatusCommand(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  bool doSet = ((uint8_t)emberUnsignedCommandArgument(1)) == 1;
  uint8_t bit
    = (emberStringCommandArgument(-1, NULL)[0] == 'r'
       ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_REMOTE_LOCKOUT
       : (emberStringCommandArgument(-1, NULL)[0] == 't'
          ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_TEMPER_DETECTED
          : (emberStringCommandArgument(-1, NULL)[0] == 'f'
             ? EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_FAILED_COMMUNICATION
             : EMBER_AF_BARRIER_CONTROL_SAFETY_STATUS_POSITION_FAILURE)));

  uint16_t safetyStatus
    = emAfPluginBarrierControlServerGetSafetyStatus(endpoint);
  if (doSet) {
    SETBITS(safetyStatus, bit);
  } else {
    CLEARBITS(safetyStatus, bit);
  }

  EmberAfStatus status
    = emberAfWriteServerAttribute(endpoint,
                                  ZCL_BARRIER_CONTROL_CLUSTER_ID,
                                  ZCL_BARRIER_SAFETY_STATUS_ATTRIBUTE_ID,
                                  (uint8_t *)&safetyStatus,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
  assert(status == EMBER_ZCL_STATUS_SUCCESS);

  printSafetyStatus(safetyStatus);
}
#endif
