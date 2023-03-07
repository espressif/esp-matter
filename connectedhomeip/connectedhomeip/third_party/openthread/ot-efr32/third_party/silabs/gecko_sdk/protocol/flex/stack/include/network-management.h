/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/**
 * @addtogroup network_management
 * @brief Connect API for finding, forming, joining, and leaving Connect
 * networks.
 *
 * See network-management.h for source code.
 * @{
 */

#ifndef __NETWORK_FORMATION_H__
#define __NETWORK_FORMATION_H__

/** @brief The maximum length in bytes of the application beacon payload.
 */
#define EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH     16

/** @brief The maximum length in bytes of the beacon fields (superframe, GTS,
 *   pending address) as per 802.15.4 specs.
 */
#define EMBER_MAC_MAX_BEACON_FIELDS_LENGTH          84

/** @brief A special timeout value that disables aging of the child table.
 */
#define EMBER_CHILD_TABLE_AGING_DISABLED            0x20C400

/** @brief The maximum timeout in seconds after which a stale entry may be
 *   removed from the child table.
 */
#define EMBER_CHILD_TABLE_MAX_TIMEOUT_S   (EMBER_CHILD_TABLE_AGING_DISABLED - 1)

/** @name Handlers
 * @{
 */

/** @brief Invoked if a beacon is received during the scanning procedure
 *  if the handler was initiated by the application with the
 *  ::emberStartActiveScan() stack APIs.
 *
 * @param[in] panId   The source pan ID of the received beacon.
 *
 * @param[in] source  The source node address of the received beacon.
 *
 * @param[in] rssi  The RSSI the beacon was received with.
 *
 * @param[in] permitJoining   The permit joining flag in the received beacon.
 *
 * @param[in] beaconFieldsLength  The length in bytes of the beacon fields
 * defined as per 802.15.4 specs (superframe, GTS fields and pending address
 * fields) of the received beacon.
 *
 * @param[in] beaconFields   A pointer to the beacon fields defined as per
 * 802.15.4 specs (superframe, GTS fields and pending address fields) of the
 * received beacon.
 *
 * @param[in] beaconPayloadLength  The length in bytes of the application beacon
 *  payload of the received beacon.
 *
 * @param[in] beaconPayload   A pointer to the application beacon payload of the
 *  received beacon.
 */
void emberIncomingBeaconHandler(EmberPanId panId,
                                EmberMacAddress *source,
                                int8_t rssi,
                                bool permitJoining,
                                uint8_t beaconFieldsLength,
                                uint8_t *beaconFields,
                                uint8_t beaconPayloadLength,
                                uint8_t *beaconPayload);

/** @brief Invoked after the application calls the ::emberStartActiveScan()
 *  stack API to inform the application that the scanning procedure is complete.
 */
void emberActiveScanCompleteHandler(void);

/** @brief Invoked after the application calls the ::emberStartEnergyScan()
 *  stack API to inform the application that the energy scan procedure
 *  is complete and to provide statistics.
 *
 *  @param[in] mean        The average energy detected in dBm.
 *  @param[in] min         The minimum energy detected in dBm.
 *  @param[in] max         The maximum energy detected in dBm.
 *  @param[in] variance    The variance of the energy detected in dBm.
 */
void emberEnergyScanCompleteHandler(int8_t mean,
                                    int8_t min,
                                    int8_t max,
                                    uint16_t variance);

/**
 * @}
 */

/** @brief Initialize the radio and the Ember stack.
 *
 * Device configuration functions must be called before ::emberInit()
 * is called.
 *
 * @note The application must check the return value of this function. If the
 * initialization fails, normal messaging functions are not available.
 * Some failure modes are not fatal, but the application must follow certain
 * procedures to permit recovery.
 * Ignoring the return code results in unpredictable radio and API behavior.
 * (In particular, problems with association will occur.)
 *
 * @return An ::EmberStatus value indicating successful initialization or the
 *   reason for failure.
 */
