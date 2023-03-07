/***************************************************************************//**
 * @file
 * @brief EmberZNet API for accessing and setting stack information.
 * See @ref stack_info for documentation.
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

#ifndef SILABS_STACK_INFO_H
#define SILABS_STACK_INFO_H

/**
 * @addtogroup stack_info
 *
 * See stack-info.h for source code.
 * @{
 */

/** @brief A callback invoked when the status of the stack changes.
 * If the status parameter equals ::EMBER_NETWORK_UP, the
 * ::emberGetNetworkParameters()
 * function can be called to get the new network parameters. If any of the
 * parameters are stored in nonvolatile memory by the application, the
 * stored values should be updated.
 *
 * The application is free to begin messaging after it receives the
 * ::EMBER_NETWORK_UP status.  However, routes discovered immediately after
 * the stack comes up may be suboptimal because the routes are based
 * on the neighbor table's information about two-way links with neighboring nodes,
 * which is obtained from periodic ZigBee Link Status messages.  It can take
 * two or three link status exchange periods (of 16 seconds each) before the
 * neighbor table has a good estimate of link quality to neighboring nodes.
 * Therefore, the application may improve the quality of initially discovered
 * routes by waiting after startup to give the neighbor table time
 * to be populated.
 *
 * @param status  Stack status. One of the following:
 * - ::EMBER_NETWORK_UP
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_JOIN_FAILED
 * - ::EMBER_MOVE_FAILED
 * - ::EMBER_CANNOT_JOIN_AS_ROUTER
 * - ::EMBER_NODE_ID_CHANGED
 * - ::EMBER_PAN_ID_CHANGED
 * - ::EMBER_CHANNEL_CHANGED
 * - ::EMBER_NETWORK_OPENED
 * - ::EMBER_NETWORK_CLOSED
 * - ::EMBER_NO_BEACONS
 * - ::EMBER_RECEIVED_KEY_IN_THE_CLEAR
 * - ::EMBER_NO_NETWORK_KEY_RECEIVED
 * - ::EMBER_NO_LINK_KEY_RECEIVED
 * - ::EMBER_PRECONFIGURED_KEY_REQUIRED
 */
void emberStackStatusHandler(EmberStatus status);

/** @brief Return the current join status.
 *
 *   Returns a value indicating whether the node is joining,
 *   joined to, or leaving a network.
 *
 * @return An ::EmberNetworkStatus value indicating the current join status.
 */
EmberNetworkStatus emberNetworkState(void);

/** @brief Indicate whether the stack is currently up.
 *
 *   Returns true if the stack is joined to a network and
 *   ready to send and receive messages.  This reflects only the state
 *   of the local node; it does not indicate whether other nodes are
 *   able to communicate with this node.
 *
 * @return True if the stack is up, false otherwise.
 */
bool emberStackIsUp(void);

/** @brief Write node data to the stack token.
 *
 * @param bool Indicates whether the node type should be erased.
 *
 * @return The status of the operation and an error code if unsuccessful.
 */
EmberStatus emberWriteNodeData(bool erase);

/**
 * @brief Check when the next stack event is scheduled to run.
 * @return
 *   Returns the number of milliseconds before the next stack event is
 *   scheduled to run.
 */
uint32_t emberMsToNextStackEvent(void);

/** @brief Set the power descriptor to the specified value. The power
 * descriptor is a dynamic value, therefore this function should be called
 * whenever the value changes.
 *
 * @param descriptor  The new power descriptor for the local node.
 */
void emberSetPowerDescriptor(uint16_t descriptor);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Return the EUI64 ID of the local node.
 *
 * @return The 64-bit ID.
 */
EmberEUI64 emberGetEui64(void);

/** @brief Determine whether \c eui64 is the local node's EUI64 ID.
 *
 * @param eui64  An EUI64 ID.
 *
 * @return True if \c eui64 is the local node's ID, otherwise false.
 */
bool emberIsLocalEui64(EmberEUI64 eui64);

/** @brief Return the 16-bit node ID of local node on the current logical
 * network.
 *
 * @return The 16-bit ID.
 */
EmberNodeId emberGetNodeId(void);

/** @brief Return the 16-bit node ID of local node on the network it is
 * currently tuned on.
 *
 * @return The 16-bit ID.
 */
EmberNodeId emberRadioGetNodeId(void);

/** @brief Set the manufacturer code to the specified value. The
 * manufacturer code is one of the fields of the node descriptor.
 *
 * @param code  The manufacturer code for the local node.
 */
void emberSetManufacturerCode(uint16_t code);

