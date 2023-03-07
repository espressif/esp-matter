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
 * @addtogroup message
 * @brief Connect APIs and handlers for sending and receiving messages.
 *
 * Note that MAC mode and Extended star/direct mode use different APIs for
 * messaging.
 *
 * See message.h for source code.
 * @{
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/** @brief \deprecated  The maximum length in bytes of the application payload
 *  for an unsecured message.  This define has been deprecated, you should use
 *  the ::emberGetMaximumPayloadLength API instead.
 */
#define EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH     111

/** @brief \deprecated  The maximum length in bytes of the application payload
 * for a secured message. This define has been deprecated, you should use the
 * ::emberGetMaximumPayloadLength API instead.
 */
#define EMBER_MAX_SECURED_APPLICATION_PAYLOAD_LENGTH       102

/** @brief The maximum allowed endpoint value.
 */
#define EMBER_MAX_ENDPOINT                                 0xF

/**
 * @name Handlers
 * @anchor Handlers-message
 * The Application Framework implements all handlers, directly calling their
 * associated callbacks. By default, Connect projects declare such callbacks as
 * stubs in flex-callbacks-stubs.c. Hence, to use an enabled Connect feature,
 * applications should replace the stub with their own implementation of the
 * associated callback (typically in flex-callbacks.c).  See UG235.04 for more
 * info.
 * @{
 */

/** @brief This handler is invoked when the stack has completed sending a
 *  message.
 *
 * @param[in] status   An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if an ACK was received from the destination or no ACK was
 * requested.
 * - ::EMBER_MAC_NO_ACK_RECEIVED if an ACK was requested and no ACK was
 * received.
 * - ::EMBER_MAC_INDIRECT_TIMEOUT if the destination is a sleepy node and the
 * packet timed-out before the sleepy node sent a data request.
 * - ::EMBER_MAC_INDIRECT_MESSAGE_PURGED if the destination is a sleepy node
 *  and it was removed from the child table while the packet was stored in the
 *  indirect queue.
 * - ::EMBER_PHY_TX_CCA_FAIL if the node failed all the clear channel assessment
 * attempts.
 * - ::EMBER_PHY_TX_INCOMPLETE if the transmission was not completed correctly.
 *
 * @param[in] message  An ::EmberOutgoingMessage describing the outgoing packet.
 *
 * @warning Implement associated callback
 *   @ref emberAfMessageSentCallback() to use. See @ref Handlers-message
 *  "Handlers" for additional information.
 */
void emberMessageSentHandler(EmberStatus status, EmberOutgoingMessage *message);

/** @brief This handler is invoked when a node of ::EMBER_MAC_MODE_DEVICE type or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE type has completed sending a MAC frame.
 *
 * @param[in] status   An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if an ACK was received from the destination or no ACK was
 * requested.
 * - ::EMBER_MAC_NO_ACK_RECEIVED if an ACK was requested and no ACK was
 * received.
 * - ::EMBER_MAC_INDIRECT_TIMEOUT if the MAC frame was sent out via the indirect
 * queue and the it timed-out before a data request was received.
 * - ::EMBER_MAC_INDIRECT_MESSAGE_PURGED if the MAC frame was sent out via the
 * indirect queue and it was removed prior to a data request being received. See
 * ::emberPurgeIndirectMessages().
 * - ::EMBER_MAC_SECURITY_FAILED if the stack failed to encrypt the message.
 * This typically occurs when a node is sending a message using short source
 * addressing with an address other than the node's short address and the no
 * mapping to a corresponding address was found in the short-to-long address
 * mapping table. The application should use the
 * ::emberMacAddShortToLongAddressMapping to populate such table.
 * - ::EMBER_PHY_TX_CCA_FAIL if the node failed all the clear channel assessment
 * attempts.
 * - ::EMBER_PHY_TX_INCOMPLETE if the transmission was not completed correctly.
 *
 * @param[in] message  An ::EmberOutgoingMacMessage describing the outgoing MAC
 * frame.
 *
 * @warning Implement associated callback
 *   @ref emberAfMacMessageSentCallback() to use. See @ref Handlers-message
 *  "Handlers" for additional information.
 */
void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message);

/** @brief This handler is invoked when a packet has been received from a node
 * type other than ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] message  An ::EmberIncomingMessage describing the incoming packet.
 *
 * @warning Implement associated callback
 *   @ref emberAfIncomingMessageCallback() to use. See @ref Handlers-message
 *  "Handlers" for additional information.
 */
void emberIncomingMessageHandler(EmberIncomingMessage *message);