EmberStatus emberInit(void);

/** @brief A periodic tick routine that should be called in the main loop
 * in the application.
 */
void emberTick(void);

/** @brief Resume the network operation after a reboot.
 *
 * This API must be called on boot prior to ANY network operations.
 * It initializes the networking system and attempts to resume the
 * previous network identity and configuration.  If the node was not previously
 * joined, this routine should still be called.
 *
 * If the node was previously joined to a network, it will retain its original
 * type (e.g., coordinator, router, end device, and so on.)
 *
 * ::EMBER_NOT_JOINED is returned if the node is not part of a network.
 *
 * @return An ::EmberStatus value that indicates one of the following:
 *   - successful initialization,
 *   - ::EMBER_NOT_JOINED if the node is not part of a network, or
 *   - the reason for failure.
 */
EmberStatus emberNetworkInit(void);

/** @brief Start an active scan. ::EMBER_SUCCESS signals that
 * the scan successfully started. Upon receiving a beacon,
 * the ::emberIncomingBeaconHandler() stack handler is called. At
 * the end of the scanning procedure, the ::emberActiveScanCompleteHandler()
 * stack handler is called. Note that, while a scan can be
 * initiated when the node is currently joined to a network, the node
 * will generally be unable to communicate with its PAN during
 * the scan period. In particular, time-sensitive network operations
 * might be affected because a scan operation will
 * prevent any network operation for the duration of the scan.
 *
 * Possible error responses and their meanings:
 * - ::EMBER_INVALID_CALL, the node is currently frequency hopping.
 * - ::EMBER_MAC_SCANNING, indicates an ongoing scan.
 * - ::EMBER_PHY_INVALID_CHANNEL, the specified channel is not a valid channel
 *   on the current platform.
 *
 * @param[in] channel  The channel to scan.
 */
EmberStatus emberStartActiveScan(uint16_t channel);

/** @brief Set the time in milliseconds the node will spend listening for
 * incoming beacons during an active scan. The default value is set based on the
 * symbol time of the current PHY configuration according to the 802.15.4 specs.
 *
 * @param[in] durationMs  The active scan duration in milliseconds. A value of
 * 0xFFFF restores the default value.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the active scan duration
 * was successfully set, otherwise an ::EmberStatus value indicating the reason
 * of failure.
 */
EmberStatus emberSetActiveScanDuration(uint16_t durationMs);

/** @brief Get the current active scan duration in milliseconds.
 *
 * @return a 16-bit integer indicating the current duration in millisecond of
 * the active scan.
 */
uint16_t emberGetActiveScanDuration(void);

/** @brief Start an energy scan. ::EMBER_SUCCESS signals
 * that the scan successfully started. At the end of the scanning
 * procedure, the ::emberEnergyScanCompleteHandler() stack handler
 * is called. Note that, while a scan can be initiated
 * when the node is currently joined to a network, the node is generally
 * unable to communicate with its PAN during the scan period. In particular,
 * time-sensitive network operations might be affected because a scan operation
 * will prevent any network operation for the duration of the scan.
 *
 * Possible error responses and their meanings:
 * - ::EMBER_INVALID_CALL, the node is currently frequency hopping.
 * - ::EMBER_BAD_ARGUMENT, the samples parameter is invalid.
 * - ::EMBER_MAC_SCANNING, indicates an ongoing scan.
 * - ::EMBER_PHY_INVALID_CHANNEL, the specified channel is not a valid channel
 *   on the current platform.
 * - ::EMBER_NO_BUFFERS, the stack doesn't have enough memory at the moment to
 *   perform the requested scan.
 *
 * @param[in] channel  The channel to scan.
 *
 * @param[in] samples   The number of energy samples to be produced. Each sample
 *   is performed averaging the detected energy over X symbols time, whereas X
 *   depends on the selected PHY configuration and set by default to 8. The
 *   symbol time duration also depends on the selected PHY configuration.
 */
