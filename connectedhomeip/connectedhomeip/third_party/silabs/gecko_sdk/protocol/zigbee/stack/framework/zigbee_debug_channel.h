/******************************************************************************/
/**
 * @file
 * @brief Development Kit debugging utilities
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_ZIGBEE_DEBUG_CHANNEL_H
#define SILABS_ZIGBEE_DEBUG_CHANNEL_H

#include <sl_iostream_swo_itm_8.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "sl_status.h"

#if !defined(EMBER_TEST_ASSERT)
 #define EMBER_TEST_ASSERT(x) assert(x)
#endif

// Subtypes for use with EM_DEBUG_STATS type.
enum {
  EM_STATS_MAC_CSMA_FAILURE,
  EM_STATS_MAC_SEND_SUCCESS,
  EM_STATS_MAC_SEND_FAILURE,
  EM_STATS_MAC_RECEIVE,        // in mac-arbiter.c:
  EM_STATS_DROP_DEBUG_MESSAGE, // in debug.c
  EM_STATS_OUT_OF_BUFFERS,     // in packet-buffer.c
  EM_STATS_ARRAY_SIZE          // this entry must be last
};

//******************************************************************************

/**
 * @addtogroup debug
 * @brief A set of utilities for printing to the debug backchannel.
 * @{
 */

/**
 * @brief Builds and prints a message on the debug channel using a SL-style format string
 *
 * @param[in]   debugType       the debug message type
 * @param[in]   formatString    SL-style format string
 */
void emDebugBinaryFormat(sl_iostream_swo_itm_8_msg_type_t debugType, const char *formatString, ...);

/**
 * @brief Wrapper for sl_iostream_printf using the debug handle
 *
 * @param formatString
 * @param ...
 */
void sl_debug_printf(const char *formatString, ...);

/**
 * @brief Prints the following information
 *        Reset type, node EUI64, Zigbee stack version information, etc.
 *
 * @param[in]
 */
void emDebugResetInfo(void);

/**
 * @brief Reads flash memory and prints memory dump on the debug channel
 *
 * @param[in]   debugType       Message Type as printed on network analyzer view
 * @param[in]   start           Memory start address rounded down to nearest 64 byte boundary
 * @param[in]   end             Memory end address
 */
void emDebugMemoryDump(uint16_t debugType, uint8_t *start, uint8_t *end);

void emDebugApiTrace(uint16_t debugType, const char * formatString, ...);

/**
 * @}
 *
 */

/* Internal helper functions to setup framing, insert message bytes and setup end of message */
void        sli_debug_start_message(sl_iostream_swo_itm_8_msg_type_t debugType, uint8_t *buffer, size_t bufferSize);
sl_status_t sli_debug_put_bytes(uint8_t unusedFlushVar, uint8_t *bytes, size_t count);
sl_status_t sli_debug_end_message(void);
void        emDebugInternalBinaryPrintf(sl_iostream_swo_itm_8_msg_type_t debugType,
                                        const char *                     formatString,
                                        va_list                          args);

