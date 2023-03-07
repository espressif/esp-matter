/***************************************************************************//**
 * @file
 * @brief CLI for the Messaging Server plugin.
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
#include "app/framework/plugin/messaging-server/messaging-server.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"

static EmberAfPluginMessagingServerMessage message;

#ifdef UC_BUILD
// plugin messaging-server message <message string>
void emAfMessagingServerCliMsg(sl_cli_command_arg_t *arguments)
{
  uint8_t length = sl_zigbee_copy_string_arg(arguments, 0,
                                             message.message + 1,
                                             EMBER_AF_PLUGIN_MESSAGING_SERVER_MESSAGE_SIZE,
                                             false);
  message.message[0] = length;
}

// plugin messaging-server append <message string>
void emAfMessagingServerCliAppend(sl_cli_command_arg_t *arguments)
{
  uint8_t oldLength = message.message[0];
  uint8_t length = sl_zigbee_copy_string_arg(arguments, 0,
                                             message.message + oldLength + 1,
                                             (EMBER_AF_PLUGIN_MESSAGING_SERVER_MESSAGE_SIZE
                                              - oldLength),
                                             false);
  message.message[0] = oldLength + length;
}

// plugin messaging-server id <messageId:4>
void emAfMessagingServerCliId(sl_cli_command_arg_t *arguments)
{
  message.messageId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin messaging-server time <start time:4> <duration:2>
void emAfMessagingServerCliTime(sl_cli_command_arg_t *arguments)
{
  message.startTime = sl_cli_get_argument_uint32(arguments, 0);
  message.durationInMinutes = sl_cli_get_argument_uint16(arguments, 1);
}

// plugin messaging-server relative-time <+/-time> <duration>

// Rather than use absolute time, this will set the start-time relative to the current time +/-
// the CLI parameter in MINUTES.
void emAfMessagingServerCliRelativeTime(sl_cli_command_arg_t *arguments)
{
  message.startTime = (emberAfGetCurrentTime()
                       + (sl_cli_get_argument_uint32(arguments, 0) * 60));
  message.durationInMinutes = sl_cli_get_argument_uint16(arguments, 1);
}

// plugin messaging-server transmission <normal | ipan | both>
void emAfMessagingServerCliTransmission(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t* commandChar = (uint8_t *)sl_cli_get_command_string(arguments, position);
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_TRANSMISSION_MASK;
  if (memcmp(commandChar, "both", strlen("both")) == 0) { // both
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_TRANSMISSION_NORMAL_AND_ANONYMOUS;
  } else if (memcmp(commandChar, "ipan", strlen("ipan")) == 0) { // inter pan
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_TRANSMISSION_ANONYMOUS;
  } else {
    // Do nothing for 'normal'.
    // MISRA requires ..else if.. to have terminating else.
  }
}

// plugin messaging-server importance <low | medium | high | critical>
void emAfMessagingServerCliImportance(sl_cli_command_arg_t *arguments)
{
  uint8_t* commandChar = (uint8_t *)sl_cli_get_command_string(arguments, 3);
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_IMPORTANCE_MASK;
  if (memcmp(commandChar, "medium", strlen("medium")) == 0) { // medium
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_MEDIUM;
  } else if (memcmp(commandChar, "high", strlen("high")) == 0) { // high
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_HIGH;
  } else if (memcmp(commandChar, "critical", strlen("critical")) == 0) { // critical
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_CRITICAL;
  } else {
    // Do nothing for 'low' importance.
    // MISRA requires ..else if.. to have terminating else.
  }
}

// plugin messaging-server confirm <not | req>
void emAfMessagingServerCliConfirm(sl_cli_command_arg_t *arguments)
{
  uint8_t* commandChar = (uint8_t *)sl_cli_get_command_string(arguments, 3);
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_CONFIRMATION_MASK;
  if (memcmp(commandChar, "req", strlen("req")) == 0) { // required
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_CONFIRMATION_REQUIRED;
  }
  // Do nothing for 'not' (not required).
}

// plugin messaging-server <valid | invalid>
void emAfMessagingServerCliValid(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t command_first_character = sl_cli_get_command_string(arguments, position)[0];
  emberAfPluginMessagingServerSetMessage(endpoint,
                                         command_first_character == 'v'
                                         ? &message
                                         : NULL);
}

// plugin messaging-server display <nodeId:2> <srcEndpoint:1> <dstEndpoint:1>
void emAfMessagingServerCliDisplay(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  emberAfPluginMessagingServerDisplayMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint);
}

// plugin messaging-server cancel <nodeId:2> <srcEndpoint:1> <dstEndpoint:1>
void emAfMessagingServerCliCancel(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  emberAfPluginMessagingServerCancelMessage(nodeId,
                                            srcEndpoint,
                                            dstEndpoint);
}

// plugin messaging-server print <endpoint:1>
void emAfMessagingServerCliPrint(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  emAfPluginMessagingServerPrintInfo(endpoint);
}

#else // !UC_BUILD

void emAfMessagingServerCliMsg(void);
void emAfMessagingServerCliAppend(void);
void emAfMessagingServerCliId(void);
void emAfMessagingServerCliTime(void);
void emAfMessagingServerCliRelativeTime(void);
void emAfMessagingServerCliTransmission(void);
void emAfMessagingServerCliImportance(void);
void emAfMessagingServerCliConfirm(void);
void emAfMessagingServerCliValid(void);
void emAfMessagingServerCliDisplay(void);
void emAfMessagingServerCliCancel(void);
void emAfMessagingServerCliPrint(void);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginMessagingServerTransmissionCommands[] = {
  emberCommandEntryAction("normal", emAfMessagingServerCliTransmission, "", ""),
  emberCommandEntryAction("ipan", emAfMessagingServerCliTransmission, "", ""),
  emberCommandEntryAction("both", emAfMessagingServerCliTransmission, "", ""),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginMessagingServerImportanceCommands[] = {
  emberCommandEntryAction("low", emAfMessagingServerCliImportance, "", ""),
  emberCommandEntryAction("medium", emAfMessagingServerCliImportance, "", ""),
  emberCommandEntryAction("high", emAfMessagingServerCliImportance, "", ""),
  emberCommandEntryAction("critical", emAfMessagingServerCliImportance, "", ""),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginMessagingServerConfirmCommands[] = {
  emberCommandEntryAction("not", emAfMessagingServerCliConfirm, "", ""),
  emberCommandEntryAction("req", emAfMessagingServerCliConfirm, "", ""),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginMessagingServerCommands[] = {
  emberCommandEntryAction("message", emAfMessagingServerCliMsg, "b", ""),
  emberCommandEntryAction("append", emAfMessagingServerCliAppend, "b", ""),
  emberCommandEntryAction("id", emAfMessagingServerCliId, "w", ""),
  emberCommandEntryAction("time", emAfMessagingServerCliTime, "wv", ""),
  emberCommandEntryAction("relative-time", emAfMessagingServerCliRelativeTime, "sv", ""),
  emberCommandEntryAction("transmission", NULL, (const char *)emberAfPluginMessagingServerTransmissionCommands, ""),
  emberCommandEntryAction("importance", NULL, (const char *)emberAfPluginMessagingServerImportanceCommands, ""),
  emberCommandEntryAction("confirm", NULL, (const char *)emberAfPluginMessagingServerConfirmCommands, ""),
  emberCommandEntryAction("valid", emAfMessagingServerCliValid, "u", ""),
  emberCommandEntryAction("invalid", emAfMessagingServerCliValid, "u", ""),
  emberCommandEntryAction("display", emAfMessagingServerCliDisplay, "vuu", ""),
  emberCommandEntryAction("cancel", emAfMessagingServerCliCancel, "vuu", ""),
  emberCommandEntryAction("print", emAfMessagingServerCliPrint, "u", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin messaging-server message <message string>
void emAfMessagingServerCliMsg(void)
{
  uint8_t length = emberCopyStringArgument(0,
                                           message.message + 1,
                                           EMBER_AF_PLUGIN_MESSAGING_SERVER_MESSAGE_SIZE,
                                           false);
  message.message[0] = length;
}

// plugin messaging-server append <message string>
void emAfMessagingServerCliAppend(void)
{
  uint8_t oldLength = message.message[0];
  uint8_t length = emberCopyStringArgument(0,
                                           message.message + oldLength + 1,
                                           (EMBER_AF_PLUGIN_MESSAGING_SERVER_MESSAGE_SIZE
                                            - oldLength),
                                           false);
  message.message[0] = oldLength + length;
}

// plugin messaging-server id <messageId:4>
void emAfMessagingServerCliId(void)
{
  message.messageId = emberUnsignedCommandArgument(0);
}

// plugin messaging-server time <start time:4> <duration:2>
void emAfMessagingServerCliTime(void)
{
  message.startTime = emberUnsignedCommandArgument(0);
  message.durationInMinutes = (uint16_t)emberUnsignedCommandArgument(1);
}

// plugin messaging-server relative-time <+/-time> <duration>

// Rather than use absolute time, this will set the start-time relative to the current time +/-
// the CLI parameter in MINUTES.
void emAfMessagingServerCliRelativeTime(void)
{
  message.startTime = (emberAfGetCurrentTime()
                       + (emberSignedCommandArgument(0) * 60));
  message.durationInMinutes = (uint16_t)emberUnsignedCommandArgument(1);
}

// plugin messaging-server transmission <normal | ipan | both>
void emAfMessagingServerCliTransmission(void)
{
  uint8_t commandChar = emberCurrentCommand->name[0];
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_TRANSMISSION_MASK;
  if (commandChar == 'b') { // both
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_TRANSMISSION_NORMAL_AND_ANONYMOUS;
  } else if (commandChar == 'i') { // inter pan
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_TRANSMISSION_ANONYMOUS;
  } else {
    // Do nothing for 'normal'.
    // MISRA requires ..else if.. to have terminating else.
  }
}

// plugin messaging-server importance <low | medium | high | critical>
void emAfMessagingServerCliImportance(void)
{
  uint8_t commandChar = emberCurrentCommand->name[0];
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_IMPORTANCE_MASK;
  if (commandChar == 'm') { // medium
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_MEDIUM;
  } else if (commandChar == 'h') { // high
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_HIGH;
  } else if (commandChar == 'c') { // critical
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_IMPORTANCE_CRITICAL;
  } else {
    // Do nothing for 'low' importance.
    // MISRA requires ..else if.. to have terminating else.
  }
}

// plugin messaging-server confirm <not | req>
void emAfMessagingServerCliConfirm(void)
{
  uint8_t commandChar = emberCurrentCommand->name[0];
  message.messageControl &= ~ZCL_MESSAGING_CLUSTER_CONFIRMATION_MASK;
  if (commandChar == 'r') { // required
    message.messageControl |= EMBER_ZCL_MESSAGING_CONTROL_CONFIRMATION_REQUIRED;
  }
  // Do nothing for 'not' (not required).
}

// plugin messaging-server <valid | invalid>
void emAfMessagingServerCliValid(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPluginMessagingServerSetMessage(endpoint,
                                         emberCurrentCommand->name[0] == 'v'
                                         ? &message
                                         : NULL);
}

// plugin messaging-server display <nodeId:2> <srcEndpoint:1> <dstEndpoint:1>
void emAfMessagingServerCliDisplay(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  emberAfPluginMessagingServerDisplayMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint);
}

// plugin messaging-server cancel <nodeId:2> <srcEndpoint:1> <dstEndpoint:1>
void emAfMessagingServerCliCancel(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  emberAfPluginMessagingServerCancelMessage(nodeId,
                                            srcEndpoint,
                                            dstEndpoint);
}

// plugin messaging-server print <endpoint:1>
void emAfMessagingServerCliPrint(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  emAfPluginMessagingServerPrintInfo(endpoint);
}
#endif