EmberStatus emberStartEnergyScan(uint16_t channel, uint8_t samples);

/** @brief Allow the application to set the application portion of the
 * beacon payload. It's by default set to the empty string.
 *
 * @param[in] payloadLength  The length in bytes of the application beacon
 *   payload to be set. This value can not exceed
 *   ::EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH.
 *
 * @param[out] payload   A pointer to the application beacon payload to be set.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the application beacon
 *   payload was successfully set, otherwise an ::EmberStatus value indicating
 *   the reason of failure.
 */
EmberStatus emberSetApplicationBeaconPayload(uint8_t payloadLength,
                                             uint8_t *payload);

/** @brief Form a new network by becoming the coordinator. This API requires
 * the parent-support library to be present.
 *
 * @param[in] parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network to be formed.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 *   of the new network or an ::EmberStatus value indicating the reason of
 *   failure.
 *
 * @ingroup parent_support
 */
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters);

/** @brief Cause the stack to associate with the network using the
 * specified network parameters. It can take several seconds for the stack to
 * associate with the local network. Do not send messages until a call to the
 * ::emberStackStatusHandler() callback informs you that the stack is up.
 * Notice that forming a network causes the node's security frame counter to
 * be reset.
 *
 * @param[in] nodeType    Specification of the role that this node will have in
 *   the network. This role can be ::EMBER_STAR_RANGE_EXTENDER,
 *   ::EMBER_STAR_END_DEVICE, ::EMBER_STAR_SLEEPY_END_DEVICE,
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE. If the node is
 *   frequency hopping, the role can not be ::EMBER_STAR_RANGE_EXTENDER.
 *
 * @param[in] nodeId    An ::EmberNodeId value indicating the short ID the node
 *   intends to use for addressing purposes. If this value is
 *   ::EMBER_NULL_NODE_ID, the network coordinator will allocate a new short
 *   address. Addresses should be allocated by the coordinator unless there is a
 *   specific need to join a network with a specific ID. If a specific ID is
 *   used, uniqueness should be guaranteed across the entire network by the
 *   application, via some out of band means. Notice that nodes of
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE require this
 *   parameter to be set to ::EMBER_NULL_NODE_ID.
 *
 * @param[in] parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network with which the node should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the association process began successfully or
 *   - the reason for failure.
 */
EmberStatus emberJoinNetworkExtended(EmberNodeType nodeType,
                                     EmberNodeId nodeId,
                                     EmberNetworkParameters *parameters);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Cause the stack to associate with the network using the
 * specified network parameters. The network ID is assigned by the network
 * coordinator.
 * It can take several seconds for the stack to associate with the local
 * network. Do not send messages until a call to the ::emberStackStatusHandler()
 * callback informs you that the stack is up.
 * Notice that joining a network causes the node's security frame counter to
 * be reset.
 *
 * @param[in] nodeType    Specification of the role that this node will have in
 *   the network. This role can be ::EMBER_STAR_RANGE_EXTENDER,
 *   ::EMBER_STAR_END_DEVICE, ::EMBER_STAR_SLEEPY_END_DEVICE,
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE. If the node is
 *   frequency hopping, the role can not be ::EMBER_STAR_RANGE_EXTENDER.
 *
 * @param[in] parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network with which the node should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the association process began successfully or
 *   - the reason for failure.
 */
EmberStatus emberJoinNetwork(EmberNodeType nodeType,
                             EmberNetworkParameters *parameters);
#else
#define emberJoinNetwork(nodeType, parameters) \
  (emberJoinNetworkExtended((nodeType), EMBER_NULL_NODE_ID, (parameters)))
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Tell the stack to allow other nodes to join the network
 * with this node as their parent.  Joining is initially disabled by default.
 * This function may only be called after the node is part of a network
 * and the stack is up.
 *
 * @param[in] duration  A value of 0x00 disables joining. A value of 0xFF
 *   enables joining indefinitely.  Any other value enables joining for that
 *   number of seconds.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the permit joining
 *   was successfully set, otherwise an ::EmberStatus value indicating the
 *   reason of failure.
 */