/** @brief Set the maximum incoming transfer size to the specified value.
 * The maximum incoming transfer size is one of the fields of the node
 * descriptor.
 *
 * @param size  The maximum incoming transfer size for the local node.
 */
void emberSetMaximumIncomingTransferSize(uint16_t size);

/** @brief Set the maximum outgoing transfer size to the specified value.
 * The maximum outgoing transfer size is one of the fields of the node
 * descriptor.
 *
 * @param size  The maximum outgoing transfer size for the local node.
 */
void emberSetMaximumOutgoingTransferSize(uint16_t size);

/** @brief Set the descriptor capability field of the node.
 *
 * @param capability  The descriptor capability of the local node.
 */
void emberSetDescriptorCapability(uint8_t capability);

#else   // Doxygen ignores the following
extern EmberEUI64 emLocalEui64;
#define emberGetEui64() (emLocalEui64)
#define emberIsLocalEui64(eui64) \
  (MEMCOMPARE((eui64), emLocalEui64, EUI64_SIZE) == 0)

EmberNodeId emberGetNodeId(void);
EmberNodeId emberRadioGetNodeId(void);

extern uint16_t emManufacturerCode;
extern uint16_t emPowerDescriptor;
extern uint16_t emMaximumIncomingTransferSize;
extern uint16_t emMaximumOutgoingTransferSize;
extern uint8_t emDescriptorCapability;

enum {
  EMBER_ROUTE_ERROR_NO_ROUTE_AVAILABLE    = 0x00,
  EMBER_ROUTE_ERROR_TREE_LINK_FAILURE     = 0x01,
  EMBER_ROUTE_ERROR_NON_TREE_LINK_FAILURE = 0x02,
  EMBER_ROUTE_ERROR_LOW_BATTERY_LEVEL     = 0x03,
  EMBER_ROUTE_ERROR_NO_ROUTING_CAPACITY   = 0x04,
  EMBER_ROUTE_ERROR_NO_INDIRECT_CAPACITY  = 0x05,
  EMBER_ROUTE_ERROR_INDIRECT_TRANSACTION_EXPIRY = 0x06,
  EMBER_ROUTE_ERROR_TARGET_DEVICE_UNAVAILABLE = 0x07,
  EMBER_ROUTE_ERROR_TARGET_ADDRESS_UNALLOCATED = 0x08,
  EMBER_ROUTE_ERROR_PARENT_LINK_FAILURE   = 0x09,
  EMBER_ROUTE_ERROR_VALIDATE_ROUTE        = 0x0A,
  EMBER_ROUTE_ERROR_SOURCE_ROUTE_FAILURE  = 0x0B,
  EMBER_ROUTE_ERROR_MANY_TO_ONE_ROUTE_FAILURE = 0x0C,
  EMBER_ROUTE_ERROR_ADDRESS_CONFLICT      = 0x0D,
  EMBER_ROUTE_ERROR_VERIFY_ADDRESSES      = 0x0E,
  EMBER_ROUTE_ERROR_PAN_IDENTIFIER_UPDATE = 0x0F,
  ZIGBEE_NETWORK_STATUS_NETWORK_ADDRESS_UPDATE = 0x10,
  ZIGBEE_NETWORK_STATUS_BAD_FRAME_COUNTER = 0x11,
  ZIGBEE_NETWORK_STATUS_BAD_KEY_SEQUENCE_NUMBER = 0x12,
  ZIGBEE_NETWORK_STATUS_UNKNOWN_COMMAND = 0x13
};

/** @brief Set the error code that a router uses to notify
 * the message initiator about a broken route.
 *
 * @param errorCode one of the 2 error codes that the stack can handle:
 * EMBER_ROUTE_ERROR_NO_ROUTE_AVAILABLE(0x00) or
 * EMBER_ROUTE_ERROR_NON_TREE_LINK_FAILURE(0x02) .
 *
 * @return ::EMBER_BAD_ARGUMENT if error code is neither
 * EMBER_ROUTE_ERROR_NO_ROUTE_AVAILABLE(0x00) nor
 * EMBER_ROUTE_ERROR_NON_TREE_LINK_FAILURE(0x02)
 *  Returns ::EMBER_SUCCESS otherwise
 */
EmberStatus emberSetBrokenRouteErrorCode(uint8_t errorCode);

#define emberSetManufacturerCode(code) \
  (emManufacturerCode = (code))
#define emberSetMaximumIncomingTransferSize(size) \
  (emMaximumIncomingTransferSize = (size))
#define emberSetMaximumOutgoingTransferSize(size) \
  (emMaximumOutgoingTransferSize = (size))
#define emberSetDescriptorCapability(capability) \
  (emDescriptorCapability = (capability))