enum {
  EM_DEBUG_SET_BINDING = 0x00,   // Set binding function call.
  EM_DEBUG_DELETE_BINDING = 0x01,   // Delete binding function call.
  EM_DEBUG_CLEAR_BINDING_TABLE = 0x02,   // Clear binding table function call.
  EM_DEBUG_SEND_LIMITED_MULTICAST = 0x06,   // Send limited multicast function call.
  EM_DEBUG_SEND_UNICAST = 0x08,   // Send unicast function call.
  EM_DEBUG_SEND_BROADCAST = 0x09,   // Send broadcast function call.
  EM_DEBUG_PROXY_BROADCAST = 0x0A,   // Proxy broadcast function call.
  EM_DEBUG_CANCEL_MESSAGE = 0x0B,   // Cancel message function call.
  EM_DEBUG_SEND_REPLY = 0x0C,   // Send reply function call.
  EM_DEBUG_SET_REPLY_BINDING = 0x0D,   // Set reply binding function call.
  EM_DEBUG_MESSAGE_SENT = 0x0E,   // Message sent function call.
  EM_DEBUG_INCOMING_MESSAGE_HANDLER = 0x11,   // Incoming message handler function call.
  EM_DEBUG_STACK_STATUS_HANDLER = 0x13,   // Stack status handler function call.
  EM_DEBUG_NETWORK_INIT = 0x14,   // Network init function call.
  EM_DEBUG_FORM_NETWORK = 0x15,   // Form network function call.
  EM_DEBUG_JOIN_NETWORK = 0x16,   // Join network function call.
  EM_DEBUG_LEAVE_NETWORK = 0x17,   // Leave network function call.
  EM_DEBUG_PERMIT_JOINING = 0x18,   // Permit joining function call.
  EM_DEBUG_POLL_FOR_DATA = 0x19,   // Poll for data function call.
  EM_DEBUG_POLL_HANDLER = 0x1A,   // Poll handler function call.
  EM_DEBUG_TRUST_CENTER_JOIN_HANDLER = 0x1E,   // Trust center join handler function call.
  EM_DEBUG_SET_MESSAGE_FLAG = 0x20,   // Set message flag function call.
  EM_DEBUG_CLEAR_MESSAGE_FLAG = 0x21,   // Clear message flag function call.
  EM_DEBUG_POLL_COMPLETE_HANDLER = 0x22,   // Poll complete handler function call.
  EM_DEBUG_CHILD_JOIN_HANDLER = 0x23,   // Child join handler function call.
  EM_DEBUG_START_SCAN = 0x24,   // Start scan function call.
  EM_DEBUG_STOP_SCAN = 0x25,   // Stop scan function call.
  EM_DEBUG_SCAN_COMPLETE_HANDLER = 0x26,   // Scan complete handler function call.
  EM_DEBUG_NETWORK_FOUND_HANDLER = 0x27,   // Network found handler function call.
  EM_DEBUG_ENERGY_SCAN_RESULT_HANDLER = 0x28,   // Energy scan result handler function call.
  EM_DEBUG_SET_INITIAL_SECURITY_STATE = 0x2B,   // Set initial security state function call.
  EM_DEBUG_REJOIN_NETWORK = 0x2C,   // Rejoin network function call.
  EM_DEBUG_STACK_POWER_DOWN = 0x2D,   // Stack power down function call.
  EM_DEBUG_STACK_POWER_UP = 0x2E,   // Stack power up function call.
  EM_DEBUG_SET_EXTENDED_SECURITY_BITMASK = 0x2F,   // Set extended security bitmask function call.
};

#define EXEC_AND_ASSERT_IF_DEBUG(foo) assert(foo)

#define API_TRACE_SET_BINDING(index)    \
  emDebugApiTrace(EM_DEBUG_API_TRACE,   \
                  "BB",                 \
                  EM_DEBUG_SET_BINDING, \
                  (index)               \
                  )
#define API_TRACE_DELETE_BINDING(index)    \
  emDebugApiTrace(EM_DEBUG_API_TRACE,      \
                  "BB",                    \
                  EM_DEBUG_DELETE_BINDING, \
                  (index)                  \
                  )
#define API_TRACE_CLEAR_BINDING_TABLE( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,          \
                  "B",                         \
                  EM_DEBUG_CLEAR_BINDING_TABLE \
                  )
#define API_TRACE_SEND_LIMITED_MULTICAST(groupId, profileId, clusterId, sourceEndpoint, destinationEndpoint, options, radius, nonmemberRadius) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                          \
                  "BWWBBBWBB",                                                                                                                 \
                  EM_DEBUG_SEND_LIMITED_MULTICAST,                                                                                             \
                  (groupId),                                                                                                                   \
                  (profileId),                                                                                                                 \
                  (clusterId),                                                                                                                 \
                  (sourceEndpoint),                                                                                                            \
                  (destinationEndpoint),                                                                                                       \
                  (options),                                                                                                                   \
                  (radius),                                                                                                                    \
                  (nonmemberRadius)                                                                                                            \
                  )
#define API_TRACE_SEND_UNICAST(indexOrDestination, profileId, clusterId, sourceEndpoint, destinationEndpoint, options) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                  \
                  "BWWWBBW",                                                                                           \
                  EM_DEBUG_SEND_UNICAST,                                                                               \
                  (indexOrDestination),                                                                                \
                  (profileId),                                                                                         \
                  (clusterId),                                                                                         \
                  (sourceEndpoint),                                                                                    \
                  (destinationEndpoint),                                                                               \
                  (options)                                                                                            \
                  )
