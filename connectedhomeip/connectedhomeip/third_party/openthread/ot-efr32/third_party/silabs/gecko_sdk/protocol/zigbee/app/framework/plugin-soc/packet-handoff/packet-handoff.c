/***************************************************************************//**
 * @file
 * @brief Implementation of the packet-handoff mechanism.  Definitions for the
 * emberPacketHandoff stack hooks, intercepting packets from the stack,
 * copying the data into a flat buffer, then passing the flat packet
 * to other plugins for further processing.
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "buffer_manager/legacy-packet-buffer.h"

#ifdef PRO_COMPLIANCE
 #include "app/test/pro-compliance.h"
 #include "app/test/pro-compliance-printing.h"
#endif

#ifndef PACKET_HANDOFF_BUFFER_SIZE
  #define PACKET_HANDOFF_BUFFER_SIZE 256
#endif

#ifdef UC_BUILD
#include "packet-handoff-config.h"
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ALL_PACKETS == 1)
#define ALLOW_ALL_PACKETS
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_RAW_MAC == 1)
#define ALLOW_RAW_MAC
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_MAC_COMMAND == 1)
#define ALLOW_MAC_COMMAND
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA == 1)
#define ALLOW_NETWORK_DATA
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND == 1)
#define ALLOW_NETWORK_COMMAND
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_DATA == 1)
#define ALLOW_APS_DATA
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_COMMAND == 1)
#define ALLOW_APS_COMMAND
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZDO == 1)
#define ALLOW_ZDO
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZCL == 1)
#define ALLOW_ZCL
#endif
#if (EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_BEACON == 1)
#define ALLOW_BEACON
#endif
#define emberPacketHandoffIncomingHandler emAfPacketHandoffIncomingCallback
#define emberPacketHandoffOutgoingHandler emAfPacketHandoffOutgoingCallback
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ALL_PACKETS
#define ALLOW_ALL_PACKETS
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_RAW_MAC
#define ALLOW_RAW_MAC
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_MAC_COMMAND
#define ALLOW_MAC_COMMAND
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA
#define ALLOW_NETWORK_DATA
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND
#define ALLOW_NETWORK_COMMAND
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_DATA
#define ALLOW_APS_DATA
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_COMMAND
#define ALLOW_APS_COMMAND
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZDO
#define ALLOW_ZDO
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZCL
#define ALLOW_ZCL
#endif
#ifdef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_BEACON
#define ALLOW_BEACON
#endif
#endif // UC_BUILD

// Callbacks
EmberPacketAction emberAfIncomingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      uint8_t* packetData,
                                                      uint8_t* size_p,
                                                      void* data);

EmberPacketAction emberAfOutgoingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      uint8_t* packetData,
                                                      uint8_t* size_p,
                                                      void* data);

// ----------------------------------------------------------------
// Intermediate Packet Handoff Mechanism
// Converts ::EmberMessageBuffer into a flat array then produces the callback
// to emberAfIncomingPacketFilterCallback and emberAfOutgoingPacketFilterCallback
// ----------------------------------------------------------------

EmberPacketAction emberPacketHandoffIncomingHandler(EmberZigbeePacketType packetType,
                                                    EmberMessageBuffer packetBuffer,
                                                    uint8_t index,
                                                    void *data)
{
  uint8_t flatPacket[PACKET_HANDOFF_BUFFER_SIZE];

  switch (packetType) {
    #ifndef ALLOW_ALL_PACKETS

      #ifndef ALLOW_RAW_MAC
    case EMBER_ZIGBEE_PACKET_TYPE_RAW_MAC:
      return EMBER_ACCEPT_PACKET;
      #endif // RAW_MAC

      #ifndef ALLOW_MAC_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_MAC_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // MAC_COMMAND

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA_PRE_DECRYPTION
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_DATA_PRE_DECRYPTION:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND_PRE_DECRYPTION
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_COMMAND_PRE_DECRYPTION:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_NETWORK_DATA
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_DATA:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef ALLOW_NETWORK_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_APS_DATA
    case EMBER_ZIGBEE_PACKET_TYPE_APS_DATA:
      return EMBER_ACCEPT_PACKET;
      #endif // APS_DATA

      #ifndef ALLOW_APS_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_APS_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // APS_COMMAND

      #ifndef ALLOW_ZDO
    case EMBER_ZIGBEE_PACKET_TYPE_ZDO:
      return EMBER_ACCEPT_PACKET;
      #endif // ZDO

      #ifndef ALLOW_ZCL
    case EMBER_ZIGBEE_PACKET_TYPE_ZCL:
      return EMBER_ACCEPT_PACKET;
      #endif // ZCL

      #ifndef ALLOW_BEACON
    case EMBER_ZIGBEE_PACKET_TYPE_BEACON:
      return EMBER_ACCEPT_PACKET;
      #endif // BEACON

       #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_INTERPAN
    case EMBER_ZIGBEE_PACKET_TYPE_INTERPAN:
      return EMBER_ACCEPT_PACKET;
      #endif // INTERPAN

    #endif // !ALLOW_ALL_PACKETS
    default:
    {
      // Proceed only if the index is with in the length of the packet.
      // This check is important to calculate the length before a buffer copy function.
      // is called.
      if (emberMessageBufferLength(packetBuffer) < index) {
        // Return deafault action
        return EMBER_ACCEPT_PACKET;
      }
      uint8_t packetLength = emberMessageBufferLength(packetBuffer) - index;
      EmberPacketAction act;
      emberCopyFromLinkedBuffers(packetBuffer,
                                 index,
                                 flatPacket,
                                 packetLength);
      act = emberAfIncomingPacketFilterCallback(packetType, flatPacket, &packetLength, data);
      if (act == EMBER_MANGLE_PACKET) {
        EmberStatus status = emberSetLinkedBuffersLength(packetBuffer,
                                                         packetLength + index);
        // If changing the length of the buffer is unsuccessful (ran out of free block)
        // tell the stack to drop the packet because something has probably gone wrong
        if (status != EMBER_SUCCESS) {
          return EMBER_DROP_PACKET;
        } else {
          emberCopyToLinkedBuffers(flatPacket,
                                   packetBuffer,
                                   index,
                                   packetLength);
        }
      }
      return act;
    }
  }
}

EmberPacketAction emberPacketHandoffOutgoingHandler(EmberZigbeePacketType packetType,
                                                    EmberMessageBuffer packetBuffer,
                                                    uint8_t index,
                                                    void *data)
{
  uint8_t flatPacket[PACKET_HANDOFF_BUFFER_SIZE];

  switch (packetType) {
    #ifndef ALLOW_ALL_PACKETS

      #ifndef ALLOW_RAW_MAC
    case EMBER_ZIGBEE_PACKET_TYPE_RAW_MAC:
      return EMBER_ACCEPT_PACKET;
      #endif // RAW_MAC

      #ifndef ALLOW_MAC_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_MAC_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // MAC_COMMAND

      #ifndef ALLOW_NETWORK_DATA
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_DATA:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef ALLOW_NETWORK_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_NWK_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_APS_DATA
    case EMBER_ZIGBEE_PACKET_TYPE_APS_DATA:
      return EMBER_ACCEPT_PACKET;
      #endif // APS_DATA

      #ifndef ALLOW_APS_COMMAND
    case EMBER_ZIGBEE_PACKET_TYPE_APS_COMMAND:
      return EMBER_ACCEPT_PACKET;
      #endif // APS_COMMAND

      #ifndef ALLOW_ZDO
    case EMBER_ZIGBEE_PACKET_TYPE_ZDO:
      return EMBER_ACCEPT_PACKET;
      #endif // ZDO

      #ifndef ALLOW_ZCL
    case EMBER_ZIGBEE_PACKET_TYPE_ZCL:
      return EMBER_ACCEPT_PACKET;
      #endif // ZCL

      #ifndef ALLOW_BEACON
    case EMBER_ZIGBEE_PACKET_TYPE_BEACON:
      return EMBER_ACCEPT_PACKET;
      #endif // BEACON

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_INTERPAN
    case EMBER_ZIGBEE_PACKET_TYPE_INTERPAN:
      return EMBER_ACCEPT_PACKET;
      #endif // ZLL

    #endif // !ALLOW_ALL_PACKETS
    default:
    {
      uint8_t packetLength = emberMessageBufferLength(packetBuffer) - index;
      EmberPacketAction act;
      emberCopyFromLinkedBuffers(packetBuffer,
                                 index,
                                 flatPacket,
                                 packetLength);
      if (emPacketHeaderPayload(packetBuffer) != EMBER_NULL_MESSAGE_BUFFER) {
        EmberMessageBuffer payload = emPacketHeaderPayload(packetBuffer);
        uint8_t payloadLength = emberMessageBufferLength(payload);
        emberCopyFromLinkedBuffers(payload,
                                   0,
                                   flatPacket + packetLength,
                                   payloadLength);
        packetLength += payloadLength;
      }
      act = emberAfOutgoingPacketFilterCallback(packetType, flatPacket, &packetLength, data);
      if (act == EMBER_MANGLE_PACKET) {
        EmberStatus status = emberSetLinkedBuffersLength(packetBuffer,
                                                         packetLength + index);
        // If changing the length of the buffer is unsuccessful (ran out of free block)
        // tell the stack to drop the packet because something has probably gone wrong
        if (status != EMBER_SUCCESS) {
          return EMBER_DROP_PACKET;
        } else {
          emberCopyToLinkedBuffers(flatPacket,
                                   packetBuffer,
                                   index,
                                   packetLength);
          if (emPacketHeaderPayload(packetBuffer) != EMBER_NULL_MESSAGE_BUFFER) {
            // The payload is now appended to the end of the packet
            emSetPacketHeaderPayload(packetBuffer, EMBER_NULL_MESSAGE_BUFFER);
          }
        }
      }
      return act;
    }
  }
}
