/***************************************************************************//**
 * @file
 * @brief CLI for the Tunneling Server plugin.
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
#include "tunneling-server.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#ifdef UC_BUILD
void emAfPluginTunnelingServerCliPrint(void);
void emAfPluginTunnelingServerPrintCliCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginTunnelingServerCliPrint();
}

void emAfPluginTunnelingServerCliTransfer(sl_cli_command_arg_t *arguments)
{
  uint16_t tunnelIndex =  sl_cli_get_argument_uint16(arguments, 0);
  uint8_t data[255];
  uint16_t dataLen = sl_zigbee_copy_hex_arg(arguments, 1, data, sizeof(data), false);
  EmberAfStatus status = emberAfPluginTunnelingServerTransferData(tunnelIndex,
                                                                  data,
                                                                  dataLen);
  emberAfTunnelingClusterPrintln("%p 0x%x", "transfer", status);
}

void emberAfPluginTunnelingServerToggleBusyCliCommand(sl_cli_command_arg_t *arguments)
{
  emberAfPluginTunnelingServerToggleBusyCommand();
}

#else
void emAfPluginTunnelingServerCliTransfer(void);

#if !defined(EMBER_AF_GENERATE_CLI)

void emAfPluginTunnelingServerCliPrint(void);

EmberCommandEntry emberAfPluginTunnelingServerCommands[] = {
  emberCommandEntryAction("transfer", emAfPluginTunnelingServerCliTransfer, "vb",
                          "Transfer data through the tunnel"),
  emberCommandEntryAction("busy", emberAfPluginTunnelingServerToggleBusyCommand, "",
                          "Toggly the busy status of the tunnel"),
  emberCommandEntryAction("print", emAfPluginTunnelingServerCliPrint, "",
                          "Print the list of tunnels"),
  emberCommandEntryTerminator(),
};

#endif // EMBER_AF_GENERATE_CLI

// plugin tunneling-server transfer <tunnel index:2> <data>
void emAfPluginTunnelingServerCliTransfer(void)
{
  uint16_t tunnelIndex = (uint16_t)emberUnsignedCommandArgument(0);
  uint8_t data[255];
  uint16_t dataLen = emberCopyStringArgument(1, data, sizeof(data), false);
  EmberAfStatus status = emberAfPluginTunnelingServerTransferData(tunnelIndex,
                                                                  data,
                                                                  dataLen);
  emberAfTunnelingClusterPrintln("%p 0x%x", "transfer", status);
}

void emberAfPluginTunnelingServerToggleBusyCliCommand(void)
{
  emberAfPluginTunnelingServerToggleBusyCommand();
}
#endif //UC_BUILD