#define API_TRACE_SEND_BROADCAST(destination, profileId, clusterId, sourceEndpoint, destinationEndpoint, options, radius) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                     \
                  "BWWWBBWB",                                                                                             \
                  EM_DEBUG_SEND_BROADCAST,                                                                                \
                  (destination),                                                                                          \
                  (profileId),                                                                                            \
                  (clusterId),                                                                                            \
                  (sourceEndpoint),                                                                                       \
                  (destinationEndpoint),                                                                                  \
                  (options),                                                                                              \
                  (radius)                                                                                                \
                  )
#define API_TRACE_PROXY_BROADCAST(source, destination, sequence, profileId, clusterId, sourceEndpoint, destinationEndpoint, options, radius) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                        \
                  "BWWBWWBBWB",                                                                                                              \
                  EM_DEBUG_PROXY_BROADCAST,                                                                                                  \
                  (source),                                                                                                                  \
                  (destination),                                                                                                             \
                  (sequence),                                                                                                                \
                  (profileId),                                                                                                               \
                  (clusterId),                                                                                                               \
                  (sourceEndpoint),                                                                                                          \
                  (destinationEndpoint),                                                                                                     \
                  (options),                                                                                                                 \
                  (radius)                                                                                                                   \
                  )
#define API_TRACE_CANCEL_MESSAGE(message)  \
  emDebugApiTrace(EM_DEBUG_API_TRACE,      \
                  "BB",                    \
                  EM_DEBUG_CANCEL_MESSAGE, \
                  (message)                \
                  )
#define API_TRACE_SEND_REPLY(clusterId, reply) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,          \
                  "BWB",                       \
                  EM_DEBUG_SEND_REPLY,         \
                  (clusterId),                 \
                  (reply)                      \
                  )
#define API_TRACE_SET_REPLY_BINDING(index)    \
  emDebugApiTrace(EM_DEBUG_API_TRACE,         \
                  "BB",                       \
                  EM_DEBUG_SET_REPLY_BINDING, \
                  (index)                     \
                  )
#define API_TRACE_MESSAGE_SENT(mode, destination, profileId, clusterId, sourceEndpoint, destinationEndpoint, options, status) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                         \
                  "BBWWWBBWB",                                                                                                \
                  EM_DEBUG_MESSAGE_SENT,                                                                                      \
                  (mode),                                                                                                     \
                  (destination),                                                                                              \
                  (profileId),                                                                                                \
                  (clusterId),                                                                                                \
                  (sourceEndpoint),                                                                                           \
                  (destinationEndpoint),                                                                                      \
                  (options),                                                                                                  \
                  (status)                                                                                                    \
                  )
#define API_TRACE_INCOMING_MESSAGE_HANDLER(type, profileId, clusterId, sourceEndpoint, destinationEndpoint, options) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                \
                  "BBWWBBW",                                                                                         \
                  EM_DEBUG_INCOMING_MESSAGE_HANDLER,                                                                 \
                  (type),                                                                                            \
                  (profileId),                                                                                       \
                  (clusterId),                                                                                       \
                  (sourceEndpoint),                                                                                  \
                  (destinationEndpoint),                                                                             \
                  (options)                                                                                          \
                  )
#define API_TRACE_STACK_STATUS_HANDLER(stackStatus) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,               \
                  "BB",                             \
                  EM_DEBUG_STACK_STATUS_HANDLER,    \
                  (stackStatus)                     \
                  )
#define API_TRACE_NETWORK_INIT(nodeType) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,    \
                  "BB",                  \
                  EM_DEBUG_NETWORK_INIT, \
                  (nodeType)             \
                  )
#define API_TRACE_FORM_NETWORK(extendedPanId, panId, radioTxPower, radioChannel) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                            \
                  "B8pWBB",                                                      \
                  EM_DEBUG_FORM_NETWORK,                                         \
                  (extendedPanId),                                               \
                  (panId),                                                       \
                  (radioTxPower),                                                \
                  (radioChannel)                                                 \
                  )
