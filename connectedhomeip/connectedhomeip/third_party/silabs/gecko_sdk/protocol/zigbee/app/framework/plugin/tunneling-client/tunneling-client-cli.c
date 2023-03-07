/***************************************************************************//**
 * @file
 * @brief CLI for the Tunneling Client plugin.
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
#include "tunneling-client.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"

#ifdef UC_BUILD
void emAfPluginTunnelingClientCliPrintCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginTunnelingClientCliPrint();
}
// plugin tunneling-client request <server:2> <client endpoint:1> <server endpoint:1> <protocol id:1> <manufacturer code:2> <flow control support:1>
void emAfPluginTunnelingClientCliRequest(sl_cli_command_arg_t *arguments)
{
  EmberNodeId server = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t clientEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t serverEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t protocolId = sl_cli_get_argument_uint8(arguments, 3);
  uint16_t manufacturerCode = sl_cli_get_argument_uint16(arguments, 4);
  bool flowControlSupport = (bool)sl_cli_get_argument_uint8(arguments, 5);
  EmberAfPluginTunnelingClientStatus status = emberAfPluginTunnelingClientRequestTunnel(server,
                                                                                        clientEndpoint,
                                                                                        serverEndpoint,
                                                                                        protocolId,
                                                                                        manufacturerCode,
                                                                                        flowControlSupport);
  emberAfTunnelingClusterPrintln("%p 0x%x", "request", status);
}

// plugin tunneling-client transfer <tunnel index:1> <data>
void emAfPluginTunnelingClientCliTransfer(sl_cli_command_arg_t *arguments)
{
  uint8_t tunnelIndex = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t data[255];
  uint16_t dataLen = sl_zigbee_copy_string_arg(arguments, 1, data, sizeof(data), false);
  EmberAfStatus status = emberAfPluginTunnelingClientTransferData(tunnelIndex,
                                                                  data,
                                                                  dataLen);
  emberAfTunnelingClusterPrintln("%p 0x%x", "transfer", status);
}

// plugin tunneling-client close <tunnel index:1>
void emAfPluginTunnelingClientCliClose(sl_cli_command_arg_t *arguments)
{
  uint8_t tunnelIndex = sl_cli_get_argument_uint8(arguments, 0);
  EmberAfStatus status = emberAfPluginTunnelingClientCloseTunnel(tunnelIndex);
  emberAfTunnelingClusterPrintln("%p 0x%x", "close", status);
}
#else

#if !defined(EMBER_AF_GENERATE_CLI)
void emAfPluginTunnelingClientCliRequest(void);
void emAfPluginTunnelingClientCliTransfer(void);
void emAfPluginTunnelingClientCliClose(void);
void emAfPluginTunnelingClientCliPrint(void);

EmberCommandEntry emberAfPluginTunnelingClientCommands[] = {
  emberCommandEntryAction("request", emAfPluginTunnelingClientCliRequest, "vuuuvu",
                          "Send a tunnel request command"),
  emberCommandEntryAction("transfer", emAfPluginTunnelingClientCliTransfer, "ub",
                          "Transfer data through a previously setup tunnel"),
  emberCommandEntryAction("close", emAfPluginTunnelingClientCliClose, "u",
                          "Close a tunnel"),
  emberCommandEntryAction("print", emAfPluginTunnelingClientCliPrint, "",
                          "Print the list of tunnels"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin tunneling-client request <server:2> <client endpoint:1> <server endpoint:1> <protocol id:1> <manufacturer code:2> <flow control support:1>
void emAfPluginTunnelingClientCliRequest(void)
{
  EmberNodeId server = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t clientEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t serverEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t protocolId = (uint8_t)emberUnsignedCommandArgument(3);
  uint16_t manufacturerCode = (uint16_t)emberUnsignedCommandArgument(4);
  bool flowControlSupport = (bool)emberUnsignedCommandArgument(5);
  EmberAfPluginTunnelingClientStatus status = emberAfPluginTunnelingClientRequestTunnel(server,
                                                                                        clientEndpoint,
                                                                                        serverEndpoint,
                                                                                        protocolId,
                                                                                        manufacturerCode,
                                                                                        flowControlSupport);
  emberAfTunnelingClusterPrintln("%p 0x%x", "request", status);
}

// plugin tunneling-client transfer <tunnel index:1> <data>
void emAfPluginTunnelingClientCliTransfer(void)
{
  uint8_t tunnelIndex = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t data[255];
  uint16_t dataLen = emberCopyStringArgument(1, data, sizeof(data), false);
  EmberAfStatus status = emberAfPluginTunnelingClientTransferData(tunnelIndex,
                                                                  data,
                                                                  dataLen);
  emberAfTunnelingClusterPrintln("%p 0x%x", "transfer", status);
}

// plugin tunneling-client close <tunnel index:1>
void emAfPluginTunnelingClientCliClose(void)
{
  uint8_t tunnelIndex = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfStatus status = emberAfPluginTunnelingClientCloseTunnel(tunnelIndex);
  emberAfTunnelingClusterPrintln("%p 0x%x", "close", status);
}
#endif // UC_BUILD