#endif

/** @brief Copy the current network parameters into the structure
 * provided by the caller.
 *
 * @param parameters  A pointer to an EmberNetworkParameters value
 *  into which the current network parameters will be copied.
 *
 * @return An ::EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus emberGetNetworkParameters(EmberNetworkParameters *parameters);

/** @brief Copy the current radio parameters into the structure
 * provided by the caller.
 *
 * @param phyIndex  The index of the PHY interface for radio parameters.
 *            - For 2.4 or SubGHz only (switched) device, index will be ignored.
 *            - For simultaneous dual radio, returns radio parameters based on
 *              the provided PHY index.
 *  parameters  A pointer to an EmberMultiPhyRadioParameters value
 *  into which the current radio parameters will be copied.
 *
 * @return An ::EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus emberGetRadioParameters(uint8_t phyIndex, EmberMultiPhyRadioParameters *parameters);

/** @brief Copy the current node type into the location provided
 * by the caller.
 *
 * @param resultLocation  A pointer to an EmberNodeType value
 *  into which the current node type will be copied.
 *
 * @return An ::EmberStatus value that indicates the success or failure
 *  of the command.
 */
EmberStatus emberGetNodeType(EmberNodeType *resultLocation);

/** @brief Set the channel to use for sending and receiving messages on the
 * current logical network.
 * For a list of available radio channels, see the technical specification for
 * the RF communication module in your Developer Kit.
 *
 * Note: When using this API,
 * all devices on a network must use the same channel.
 *
 * @param channel  The desired radio channel.
 *
 * @return An ::EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus emberSetRadioChannel(uint8_t channel);

/**
 * @brief Set the current logical and physical channel.
 */
EmberStatus emberSetLogicalAndRadioChannel(uint8_t channel);

/** @brief Get the radio channel to which a node is set on the current
 * logical network. The possible return values depend on the radio in use.
 * For a list of available radio channels, see the technical specification
 * for the RF communication module in your Developer Kit.
 *
 * @return The current radio channel.
 */
uint8_t emberGetRadioChannel(void);

/** @brief Set the radio output power at which a node is to operate for the
 * current logical network. Ember radios have discrete power settings. For a
 * list of available power settings, see the technical specification for the RF
 * communication module in your Developer Kit.
 * Note that using this API on a running network
 * will directly impact the established link qualities which the neighboring nodes
 * have with the node on which it is called.  This can lead to disruption of
 * existing routes and erratic network behavior.
 * Note that, if the requested power level is not available on a given radio, this
 * function will use the next higher available power level.
 *
 * @param power  A desired radio output power, in dBm.
 *
 * @return An ::EmberStatus value indicating the success or
 *  failure of the command.  Failure indicates that the requested power level
 *  is out of range.
 */
EmberStatus emberSetRadioPower(int8_t power);

/** @brief Get the radio output power of the current logical network at which
 * a node is operating. Ember radios have discrete power settings. For a list of
 * available power settings, see the technical specification for the RF
 * communication module in your Developer Kit.
 *
 * @return The current radio output power, in dBm.
 */
int8_t emberGetRadioPower(void);

/** @brief Return the RAIL level handle required to make RAIL API calls. Will
 * return NULL if RAIL is not in use.
 *
 * @return A RAIL handle
 */
void *emberGetRailHandle(void);

/** @brief Return the local node's PAN ID of the current logical network.
 *
 * @return A PAN ID.
 */
EmberPanId emberGetPanId(void);

/** @brief Return the local node's PAN ID of the current radio network.
 *
 * @return A PAN ID.
 */
EmberPanId emberRadioGetPanId(void);

/** @brief Set the configured 802.15.4 CCA mode in the radio. For a list of
 * CCA modes, please refer to RAIL documentation regarding
 * RAIL_IEEE802154_CcaMode_t.
 *
 * @param ccaMode  A RAIL_IEEE802154_CcaMode_t value
 *
 * @return EMBER_SUCCESS upon success, EMBER_ERR_FATAL otherwise.
 *
 * @note The default CCA mode for zigbee is RAIL_IEEE802154_CCA_MODE_RSSI.
 *
 * @note As mentioned in the RAIL documentation, on platforms that don't support
 * different CCA modes, a call to this function will do nothing.
 */
EmberStatus emberSetRadioIeee802154CcaMode(uint8_t ccaMode);

/** @brief Fetch a node's 8 byte Extended PAN identifier. If this is called
 *  when a device is not currently on a network (see ::emberNetworkState),
 *  the Extended PAN ID returned will be an invalid value.
 */
void emberGetExtendedPanId(uint8_t *resultLocation);