#define API_TRACE_JOIN_NETWORK(nodeType, extendedPanId, panId, radioTxPower, radioChannel, joinMethod, nwkManagerId, nwkUpdateId, channels) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                       \
                  "BB8pWBBBWBD",                                                                                                            \
                  EM_DEBUG_JOIN_NETWORK,                                                                                                    \
                  (nodeType),                                                                                                               \
                  (extendedPanId),                                                                                                          \
                  (panId),                                                                                                                  \
                  (radioTxPower),                                                                                                           \
                  (radioChannel),                                                                                                           \
                  (joinMethod),                                                                                                             \
                  (nwkManagerId),                                                                                                           \
                  (nwkUpdateId),                                                                                                            \
                  (channels)                                                                                                                \
                  )
#define API_TRACE_LEAVE_NETWORK( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,    \
                  "B",                   \
                  EM_DEBUG_LEAVE_NETWORK \
                  )
#define API_TRACE_PERMIT_JOINING(duration) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,      \
                  "BB",                    \
                  EM_DEBUG_PERMIT_JOINING, \
                  (duration)               \
                  )
#define API_TRACE_POLL_FOR_DATA( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,    \
                  "B",                   \
                  EM_DEBUG_POLL_FOR_DATA \
                  )
#define API_TRACE_POLL_HANDLER(id, sendAppJitMessage) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                 \
                  "BWB",                              \
                  EM_DEBUG_POLL_HANDLER,              \
                  (id),                               \
                  (sendAppJitMessage)                 \
                  )
#define API_TRACE_TRUST_CENTER_JOIN_HANDLER(status, decision) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                         \
                  "BBB",                                      \
                  EM_DEBUG_TRUST_CENTER_JOIN_HANDLER,         \
                  (status),                                   \
                  (decision)                                  \
                  )
#define API_TRACE_SET_MESSAGE_FLAG(childId)  \
  emDebugApiTrace(EM_DEBUG_API_TRACE,        \
                  "BW",                      \
                  EM_DEBUG_SET_MESSAGE_FLAG, \
                  (childId)                  \
                  )
#define API_TRACE_CLEAR_MESSAGE_FLAG(childId)  \
  emDebugApiTrace(EM_DEBUG_API_TRACE,          \
                  "BW",                        \
                  EM_DEBUG_CLEAR_MESSAGE_FLAG, \
                  (childId)                    \
                  )
#define API_TRACE_POLL_COMPLETE_HANDLER(status)   \
  emDebugApiTrace(EM_DEBUG_API_TRACE,             \
                  "BB",                           \
                  EM_DEBUG_POLL_COMPLETE_HANDLER, \
                  (status)                        \
                  )
#define API_TRACE_CHILD_JOIN_HANDLER(childIndex, joining) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                     \
                  "BBB",                                  \
                  EM_DEBUG_CHILD_JOIN_HANDLER,            \
                  (childIndex),                           \
                  (joining)                               \
                  )
#define API_TRACE_START_SCAN(scanType, channelMask, duration) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                         \
                  "BBDB",                                     \
                  EM_DEBUG_START_SCAN,                        \
                  (scanType),                                 \
                  (channelMask),                              \
                  (duration)                                  \
                  )
#define API_TRACE_STOP_SCAN( )        \
  emDebugApiTrace(EM_DEBUG_API_TRACE, \
                  "B",                \
                  EM_DEBUG_STOP_SCAN  \
                  )
#define API_TRACE_SCAN_COMPLETE_HANDLER(data, status) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                 \
                  "BBB",                              \
                  EM_DEBUG_SCAN_COMPLETE_HANDLER,     \
                  (data),                             \
                  (status)                            \
                  )
#define API_TRACE_NETWORK_FOUND_HANDLER(panId, permitJoin, stackProfile) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                    \
                  "BWBB",                                                \
                  EM_DEBUG_NETWORK_FOUND_HANDLER,                        \
                  (panId),                                               \
                  (permitJoin),                                          \
                  (stackProfile)                                         \
                  )
#define API_TRACE_ENERGY_SCAN_RESULT_HANDLER(channel, rssi) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                       \
                  "BBB",                                    \
                  EM_DEBUG_ENERGY_SCAN_RESULT_HANDLER,      \
                  (channel),                                \
                  (rssi)                                    \
                  )
