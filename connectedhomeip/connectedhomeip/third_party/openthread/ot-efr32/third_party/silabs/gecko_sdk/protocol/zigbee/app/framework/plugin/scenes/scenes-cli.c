/***************************************************************************//**
 * @file
 * @brief CLI for the Scenes plugin.
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
#include "scenes.h"

#ifdef UC_BUILD
void sli_plugin_scenes_server_clear(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("Clearing all scenes.");
  emberAfScenesClusterClearSceneTableCallback(EMBER_BROADCAST_ENDPOINT);
}
#else //UC_BUILD
void emAfPluginScenesServerClear(void);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginScenesCommands[] = {
  emberCommandEntryAction("print", emAfPluginScenesServerPrintInfo, "", "Print the scenes table."),
  emberCommandEntryAction("clear", emAfPluginScenesServerClear, "", "Clear the scenes table on every endpoint."),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

void emAfPluginScenesServerClear(void)
{
  emberAfCorePrintln("Clearing all scenes.");
  emberAfScenesClusterClearSceneTableCallback(EMBER_BROADCAST_ENDPOINT);
}
#endif
