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

/** @brief Network Found
 *
 * This is called by the form-and-join library to notify the application of the
 * network found after a call to ::emberScanForJoinableNetwork() or
 * ::emberScanForNextJoinableNetwork(). See form-and-join documentation for
 * more information.
 *
 * @param networkFound   Ver.: always
 * @param lqi   Ver.: always
 * @param rssi   Ver.: always
 */
WEAK(void emberAfPluginFormAndJoinNetworkFoundCallback(EmberZigbeeNetwork *networkFound,
                                                       uint8_t lqi,
                                                       int8_t rssi))
{
}

/** @brief Unused Pan Id Found
 *
 * This function is called when the form-and-join library finds an unused PAN
 * ID that can be used to form a new network on.
 *
 * @param panId A randomly generated PAN ID without other devices on it.
 * Ver.: always
 * @param channel The channel where the PAN ID can be used to form a new
 * network. Ver.: always
 */
WEAK(void emberAfPluginFormAndJoinUnusedPanIdFoundCallback(EmberPanId panId,
                                                           uint8_t channel))
{
}
