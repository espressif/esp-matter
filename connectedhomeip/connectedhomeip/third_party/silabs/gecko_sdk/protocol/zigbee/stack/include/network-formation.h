/***************************************************************************//**
 * @file
 * @brief See @ref network_formation for documentation.
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

#ifndef SILABS_NETWORK_INFORMATION_H
#define SILABS_NETWORK_INFORMATION_H

/**
 * @addtogroup network_formation
 * @brief EmberZNet API for finding, forming, joining, and leaving
 * Zigbee networks.
 *
 * See network-formation.h for source code.
 * @{
 */

/** @brief Initialize the radio and the EmberZNet stack.
 *
 * Device configuration functions must be called before ::emberInit()
 * is called.
 *
 * @note The application must check the return value of this function. If the
 * initialization fails, normal messaging functions will not be available.
 * Some failure modes are not fatal, but the application must follow certain
 * procedures to permit recovery.
 * Ignoring the return code will result in unpredictable radio and API behavior.
 * (In particular, problems with association will occur.)
 *
 * @return An ::EmberStatus value indicating successful initialization or the
 * reason for failure.
 */
EmberStatus emberInit(void);

/** @brief A periodic tick routine that should be called:
 * - in the application's main event loop,
 * - as soon as possible after any radio interrupts, and
 * - after ::emberInit().
 */
void emberTick(void);

/** @brief Resume network operation after a reboot.
 *
 *   Call this function on boot prior to **any**
 *   network operations. It will initialize the networking system
 *   and attempt to resume the previous network identity and configuration.
 *   If the node was not previously joined, this routine should still be called.
 *
 *   If the node was previously joined to a network, it will retain its original
 *   type (e.g., coordinator, router, end device, and so on.)
 *
 *   ::EMBER_NOT_JOINED is returned
 *   if the node is not part of a network.
 *
 *.  This function has encapsulated the old behavior of emberNetworkInitExtended().
 *
 * @param networkInitStruct  Defines whether an orphan scan/rejoin request/or neither
 *   is performed during network initialization.
 *
 * @return An ::EmberStatus value that indicates one of the following:
 * - successful initialization,
 * - ::EMBER_NOT_JOINED if the node is not part of a network, or
 * - the reason for failure.
 */
EmberStatus emberNetworkInit(EmberNetworkInitStruct* networkInitStruct);

/** @brief Form a new network by becoming the coordinator.
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network.  This also
 *   applies when a device leaves a network and wants to form another one.

 * @param parameters  Specification of the new network.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 * of the new network, or the reason that the network formation failed.
 */
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters);

/** @brief Form a new Sleepy-to-Sleepy network.
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network.  This also
 *   applies when a device leaves a network and wants to form another one.

 * @param parameters  Specification of the new network.
 * @param initiator  Indicates if the device is a initiator or a joiner.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 * of the new network, or the reason that the network formation failed.
 */
EmberStatus emberSleepyToSleepyNetworkStart(EmberNetworkParameters *parameters, bool initiator);

/** @brief Tell the stack to allow other nodes to join the network
 * with this node as their parent. Joining is initially disabled by default.
 * This function may only be called after the node is part of a network
 * and the stack is up.
 *
 * @param duration  A value of 0x00 disables joining. A value of 0xFF
 *  enables joining. Any other value enables joining for that number of
 *  seconds.
 */
EmberStatus emberPermitJoining(uint8_t duration);

/** @brief Cause the stack to associate with the network using the
 * specified network parameters. It can take several seconds for the stack to
 * associate with the local network. Do not send messages until a call to the
 * ::emberStackStatusHandler() callback informs you that the stack is up.
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network.  This also
 *   applies when a device leaves a network and wants to join another one.
 *
 * @param nodeType    Specification of the role that this node will have in
 *   the network. This role must not be ::EMBER_COORDINATOR. To be a coordinator,
 *   call ::emberFormNetwork().
 *
 * @param parameters  Specification of the network with which the node
 *   should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 * - that the association process began successfully, or
 * - the reason for failure.
 */
EmberStatus emberJoinNetwork(EmberNodeType nodeType,
                             EmberNetworkParameters *parameters);

