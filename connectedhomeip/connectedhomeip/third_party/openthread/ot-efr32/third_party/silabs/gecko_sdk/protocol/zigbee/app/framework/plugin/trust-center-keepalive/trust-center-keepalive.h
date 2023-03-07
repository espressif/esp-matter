/***************************************************************************//**
 * @file
 * @brief Definitions for the Trust Center Keepalive plugin.
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

#ifndef SILABS_TRUST_CENTER_KEEPALIVE_H
#define SILABS_TRUST_CENTER_KEEPALIVE_H

/**
 * @defgroup trust-center-keepalive Trust Center Keepalive
 * @ingroup component
 * @brief API and Callbacks for the Trust Center Keepalive Component
 *
 * Silicon Labs implementation of Trust Center Keepalive for SE 1.2b
 * The component periodically sends keepalive signals to the trust center
 * to verify that it is accessible.  If the trust center fails to acknowledge
 * a series of keepalive signals, the component searches for another instance
 * of the trust center on a different channel or short PAN ID.  The frequency
 * with which the component sends the keepalive signals has a default, but is
 * ultimately determined by the attributes on the Trust Center.
 * This component periodically reads the "TC Keep-Alive Base" and
 * "TC Keep-Alive Jitter" to determine the rate that it will send
 * future keepalives. Trust Center Keepalive is part of the optional
 * Trust Center Swap-Out feature of Smart Energy 1.1.  Devices are not
 * required to implement this functionality. The trust center does not
 * send keepalives, so this component should be disabled if the device is
 * acting as the trust center.
 *
 */

/**
 * @addtogroup trust-center-keepalive
 * @{
 */

// The duration in milliseconds to wait between two successive keepalives.  The
// period shall be between five and 20 minutes, according to section 5.4.2.2.3.4
// of 105638r09.  The plugin option, specified in minutes, is converted here to
// milliseconds.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DELAY_INTERVAL \
  (EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_INTERVAL * MILLISECOND_TICKS_PER_MINUTE)

// The number of unacknowledged keepalives permitted before declaring that the
// trust center is inaccessible and initiating a search for it.  Section
// 5.4.2.2.3.4 of 105638r09 specifies that this value shall be three.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_FAILURE_LIMIT 3

/**
 * @name API
 * @{
 */

/** @brief Tick the network event.
 *
 * @param SLXU_UC_EVENT Ver.: always
 *
 */
void emberAfPluginTrustCenterKeepaliveTickNetworkEventHandler(SLXU_UC_EVENT);

/** @brief Enable keep alive.
 *
 */
void emberAfPluginTrustCenterKeepaliveEnable(void);

/** Disable keep alive.
 *
 */
void emberAfPluginTrustCenterKeepaliveDisable(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup trust_center_keepalive_cb Trust Center Keepalive
 * @ingroup af_callback
 * @brief Callbacks for Trust Center Keepalive Component
 *
 */

/**
 * @addtogroup trust_center_keepalive_cb
 * @{
 */

/** @brief Overwrite default timing parameters.
 * This function is called by the Trust Center Keep Alive plugin when the Trust
 * Center Keep Alive process is started.
 * Applications may consume this callback and have it return true to
 * have the Trust Center Keep Alive plugin code to start the keep alive process
 * with timing parameters other than the default values. If this callback returns
 * false, the Trust Center Keep Alive plugin code will proceed to start with the
 * default timing parameters.
 *
 * @param baseTimeSeconds, base time for keep alive signalling to be set in seconds
 *
 * @param jitterTimeSeconds, jitter time for keep alive signalling to be set in seconds
 *
 * @return bool
 */
bool emberAfTrustCenterKeepaliveOverwriteDefaultTimingCallback(uint16_t *baseTimeSeconds,
                                                               uint16_t *jitterTimeSeconds);

/** @brief Read attributes response callback.
 *
 * @param buffer Ver.: always
 * @param bufLen Ver.: always
 *
 */
void emAfPluginTrustCenterKeepaliveReadAttributesResponseCallback(uint8_t *buffer,
                                                                  uint16_t bufLen);

/** @brief Timeout.
 *
 * This function is called by the Trust Center Keepalive plugin when the Trust
 * Center fails to acknowledge enough keepalive messages. Upon timing out,
 * the Trust Center Keepalive plugin code will initiate a search for a new
 * Trust Center, which falls in line with the Trust Center Swapout feature.
 * Applications may consume this callback and have it return true in order to
 * prevent the Trust Center Keepalive plugin code from issuing a Trust Center
 * (insecure) rejoin. Doing so will also give the application a chance to
 * implement its own rejoin algorithm or logic. If this callback returns false,
 * the Trust Center Keepalive plugin code will proceed with issuing a Trust
 * Center rejoin.
 *
 * @return bool
 *
 */
bool emberAfPluginTrustCenterKeepaliveTimeoutCallback(void);

/** @brief Connectivity established.
 * This function is called by the Trust Center Keepalive plugin when a read
 * attribute response command from the trust center is received after trust
 * center connectivity had previously been lost. This function is also called
 * after a match descriptor response.
 */
void emberAfPluginTrustCenterKeepaliveConnectivityEstablishedCallback(void);

/** @brief Serverless Keepalive supported.
 * This function is called by the Trust Center Keep Alive plugin when service
 * discovery receives a response indicating that the server does not support the
 * cluster.
 * Applications may consume this callback and have it return true to
 * have the Trust Center Keep Alive plugin code to still start the keep alive
 * process even if the server cluster is not discovered. If this callback returns
 * false, the Trust Center Keep Alive plugin code will only proceed to start the
 * process in case the service discovery was successful.
 *
 * @return bool
 *
 */
bool emberAfPluginTrustCenterKeepaliveServerlessIsSupportedCallback(void);

/** @brief Serverless Keepalive enabled.
 * This function is called by the Trust Center Keep Alive plugin when a service
 * discovery is done, a response has arrived and
 * emberAfPluginTrustCenterKeepaliveServerlessIsSupportedCallback() returned true.
 * Applications may consume this callback and have it return true to
 * have the Trust Center Keep Alive plugin code to start the keep alive process
 * right away. If this callback returns false, the Trust Center Keep Alive plugin
 * code will return to the disabled state waiting to be enabled.
 *
 * @return bool
 *
 */
bool emberAfTrustCenterKeepaliveServerlessIsEnabledCallback(void);

/** @} */ // end of trust_center_keepalive_cb
/** @} */ // end of name Callbacks
/** @} */ // end of trust-center-keepalive

bool emAfPluginTrustCenterKeepaliveTcHasServerCluster(void);

void emAfSendKeepaliveSignal(void);

uint8_t emAfPluginTrustCenterKeepaliveGetBaseTimeMinutes(void);

uint16_t emAfPluginTrustCenterKeepaliveGetBaseTimeSeconds(void);

uint16_t emAfPluginTrustCenterKeepaliveGetJitterTimeSeconds(void);

#endif // SILABS_TRUST_CENTER_KEEPALIVE_H
