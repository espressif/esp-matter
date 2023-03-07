/***************************************************************************//**
 * @file
 * @brief Gateway specific CLI behavior for a host application.
 *   In this case we assume our application is running on
 *   a PC with Unix library support, connected to an NCP via serial uart.
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

// common include file
#include "app/framework/util/common.h"

#include <time.h>

//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------
// Globals

// The difference in seconds between the ZigBee Epoch: January 1st, 2000
// and the Unix Epoch: January 1st 1970.
#define UNIX_ZIGBEE_EPOCH_DELTA (uint32_t)946684800UL

#ifdef UC_BUILD

void emberAfPluginGatewaySupportTimeSyncLocal(sl_cli_command_arg_t *arguments)
{
  time_t unixTime = time(NULL);
  unixTime -= UNIX_ZIGBEE_EPOCH_DELTA;
  emberAfSetTime(unixTime);
  emberAfPrintTime(emberAfGetCurrentTime());
}

#else

#if !defined(EMBER_AF_GENERATE_CLI)

EmberCommandEntry emberAfPluginGatewayCommands[] = {
  emberCommandEntryAction("time-sync-local",
                          emberAfPluginGatewaySupportTimeSyncLocal,
                          "",
                          "This command retrieves the local unix time and syncs the Time Server attribute to it."),
  emberCommandEntryTerminator(),
};

#endif

//------------------------------------------------------------------------------
// Functions

void emberAfPluginGatewaySupportTimeSyncLocal(void)
{
  time_t unixTime = time(NULL);
  unixTime -= UNIX_ZIGBEE_EPOCH_DELTA;
  emberAfSetTime(unixTime);
  emberAfPrintTime(emberAfGetCurrentTime());
}
#endif
