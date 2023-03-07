/***************************************************************************//**
 * @file
 * @brief Ember data type definitions.
 *
 *  See @ref zll_types for details.
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

#ifndef SILABS_ZLL_TYPES_H
#define SILABS_ZLL_TYPES_H

/**
 * @addtogroup zll_types
 *
 * See zll-types.h for source code.
 * @{
 */

/**
 * @name ZigBee Light Link Types
 */
//@{

/**
 * @brief The list of primary ZLL channels.
 */
#define EMBER_ZLL_PRIMARY_CHANNEL_MASK ((uint32_t)(BIT32(11)   \
                                                   | BIT32(15) \
                                                   | BIT32(20) \
                                                   | BIT32(25)))

/**
 * @brief The list of secondary ZLL channels.
 */
#define EMBER_ZLL_SECONDARY_CHANNEL_MASK ((uint32_t)(BIT32(12)   \
                                                     | BIT32(13) \
                                                     | BIT32(14) \
                                                     | BIT32(16) \
                                                     | BIT32(17) \
                                                     | BIT32(18) \
                                                     | BIT32(19) \
                                                     | BIT32(21) \
                                                     | BIT32(22) \
                                                     | BIT32(23) \
                                                     | BIT32(24) \
                                                     | BIT32(26)))

/**
 * @brief A distinguished network identifier in the ZLL network address space
 * that indicates no free network identifiers were assigned to the device.
 */
#define EMBER_ZLL_NULL_NODE_ID 0x0000

/**
 * @brief The minimum network identifier in the ZLL network address space.
 */
#define EMBER_ZLL_MIN_NODE_ID 0x0001

/**
 * @brief The maximum network identifier in the ZLL network address space.
 */
#define EMBER_ZLL_MAX_NODE_ID 0xFFF7

/**
 * @brief A distinguished group identifier in the ZLL group address space that
 * indicates no free group identifiers were assigned to the device.
 */
#define EMBER_ZLL_NULL_GROUP_ID 0x0000

/**
 * @brief The minimum group identifier in the ZLL group address space.
 */
#define EMBER_ZLL_MIN_GROUP_ID 0x0001

/**
 * @brief The maximum group identifier in the ZLL group address space.
 */
#define EMBER_ZLL_MAX_GROUP_ID 0xFEFF

/**
 * @brief A bitmask indicating the state of the ZLL device.
 *    This maps directly to the ZLL information field in the scan response.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberZllState
#else
typedef uint16_t EmberZllState;
enum
#endif
{
  /** No state. */
  EMBER_ZLL_STATE_NONE                       = 0x0000,
  /** The device is factory new. */
  EMBER_ZLL_STATE_FACTORY_NEW                = 0x0001,
  /** The device is capable of assigning addresses to other devices. */
  EMBER_ZLL_STATE_ADDRESS_ASSIGNMENT_CAPABLE = 0x0002,
  /** The device is initiating a link operation. */
  EMBER_ZLL_STATE_LINK_INITIATOR             = 0x0010,
  /** The device is requesting link priority. */
  EMBER_ZLL_STATE_LINK_PRIORITY_REQUEST      = 0x0020,
  /** The device is a ZigBee 3.0 device. */
  EMBER_ZLL_STATE_PROFILE_INTEROP            = 0x0080,
  /** The device is on a non-ZLL network. */
  EMBER_ZLL_STATE_NON_ZLL_NETWORK            = 0x0100,
};

/**
 * @brief Information about the ZLL security state and how to transmit
 *    the network key to the device securely.
 */
typedef struct {
  uint32_t transactionId;
  uint32_t responseId;
  uint16_t bitmask;
} EmberZllSecurityAlgorithmData;

/**
 * @brief Information about the ZLL network and specific device
 *   that responded to a ZLL scan request.
 */
typedef struct {
  EmberZigbeeNetwork zigbeeNetwork;
  EmberZllSecurityAlgorithmData securityAlgorithm;
  EmberEUI64 eui64;
  EmberNodeId nodeId;
  EmberZllState state;
  EmberNodeType nodeType;
  uint8_t numberSubDevices;
  uint8_t totalGroupIdentifiers;
  uint8_t rssiCorrection;
} EmberZllNetwork;

/**
 * @brief Information discovered during a ZLL scan about the ZLL device's
 *   endpoint information.
 */
typedef struct {
  EmberEUI64 ieeeAddress;
  uint8_t endpointId;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t version;
  uint8_t groupIdCount;
} EmberZllDeviceInfoRecord;

/**
 * @brief Network and group address assignment information.
 */
typedef struct {
  EmberNodeId nodeId;
  EmberNodeId freeNodeIdMin;
  EmberNodeId freeNodeIdMax;
  EmberMulticastId groupIdMin;
  EmberMulticastId groupIdMax;
  EmberMulticastId freeGroupIdMin;
  EmberMulticastId freeGroupIdMax;
} EmberZllAddressAssignment;