#define API_TRACE_SET_INITIAL_SECURITY_STATE(mask, preconfiguredKey, networkKey, keySequence) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                         \
                  "BW16p16pB",                                                                \
                  EM_DEBUG_SET_INITIAL_SECURITY_STATE,                                        \
                  (mask),                                                                     \
                  (preconfiguredKey),                                                         \
                  (networkKey),                                                               \
                  (keySequence)                                                               \
                  )
#define API_TRACE_REJOIN_NETWORK(haveKey, channelMask, status) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                          \
                  "BBDB",                                      \
                  EM_DEBUG_REJOIN_NETWORK,                     \
                  (haveKey),                                   \
                  (channelMask),                               \
                  (status)                                     \
                  )
#define API_TRACE_STACK_POWER_DOWN( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,       \
                  "B",                      \
                  EM_DEBUG_STACK_POWER_DOWN \
                  )
#define API_TRACE_STACK_POWER_UP( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,     \
                  "B",                    \
                  EM_DEBUG_STACK_POWER_UP \
                  )
#define API_TRACE_SET_EXTENDED_SECURITY_BITMASK(mask)     \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                     \
                  "BW",                                   \
                  EM_DEBUG_SET_EXTENDED_SECURITY_BITMASK, \
                  (mask)                                  \
                  )
#define API_TRACE_RF4CE_SET_PAIRING_TABLE_ENTRY(pairingIndex, securityLinkKey, destLongId, frameCounter, sourceNodeId, destPanId, destNodeId, destVendorId, destProfileIdListLength, info, channel, capabilities, lastSeqn) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                                                                                                       \
                  "BB16p8pDWWWWBBBBB",                                                                                                                                                                                      \
                  EM_DEBUG_RF4CE_SET_PAIRING_TABLE_ENTRY,                                                                                                                                                                   \
                  (pairingIndex),                                                                                                                                                                                           \
                  (securityLinkKey),                                                                                                                                                                                        \
                  (destLongId),                                                                                                                                                                                             \
                  (frameCounter),                                                                                                                                                                                           \
                  (sourceNodeId),                                                                                                                                                                                           \
                  (destPanId),                                                                                                                                                                                              \
                  (destNodeId),                                                                                                                                                                                             \
                  (destVendorId),                                                                                                                                                                                           \
                  (destProfileIdListLength),                                                                                                                                                                                \
                  (info),                                                                                                                                                                                                   \
                  (channel),                                                                                                                                                                                                \
                  (capabilities),                                                                                                                                                                                           \
                  (lastSeqn)                                                                                                                                                                                                \
                  )
#define API_TRACE_RF4CE_GET_PAIRING_TABLE_ENTRY(pairingIndex) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                         \
                  "BB",                                       \
                  EM_DEBUG_RF4CE_GET_PAIRING_TABLE_ENTRY,     \
                  (pairingIndex)                              \
                  )
#define API_TRACE_RF4CE_DELETE_PAIRING_TABLE_ENTRY(pairingIndex) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                            \
                  "BB",                                          \
                  EM_DEBUG_RF4CE_DELETE_PAIRING_TABLE_ENTRY,     \
                  (pairingIndex)                                 \
                  )
#define API_TRACE_RF4CE_SET_APPLICATION_INFO(capabilities) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                      \
                  "BB",                                    \
                  EM_DEBUG_RF4CE_SET_APPLICATION_INFO,     \
                  (capabilities)                           \
                  )
#define API_TRACE_RF4CE_GET_APPLICATION_INFO( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                 \
                  "B",                                \
                  EM_DEBUG_RF4CE_GET_APPLICATION_INFO \
                  )
#define API_TRACE_RF4CE_KEY_UPDATE(pairingIndex, key) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                 \
                  "BB16p",                            \
                  EM_DEBUG_RF4CE_KEY_UPDATE,          \
                  (pairingIndex),                     \
                  (key)                               \
                  )
#define API_TRACE_RF4CE_SEND(pairingIndex, profileId, vendorId, txOptions, messageTag) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                  \
                  "BBBWBB",                                                            \
                  EM_DEBUG_RF4CE_SEND,                                                 \
                  (pairingIndex),                                                      \
                  (profileId),                                                         \
                  (vendorId),                                                          \
                  (txOptions),                                                         \
                  (messageTag)                                                         \
                  )
