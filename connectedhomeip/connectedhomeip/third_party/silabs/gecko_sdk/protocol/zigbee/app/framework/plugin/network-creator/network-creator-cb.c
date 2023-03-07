/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Creator plugin, which handles forming
 *        a network per the Zigbee Base Device Behavior specification.
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

#include "af.h"
#include "network-creator-config.h"
#include "network-creator.h"

/** @brief Complete
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels Ver.: always
 */
WEAK(void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                      bool usedSecondaryChannels))
{
}

/** @brief Get Pan Id
 *
 * This callback is called when the Network Creator plugin needs the PAN ID for
 * the network it is about to create. By default, the callback will return a
 * random 16-bit value.
 *
 */
WEAK(EmberPanId emberAfPluginNetworkCreatorGetPanIdCallback(void))
{
  return emberGetPseudoRandomNumber();
}

/** @brief Get Power For Radio Channel
 *
 * This callback is called when the Network Creator plugin needs the radio power for
 * the network it is about to create. By default, the callback will use the radio
 * power specified in the relevant plugin option.
 *
 */
WEAK(int8_t emberAfPluginNetworkCreatorGetRadioPowerCallback(void))
{
  return EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER;
}