EmberStatus emberPermitJoining(uint8_t duration);

/** @brief Cause the stack to go up with the passed network parameters without
 * performing any over-the-air message exchange.
 * Notice that commissioning a network causes the node's security frame counter
 * to be reset.
 *
 * @param[in] nodeType    Specifies the role that this node will have in
 *   the network. The only device types allowed in the commissioning API are
 *   ::EMBER_DIRECT_DEVICE, ::EMBER_MAC_MODE_DEVICE and
 *   ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] nodeId  An ::EmberNodeId value that specifies the short ID the
 * node will have. The passed node ID must be a valid short address
 * (any value other than ::EMBER_NULL_NODE_ID or ::EMBER_BROADCAST_ADDRESS).
 * A value of ::EMBER_USE_LONG_ADDRESS is allowed only when commissioning the
 * node as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE and will
 * cause the node to send MAC level control messages such as data polls or
 * beacons using long source addressing.
 *
 * @param[in] parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network the node should participate in.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the node successfully commissioned the passed network parameters
 *   - the reason for failure.
 */
EmberStatus emberJoinCommissioned(EmberNodeType nodeType,
                                  EmberNodeId nodeId,
                                  EmberNetworkParameters *parameters);

/** @brief The maximum length in bytes of the join payload.
 */
#define EMBER_MAX_SELECTIVE_JOIN_PAYLOAD_LENGTH           50

/** @brief When invoked at a ::EMBER_STAR_COORDINATOR or a
 * ::EMBER_STAR_RANGE_EXTENDER, it causes the stack to only accept subsequent
 * joining nodes with matching joining payload. When invoked at a node that has
 * not yet joined a network, it sets the joining payload that will be included
 * in the joining process. Notice, the join payload is included in a
 * non-standard 802.15.4 command, therefore this feature is not available for
 * nodes operating as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] payloadLength   The length in bytes of the passed joining payload.
 *   This can not exceed ::EMBER_MAX_SELECTIVE_JOIN_PAYLOAD_LENGTH.
 *
 * @param[in] payload   A pointer to the payload to be set.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the node successfully set the join payload.
 *   - the reason for failure.
 */
EmberStatus emberSetSelectiveJoinPayload(uint8_t payloadLength,
                                         uint8_t *payload);

/** @brief Clear the join payload previously set with the
 * ::emberSetSelectiveJoinPayload() API. When invoked at an
 * ::EMBER_STAR_COORDINATOR or an ::EMBER_STAR_RANGE_EXTENDER it causes the
 * stack to accept joining nodes with any join payload pattern. When invoked at
 * a node that has not yet joined a network, it clears the join payload.
 * Subsequent joining attempts will not include any join payload in the
 * over-the-air joining handshake.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the node successfully cleared the join payload.
 *   - the reason for failure.
 */
EmberStatus emberClearSelectiveJoinPayload(void);

/** @brief The maximum number of entries the auxiliary address filtering table
 * can hold.
 */
#define EMBER_MAX_AUXILIARY_ADDRESS_FILTERING_TABLE_LENGTH   2

/** @brief Set an entry in the auxiliary address filtering table at a given
 * address. Nodes of ::EMBER_DIRECT_DEVICE device type can receive incoming
 * messages destined to any of the node IDs in the auxiliary address filtering
 * table (while also receiving  messages destined to actual node ID). If the
 * passed node ID is ::EMBER_NULL_NODE_ID, the entry is cleared.
 *
 * @param[in] nodeId    An ::EmberNodeId value to be added to the auxiliary
 * address filtering table at the passed entry index.
 *
 * @param[in] entryIndex  The index of the auxiliary address filtering table
 * entry to be set.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if auxiliary address
 * filtering table entry was successfully set. An ::EmberStatus value of
 * ::EMBER_INVALID_CALL if the passed entry index is invalid.
 */