/** @brief The application must provide a definition for this variable. */
extern const uint8_t emberStackProfileId[];

/** @brief Endpoint information (a ZigBee Simple Descriptor).
 *
 * This is a ZigBee Simple Descriptor and contains information
 * about an endpoint.  This information is shared with other nodes in the
 * network by the ZDO.
 */

typedef struct {
  /** Identifies the endpoint's application profile. */
  uint16_t profileId;
  /** The endpoint's device ID within the application profile. */
  uint16_t deviceId;
  /** The endpoint's device version. */
  uint8_t deviceVersion;
  /** The number of input clusters. */
  uint8_t inputClusterCount;
  /** The number of output clusters. */
  uint8_t outputClusterCount;
} EmberEndpointDescription;

/** @brief Give the endpoint information for a particular endpoint.
 */

typedef struct {
  /** An endpoint of the application on this node. */
  uint8_t endpoint;
  /** The endpoint's description. */
  EmberEndpointDescription const * description;
  /** Input clusters the endpoint will accept. */
  uint16_t const * inputClusterList;
  /** Output clusters the endpoint may send. */
  uint16_t const * outputClusterList;
} EmberEndpoint;

/** @brief The application must provide a definition for this variable. */
extern uint8_t emberEndpointCount;

/** @brief If emberEndpointCount is nonzero, the application must
 * provide descriptions for each endpoint.
 *
 * This can be done either
 * by providing a definition of emberEndpoints or by providing definitions
 * of ::emberGetEndpoint(), ::emberGetEndpointDescription() and
 * ::emberGetEndpointCluster().  Using the array is often simpler, but consumes
 * large amounts of memory if emberEndpointCount is large.
 *
 * If the application provides definitions for the three functions, it must
 * define EMBER_APPLICATION_HAS_GET_ENDPOINT in its
 * CONFIGURATION_HEADER.
 */
extern EmberEndpoint emberEndpoints[];

/** @brief Retrieve the endpoint number for
 * the index'th endpoint.  <code> index </code> must be less than
 * the value of emberEndpointCount.
 *
 * This function is provided by the stack, using the data from
 * emberEndpoints, unless the application defines
 * EMBER_APPLICATION_HAS_GET_ENDPOINT in its CONFIGURATION_HEADER.
 *
 * @param index  The index of an endpoint (as distinct from its endpoint
 * number).  This must be less than the value of emberEndpointCount.
 *
 * @return The endpoint number for the index'th endpoint.
 */
uint8_t emberGetEndpoint(uint8_t index);

/** @brief Retrieve the endpoint description for the
 * given endpoint.
 *
 * This function is provided by the stack, using the data from
 * emberEndpoints, unless the application defines
 * ::EMBER_APPLICATION_HAS_GET_ENDPOINT in its ::CONFIGURATION_HEADER.
 *
 * @param endpoint  The endpoint whose description is to be returned.
 *
 * @param result    A pointer to the location to which to copy the endpoint
 *    description.
 *
 * @return True if the description was copied to result, or false if the
 * endpoint is not active.
 */
bool emberGetEndpointDescription(uint8_t endpoint,
                                 EmberEndpointDescription *result);

/** @brief Retrieve a cluster ID from one of the cluster lists associated
 * with the given endpoint.
 *
 * This function is provided by the stack, using the data from
 * emberEndpoints, unless the application defines
 * ::EMBER_APPLICATION_HAS_GET_ENDPOINT in its CONFIGURATION_HEADER.
 *
 * @param endpoint   The endpoint from which the cluster ID is to be read.
 *
 * @param listId     The list from which the cluster ID is to be read.
 *
 * @param listIndex  The index of the desired cluster ID in the list. This value
 * must be less than the length of the list. The length can be found in the
 * EmberEndpointDescription for this endpoint.
 *
 * @return The cluster ID at position listIndex in the specified endpoint
 * cluster list.
 */
uint16_t emberGetEndpointCluster(uint8_t endpoint,
                                 EmberClusterListId listId,
                                 uint8_t listIndex);

/** @brief Determine whether \c nodeId is valid.
 *
 * @param nodeId  A node ID.
 *
 * @return True if \c nodeId is valid, false otherwise.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
bool emberIsNodeIdValid(EmberNodeId nodeId);
#else
#define emberIsNodeIdValid(nodeId) ((nodeId) < EMBER_DISCOVERY_ACTIVE_NODE_ID)
#endif

/** @brief Return the node ID that corresponds to the specified EUI64.
 * The node ID is found by searching through all stack tables for the specified
 * EUI64.
 *
 * @param eui64  The EUI64 of the node to look up.
 *
 * @return The short ID of the node or ::EMBER_NULL_NODE_ID if the short ID
 * is not known.
 */