#define API_TRACE_RF4CE_MESSAGE_SENT_HANDLER(status, pairingIndex, txOptions, profileId, vendorId, messageTag) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                          \
                  "BBBBBWB",                                                                                   \
                  EM_DEBUG_RF4CE_MESSAGE_SENT_HANDLER,                                                         \
                  (status),                                                                                    \
                  (pairingIndex),                                                                              \
                  (txOptions),                                                                                 \
                  (profileId),                                                                                 \
                  (vendorId),                                                                                  \
                  (messageTag)                                                                                 \
                  )
#define API_TRACE_RF4CE_INCOMING_MESSAGE_HANDLER(pairingIndex, profileId, vendorId, txOptions) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                          \
                  "BBBWB",                                                                     \
                  EM_DEBUG_RF4CE_INCOMING_MESSAGE_HANDLER,                                     \
                  (pairingIndex),                                                              \
                  (profileId),                                                                 \
                  (vendorId),                                                                  \
                  (txOptions)                                                                  \
                  )
#define API_TRACE_RF4CE_START(nodeCapabilities, vendorId, power) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                            \
                  "BBWB",                                        \
                  EM_DEBUG_RF4CE_START,                          \
                  (nodeCapabilities),                            \
                  (vendorId),                                    \
                  (power)                                        \
                  )
#define API_TRACE_RF4CE_STOP( )       \
  emDebugApiTrace(EM_DEBUG_API_TRACE, \
                  "B",                \
                  EM_DEBUG_RF4CE_STOP \
                  )
#define API_TRACE_RF4CE_DISCOVERY(panId, nodeId, searchDevType, discDuration, maxDiscRepetitions) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                             \
                  "BWWBWB",                                                                       \
                  EM_DEBUG_RF4CE_DISCOVERY,                                                       \
                  (panId),                                                                        \
                  (nodeId),                                                                       \
                  (searchDevType),                                                                \
                  (discDuration),                                                                 \
                  (maxDiscRepetitions)                                                            \
                  )
#define API_TRACE_RF4CE_DISCOVERY_COMPLETE_HANDLER(status)   \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                        \
                  "BB",                                      \
                  EM_DEBUG_RF4CE_DISCOVERY_COMPLETE_HANDLER, \
                  (status)                                   \
                  )
#define API_TRACE_RF4CE_DISCOVERY_REQUEST_HANDLER(srcIeeeAddr, nodeCapabilities, vendorId, appCapabilities, searchDevType, rxLinkQuality) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                     \
                  "B8pBWBBB",                                                                                                             \
                  EM_DEBUG_RF4CE_DISCOVERY_REQUEST_HANDLER,                                                                               \
                  (srcIeeeAddr),                                                                                                          \
                  (nodeCapabilities),                                                                                                     \
                  (vendorId),                                                                                                             \
                  (appCapabilities),                                                                                                      \
                  (searchDevType),                                                                                                        \
                  (rxLinkQuality)                                                                                                         \
                  )
#define API_TRACE_RF4CE_DISCOVERY_RESPONSE_HANDLER(atCapacity, channel, panId, srcIeeeAddr, nodeCapabilities, vendorId, appCapabilities, rxLinkQuality, discRequestLqi) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                                                   \
                  "BBBW8pBWBBB",                                                                                                                                        \
                  EM_DEBUG_RF4CE_DISCOVERY_RESPONSE_HANDLER,                                                                                                            \
                  (atCapacity),                                                                                                                                         \
                  (channel),                                                                                                                                            \
                  (panId),                                                                                                                                              \
                  (srcIeeeAddr),                                                                                                                                        \
                  (nodeCapabilities),                                                                                                                                   \
                  (vendorId),                                                                                                                                           \
                  (appCapabilities),                                                                                                                                    \
                  (rxLinkQuality),                                                                                                                                      \
                  (discRequestLqi)                                                                                                                                      \
                  )
#define API_TRACE_RF4CE_ENABLE_AUTO_DISCOVERY_RESPONSE(duration) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                            \
                  "BW",                                          \
                  EM_DEBUG_RF4CE_ENABLE_AUTO_DISCOVERY_RESPONSE, \
                  (duration)                                     \
                  )
