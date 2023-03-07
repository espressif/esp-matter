/***************************************************************************//**
 * @file
 * @brief EmberZNet API for sending and receiving messages.
 * See @ref message for documentation.
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

#ifndef SILABS_MESSAGE_H
#define SILABS_MESSAGE_H

/**
 * @addtogroup message
 *
 * See message.h for source code.
 * @{
 */

/** @brief Return the maximum size of the payload that the Application
 * Support sub-layer will accept.
 *
 * The size depends on the security level in use.
 * The value is the same as that found in the node descriptor.
 *
 * @return The maximum APS payload length.
 */
uint8_t emberMaximumApsPayloadLength(void);

/** @brief The per-hop delay allowed for in the calculation of the
 * APS ACK timeout value.  This is defined in the Zigbee specification.
 * This times the maximum number of hops (EMBER_MAX_HOPS)
 * plus the terminal encrypt/decrypt time is the timeout between retries of an
 * APS ACKed message in milliseconds.
 */
#define EMBER_APSC_MAX_ACK_WAIT_HOPS_MULTIPLIER_MS     50

/** @brief The terminal encrypt/decrypt time allowed for in the calculation
 * of the APS ACK timeout value.  This is defined in the ZigBee specification.
 */
#define EMBER_APSC_MAX_ACK_WAIT_TERMINAL_SECURITY_MS  100

/** @brief The APS ACK timeout value.
 * The stack waits this amount of time between resends of APS retried messages.
 * The default value is:
 * <pre>
 *   ((EMBER_APSC_MAX_ACK_WAIT_HOPS_MULTIPLIER_MS
 *     * EMBER_MAX_HOPS)
 *    + EMBER_APSC_MAX_ACK_WAIT_TERMINAL_SECURITY_MS)
 * </pre>
 */
extern uint16_t emberApsAckTimeoutMs;

/** @brief Send a multicast message for an alias source to all endpoints that
 * share a specific multicast ID and are within a specified number of hops of
 * the sender.
 *
 * @param apsFrame         The APS frame for the message.  The multicast will be sent
 *  to the groupId in this frame.
 *
 * @param radius           The message will be delivered to all nodes within this number
 *  of hops of the sender. A value of zero is converted to EMBER_MAX_HOPS.
 *
 * @param nonmemberRadius  The number of hops that the message will be forwarded
 *  by devices that are not members of the group.  A value of 7 or greater is
 *  treated as infinite.
 *
 * @param message          A message.
 *
 * @param alias The alias from which to send the multicast.
 *
 * @param sequence The NWK sequence number for the message.
 *
 * @return An ::EmberStatus value. For any result other than ::EMBER_SUCCESS,
 *  the message will not be sent.\n\n
 * - ::EMBER_SUCCESS - The message has been submitted for
 * transmission.
 * - ::EMBER_INVALID_BINDING_INDEX - The \c bindingTableIndex
 * refers to a non-multicast binding.
 * - ::EMBER_NETWORK_DOWN - The node is not part of a network.
 * - ::EMBER_MESSAGE_TOO_LONG - The message is too large to
 * fit in a MAC layer frame.
 * - ::EMBER_NO_BUFFERS  - The free packet buffer pool is empty.
 * - ::EMBER_NETWORK_BUSY - Insufficient resources available
 * in Network or MAC layers to send message.
 */
EmberStatus emberSendMulticastWithAlias(EmberApsFrame *apsFrame,
                                        uint8_t radius,
                                        uint8_t nonmemberRadius,
                                        EmberMessageBuffer message,
                                        EmberNodeId alias,
                                        uint8_t sequence);

/** @brief Send a multicast message to all endpoints that share
 * a specific multicast ID and are within a specified number of hops of the
 * sender.
 *
 * @param apsFrame         The APS frame for the message.  The multicast will be sent
 *  to the groupId in this frame.
 *
 * @param radius           The message will be delivered to all nodes within this number
 *  of hops of the sender. A value of zero is converted to EMBER_MAX_HOPS.
 *
 * @param nonmemberRadius  The number of hops that the message will be forwarded
 *  by devices that are not members of the group.  A value of 7 or greater is
 *  treated as infinite.
 *
 * @param message          A message.
 *
 * @return An ::EmberStatus value. For any result other than ::EMBER_SUCCESS,
 *  the message will not be sent.\n\n
 * - ::EMBER_SUCCESS - The message has been submitted for
 * transmission.
 * - ::EMBER_INVALID_BINDING_INDEX - The \c bindingTableIndex
 * refers to a non-multicast binding.
 * - ::EMBER_NETWORK_DOWN - The node is not part of a network.
 * - ::EMBER_MESSAGE_TOO_LONG - The message is too large to
 * fit in a MAC layer frame.
 * - ::EMBER_NO_BUFFERS  - The free packet buffer pool is empty.
 * - ::EMBER_NETWORK_BUSY - Insufficient resources available
 * in Network or MAC layers to send message.
 */
EmberStatus emberSendMulticast(EmberApsFrame *apsFrame,
                               uint8_t radius,
                               uint8_t nonmemberRadius,
                               EmberMessageBuffer message);