EmberNodeId emberLookupNodeIdByEui64(EmberEUI64 eui64);

/** @brief Return the EUI64 that corresponds to the specified node ID.
 * The EUI64 is found by searching through all stack tables for the specified
 * node ID.
 *
 * @param nodeId       The short ID of the node to look up.
 *
 * @param eui64Return  The EUI64 of the node is copied here if it is known.
 *
 * @return An ::EmberStatus value:\n\n
 * - ::EMBER_SUCCESS - eui64Return has been set to the EUI64 of the node.
 * - ::EMBER_ERR_FATAL - The EUI64 of the node is not known.
 */
EmberStatus emberLookupEui64ByNodeId(EmberNodeId nodeId,
                                     EmberEUI64 eui64Return);

/** @brief A callback invoked to inform the application of the
 * occurrence of an event defined by ::EmberCounterType, for example,
 * transmissions and receptions at different layers of the stack.
 *
 * The application must define ::EMBER_APPLICATION_HAS_COUNTER_HANDLER
 * in its CONFIGURATION_HEADER to use this.
 * This function may be called in ISR context, so processing should
 * be kept to a minimum.
 *
 * @param type  Type of the event.
 * @param info could map to:
 * 1. data: For transmission events, the number of retries used.
 * For most other events, this parameter is unused and is set to zero.
 * 2. phyIndex: use for mac specific counters specifying if they belong to sub-GHz or 2.4 GHz.
 * 3. destinationNodeId: identifying which connection/destinationId a specific counter refers to.
 * 4. Any combination of the parameters above.
 */

void emberCounterHandler(EmberCounterType type, EmberCounterInfo info);
bool emberCounterRequiresPhyIndex(EmberCounterType type);
bool emberCounterRequiresDestinationNodeId(EmberCounterType type);

/** @brief A callback invoked to inform the application that a stack token has
 * changed.
 *
 * @param tokenAddress  The address of the stack token that has changed.
 */
void emberStackTokenChangedHandler(uint16_t tokenAddress);

/** @brief A callback to allow the application to manage idling the MCU.
 *
 * @param idleTimeMs  The time in millisecond the stack is allowed to idle.
 *
 * @return   True if the application is managing idling the MCU, false otherwise.
 * If this function returns false, the stack will manage idling the MCU.
 */
bool emberRtosIdleHandler(uint32_t *idleTimeMs);

/** @brief A callback to request the application to wake up the stack task.
 */
void emberRtosStackWakeupIsrHandler(void);

/** @brief Copy a neighbor table entry to the structure that
 * \c result points to.  Neighbor table entries are stored in
 * ascending order by node ID, with all unused entries at the end
 * of the table.  The number of active neighbors can be obtained
 * using ::emberNeighborCount().
 *
 * @param index   The index of a neighbor table entry.
 *
 * @param result  A pointer to the location to which to copy the neighbor
 * table entry.
 *
 * @return ::EMBER_ERR_FATAL if the index is greater or equal to the
 * number of active neighbors, or if the device is an end device.
 * Returns ::EMBER_SUCCESS otherwise.
 */
EmberStatus emberGetNeighbor(uint8_t index, EmberNeighborTableEntry *result);

/** @brief Set the routing shortcut threshold to directly use a neighbor instead of
 * performing routing.
 *
 * @param costThresh the routing shortcut value that should be used to directly use a neighbor
 * instead of performing routing.
 *
 * @return ::return EMBER_BAD_ARGUMENT if the costThresh is not 1, 3, 5, 7
 * Returns ::EMBER_SUCCESS otherwise.
 */
EmberStatus emberSetRoutingShortcutThreshold(uint8_t costThresh);

/** @brief Get the routing shortcut threshold used to separate between directly using a neighbor
 * vs. performing routing.
 *
 * @return the routingShortcutThresh which takes one of the following values: 1, 3, 5, or 7.
 *
 */
uint8_t emberGetRoutingShortcutThreshold(void);

/** @brief Set neighbor's initial outgoing link cost
 *  @param cost The new default cost.
 *
 *  @return ::EMBER_BAD_ARGUMENT if the cost is not any of 0, 1, 3, 5, or 7.
 *  Returns ::EMBER_SUCCESS otherwise
 *  NOTE: There is no EZSP version of this function yet.
 */
EmberStatus emberSetInitialNeighborOutgoingCost(uint8_t cost);

/** @brief Get neighbor's initial outgoing link cost.
 *
 *  @return The default cost associated with new neighbor's outgoing links.
 *  NOTE: There is no EZSP version of this function yet.
 */
