/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Server plugin.
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

#ifndef SILABS_ZIGBEE_ZLL_COMMISSIONING_SERVER_H
#define SILABS_ZIGBEE_ZLL_COMMISSIONING_SERVER_H

/**
 * @defgroup zll-commissioning-server ZLL Commissioning Server
 * @ingroup component cluster
 * @brief API and Callbacks for the ZLL Commissioning Cluster Server Component
 *
 * Server side implementation of the ZigBee 3.0 touchlink commissioning mechanism.
 *
 */

/**
 * @addtogroup zll-commissioning-server
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief No touchlink for non-factory new device.
 *
 * This function will cause an NFN device to refuse network start/join/update
 * requests and thus to forbid commissioning by touchlinking. This can be useful
 * to restrict touchlink stealing.
 */
EmberStatus emberAfZllNoTouchlinkForNFN(void);
/** @brief No reset for non-factory new device.
 *
 * This function will cause an NFN device on a centralized security network to
 * a touchlink reset-to-factory-new request from a remote device.
 */
EmberStatus emberAfZllNoResetForNFN(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup zll_commissioning_server_cb ZLL commissioning Server
 * @ingroup af_callback
 * @brief Callbacks for ZLL commissioning Server  Component
 *
 */

/**
 * @addtogroup zll_commissioning_server_cb
 * @{
 */

// For legacy code
#define emberAfPluginZllCommissioningGroupIdentifierCountCallback \
  emberAfPluginZllCommissioningServerGroupIdentifierCountCallback
#define emberAfPluginZllCommissioningGroupIdentifierCallback \
  emberAfPluginZllCommissioningServerGroupIdentifierCallback
#define emberAfPluginZllCommissioningEndpointInformationCountCallback \
  emberAfPluginZllCommissioningServerEndpointInformationCountCallback
#define emberAfPluginZllCommissioningEndpointInformationCallback \
  emberAfPluginZllCommissioningServerEndpointInformationCallback
#define emberAfPluginZllCommissioningIdentifyCallback \
  emberAfPluginZllCommissioningServerIdentifyCallback

/** @brief Determine the
 * number of group identifiers in use by a specific endpoint on the device.
 *
 * This function is called by the ZLL Commissioning Server plugin.
 * The total number of group identifiers on the device, which are shared by all
 * endpoints, is defined by ::EMBER_ZLL_GROUP_ADDRESSES.
 *
 * @param endpoint The endpoint for which the group identifier count is
 * requested. Ver.: always
 */
uint8_t emberAfPluginZllCommissioningServerGroupIdentifierCountCallback(uint8_t endpoint);

/** @brief Obtain
 * information about the group identifiers in use by a specific endpoint on the
 * device
 *
 * This function is called by the ZLL Commissioning Server plugin.
 * The application should populate the record with information about
 * the group identifier and return true. If no information is available for the
 * given endpoint and index, the application should return false.
 *
 * @param endpoint The endpoint for which the group identifier is requested.
 * Ver.: always
 * @param index The index of the group on the endpoint. Ver.: always
 * @param record The group information record. Ver.: always
 */
bool emberAfPluginZllCommissioningServerGroupIdentifierCallback(uint8_t endpoint,
                                                                uint8_t index,
                                                                EmberAfPluginZllCommissioningGroupInformationRecord *record);

/** @brief Determine the
 * number of remote endpoints controlled by a specific endpoint on the local
 * device.
 *
 * This function is called by the ZLL Commissioning Server plugin.
 *
 * @param endpoint The local endpoint for which the remote endpoint information
 * count is requested. Ver.: always
 */
uint8_t emberAfPluginZllCommissioningServerEndpointInformationCountCallback(uint8_t endpoint);

/** @brief Obtain
 * information about the remote endpoints controlled by a specific endpoint on
 * the local device.
 *
 * This function is called by the ZLL Commissioning Server plugin.
 * The application should populate the record with
 * information about the remote endpoint and return true. If no information is
 * available for the given endpoint and index, the application should return
 * false.
 *
 * @param endpoint The local endpoint for which the remote endpoint information
 * is requested. Ver.: always
 * @param index The index of the remote endpoint information on the local
 * endpoint. Ver.: always
 * @param record The endpoint information record. Ver.: always
 */
bool emberAfPluginZllCommissioningServerEndpointInformationCallback(uint8_t endpoint,
                                                                    uint8_t index,
                                                                    EmberAfPluginZllCommissioningEndpointInformationRecord *record);

/** @brief Notify the
 * application that it should take an action to identify itself.
 *
 * This function is called by the ZLL Commissioning Server plugin. This typically
 * occurs when an Identify Request is received via inter-PAN messaging.
 *
 * @param durationS If the duration is zero, the device should exit identify
 * mode. If the duration is 0xFFFF, the device should remain in identify mode
 * for the default time. Otherwise, the duration specifies the length of time
 * in seconds that the device should remain in identify mode. Ver.: always
 */
void emberAfPluginZllCommissioningServerIdentifyCallback(uint16_t durationS);

/** @} */ // end of zll_commissioning_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of zll-commissioning-server

#endif // SILABS_ZIGBEE_ZLL_COMMISSIONING_SERVER_H