/** @brief Send a unicast message per the Zigbee specification.
 *
 * The message will arrive at its destination only if there is a known route
 * to the destination node.  Setting the ::ENABLE_ROUTE_DISCOVERY
 * option will cause a route to be discovered if none is known.  Setting the
 * ::FORCE_ROUTE_DISCOVERY option will force route discovery.
 * Routes to end-device children of the local node are always known.
 *
 * Setting the @c APS_RETRY option will cause the message to be
 * retransmitted until either a matching acknowledgment is received or three
 * transmissions have been made.
 *
 * @note Using the ::FORCE_ROUTE_DISCOVERY option will cause the first
 * transmission to be consumed by a route request as part of discovery, so
 * the application payload of this packet will not reach its destination on
 * the first attempt.  For the packet to reach its destination, the
 * APS_RETRY option must be set so that another attempt is made to transmit
 * the message with its application payload after the route has been
 * constructed.
 *
 * Setting the ::DESTINATION_EUI64 option will cause the long
 * ID of the destination to be included in the network header.  This is
 * the only way to absolutely guarantee that the message is delivered to
 * the correct node.  Without it, a message may on occasion be delivered
 * to the wrong destination in the event of an ID conflict that has
 * not yet been detected and resolved by the network layer.
 *
 * @note When sending fragmented messages, the stack will only assign a new APS
 * sequence number for the first fragment of the message (i.e.,
 * ::EMBER_APS_OPTION_FRAGMENT is set and the low-order byte of the groupId
 * field in the APS frame is zero).  For all subsequent fragments of the same
 * message, the application must set the sequence number field in the APS frame
 * to the sequence number assigned by the stack to the first fragment.
 *
 * @param type               Specifies the outgoing message type.  Must be one of
 * ::EMBER_OUTGOING_DIRECT, ::EMBER_OUTGOING_VIA_ADDRESS_TABLE, or
 * ::EMBER_OUTGOING_VIA_BINDING.
 *
 * @param indexOrDestination Depending on the type of addressing used, this
 *  is either the EmberNodeId of the destination, an index into the address table,
 *  or an index into the binding table.
 *
 * @param apsFrame           The APS frame which is to be added to the message.
 *
 * @param message            Contents of the message.
 *
 * @return An ::EmberStatus value. For any result other than
 *   ::EMBER_SUCCESS, the message will not be sent.
 * - ::EMBER_SUCCESS - The message has been submitted for transmission.
 * - ::EMBER_INVALID_BINDING_INDEX - The \c bindingTableIndex
 * refers to a non-unicast binding.
 * - ::EMBER_NETWORK_DOWN - The node is not part of a network.
 * - ::EMBER_MESSAGE_TOO_LONG - The message is too large to
 * fit in a MAC layer frame.
 * - ::EMBER_MAX_MESSAGE_LIMIT_REACHED - The
 * ::EMBER_APS_UNICAST_MESSAGE_COUNT limit has been reached.
 */
EmberStatus emberSendUnicast(EmberOutgoingMessageType type,
                             uint16_t indexOrDestination,
                             EmberApsFrame *apsFrame,
                             EmberMessageBuffer message);

/** @brief Send a broadcast message as per the ZigBee specification.
 *
 * The message will be delivered to all nodes within @c radius
 * hops of the sender. A radius of zero is converted to ::EMBER_MAX_HOPS.
 *
 * @param destination  The destination to which to send the broadcast.
 *  This must be one of three ZigBee broadcast addresses.
 *
 * @param apsFrame     The APS frame data to be included in the message.
 *
 * @param radius       The maximum number of hops the message will be relayed.
 *
 * @param message      The actual message to be sent.
 *
 * @return An ::EmberStatus value.
 */
EmberStatus emberSendBroadcast(EmberNodeId destination,
                               EmberApsFrame *apsFrame,
                               uint8_t radius,
                               EmberMessageBuffer message);

/** @brief Proxy a broadcast message for another node.
 *
 * The message will be delivered to all nodes within @c radius
 * hops of the local node. A radius of zero is converted to ::EMBER_MAX_HOPS.
 *
 * @param source       The source from which to send the broadcast.
 *
 * @param destination  The destination to which to send the broadcast.
 *  This must be one of three ZigBee broadcast addresses.
 *
 * @param sequence     The NWK sequence number for the message.
 *
 * @param apsFrame     The APS frame data to be included in the message.
 *
 * @param radius       The maximum number of hops the message will be relayed.
 *
 * @param message      The actual message to be sent.
 *
 * @return An ::EmberStatus value.
 */
EmberStatus emberProxyBroadcast(EmberNodeId source,
                                EmberNodeId destination,
                                uint8_t sequence,
                                EmberApsFrame *apsFrame,
                                uint8_t radius,
                                EmberMessageBuffer message);

/** @brief Proxy a broadcast message for another node.
 *
 * The function is used in conjunction with emberProxyBroadcast, which proxies
 * a message from a short ID. This function is used where a long source is also
 * specified in the NWK frame control.
 *
 * @param euiSource    The long source from which to send the broadcast.
 *
 * @return An ::EmberStatus value.
 */
EmberStatus emberProxyNextBroadcastFromLong(uint8_t* euiSource);

/** @brief Send a route request packet that creates routes from
 * every node in the network back to this node.
 *
 * This function should be
 * called by an application that wants to communicate with
 * many nodes, for example, a gateway, central monitor, or controller.
 * A device using this function was referred to as an "aggregator" in
 * EmberZNet 2.x and earlier, and is referred to as a "concentrator"
 * in the ZigBee specification and EmberZNet 3.
 *
 * This function enables large scale networks because the other devices
 * do not have to individually perform bandwidth-intensive route discoveries.
 * Instead, when a remote node sends an APS unicast to a concentrator, its
 * network layer automatically delivers a special route record packet first,
 * which lists the network IDs of all intermediate relays.
 * The concentrator can then use source routing to send outbound APS unicasts.
 * (A source routed message is one in which the entire route is listed in
 * the network layer header.) This allows the concentrator to communicate
 * with thousands of devices without requiring large route tables on
 * neighboring nodes.
 *
 * This function is only available in ZigBee Pro (stack profile 2),
 * and cannot be called on end devices. Any router can be a concentrator
 * (not just the coordinator), and there can be multiple concentrators on
 * a network.
 *
 * Note that a concentrator does not automatically obtain routes to all network
 * nodes after calling this function. Remote applications must first initiate
 * an inbound APS unicast.
 *
 * Many-to-one routes are not repaired automatically. Instead, the concentrator
 * application must call this function to rediscover the routes as necessary,
 * for example, upon failure of a retried APS message. This is necessary because
 * there is no scalable one-size-fits-all route repair strategy.
 * A common and recommended strategy is for the concentrator application to
 * refresh the routes by calling this function periodically.
 *
 * @param concentratorType   Must be either ::EMBER_HIGH_RAM_CONCENTRATOR or
 * ::EMBER_LOW_RAM_CONCENTRATOR. The former is used when the caller has
 * enough memory to store source routes for the whole network. In that
 * case, remote nodes stop sending route records once the concentrator has
 * successfully received one. The latter is used when the concentrator
 * has insufficient RAM to store all outbound source routes. In that
 * case, route records are sent to the concentrator prior to every inbound
 * APS unicast.
 *
 * @param radius             The maximum number of hops the route request will be
 * relayed.  A radius of zero is converted to ::EMBER_MAX_HOPS.
 *
 * @return ::EMBER_SUCCESS if the route request was successfully submitted to
 * the transmit queue, and ::EMBER_ERR_FATAL otherwise.
 */