/** @brief Cause the stack to associate with the network using the
 * specified network parameters in the beacon parameter. It can take several
 * seconds for the stack to associate with the local network. Do not send
 * messages until a call to the ::emberStackStatusHandler() callback informs you
 * that the stack is up. This function, unlike ::emberJoinNetwork(), does not
 * issue an active scan before joining. Instead, this function will cause the
 * local node to issue a MAC Association Request directly to the specified
 * target node. It is assumed that the beacon parameter is an artifact after
 * issuing an active scan (see ::emberGetBestBeacon() and ::emberGetNextBeacon()
 * for more).
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network. This also
 *   applies when a device leaves a network and wants to join another one.
 *
 * @param localNodeType  Specification of the role that this node will have in
 *   the network. This role must not be ::EMBER_COORDINATOR. To be a
 *   coordinator, call ::emberFormNetwork().
 *
 * @param beacon  Specification of the network with which the node should
 *   associate.
 *
 * @param radioTxPower  The radio transmit power to use, specified in dBm.
 *
 * @param clearBeaconsAfterNetworkUp  If true, clear beacons in cache upon join
 *   success. Do nothing for join failure.
 *
 * @return An ::EmberStatus value that indicates either:
 * - that the association process began successfully, or
 * - the reason for failure.
 */

EmberStatus emberJoinNetworkDirectly(EmberNodeType localNodeType,
                                     EmberBeaconData* beacon,
                                     int8_t radioTxPower,
                                     bool clearBeaconsAfterNetworkUp);

/** @brief Cause the stack to leave the current network.
 * This generates a call to the ::emberStackStatusHandler() callback to indicate
 * that the network is down. The radio will not be used until after a later call
 * to ::emberFormNetwork() or ::emberJoinNetwork().
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 * A status of ::EMBER_INVALID_CALL indicates that the node is either not
 * joined to a network or is already in the process of leaving.
 */
EmberStatus emberLeaveNetwork(void);

/** @brief Send a Zigbee NWK leave command to the specified destination.
 *
 * @param destination is the node ID of the device that is being told to
 *   leave.
 *
 * @param flags is a bitmask indicating additional considerations for
 *   the leave request.  See the ::EmberLeaveRequestFlags enumeration for more
 *   information.  Multiple bits may be set.
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 * A status of ::EMBER_INVALID_CALL indicates that the node not currently
 * joined to the network, or the destination is the local node.  To tell
 * the local device to leave, use the ::emberLeaveNetwork() API.
 */
EmberStatus emberSendZigbeeLeave(EmberNodeId destination,
                                 EmberLeaveRequestFlags flags);

/** @brief Call this function when contact with the
 * network has been lost. The most common use case is when an end device
 * can no longer communicate with its parent and wishes to find a new one.
 * Another case is when a device has missed a Network Key update and no
 * longer has the current Network Key.
 *
 * Note that a call to ::emberPollForData() on an end device that has lost
 * contact with its parent will automatically call ::emberRejoinNetwork(true).
 *
 * The stack will call ::emberStackStatusHandler() to indicate that the network
 * is down, then try to re-establish contact with the network by performing
 * an active scan, choosing a network with matching extended pan ID, and
 * sending a Zigbee network rejoin request.  A second call to the
 * ::emberStackStatusHandler() callback indicates either the success
 * or the failure of the attempt.  The process takes
 * approximately 150 milliseconds per channel to complete.
 *
 * This call replaces the ::emberMobileNodeHasMoved() API from EmberZNet 2.x,
 * which used MAC association and consequently took half a second longer
 * to complete.
 *
 * @param haveCurrentNetworkKey  This parameter determines whether the request
 * to rejoin the Network is sent encrypted (true) or unencrypted (false). The
 * application should first try to use encryption. If that fails,
 * the application should call this function again and use no encryption.
 * If the unencrypted rejoin is successful then device will be in the joined but
 * unauthenticated state. The Trust Center will be notified of the rejoin and
 * send an updated Network encrypted using the device's Link Key. Sending the
 * rejoin unencrypted is only supported on networks using Standard Security
 * with link keys (i.e., ZigBee 2006 networks do not support it).
 *
 * @param channelMask A mask indicating the channels to be scanned.
 * See ::emberStartScan() for format details.
 *
 * @param reason An enumeration indicating why the rejoin occurred.
 * The stack will set the reason based on the ::EmberRejoinReason.
 * An application should use one of the APP_EVENT rejoin reasons.  The stack
 * will never use these.  Only if the function return code is EMBER_SUCCESS
 * will the rejoin reason be set.
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 */
EmberStatus emberFindAndRejoinNetworkWithReason(bool haveCurrentNetworkKey,
                                                uint32_t channelMask,
                                                EmberRejoinReason reason);

