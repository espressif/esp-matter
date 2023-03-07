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
#include "network-find.h"
#include "sl_component_catalog.h"

/** @brief Finished
 *
 * This callback is fired when the network-find plugin is finished with the
 * forming or joining process. The result of the operation will be returned in
 * the status parameter.
 *
 * @param status   Ver.: always
 */
WEAK(void emberAfPluginNetworkFindFinishedCallback(EmberStatus status))
{
}

/** @brief Get Radio Power For Channel
 *
 * This callback is called by the framework when it is setting the radio power
 * during the discovery process. The framework will set the radio power
 * depending on what is returned by this callback.
 *
 * @param pgChan   Ver.: always
 */
WEAK(int8_t emberAfPluginNetworkFindGetRadioPowerForChannelCallback(uint8_t pgChan))
{
  return EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER;
}

/** @brief Join
 *
 * This callback is called by the plugin when a joinable network has been
 * found. If the application returns true, the plugin will attempt to join the
 * network. Otherwise, the plugin will ignore the network and continue
 * searching. Applications can use this callback to implement a network
 * blacklist.
 *
 * @param networkFound   Ver.: always
 * @param lqi   Ver.: always
 * @param rssi   Ver.: always
 */
WEAK(bool emberAfPluginNetworkFindJoinCallback(EmberZigbeeNetwork *networkFound,
                                               uint8_t lqi,
                                               int8_t rssi))
{
  return true;
}

/** @brief Get Energy Threshold For Channel
 *
 * This callback is called during the energy scan when forming the network.
 * Should the energy level exceed the value returned by this callback, the
 * current channel will not be considered a suitable candidate for forming.
 * Should none of the channels in the channel mask be considered suitable,
 * the scan will fall back on all channels, including those not on the
 * channel mask. The return value is RSSI, in dBm.
 * This callback is called only when the fallback functionality is enabled.
 *
 * @param pgChan   Ver.: always
 */
WEAK(int8_t emberAfPluginNetworkFindGetEnergyThresholdForChannelCallback(uint8_t pgChan))
{
#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT)
  return emberAfGetPageFrom8bitEncodedChanPg(pgChan) == 0
         ? EMBER_AF_PLUGIN_NETWORK_FIND_CUT_OFF_VALUE
         : EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_CUT_OFF_VALUE;
#else // !SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  return EMBER_AF_PLUGIN_NETWORK_FIND_CUT_OFF_VALUE;
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
}

/** @brief Get Enable Scanning All Channels
 *
 * Returns true is the fallback on scanning all channels is enabled,
 * false if not.
 */
WEAK(bool emberAfPluginNetworkFindGetEnableScanningAllChannelsCallback(void))
{
#if (EMBER_AF_PLUGIN_NETWORK_FIND_ENABLE_ALL_CHANNELS == 1)
  return true;
#else
  return false;
#endif
}