EmberStatus emberSendManyToOneRouteRequest(uint16_t concentratorType,
                                           uint8_t radius);

/** @brief Return the source route table entry fields.
 * @param index        The index of the source route table entry.
 * @param destination  The value of the destination field in that entry.
 * @param closerIndex  The value of the closer index entry field in that entry.
 */

EmberStatus emberGetSourceRouteTableEntry(uint8_t index,
                                          EmberNodeId *destination,
                                          uint8_t *closerIndex);

/** @brief Return the number of filled entries in the source route table.
 */

uint8_t emberGetSourceRouteTableFilledSize(void);

/** @brief Return the source route table total size.
 */

uint8_t emberGetSourceRouteTableTotalSize(void);

/**
 * @brief Indicate that a many-to-one route to the concentrator
 * with the given short and long ID is available for use.
 *
 * The application must define
 * @c EMBER_APPLICATION_HAS_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER
 * in its configuration header to use this.
 *
 * @param source  The short ID of the concentrator that initiated the
 * many-to-one route request.
 * @param longId  The EUI64 of the concentrator.
 * @param cost    The path cost to the concentrator.
 */
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               uint8_t cost);

/**
 * @brief Invoke when a route error message is received.
 *
 * A status of ::EMBER_SOURCE_ROUTE_FAILURE indicates that a
 * source-routed unicast sent from this node encountered a
 * broken link.  Note that this case occurs only if this node is
 * a concentrator using many-to-one routing for inbound
 * messages and source-routing for outbound messages.
 * The node prior to the broken link generated the route
 * error message and returned it to us along the many-to-one route.
 *
 * A status of ::EMBER_MANY_TO_ONE_ROUTE_FAILURE also occurs only
 * if the local device is a concentrator, and indicates that
 * a unicast sent to the local device along a many-to-one route
 * encountered a broken link.  The node prior to the broken
 * link generated the route error message and forwarded it
 * to the local device via a randomly chosen neighbor, taking advantage
 * of the many-to-one nature of the route.
 *
 * A status of ::EMBER_MAC_INDIRECT_TIMEOUT indicates that a
 * message sent to the target end device could not be delivered
 * by the parent because the indirect transaction timer expired.
 * Upon receipt of the route error, the stack sets the extended
 * timeout for the target node in the address table, if present.
 * It then calls this handler to indicate receipt of the error.
 *
 * Note that if the original unicast data message is sent using
 * the ::EMBER_APS_OPTION_RETRY option, a new route error message
 * is generated for each failed retry. Therefore, it is not unusual
 * to receive three route error messages in succession for a
 * single failed retried APS unicast. On the other hand, it is also
 * not guaranteed that any route error messages will be
 * delivered successfully at all. The only sure way to detect
 * a route failure is to use retried APS messages and to check the
 * status of the ::emberMessageSentHandler().
 *
 * If the application includes this callback,
 * it must define @c EMBER_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
 * in its configuration header.
 *
 * @param status  ::EMBER_SOURCE_ROUTE_FAILURE,
 * ::EMBER_MANY_TO_ONE_ROUTE_FAILURE,
 * ::EMBER_MAC_INDIRECT_TIMEOUT
 * @param target  The short ID of the remote node.
 */
void emberIncomingRouteErrorHandler(EmberStatus status,
                                    EmberNodeId target);

/**
 * @brief Invoke when a network status message
 * is received that informs the application of the Over-the-Air
 * error codes for the specific destination.
 *
 * Note: Network analyzer may flag this message as "route error" which
 * is the old name for the "network status" command.
 *
 * This handler is a superset of emberIncomingRouteErrorHandler.
 * The old API was only invoking the handler for a couple of the possible
 * error codes and these were being translated into EmberStatus.
 *
 * To make the API more generic and extensible in future, the new
 * API emberIncomingNetworkStatusHandler simply copies the OTA
 * error code as is. If the application includes this callback,
 * it must define @c EMBER_APPLICATION_HAS_INCOMING_NETWORK_STATUS_HANDLER
 * in its configuration header.
 * @param errorCode  uint8_t
 * @param target  The short ID of the remote node.
 */
void emberIncomingNetworkStatusHandler(uint8_t errorCode,
                                       EmberNodeId target);

/** @brief SoC only. It reports the arrival of a route record command frame
 * to the application.
 *
 * The route record command frame lists the short
 * IDs of the relays that were used along the route from the source to us.
 * This information is used by aggregators to be able to initiate
 * source routed messages.  The application must
 * define @c EMBER_APPLICATION_HAS_OVERRIDE_SOURCE_ROUTING in its
 * configuration header to use this.
 *
 * @param source          The id of the node that initiated the route record.
 * @param sourceEui       The EUI64 of the node that initiated the route record.
 * @param relayCount      The number of relays in the list.
 * @param header          The message buffer containing the route record frame.
 * @param relayListIndex  The starting index of the relay list.  The
 * relay closest to the source is listed first, and the relay closest
 * to us is listed last.  Short ids are stored low byte first.  Be
 * careful to use buffer-boundary-safe APIs to read the list.
 * @param consumed if set to true stack won't process the Incoming route record
 */
void emberOverrideIncomingRouteRecordHandler(EmberNodeId source,
                                             EmberEUI64 sourceEui,
                                             uint8_t relayCount,
                                             EmberMessageBuffer header,
                                             uint8_t relayListIndex,
                                             bool* consumed);

