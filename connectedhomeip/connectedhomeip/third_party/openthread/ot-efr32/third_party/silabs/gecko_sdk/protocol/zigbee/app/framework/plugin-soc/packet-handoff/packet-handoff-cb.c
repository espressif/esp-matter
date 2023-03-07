/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"

/** @brief A protocol layer packet has been received by the stack.
 *
 * This is called when the stack receives a packet that is meant
 * for one of the protocol layers specified in ::EmberZigbeePacketType.
 *
 * The packetType argument is one of the values of the
 * ::EmberZigbeePacketType enum. If the stack receives an 802.15.4
 * MAC beacon, it will call this function with the packetType argument
 * set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON.
 *
 * The implementation of this callback may alter the data contained in
 * packetData, modify options and flags in the auxillary data, or
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
WEAK(EmberPacketAction emberAfIncomingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                           uint8_t* packetData,
                                                           uint8_t* size_p,
                                                           void *data))
{
  return EMBER_ACCEPT_PACKET;
}

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
 * packetData, modify options and flags in the auxillary data, or
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
WEAK(EmberPacketAction emberAfOutgoingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                           uint8_t* packetData,
                                                           uint8_t* size_p,
                                                           void* data))
{
  return EMBER_ACCEPT_PACKET;
}
