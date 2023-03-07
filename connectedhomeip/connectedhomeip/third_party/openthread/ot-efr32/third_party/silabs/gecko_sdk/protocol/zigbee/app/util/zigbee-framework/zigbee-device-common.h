/***************************************************************************//**
 * @file
 * @brief ZigBee Device Object (ZDO) functions available on all platforms.
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

#ifndef SILABS_ZIGBEE_DEVICE_COMMON_H
#define SILABS_ZIGBEE_DEVICE_COMMON_H

/** @addtogroup util_zdo
 * @{
 */

/**
 * @brief ZDO messages start with a sequence number.
 */
#define ZDO_MESSAGE_OVERHEAD 1

/** @name Service Discovery Functions
 * @{
 */

/**
 * @brief Request the specified node to send its node descriptor.
 * The node descriptor contains information about the capabilities of the ZigBee
 * node. It describes logical type, APS flags, frequency band, MAC capabilities
 * flags, manufacturer code and maximum buffer size. It is defined in the ZigBee
 * Application Framework Specification.
 *
 * @param target  The node whose node descriptor is desired.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An ::EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberNodeDescriptorRequest(EmberNodeId target,
                                       EmberApsOption options);
#else
// Macroized to save code space.
EmberStatus emberSendZigDevRequestTarget(EmberNodeId target,
                                         uint16_t clusterId,
                                         EmberApsOption options);
#define emberNodeDescriptorRequest(target, opts) \
  (emberSendZigDevRequestTarget((target), NODE_DESCRIPTOR_REQUEST, (opts)))
#endif

/**
 * @brief Request the specified node to send its power descriptor.
 * The power descriptor gives a dynamic indication of the power
 * status of the node. It describes current power mode,
 * available power sources, current power source and
 * current power source level. It is defined in the ZigBee
 * Application Framework Specification.
 *
 * @param target  The node whose power descriptor is desired.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberPowerDescriptorRequest(EmberNodeId target,
                                        EmberApsOption options);
#else
// Macroized to save code space.
#define emberPowerDescriptorRequest(target, opts) \
  (emberSendZigDevRequestTarget((target), POWER_DESCRIPTOR_REQUEST, (opts)))
#endif

/**
 * @brief Request the specified node to send the simple descriptor for
 * the specified endpoint.
 * The simple descriptor contains information specific
 * to a single endpoint. It describes the application profile identifier,
 * application device identifier, application device version, application flags,
 * application input clusters and application output clusters. It is defined in
 * the ZigBee Application Framework Specification.
 *
 * @param target  The node of interest.
 * @param targetEndpoint  The endpoint on the target node whose simple
 * descriptor is desired.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
EmberStatus emberSimpleDescriptorRequest(EmberNodeId target,
                                         uint8_t targetEndpoint,
                                         EmberApsOption options);

/**
 * @brief Request the specified node to send a list of its active
 * endpoints. An active endpoint is one for which a simple descriptor is
 * available.
 *
 * @param target  The node whose active endpoints are desired.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberActiveEndpointsRequest(EmberNodeId target,
                                        EmberApsOption options);
#else
// Macroized to save code space.
#define emberActiveEndpointsRequest(target, opts) \
  (emberSendZigDevRequestTarget((target), ACTIVE_ENDPOINTS_REQUEST, (opts)))
#endif

/** @} END name group */

/** @name Binding Manager Functions
 * @{
 */