/** @brief SoC only. The application can implement this callback to
 * supply source routes to outgoing messages.
 *
 * The application must define :EMBER_APPLICATION_HAS_OVERRIDE_SOURCE_ROUTING in its
 * configuration header to use this.  The application uses the
 * supplied destination to look up a source route.  If available,
 * the application appends the source route to the supplied header
 * using the proper frame format, as described in section 3.4.1.9
 * "Source Route Subframe Field" of the ZigBee specification.
 * If a source route is appended, the stack takes care of
 * setting the proper flag in the network frame control field.
 * See app/util/source-route.c for a sample implementation.
 *
 * If header is :EMBER_NULL_MESSAGE_BUFFER the only action is to return
 * the size of the source route frame needed to the destination.
 *
 * @param destination   The network destination of the message.
 * @param *header        Pointer to the message buffer containing the partially
 * complete packet header.  The application appends the source
 * route frame to this header.
 * @param consumed if set to true stack would skip appending the existing source route entry from the table
 *
 * @return The size in bytes of the source route frame, or zero
 * if there is not one available.
 */
uint8_t emberOverrideAppendSourceRouteHandler(EmberNodeId destination,
                                              EmberMessageBuffer* header,
                                              bool* consumed);

/** @brief DEPRECTAED emberOverrideIncomingRouteRecordHandler should be used
 *  Reports the arrival of a route record command frame
 * to the application.
 *
 * The route record command frame lists the short
 * IDs of the relays that were used along the route from the source to us.
 * This information is used by aggregators to be able to initiate
 * source routed messages.  The application must
 * define @c EMBER_APPLICATION_HAS_SOURCE_ROUTING in its
 * configuration header to use this.
 *
 * @param source          The id of the node that initiated the route record.
 * @param sourceEui       The EUI64 of the node that initiated the route record.
 * @param relayCount      The number of relays in the list.
 * @param header          The message buffer containing the route record frame.
 * @param relayListIndex  The starting index of the relay list.  The
 * relay closest to the source is listed first, and the relay closest
 * to us is listed last.  Short ids are stored low byte first.  Be
 * careful to use buffer-boundary-safe APIs to read the list.
 */
void emberIncomingRouteRecordHandler(EmberNodeId source,
                                     EmberEUI64 sourceEui,
                                     uint8_t relayCount,
                                     EmberMessageBuffer header,
                                     uint8_t relayListIndex);

/** @brief DEPRECATED emberOverrideAppendSourceRouteHandler should be used
 * The application can implement this callback to
 * supply source routes to outgoing messages.
 *
 * The application must define :EMBER_APPLICATION_HAS_SOURCE_ROUTING in its
 * configuration header to use this.  The application uses the
 * supplied destination to look up a source route.  If available,
 * the application appends the source route to the supplied header
 * using the proper frame format, as described in section 3.4.1.9
 * "Source Route Subframe Field" of the ZigBee specification.
 * If a source route is appended, the stack takes care of
 * setting the proper flag in the network frame control field.
 * See app/util/source-route.c for a sample implementation.
 *
 * If header is :EMBER_NULL_MESSAGE_BUFFER the only action is to return
 * the size of the source route frame needed to the destination.
 *
 * @param destination   The network destination of the message.
 * @param header        The message buffer containing the partially
 * complete packet header.  The application appends the source
 * route frame to this header.
 *
 * @return The size in bytes of the source route frame, or zero
 * if there is not one available.
 */
uint8_t emberAppendSourceRouteHandler(EmberNodeId destination,
                                      EmberMessageBuffer header);
/** @brief DEPRECATED.
 *
 * @param message  A message.
 *
 * @return Always returns ::EMBER_SUCCESS.
 */
EmberStatus emberCancelMessage(EmberMessageBuffer message);

/** @brief Invoke by the stack when it has completed
 * sending a message.
 *
 * @param type                The type of message sent.
 *
 * @param indexOrDestination  The destination to which the message was sent.
 *  For direct unicasts, the destination to which the message was sent;
 *  for other unicasts, the address table or binding index to which the
 *  message was sent.
 *  The value is unspecified for multicasts and broadcasts.
 *
 * @param apsFrame             The APS frame for the message.
 *
 * @param message              The message that was sent.
 *
 * @param status               An ::EmberStatus value of ::EMBER_SUCCESS if
 * an ACK was received from the destination or
 * ::EMBER_DELIVERY_FAILED if no ACK was received.
 */
void emberMessageSentHandler(EmberOutgoingMessageType type,
                             uint16_t indexOrDestination,
                             EmberApsFrame *apsFrame,
                             EmberMessageBuffer message,
                             EmberStatus status);

/** @brief Invoke by the EmberZNet stack when a message is
 * received.
 *
 * The following functions may be called from ::emberIncomingMessageHandler():
 * - ::emberGetLastHopLqi()
 * - ::emberGetLastHopRssi()
 * - ::emberGetLastHopTimestamp()
 * - ::emberGetSender()
 * - ::emberGetSenderEui64()
 * - ::emberGetBindingIndex()
 * - ::emberSendReply() (for incoming APS retried unicasts only)
 * - ::emberSetReplyBinding()
 * - ::emberNoteSendersBinding()
 *
 * @param type      The type of the incoming message. One of the following:
 *  - ::EMBER_INCOMING_UNICAST
 *  - ::EMBER_INCOMING_UNICAST_REPLY
 *  - ::EMBER_INCOMING_MULTICAST
 *  - ::EMBER_INCOMING_MULTICAST_LOOPBACK
 *  - ::EMBER_INCOMING_BROADCAST
 *  - ::EMBER_INCOMING_BROADCAST_LOOPBACK
 *
 * @param apsFrame  The APS frame from the incoming message.
 *
 * @param message   The message that was sent.
 */
void emberIncomingMessageHandler(EmberIncomingMessageType type,
                                 EmberApsFrame *apsFrame,
                                 EmberMessageBuffer message);