/** @brief This call is the same emberFindAndRejoinNetworkWithReason(). However, the
 *  reason is assumed to be ::EMBER_REJOIN_REASON_APP_EVENT_1.
 */
EmberStatus emberFindAndRejoinNetwork(bool haveCurrentNetworkKey,
                                      uint32_t channelMask);

/** @brief Attempt to rejoin the network with a different device type.
 * @param haveCurrentNetworkKey This parameter determines whether the request
 * to rejoin the Network is sent encrypted (true) or unencrypted (false). The
 * application should first try to use encryption. If that fails,
 * the application should call this function again and use no encryption.
 * If the unencrypted rejoin is successful then device will be in the joined but
 * unauthenticated state. The Trust Center will be notified of the rejoin and
 * send an updated Network encrypted using the device's Link Key. Sending the
 * rejoin unencrypted is only supported on networks using Standard Security
 * with link keys (i.e., ZigBee 2006 networks do not support it).
 *
 * @param channelMask A mask indicating the channels to be scanned.
 * See ::emberStartScan() for format details.
 *
 * @param nodeType An enumeration indicating the device type to rejoin as. The
 * stack only accepts ::EMBER_END_DEVICE and ::EMBER_SLEEPY_END_DEVICE.
 */
EmberStatus emberFindAndRejoinNetworkWithNodeType(bool haveCurrentNetworkKey,
                                                  uint32_t channelMask,
                                                  EmberNodeType nodeType);

/** @brief Return the enumeration for why a previous rejoin.
 */
EmberRejoinReason emberGetLastRejoinReason(void);

/** @brief A convenience function which calls ::emberFindAndRejoinNetwork()
 * with a channel mask value for scanning only the current channel.
 * Included for back-compatibility.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberRejoinNetwork(bool haveCurrentNetworkKey);

#else
#define emberRejoinNetwork(haveKey) emberFindAndRejoinNetwork((haveKey), 0)
#endif

/** @brief Start a scan. ::SL_STATUS_OK signals that
 * the scan successfully started. Note that although a scan can be initiated
 * while the node is currently joined to a network, the node will generally
 * be unable to communicate with its PAN during the scan period.
 * Take care when performing scans of any significant duration while
 * presently joined to an existing PAN.
 *
 * Possible error responses and their meanings:
 * - ::SL_STATUS_MAC_SCANNING, already scanning.
 * - ::SL_STATUS_BAD_SCAN_DURATION, a duration value that is
 *   not 0..14 inclusive is set.
 * - ::SL_STATUS_MAC_INCORRECT_SCAN_TYPE, an undefined
 *   scanning type is requested;
 * - ::SL_STATUS_INVALID_CHANNEL_MASK, the channel mask did not specify any
 *   valid channels on the current platform.
 *
 * @param scanType     Indicates the type of scan to be performed.
 *  Possible values:  ::EMBER_ENERGY_SCAN, ::EMBER_ACTIVE_SCAN.
 *
 * @param channelMask  Bits set as 1 indicate that this particular channel
 * should be scanned. Bits set to 0 indicate that this particular channel
 * should not be scanned. For example, a channelMask value of 0x00000001
 * indicates that only channel 0 should be scanned. Valid channels range
 * from 11 to 26 inclusive. This translates to a channel mask value of 0x07
 * FF F8 00. As a convenience, a channelMask of 0 is reinterpreted as the
 * mask for the current channel.
 *
 * @param duration     Sets the exponent of the number of scan periods,
 * where a scan period is 960 symbols, and a symbol is 16 microseconds.
 * The scan will occur for ((2^duration) + 1) scan periods.  The value
 * of duration must be less than 15.  The time corresponding to the first
 * few values is as follows: 0 = 31 msec, 1 = 46 msec, 2 = 77 msec,
 * 3 = 138 msec, 4 = 261 msec, 5 = 507 msec, 6 = 998 msec.
 */