/** @brief This handler is invoked when a node of ::EMBER_MAC_MODE_DEVICE type
 *  or ::EMBER_MAC_MODE_SLEEPY_DEVICE has received a MAC frame.
 *
 * @param[in] message  An ::EmberIncomingMacMessage describing the incoming
 * packet.
 *
 * @warning Implement associated callback
 *   @ref emberAfIncomingMacMessageCallback() to use. See @ref Handlers-message
 *  "Handlers" for additional information.

 */
void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message);

//@}//END Handlers

/** @brief Send a message to the passed destination short ID.
 *
 * @param[in] destination The destination node short ID.
 *
 * @param[in] endpoint    The destination endpoint of the outgoing message.
 *                        This value can't exceed ::EMBER_MAX_ENDPOINT.
 *
 * @param[in] messageTag  A value chosen by the application. This value will be
 *                        passed in the corresponding
 *                        ::emberMessageSentHandler() call.
 *
 * @param[in] messageLength The size of the message payload in bytes. Use the
 * ::emberGetMaximumPayloadLength() API to determine the maximum message length
 * allowed.
 *
 * @param[in] message A pointer to an array of bytes containing the message
 * payload.
 *
 * @param[in] options  Specifies the ::EmberMessageOptions for the outgoing
 * message.
 *
 * @return an ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the message was accepted by the stack. If a success
 * status is returned, the ::emberMessageSentHandler() callback is invoked
 * by the stack to indicate whether the message was successfully delivered or
 * the reason for failure.
 * - ::EMBER_INVALID_CALL if the node is not joined to a network or the node is
 * of ::EMBER_MAC_MODE_DEVICE device type or ::EMBER_MAC_MODE_SLEEPY_DEVICE (use
 * ::emberMacMessageSend instead).
 * - ::EMBER_BAD_ARGUMENT if the packet length is 0, the passed TX options
 * indicates some feature that is not supported, the passed endpoint
 * exceeds ::EMBER_MAX_ENDPOINT
 * - ::EMBER_MESSAGE_TOO_LONG if the message does not fit in a single frame.
 * - ::EMBER_PHY_TX_BUSY if the message cannot be sent since the node does not
 * support MAC queuing and the radio is currently busy.
 * - ::EMBER_MAC_TRANSMIT_QUEUE_FULL if the outgoing MAC queue is currently
 * full.
 * - ::EMBER_NO_BUFFERS if the stack could not allocate enough RAM to store the
 * submitted message.
 * - ::EMBER_MAC_UNKNOWN_DESTINATION if the node is part of a star network and
 * the destination node does not appear in the node's routing table.
 * - ::EMBER_MAC_SECURITY_NOT_SUPPORTED if the message was requested to be sent
 * out secured and either the local node does not support security or the
 * destination node is known to not support security.
 * - ::EMBER_MAC_BUSY if the message was not accepted because the MAC is
 * currently performing some critical operation.
 */
EmberStatus emberMessageSend(EmberNodeId destination,
                             uint8_t endpoint,
                             uint8_t messageTag,
                             EmberMessageLength messageLength,
                             uint8_t *message,
                             EmberMessageOptions options);

/** @brief Create a MAC level frame and sends it to the passed destination.
 * This API can only be used for nodes of ::EMBER_MAC_MODE_DEVICE node type or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE node type.
 *
 * @param[in] macFrame   A pointer to an ::EmberMacFrame struct that specifies
 * the source and destination addresses and the source and destination PAN IDs
 * for the message to be sent. Note that if the source/destination PAN ID is not
 * specified, it defaults to the node's PAN ID. Also, the destination
 * address mode must be either ::EMBER_MAC_ADDRESS_MODE_SHORT or
 * ::EMBER_MAC_ADDRESS_MODE_LONG.
 *
 * @param[in] messageTag  A value chosen by the application. This value will be
 * passed in the corresponding ::emberMacMessageSentHandler() call.
 *
 * @param[in] messageLength The size in bytes of the message payload. The
 * application can use the ::emberGetMaximumPayloadLength() API to determine the
 * maximum allowable payload, given a permutation of source and destination
 * addressing and other TX options.
 *
 * @param[in] message A pointer to an array of bytes containing the message
 * payload.
 *
 * @param[in] options Specifies the ::EmberMessageOptions for the outgoing
 * message.
 *
 * @return an ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the message was accepted by the stack. If a success
 * status is returned, the ::emberMacMessageSentHandler() callback will be
 * invoked by the stack to indicate whether the message was successfully
 * delivered or the reason for failure.
 * - ::EMBER_INVALID_CALL if the node is of a node type other than
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 * - ::EMBER_BAD_ARGUMENT if the packet length is 0, the passed TX options
 * indicates some feature that is not supported or the destination address mode
 * is set to ::EMBER_MAC_ADDRESS_MODE_NONE.
 * - ::EMBER_MESSAGE_TOO_LONG if the message does not fit in a single frame.
 * - ::EMBER_PHY_TX_BUSY if the message cannot be sent since the node does not
 * support MAC queuing and the radio is currently busy.
 * - ::EMBER_MAC_TRANSMIT_QUEUE_FULL if the outgoing MAC queue is currently
 * full.
 * - ::EMBER_NO_BUFFERS if the stack could not allocate enough RAM to store the
 * submitted message.
 * - ::EMBER_MAC_SECURITY_NOT_SUPPORTED if the message was requested to be sent
 * out with a security but no security plugin was enabled.
 * - ::EMBER_MAC_BUSY if the message was not accepted because the MAC is
 * currently performing a critical operation.
 */