/** @brief Get the link quality from the node that last relayed the
 * current message.
 *
 * @note This function may only be called from within
 * - ::emberIncomingMessageHandler()
 * - ::emberNetworkFoundHandler()
 * - ::emIncomingRouteRecord()
 * - ::emberMacPassthroughMessageHandler()
 * - ::emberIncomingBootloadMessageHandler()
 * .
 * When this function is called from within one of these handler functions, the
 * link quality reported corresponds to the header being processed in that
 * hander function.  If this function is called outside of these handler
 * functions, the link quality reported will correspond to a message that was
 * processed earlier.
 *
 * This function is not available from within emberPollHandler() or
 * emberPollCompleteHandler(). The link quality information of interest during
 * the emberPollHandler() is from the data request packet itself. This message
 * must be handled quickly due to strict 15.4 timing requirements, and the link
 * quality information is not recorded by the stack. The link quality
 * information of interest during the emberPollCompleteHandler() is from the
 * ACK to the data request packet. The ACK is handled by the hardware and the
 * link quality information does not make it up to the stack.
 *
 * @param lastHopLqi  The link quality for the last incoming message processed.
 *
 * @return This function always returns ::EMBER_SUCCESS.  It is not necessary
 * to check this return value.
 */
EmberStatus emberGetLastHopLqi(uint8_t *lastHopLqi);

/** @brief Get the receive signal strength indication (RSSI) for the
 * current message.
 *
 * After a successful call to this function, the quantity
 * referenced by lastHopRssi will contain the energy level (in units of dBm)
 * observed during the last packet received.
 * @note This function may only be called from within:
 * - ::emberIncomingMessageHandler()
 * - ::emberNetworkFoundHandler()
 * - ::emIncomingRouteRecord()
 * - ::emberMacPassthroughMessageHandler()
 * - ::emberIncomingBootloadMessageHandler()
 * .
 * When this function is called from within one of these handler functions, the
 * RSSI reported corresponds to the header being processed in that
 * handler function.  If this function is called outside of these handler
 * functions, the RSSI reported will correspond to a message that was
 * processed earlier.
 *
 * This function is not available from within emberPollHandler() or
 * emberPollCompleteHandler(). The RSSI information of interest during
 * the emberPollHandler() is from the data request packet itself. This message
 * must be handled quickly due to strict 15.4 timing requirements, and the
 * RSSI information is not recorded by the stack. The RSSI
 * information of interest during the emberPollCompleteHandler() is from the
 * ACK to the data request packet. The ACK is handled by the hardware and the
 * RSSI information does not make it up to the stack.
 *
 * @param lastHopRssi  The RSSI for the last incoming message processed.
 *
 * @return This function always returns ::EMBER_SUCCESS.  It is not necessary
 * to check this return value.
 */
EmberStatus emberGetLastHopRssi(int8_t *lastHopRssi);

/** @brief Get the receive timestamp (value of the MAC timer; ticks since up)
 * of the moment when the Start Frame Delimiter (SFD) was received for the
 * current message.
 *
 * Note: This API is mainly intended for use on EFR devices.
 *
 * RAIL timers tick every microsecond.  ::RAIL_GetTime can provide the value of
 * the current RAIL timebase (this wraps after ~4095 seconds.)
 *
 * The timestamp provided by this API is a 24-bit value passed up from the PHY
 * to the MAC layer, with a range of ~16.8 seconds.  So, to calculate relative
 * time using this timestamp, make sure to mask off the lower 24 bits provided
 * by ::Rail_GetTime.
 *
 * (EFR32xG1 ticks every 2us while other EFR32 platforms tick at 0.5us rate.
 * Make sure to adjust the relative timestamp value accordingly.)
 *
 * After a successful call to this function, the quantity
 * referenced by lastHopTimestamp will contain the value of the MAC timer
 * observed during the last packet received.
 * @note This function may only be called from within:
 * - ::emberIncomingMessageHandler()
 * - ::emberNetworkFoundHandler()
 * - ::emIncomingRouteRecord()
 * - ::emberMacPassthroughMessageHandler()
 * - ::emberIncomingBootloadMessageHandler()
 *
 * When this function is called from within one of these handler functions, the
 * timestamp reported corresponds to the header being processed in that
 * handler function.  If this function is called outside of these handler
 * functions, the timestamp reported will correspond to a message that was
 * processed earlier.
 *
 * This function is not available from within emberPollHandler() or
 * emberPollCompleteHandler(). The timestamp of interest during
 * the emberPollHandler() is from the data request packet itself. This message
 * must be handled quickly due to strict 15.4 timing requirements, and the
 * timestamp is not recorded by the stack. The timestamp
 * of interest during the emberPollCompleteHandler() is from the
 * ACK to the data request packet. The ACK is handled by the hardware and the
 * timestamp does not make it up to the stack.
 *
 * @param lastHopTimestamp  The timestamp for the last incoming message processed.
 *
 * @return This function always returns ::EMBER_SUCCESS.  It is not necessary
 * to check this return value.
 */
EmberStatus emberGetLastHopTimestamp(uint32_t *lastHopTimestamp);

/** @brief Return the node ID of the sender of the current incoming
 * message.
 * @note This function can be called only from within
 * ::emberIncomingMessageHandler().
 *
 * @return The sender of the current incoming message.
 */
EmberNodeId emberGetSender(void);

/** @brief Return the EUI64 of the sender of the current incoming
 * message if the sender chose to include this information in the message. The
 * ::EMBER_APS_OPTION_SOURCE_EUI64 bit in the options field of the APS frame of
 * the incoming message indicates that the EUI64 is present in the message.
 * @note This function can be called only from within
 * ::emberIncomingMessageHandler().
 *
 * @param senderEui64  The EUI64 of the sender.
 *
 * @return An EmberStatus value:
 * - ::EMBER_SUCCESS - senderEui64 has been set to the EUI64 of the
 * sender of the current incoming message.
 * - ::EMBER_INVALID_CALL -  Either:
 *  -# This function was called outside of the context of
 *     the ::emberIncomingMessageHandler() callback
 *  -# It was called in the context of ::emberIncomingMessageHandler()
 *     but the incoming message did not include the EUI64 of the sender.
 */