/**
 * @brief Send a request to create a binding entry with the specified
 * contents on the specified node.
 *
 * @param target  The node on which the binding will be created.
 * @param source  The source EUI64 in the binding entry.
 * @param sourceEndpoint  The source endpoint in the binding entry.
 * @param clusterId  The cluster ID in the binding entry.
 * @param type  The type of binding, either ::UNICAST_BINDING,
 *   ::MULTICAST_BINDING, or ::UNICAST_MANY_TO_ONE_BINDING.
 *   ::UNICAST_MANY_TO_ONE_BINDING is an Ember-specific extension
 *   and should be used only when the target is an Ember device.
 * @param destination  The destination EUI64 in the binding entry for
 *   ::UNICAST_BINDING or ::UNICAST_MANY_TO_ONE_BINDING.
 * @param groupAddress  The group address for the ::MULTICAST_BINDING.
 * @param destinationEndpoint  The destination endpoint in the binding entry for
 *   the ::UNICAST_BINDING or ::UNICAST_MANY_TO_ONE_BINDING.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberBindRequest(EmberNodeId target,
                             EmberEUI64 source,
                             uint8_t sourceEndpoint,
                             uint16_t clusterId,
                             uint8_t type,
                             EmberEUI64 destination,
                             EmberMulticastId groupAddress,
                             uint8_t destinationEndpoint,
                             EmberApsOption options);
#else
// Macroized to save code space.
#define emberBindRequest(target,                              \
                         src,                                 \
                         srcEndpt,                            \
                         cluster,                             \
                         type,                                \
                         dest,                                \
                         groupAddress,                        \
                         destEndpt,                           \
                         opts)                                \
                                                              \
  (emberSendZigDevBindRequest((target),                       \
                              BIND_REQUEST,                   \
                              (src), (srcEndpt), (cluster),   \
                              (type), (dest), (groupAddress), \
                              (destEndpt), (opts)))

EmberStatus emberSendZigDevBindRequest(EmberNodeId target,
                                       uint16_t bindClusterId,
                                       EmberEUI64 source,
                                       uint8_t sourceEndpoint,
                                       uint16_t clusterId,
                                       uint8_t type,
                                       EmberEUI64 destination,
                                       EmberMulticastId groupAddress,
                                       uint8_t destinationEndpoint,
                                       EmberApsOption options);
#endif

/**
 * @brief Send a request to remove a binding entry with the specified
 * contents from the specified node.
 *
 * @param target          The node on which the binding will be removed.
 * @param source          The source EUI64 in the binding entry.
 * @param sourceEndpoint  The source endpoint in the binding entry.
 * @param clusterId       The cluster ID in the binding entry.
 * @param type            The type of binding, either ::UNICAST_BINDING,
 *  ::MULTICAST_BINDING, or ::UNICAST_MANY_TO_ONE_BINDING.
 *  ::UNICAST_MANY_TO_ONE_BINDING is an Ember-specific extension
 *  and should be used only when the target is an Ember device.
 * @param destination     The destination EUI64 in the binding entry for the
 *   ::UNICAST_BINDING or ::UNICAST_MANY_TO_ONE_BINDING.
 * @param groupAddress    The group address for the ::MULTICAST_BINDING.
 * @param destinationEndpoint  The destination endpoint in the binding entry for
 *   the ::UNICAST_BINDING or ::UNICAST_MANY_TO_ONE_BINDING.
 * @param options         The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_NO_BUFFERS
 * _ ::EMBER_NETWORK_DOWN
 * - ::EMBER_NETWORK_BUSY
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberUnbindRequest(EmberNodeId target,
                               EmberEUI64 source,
                               uint8_t sourceEndpoint,
                               uint16_t clusterId,
                               uint8_t type,
                               EmberEUI64 destination,
                               EmberMulticastId groupAddress,
                               uint8_t destinationEndpoint,
                               EmberApsOption options);
#else
// Macroized to save code space.
#define emberUnbindRequest(target,                            \
                           src,                               \
                           srcEndpt,                          \
                           cluster,                           \
                           type,                              \
                           dest,                              \
                           groupAddress,                      \
                           destEndpt,                         \
                           opts)                              \
                                                              \
  (emberSendZigDevBindRequest((target),                       \
                              UNBIND_REQUEST,                 \
                              (src), (srcEndpt), (cluster),   \
                              (type), (dest), (groupAddress), \
                              (destEndpt), (opts)))
#endif

/** @} END name group  */

/** @name Node Manager Functions
 * @{
 */