uint8_t emberGetInitialNeighborOutgoingCost(void);

/** @brief Indicate whether a rejoining neighbor's incoming FC should be reset.
 *  @param reset True or False.
 *
 *  @return ::void
 *  NOTE: There is no EZSP version of this function yet.
 */
void emberResetRejoiningNeighborsFC(bool reset);

/** @brief Check whether resetting the incoming FC for a rejoining neighbor is enabled.
 *  @param void
 *
 *  @return ::True or False.
 *  NOTE: There is no EZSP version of this function yet.
 */
bool emberIsResetRejoiningNeighborsFCEnabled(void);

/** @brief Get the last received frame counter as found in the
 *   Network Auxiliary header for the specified neighbor or child.
 *  @param nodeEui64  The neighbor or child EUI to search.
 *  @param returnFrameCounter  The frame counter variable to fill in.
 *
 *  @return EmberStatus depending on whether the frame counter is found in the
 *   neighbor or child table.
 */
EmberStatus emberGetNeighborFrameCounter(EmberEUI64 nodeEui64,
                                         uint32_t *returnFrameCounter);

/** @brief Sets the frame counter for the specified neighbor or child.
 *  @param nodeEui64  The neighbor or child EUI to search.
 *  @param frameCounter  The frame counter to set.
 *
 *  @return EmberStatus depending on whether the frame counter is set in the
 *   neighbor or child table.
 */
EmberStatus emberSetNeighborFrameCounter(EmberEUI64 nodeEui64,
                                         uint32_t frameCounter);

/** @brief Copy a route table entry to the structure that the
 * \c result points to. Unused route table entries have a destination
 * of 0xFFFF.  The route table size
 * can be obtained via ::emberRouteTableSize().
 *
 * @param index   The index of a route table entry.
 *
 * @param result  A pointer to the location to which to copy the route
 * table entry.
 *
 * @return ::EMBER_ERR_FATAL if the index is out of range or the device
 * is an end device, and ::EMBER_SUCCESS otherwise.
 */
EmberStatus emberGetRouteTableEntry(uint8_t index, EmberRouteTableEntry *result);

/** @brief Return the stack profile of the network which the
 * node has joined.
 *
 * @return  The stack profile.
 */
uint8_t emberStackProfile(void);

/** @brief Return the security level of the network which the
 * node has joined.
 *
 * @return  The security level.
 */
uint8_t emberSecurityLevel(void);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Return the depth of the node in the network.
 *
 * @return The current depth.
 */
uint8_t emberTreeDepth(void);
#endif
/** @brief Return the number of active entries in the neighbor table.
 *
 * @return The number of active entries in the neighbor table.
 */
uint8_t emberNeighborCount(void);
#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Return the size of the route table.
 *
 * @return The size of the route table.
 */
uint8_t emberRouteTableSize(void);

#else   // Doxgyen ignores the following
// The '+ 0' prevents anyone from accidentally assigning to these.
#define emberTreeDepth()           (emTreeDepth         + 0)
#define emberMaxDepth()            (emMaxDepth          + 0)
#define emberRouteTableSize()      (emRouteTableSize    + 0)

extern uint8_t emDefaultSecurityLevel;
extern uint8_t emMaxHops;
extern uint8_t emRouteTableSize;
extern uint8_t emSourceRouteTableSize;
extern uint8_t emMaxDepth;                // The maximum tree depth.
extern uint8_t emTreeDepth;               // The current depth.
extern uint8_t emEndDeviceConfiguration;

/** @brief Set the stack profile of the network.
 * This API is for SoC Only, Host-NCP should use
 * the ::EZSP_CONFIG_STACK_PROFILE configId.
 *
 * @param ::EMBER_STACK_PROFILE_NONE or
 * ::EMBER_STACK_PROFILE_ZIGBEE_PRO.
 *
 */
void emberSetStackProfile(uint8_t stackProfile);

/** @brief Set the security level of the network.
 * This API is for SoC Only, Host-NCP should use
 * the ::EZSP_CONFIG_SECURITY_LEVEL configId.
 *
 * @param ::EMBER_SECURITY_LEVEL_NONE or
 * ::EMBER_SECURITY_LEVEL_Z3.
 */
void emberSetSecurityLevel(uint8_t securityLevel);
#endif

/** @brief Increment and returns the Zigbee sequence number.
 *
 * @return The next Zigbee sequence number.
 */
uint8_t emberNextZigbeeSequenceNumber(void);