EmberStatus emberGetSenderEui64(EmberEUI64 senderEui64);

/** @brief Send a reply for an application that has received a unicast message.
 *
 * The reply will be included with the ACK that the stack
 * automatically sends back.
 * @note  This function may be called only from within
 * ::emberIncomingMessageHandler().
 *
 * @param clusterId  The cluster ID to use for the reply.
 *
 * @param reply      A reply message.
 *
 * @return An ::EmberStatus value. For any result other than ::EMBER_SUCCESS,
 * the message will not be sent.
 * - ::EMBER_SUCCESS - The message has been submitted for transmission.
 * - ::EMBER_INVALID_CALL -  Either:
 *   -# This function was called outside of the context of the
 *      ::emberIncomingMessageHandler() callback
 *   -# It was called in the context of ::emberIncomingMessageHandler()
 *      but the incoming message was not a unicast
 *   -# It was called more than once in the context of
 *      ::emberIncomingMessageHandler().
 * - ::EMBER_NETWORK_BUSY - Either:
 *   -# No route available.
 *   -# Insufficient resources available in Network or MAC layers to send
 *      message.
 */
EmberStatus emberSendReply(uint16_t clusterId, EmberMessageBuffer reply);

/**@brief Set the fragment data to be used when sending a reply to a unicast
 * message.
 * @note  This function may be called only from within
 * ::emberIncomingMessageHandler().
 *
 * @param fragmentData The low byte is the block number of the reply. The high
 * byte is the ACK bitfield of the reply.
 */
void emberSetReplyFragmentData(uint16_t fragmentData);

/** @brief Indicate whether any messages are currently being
 * sent using this address table entry.
 *
 * Note that this function does
 * not indicate whether the address table entry is unused. To
 * determine whether an address table entry is unused, check the
 * remote node ID.  The remote node ID will have the value
 * ::EMBER_TABLE_ENTRY_UNUSED_NODE_ID when the address table entry is
 * not in use.
 *
 * @param addressTableIndex  The index of an address table entry.
 *
 * @return True if the address table entry is active, false otherwise.
 */
bool emberAddressTableEntryIsActive(uint8_t addressTableIndex);

/** @brief Set the EUI64 of an address table entry.
 *
 *  This function will also check other address table entries, the child
 *  table and the neighbor table to see if the node ID for the given
 *  EUI64 is already known. If known, this function will also set
 *  the node ID. If not known, it will set the node ID to ::EMBER_UNKNOWN_NODE_ID.
 *
 * @param addressTableIndex  The index of an address table entry.
 *
 * @param eui64              The EUI64 to use for the address table entry.
 *
 * @return ::EMBER_SUCCESS if the EUI64 was successfully set,
 * and ::EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE otherwise.
 */
EmberStatus emberSetAddressTableRemoteEui64(uint8_t addressTableIndex,
                                            EmberEUI64 eui64);

/** @brief Set the short ID of an address table entry.
 *
 * Usually the application will not need to set the short ID in the address
 * table. Once the remote EUI64 is set, the stack is capable of figuring
 * out the short ID on its own.  However, in cases where the
 * application does set the short ID, the application must set the
 * remote EUI64 prior to setting the short ID.
 *
 * @param addressTableIndex  The index of an address table entry.
 *
 * @param id                 The short ID corresponding to the remote node whose
 *           EUI64 is stored in the address table at the given index or
 *           ::EMBER_TABLE_ENTRY_UNUSED_NODE_ID which indicates that the entry
 *           stored in the address table at the given index is not in use.
 */
void emberSetAddressTableRemoteNodeId(uint8_t addressTableIndex,
                                      EmberNodeId id);

/** @brief Get the EUI64 of an address table entry.
 *
 * @param addressTableIndex  The index of an address table entry.
 *
 * @param eui64              The EUI64 of the address table entry is copied
 *                           to this location.
 */
void emberGetAddressTableRemoteEui64(uint8_t addressTableIndex,
                                     EmberEUI64 eui64);

/** @brief Get the short ID of an address table entry.
 *
 * @param addressTableIndex  The index of an address table entry.
 *
 * @return One of the following:
 * - The short ID corresponding to the remote node whose EUI64 is
 *   stored in the address table at the given index.
 * - ::EMBER_UNKNOWN_NODE_ID - Indicates that the EUI64 stored in the
 *   address table at the given index is valid but the short ID is
 *   currently unknown.
 * - ::EMBER_DISCOVERY_ACTIVE_NODE_ID - Indicates that the EUI64 stored
 *   in the address table at the given location is valid and network
 *   address discovery is underway.
 * - ::EMBER_TABLE_ENTRY_UNUSED_NODE_ID - Indicates that the entry
 *   stored in the address table at the given index is not in use.
 */
EmberNodeId emberGetAddressTableRemoteNodeId(uint8_t addressTableIndex);

/** @brief Tell the stack whether or not the normal interval between
 * retransmissions of a retried unicast message should be increased by
 * ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
 *
 * The interval needs to be increased when
 * sending to a sleepy node so that the message is not retransmitted until the
 * destination has had time to wake up and poll its parent. The stack will
 * automatically extend the timeout:
 * - For our own sleepy children.
 * - When an address response is received from a parent on behalf of its child.
 * - When an indirect transaction expiry route error is received.
 * - When an end device announcement is received from a sleepy node.
 *
 * @param remoteEui64      The address of the node for which the timeout
 *                         is to be set.
 *
 * @param extendedTimeout  True if the retry interval should be increased by
 *                         ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
 *                         False if the normal retry interval should be used.
 */
void emberSetExtendedTimeout(EmberEUI64 remoteEui64, bool extendedTimeout);

/** @brief Indicate whether or not the stack will extend the normal
 * interval between retransmissions of a retried unicast message by
 * ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
 *
 * @param remoteEui64  The address of the node for which the timeout is to be
 * returned.
 *
 * @return true if the retry interval will be increased by
 * ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT and false if the normal retry interval
 * will be used.
 */