sl_status_t emberStartScan(EmberNetworkScanType scanType, uint32_t channelMask, uint8_t duration);

/** @brief Handle the reception of a beacon.
 *
 * NOTE: SoC only
 */
typedef bool (*EmberNetworkFoundCallback)(EmberZigbeeNetwork *network, uint8_t rssi, int8_t lqi, uint16_t senderNodeId, uint8_t parentPriority);

/** @brief Handle the conclusion of an active
 * or energy scan.
 *
 * NOTE: SoC only
 */
typedef bool (*EmberScanCompleteCallback)(uint8_t channel, EmberStatus status);

#ifndef EZSP_HOST

/** @brief Kick off a procedure to scan for beacons, filter results, and
 * provide a response ZCL frame including the best eligible parents. This
 * procedure uses the emberStartScan() API.
 *
 * @param useStandardBeacons If true, the surveyBeacon procedure will use
 * standard beacons. If false, the surveyBeacon procedure will use enhanced
 * beacons.
 *
 * @param networkFoundCallback The function pointer that is called after
 * the reception of a beacon.
 *
 * @param scanCompleteCallback The function pointer that is called after
 * the beacon collection phase of the surveyBeacon procedure is over.
 *
 * NOTE: SoC only
 */
EmberStatus emberSurveyBeacons(bool useStandardBeacons,
                               EmberNetworkFoundCallback networkFoundCallback,
                               EmberScanCompleteCallback scanCompleteCallback);
#endif

/** @brief Kick off a procedure to find an unused panId on a low-activity
 * channel that is included in the passed-in channel mask. This procedure
 * uses the emberStartScan() API.
 *
 * @param channelMask The set of channels that will be scanned to find an
 * available panId.
 *
 * @param duration The length of time that will be spent scanning for an
 * available panId.
 */
EmberStatus emberFindUnusedPanId(uint32_t channelMask, uint8_t duration);

/** @brief Terminate a scan in progress.
 *
 * @return Returns ::EMBER_SUCCESS if successful.
 */
EmberStatus emberStopScan(void);

/** @brief Indicate the status of the current scan. When the scan has
 * completed, the stack will call this function with status set to
 * ::EMBER_SUCCESS. Prior to the scan completing, the stack may call this
 * function with other status values. Non-EMBER_SUCCESS status values indicate
 * that the scan failed to start successfully on the channel indicated by the
 * channel parameter. The current scan is ongoing until the stack calls this
 * function with status set to ::EMBER_SUCCESS.
 *
 * @param channel  The channel on which the current error occurred.
 * Undefined for the case of ::EMBER_SUCCESS.
 *
 * @param status   The error condition that occurred on the current channel.
 * Value will be ::EMBER_SUCCESS when the scan has completed.
 */
void emberScanCompleteHandler(uint8_t channel, EmberStatus status);

/** @brief Report the maximum RSSI value measured on the channel.
 *
 * @param channel       The 802.15.4 channel number on which the RSSI value.
 *   was measured.
 *
 * @param maxRssiValue  The maximum RSSI value measured (in units of dBm).
 *
 */
void emberEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue);

/** @brief Report that a network was found and gives the network
 * parameters used for deciding which network to join.
 *
 * @param networkFound A pointer to a ::EmberZigbeeNetwork structure
 *   that contains the discovered network and its associated parameters.
 *
 * @param lqi  The link quality indication of the network found.
 *
 * @param rssi  The received signal strength indication of the network found.
 */
void emberNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                              uint8_t lqi,
                              int8_t rssi);

/** @brief Return an unused panID and channel pair
 * found via the find unused panId scan procedure.
 *
 * @param panId The unused panID.
 *
 * @param channel The channel which the unused panId was found on.
 */
void emberUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel);

/** @brief Indicate whether the stack is in the process of performing
 *  a rejoin.
 *
 * @return Returns true if the device is in the process of performing a rejoin.
 *         Returns false otherwise.
 */
bool emberStackIsPerformingRejoin(void);

