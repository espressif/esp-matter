/***************************************************************************//**
 * @file
 * @brief EmberZNet security API
 * See @ref security for documentation
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

#ifndef SILABS_TRUST_CENTER_H
#define SILABS_TRUST_CENTER_H

/**
 * @addtogroup trust_center
 *
 * This file describes the routines used by the Trust Center to manage
 * devices in the network. The Trust center decides whether to use
 * preconfigured keys and manages passing out keys to joining
 * and rejoining devices. The Trust Center also sends out new keys
 * and decides when to start using them.
 *
 * See trust-center.h for source code
 * @{
 */

/** @brief A Trust Center device configuration bitmask example.
 *    The Trust Center is expected to be set up with a Network Key
 *    Preconfigured Link Key that is global throughout all devices
 *    on the Network. The decision whether or not to send the key
 *    in-the-clear is NOT controlled through this bitmask. It
 *    is controlled via the emberTrustCenterJoinHandler(...) function.
 */
#define EMBER_FORM_TRUST_CENTER_NETWORK_BITMASK \
  (EMBER_STANDARD_SECURITY_MODE                 \
   | EMBER_TRUST_CENTER_GLOBAL_LINK_KEY         \
   | EMBER_HAVE_NETWORK_KEY                     \
   | EMBER_HAVE_PRECONFIGURED_KEY)

/** @brief A coordinator device configuration bitmask example.
 *    The coordinator is expected to be set up with a Network Key
 *    and a Preconfigured Link Key that is global throughout all devices
 *    on the Network. The decision whether or not to send the key
 *    in-the-clear is decentralized. Each individual router
 *    can make this decision via the emberTrustCenterJoinHandler(...) function.
 */
#define EMBER_FORM_DISTRIBUTED_TRUST_CENTER_NETWORK_BITMASK \
  (EMBER_STANDARD_SECURITY_MODE                             \
   | EMBER_TRUST_CENTER_GLOBAL_LINK_KEY                     \
   | EMBER_DISTRIBUTED_TRUST_CENTER_MODE                    \
   | EMBER_HAVE_NETWORK_KEY                                 \
   | EMBER_HAVE_PRECONFIGURED_KEY)

/** @brief Broadcast a new encryption key but
 * does not tell the nodes in the network to start using it.
 *
 * To broadcast a new key, use ::emberBroadcastNetworkKeySwitch(),
 * which is only valid for the Trust Center/Coordinator.
 * It is not valid when operating in Distributed Trust Center mode.
 *
 * It is up to the application to determine how quickly to send
 * the Switch Key after sending the alternate encryption key.
 * The factors to consider are the polling rate of sleepy end devices
 * and the buffer size of their parent nodes. Sending too quickly
 * may cause a sleepy end device to miss the Alternate Encryption Key
 * and only get the Switch Key message, which means it will be unable
 * to change to the new network key.
 *
 * @param key  A pointer to a 16-byte encryption key
 * (EMBER_ENCRYPTION_KEY_SIZE).  A NULL (or all zero key) may be passed in,
 * which will cause the stack to randomly generate a new key.
 *
 * @return An ::EmberStatus value that indicates the success
 * or failure of the command.
 */
EmberStatus emberBroadcastNextNetworkKey(EmberKeyData* key);

/** @brief Send a unicast update of the network key
 *  to the target device. The APS command will be encrypted using
 *  the device's current APS link key. On success, the bit
 *  ::EMBER_KEY_UNICAST_NWK_KEY_UPDATE_SENT will be set in the link key
 *  table entry for the device. When a successful call is made to
 *  ::emberBroadcastNetworkKeySwitch(), the bit will be cleared for all
 *  entries.
 *
 *  On the first call to this function, the trust center's local copy of
 *  the alternate NWK key will be updated with the new value.
 *
 *  Both the short and long address of the device must be known ahead of
 *  time and passed in as parameters. It is assumed that the application
 *  has already generated the new network key and will pass the same key value
 *  on subsequent calls to send the key to different nodes in the network.
 *
 *  @param targetStort The short node ID of the device to send a NWK key
 *  update to.
 *
 *  @param targetLong The EUI64 of the node to send a key update NWK key
 *  update to.
 *
 *  @param nwkKey A pointer to the new NWK key value.
 *
 * @return An ::EmberStatus value that indicates the success or failure
 *   of the command.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberSendUnicastNetworkKeyUpdate(EmberNodeId targetShort,
                                             EmberEUI64  targetLong,
                                             EmberKeyData* newKey);
#else
EmberStatus emSendAlternateNetworkKeyToAddress(EmberNodeId targetShort,
                                               EmberEUI64 targetLong,
                                               EmberKeyData* newKey);
#define emberSendUnicastNetworkKeyUpdate(shortAddr, longAddr, key) \
  emSendAlternateNetworkKeyToAddress((shortAddr), (longAddr), (key))
#endif

/** @brief Send the network key
 *  to the target device. The APS command will be encrypted using
 *  the device's current APS link key. The call fails if no
 *  link key is available.
 *
 *
 *  Both the short and long address of the device must be known ahead of
 *  time and passed in as parameters. It is assumed that the application
 *  has already generated the new network key and will pass the same key value
 *  on subsequent calls to send the key to different nodes in the network.
 *
 *  @param targetStort The short node ID of the device to send a NWK key
 *  to.
 *
 *  @param targetLong The EUI64 of the node to send a  NWK key
 *  to.
 *
 *  @param parentShortId The short node ID of the parent of the device to send a NWK key
 *  to.
 *
 *
 * @return An ::EmberStatus value that indicates the success or failure
 *   of the command.
 */

