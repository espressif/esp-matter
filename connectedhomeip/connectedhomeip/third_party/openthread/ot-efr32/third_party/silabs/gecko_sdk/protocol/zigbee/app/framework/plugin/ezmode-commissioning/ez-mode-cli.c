/***************************************************************************//**
 * @file
 * @brief CLI for the EZ-Mode plugin.
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
#include "ez-mode.h"

#ifdef UC_BUILD
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "ezmode-commissioning-config.h"
#define MAX_CLUSTER_IDS 3
static EmberAfClusterId clusterIds[MAX_CLUSTER_IDS];

// plugin ezmode-commissioning client <endpoint:1> <direction:1> <cluster ids:n>
void emberAfPluginEzModeCommissioningClientCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  EmberAfEzModeCommissioningDirection direction = (EmberAfEzModeCommissioningDirection)sl_cli_get_argument_uint8(arguments, 1);
  uint8_t clusterIdLength;
  uint8_t count = sl_cli_get_argument_count(arguments) - 2;
  if (MAX_CLUSTER_IDS < count) {
    count = MAX_CLUSTER_IDS;
    emberAfAppPrintln("Cluster ids truncated to %d", count);
  }
  for (clusterIdLength = 0; clusterIdLength < count; clusterIdLength++) {
    clusterIds[clusterIdLength]
      = (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, clusterIdLength + 2);
  }
  status = emberAfEzmodeClientCommission(endpoint,
                                         direction,
                                         clusterIds,
                                         clusterIdLength);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "client", status);
}

// plugin ezmode-commissioning server <endpoint:1> <identify time:1>
void emberAfPluginEzModeCommissioningServerCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint16_t identifyTimeS = sl_cli_get_argument_uint16(arguments, 1);
  if (identifyTimeS == 0) {
    identifyTimeS = EMBER_AF_PLUGIN_EZMODE_COMMISSIONING_IDENTIFY_TIMEOUT;
  }
  status = emberAfEzmodeServerCommissionWithTimeout(endpoint, identifyTimeS);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "server", status);
}

#else //UC_BUILD

#ifdef EMBER_AF_LEGACY_CLI
  #error The EZ-Mode Commissioning plugin is not compatible with the legacy CLI.
#endif

#define MAX_CLUSTER_IDS 3
static EmberAfClusterId clusterIds[MAX_CLUSTER_IDS];

// plugin ezmode-commissioning client <endpoint:1> <direction:1> <cluster ids:n>
void emberAfPluginEzModeCommissioningClientCommand(void)
{
  EmberStatus status;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfEzModeCommissioningDirection direction = (EmberAfEzModeCommissioningDirection)emberUnsignedCommandArgument(1);
  uint8_t clusterIdLength;
  uint8_t count = emberCommandArgumentCount() - 2;
  if (MAX_CLUSTER_IDS < count) {
    count = MAX_CLUSTER_IDS;
    emberAfAppPrintln("Cluster ids truncated to %d", count);
  }
  for (clusterIdLength = 0; clusterIdLength < count; clusterIdLength++) {
    clusterIds[clusterIdLength]
      = (EmberAfClusterId)emberUnsignedCommandArgument(clusterIdLength + 2);
  }
  status = emberAfEzmodeClientCommission(endpoint,
                                         direction,
                                         clusterIds,
                                         clusterIdLength);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "client", status);
}

// plugin ezmode-commissioning server <endpoint:1> <identify time:1>
void emberAfPluginEzModeCommissioningServerCommand(void)
{
  EmberStatus status;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint16_t identifyTimeS = (uint16_t)emberUnsignedCommandArgument(1);
  if (identifyTimeS == 0) {
    identifyTimeS = EMBER_AF_PLUGIN_EZMODE_COMMISSIONING_IDENTIFY_TIMEOUT;
  }
  status = emberAfEzmodeServerCommissionWithTimeout(endpoint, identifyTimeS);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "server", status);
}
#endif
