/***************************************************************************//**
 * @file
 * @brief EZSP utility library. These functions are provided to
 * make it easier to port applications from the environment where the
 * Ember stack and application where on a single processor to an
 * environment where the stack is running on an Ember radio chip and
 * the application is running on a separte host processor and
 * utilizing the EZSP library.
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

#ifndef SILABS_EZSP_UTILS_H
#define SILABS_EZSP_UTILS_H

//----------------------------------------------------------------
// Zigbee Simple Descriptor:

/** @brief Endpoint information (a ZigBee Simple Descriptor)
 * @description This is a ZigBee Simple Descriptor and contains information
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

/** @brief Gives the endpoint information for a particular endpoint.
 * @description Gives the endpoint information for a particular endpoint.
 */

typedef struct {
  /** An endpoint of the application on this node. */
  uint8_t endpoint;
  /** The endpoint's description. */
  const EmberEndpointDescription *description;
  /** Input clusters the endpoint will accept. */
  const uint16_t* inputClusterList;
  /** Output clusters the endpoint may send. */
  const uint16_t* outputClusterList;
} EmberEndpoint;

extern uint8_t emberEndpointCount;
extern EmberEndpoint emberEndpoints[];

/** @description Defines config parameter incompatibilities between the
 *   host and node
 */
enum {
  EZSP_UTIL_INCOMPATIBLE_PROTOCOL_VERSION                   = 0x00000001,
  EZSP_UTIL_INCOMPATIBLE_STACK_ID                           = 0x00000002,
  EZSP_UTIL_INCOMPATIBLE_PACKET_BUFFER_COUNT                = 0x00000008,
  EZSP_UTIL_INCOMPATIBLE_NEIGHBOR_TABLE_SIZE                = 0x00000010,
  EZSP_UTIL_INCOMPATIBLE_APS_UNICAST_MESSAGE_COUNT          = 0x00000020,
  EZSP_UTIL_INCOMPATIBLE_BINDING_TABLE_SIZE                 = 0x00000040,
  EZSP_UTIL_INCOMPATIBLE_ADDRESS_TABLE_SIZE                 = 0x00000080,
  EZSP_UTIL_INCOMPATIBLE_MULTICAST_TABLE_SIZE               = 0x00000100,
  EZSP_UTIL_INCOMPATIBLE_ROUTE_TABLE_SIZE                   = 0x00000200,
  EZSP_UTIL_INCOMPATIBLE_DISCOVERY_TABLE_SIZE               = 0x00000400,
  EZSP_UTIL_INCOMPATIBLE_BROADCAST_ALARM_DATA_SIZE          = 0x00000800,
  EZSP_UTIL_INCOMPATIBLE_UNICAST_ALARM_DATA_SIZE            = 0x00001000,
  EZSP_UTIL_INCOMPATIBLE_STACK_PROFILE                      = 0x00004000,
  EZSP_UTIL_INCOMPATIBLE_SECURITY_LEVEL                     = 0x00008000,
  EZSP_UTIL_INCOMPATIBLE_MAX_HOPS                           = 0x00040000,
  EZSP_UTIL_INCOMPATIBLE_MAX_END_DEVICE_CHILDREN            = 0x00080000,
  EZSP_UTIL_INCOMPATIBLE_INDIRECT_TRANSMISSION_TIMEOUT      = 0x00100000,
  EZSP_UTIL_INCOMPATIBLE_END_DEVICE_POLL_TIMEOUT            = 0x00200000,
  EZSP_UTIL_INCOMPATIBLE_FRAGMENT_WINDOW_SIZE               = 0x04000000,
  EZSP_UTIL_INCOMPATIBLE_FRAGMENT_DELAY_MS                  = 0x08000000,
  EZSP_UTIL_INCOMPATIBLE_KEY_TABLE_SIZE                     = 0x10000000,
  EZSP_UTIL_INCOMPATIBLE_NUMBER_OF_SUPPORTED_NETWORKS       = 0x20000000,
};

// Replacement for SOFTWARE_VERSION
extern uint16_t ezspUtilStackVersion;

// Replacement function for emberInit - checks configuration, sets the
// stack profile, and registers endpoints.
EmberStatus ezspUtilInit(uint8_t serialPort);

void emberTick(void);

extern EmberEUI64 emLocalEui64;
// The ezsp util library keeps track of the local node's EUI64 and
// short id so that there isn't the need for communication with the
// Ember radio node every time these values are queried.
#define emberGetEui64() (emLocalEui64)
#define emberIsLocalEui64(eui64) \
  (MEMCOMPARE(eui64, emLocalEui64, EUI64_SIZE) == 0)