EmberStatus emberUnicastCurrentNetworkKey(EmberNodeId targetShort,
                                          EmberEUI64  targetLong,
                                          EmberNodeId parentShortId);

/** @brief Broadcast a switch key message to
 * tell all nodes to change to the sequence number of the previously
 * sent Alternate Encryption Key.
 *
 * This function is only valid for the Trust Center/Coordinator
 * and will also cause the Trust Center/Coordinator to change its Network
 * Key. It is not valid when operating in Distributed Trust Center mode.
 *
 * @return An ::EmberStatus value that indicates the success or failure
 * of the command.
 */
EmberStatus emberBroadcastNetworkKeySwitch(void);

/** @brief Allow the application running on the Trust
 * Center (which is the coordinator for ZigBee networks) to control
 * which nodes are allowed to join the network. If the node is allowed to join,
 * the trust center must decide whether to send the Network Key encrypted
 * or unencrypted to the joining node.
 *
 * A default handler is provided and its behavior is as follows.
 * A status of ::EMBER_DEVICE_SECURED_REJOIN means that the device has the
 * Network Key, no action is required from the Trust Center.  A status of
 * ::EMBER_DEVICE_LEFT also requires no action. In both cases, ::EMBER_NO_ACTION
 * is returned.
 *
 * When operating in a network with a Trust Center and there is a Global
 * Link Key configured, ::EMBER_USE_PRECONFIGURED_KEY will be returned, which
 * means the Trust Center is using a preconfigured Link Key. The Network Key
 * will be sent to the joining node encrypted with the Link Key. If a Link Key
 * has not been set on the Trust Center, ::EMBER_DENY_JOIN is returned.
 *
 * The ::EMBER_ASK_TRUST_CENTER decision has been deprecated. This function
 * will not be called for a router or end device when operating in a Network
 * With a Trust Center.
 *
 * If the device is a router in a network that is operating in a Distributed
 * Trust Center Security mode, the handler will be called by the stack.
 *
 * The default handler in a Distributed Trust Center Security mode network is
 * as follows:
 * If the router received an encrypted Network Key when it joined, a
 * preconfigured Link key will be used to send the Network Key Encrypted to
 * the joining device (::EMBER_USE_PRECONFIGURED_KEY). If the router received
 * the Network Key in the clear, it will also send the key in the clear to
 * the joining node (::EMBER_SEND_KEY_IN_THE_CLEAR).
 *
 * @param newNodeId       The node ID of the device wishing to join.
 * @param newNodeEui64    The EUI64 of the device wishing to join.
 * @param status          The EmberUpdateDeviceStatus indicating whether the device
 * is joining/rejoining or leaving.
 * @param parentOfNewNode The node ID of the parent of the device wishing to join.
 *
 * @return ::EMBER_USE_PRECONFIGURED_KEY to allow the node to join without sending it the key.
 * ::EMBER_SEND_KEY_IN_THE_CLEAR to allow the node to join and send it the key.
 * ::EMBER_DENY_JOIN to reject the join attempt.
 * The value should not be returned if the local node is itself the trust center.
 */
EmberJoinDecision emberTrustCenterJoinHandler(EmberNodeId newNodeId,
                                              EmberEUI64 newNodeEui64,
                                              EmberDeviceUpdate status,
                                              EmberNodeId parentOfNewNode);