/**@brief Get maximum number of NWK retries.
 *
 * Returns the maximum number of NWK retries that will be attempted
 *
 * @return uint8_t Max NWK retries
 */
uint8_t emberGetMaxNwkRetries(void);

/** @name Radio-specific Functions*/
//@{

/** @brief Enable boost power mode and/or the alternate transmit path.
 *
 * Boost power mode is a high-performance radio mode,
 * which offers increased transmit power and receive sensitivity at the cost of
 * an increase in power consumption. ::emberInit() calls this
 * function using the power mode and transmitter output settings as specified
 * in the MFG_PHY_CONFIG token (with each bit inverted so that the default
 * token value of 0xffff corresponds to normal power mode and bi-directional RF
 * transmitter output). The application only needs to call
 * ::emberSetTxPowerMode() to use a power mode or transmitter output
 * setting different than specified in the MFG_PHY_CONFIG token.
 * After the initial call to ::emberSetTxPowerMode(), the stack
 * will automatically maintain the specified power mode configuration across
 * sleep/wake cycles.
 *
 * @param txPowerMode Specifies which transmit power mode options should
 * be activated. This parameter should be set to one of the literal values
 * described in stack/include/ember-types.h. Any power option not specified
 * in the txPowerMode parameter will be deactivated.
 *
 * @return ::EMBER_SUCCESS if successful; an error code otherwise.
 */
EmberStatus emberSetTxPowerMode(uint16_t txPowerMode);

/** @brief Return the current configuration of boost power mode and alternate
 * transmitter output.
 *
 * @return The current TX power mode.
 */
uint16_t emberGetTxPowerMode(void);

/** @brief Set the short node ID of the node. Notice that it can
 * only be set if the stack is in the INITAL state.
 *
 * @param nodeId Specifies the short ID to be assigned to the node.
 *
 * @return ::EMBER_SUCCESS if successful; an error code otherwise.
 */
EmberStatus emberSetNodeId(EmberNodeId nodeId);

/** @brief Return the number of PHY interfaces present.
 *
 * @return The number of PHY interface(s).
 */
uint8_t emberGetPhyInterfaceCount(void);

/** @brief The radio calibration callback function.
 *
 * The Voltage Controlled Oscillator (VCO) can drift with
 * temperature changes. During every call to ::emberTick(), the stack will
 * check to see if the VCO has drifted. If the VCO has drifted, the stack
 * will call ::emberRadioNeedsCalibratingHandler() to inform the application
 * that it should perform calibration of the current channel as soon as
 * possible. Calibration can take up to 150 ms. The default callback function
 * implementation provided here performs the calibration immediately. The
 * application can define its own callback by defining
 * ::EMBER_APPLICATION_HAS_CUSTOM_RADIO_CALIBRATION_CALLBACK in its
 * CONFIGURATION_HEADER. It can then failsafe any critical processes or
 * peripherals before calling ::emberCalibrateCurrentChannel(). The
 * application must call ::emberCalibrateCurrentChannel() in
 * response to this callback to maintain expected radio performance.
 */
void emberRadioNeedsCalibratingHandler(void);

/** @brief Calibrate the current channel. The stack will notify the
 * application of the need for channel calibration via the
 * ::emberRadioNeedsCalibratingHandler() callback function during
 * ::emberTick().  This function should only be called from within the context
 * of the ::emberRadioNeedsCalibratingHandler() callback function. Calibration
 * can take up to 150 ms. Note if this function is called when the radio is
 * off, it will turn the radio on and leave it on.
 */
void sl_mac_calibrate_current_channel(void);

//General connectivity status APIs

/** @brief Set the device uptime to be long or short
 * the uptime is then advertised in the beacons
 */
void emberSetLongUpTime(bool hasLongUpTime);

/** @brief Set the hub connectivity to be true or false
 * the hub connectivity is then advertised in the beacons
 */
void emberSetHubConnectivity(bool connected);

/** @return True if device uptime is long, false otherwise
 */
bool emberIsUpTimeLong(void);

/** @return True if there is hub connectivity, false otherwise
 */
bool emberIsHubConnected(void);

/**
 * @brief Get the current scheduler priorities used for Zigbee
 * multiprotocol operations.
 * @param priorities A pointer to a structure to store the current priorities
 * in. This must not be NULL.
 * @return ::EMBER_SUCCESS if called in a multiprotocol app and
 * ::EMBER_INVALID_CALL otherwise.
 *
 * For more details on what these priorities are and how to use them see UG305,
 * the Dynamic Multiprotocol User Guide.
 */
EmberStatus emberRadioGetSchedulerPriorities(EmberMultiprotocolPriorities *priorities);