EmberStatus emberMacMessageSend(EmberMacFrame *macFrame,
                                uint8_t messageTag,
                                EmberMessageLength messageLength,
                                uint8_t *message,
                                EmberMessageOptions options);

/** @brief Send a data request command to the parent node. Note that if the
 * node short ID is a value of ::EMBER_USE_LONG_ADDRESS, the node shall use its
 * long ID as source address.
 *
 * @return and ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the data poll was accepted by the MAC layer.
 * - ::EMBER_INVALID_CALL if the node is not joined to a network, the node is
 * not an end device, an ::EMBER_MAC_MODE_DEVICE or an
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE, or the node is of ::EMBER_MAC_MODE_DEVICE
 * or ::EMBER_MAC_MODE_SLEEPY_DEVICE node type, is not joined to a coordinator
 * and the poll destination was not correctly set via the
 * ::emberSetPollDestinationAddress() API.
 * - ::EMBER_MAC_BUSY if the MAC is currently performing a critical
 * operation.
 */
EmberStatus emberPollForData(void);

/** @brief Set data polls destination address for nodes of
 * ::EMBER_MAC_MODE_DEVICE node type or ::EMBER_MAC_MODE_SLEEPY_DEVICE node
 *  type.
 *
 * @return and ::EmberStatus value of ::EMBER_SUCCESS if the data poll
 * destination was correctly set, or another ::EmberStatus value indicating the
 * reason of failure.
 */
EmberStatus emberSetPollDestinationAddress(EmberMacAddress *destination);

/**
 * @brief Purge all indirect transmissions from the indirect message queue.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if all indirect
 * messages were purged, or another ::EmberStatus value indicating the reason
 * of failure.
 *
 * @ingroup parent_support
 **/
EmberStatus emberPurgeIndirectMessages(void);

/**
 * @brief Set indirect queue timeout value. The indirect queue timeout
 * is set by default to ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS.
 *
 * @param timeoutMs  The timeout in milliseconds to be set.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the passed timeout was
 * successfully set, or a value of ::EMBER_BAD_ARGUMENT if the passed value is
 * invalid.
 *
 * @ingroup parent_support
 **/
EmberStatus emberSetIndirectQueueTimeout(uint32_t timeoutMs);

/**
 * @brief Return the maximum payload according to the passed source and
 * destination addressing modes, the passed secured flag, and the current
 * configuration of the node.
 *
 * @param[in] srcAddressMode  An ::EmberMacAddressMode value indicating the mode
 * of the source address. Note, this parameter is only meaningful if the node
 * was started as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] dstAddressMode  An ::EmberMacAddressMode value indicating the mode
 * of the destination address. Note, this parameter is only meaningful if the
 * node was started as ::EMBER_MAC_MODE_DEVICE or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] interpan  Indicates whether the frame is an interpan frame or not.
 * Note, this parameter is only meaningful if the node was started as
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param[in] secured   Indicates whether the frame should be secured or not.
 *
 * @return The maximum payload length in bytes achievable according to the
 * passed parameters or \b 0xFF if the node is currently active on a network or
 * any of the passed parameters are invalid.
 **/
uint8_t emberGetMaximumPayloadLength(EmberMacAddressMode srcAddressMode,
                                     EmberMacAddressMode dstAddressMode,
                                     bool interpan,
                                     bool secured);

#endif //__MESSAGE_H__

/** @} // END addtogroup
 */
