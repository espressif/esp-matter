/***************************************************************************//**
 * @file
 * @brief CLI for the Device Query Service plugin.
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
#include "app/framework/util/af-main.h"
#include "app/framework/plugin/device-query-service/device-query-service.h"
#include "app/framework/plugin/device-database/device-database.h"

//============================================================================
// Globals

//============================================================================
// Forward declarations

//============================================================================
#ifdef UC_BUILD
void emAfPluginDeviceQueryServiceEnableDisableCommand(sl_cli_command_arg_t *arguments)
{
  emberAfPluginDeviceQueryServiceEnableDisable(memcmp(arguments->argv[arguments->arg_ofs - 1], "enable", strlen("enable")) == 0);
}

void emAfPluginDeviceQueryServiceStatusCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 currentEui64;
  bool enabled = emberAfPluginDeviceQueryServiceGetEnabledState();
  emberAfPluginDeviceQueryServiceGetCurrentDiscoveryTargetEui64(currentEui64);
  emberAfCorePrintln("Enabled: %p", enabled ? "yes" : "no");
  emberAfCorePrint("Current Discovery Target: ");
  emberAfPrintBigEndianEui64(currentEui64);
  emberAfCorePrintln("");
  const EmberAfDeviceInfo* device = emberAfPluginDeviceDatabaseFindDeviceByEui64(currentEui64);
  emberAfCorePrintln("Status: %p",
                     (device == NULL
                      ? ""
                      : emberAfPluginDeviceDatabaseGetStatusString(device->status)));
}
#else
void emAfPluginDeviceQueryServiceEnableDisableCommand(void)
{
  emberAfPluginDeviceQueryServiceEnableDisable(emberCommandName()[0] == 'e');
}

void emAfPluginDeviceQueryServiceStatusCommand(void)
{
  EmberEUI64 currentEui64;
  bool enabled = emberAfPluginDeviceQueryServiceGetEnabledState();
  emberAfPluginDeviceQueryServiceGetCurrentDiscoveryTargetEui64(currentEui64);
  emberAfCorePrintln("Enabled: %p", enabled ? "yes" : "no");
  emberAfCorePrint("Current Discovery Target: ");
  emberAfPrintBigEndianEui64(currentEui64);
  emberAfCorePrintln("");
  const EmberAfDeviceInfo* device = emberAfPluginDeviceDatabaseFindDeviceByEui64(currentEui64);
  emberAfCorePrintln("Status: %p",
                     (device == NULL
                      ? ""
                      : emberAfPluginDeviceDatabaseGetStatusString(device->status)));
}
#endif