/** @brief Indicate the reason why the device left the network (if any). This also
 *    will return the device that sent the leave message, if the leave
 *    was due to an over-the-air message.
 *
 *    If returnNodeIdThatSentLeave is a non-NULL pointer, the node ID
 *    of the device that sent the leave message will be written to the value
 *    indicated by the pointer. If the leave was not due to an over-the-air
 *    message (but an internal API call instead), EMBER_UNKNOWN_NODE_ID is
 *    returned.
 *
 *  @return Returns EmberLeaveReason enumeration, or EMBER_LEAVE_REASON_NONE
 *    if the device has not left the network.
 */
EmberLeaveReason emberGetLastLeaveReason(EmberNodeId* returnNodeIdThatSentLeave);

/** @brief Indicate the state of the permit joining in the MAC.
 */
bool emberGetPermitJoining(void);

/** @brief Set the Network Update ID to the desired value. The Network Update ID
 *    value cannot be manually changed after a network is joined, so this
 *    function must be called before calling emberFormNetwork.
 *
 *  @note This Network Update ID should not normally be changed, as it is used
 *    by the stack to track changes in the network.
 *
 *  @return EMBER_SUCCESS if called when not on network, else EMBER_INVALID_CALL.
 */
EmberStatus emberSetNwkUpdateId(uint8_t nwkUpdateId);

/** @brief Set the duration of a beacon jitter, in the units used by the 15.4
 *    scan parameter (((1 << duration) + 1) * 15 ms), when responding to a beacon
 *    request.
 *
 *  @note Because the recommended scan duration of a joining device is 3 for 2.4
 *    GHz devices, any value passed in to this function greater than 3 will be
 *    rejected.
 *
 *  @return EMBER_BAD_ARGUMENT if the argument is invalid, else EMBER_SUCCESS.
 */
EmberStatus emberSetBeaconJitterDuration(uint8_t beaconJitterDuration);

/**
 * @brief Set the policy decision for Trust Center (insecure)
 *   rejoins for devices using the well-known link key. If rejoining using the
 *   well-known key is allowed, it is disabled again after
 *   emAllowTcRejoinsUsingWellKnownKeyTimeoutSec seconds.
 */
void emberSetTcRejoinsUsingWellKnownKeyAllowed(bool allow);

/**
 * @brief Get the policy decision for Trust Center (insecure)
 *   rejoins for devices using the well-known link key.
 */
bool emberTcRejoinsUsingWellKnownKeyAllowed(void);

/*
 * @brief Extend a joiner's timeout to wait for the network key
 * on the joiner default key timeout is 3 sec, and only values greater equal to 3 sec are accepted.
 *
 * @return EMBER_BAD_ARGUMENT if the argument is invalid, else EMBER_SUCCESS.
 */
EmberStatus emberSetupDelayedJoin(uint8_t networkKeyTimeoutS);

/*
 * @brief Return the value of network key timeout in seconds.
 *
 * @return value of network key timeout in seconds
 */
uint8_t emberGetNetworkKeyTimeout(void);

/*
 * brief Reschedule sending link status message, first one being sent immediately.
 */

void emberRescheduleLinkStatusMsg(void);

/** @brief Configure the minimum RSSI for receiving packets.
 *
 * The minimum RSSI is used to classify good/bad parent signal strengths.
 * Any packet in the range of minimum RSSI + 30 db is considered to be "bad".
 * By default, this value is set to -100. Currently, there is no EZSP support.
 * This is left here for backward compatibility.
 * You can achieve the same EZSP applicable setting with emberSetBeaconClassificationParams
 */
void emberSetMinRSSI(int8_t minRSSI);

/** @brief Enable/disable the parent classification algorithm on the stack
 *  side. Parent classification considers whether a received beacon indicates
 *  trust center connectivity and long uptime on the network.
 * This is left here for backward compatibility.
 * You can achieve the same with emberSetBeaconClassificationParams
 *
 */
void emberSetParentClassificationEnabled(bool enabled);

/** @brief Get the enable state the parent classification algorithm.
 * This is left here for backward compatibility.
 * You can achieve the same with emberGetBeaconClassificationParams
 *
 */
bool emberGetParentClassificationEnabled(void);

/** @brief Set the short PAN ID the device will accept in a NLME Network Update command.
 *
 * If a NLME Network Update command is received by the device specifying
 * a short PAN ID that does not match with the given PAN ID, then the NLME Network
 * Update message will be ignored by the device. A value of 0xFFFF indicates
 * that any short PAN ID received in a NLME Network Update command will be accepted
 * which is also the default value set by the stack.
 *
 * @param panId     A pending network update short PAN ID.
 */
