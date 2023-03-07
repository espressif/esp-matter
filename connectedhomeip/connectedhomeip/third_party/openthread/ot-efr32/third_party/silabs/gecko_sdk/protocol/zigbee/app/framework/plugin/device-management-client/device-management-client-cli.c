/***************************************************************************//**
 * @file
 * @brief CLI for the Device Management Client plugin.
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
#include "device-management-client.h"
#include "app/framework/plugin/device-management-server/device-management-common.h"

#ifdef UC_BUILD

void emAfDeviceManagementClientCliPrint(sl_cli_command_arg_t *arguments)
{
  emberAfDeviceManagementClientPrint();
}

#else
#ifndef EMBER_AF_GENERATE_CLI
  #error The Device Management Client plugin is not compatible with the legacy CLI.
#endif

void emAfDeviceManagementClientCliPrint(void);

void emAfDeviceManagementClientCliPrint(void)
{
  emberAfDeviceManagementClientPrint();
}
#endif
