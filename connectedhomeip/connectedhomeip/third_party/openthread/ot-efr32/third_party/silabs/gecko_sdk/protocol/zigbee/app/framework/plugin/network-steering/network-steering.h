/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Steering plugin.
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

#ifndef SILABS_NETWORK_STEERING_H
#define SILABS_NETWORK_STEERING_H

#ifdef UC_BUILD
#include "network-steering-config.h"
#if (EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS == 1)
#define OPTIMIZE_SCANS
#endif
#if (EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_CALLBACK == 1)
#define RADIO_TX_CALLBACK
#endif
#if (EMBER_AF_PLUGIN_NETWORK_STEERING_TRY_ALL_KEYS == 1)
#define TRY_ALL_KEYS
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS
#define OPTIMIZE_SCANS
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_CALLBACK
#define RADIO_TX_CALLBACK
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_STEERING_TRY_ALL_KEYS
#define TRY_ALL_KEYS
#endif
#endif // UC_BUILD

/**
 * @defgroup network-steering Network Steering
 * @ingroup component
 * @brief API and Callbacks for the Network Steering Component
 *
 * This component performs the necessary steps to join the network of any Zigbee
 * Profile. It first tries to perform a join using an install code using the
 * primary channel mask, and then the secondary channel mask. If that does not
 * work, it then tries to use the default well-known link key (ZigBeeAlliance09)
 * to join on the primary channel mask, and then the secondary channel mask.
 * Finally, if the previous two key types fail, the component tries to use the
 * distributed security link key (D0D1D2...) to join on the primary channel
 * mask, and then the secondary channel mask.
 *
 */

/**
 * @addtogroup network-steering
 * @{
 */

// -----------------------------------------------------------------------------
// Constants
#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern const uint8_t emAfNetworkSteeringPluginName[];
#endif //DOXYGEN_SHOULD_SKIP_THIS

// -----------------------------------------------------------------------------
// Types

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfPluginNetworkSteeringState
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_NONE                         = 0x00,
  // The Use Configured Key states are only run if explicitly configured to do
  // so. See emAfPluginNetworkSteeringSetConfiguredKey()

#ifndef OPTIMIZE_SCANS
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_CONFIGURED      = 0x01,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_CONFIGURED    = 0x02,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_INSTALL_CODE    = 0x03,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_INSTALL_CODE  = 0x04,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_CENTRALIZED     = 0x05,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_CENTRALIZED   = 0x06,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_DISTRIBUTED     = 0x07,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_DISTRIBUTED   = 0x08,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_FINISHED                = 0x09,
#else // !OPTIMIZE_SCANS
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_INSTALL_CODE    = 0x01,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_CENTRALIZED     = 0x02,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_DISTRIBUTED     = 0x03,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_INSTALL_CODE  = 0x04,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_CENTRALIZED   = 0x05,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_DISTRIBUTED   = 0x06,

  // Either the USE_ALL_KEY states are run or the non USE_ALL_KEY states are
  // run, but never both
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_USE_ALL_KEYS    = 0x07,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_USE_ALL_KEYS  = 0x08,

  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_FINISHED                = 0x09,
#endif // OPTIMIZE_SCANS

  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK                  = 0x10,
  EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK                  = 0x20,
};
typedef uint8_t EmberAfPluginNetworkSteeringJoiningState;

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfPluginNetworkSteeringOptions
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIONS_NONE                       = 0x00,
  EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIONS_NO_TCLK_UPDATE             = 0x01,
};
typedef uint8_t EmberAfPluginNetworkSteeringOptions;

typedef struct {
  uint32_t currentChannelMask;
  EmberBeaconIterator beaconIterator;
} NetworkSteeringState;

extern EmberAfPluginNetworkSteeringOptions emAfPluginNetworkSteeringOptionsMask;

// -----------------------------------------------------------------------------
// Globals

