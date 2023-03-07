/***************************************************************************//**
 * @file
 * @brief This file defines the standalone bootloader server CLI.
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
#include "app/framework/include/af-types.h"
#include "app/framework/plugin/standalone-bootloader-common/bootloader-protocol.h"
#include "standalone-bootloader-server.h"
#include "app/framework/plugin/ota-common/ota-cli.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"

//------------------------------------------------------------------------------
// Forward declarations
#ifdef UC_BUILD
void sendLaunchRequestToTarget(sl_cli_command_arg_t *arguments)
{
  uint8_t index = sl_cli_get_argument_uint8(arguments, 0);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  sl_cli_get_argument_uint8(arguments, 1);

  emberAfPluginStandaloneBootloaderServerStartClientBootloadWithCurrentTarget(&id, tag);
}

void sendLaunchRequestToEui(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 longId;
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  sl_cli_get_argument_uint8(arguments, 2);
  sl_zigbee_copy_eui64_arg(arguments, 0, longId, false);

  emberAfPluginStandaloneBootloaderServerStartClientBootload(longId, &id, tag);
}

#else //UC_BUILD
void sendLaunchRequestToEui(void);
void sendLaunchRequestToTarget(void);

//------------------------------------------------------------------------------
// Globals

#ifndef EMBER_AF_GENERATE_CLI
static EmberCommandEntry bootloadCommands[] = {
  emberCommandEntryAction("target", sendLaunchRequestToTarget, "uu",
                          "Sends a launch bootloader request to the currently cached target"),
  emberCommandEntryAction("eui", sendLaunchRequestToEui, "buu",
                          "Send a launch bootloader request to the specified EUI64"),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginStandaloneBootloaderServerCommands[] = {
  emberCommandEntryAction("status", emAfStandaloneBootloaderServerPrintStatus, "",
                          "Prints the current status of the server"),
  emberCommandEntryAction("query", (CommandAction)emberAfPluginStandaloneBootloaderServerBroadcastQuery, "",
                          "Sends a broadcast standalone bootloader query."),
  emberCommandEntrySubMenu("bootload", bootloadCommands,
                           "Sends a request to launch the standalone bootloader"),
  emberCommandEntryAction("print-target", emAfStandaloneBootloaderServerPrintTargetClientInfoCommand, "",
                          "Print the cached info about the current bootload target"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

//------------------------------------------------------------------------------
// Functions

void sendLaunchRequestToTarget(void)
{
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  (uint8_t)emberUnsignedCommandArgument(1);

  emberAfPluginStandaloneBootloaderServerStartClientBootloadWithCurrentTarget(&id, tag);
}

void sendLaunchRequestToEui(void)
{
  EmberEUI64 longId;
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(1);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  (uint8_t)emberUnsignedCommandArgument(2);
  emberCopyEui64Argument(0, longId);

  emberAfPluginStandaloneBootloaderServerStartClientBootload(longId, &id, tag);
}
// *****************************************************************************
#endif
