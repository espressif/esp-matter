/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Creator Security plugin.
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

/**
 * @defgroup network-creator-security  Network Creator Security
 * @ingroup component
 * @brief API and Callbacks for the Network Creator Security Component
 *
 * This component performs the necessary security initialization to form a
 * Zigbee 3.0-compliant network. To enable
 * "Allow Trust Center rejoin with well known key", the
 * "emberAfPluginEzspSecurityTrustCenterJoinCallback()" function (on the NCP side)
 * may need to be implemented. For more details about this callback function,
 * see
 * "https://www.silabs.com/community/wireless/zigbee-and-thread/knowledge-base.entry.html/2018/12/26/how_to_allow_unsecur-sa3C".
 * If the link does not work, go to our community website and search
 * "How to allow unsecure rejoins on the Ember ZNet stack".
 *
 */

/**
 * @addtogroup network-creator-security
 * @{
 */

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME "NWK Creator Security"

// -----------------------------------------------------------------------------
// Globals

extern bool allowHaDevices;

/**
 * @name API
 * @{
 */

// -----------------------------------------------------------------------------
// API

/** @brief Initialize the security needed for forming and then operating on
 * a network.
 *
 * The centralizedNetwork parameter allows the caller to specify
 * whether or not the network that they plan to form will use centralized or
 * distributed security.
 *
 * @param centralizedNetwork Whether or not the network that the caller plans
 * to form will use centralized or distributed security.
 *
 * @return Status of the commencement of the network creator process.
 */
EmberStatus emberAfPluginNetworkCreatorSecurityStart(bool centralizedNetwork);

/** @brief Open a network for joining.
 *
 * This API broadcasts a permit join to
 * the network and adds a transient link key of ZigBeeAlliance09
 * if this device is a trust center.
 *
 * @return An ::EmberStatus value describing the success or failure of the
 * network opening procedure. If this node is not currently on a network,
 * this will return ::EMBER_ERR_FATAL.
 */
EmberStatus emberAfPluginNetworkCreatorSecurityOpenNetwork(void);

/** @brief Close the network.
 *
 * This API closes the network for joining. It broadcasts a permit join
 * to the network with time 0, as well as clears any transient link keys in
 * the stack.
 *
 * @return An ::EmberStatus value describing closing the network. If this node
 * is not currently on a network, this will return ::EMBER_ERR_FATAL. This
 * API will also return an error code based on the success or failure of the
 * broadcast permit join.
 */
EmberStatus emberAfPluginNetworkCreatorSecurityCloseNetwork(void);

/** @brief Set the network to accept the install code only.
 *
 * This API opens a network for joining. It broadcasts a permit join to
 * the network and adds a specified EUI64 and transient key pair if this
 * device is a trust center. Only the node that matches the specified key pair
 * is allowed to join the network.
 *
 * @param eui64 EUI 64 of the joining node.
 *
 * @param keyData the link key to be used by the joining node.
 *
 * @return An ::EmberStatus value describing the success or failure of the
 * network opening procedure. If this node is not currently on a network,
 * this will return ::EMBER_ERR_FATAL.
 */
EmberStatus emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(EmberEUI64 eui64,
                                                                      EmberKeyData keyData);

/** @brief Set the insecure rejoin policy for devices using the well-known key.
 *
 * This API sets the policy for Trust Center (insecure) rejoins for devices
 * using the well-known key. Sending a network key to devices using the well-
 * known key is a security risk and the default policy of the stack is to deny
 * such rejoins. Calling this function to allow rejoins with the well-known key
 * will allow the rejoins for a period of
 * emAllowTcRejoinsUsingWellKnownKeyTimeoutSec seconds.
 *
 * @return An ::EmberStatus value describing the success or failure of the
 * policy change. If this node is not currently on a network,
 * this will return ::EMBER_ERR_FATAL.
 */
EmberStatus emberAfPluginNetworkCreatorSecuritySetAllowRejoinsWithWellKnownKey(bool allow);

/** @} */ // end of name API
/** @} */ // end of network-creator-security
