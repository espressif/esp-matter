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

/** @brief Device Removed
 *
 * This callback is called by the plugin when a device is removed from the
 * device log.
 *
 * @param deviceId Identifier of the device removed Ver.: always
 */
WEAK(void emberAfPluginGbcsDeviceLogDeviceRemovedCallback(EmberEUI64 deviceId))
{
  emberAfAppPrint("GBCS emberAfPluginGbcsDeviceLogDeviceRemovedCallback: EUI64=");
  emberAfAppDebugExec(emberAfPrintBigEndianEui64(deviceId));
  emberAfAppPrintln("");
}