bool emberGetExtendedTimeout(EmberEUI64 remoteEui64);

/** @brief Invoke by the EmberZNet stack when an ID conflict is
 * discovered, that is, two different nodes in the network were found
 * to be using the same short ID.
 *
 * The stack automatically removes the conflicting short ID
 * from its internal tables (address, binding, route, neighbor, and
 * child tables).  The application should discontinue any other use
 * of the ID.  If the application includes this callback, it must
 * define ::EMBER_APPLICATION_HAS_ID_CONFLICT_HANDLER in its configuration
 * header.
 *
 * @param conflictingId  The short ID for which a conflict was detected.
 */
void emberIdConflictHandler(EmberNodeId conflictingId);

/** @brief Indicate whether there are pending messages in the APS retry queue.
 *
 * @return True if there is at least a pending message belonging to the current
 * network in the APS retry queue, false otherwise.
 */
bool emberPendingAckedMessages(void);

/** @brief The multicast table.
 *
 * Each entry contains a multicast ID
 * and an endpoint, indicating that the endpoint is a member of the multicast
 * group.  Only devices with an endpoint in a multicast group will receive
 * messages sent to that multicast group.
 *
 * Entries with with an endpoint of 0 are ignored (the ZDO does not a member
 * of any multicast groups).  All endpoints are initialized to 0 on startup.
 */
extern EmberMulticastTableEntry emberMulticastTable[];

/** @brief The number of entries in the multicast table.
 */
extern uint8_t emberMulticastTableSize;

/** @brief Allows the higher layers to control the broadcast behaviour of
 * a routing device. The originating device will rebroadcast the maximum number of times and
 * The configurations below only restrict the number of broadcasts from neighbouring routers.
 * The configuration settings must be done on each node.
 *
 * @param config         of the type sl_passive_ack_config_enum_t
 *                        SL_PASSIVE_ACK_DEFAULT_CONFIG - All non originating configured nodes check for passive acks from
 *                                                all neighbours. If no passive acks received it will rebroadcast max times.
 *                        SL_PASSIVE_ACK_DISABLE - Disable passive ack. All configured nodes rebroadcast the incoming processed
 *                                                broadcast the maximum configured number of times.
 *                        SL_PASSIVE_ACK_THRESHOLD_WITH_REBROADCAST - All non originating configured nodes check for passive acks from
 *                                                minAcksNeeded neighbours. They will rebroadcast received message at least once even
 *                                                if all passive acks have been received. If no passive acks received it will rebroadcast max times.
 *
 *                        SL_PASSIVE_ACK_THRESHOLD_NO_REBROADCAST - All non originating configured nodes check for passive acks from
 *                                                minAcksNeeded neighbours. They will not rebroadcast received message
 *                                                if all passive acks have been received. If no passive acks received it will rebroadcast max times.
 *                        SL_PASSIVE_ACK_THRESHOLD_WITH_REBROADCAST_ALL_NODES - This configuration is for both originating and relaying (non originating nodes).
 *                                                All configured nodes check for passive acks from
 *                                                minAcksNeeded neighbours. They will not rebroadcast received message if all passive acks have been received.
 *                                                If no passive acks received it will rebroadcast max times if minAcksNeeded is not set to 0
 *
 *
 *
 *  @param minAcksNeeded  The minimum number of acknowledgments (re-broadcasts)
 * to wait for until deeming the broadcast transmission complete.
 *
 * @return None.
 */
typedef enum {
  SL_PASSIVE_ACK_DEFAULT_CONFIG,
  SL_PASSIVE_ACK_DISABLE,
  SL_PASSIVE_ACK_THRESHOLD_WITH_REBROADCAST,
  SL_PASSIVE_ACK_THRESHOLD_NO_REBROADCAST,
  SL_PASSIVE_ACK_THRESHOLD_WITH_REBROADCAST_ALL_NODES
} sl_passive_ack_config_enum_t;

void sl_set_passive_ack_config(sl_passive_ack_config_enum_t config, uint8_t minAcksNeeded);

/** @brief Set the number of broadcast passive acknowledgments required before
 * terminating a broadcast transmission. A value of 0xFF causes the node to wait
 * for all neighbors to re-broadcast the packet before terminating the
 * transmission. The default value is 0xFF.
 * API is kept for backwards compatability.
 *
 * @param minAcksNeeded  The minimum number of acknowledgments (re-broadcasts)
 * to wait for until deeming the broadcast transmission complete.
 *
 * @return None.
 */
#define emberBroadcastSetMinAcksNeeded(minAcksNeeded) \
  sl_set_passive_ack_config(SL_PASSIVE_ACK_THRESHOLD_WITH_REBROADCAST, minAcksNeeded)

/** @brief Intercept an incoming packet from the stack and hands off to a plugin
 * for further processing
 *
 * @param packetType  The type of packet and the target stack layer.  See
 * ::EmberZigbeePacketType.
 * @param packetBuffer  The ::EmberMessageBuffer containing the packet.
 * @parma index  The starting index of the relevant packet data.  The plugin\
 * will receive a flat copy of the packet starting from this index.
 * @param data  Auxiliary data pointer
 *
 * @return  See ::EmberPacketAction
 */
EmberPacketAction emberPacketHandoffIncomingHandler(EmberZigbeePacketType packetType,
                                                    EmberMessageBuffer packetBuffer,
                                                    uint8_t index,
                                                    void *data);

/** @brief Intercept an outgoing packet from the stack and hands off to a plugin
 * for further processing
 *
 * @param packetType  The type of packet and the source stack layer.  See
 * ::EmberZigbeePacketType.
 * @param packetBuffer  The ::EmberMessageBuffer containing the packet.
 * @param index  The starting index of the relevant packet data.  The plugin\
 * will receive a flat copy of the packet starting from this index.
 * @param data  Auxiliary data pointer
 *
 * @return  See ::EmberPacketAction
 */