/**
 * @brief Set the current scheduler priorities used for Zigbee
 * multiprotocol operations.
 * @param priorities A pointer to a structure of the new priorities to use. This
 * must not be NULL.
 * @return ::EMBER_SUCCESS if called in a multiprotocol app and
 * ::EMBER_INVALID_CALL otherwise.
 *
 * For more details on what these priorities are and how to use them see UG305,
 * the Dynamic Multiprotocol User Guide.
 */
EmberStatus emberRadioSetSchedulerPriorities(const EmberMultiprotocolPriorities *priorities);

/**
 * @brief Set the multiprotocol slip time.
 * @param slipTime The value to set the slip time to for each transmit.
 * @return ::EMBER_SUCCESS if called in a multiprotocol app and
 * ::EMBER_INVALID_CALL otherwise.
 *
 * Use this to update the slip time passed to the radio scheduler for each
 * transmit from the PHY layer. The smaller this is set the lower the potential
 * multi-protocol induced latency is, but it makes packets more likely to fail
 * to be transmitted. For more details on slip time see UG305.
 */
EmberStatus emberRadioSetSchedulerSliptime(uint32_t slipTime);

/**
 * @brief Get the multiprotocol slip time.
 * @param slipTime A pointer to the place to store the current slip time
 * @return ::EMBER_SUCCESS if called in a multiprotocol app and
 * ::EMBER_INVALID_CALL otherwise.
 *
 * Use this to get the current value of the multiprotocol slip time used for
 * each transmit at the PHY layer. For more details on slip time see UG305.
 */
EmberStatus emberRadioGetSchedulerSliptime(uint32_t *slipTime);
//@}//END Radio-specific functions

/** @brief Get current beacon classification parameter settings.
 */
EmberStatus emberGetBeaconClassificationParams(EmberBeaconClassificationParams* param);

/** @brief Set current beacon classification parameters.
 */
EmberStatus emberSetBeaconClassificationParams(EmberBeaconClassificationParams* param);

/**
 * @brief Set the route record policy.
 * @param slipTime A pointer to the place to store the current slip time
 * @return ::EMBER_SUCCESS if route record policy was sucessfully set
 * ::EMBER_BAD_ARGUMENT otherwise.
 *
 * Use this to set the policy for senidng a route record message to a concentrator
 * Note that policies other than the default ROUTE_RECORD_POLICY_ACK_BY_SOURCE_ROUTED_MESSAGE
 * may result in behaviour not compliant with the Zigbee specification
 */
EmberStatus slx_zigbee_routing_set_route_record_policy(sl_zigbee_route_record_policy_type_t policy);

/**
 * @brief Get the route record policy.
 * @return The route record policy
 *
 * Use this to get the current value of the route record policy
 */
uint8_t slx_zigbee_routing_get_route_record_policy(void);

/**
 * @brief Get the total count of the tokens configured.
 * @return The token count.
 *
 * Use this to get the number of tokens configured in a node.
 */
uint8_t emberGetTokenCount(void);

/**
 * @brief Get information of a token by providing the index.
 * @param index An index to configured token array that ranges from 0 to emberGetTokenCount() - 1;
 * @param tokenInfo A pointer to hold the information in a structure provided by the caller.
 * @return Status of the call, EMBER_SUCCESS upon success or EMBER_INDEX_OUT_OF_RANGE for bad index.
 *
 */
EmberStatus emberGetTokenInfo(uint8_t index,
                              EmberTokenInfo *tokenInfo);
/**
 * @brief Get token data by providing the token key and index for the indexed token.
 * @param token A valid token key, which may be obtained using emberGetTokenInfo.
 * @param index An index in case the token is an indexed token, if token is indexed can be obtained
 *              from emberGetTokenInfo.
 * @param tokenData A pointer pointing to memory storage information, must be allocated and provided by the caller.
 * @return Status of the call, EMBER_SUCCESS upon success or EMBER_ERR_FATAL for errors.
 *
 */
EmberStatus emberGetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData);
/**
 * @brief Set token data by providing the token key, index for an indexed token and token data.
 * @param token A valid token key, which may be obtained using emberGetTokenInfo.
 * @param index An index in case the token is an indexed token, if token is indexed can be obtained
 *              from emberGetTokenInfo.
 * @param tokenData A pointer pointing to memory storage holding the token data provided by the caller.
 * @return Status of the call, EMBER_SUCCESS upon success or EMBER_ERR_FATAL for errors.
 *
 */
EmberStatus emberSetTokenData(uint32_t token,
                              uint32_t index,
                              EmberTokenData *tokenData);

/** @} END addtogroup */

#endif // SILABS_STACK_INFO_H