/**
 * @brief The ZigBee Light Link Commissioning cluster ID.
 */
#define EMBER_ZLL_CLUSTER_ID 0x1000

/**
 * @brief The ZigBee Light Link Profile ID.
 */
#define EMBER_ZLL_PROFILE_ID 0xC05E

/**
 * @brief The key encryption algorithms supported by the stack.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberZllKeyIndex
#else
typedef uint8_t EmberZllKeyIndex;
enum
#endif
{
  /** The key encryption algorithm for use during development. */
  EMBER_ZLL_KEY_INDEX_DEVELOPMENT   = 0x00,
  /** The key encryption algorithm shared by all certified devices. */
  EMBER_ZLL_KEY_INDEX_MASTER        = 0x04,
  /** The key encryption algorithm for use during development and certification. */
  EMBER_ZLL_KEY_INDEX_CERTIFICATION = 0x0F,
};

/**
 * @brief The key encryption bitmask corresponding to encryption key index
 * ::EMBER_ZLL_KEY_INDEX_DEVELOPMENT.
 */
#define EMBER_ZLL_KEY_MASK_DEVELOPMENT (1 << EMBER_ZLL_KEY_INDEX_DEVELOPMENT)

/**
 * @brief The key encryption bitmask corresponding to encryption key index
 * ::EMBER_ZLL_KEY_INDEX_MASTER.
 */
#define EMBER_ZLL_KEY_MASK_MASTER (1 << EMBER_ZLL_KEY_INDEX_MASTER)

/**
 * @brief THe key encryption bitmask corresponding to encryption key index
 * ::EMBER_ZLL_KEY_INDEX_CERTIFICATION.
 */
#define EMBER_ZLL_KEY_MASK_CERTIFICATION (1 << EMBER_ZLL_KEY_INDEX_CERTIFICATION)

/**
 * @brief The encryption key for use during development and certification in
 * conjunction with ::EMBER_ZLL_KEY_INDEX_CERTIFICATION.
 */
#define EMBER_ZLL_CERTIFICATION_ENCRYPTION_KEY      \
  { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, \
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }

/**
 * @brief The pre-configured link key for use during development and certification
 * in conjunction with ::EMBER_ZLL_KEY_INDEX_CERTIFICATION.
 */
#define EMBER_ZLL_CERTIFICATION_PRECONFIGURED_LINK_KEY \
  { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,    \
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF }

/**
 * @brief Describe the Initial Security features and requirements that
 * will be used when forming or joining ZigBee Light Link networks.
 */
typedef struct {
  /** This bitmask is unused.  All values are reserved for future use. */
  uint32_t bitmask;
  /** The key encryption algorithm advertised by the application. */
  EmberZllKeyIndex keyIndex;
  /** The encryption key for use by algorithms that require it. */
  EmberKeyData encryptionKey;
  /** The pre-configured link key used during classical ZigBee commissioning. */
  EmberKeyData preconfiguredKey;
} EmberZllInitialSecurityState;

/** @brief This enumeration indicates whether or not the stack processes ZLL
 * messages.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberZllPolicy
#else
typedef uint8_t EmberZllPolicy;
enum
#endif
{
  /** Initial state - all touchlinking is disabled. */
  EMBER_ZLL_POLICY_DISABLED = 0x00,
  /** Indicates that ZLL processing is enabled. */
  EMBER_ZLL_POLICY_ENABLED = 0x01,
  /** Indicates that ZLL target is enabled. */
  EMBER_ZLL_POLICY_TARGET = 0x02,
  /** Indicates that stealing and network update are enabled for non factory new devices. */
  EMBER_ZLL_POLICY_STEALING_ENABLED = 0x04,
  /** Indicates that remote reset is enabled for non factory new devices. */
  EMBER_ZLL_POLICY_REMOTE_RESET_ENABLED = 0x08,
};

//@} \\END ZigBee Light Link Types

typedef struct {
  uint32_t bitmask;
  uint16_t freeNodeIdMin;
  uint16_t freeNodeIdMax;
  uint16_t myGroupIdMin;
  uint16_t freeGroupIdMin;
  uint16_t freeGroupIdMax;
  uint8_t rssiCorrection;
} EmberTokTypeStackZllData;

typedef struct {
  uint32_t bitmask;
  uint8_t keyIndex;
  uint8_t encryptionKey[EMBER_ENCRYPTION_KEY_SIZE];
  uint8_t preconfiguredKey[EMBER_ENCRYPTION_KEY_SIZE];
} EmberTokTypeStackZllSecurity;

/** @} END addtogroup
 */

#endif // SILABS_ZLL_TYPES_H
