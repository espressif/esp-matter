/***************************************************************************//**
 * @file
 * @brief ZigBee Device Object (ZDO) functions not provided by the stack.
 * See @ref util_zdo for documentation.
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

#ifndef SILABS_ZIGBEE_DEVICE_LIBRARY_H
#define SILABS_ZIGBEE_DEVICE_LIBRARY_H

/// @addtogroup util_zdo
/// For getting information about nodes of a ZigBee network via a
/// ZigBee Device Object (ZDO). See zigbee-device-library.h and
/// zigbee-device-common.h for source code.
///
/// The ZDO library provides functions that construct and send several common ZDO
/// requests. It also provides a function for extracting the two addresses from
/// a ZDO address response. The format of all the ZDO requests and responses that
/// the stack supports is described in stack/include/zigbee-device-stack.h.
/// Since the library doesn't handle all of these requests and responses,
/// the application must construct any other requests it wishes to send
/// and decode any other responses it wishes to receive.
///
/// The request sending functions do the following:
///  -# Construct a correctly formatted payload buffer.
///  -# Fill in the APS frame with the correct values.
///  -# Send the message by calling either ::emberSendBroadcast()
///    or ::emberSendUnicast().
///
/// The result of the send is reported to the application as normal
/// via ::emberMessageSentHandler().
///
///
/// The following code shows an example of an application's use of
/// ::emberSimpleDescriptorRequest().
/// The command interpreter would call this function and supply the arguments.
/// @code
/// void sendSimpleDescriptorRequest(EmberCommandState *state)
/// {
///   EmberNodeId target = emberUnsignedCommandArgument(state, 0);
///   uint8_t targetEndpoint = emberUnsignedCommandArgument(state, 1);
///   if (emberSimpleDescriptorRequest(target,
///                                    targetEndpoint,
///                                    EMBER_APS_OPTION_NONE) != EMBER_SUCCESS) {
///     emberSerialPrintf(SERIAL_PORT, "emberSimpleDescriptorRequest failed\r\n");
///   }
/// }
/// @endcode
///
/// The following code shows an example of an application's use of
/// ::emberDecodeAddressResponse().
/// @code
/// void emberIncomingMessageHandler(EmberIncomingMessageType type,
///                                  EmberApsFrame *apsFrame,
///                                  EmberMessageBuffer message)
/// {
///   if (apsFrame->profileId == EMBER_ZDO_PROFILE_ID) {
///     switch (apsFrame->clusterId) {
///     case NETWORK_ADDRESS_RESPONSE:
///     case IEEE_ADDRESS_RESPONSE:
///       {
///         EmberEUI64 eui64;
///         EmberNodeId nodeId = emberDecodeAddressResponse(message, eui64);
///         // Use nodeId and eui64 here.
///         break;
///       }
///     default:
///       // Handle other incoming ZDO responses here.
///     }
///   } else {
///     // Handle incoming application messages here.
///   }
/// }
/// @endcode
///
/// @{

/** @name Service Discovery Functions
 * @{
 */

/** Request the specified node to send a list of its endpoints that
 * match the specified application profile and, optionally, lists of input
 * and/or output clusters.
 *
 * @param target  The node whose matching endpoints are desired. The request can
 * be sent unicast or broadcast ONLY to the "RX-on-when-idle-address" (0xFFFD)
 * If sent as a broadcast, any node that has matching endpoints will send a
 * response.
 * @param profile  The application profile to match.
 * @param inClusters  The list of input clusters. To not match any input
 * clusters, use ::EMBER_NULL_MESSAGE_BUFFER.
 * @param outClusters  The list of output clusters. To not match any output
 * clusters, use ::EMBER_NULL_MESSAGE_BUFFER.
 * @param options  The options to use when sending the unicast request. See
 * emberSendUnicast() for a description. This parameter is ignored if the target
 * is a broadcast address.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
EmberStatus emberMatchDescriptorsRequest(EmberNodeId target,
                                         uint16_t profile,
                                         EmberMessageBuffer inClusters,
                                         EmberMessageBuffer outClusters,
                                         EmberApsOption options);
/** @} END name group */

/** @name Binding Manager Functions
 * @{
 */

/** An end device bind request to the coordinator. The simple
 * descriptor of the specified endpoint is used to construct the request. If the
 * coordinator receives a second end device bind request then a binding is
 * created for every matching cluster.
 *
 * @param endpoint  The endpoint on the local device whose simple descriptor
 * will be used to create the request.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
EmberStatus emberEndDeviceBindRequest(uint8_t endpoint,
                                      EmberApsOption options);
/** @} END name group */

/** @name Decode Address Response Messages
 * @{
 */

/** Extract the EUI64 and the node ID from an address response
 * message.
 *
 * @param response  The received ZDO message with cluster ID
 * ::NETWORK_ADDRESS_RESPONSE or ::IEEE_ADDRESS_RESPONSE.
 * @param eui64Return  The EUI64 from the response is copied here.
 *
 * @return Returns the node ID from the response if the response status was
 * ::EMBER_ZDP_SUCCESS. Otherwise, returns ::EMBER_NULL_NODE_ID.
 */
EmberNodeId emberDecodeAddressResponse(EmberMessageBuffer response,
                                       EmberEUI64 eui64Return);

/** @} END name group */

/** @} END addtogroup */

#endif // SILABS_ZIGBEE_DEVICE_LIBRARY_H