/** @brief The first set of channels on which to search for joinable networks. */
extern uint32_t emAfPluginNetworkSteeringPrimaryChannelMask;
/** @brief The second set of channels on which to search for joinable networks. */
extern uint32_t emAfPluginNetworkSteeringSecondaryChannelMask;

/**
 * @name API
 * @{
 */

// -----------------------------------------------------------------------------
// API

/** @brief Initiate a network-steering procedure.
 *
 *
 * If the node is currently on a network, it performs network steering
 * in which it opens up the network with a broadcast
 * permit join message.
 *
 * If the node is not on a network, it scans a series of primary channels
 * (see ::emAfPluginNetworkSteeringPrimaryChannelMask) to find possible
 * networks to join. If it is unable to join any of those networks, it
 * tries scanning on a set of secondary channels
 * (see ::emAfPluginNetworkSteeringSecondaryChannelMask). Upon completion of
 * this process, the plugin calls
 * ::emberAfPluginNetworkSteeringCompleteCallback with information regarding
 * the success or failure of the procedure.
 *
 * This procedure will try to join networks using install codes, the centralized
 * default key, and the distributed default key.
 *
 * @return An ::EmberStatus value that indicates the success or failure of
 * the initiating of the network steering process.
 *
 * @note Do not call this API from a stack status callback, as this plugin acts
 * when its own stack status callback is invoked.
 */
EmberStatus emberAfPluginNetworkSteeringStart(void);

/** @brief Stops the network steering procedure.
 *
 *
 * @return An ::EmberStatus value that indicates the success or failure of
 * the initiating of the network steering process.
 */
EmberStatus emberAfPluginNetworkSteeringStop(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup nw_steering_cb Network Steering
 * @ingroup af_callback
 * @brief Callbacks for Network Steering Component
 *
 */

/**
 * @addtogroup nw_steering_cb
 * @{
 */

/** @brief Complete network steering.
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 *
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 *
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState);

/** @brief Get power for a radio channel.
 *
 * This callback is fired when the Network Steering plugin needs to set the
 * power level. The application has the ability to change the max power level
 * used for this particular channel.
 *
 * @param channel The channel that the plugin is inquiring about the power
 * level. Ver.: always
 */
int8_t emberAfPluginNetworkSteeringGetPowerForRadioChannelCallback(uint8_t channel);

/** @brief Get the distributed key.
 *
 * This callback is fired when the Network Steering plugin needs to set the distributed
 * key. The application set the distributed key from Zigbee Alliance through this callback
 * or the network steering will use the default test key.
 *
 * @param pointer to the distributed key struct
 * @return true if the key is loaded successfully, otherwise false.
 * level. Ver.: always
 */
bool emberAfPluginNetworkSteeringGetDistributedKeyCallback(EmberKeyData * key);

/** @brief Get the node type.
 *
 * This callback allows the application to set the node type that the network
 * steering process will use in joining a network.
 *
 * @param state The current ::EmberAfPluginNetworkSteeringJoiningState.
 *
 * @return An ::EmberNodeType value that the network steering process will
 * try to join a network as.
 */
EmberNodeType emberAfPluginNetworkSteeringGetNodeTypeCallback(EmberAfPluginNetworkSteeringJoiningState state);

/** @} */ // end of nw_steering_cb
/** @} */ // end of name Callbacks
/** @} */ // end of network-steering

/** @brief Override the channel mask. */
void emAfPluginNetworkSteeringSetChannelMask(uint32_t mask, bool secondaryMask);

/** @brief Set extended PAN ID to search for. */
void emAfPluginNetworkSteeringSetExtendedPanIdFilter(uint8_t* extendedPanId,
                                                     bool turnFilterOn);

/** @brief Set a different key to use when joining. */
void emAfPluginNetworkSteeringSetConfiguredKey(uint8_t *key,
                                               bool useConfiguredKey);

/** @brief Clean up the network steering process which took place */
void emAfPluginNetworkSteeringCleanup(EmberStatus status);

#endif /* __NETWORK_STEERING_H__ */