void emberSetPendingNetworkUpdatePanId(uint16_t panId);

/*
 * @brief Get the last LQI/RSSI values for neighbors in a specific debug mode.
 *
 *  @param nodeId id of the node for which we are looking for LQI and RSSI
 *
 *  @returns EMBER_SUCESS if the specific defines exist, otherwise returns EMBER_FATAL_ERR
 *  also returns the last LQI and RSSI for the specified node, if they exist
 */
EmberStatus emberGetNeighborLastLqiRssi(EmberNodeId nodeId, uint8_t *lqi, int8_t *rssi);

/*
 * @brief Send an update PAN ID message with the new PanID and can be called
 *  within the context of emberPanIdConflictHandler,( the handler itself is called by the
 *  stack to report the number of conflict reports exceeds
 *  EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD within a period of 1 minute )
 *
 *  @param new PAN ID that we want to advertise
 *
 *  @returns true if sending an update was successful, false otherwise
 */
bool emberSendPanIdUpdate(EmberPanId newPan);

/** @brief Clear all cached beacons that have been collected from an active
   scan. */
void emberClearStoredBeacons(void);

/*
 * @brief Configure the number of beacons to store when issuing active scans
 *  for networks. For classical (Home Automation and Smart Energy) joining, the
 *  most prioritized beacon is chosen to join to. For network steering (Z3.0
 *  joining), all beacons are tried until a successful join.
 *  The existing join and rejoin APIs, such as emberJoinNetwork and the several
 *  emberFindAndRejoinNetwork APIs, set the maximum number of beacons to store
 *  to 1.
 *
 *  @param numBeacons The number of beacons to cache when scanning.
 *
 *  @returns EMBER_BAD_ARGUMENT if numBeacons is greater than
 *   EMBER_MAX_BEACONS_TO_STORE, otherwise EMBER_SUCCESS
 */
/** @brief  */
EmberStatus emberSetNumBeaconsToStore(uint8_t numBeacons);

/*
 * @brief Return the first beacon in the cache. Beacons are stored in cache
 *  after issuing an active scan.
 *
 *  @param beaconIterator The iterator to use when returning the first beacon.
 *   This argument must not be NULL.
 *
 *  @returns EMBER_SUCCESS if first beacon found, EMBER_BAD_ARGUMENT if input
 *   parameters are invalid, EMBER_INVALID_CALL if no beacons stored,
 *   EMBER_ERR_FATAL if no first beacon found.
 */
/** @brief  */
EmberStatus emberGetFirstBeacon(EmberBeaconIterator* beaconIterator);

/*
 * @brief Return the next beacon in the cache. Beacons are stored in cache
 *  after issuing an active scan.
 *
 *  @param beacon The next beacon retrieved. It is assumed that
 *   ::emberGetFirstBeacon has been called first. This argument must not be
 *   NULL.
 *
 *  @returns EMBER_SUCCESS if next beacon found, EMBER_BAD_ARGUMENT if input
 *   parameters are invalid, EMBER_ERR_FATAL if no next beacon found.
 */
/** @brief  */
EmberStatus emberGetNextBeacon(EmberBeaconData* beacon);

/** @brief Return the number of cached beacons that have been collected from a
   scan. */
uint8_t emberGetNumStoredBeacons(void);

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file network-formation.h is new and is described in @ref network_formation.
 * <ul>
 * <li> <b>New items</b>
 *   - emberFindAndRejoinNetwork() - replaced emberMobileNodeHasMoved()
 *   - emberRejoinNetwork()
 *   .
 * <li> <b>Items moved from ember.h</b>
 *   - emberEnergyScanResultHandler()
 *   - emberFormNetwork()
 *   - emberInit()
 *   - emberJoinNetwork()
 *   - emberLeaveNetwork()
 *   - emberNetworkFoundHandler()
 *   - emberNetworkInit()
 *   - emberPermitJoining()
 *   - emberScanCompleteHandler()
 *   - emberStartScan()
 *   - emberStopScan()
 *   - emberTick()
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_NETWORK_INFORMATION_H