extern uint8_t emberBindingTableSize;
extern uint8_t emberTemporaryBindingEntries;

 #define emberFetchLowHighInt16u(contents) \
  (HIGH_LOW_TO_INT((contents)[1], (contents)[0]))
 #define emberFetchHighLowInt16u(contents) \
  (HIGH_LOW_TO_INT((contents)[0], (contents)[1]))
 #define emberStoreLowHighInt16u(contents, value) \
  do {                                            \
    (contents)[0] = LOW_BYTE(value);              \
    (contents)[1] = HIGH_BYTE(value);             \
  } while (0)
 #define emberFetchLowHighInt32u(contents) \
  ((contents)[0]                           \
   + (((uint32_t)(contents)[1]) << 8)      \
   + (((uint32_t)(contents)[2]) << 16)     \
   + (((uint32_t)(contents)[3]) << 24))
 #define emberFetchHighLowInt32u(contents) \
  ((contents)[3]                           \
   + (((uint32_t)(contents)[2]) << 8)      \
   + (((uint32_t)(contents)[1]) << 16)     \
   + (((uint32_t)(contents)[0]) << 24))
 #define emberStoreLowHighInt32u(contents, value) \
  do {                                            \
    (contents)[0] = BYTE_0(value);                \
    (contents)[1] = BYTE_1(value);                \
    (contents)[2] = BYTE_2(value);                \
    (contents)[3] = BYTE_3(value);                \
  } while (0)

// For back-compatibility
#define emberRejoinNetwork(haveKey) emberFindAndRejoinNetwork((haveKey), 0)

// For back-compatibility
#define ezspReset() ezspInit()

/** @brief Broadcasts a request to set the identity of the network manager and
 * the active channel mask.  The mask is used when scanning
 *  for the network after missing a channel update.
 *
 * @param networkManager   The network address of the network manager.
 * @param activeChannels   The new active channel mask.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_NO_BUFFERS
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_NETWORK_BUSY
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberSetNetworkManagerRequest(EmberNodeId networkManager,
                                          uint32_t activeChannels);
#else
#define emberSetNetworkManagerRequest(manager, channels)  \
  (emberEnergyScanRequest(EMBER_SLEEPY_BROADCAST_ADDRESS, \
                          (channels),                     \
                          0xFF,                           \
                          (manager)))
#endif

/** @brief Broadcasts a request to change the channel.  This request may
 * only be sent by the current network manager.  There is a delay of
 * several seconds from receipt of the broadcast to changing the channel,
 * to allow time for the broadcast to propagate.
 *
 * @param channel  The channel to change to.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_NO_BUFFERS
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_NETWORK_BUSY
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberChannelChangeRequest(uint8_t channel)
#else
#define emberChannelChangeRequest(channel)                \
  (emberEnergyScanRequest(EMBER_SLEEPY_BROADCAST_ADDRESS, \
                          BIT32(channel),                 \
                          0xFE,                           \
                          0))
#endif

/** @brief Function to set NWK layer outgoing frame counter
 *    (intended for device restoration purposes).  Caveats:
 *    - Can only be called before NetworkInit / FormNetwork / JoinNetwork,
 *      when emberNetworkState()==EMBER_NO_NETWORK.
 *    - This function should be called before ::emberSetInitialSecurityState,
 *      and the EMBER_NO_FRAME_COUNTER_RESET bitmask should be added to the
 *      initial security bitmask when ::emberSetInitialSecuritState is called.
 *    - If used in multi-network context, be sure to call
 *      ::emberSetCurrentNetwork() prior to calling this function.
 *
 * @param desiredValue The desired outgoing NWK frame counter value.  This
 *   should needs to be less than MAX_INT32U_VALUE to ensure that rollover
 *   does not occur on the next encrypted transmission.
 *
 * @return ::EMBER_SUCCESS if calling context is valid (emberNetworkState()
 *   == EMBER_NO_NETWORK) and desiredValue < MAX_INT32U_VALUE. Otherwise,
 *   ::EMBER_INVALID_CALL.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberSetOutgoingNwkFrameCounter(uint32_t* desiredValuePtr);
#else
  #define emberSetOutgoingNwkFrameCounter(desiredValuePtr) \
  ezspSetValue(EZSP_VALUE_NWK_FRAME_COUNTER, sizeof(desiredValuePtr), desiredValuePtr)
#endif

/** @brief Function to set APS layer outgoing frame counter for Trust Center
 *    Link Key (intended for device restoration purposes).  Caveats:
 *    - Can only be called before NetworkInit / FormNetwork / JoinNetwork,
 *      when emberNetworkState()==EMBER_NO_NETWORK.
 *    - This function should be called before ::emberSetInitialSecurityState,
 *      and the EMBER_NO_FRAME_COUNTER_RESET bitmask should be added to the
 *      initial security bitmask when ::emberSetInitialSecuritState is called.
 *    - If used in multi-network context, be sure to call
 *      ::emberSetCurrentNetwork() prior to calling this function.
 *
 * @param desiredValue The desired outgoing APS frame counter value.  This
 *   should needs to be less than MAX_INT32U_VALUE to ensure that rollover
 *   does not occur on the next encrypted transmission.
 *
 * @return ::EMBER_SUCCESS if calling context is valid (emberNetworkState()
 *   == EMBER_NO_NETWORK) and desiredValue < MAX_INT32U_VALUE. Otherwise,
 *   ::EMBER_INVALID_CALL.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberSetOutgoingApsFrameCounter(uint32_t* desiredValuePtr);
#else
  #define emberSetOutgoingApsFrameCounter(desiredValuePtr) \
  ezspSetValue(EZSP_VALUE_APS_FRAME_COUNTER, sizeof(desiredValuePtr), desiredValuePtr)
#endif

#endif // __EZSP_UTILS_H__
