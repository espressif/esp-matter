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

/** @addtogroup util_zdo
 * For getting information about nodes of a ZigBee network via a
 * ZigBee Device Object (ZDO). See zigbee-device-host.h and
 * zigbee-device-common.h for source code.
 *
 * The ZDO library provides functions that construct and send several common ZDO
 * requests. It also provides a function for extracting the two addresses from
 * a ZDO address response. The format of all the ZDO requests and responses that
 * the stack supports is described in stack/include/zigbee-device-stack.h.
 * Since the library doesn't handle all of these requests and responses,
 * the application must construct any other requests it wishes to send
 * and decode any other responses it wishes to receive.
 *
 * The request sending functions do the following:
 *  -# Construct a correctly formatted payload buffer.
 *  -# Fill in the APS frame with the correct values.
 *  -# Send the message by calling either ::ezspSendBroadcast()
 *    or ::ezspSendUnicast().
 *
 * The result of the send is reported to the application as normal
 * via ::ezspMessageSentHandler().
 *
 *
 * The following code shows an example of an application's use of
 * ::emberSimpleDescriptorRequest().
 * The command interpreter would call this function and supply the arguments.
 * @code
 * void sendSimpleDescriptorRequest(EmberCommandState *state)
 * {
 *   EmberNodeId target = emberUnsignedCommandArgument(state, 0);
 *   uint8_t targetEndpoint = emberUnsignedCommandArgument(state, 1);
 *   if (emberSimpleDescriptorRequest(target,
 *                                    targetEndpoint,
 *                                    EMBER_APS_OPTION_NONE) != EMBER_SUCCESS) {
 *     emberSerialPrintf(SERIAL_PORT, "emberSimpleDescriptorRequest failed\r\n");
 *   }
 * }
 * @endcode
 *
 * The following code shows an example of an application's use of
 * ::ezspDecodeAddressResponse().
 * @code
 * void ezspIncomingMessageHandler(EmberIncomingMessageType type,
 *                                 EmberApsFrame *apsFrame,
 *                                 uint8_t lastHopLqi,
 *                                 int8_t lastHopRssi,
 *                                 EmberNodeId sender,
 *                                 uint8_t bindingIndex,
 *                                 uint8_t addressIndex,
 *                                 uint8_t messageLength,
 *                                 uint8_t *messageContents)
 * {
 *   if (apsFrame->profileId == EMBER_ZDO_PROFILE_ID) {
 *     switch (apsFrame->clusterId) {
 *     case NETWORK_ADDRESS_RESPONSE:
 *     case IEEE_ADDRESS_RESPONSE:
 *       {
 *         EmberEUI64 eui64;
 *         EmberNodeId nodeId = ezspDecodeAddressResponse(messageContents,
 *                                                        eui64);
 *         // Use nodeId and eui64 here.
 *         break;
 *       }
 *     default:
 *       // Handle other incoming ZDO responses here.
 *     }
 *   } else {
 *     // Handle incoming application messages here.
 *   }
 * }
 * @endcode
 *
 * @{
 */

/** @name Device Discovery Functions
 * @{
 */

/**
 * @brief Request the 16 bit network address of a node whose EUI64 is known.
 *
 * @param target           The EUI64 of the node.
 * @param reportKids       true to request that the target list their children
 *                         in the response.
 * @param childStartIndex  The index of the first child to list in the response.
 *                         Ignored if @c reportKids is false.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS - The request was transmitted successfully.
 * - ::EMBER_NO_BUFFERS - Insufficient message buffers were available to construct
 * the request.
 * - ::EMBER_NETWORK_DOWN - The node is not part of a network.
 * - ::EMBER_NETWORK_BUSY - Transmission of the request failed.
 */
EmberStatus emberNetworkAddressRequest(EmberEUI64 target,
                                       bool reportKids,
                                       uint8_t childStartIndex);

/**
 * @brief Request the EUI64 of a node whose 16 bit network address is known.
 *
 * @param target           The network address of the node.
 * @param reportKids       true to request that the target list their children
 *                         in the response.
 * @param childStartIndex  The index of the first child to list in the response.
 *                         Ignored if reportKids is false.
 * @param options          The options to use when sending the request.
 *                         See ::emberSendUnicast() for a description.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_NO_BUFFERS
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_NETWORK_BUSY
 */
EmberStatus emberIeeeAddressRequest(EmberNodeId target,
                                    bool reportKids,
                                    uint8_t childStartIndex,
                                    EmberApsOption options);
/** @} END name group */

EmberStatus emberIeeeAddressRequestToTarget(EmberNodeId discoveryNodeId,
                                            bool reportKids,
                                            uint8_t childStartIndex,
                                            EmberApsOption options,
                                            EmberNodeId targetNodeIdOfRequest);

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
 * @param inCount  The number of input clusters. To not match any input
 * clusters, set this value to 0.
 * @param outCount  The number of output clusters. To not match any output
 * clusters, set this value to 0.
 * @param inClusters  The list of input clusters.
 * @param outClusters  The list of output clusters.
 * @param options  The options to use when sending the unicast request. See
 * emberSendUnicast() for a description. This parameter is ignored if the target
 * is a broadcast address.
 *
 * @return An EmberStatus value. EMBER_SUCCESS, EMBER_NO_BUFFERS,
 * EMBER_NETWORK_DOWN or EMBER_NETWORK_BUSY.
 */
EmberStatus ezspMatchDescriptorsRequest(EmberNodeId target,
                                        uint16_t profile,
                                        uint8_t inCount,
                                        uint8_t outCount,
                                        uint16_t *inClusters,
                                        uint16_t *outClusters,
                                        EmberApsOption options);
/** @} END name group */

/** @name Binding Manager Functions
 * @{
 */

/**
 * @brief An end device bind request to the coordinator. If the coordinator receives a
 * second end device bind request then a binding is created for every matching
 * cluster.
 *
 * @param localNodeId  The node ID of the local device.
 * @param localEui64  The EUI64 of the local device.
 * @param endpoint  The endpoint to be bound.
 * @param profile  The application profile of the endpoint.
 * @param inCount  The number of input clusters.
 * @param outCount  The number of output clusters.
 * @param inClusters  The list of input clusters.
 * @param outClusters  The list of output clusters.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. EMBER_SUCCESS, EMBER_NO_BUFFERS,
 * EMBER_NETWORK_DOWN or EMBER_NETWORK_BUSY.
 */
EmberStatus ezspEndDeviceBindRequest(EmberNodeId localNodeId,
                                     EmberEUI64 localEui64,
                                     uint8_t endpoint,
                                     uint16_t profile,
                                     uint8_t inCount,
                                     uint8_t outCount,
                                     uint16_t *inClusters,
                                     uint16_t *outClusters,
                                     EmberApsOption options);
/** @} END name group */

/** @name Function to Decode Address Response Messages
 * @{
 */

/**
 * @brief Extract the EUI64 and the node ID from an address response
 * message.
 *
 * @param response  The received ZDO message with cluster ID
 * NETWORK_ADDRESS_RESPONSE or IEEE_ADDRESS_RESPONSE.
 * @param eui64Return  The EUI64 from the response is copied here.
 *
 * @return Returns the node ID from the response if the response status was
 * EMBER_ZDP_SUCCESS. Otherwise, returns EMBER_NULL_NODE_ID.
 */
EmberNodeId ezspDecodeAddressResponse(uint8_t *response,
                                      EmberEUI64 eui64Return);

/** @} END name group */

/** @} END addtogroup */