EmberStatus emberSetAuxiliaryAddressFilteringEntry(EmberNodeId nodeId,
                                                   uint8_t entryIndex);

/** @brief Retrieve the content of the auxiliary address filtering
 *  table at a given address. See ::emberSetAuxiliaryAddressFilteringEntry()
 *  for details.
 *
 * @param[in] entryIndex  The index in the auxiliary address filtering table
 * entry to be retrieved.
 *
 * @return An ::EmberNodeId value of ::EMBER_NULL_NODE_ID if the passed entry
 *   index is invalid or if the passed entry index refers to an unused entry.
 *   Otherwise, it returns the content of the auxiliary address filtering table
 *   entry corresponding to the passed entry index.
 */
EmberNodeId emberGetAuxiliaryAddressFilteringEntry(uint8_t entryIndex);

/** @brief Forget the current network and reverts to a network status of
 * ::EMBER_NO_NETWORK.
 */
void emberResetNetworkState(void);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Form a new network as an ::EMBER_MAC_MODE_DEVICE by becoming the
 *  coordinator. This API should be used to form a compliant 802.15.4 PAN and
 *  to inter-operate with other 802.15.4 devices.
 *  Notice that forming a network causes the node's security frame counter to
 *  be reset.
 *
 * @param[in] parameters  An ::EmberNetworkParameters value that specifies the
 * network parameters of the network to be formed.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 *   of the new network or the reason that the network formation failed.
 */
EmberStatus emberMacFormNetwork(EmberNetworkParameters *parameters);
#else
#define emberMacFormNetwork(parameters)             \
  (emberJoinCommissioned(EMBER_MAC_MODE_DEVICE,     \
                         EMBER_COORDINATOR_ADDRESS, \
                         (parameters)))
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Configure a ::EMBER_MAC_MODE_DEVICE node to be a PAN coordinator.
 * Note, this only applies to nodes that have been commissioned as
 * ::EMBER_MAC_MODE_DEVICE.
 *
 * @param[in] isCoordinator  If set to true, the node will identify itself as
 * the PAN coordinator.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the coordinator flag was
 *   successfully set, or another ::EmberStatus value indicating the reason of
 *   failure.
 */
EmberStatus emberMacSetPanCoordinator(bool isCoordinator);

/** @brief Return an ::EmberChildFlags bitmask indicating the child flags
 * of the child corresponding to the passed MAC address.
 *
 * @note Deprecated, use emberGetChildInfo instead
 *
 * @param[in] address  A pointer to an ::EmberMacAddress that specifies the MAC
 * address of the child.
 *
 * @param[out] flags  A pointer to an ::EmberChildFlags containing the child
 * flags of the child corresponding to the passed MAC address.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the child was found in
 *   the child table, or another ::EmberStatus value indicating the reason of
 *   failure.
 *
 * @ingroup parent_support
 */
EmberStatus emberGetChildFlags(EmberMacAddress *address,
                               EmberChildFlags *flags);

/** @brief Return info on the child corresponding to the passed MAC address
 *
 * @note For star coordinators, if the input address is short, the corresponding
 * child will also be searched in the list of devices connected through range
 * extender. Long address and additional flags for these devices are not
 * available to the coordinator.
 *
 * @param[in] address  A pointer to an ::EmberMacAddress that specifies the MAC
 * address of the child (short or long).
 *
 * @param[out] addressResp  A pointer to an ::EmberMacAddress that returns the
 * other address (respectively long or short).
 *
 * @param[out] flags  A pointer to an ::EmberChildFlags containing the child
 * flags of the child corresponding to the passed MAC address.
 *
 * @note Both out parameters are optional. If set to NULL, the API will at least
 * indicate if the child was found in the network.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the child was found in
 *   the child table, ::EMBER_CHILD_NOT_FOUND if it was not, or another
 *   ::EmberStatus value indicating the reason of failure.
 *
 * @ingroup parent_support
 */
