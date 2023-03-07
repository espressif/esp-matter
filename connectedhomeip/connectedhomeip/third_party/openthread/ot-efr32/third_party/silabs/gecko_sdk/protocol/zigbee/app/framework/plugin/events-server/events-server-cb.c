/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

/** @brief Ok To Clear Log
 *
 * This function is called by the Events server plugin whenever a Clear Event
 * Log Request command is received. The application should return true if it is
 * Ok to clear the given log and false otherwise. If the request is to clear
 * all logs (i.e. logId == EMBER_ZCL_EVENT_LOG_ID_ALL_LOGS) and the application
 * allows all logs to be cleared then the application should return true. If
 * the application does not allow all logs to be cleared it should return false
 * in which case the plugin will subsequently call this callback for each event
 * log allowing the application to selectively choose which event logs are Ok
 * to be cleared.
 *
 * @param logId The identifier of the log requested to be cleared. Ver.: always
 */
WEAK(bool emberAfPluginEventsServerOkToClearLogCallback(EmberAfEventLogId logId))
{
  return true;
}

/** @brief Log Data Updated
 *
 * This function is called by the Events server plugin when any modification to
 * the plugin's event logs has been made. The argument will hint the ZCL
 * command that might be used to triggered the data change. If null, it means
 * the logging data have been updated through other means, e.g. CLI.
 *
 * @param cmd ZCL command Ver.: always
 */
WEAK(void emberAfPluginEventsServerLogDataUpdatedCallback(const EmberAfClusterCommand *cmd))
{
}
