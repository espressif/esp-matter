/***************************************************************************//**
 * @file
 * @brief Definitions for the Update TC Link Key plugin.
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

#ifndef SILABS_ZIGBEE_UPDATE_TC_LINK_KEY_H
#define SILABS_ZIGBEE_UPDATE_TC_LINK_KEY_H

/**
 * @defgroup update-tc-link-key Update TC Link Key
 * @ingroup component
 * @brief API and Callbacks for the Update TC Link Key Component
 *
 * This component provides the functionality to update the trust center
 * link key of a device on a Zigbee R21+ network. The device also requests
 * the trust center link key periodically if the trust center is pre-R21.
 * After the trust center is upgraded to R21+, if not already R21+, the periodic
 * request for a trust center link key update passes and the link key
 * is updated.
 *
 */

/**
 * @addtogroup update-tc-link-key
 * @{
 */

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME "Update TC Link Key"

/**
 * @name API
 * @{
 */

// -----------------------------------------------------------------------------
// API

/* @brief Start a link key update process.
 *
 * Kicks off a link key update process.
 *
 * @return An ::EmberStatus value. If the current node is not on a network,
 * this will return ::EMBER_NOT_JOINED. If the current node is on a
 * distributed security network, this will return
 * ::EMBER_SECURITY_CONFIGURATION_INVALID. If the current node is the
 * trust center, this will return ::EMBER_INVALID_CALL.
 */
EmberStatus emberAfPluginUpdateTcLinkKeyStart(void);

/* @brief Stop a link key update process.
 *
 * Stops a link key update process.
 *
 * @return Whether or not a TCLK update was in progress.
 */
bool emberAfPluginUpdateTcLinkKeyStop(void);

/** @brief Change the time between two subsequent update TC link key calls*/
void emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(uint32_t timeInMilliseconds);

/* @brief Set the delay until the next request is made to update the trust
 * center link key.
 */
void emberAfPluginUpdateTcLinkKeySetDelay(uint32_t delayMs);

/* @brief Stop the periodic TC link key update process.
 */
void emberAfPluginUpdateTcLinkKeySetInactive(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup update_tc_link_key_cb Update TC Link Key Update
 * @ingroup af_callback
 * @brief Callbacks for Update TC Link Key Update Component
 *
 */

/**
 * @addtogroup update_tc_link_key_cb
 * @{
 */

/** @brief Status.
 *
 * This callback is fired when the Update Link Key exchange process is updated
 * with a status from the stack. Implementations will know that the Update TC
 * Link Key plugin has completed its link key request when the keyStatus
 * parameter is EMBER_VERIFY_LINK_KEY_SUCCESS.
 *
 * @param keyStatus An ::EmberKeyStatus value describing the success or failure
 * of the key exchange process. Ver.: always
 */
void emberAfPluginUpdateTcLinkKeyStatusCallback(EmberKeyStatus keyStatus);

/** @} */ // end of update_tc_link_key_cb
/** @} */ // end of name Callbacks
/** @} */ // end of update-tc-link-key

#endif // SILABS_ZIGBEE_UPDATE_TC_LINK_KEY_H