EmberPacketAction emberPacketHandoffOutgoingHandler(EmberZigbeePacketType packetType,
                                                    EmberMessageBuffer packetBuffer,
                                                    uint8_t index,
                                                    void *data);

/** @brief Call when the stack receives a packet that is meant
 * for one of the protocol layers specified in ::EmberZigbeePacketType.
 *
 *
 * The packetType argument is one of the values of the
 * ::EmberZigbeePacketType enum. If the stack receives an 802.15.4
 * MAC beacon, it will call this function with the packetType argument
 * set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON.
 *
 * The implementation of this callback may alter the data contained in
 * packetData, modify options and flags in the axillary data, or
 * consume the packet itself, either sending the message, or discarding
 * it as it sees fit.
 *
 * @param packetType The type of packet received. See
 * ::EmberZigbeePacketType.
 * @param packetData A flat buffer containing the packet contents
 * the buffer starts with the command id followed by the payload
 * @param size_p a pointer to the size of the packet data
 * @param data This is a pointer to auxiliary data for the command.
 *
 * @return a ::EmberPacketAction indicating what action should be taken for
 * the packet, EMBER_ACCEPT_PACKET, EMBER_DROP_PACKET, or EMBER_MANGLE_PACKET
 */
EmberPacketAction emberAfIncomingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      uint8_t* packetData,
                                                      uint8_t* size_p,
                                                      void *data);

/** @brief The stack is preparing to send a protocol layer packet.
 *
 * This is called when the stack is preparing to send a packet from one
 * of the protocol layers specified in ::EmberZigbeePacketType.
 *
 * The packetType argument is one of the values of the
 * ::EmberZigbeePacketType enum. If the stack is preparing to send an
 * 802.15.4 MAC beacon, it will call this function with the packetType
 * argument set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON.
 *
 * The implementation of this callback may alter the data contained in
 * packetData, modify options and flags in the axillary data, or
 * consume the packet itself, either sending the message, or discarding
 * it as it sees fit.
 *
 * @param packetType The type of packet received. See
 * ::EmberZigbeePacketType.
 * @param packetData A flat buffer containing the packet contents
 * the buffer starts with the command id followed by the payload
 * @param size_p a pointer to the size of the packet data
 * @param data This is a pointer to auxiliary data for the command. ZDO
 * commands pass the ::EmberApsFrame associated with the packet here. Otherwise,
 * this value is NULL.
 *
 * @return a ::EmberPacketAction indicating what action should be taken for
 * the packet, EMBER_ACCEPT_PACKET, EMBER_DROP_PACKET, or EMBER_MANGLE_PACKET
 */
EmberPacketAction emberAfOutgoingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      uint8_t* packetData,
                                                      uint8_t* size_p,
                                                      void* data);

/** @brief Add a network packet into the incoming network queue.
 *
 * Quality: Experimental API for prototyping.
 *
 * The content of the networkPacket follows the formats below:
 * ----Encrypted NWK Packet----
 * NWK Header   (8 bytes)
 * NWK Security (14 bytes)
 * APS Header   (8 bytes)
 * APS Payload  (Variable)
 * NWK MIC      (4 bytes)
 *
 * ----Unencrypted NWK Packet----
 * NWK Header   (8 bytes)
 * APS Header   (8 bytes)
 * APS Payload  (Variable)
 *
 * Noted: This function does not free up the memory pointed by
 * the networkPacket pointer. Users may need to free up the memory
 * after calling this function. The network packet should not be larger than
 * 115 bytes (TRANSMIT_BUFFER_SIZE 126 bytes - PACKET_HEADER_OVERHEAD 11 bytes).
 *
 * @param rssi                 The RSSI of the incoming packet.
 * @param lqi                  The LQI of the incoming packet.
 * @param nwk_packet           The network packet, for more information see
 * ::sl_nwk_packet_exchange_t.
 *
 * @return ::EMBER_SUCCESS if the network packet was successfully added into
 * the network queue, otherwise ::EMBER_NO_BUFFERS or ::EMBER_MESSAGE_TOO_LONG.
 */
EmberStatus slx_zigbee_add_to_incoming_network_queue(int8_t rssi,
                                                     uint8_t lqi,
                                                     const sl_nwk_packet_exchange_t *nwk_packet);

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file message.h has been added to this documentation for @ref message API
 * and includes these changes:
 * <ul>
 * <li> <b>New items</b>
 * <ul>
 * <li> emberSendManyToOneRouteRequest()
 * <li> emberGetSenderEui64()
 * <li> emberAddressTableEntryIsActive()
 * <li> emberSetAddressTableRemoteEui64()
 * <li> emberSetAddressTableRemoteNodeId()
 * <li> emberGetExtendedTimeout()
 * <li> emberGetAddressTableRemoteEui64()
 * <li> emberGetAddressTableRemoteNodeId()
 * <li> emberIdConflictHandler()
 * <li> emberIncomingManyToOneRouteRequestHandler()
 * <li> emberGetSenderEui64()
 * <li> emIncomingRouteRecord()
 * <li> emberSetExtendedTimeout()
 * <li> ::emberMulticastTable
 * <li> ::emberMulticastTableSize
 * <li> ::EMBER_PRIVATE_PROFILE_ID
 * </ul>
 * <li> <b>Changed items</b>
 * <ul>
 * <li> emberMessageSent() - Name changed to emberMessageSentHandler(),
 *      new type, index, and apsFrame arguments, and moved from ember.h.
 * <li> emberSendMulticast() - The arguments and over-the-air protocol have
 *      changed and moved from ember.h.
 * <li> emberSendUnicast() - The arguments and over-the-air protocol have
 *      changed and moved from ember.h.
 *      New functionality subsumes the former emberSendDatagram().
 * <li> emberSendBroadcast() - New destination argument and moved from ember.h.
 * <li> emberSendReply() - Cluster id argument changed from uint8_t to uint16_t
 *      and moved from ember.h.
 * </ul>
 * <li> <b>Removed items</b>
 * <ul>
 * <li>
 * </ul>
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_MESSAGE_H
