/***************************************************************************//**
 * @file
 * @brief EmberZNet API for sending and receiving RAW messages, which
 *   do not go out over the Zigbee network.  These messages have MAC
 *   headers formatted by the application.
 *
 * See @ref raw-message for documentation.
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

#ifndef SILABS_RAW_MESSAGE_H
#define SILABS_RAW_MESSAGE_H

#include "mac-header.h"

/**
 * @addtogroup raw_message
 *
 * This file defines routines and callbacks for dealing with the transmission
 * and reception of rawly constructed messages.
 *
 * See raw-message.h for source code.
 * @{
 */

/** @brief This function allows the application to configure the stack to
 *    accept raw MAC data messages that it normally would not accept.
 *    When the stack receives a message matching one of the filters it
 *    will call ::emberMacFilterMatchMessageHandler().
 *    This function should point to a list of ::EmberMacFilterMatchData
 *    or NULL to clear all filters.  The passed value must point to
 *    a valiable in GLOBAL memory.
 *
 * @param macFilterMatchList A pointer to a list of ::EmberMacFilterMatchData values.
 *   The last value should set the bit EMBER_MAC_FILTER_MATCH_END and nothing else.
 *
 * @return ::EMBER_SUCCESS if the MAC filter match list has been configured correctly.
 *   ::EMBER_BAD_ARGUMENT if one of the filters matches a Zigbee MAC header and cannot
 *   be used.
 */
EmberStatus emberSetMacFilterMatchList(const EmberMacFilterMatchData* macFilterMatchList);

/** @brief This function is called when the stack has received a raw MAC message that has
 *    matched one of the application's configured MAC filters.
 *
 * @param macFilterMatchStruct This is a pointer to a structure containing information
 *   about the matching message, including the index of the filter that was matched,
 *   and the actual data of the message.
 */
void emberMacFilterMatchMessageHandler(const EmberMacFilterMatchStruct* macFilterMatchStruct);

/**@brief A callback invoked by the EmberZNet stack to filter out incoming
 * application MAC passthrough messages.  If this returns true for a message
 * the complete message will be passed to emberMacPassthroughMessageHandler()
 * with a type of EMBER_MAC_PASSTHROUGH_APPLICATION.
 *
 * Note that this callback may be invoked in ISR context and should execute as
 * quickly as possible.
 *
 * Note that this callback may be called more than once per incoming message.
 * Therefore the callback code should not depend on being called only once,
 * and should return the same value each time it is called with a given header.
 *
 * If the application includes this callback, it must define
 * EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_FILTER_HANDLER in its
 * CONFIGURATION_HEADER.
 *
 * @param macHeader        A pointer to the initial portion of the
 *     incoming MAC header.  This contains the MAC frame control and
 *     addressing fields.  Subsequent MAC fields, and the MAC payload,
 *     may not be present.
 * @return true if the messages is an application MAC passthrough message.
 */
bool emberMacPassthroughFilterHandler(uint8_t *macHeader);

/** @brief Sends the given message over the air without higher layer
 * network headers.
 *
 * The first two bytes are interpreted as the 802.15.4 frame control field.
 * If the Ack Request bit is set, the packet will be retried as necessary.
 * Completion is reported via ::emberRawTransmitCompleteHandler().
 * Note that the sequence number specified in this packet is not taken into
 * account by the MAC layer. The MAC layer overwrites the sequence number field
 * with the next available sequence number.
 *
 * @param message  The message to transmit.
 *                priority transmit priority
 *		  useCca flag should the CCA stay on or not
 *
 * @return ::EMBER_SUCCESS if the message was successfully submitted to
 * the transmit queue, and ::EMBER_ERR_FATAL otherwise.
 */
EmberStatus emberSendRawMessageExtended(EmberMessageBuffer message, EmberTransmitPriority priority, bool useCca);

/** @brief Sends the given message over the air without higher layer
 * network headers.
 *
 * The first two bytes are interpreted as the 802.15.4 frame control field.
 * If the Ack Request bit is set, the packet will be retried as necessary.
 * Completion is reported via ::emberRawTransmitCompleteHandler().
 * Note that the sequence number specified in this packet is not taken into
 * account by the MAC layer. The MAC layer overwrites the sequence number field
 * with the next available sequence number.
 *
 * @param message  The message to transmit.
 *
 * @return ::EMBER_SUCCESS if the message was successfully submitted to
 * the transmit queue, and ::EMBER_ERR_FATAL otherwise.
 */
#define emberSendRawMessage(message) \
  (emberSendRawMessageExtended(message, EMBER_MAC_TRANSMIT_PRIORITY_NORMAL, true))

/**@brief A callback invoked by the EmberZNet stack when the
 * MAC has finished transmitting a raw message.
 *
 * If the application includes this callback, it must define
 * EMBER_APPLICATION_HAS_RAW_HANDLER in its CONFIGURATION_HEADER.
 *
 * @param message  The raw message that was sent.
 * @param status   ::EMBER_SUCCESS if the transmission was successful,
 *                 or ::EMBER_DELIVERY_FAILED if not.
 */
void emberRawTransmitCompleteHandler(EmberMessageBuffer message,
                                     EmberStatus status);

// Old APIs

/**@brief A callback invoked by the EmberZNet stack when a
 * MAC passthrough message is received.  These are messages that
 * applications may wish to receive but that do
 * not follow the normal ZigBee format.
 *
 * Examples of MAC passthrough messages are Embernet
 * messages and first generation (v1) standalone bootloader messages,
 * and SE (Smart Energy) InterPAN messages.
 * These messages are handed directly to the application and are not
 * otherwise handled by the EmberZNet stack.
 * Other uses of this API are not tested or supported at this time.
 * If the application includes this callback, it must define
 * EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_HANDLER in its CONFIGURATION_HEADER
 * and set emberMacPassthroughFlags to indicate the kind(s) of messages
 * that the application wishes to receive.
 *
 * @param messageType    The type of MAC passthrough message received.
 * @param message        The MAC passthrough message that was received.
 */
void emberMacPassthroughMessageHandler(EmberMacPassthroughType messageType,
                                       EmberMessageBuffer message);

/**@brief Applications wishing to receive MAC passthrough messages must
 * set this to indicate the types of messages they wish to receive.
 */
extern EmberMacPassthroughType emberMacPassthroughFlags;

/**@brief Applications wishing to receive EmberNet messages filtered by
 * source address must set this to the desired source address, as well as
 * setting the EMBER_MAC_PASSTHROUGH_EMBERNET_SOURCE bit in
 * emberMacPassthroughFlags.
 */
extern EmberNodeId emberEmbernetPassthroughSourceAddress;

/** @} END addtogroup */

#endif // SILABS_RAW_MESSAGE_H