/** @brief Cause a coordinator to become the Trust Center
 *    when it is operating in a network that is not using one. It will
 *    send out an updated Network Key to all devices that will indicate a
 *    transition of the network to now use a Trust Center. The Trust Center
 *    should also switch all devices to using the new network key with a call
 *    to emberBroadcastNetworkKeySwitch().
 *
 * @param newNetworkKey  The key data for the Updated Network Key.
 *
 * @return An ::EmberStatus value that indicates the success or failure
 * of the command.
 */
EmberStatus emberBecomeTrustCenter(EmberKeyData* newNetworkKey);

/** @brief Control the policy that the Trust Center uses
 *  for determining whether to allow or deny requests for Trust Center
 *  link keys.
 *
 *  The following is a good set of guidelines for TC Link key requests:
 *    - If preconfigured TC link keys are set up on devices, requests
 *      for the TC key should never be allowed (::EMBER_DENY_TC_LINK_KEY_REQUESTS).
 *    - If devices request link keys during joining (i.e., join in the clear
 *      and set ::EMBER_GET_LINK_KEY_WHEN_JOINING) then it is advisable to
 *      allow requesting keys from the TC for a short
 *      period of time (e.g., the same amount of time "permit joining" is turned
 *      on). Afterwards, requests for the TC link key should be denied.
 */
#ifdef EMBER_MULTI_NETWORK_STRIPPED
#define emGetNetworkIndexForForkedGlobal()  0
#else // EMBER_MULTI_NETWORK_STRIPPED
extern uint8_t emGetNetworkIndexForForkedGlobal(void);
#endif // EMBER_MULTI_NETWORK_STRIPPED

extern EmberTcLinkKeyRequestPolicy emberTrustCenterLinkKeyRequestPolicies[];
#define emberTrustCenterLinkKeyRequestPolicy emberTrustCenterLinkKeyRequestPolicies[emGetNetworkIndexForForkedGlobal()]

/** @brief Control the policy that the Trust Center uses
 *  for determining whether to allow or deny requests for application
 *  link keys between device pairs. When a request is received and the
 *  policy is ::EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY, the
 *  TC will generate a random key and send a copy to both devices encrypted
 *  with their individual link keys.
 *
 *  Generally, application link key requests may always be allowed.
 */
extern EmberAppLinkKeyRequestPolicy emberAppLinkKeyRequestPolicies[];
#define emberAppLinkKeyRequestPolicy emberAppLinkKeyRequestPolicies[emGetNetworkIndexForForkedGlobal()]

/** @brief Send an APS remove device command to the destination.
 *    If the destination is an end device, this must be sent to
 *    the parent of the end device. In that case, the deviceToRemoveLong and the
 *    destLong will be different values. Otherwise, if a router is
 *    being asked to leave, those parameters will be the same.
 *    This command will be APS encrypted with the destination device's link key,
 *    which means a link key must be present.
 *
 * @param destShort The short node ID of the destination of the command.
 *
 * @param destLong The EUI64 of the destination of the command.
 *
 * @param deviceToRemoveLong The EUI64 of the target device being asked to leave.
 *
 * @return An ::EmberStatus value indicating success or failure of the
 *   operation.
 */
EmberStatus emberSendRemoveDevice(EmberNodeId destShort,
                                  EmberEUI64 destLong,
                                  EmberEUI64 deviceToRemoveLong);

/** @brief Send the trust center link key to a node.
 *
 * This function sends an APS TransportKey command containing the current trust
 * center link key. The node to which the command is sent is specified via the
 * short and long address arguments.
 *
 * @param destinationNodeId The short address of the node to which this command
 *   will be sent.
 * @param destinationEui64 The long address of the node to which this command
 *   will be sent.
 * @return An ::EmberStatus value indicating success or failure of the operation.
 */
EmberStatus emberSendTrustCenterLinkKey(EmberNodeId destinationNodeId,
                                        EmberEUI64 destinationEui64);

// @} END addtogroup

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The entire file trust-center.h is new and is described in @ref trust_center.
 * <ul>
 * <li> <b>New items</b>
 * <ul>
 * <li>
 * </ul>
 * <li> <b>Changed items</b>
 * <ul>
 * <li>
 * </ul>
 * <li> <b>Removed items</b>
 * <ul>
 * <li>
 * </ul>
 * </ul>
 * -->
 */

#endif // SILABS_TRUST_CENTER_H