EmberStatus emberGetChildInfo(EmberMacAddress *address,
                              EmberMacAddress *addressResp,
                              EmberChildFlags *flags);

/** @brief Remove the node corresponding to the passed MAC address from the
 * child table.
 *
 * @param[in] address  A pointer to an ::EmberMacAddress that specifies the MAC
 * address of the child to be removed.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node was
 *   successfully removed from the child table, or another ::EmberStatus value
 *   indicating the reason of failure.
 *
 * @ingroup parent_support
 */
EmberStatus emberRemoveChild(EmberMacAddress *address);

/** @brief Allow a star topology node that previously joined a network to leave
 * the network. The node will notify the parent node and eventually leave the
 * network. The application is notified that the leave procedure completed via
 * the ::emberStackStatusHandler() handler.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node successfully
 * initiated the network leave procedure, or another ::EmberStatus value
 * indicating the reason of failure.
 */
EmberStatus emberNetworkLeave(void);

/**
 * @brief Populate the short-to-long address mapping table at the MAC layer.
 * The table is meaningful only when running as ::EMBER_MAC_MODE_DEVICE
 * or ::EMBER_MAC_MODE_SLEEPY_DEVICE. The standard 802.15.4 encryption and
 * authentication process requires the security nonce to be populated with the
 * source node long ID. A receiver must do the same to decrypt a
 * secured incoming message. This short-to-long mapping table is used to decrypt
 * a secured incoming packet from a node using short source addressing. If no
 * entry is found in this table, the incoming message will be dropped.
 * This table is also used to encrypt secured outgoing messages with short
 * source addressing in case the node is sending out a secured message with a
 * short source address other than its own.
 *
 * @note Because the table is stored in RAM, the application should ensure
 * it gets correctly re-populated upon reboot.
 *
 * @note Adding a new entry will cause the removal of existing entries matching
 * the passed short ID or long ID.
 *
 * @param[in]   shortId   The short address of the [short, long] entry to be
 *  added to the table.
 *
 * @param[in]   longId   The long address of the [short, long] entry to be
 *  added to the table.
 *
 * @return an ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the mapping was successfully added to the table.
 * - ::EMBER_INVALID_CALL if the node is not running as ::EMBER_MAC_MODE_DEVICE
 *   or as ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 * - ::EMBER_TABLE_FULL if the table is currently full.
 * - ::EMBER_NO_BUFFERS if the heap does not currently have enough space for the
 *   new entry. The size of the table is controlled by the
 *   ::EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE.
 **/
EmberStatus emberMacAddShortToLongAddressMapping(EmberNodeId shortId,
                                                 EmberEUI64 longId);

/**
 * @brief Clear the short-to-long address mapping table at the MAC layer.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if table was cleared, or
 * another ::EmberStatus value indicating the reason of failure.
 **/
EmberStatus emberMacClearShortToLongAddressMappings(void);

/**
 * @}
 */

//------------------------------------------------------------------------------
// Frequency Hopping

/** @brief A special value for the frequency hopping non sleepy client timeout
 * that disables synchronization loss detection.
 */
#define EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_DISABLE_TIMEOUT 0xFFFF

/**
 * @addtogroup frequency_hopping
 * @brief API and callbacks for frequency hopping configuration
 *
 * See network-management.h for source code.
 * @{
 */