/**
 * @brief Request the specified node to send its LQI (neighbor) table.
 * The response gives PAN ID, EUI64, node ID and cost for each neighbor. The
 * EUI64 is only available if security is enabled. The other fields in the
 * response are set to zero. The response format is defined in the ZigBee Device
 * Profile Specification.
 *
 * @param target  The node whose LQI table is desired.
 * @param startIndex  The index of the first neighbor to include in the
 * response.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberLqiTableRequest(EmberNodeId target,
                                 uint8_t startIndex,
                                 EmberApsOption options);
#else
#define emberLqiTableRequest(target, startIndex, options) \
  (emberTableRequest(LQI_TABLE_REQUEST, (target), (startIndex), (options)))

EmberStatus emberTableRequest(uint16_t clusterId,
                              EmberNodeId target,
                              uint8_t startIndex,
                              EmberApsOption options);
#endif

/**
 * @brief Request the specified node to send its routing table.
 * The response gives destination node ID, status and many-to-one flags,
 * and the next hop node ID.
 * The response format is defined in the ZigBee Device
 * Profile Specification.
 *
 * @param target  The node whose routing table is desired.
 * @param startIndex  The index of the first route entry to include in the
 * response.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberRoutingTableRequest(EmberNodeId target,
                                     uint8_t startIndex,
                                     EmberApsOption options);
#else
#define emberRoutingTableRequest(target, startIndex, options) \
  (emberTableRequest(ROUTING_TABLE_REQUEST, (target), (startIndex), (options)))
#endif

/**
 * @brief Request the specified node to send its nonvolatile bindings.
 * The response gives source address, source endpoint, cluster ID, destination
 * address and destination endpoint for each binding entry. The response format
 * is defined in the ZigBee Device Profile Specification.
 * Note that bindings that have the Ember-specific ::UNICAST_MANY_TO_ONE_BINDING
 * type are reported as having the standard ::UNICAST_BINDING type.
 *
 * @param target  The node whose binding table is desired.
 * @param startIndex  The index of the first binding entry to include in the
 * response.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberBindingTableRequest(EmberNodeId target,
                                     uint8_t startIndex,
                                     EmberApsOption options);
#else
#define emberBindingTableRequest(target, startIndex, options) \
  (emberTableRequest(BINDING_TABLE_REQUEST, (target), (startIndex), (options)))
#endif
/**
 * @brief Request the specified node to remove the specified device from
 * the network. The device to be removed must be the node to which the request
 * is sent or one of its children.
 *
 * @param target  The node which will remove the device.
 * @param deviceAddress  All zeros if the target is to remove itself from
 *    the network or the EUI64 of a child of the target device to remove
 *    that child.
 * @param leaveRequestFlags  A bitmask of leave options.
 *   Include ::LEAVE_REQUEST_REMOVE_CHILDREN_FLAG if the target is
 *   to remove their children and/or ::LEAVE_REQUEST_REJOIN_FLAG
 *   if the target is to rejoin the network immediately after
 *   leaving.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
EmberStatus emberLeaveRequest(EmberNodeId target,
                              EmberEUI64 deviceAddress,
                              uint8_t leaveRequestFlags,
                              EmberApsOption options);

/**
 * @brief Request the specified node to allow or disallow association.
 *
 * @param target  The node which will allow or disallow association. The request
 * can be broadcast by using a broadcast address (0xFFFC/0xFFFD/0xFFFF). No
 * response is sent if the request is broadcast.
 * @param duration  A value of 0x00 disables joining. A value of 0xFF enables
 * joining.  Any other value enables joining for that number of seconds.
 * @param authentication  Controls Trust Center authentication behavior.
 * @param options  The options to use when sending the request. See
 * emberSendUnicast() for a description. This parameter is ignored if the target
 * is a broadcast address.
 *
 * @return An EmberStatus value. ::EMBER_SUCCESS, ::EMBER_NO_BUFFERS,
 * ::EMBER_NETWORK_DOWN or ::EMBER_NETWORK_BUSY.
 */
EmberStatus emberPermitJoiningRequest(EmberNodeId target,
                                      uint8_t duration,
                                      uint8_t authentication,
                                      EmberApsOption options);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Change the default radius for broadcast ZDO requests
 *
 * @param radius  The radius to be used for future ZDO request broadcasts.
 */
void emberSetZigDevRequestRadius(uint8_t radius);

/**
 * @brief Retrieve the default radius for broadcast ZDO requests
 *
 * @return  The radius to be used for future ZDO request broadcasts.
 */
uint8_t emberGetZigDevRequestRadius(void);
/**
 * @brief Provide access to the application ZDO transaction sequence number for
 * last request. This function has been deprecated and replaced by
 * emberGetLastAppZigDevRequestSequence().
 *
 * @return  Last application ZDO transaction sequence number used
 */
uint8_t emberGetLastZigDevRequestSequence(void);
#else
extern uint8_t zigDevRequestRadius;
#define emberGetZigDevRequestRadius() (zigDevRequestRadius)
#define emberSetZigDevRequestRadius(x)        (zigDevRequestRadius = x)
#define emberGetLastZigDevRequestSequence() \
  (emberGetLastAppZigDevRequestSequence())
#endif

/**
 * @brief Provide access to the application ZDO transaction sequence number for
 * last request.
 *
 * @return  Last application ZDO transaction sequence number used
 */
uint8_t emberGetLastAppZigDevRequestSequence(void);

/** @} END name group */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//------------------------------------------------------------------------------
// Utility functions used by the library code.

EmberStatus emberSendZigDevRequest(EmberNodeId destination,
                                   uint16_t clusterId,
                                   EmberApsOption options,
                                   uint8_t *contents,
                                   uint8_t length);

/**
 * @brief Get the next device request sequence number.
 *
 * Requests have sequence numbers so that they can be matched up with the
 * responses.  To avoid complexities, the library uses numbers with the high
 * bit clear and the stack uses numbers with the high bit set.
 *
 * @return The next device request sequence number
 */
uint8_t emberNextZigDevRequestSequence(void);

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @} END addtogroup */

#endif // SILABS_ZIGBEE_DEVICE_COMMON_H
