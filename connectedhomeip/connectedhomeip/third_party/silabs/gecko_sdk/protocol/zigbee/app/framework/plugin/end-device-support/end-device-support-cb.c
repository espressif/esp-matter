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

/** @brief Poll Completed
 *
 * This function is called by the End Device Support plugin after a poll is
 * completed.
 *
 * @param status Return status of a completed poll operation Ver.: always
 */
WEAK(void emberAfPluginEndDeviceSupportPollCompletedCallback(EmberStatus status))
{
}

/** @brief Lost Parent Connectivity
 *
 * This function is called by the End Device Support plugin when the end device
 * cannot connect to its parent. This callback is called under the following
 * instances:
 * - When the network state of the device is EMBER_JOINED_NETWORK_NO_PARENT.
 * - If there are 3 or more data poll transaction failures for an end device.
 *
 * If this callback returns false, the End Device Support plugin code will
 * proceed with issuing a Trust Center rejoin. Otherwise, if this function
 * returns true, the End Device Support plugin will not issue a Trust Center
 * rejoin.
 */
WEAK(bool emberAfPluginEndDeviceSupportLostParentConnectivityCallback(void))
{
  return false;
}

/** @brief Pre Network Move
 *
 * This function is called by the End Device Support plugin just before it
 * issues a Network Move, or a sequence of a secure rejoin followed by several
 * trust center rejoins. Various plugins issue a Network Move when certain
 * conditions are met, such as when the network state is EMBER_JOINED_NO_PARENT.
 *
 * Users can consume this callback and return true to prevent the End Device
 * Support plugin from issuing rejoin requests. Consuming the callback also
 * gives the application a chance to implement its own rejoin logic. If this
 * callback returns false, the End Device Support plugin will issue network
 * rejoins as usual.
 */
WEAK(bool emberAfPluginEndDeviceSupportPreNetworkMoveCallback(void))
{
  return false;
}
