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

/** @brief Join
 *
 * This callback is called by the ZLL Commissioning Network plugin when a joinable
 * network has been found. If the application returns true, the plugin will
 * attempt to join the network. Otherwise, the plugin will ignore the network
 * and continue searching. Applications can use this callback to implement a
 * network blacklist. Note that this callback is not called during touch
 * linking.
 *
 * @param networkFound   Ver.: always
 * @param lqi   Ver.: always
 * @param rssi   Ver.: always
 */
WEAK(bool emberAfPluginZllCommissioningNetworkJoinCallback(EmberZigbeeNetwork *networkFound,
                                                           uint8_t lqi,
                                                           int8_t rssi))
{
  return true;
}