#define API_TRACE_RF4CE_AUTO_DISCOVERY_RESPONSE_COMPLETE_HANDLER(status, srcIeeeAddr, nodeCapabilities, vendorId, appCapabilities, searchDevType) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                             \
                  "BB8pBWBB",                                                                                                                     \
                  EM_DEBUG_RF4CE_AUTO_DISCOVERY_RESPONSE_COMPLETE_HANDLER,                                                                        \
                  (status),                                                                                                                       \
                  (srcIeeeAddr),                                                                                                                  \
                  (nodeCapabilities),                                                                                                             \
                  (vendorId),                                                                                                                     \
                  (appCapabilities),                                                                                                              \
                  (searchDevType)                                                                                                                 \
                  )
#define API_TRACE_RF4CE_PAIR(channel, panId, ieeeAddr, keyExchangeTransferCount) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                            \
                  "BBW8pB",                                                      \
                  EM_DEBUG_RF4CE_PAIR,                                           \
                  (channel),                                                     \
                  (panId),                                                       \
                  (ieeeAddr),                                                    \
                  (keyExchangeTransferCount)                                     \
                  )
#define API_TRACE_RF4CE_PAIR_COMPLETE_HANDLER(status, pairingIndex, vendorId, appCapabilities) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                          \
                  "BBBWB",                                                                     \
                  EM_DEBUG_RF4CE_PAIR_COMPLETE_HANDLER,                                        \
                  (status),                                                                    \
                  (pairingIndex),                                                              \
                  (vendorId),                                                                  \
                  (appCapabilities)                                                            \
                  )
#define API_TRACE_RF4CE_PAIR_REQUEST_HANDLER(status, pairingIndex, srcIeeeAddr, nodeCapabilities, vendorId, appCapabilities, keyExchangeTransferCount) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                                  \
                  "BBB8pBWBB",                                                                                                                         \
                  EM_DEBUG_RF4CE_PAIR_REQUEST_HANDLER,                                                                                                 \
                  (status),                                                                                                                            \
                  (pairingIndex),                                                                                                                      \
                  (srcIeeeAddr),                                                                                                                       \
                  (nodeCapabilities),                                                                                                                  \
                  (vendorId),                                                                                                                          \
                  (appCapabilities),                                                                                                                   \
                  (keyExchangeTransferCount)                                                                                                           \
                  )
#define API_TRACE_RF4CE_UNPAIR(pairingIndex) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,        \
                  "BB",                      \
                  EM_DEBUG_RF4CE_UNPAIR,     \
                  (pairingIndex)             \
                  )
#define API_TRACE_RF4CE_UNPAIR_HANDLER(pairingIndex) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                \
                  "BB",                              \
                  EM_DEBUG_RF4CE_UNPAIR_HANDLER,     \
                  (pairingIndex)                     \
                  )
#define API_TRACE_RF4CE_UNPAIR_COMPLETE_HANDLER(pairingIndex) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                         \
                  "BB",                                       \
                  EM_DEBUG_RF4CE_UNPAIR_COMPLETE_HANDLER,     \
                  (pairingIndex)                              \
                  )
#define API_TRACE_RF4CE_SET_POWER_SAVING_PARAMETERS(dutyCycle, activePeriod) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                        \
                  "BDD",                                                     \
                  EM_DEBUG_RF4CE_SET_POWER_SAVING_PARAMETERS,                \
                  (dutyCycle),                                               \
                  (activePeriod)                                             \
                  )
#define API_TRACE_RF4CE_SET_FREQUENCY_AGILITY_PARAMETERS(rssiWindowSize, channelChangeReads, rssiThreshold, readInterval, readDuration) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                                                                                                   \
                  "BBBBWB",                                                                                                             \
                  EM_DEBUG_RF4CE_SET_FREQUENCY_AGILITY_PARAMETERS,                                                                      \
                  (rssiWindowSize),                                                                                                     \
                  (channelChangeReads),                                                                                                 \
                  (rssiThreshold),                                                                                                      \
                  (readInterval),                                                                                                       \
                  (readDuration)                                                                                                        \
                  )
#define API_TRACE_RF4CE_SET_DISCOVERY_LQI_THRESHOLD(threshold) \
  emDebugApiTrace(EM_DEBUG_API_TRACE,                          \
                  "BB",                                        \
                  EM_DEBUG_RF4CE_SET_DISCOVERY_LQI_THRESHOLD,  \
                  (threshold)                                  \
                  )
#endif // SILABS_ZIGBEE_DEBUG_CHANNEL_H
