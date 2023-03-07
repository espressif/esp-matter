/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Creator plugin.
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

#ifndef SILABS_NETWORK_CREATOR_H
#define SILABS_NETWORK_CREATOR_H

/**
 * @defgroup network-creator  Network Creator
 * @ingroup component
 * @brief API and Callbacks for the Network Creator Component
 *
 * This component performs the necessary steps to create a network according
 * to the Base Device Behavior specification. The component performs an active scan
 * followed by an energy scan across a primary channel set to decide which
 * channel(s) are valid candidates for network formation. If the component fails to
 * form a network on any primary channels, it moves to a secondary channel mask.
 * Before every attempt at network formation, this component sets up the stack to
 * use Zigbee 3.0 security.
 *
 */

/**
 * @addtogroup network-creator
 * @{
 */

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_NETWORK_CREATOR_PLUGIN_NAME "NWK Creator"

// -----------------------------------------------------------------------------
// Globals

extern uint32_t emAfPluginNetworkCreatorPrimaryChannelMask;
extern uint32_t emAfPluginNetworkCreatorSecondaryChannelMask;

/**
 * @name API
 * @{
 */

// -----------------------------------------------------------------------------
// API

/** @brief Command the network creator to form a network with the following qualities.
 *
 *
 *  @param centralizedNetwork Whether or not to form a network using
 *  centralized security. If this argument is false, a network with
 *  distributed security will be formed.
 *
 *  @return Status of the commencement of the network creator process.
 */
EmberStatus emberAfPluginNetworkCreatorStart(bool centralizedNetwork);

/** @brief Stop the network creator process.
 *
 * Stops the network creator formation process.
 */
void emberAfPluginNetworkCreatorStop(void);

/** @brief Command the network creator to form a centralized network.
 *
 *  Commands the network creator to form a centralized or distributed
 *  network with specified PAN ID, TX power, and channel.
 *
 *  @param centralizedNetwork Indicates whether or not to form a network using
 *  centralized security. If this argument is false, a network with
 *  distributed security will be formed.
 *
 *  @param panId The pan ID of the network to be formed.
 *  @param radioTxPower The TX power of the network to be formed.
 *  @param channel The channel of the network to be formed.
 *
 *  @return Status of the commencement of the network creator process.
 */
EmberStatus emberAfPluginNetworkCreatorNetworkForm(bool centralizedNetwork,
                                                   EmberPanId panId,
                                                   int8_t radioTxPower,
                                                   uint8_t channel);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup nw_creator_cb Network Creator
 * @ingroup af_callback
 * @brief Callbacks for Network Creator Component
 *
 */

/**
 * @addtogroup nw_creator_cb
 * @{
 */

/** @brief Complete the network creation process.
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 *
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels. Ver.: always
 */
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels);

/** @brief Get PAN ID.
 *
 * This callback is called when the Network Creator plugin needs the PAN ID for
 * the network it is about to create. By default, the callback returns a
 * random 16-bit value.
 *
 */
EmberPanId emberAfPluginNetworkCreatorGetPanIdCallback(void);

/** @brief Get power for the radio channel.
 *
 * This callback is called when the Network Creator plugin needs the radio power for
 * the network it is about to create. By default, the callback uses the radio
 * power specified in the relevant plugin option.
 *
 */
int8_t emberAfPluginNetworkCreatorGetRadioPowerCallback(void);

/** @} */ // end of nw_creator_cb
/** @} */ // end of name Callbacks
/** @} */ // end of network-creator

#endif /* __NETWORK_CREATOR_H__ */