/** @brief Set the channel mask for frequency hopping. This API can only be
 * invoked when the node is not frequency hopping.
 *
 * @note The application is responsible for applying this setting to both the
 * server and clients.
 *
 * @param[in] channelMaskLength Length of the bitmap in bytes
 *
 * @param[in] channelMask  A pointer to a bitmap representing allowed channels
 * for frequency hopping.
 *
 * @note The bitmap size needs to be at least
 * (EMBER_FREQUENCY_HOPPING_END_CHANNEL + 8) >> 3 or an error is thrown.
 *
 * @note The bitmap needs to be set again after stopping frequency hopping.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node successfully
 *   set the bitmask. An ::EmberStatus value of ::EMBER_INVALID_CALL if the node
 *   is currently performing frequency hopping. An ::EmberStatus value of
 *   ::EMBER_BAD_ARGUMENT if the resulting channel list is empty, or if
 *   channelMaskLength is shorter than expected.
 */
EmberStatus emberFrequencyHoppingSetChannelMask(uint8_t channelMaskLength,
                                                uint8_t *channelMask);

/** @brief Start the device operating as a frequency hopping server. This API
 * can only be invoked when the node is joined to a network. Notice that the
 * server upon starting hopping shall perform an initial advertisement across
 * the entire channel hopping sequence. This is done to resynchronize clients
 * in case the server was started as result of a reboot.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node successfully
 *   initiated frequency hopping server operations. An ::EmberStatus value of
 *   ::EMBER_INVALID_CALL if the node is not currently joined to a network or if
 *   the node is already performing frequency hopping.
 */
EmberStatus emberFrequencyHoppingStartServer(void);

/** @brief Start operating as a frequency hopping client and synchronize
 * with the specified server. This API can be invoked on nodes that are already
 * joined to a network (with the exception of nodes started as
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE) and nodes that are
 * not joined to a network yet. If the node is already performing frequency
 * hopping, this API returns ::EMBER_INVALID_CALL.
 * If this API returns ::EMBER_SUCCESS, the
 * ::emberFrequencyHoppingStartClientCompleteHandler() is invoked
 * asynchronously to inform the application whether the node successfully
 * synchronized with the specified server or to inform the application of the
 * reason of failure.
 * After the client is synced to a server, it may seamlessly perform the
 * resynchronization process if needed. Sleepy devices in particular
 * periodically perform the resynchronization process. If the client fails a
 * resynchronization process, it informs the application by invoking the
 * ::emberStackStatusHandler() handler with ::EMBER_MAC_SYNC_TIMEOUT status.
 * When this occurs, the client will no longer be synced to the server.
 * The application may elect to attempt a new synchronization process by
 * invoking this API again.
 *
 * @param[in] serverNodeId   An ::EmberNodeId value indicating the node ID of
 *   the server to synchronize with.
 *
 * @param[in] serverPanId    An ::EmberPanId value indicating the PAN ID of the
 *   server to synchronize with. Note that this parameter is meaningful only if
 *   the node is not currently joined to any network.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS indicating that the node
 *   successfully initiated the synchronization process with the server,
 *   otherwise an ::EmberStatus value indicating the reason of failure.
 */
EmberStatus emberFrequencyHoppingStartClient(EmberNodeId serverNodeId,
                                             EmberPanId serverPanId);

/** @brief Stop frequency hopping. This API can only be invoked when
 * the node is frequency hopping. Applicable for both server and client.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS indicating that the node
 *   successfully stopped frequency hopping. An ::EmberStatus value of
 *   ::EMBER_INVALID_CALL if the node is not currently frequency hopping.
 */
EmberStatus emberFrequencyHoppingStop(void);

/** @{
 * @name Callbacks
 */

/** @brief This stack handler is invoked after the application calls the
 *  ::emberFrequencyHoppingStartClient() stack API to inform the application
 *  that the synchronization process with the server is complete.
 * See ::emberFrequencyHoppingStartClient() for details
 *
 *  @param[in] status     An ::EmberStatus value indicating whether the
 *   synchronization process with the server was completed successfully or the
 *   reason for failure.
 */
void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status);

/**
 * @}
 *
 * @}
 */

#endif //__NETWORK_FORMATION_H__
