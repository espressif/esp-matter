/***************************************************************************//**
 * @file
 * @brief Routines for finding and joining any viable network via scanning, rather
 * than joining a specific network.
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

#ifndef NETWORK_FIND_DOT_H_INCLUDED
#define NETWORK_FIND_DOT_H_INCLUDED

/**
 * @defgroup network-find  Network Find
 * @ingroup component
 * @brief API and Callbacks for the Network Find Component
 *
 * NOTE: This component is deprecated in Zigbee 3.0, but is still used for
 * Zigbee Smart Energy requirements. See the Network Creator and Network
 * Steering components instead for Zigbee 3.0-compliant implementations of this
 * behavior. This component provides legacy Zigbee Home Automation (pre-3.0)
 * implementation of routines for finding and joining any viable network via
 * scanning, rather than joining a specific network. If the application will be
 * deployed in an environment with potentially a lot of open networks,
 * consider implementing a mechanism for network deny
 * listing. Network deny listing may be accomplished by implementing
 * the emberAfPluginNetworkFindJoinCallback. To extend the functionality to
 * sub-GHz channels, also enable the Network Find (Sub-GHz) component.

 *
 */

/**
 * @addtogroup network-find
 * @{
 */

#include "app/framework/include/af.h"
#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
#include "network-find-config-se.h"
#else // !SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
#include "network-find-config.h"
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
#endif  // UC_BUILD

/**
 * @name API
 * @{
 */

/** @brief Set the channel mask for "find unused" and "find joinable".
 * Permitted pages are 0 and, if Sub-GHz channels are included, then also 28-31.
 */
EmberStatus emberAfSetFormAndJoinChannelMask(uint8_t page, uint32_t mask);

/** @brief Return the channel mask for a given page.
 * Only the bottom 27 bits can be set. The top 5 bits are reserved for the page
 * number and are always zero in a returned channel mask. That leaves the value
 * 0xFFFFFFFF free to indicate an invalid page error.
 */
uint32_t emberAfGetFormAndJoinChannelMask(uint8_t page);

/** @brief Set the search mode for "find unused" and "find joinable".
 * Mode is a bitmask. Permitted values are set by the FIND_AND_JOIN_MODE_...
 * macros.
 */
EmberStatus emberAfSetFormAndJoinSearchMode(uint8_t mode);

/** @brief Returns the current search mode.
 */
uint8_t emberAfGetFormAndJoinSearchMode(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup nw_find_cb Network Find
 * @ingroup af_callback
 * @brief Callbacks for Network Find Component
 *
 */

/**
 * @addtogroup nw_find_cb
 * @{
 */

/** @brief Finish forming or joining process.
 *
 * This callback is fired when the network-find plugin is finished with the
 * forming or joining process. The result of the operation will be returned in
 * the status parameter.
 *
 * @param status   Ver.: always
 */
void emberAfPluginNetworkFindFinishedCallback(EmberStatus status);

/** @brief Get radio power for a channel.
 *
 * This callback is called by the framework when it is setting the radio power
 * during the discovery process. The framework will set the radio power
 * depending on what is returned by this callback.
 *
 * @param pgChan   Ver.: always
 */
int8_t emberAfPluginNetworkFindGetRadioPowerForChannelCallback(uint8_t pgChan);

/** @brief Join a network.
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
bool emberAfPluginNetworkFindJoinCallback(EmberZigbeeNetwork *networkFound,
                                          uint8_t lqi,
                                          int8_t rssi);

/** @brief Get the energy threshold for a channel.
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
int8_t emberAfPluginNetworkFindGetEnergyThresholdForChannelCallback(uint8_t pgChan);

/** @brief Get enable scanning all channels.
 *
 * Returns true is the fallback on scanning all channels is enabled,
 * false if not.
 */
bool emberAfPluginNetworkFindGetEnableScanningAllChannelsCallback(void);

/** @} */ // end of nw_find_cb
/** @} */ // end of name Callbacks
/** @} */ // end of network-find

//------------------------------------------------------------------------------
// Application Framework Internal Functions
//
// The functions below are non-public internal function used by the application
// framework. They are NOT to be used by the application.

/** @brief Returns true or false depending on whether
 *         the search for an unused network is currently in progress scanning
 *         all channels.
 * @return True if yes, false if the current scan is on preferred channels only.
 */
bool emAfIsCurrentSearchForUnusedNetworkScanningAllChannels(void);

/** @brief Return the channel mask for the current scan.
 * Similar to emberAfGetFormAndJoinChannelMask(), but may return the configured
 * channel mask or all channels mask, depending on the current scan state.
 */
uint32_t emAfGetSearchForUnusedNetworkChannelMask(uint8_t page);

/** @brief Secondary interface formed callback.
 * Called after forming the network to notify the plugin that the secondary
 * interface (in case of a dual-PHY implementation) has been formed. It is used
 * by the plugin to reset its internal state. Strictly for internal use only.
 */
void emAfSecondaryInterfaceFormedCallback(EmberStatus status);

#endif // NETWORK_FIND_DOT_H_INCLUDED
