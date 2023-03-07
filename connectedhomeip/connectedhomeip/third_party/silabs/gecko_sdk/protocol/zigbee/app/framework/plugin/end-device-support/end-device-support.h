/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Polling plugin.
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

#ifndef SL_ZIGBEE_END_DEVICE_SUPPORT_H
#define SL_ZIGBEE_END_DEVICE_SUPPORT_H

/**
 * @defgroup end-device-support  End Device Support
 * @ingroup component
 * @brief API and Callbacks for the End Device Support Component
 *
 * Silicon Labs implementation of polling for end devices.
 * Note that the Poll Control Server component sets the long and short
 * poll intervals at startup.  If both components are enabled, the
 * long and short poll intervals set here will not be used.
 *
 */

/**
 * @addtogroup end-device-support
 * @{
 */

#ifdef UC_BUILD
#include "end-device-support-config.h"
#if (EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ENABLE_POLL_COMPLETED_CALLBACK == 1)
#define ENABLE_POLL_COMPLETED_CALLBACK
#endif
#if (EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY == 1)
#define ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ENABLE_POLL_COMPLETED_CALLBACK
#define ENABLE_POLL_COMPLETED_CALLBACK
#endif
#ifdef EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY
#define ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY
#endif
#endif // UC_BUILD

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern bool emAfEnablePollCompletedCallback;

typedef struct {
  uint32_t pollIntervalMs;
  uint8_t numPollsFailing;
} EmAfPollingState;

extern EmAfPollingState emAfPollingStates[];
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @name Handlers
 * @{
 */

void emberAfPluginEndDeviceSupportMoveNetworkEventHandler(SLXU_UC_EVENT);

/** @} */ // end of name Handlers

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup end_device_support_cb End Device Support
 * @ingroup af_callback
 * @brief Callbacks for End Device Support Component
 *
 */

/**
 * @addtogroup end_device_support_cb
 * @{
 */

/** @brief Poll is completed.
 *
 * This function is called by the End Device Support plugin after a poll is
 * completed.
 *
 * @param status Return status of a completed poll operation Ver.: always
 */
void emberAfPluginEndDeviceSupportPollCompletedCallback(EmberStatus status);

/** @brief Parent connectivity is lost.
 *
 * This function is called by the End Device Support plugin when the end device
 * can't connect to its parent. This callback is called under the following
 * conditions:
 * - When the network state of the device is EMBER_JOINED_NETWORK_NO_PARENT.
 * - If there are 3 or more data poll transaction failures for an end device.
 *
 * If this callback returns false, the End Device Support plugin code will
 * proceed with issuing a Trust Center rejoin. Otherwise, if this function
 * returns true, the End Device Support plugin will not issue a Trust Center
 * rejoin.
 */
bool emberAfPluginEndDeviceSupportLostParentConnectivityCallback(void);

/** @brief Pre network move.
 *
 * This function is called by the End Device Support plugin just before it
 * issues a network move, or a sequence of a secure rejoin followed by several
 * trust center rejoins. Various plugins issue a network move when certain
 * conditions are met, such as when the network state is EMBER_JOINED_NO_PARENT.
 *
 * Users can consume this callback and return true to prevent the End Device
 * Support plugin from issuing rejoin requests. Consuming the callback also
 * gives the application a chance to implement its own rejoin logic. If this
 * callback returns false, the End Device Support plugin will issue network
 * rejoins as usual.
 */
bool emberAfPluginEndDeviceSupportPreNetworkMoveCallback(void);
/** @} */ // end of end_device_support_cb
/** @} */ // end of name Callbacks
/** @} */ // end of end-device-support

void emAfPollCompleteHandler(EmberStatus status, uint8_t limit);
void emAfPluginEndDeviceSupportMoveInit(void);
void emAfPluginEndDeviceSupportPollingInit(void);

#endif // SL_ZIGBEE_END_DEVICE_SUPPORT_H
