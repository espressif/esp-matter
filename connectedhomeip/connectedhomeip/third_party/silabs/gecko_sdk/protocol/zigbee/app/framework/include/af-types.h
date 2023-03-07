/***************************************************************************//**
 * @file
 * @brief The include file for all the types for Ember ApplicationFramework
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

/** @addtogroup aftypes Application Framework Types Reference
 * This documentation describes the types used by the Zigbee
 * Application Framework.
 * @{
 */

#ifndef SILABS_AF_API_TYPES
#define SILABS_AF_API_TYPES

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "zap-type.h"
#include "zigbee_app_framework_common.h"
#ifdef SL_CATALOG_ZIGBEE_MESSAGING_CLIENT_PRESENT
#include "messaging-client-config.h"
#endif  // SL_CATALOG_ZIGBEE_MESSAGING_CLIENT_PRESENT
#else
#include "enums.h"
#endif

#ifdef EZSP_HOST
#include "app/util/ezsp/ezsp-enum.h"
#endif

/**
 * @brief Refer to the Zigbee application profile ID
 */
typedef uint16_t EmberAfProfileId;

/**
 * @brief Refer to the ZCL attribute ID
 */
typedef uint16_t EmberAfAttributeId;

/**
 * @brief Refer to the ZCL cluster ID
 */
typedef uint16_t EmberAfClusterId;

/**
 * @brief Refer to the ZCL attribute type
 */
typedef uint8_t EmberAfAttributeType;

/**
 * @brief User for the cluster mask
 */
typedef uint8_t EmberAfClusterMask;

/**
 * @brief User for the attribute mask
 */
typedef uint8_t EmberAfAttributeMask;

/**
 * @brief Use for either of the cluster functions.
 *
 * This type is used for the array of the cluster functions and should
 * always be cast into one of the specific functions before being called.
 */
typedef void (*EmberAfGenericClusterFunction)(void);

/**
 * @brief Cluster function-generated array member type.
 *
 * This type is used for the generated array of cluster functions. It carries
 * the info that will be used to construct the cluster->function array at
 * runtime.
 */
typedef struct {
  EmberAfClusterId                    clusterId;
  EmberAfClusterMask                  functionMask;
  const EmberAfGenericClusterFunction functionPtr;
} emberClusterFunctionStructure;

/**
 * @brief A distinguished manufacturer code that is used to indicate the
 * absence of a manufacturer-specific profile, cluster, command, or attribute.
 */
#define EMBER_AF_NULL_MANUFACTURER_CODE 0x0000u

/**
 * @brief An invalid profile ID
 * This is a reserved profileId.
 */
 #define EMBER_AF_INVALID_PROFILE_ID 0xFFFFu

/**
 * @brief Use for default values.
 *
 * Default value is either a value itself, if it is 2 bytes or less,
 * or a pointer to the value itself, if attribute type is longer than
 * 2 bytes.
 */
typedef union {
  /**
   * Point to data if size is more than 2 bytes.
   * If size is more than 2 bytes and this value is NULL,
   * the default value is all zeroes.
   */
  uint8_t *ptrToDefaultValue;
  /**
   * Actual default value if the attribute size is 2 bytes or less.
   */
  uint16_t defaultValue;
} EmberAfDefaultAttributeValue;

/**
 * @brief Describe the attribute default, min, and max values.
 *
 * This struct is required if the attribute mask specifies that this
 * attribute has a known min and max values.
 */
typedef struct {
  /**
   * Default value of the attribute.
   */
  EmberAfDefaultAttributeValue defaultValue;
  /**
   * Minimum allowed value.
   */
  EmberAfDefaultAttributeValue minValue;
  /**
   * Maximum allowed value.
   */
  EmberAfDefaultAttributeValue maxValue;
} EmberAfAttributeMinMaxValue;

/**
 * @brief Union describing the attribute default/min/max values.
 */
typedef union {
  /**
   * Point to data if size is more than 2 bytes.
   * If size is more than 2 bytes, and this value is NULL,
   * then the default value is all zeroes.
   */
  uint8_t *ptrToDefaultValue;
  /**
   * Actual default value if the attribute size is 2 bytes or less.
   */
  uint16_t defaultValue;
  /**
   * Point to the min max attribute value structure, if min/max is
   * supported for this attribute.
   */
  EmberAfAttributeMinMaxValue *ptrToMinMaxValue;
} EmberAfDefaultOrMinMaxAttributeValue;

/**
 * @brief Each attribute has its metadata stored in the struct.
 *
 * There is only one of these per attribute across all endpoints.
 */
typedef struct {
  /**
   * Attribute ID, according to the ZCL specs.
   */
  EmberAfAttributeId attributeId;
  /**
   * Attribute type, according to the ZCL specs.
   */
  EmberAfAttributeType attributeType;
  /**
   * Size of this attribute in bytes.
   */
  uint8_t size;
  /**
   * Attribute mask, which tags the attribute with a specific
   * functionality. See ATTRIBUTE_MASK_ macros defined
   * in af-storage.h.
   */
  EmberAfAttributeMask mask;
  /**
   * Pointer to the default value union. The actual value stored
   * depends on the mask.
   */
  EmberAfDefaultOrMinMaxAttributeValue  defaultValue;
} EmberAfAttributeMetadata;

/**
 * @brief Struct describing a cluster
 */
typedef struct {
  /**
   *  ID of the cluster according to the ZCL spec.
   */
  EmberAfClusterId clusterId;
  /**
   * Pointer to attribute metadata array for this cluster.
   */
  EmberAfAttributeMetadata *attributes;
  /**
   * Total number of attributes.
   */
  uint16_t attributeCount;
  /**
   * Total size of non-external, non-singleton, attribute for this cluster.
   */
  uint16_t clusterSize;
  /**
   * Mask with additional functionality for the cluster. See CLUSTER_MASK
   * macros.
   */
  EmberAfClusterMask mask;

  /**
   * An array into the cluster functions. The length of the array
   * is determined by the function bits in mask. This may be null
   * if this cluster has no functions.
   */
  const EmberAfGenericClusterFunction *functions;
} EmberAfCluster;

/**
 * @brief Find an attribute in storage. Together, the elements
 * in this search record constitute the "primary key" used to identify a unique
 * attribute value in the attribute storage.
 */
typedef struct {
  /**
   * Endpoint that the attribute is located on.
   */
  uint8_t endpoint;

  /**
   * Cluster that the attribute is located on. If the cluster
   * ID is inside the manufacturer specific range, 0xfc00 - 0xffff.
   * The manufacturer code should also be set to the code associated
   * with the manufacturer-specific cluster.
   */
  EmberAfClusterId clusterId;

  /**
   * Cluster mask for the cluster, used to determine if it is
   * the server or client version of the cluster. See CLUSTER_MASK_
   * macros defined in af-storage.h
   */
  EmberAfClusterMask clusterMask;

  /**
   * The two byte identifier for the attribute. If the cluster ID is
   * inside the manufacturer-specific range 0xfc00 - 0xffff, or the manufacturer
   * code is NOT 0, the attribute is assumed to be manufacturer specific.
   */
  EmberAfAttributeId attributeId;

  /**
   * Manufacturer Code associated with the cluster and or attribute.
   * If the cluster ID is inside the manufacturer-specific
   * range, this value should indicate the manufacturer code for the
   * manufacturer-specific cluster. Otherwise, if this value is non zero
   * and the cluster ID is a standard ZCL cluster,
   * it is assumed that the attribute being sought is a manufacturer-specific
   * extension to the standard ZCL cluster indicated by the cluster ID.
   */
  uint16_t manufacturerCode;
} EmberAfAttributeSearchRecord;

/**
 * Construct a table of manufacturer codes for
 * manufacturer-specific attributes and clusters.
 */
typedef struct {
  uint16_t index;
  uint16_t manufacturerCode;
} EmberAfManufacturerCodeEntry;

/**
 * Compare two ZCL attribute values. The size of this data
 * type depends on the platform.
 */
typedef uint64_t EmberAfDifferenceType;

/**
 * @brief Contain the superset of values
 * passed to both emberIncomingMessageHandler on the SoC and
 * ezspIncomingMessageHandler on the host.
 */
typedef struct {
  /**
   * The type of the incoming message.
   */
  EmberIncomingMessageType type;
  /**
   * APS frame for the incoming message.
   */
  EmberApsFrame* apsFrame;
  /**
   * The message copied into a flat buffer.
   */
  uint8_t* message;
  /**
   * Length of the incoming message.
   */
  uint16_t msgLen;
  /**
   * Two byte node ID of the sending node.
   */
  uint16_t source;
  /**
   * Link quality from the node that last relayed
   * the message.
   */
  uint8_t lastHopLqi;
  /**
   * The energy level (in units of dBm) observed during the reception.
   */
  int8_t lastHopRssi;
  /**
   * The index of a binding that matches the message
   * or 0xFF if there is no matching binding.
   */
  uint8_t bindingTableIndex;
  /**
   * The index of the entry in the address table
   * that matches the sender of the message or 0xFF
   * if there is no matching entry.
   */
  uint8_t addressTableIndex;
  /**
   * The index of the network on which this message was received.
   */
  uint8_t networkIndex;
} EmberAfIncomingMessage;

/**
 * @brief Interpan Message type: unicast, broadcast, or multicast.
 */
typedef uint8_t EmberAfInterpanMessageType;
#define EMBER_AF_INTER_PAN_UNICAST   0x00u
#define EMBER_AF_INTER_PAN_BROADCAST 0x08u
#define EMBER_AF_INTER_PAN_MULTICAST 0x0Cu

// Legacy names
#define INTER_PAN_UNICAST   EMBER_AF_INTER_PAN_UNICAST
#define INTER_PAN_BROADCAST EMBER_AF_INTER_PAN_BROADCAST
#define INTER_PAN_MULTICAST EMBER_AF_INTER_PAN_MULTICAST

#define EMBER_AF_INTERPAN_OPTION_NONE                 0x0000u
#define EMBER_AF_INTERPAN_OPTION_APS_ENCRYPT          0x0001u
#define EMBER_AF_INTERPAN_OPTION_MAC_HAS_LONG_ADDRESS 0x0002u

/**
 * @brief The options for sending/receiving interpan messages.
 */
typedef uint16_t EmberAfInterpanOptions;

/**
 * @brief Interpan header used for sending and receiving interpan
 *   messages.
 */
typedef struct {
  EmberAfInterpanMessageType messageType;

  /**
   * MAC addressing, which is the destination for
   * outgoing messages and source for incoming messages,
   * which always has a long address.
   */
  EmberEUI64 longAddress;
  EmberNodeId shortAddress;
  EmberPanId panId;

  /**
   * APS data
   */
  EmberAfProfileId profileId;
  EmberAfClusterId clusterId;
  /**
   * The groupId is only used for
   * EMBER_AF_INTERPAN_MULTICAST
   */
  EmberMulticastId groupId;
  EmberAfInterpanOptions options;
} EmberAfInterpanHeader;

// Legacy Name
#define InterPanHeader EmberAfInterpanHeader

/**
 * @brief The options for what interpan messages are allowed.
 */
typedef uint8_t EmberAfAllowedInterpanOptions;

#define EMBER_AF_INTERPAN_DIRECTION_CLIENT_TO_SERVER 0x01u
#define EMBER_AF_INTERPAN_DIRECTION_SERVER_TO_CLIENT 0x02u
#define EMBER_AF_INTERPAN_DIRECTION_BOTH             0x03u
#define EMBER_AF_INTERPAN_GLOBAL_COMMAND             0x04u
#define EMBER_AF_INTERPAN_MANUFACTURER_SPECIFIC      0x08u

/**
 * @brief Define an interpan message that
 *   will be accepted by the interpan filters.
 */
typedef struct {
  EmberAfProfileId profileId;
  EmberAfClusterId clusterId;
  uint8_t commandId;
  EmberAfAllowedInterpanOptions options;
} EmberAfAllowedInterPanMessage;

/**
 * @brief Struct wrapper
 *   for all data for a command which comes
 *   in over the air. This enables struct is used to
 *   encapsulate a command in a single place on the stack
 *   and pass a pointer to that location around during
 *   command processing
 */
typedef struct {
  /**
   * APS frame for the incoming message
   */
  EmberApsFrame            *apsFrame;
  EmberIncomingMessageType  type;
  EmberNodeId               source;
  uint8_t                    *buffer;
  uint16_t                    bufLen;
  bool                   clusterSpecific;
  bool                   mfgSpecific;
  uint16_t                    mfgCode;
  uint8_t                     seqNum;
  uint8_t                     commandId;
  uint8_t                     payloadStartIndex;
  uint8_t                     direction;
  EmberAfInterpanHeader    *interPanHeader;
  uint8_t                     networkIndex;
} EmberAfClusterCommand;

/**
 * @brief Describe clusters that are on the endpoint.
 */
typedef struct {
  /**
   * Pointer to the cluster structs, describing clusters on this
   * endpoint type.
   */
  EmberAfCluster *cluster;
  /**
   * Number of clusters in this endpoint type.
   */
  uint8_t  clusterCount;
  /**
   * Size of all non-external, non-singlet attribute, in this endpoint type.
   */
  uint16_t endpointSize;
} EmberAfEndpointType;

#ifdef EZSP_HOST
typedef EzspDecisionId EmberAfTcLinkKeyRequestPolicy;
typedef EzspDecisionId EmberAfAppLinkKeyRequestPolicy;
  #define EMBER_AF_ALLOW_TC_KEY_REQUESTS                       EZSP_ALLOW_TC_KEY_REQUESTS_AND_SEND_CURRENT_KEY
  #define EMBER_AF_DENY_TC_KEY_REQUESTS                        EZSP_DENY_TC_KEY_REQUESTS
  #define EMBER_AF_ALLOW_APP_KEY_REQUESTS                      EZSP_ALLOW_APP_KEY_REQUESTS
  #define EMBER_AF_DENY_APP_KEY_REQUESTS                       EZSP_DENY_APP_KEY_REQUESTS
#else
typedef EmberTcLinkKeyRequestPolicy EmberAfTcLinkKeyRequestPolicy;
typedef EmberAppLinkKeyRequestPolicy EmberAfAppLinkKeyRequestPolicy;
  #define EMBER_AF_ALLOW_TC_KEY_REQUESTS                       EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY
  #define EMBER_AF_DENY_TC_KEY_REQUESTS                        EMBER_DENY_TC_LINK_KEY_REQUESTS
  #define EMBER_AF_ALLOW_APP_KEY_REQUESTS                      EMBER_ALLOW_APP_LINK_KEY_REQUEST
  #define EMBER_AF_DENY_APP_KEY_REQUESTS                       EMBER_DENY_APP_LINK_KEY_REQUESTS
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfSecurityProfile
#else
typedef uint8_t EmberAfSecurityProfile;
enum
#endif
{
  EMBER_AF_SECURITY_PROFILE_NONE    = 0x00,
  EMBER_AF_SECURITY_PROFILE_HA      = 0x01,
  EMBER_AF_SECURITY_PROFILE_HA12    = 0x02,
  EMBER_AF_SECURITY_PROFILE_SE_TEST = 0x03,
  EMBER_AF_SECURITY_PROFILE_SE_FULL = 0x04,
  EMBER_AF_SECURITY_PROFILE_Z3      = 0x05,
  EMBER_AF_SECURITY_PROFILE_CUSTOM  = 0xFF,
};

typedef struct {
  EmberAfSecurityProfile       securityProfile;
  uint16_t                       tcBitmask;
  EmberExtendedSecurityBitmask tcExtendedBitmask;
  uint16_t                       nodeBitmask;
  EmberExtendedSecurityBitmask nodeExtendedBitmask;
  EmberAfTcLinkKeyRequestPolicy  tcLinkKeyRequestPolicy;
  EmberAfAppLinkKeyRequestPolicy  appLinkKeyRequestPolicy;
  EmberKeyData                 preconfiguredKey;
} EmberAfSecurityProfileData;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct {
  EmberNodeType nodeType;
  EmberAfSecurityProfile securityProfile;
} EmAfZigbeeProNetwork;

#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfEndpointBitmask;
#else
typedef uint8_t EmberAfEndpointBitmask;
enum
#endif
{
  EMBER_AF_ENDPOINT_DISABLED = 0x00,
  EMBER_AF_ENDPOINT_ENABLED  = 0x01,
};

/**
 * @brief Map actual endpoint type onto a specific endpoint.
 */
typedef struct {
  /**
   * Actual Zigbee endpoint number.
   */
  uint8_t endpoint;
  /**
   * Profile ID of the device on this endpoint.
   */
  EmberAfProfileId profileId;
  /**
   * Device ID of the device on this endpoint.
   */
  uint16_t deviceId;
  /**
   * Version of the device.
   */
  uint8_t deviceVersion;
  /**
   * Endpoint type for this endpoint.
   */
  EmberAfEndpointType *endpointType;
  /**
   * Network index for this endpoint.
   */
  uint8_t networkIndex;
  /**
   * Meta-data about the endpoint.
   */
  EmberAfEndpointBitmask bitmask;
} EmberAfDefinedEndpoint;

// Cluster-specific types

/**
 * @brief Bitmask data type for storing one bit of information for each ESI in
 * the ESI table.
 */
#if (EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE <= 8)
typedef uint8_t EmberAfPluginEsiManagementBitmask;
#elif (EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE <= 16)
typedef uint16_t EmberAfPluginEsiManagementBitmask;
#elif (EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE <= 32)
typedef uint32_t EmberAfPluginEsiManagementBitmask;
#else
  #error "EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE cannot exceed 32"
#endif

/**
 * @brief Describe a load control event.
 *
 * This struct is used in the load control event callback and
 * within the demand response load control cluster code.
 */
typedef struct {
  uint32_t     eventId;
#if defined(EMBER_AF_PLUGIN_DRLC_SERVER) || defined(SL_CATALOG_ZIGBEE_DRLC_SERVER_PRESENT)
  EmberEUI64 source;
  uint8_t      sourceEndpoint;
#endif //EMBER_AF_PLUGIN_DRLC_SERVER || SL_CATALOG_ZIGBEE_DRLC_SERVER_PRESENT

#if defined(EMBER_AF_PLUGIN_DRLC) || defined(SL_CATALOG_ZIGBEE_DRLC_PRESENT)
  EmberAfPluginEsiManagementBitmask esiBitmask;
#endif //EMBER_AF_PLUGIN_DRLC || SL_CATALOG_ZIGBEE_DRLC_PRESENT

  uint8_t      destinationEndpoint;
  uint16_t     deviceClass;
  uint8_t      utilityEnrollmentGroup;
  /**
   * Start time in seconds
   */
  uint32_t     startTime;
  /**
   * Duration in minutes
   */
  uint16_t     duration;
  uint8_t      criticalityLevel;
  uint8_t      coolingTempOffset;
  uint8_t      heatingTempOffset;
  int16_t     coolingTempSetPoint;
  int16_t     heatingTempSetPoint;
  int8_t      avgLoadPercentage;
  uint8_t      dutyCycle;
  uint8_t      eventControl;
  uint32_t     startRand;
  uint32_t     durationRand;
  uint8_t      optionControl;
} EmberAfLoadControlEvent;

/**
 * @brief Indicate the result of the
 *   service discovery. Unicast discoveries are completed
 *   as soon as a response is received. Broadcast discoveries
 *   wait a period of time for multiple responses to be received.
 */
typedef enum {
  EMBER_AF_BROADCAST_SERVICE_DISCOVERY_COMPLETE               = 0x00,
  EMBER_AF_BROADCAST_SERVICE_DISCOVERY_RESPONSE_RECEIVED      = 0x01,
  EMBER_AF_UNICAST_SERVICE_DISCOVERY_TIMEOUT                  = 0x02,
  EMBER_AF_UNICAST_SERVICE_DISCOVERY_COMPLETE_WITH_RESPONSE   = 0x03,
  EMBER_AF_BROADCAST_SERVICE_DISCOVERY_COMPLETE_WITH_RESPONSE = 0x04,
  EMBER_AF_UNICAST_SERVICE_DISCOVERY_COMPLETE_WITH_EMPTY_RESPONSE   = 0x05,
  EMBER_AF_BROADCAST_SERVICE_DISCOVERY_COMPLETE_WITH_EMPTY_RESPONSE = 0x06,
} EmberAfServiceDiscoveryStatus;

#define EM_AF_DISCOVERY_RESPONSE_MASK (0x05u)

/**
 * @brief A simple way to determine if the service discovery callback
 *   has a response.
 */
#define emberAfHaveDiscoveryResponseStatus(status)  ((status) & EM_AF_DISCOVERY_RESPONSE_MASK)

/**
 * @brief Contain general information about service discovery.
 */
typedef struct {
  /**
   * The status indicates both the type of request (broadcast or unicast)
   * and whether a response has been received.
   */
  EmberAfServiceDiscoveryStatus status;

  /**
   * Indicate which ZDO request cluster was associated with the request.
   * It is helpful for a callback that may be used for multiple ZDO request types
   * to determine the type of data returned. It is based on the
   * ZDO cluster values defined in ember-types.h.
   */
  uint16_t zdoRequestClusterId;

  /**
   * This is the address of the device that matched the request, which may
   * be different than the device that *actually* is responding.  This occurs
   * when parents respond on behalf of their children.
   */
  EmberNodeId matchAddress;

  /**
   * This data is non-NULL only if the status code indicates a response.
   * When data is present, the type is determined according to the ZDO cluster ID sent out.
   * For NETWORK_ADDRESS_REQUEST or IEEE_ADDRESS_REQUEST, the long ID will
   * be contained in the responseData, which is a value of type ::EmberEUI64.
   * The short ID will be in the matchAddress parameter field.
   * For the MATCH_DESCRIPTORS_REQUEST the responseData will point
   * to an ::EmberAfEndpointList structure.
   */
  const void* responseData;
} EmberAfServiceDiscoveryResult;

/**
 * @brief A list of endpoints received during a service discovery attempt,
 *   which is returned for a match descriptor request and a
 *   active endpoint request.
 */
typedef struct {
  uint8_t count;
  const uint8_t* list;
} EmberAfEndpointList;

/**
 * @brief A list of clusters received during a service discovery attempt,
 * which is returned for a simple descriptor request.
 */
typedef struct {
  uint8_t inClusterCount;
  const uint16_t* inClusterList;
  uint8_t outClusterCount;
  const uint16_t* outClusterList;
  EmberAfProfileId profileId;
  uint16_t deviceId;
  uint8_t endpoint;
} EmberAfClusterList;

/**
 * @brief Define a callback where a code element or cluster can be informed
 * as to the result of a service discovery they have requested.
 * For each match, the callback is fired with all the resulting matches from
 * that source. If the discovery was unicast to a specific device,
 * the callback is only fired once with either MATCH_FOUND or COMPLETE
 * (no matches found). If the discovery is broadcast, multiple callbacks
 * may be fired with ::EMBER_AF_BROADCAST_SERVICE_DISCOVERY_RESPONSE_RECEIVED.
 * After a few seconds, the callback is fired with
 * ::EMBER_AF_BROADCAST_SERVICE_DISCOVERY_COMPLETE_WITH_RESPONSE or
 * ::EMBER_AF_BROADCAST_SERVICE_DISCOVERY_COMPLETE_WITH_EMPTY_RESPONSE as the result.
 */
typedef void (EmberAfServiceDiscoveryCallback)(const EmberAfServiceDiscoveryResult* result);

/**
 * @brief Define a callback where a code element or cluster can be
 * informed as to the result of a request to initiate a partner link key
 * exchange.  The callback will be triggered with success equal to true if the
 * exchange completed successfully.
 */
typedef void (EmberAfPartnerLinkKeyExchangeCallback)(bool success);

/**
 * @brief Control how the device will poll for a given
 * active cluster-related event. When the event is scheduled, the application
 * can pass a poll control value which will be stored along with the event.
 * The processor is only allowed to poll according to the most restrictive
 * value for all active event.  For example, if two events are active, one
 * with EMBER_AF_LONG_POLL and the other with EMBER_AF_SHORT_POLL, the
 * processor will short poll until the second event is deactivated.
 */
typedef enum {
  EMBER_AF_LONG_POLL,
  EMBER_AF_SHORT_POLL,
} EmberAfEventPollControl;

/**
 * @brief Control how the device
 *        will sleep for a given active cluster related event.
 *        When the event is scheduled, the scheduling code can
 *        pass a sleep control value which will be stored along
 *        with the event. The processor is only allowed to sleep
 *        according to the most restrictive sleep control value
 *        for any active event. For example, if two events
 *        are active, one with EMBER_AF_OK_TO_HIBERNATE and the
 *        other with EMBER_AF_OK_TO_NAP, the processor
 *        will only be allowed to sleep until the second event
 *        is deactivated.
 */
typedef enum {
  EMBER_AF_OK_TO_SLEEP,
  /** @deprecated. */
  EMBER_AF_OK_TO_HIBERNATE = EMBER_AF_OK_TO_SLEEP,
  /** @deprecated. */
  EMBER_AF_OK_TO_NAP,
  EMBER_AF_STAY_AWAKE,
} EmberAfEventSleepControl;

/**
 * @brief Track tasks that the Application
 * framework cares about. These are intended to be tasks
 * that should keep the device out of hibernation like an
 * application level request / response. If the response does
 * not come in as a data ACK, the application needs
 * to stay out of hibernation to wait and poll for it.
 *
 * Some tasks do not necessarily have a response. For
 * example, a ZDO request may or may not have a response. In this
 * case, the application framework cannot rely on the fact that
 * a response will come in to end the wake cycle, so the Application
 * framework must time out the wake cycle if no expected
 * response is received or no other event can be relied upon to
 * end the wake cycle.
 *
 * Add these type of tasks to the wake timeout mask
 * by calling ::emberAfSetWakeTimeoutBitmaskCallback so that they
 * can be governed by a timeout instead of a request / response.
 *
 * The current tasks bitmask is an uint32_t bitmask used to
 * track which tasks are active at any given time. The bottom 16 bits,
 * values 0x01 - 0x8000 are reserved for Ember's use. The top
 * 16 bits are reserved for the customer, values 0x10000 -
 * 0x80000000
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfApplicationTask
#else
typedef uint32_t EmberAfApplicationTask;
enum
#endif
{
  // we may be able to remove these top two since they are
  // handled by the stack on the SOC.
  EMBER_AF_WAITING_FOR_DATA_ACK                     = 0x00000001, //not needed?
  EMBER_AF_LAST_POLL_GOT_DATA                       = 0x00000002, //not needed?
  EMBER_AF_WAITING_FOR_SERVICE_DISCOVERY            = 0x00000004,
  EMBER_AF_WAITING_FOR_ZDO_RESPONSE                 = 0x00000008,
  EMBER_AF_WAITING_FOR_ZCL_RESPONSE                 = 0x00000010,
  EMBER_AF_WAITING_FOR_REGISTRATION                 = 0x00000020,
  EMBER_AF_WAITING_FOR_PARTNER_LINK_KEY_EXCHANGE    = 0x00000040,
  EMBER_AF_FORCE_SHORT_POLL                         = 0x00000080,
  EMBER_AF_FRAGMENTATION_IN_PROGRESS                = 0x00000100,
  EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY = 0x00000200,
  EMBER_AF_WAITING_FOR_TC_KEY_UPDATE                = 0x00000400,
};

/**
 * @brief Keep track of cluster related events and
 * their sleep control values.
 * The cluster code will not know at
 * runtime all of the events that it has access to in the event table.
 * This structure is stored by the application framework in an event
 * context table, which along with helper functions allows the cluster
 * code to schedule and deactivate its associated events.
 */
typedef struct {
  /**
   * The endpoint of the associated cluster event.
   */
  uint8_t endpoint;
  /**
   * The cluster ID of the associated cluster event.
   */
  EmberAfClusterId clusterId;
  /**
   * The server/client identity of the associated cluster event.
   */
  bool isClient;
  /**
   * A poll control value used to control the network polling behavior while
   * the event is active.
   */
  EmberAfEventPollControl pollControl;
  /**
   * A sleep control value used to control the processor's sleep
   * behavior while the event is active.
   */
  EmberAfEventSleepControl sleepControl;
  /**
   * A pointer to the event control value, which is stored in the event table
   * and is used to actually schedule the event.
   */
  EmberEventControl *eventControl;
} EmberAfEventContext;

#ifdef UC_BUILD
// TODO: update doxygen
/**
 * @brief Keep track of cluster related events and
 * their sleep control values. The cluster code will not know at
 * runtime all of the events that it has access to in the event table
 * This structure is stored by the application framework in an event
 * context table which along with helper functions allows the cluster
 * code to schedule and deactivate its associated events.
 */
typedef struct {
  /**
   * The endpoint of the associated cluster event.
   */
  uint8_t endpoint;
  /**
   * The cluster ID of the associated cluster event.
   */
  EmberAfClusterId clusterId;
  /**
   * The server/client identity of the associated cluster event.
   */
  bool isClient;
  /**
   * A poll control value used to control the network polling behavior while
   * the event is active.
   */
  EmberAfEventPollControl pollControl;
  /**
   * A sleep control value used to control the processor's sleep
   * behavior while the event is active.
   */
  EmberAfEventSleepControl sleepControl;
  /**
   * A pointer to the event control value which is stored in the event table
   * and is used to actually schedule the event.
   */
  sl_zigbee_event_t *event;
} sl_zigbee_event_context_t;
#endif

/**
 * @brief Refer to the handler for network events.
 */
typedef void (*EmberAfNetworkEventHandler)(void);

/**
 * @brief Refer to the handler for endpoint events.
 */
typedef void (*EmberAfEndpointEventHandler)(uint8_t endpoint);

/**
 * @brief Indicate the absence of a Group table entry.
 */
#define EMBER_AF_GROUP_TABLE_NULL_INDEX 0xFFu
/**
 * @brief Value used when setting or getting the endpoint in a Group table
 * entry.  It indicates that the entry is not in use.
 */
#define EMBER_AF_GROUP_TABLE_UNUSED_ENDPOINT_ID 0x00u
/**
 * @brief Maximum length of Group names, not including the length byte.
 */
#define ZCL_GROUPS_CLUSTER_MAXIMUM_NAME_LENGTH 16u
/**
 * @brief Store group table entries in RAM or in tokens,
 * depending on the platform.  If the endpoint field is
 * ::EMBER_AF_GROUP_TABLE_UNUSED_ENDPOINT_ID, the entry is unused.
 */
typedef struct {
  uint8_t  endpoint; // 0x00 when not in use
  uint16_t groupId;
  uint8_t  bindingIndex;
#ifdef EMBER_AF_PLUGIN_GROUPS_SERVER_NAME_SUPPORT
  uint8_t  name[ZCL_GROUPS_CLUSTER_MAXIMUM_NAME_LENGTH + 1];
#endif
} EmberAfGroupTableEntry;

/**
 * @brief Indicates the absence of a Scene table entry.
 */
#define EMBER_AF_SCENE_TABLE_NULL_INDEX 0xFFu
/**
 * @brief Value used when setting or getting the endpoint in a Scene table
 * entry.  It indicates that the entry is not in use.
 */
#define EMBER_AF_SCENE_TABLE_UNUSED_ENDPOINT_ID 0x00u
/**
 * @brief Maximum length of Scene names, not including the length byte.
 */
#define ZCL_SCENES_CLUSTER_MAXIMUM_NAME_LENGTH 16u
/**
 * @brief The group identifier for the global scene.
 */
#define ZCL_SCENES_GLOBAL_SCENE_GROUP_ID 0x0000u
/**
 * @brief The scene identifier for the global scene.
 */
#define ZCL_SCENES_GLOBAL_SCENE_SCENE_ID 0x00u

// EMZIGBEE-6833: EmberAfSceneTableEntry declaration should be moved to the scenes component.
#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_SCENES_PRESENT
#include "scenes-config.h"
#if (EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT == 1)
#define SCENES_NAME_SUPPORT
#endif
#endif // SL_CATALOG_ZIGBEE_SCENES_PRESENT
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT
#define SCENES_NAME_SUPPORT
#endif
#endif // UC_BUILD

/**
 * @brief Store scene table entries in RAM or in tokens,
 * depending on a plugin setting.  If endpoint field is
 * ::EMBER_AF_SCENE_TABLE_UNUSED_ENDPOINT_ID, the entry is unused.
 */
typedef struct {
  uint8_t   endpoint;                // 0x00 when this record is not in use
  uint16_t  groupId;                 // 0x0000 if not associated with a group
  uint8_t   sceneId;
#ifdef SCENES_NAME_SUPPORT
  uint8_t   name[ZCL_SCENES_CLUSTER_MAXIMUM_NAME_LENGTH + 1];
#endif
  uint16_t  transitionTime;          // in seconds
  uint8_t   transitionTime100ms;     // in tenths of a seconds
#ifdef ZCL_USING_ON_OFF_CLUSTER_SERVER
  bool hasOnOffValue;
  bool onOffValue;
#endif
#ifdef ZCL_USING_LEVEL_CONTROL_CLUSTER_SERVER
  bool hasCurrentLevelValue;
  uint8_t   currentLevelValue;
#endif
#ifdef ZCL_USING_THERMOSTAT_CLUSTER_SERVER
  bool hasOccupiedCoolingSetpointValue;
  int16_t  occupiedCoolingSetpointValue;
  bool hasOccupiedHeatingSetpointValue;
  int16_t  occupiedHeatingSetpointValue;
  bool hasSystemModeValue;
  uint8_t   systemModeValue;
#endif
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_SERVER
  bool hasCurrentXValue;
  uint16_t  currentXValue;
  bool hasCurrentYValue;
  uint16_t  currentYValue;
  bool hasEnhancedCurrentHueValue;
  uint16_t  enhancedCurrentHueValue;
  bool hasCurrentSaturationValue;
  uint8_t   currentSaturationValue;
  bool hasColorLoopActiveValue;
  uint8_t   colorLoopActiveValue;
  bool hasColorLoopDirectionValue;
  uint8_t   colorLoopDirectionValue;
  bool hasColorLoopTimeValue;
  uint16_t  colorLoopTimeValue;
  bool hasColorTemperatureMiredsValue;
  uint16_t  colorTemperatureMiredsValue;
#endif //ZCL_USING_COLOR_CONTROL_CLUSTER_SERVER
#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_SERVER
  bool hasLockStateValue;
  uint8_t   lockStateValue;
#endif
#ifdef ZCL_USING_WINDOW_COVERING_CLUSTER_SERVER
  bool hasCurrentPositionLiftPercentageValue;
  uint8_t   currentPositionLiftPercentageValue;
  bool hasCurrentPositionTiltPercentageValue;
  uint8_t   currentPositionTiltPercentageValue;
#endif
} EmberAfSceneTableEntry;

#if !defined(EMBER_AF_PLUGIN_MESSAGING_CLIENT) && !defined(SL_CATALOG_ZIGBEE_MESSAGING_CLIENT_PRESENT)
// To forward declare callbacks regardless of whether the plugin
// is enabled,  define all data structures. To define
// the messaging client data struct, declare this variable.
  #define EMBER_AF_PLUGIN_MESSAGING_CLIENT_MESSAGE_SIZE 0
#endif

typedef struct {
  bool    valid;
  bool    active;
  EmberAfPluginEsiManagementBitmask esiBitmask;
  uint8_t      clientEndpoint;
  uint32_t     messageId;
  uint8_t      messageControl;
  uint32_t     startTime;
  uint32_t     endTime;
  uint16_t     durationInMinutes;
  uint8_t      message[EMBER_AF_PLUGIN_MESSAGING_CLIENT_MESSAGE_SIZE + 1];
} EmberAfPluginMessagingClientMessage;

#define ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH 11u
typedef struct {
  bool valid;
  bool active;
  uint8_t   clientEndpoint;
  uint32_t  providerId;
  uint8_t   rateLabel[ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH + 1];
  uint32_t  issuerEventId;
  uint32_t  currentTime;
  uint8_t   unitOfMeasure;
  uint16_t  currency;
  uint8_t   priceTrailingDigitAndPriceTier;
  uint8_t   numberOfPriceTiersAndRegisterTier;
  uint32_t  startTime;
  uint32_t  endTime;
  uint16_t  durationInMinutes;
  uint32_t  price;
  uint8_t   priceRatio;
  uint32_t  generationPrice;
  uint8_t   generationPriceRatio;
  uint32_t  alternateCostDelivered;
  uint8_t   alternateCostUnit;
  uint8_t   alternateCostTrailingDigit;
  uint8_t   numberOfBlockThresholds;
  uint8_t   priceControl;
} EmberAfPluginPriceClientPrice;

/**
 * @brief Specifies CPP Authorization values
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfPluginPriceCppAuth
#else
typedef uint8_t EmberAfPluginPriceCppAuth;
enum
#endif
{
  EMBER_AF_PLUGIN_PRICE_CPP_AUTH_PENDING  = 0,
  EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED = 1,
  EMBER_AF_PLUGIN_PRICE_CPP_AUTH_REJECTED = 2,
  EMBER_AF_PLUGIN_PRICE_CPP_AUTH_FORCED   = 3,
  EMBER_AF_PLUGIN_PRICE_CPP_AUTH_RESERVED = 4
};

/**
 * @brief Use when setting or getting the endpoint in a report table
 * entry.  It indicates that the entry is not in use.
 */
#define EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID 0x00u
/**
 * @brief Store reporting configurations.  If endpoint
 * field is ::EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID, the entry is
 * unused.
 */
typedef struct {
  /** EMBER_ZCL_REPORTING_DIRECTION_REPORTED for reports sent from the local
   *  device or EMBER_ZCL_REPORTING_DIRECTION_RECEIVED for reports received
   *  from a remote device.
   */
  EmberAfReportingDirection direction;
  /** The local endpoint from which the attribute is reported or to which the
   * report is received.  If ::EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID,
   * the entry is unused.
   */
  uint8_t endpoint;
  /** The cluster where the attribute is located. */
  EmberAfClusterId clusterId;
  /** The ID of the attribute being reported or received. */
  EmberAfAttributeId attributeId;
  /** CLUSTER_MASK_SERVER for server-side attributes or CLUSTER_MASK_CLIENT for
   *  client-side attributes.
   */
  uint8_t mask;
  /** Manufacturer code associated with the cluster and/or attribute.  If the
   *  cluster ID is inside the manufacturer-specific range, this value
   *  indicates the manufacturer code for the cluster.  Otherwise, if this
   *  value is non-zero and the cluster ID is a standard ZCL cluster, it
   *  indicates the manufacturer code for attribute.
   */
  uint16_t manufacturerCode;
  union {
    struct {
      /** The minimum reporting interval, measured in seconds. */
      uint16_t minInterval;
      /** The maximum reporting interval, measured in seconds. */
      uint16_t maxInterval;
      /** The minimum change to the attribute that will result in a report
       *  being sent.
       */
      uint32_t reportableChange;
    } reported;
    struct {
      /** The node id of the source of the received reports. */
      EmberNodeId source;
      /** The remote endpoint from which the attribute is reported. */
      uint8_t endpoint;
      /** The maximum expected time between reports, measured in seconds. */
      uint16_t timeout;
    } received;
  } data;
} EmberAfPluginReportingEntry;

typedef enum {
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_SUCCESS                          = 0x00,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_BUSY                             = 0x01,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_NO_MORE_TUNNEL_IDS               = 0x02,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_PROTOCOL_NOT_SUPPORTED           = 0x03,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_FLOW_CONTROL_NOT_SUPPORTED       = 0x04,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_IEEE_ADDRESS_REQUEST_FAILED      = 0xF9,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_IEEE_ADDRESS_NOT_FOUND           = 0xFA,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_ADDRESS_TABLE_FULL               = 0xFB,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_LINK_KEY_EXCHANGE_REQUEST_FAILED = 0xFC,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_LINK_KEY_EXCHANGE_FAILED         = 0xFD,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_REQUEST_TUNNEL_FAILED            = 0xFE,
  EMBER_AF_PLUGIN_TUNNELING_CLIENT_REQUEST_TUNNEL_TIMEOUT           = 0xFF,
} EmberAfPluginTunnelingClientStatus;

/**
 * @brief Status codes used by the ZLL Commissioning plugin.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfZllCommissioningStatus
#else
typedef uint8_t EmberAfZllCommissioningStatus;
enum
#endif
{
  EMBER_AF_ZLL_ABORTED_BY_APPLICATION                      = 0x00,
  EMBER_AF_ZLL_CHANNEL_CHANGE_FAILED                       = 0x01,
  EMBER_AF_ZLL_JOINING_FAILED                              = 0x02,
  EMBER_AF_ZLL_NO_NETWORKS_FOUND                           = 0x03,
  EMBER_AF_ZLL_PREEMPTED_BY_STACK                          = 0x04,
  EMBER_AF_ZLL_SENDING_START_JOIN_FAILED                   = 0x05,
  EMBER_AF_ZLL_SENDING_DEVICE_INFORMATION_REQUEST_FAILED   = 0x06,
  EMBER_AF_ZLL_SENDING_IDENTIFY_REQUEST_FAILED             = 0x07,
  EMBER_AF_ZLL_SENDING_RESET_TO_FACTORY_NEW_REQUEST_FAILED = 0x08,
  EMBER_AF_ZLL_NETWORK_FORMATION_FAILED                    = 0x09,
  EMBER_AF_ZLL_NETWORK_UPDATE_OPERATION                    = 0x0A,
};

/**
 * @brief Represent Group Information Records used by ZLL
 * Commissioning.
 */
typedef struct {
  EmberMulticastId groupId;
  uint8_t            groupType;
} EmberAfPluginZllCommissioningGroupInformationRecord;

/**
 * @brief Represent Endpoint Information Records used by
 * ZLL Commissioning.
 */
typedef struct {
  EmberNodeId networkAddress;
  uint8_t       endpointId;
  uint16_t      profileId;
  uint16_t      deviceId;
  uint8_t       version;
} EmberAfPluginZllCommissioningEndpointInformationRecord;

/**
 * @brief This is a unique identifier for referencing Zigbee Over-the-air upgrade
 *   images.  It is used by the OTA plugins when passing around information about
 *   an upgrade file.
 */
typedef struct {
  uint16_t manufacturerId;
  uint16_t imageTypeId;
  uint32_t firmwareVersion;

  /**
   * This is only used for device specific files.
   * It will be set to all zeroes when the image does not
   * have an upgrade destination field in it.
   * Little endian format.
   */
  uint8_t deviceSpecificFileEui64[EUI64_SIZE];
} EmberAfOtaImageId;

/**
 * @brief The list of options possible for the image block request/response.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfImageBlockRequestOptions
#else
typedef uint8_t EmberAfImageBlockRequestOptions;
enum
#endif
{
  EMBER_AF_IMAGE_BLOCK_REQUEST_OPTIONS_NONE                          = 0x00,
  // Client supports Min Block Request field
  EMBER_AF_IMAGE_BLOCK_REQUEST_MIN_BLOCK_REQUEST_SUPPORTED_BY_CLIENT = 0x01,
  // Server supports Min Block Request field
  EMBER_AF_IMAGE_BLOCK_REQUEST_MIN_BLOCK_REQUEST_SUPPORTED_BY_SERVER = 0x02,
  // The Image Block Request is actually simulated in place of an actually
  // received Image Page Request
  EMBER_AF_IMAGE_BLOCK_REQUEST_SIMULATED_FROM_PAGE_REQUEST           = 0x04
};

/**
 * @brief Passed to the
 * emberAfImageBlockRequestCallback() to let the application decide what to do.
 */
typedef struct {
  const EmberAfOtaImageId* id;
  uint32_t offset;
  uint32_t waitTimeSecondsResponse;
  EmberNodeId source;
  EmberEUI64 sourceEui;           // optionally present in messages
  // The minBlockRequestPeriod can be treated as milliseconds or seconds on the
  // client. The OTA server plugin has optional support to probe clients and
  // treat this field with appropriate units (ms or sec)
  uint16_t minBlockRequestPeriod; // optionally present in messages
  uint8_t maxDataSize;
  uint8_t clientEndpoint;
  EmberAfImageBlockRequestOptions bitmask;
} EmberAfImageBlockRequestCallbackStruct;

/**
 * @brief Contain the success or error code of an OTA storage
 *   device operation.
 */
typedef enum {
  EMBER_AF_OTA_STORAGE_SUCCESS               = 0,
  EMBER_AF_OTA_STORAGE_ERROR                 = 1,
  EMBER_AF_OTA_STORAGE_RETURN_DATA_TOO_LONG  = 2,
  EMBER_AF_OTA_STORAGE_PARTIAL_FILE_FOUND    = 3,
  EMBER_AF_OTA_STORAGE_OPERATION_IN_PROGRESS = 4,
} EmberAfOtaStorageStatus;

/**
 * @brief Contain the success or error code of an OTA download
 * operation.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaDownloadResult
#else
typedef uint8_t EmberAfOtaDownloadResult;
enum
#endif
{
  EMBER_AF_OTA_DOWNLOAD_AND_VERIFY_SUCCESS = 0,
  EMBER_AF_OTA_DOWNLOAD_TIME_OUT           = 1,
  EMBER_AF_OTA_VERIFY_FAILED               = 2,
  EMBER_AF_OTA_SERVER_ABORTED              = 3,
  EMBER_AF_OTA_CLIENT_ABORTED              = 4,
  EMBER_AF_OTA_ERASE_FAILED                = 5,
};

/**
 * @brief The maximum size of the string that is present in the header
 *   of the Zigbee Over-the-air file format.
 */
#define EMBER_AF_OTA_MAX_HEADER_STRING_LENGTH 32u

#define UID_SIZE 32u
/**
 * @brief This structure is an in-memory representation of
 *   the Over-the-air header data that resides on disk.
 *   It is not a byte-for-byte copy.
 */
typedef struct {
  // Magic Number omitted since it is always the same.
  uint16_t headerVersion;
  uint16_t headerLength;
  uint16_t fieldControl;
  uint16_t manufacturerId;
  uint16_t imageTypeId;           // a.k.a. Device ID
  uint32_t firmwareVersion;
  uint16_t zigbeeStackVersion;

  /**
   * @brief The spec. does not require that the string be NULL terminated in the
   *   header stored on disk.  Therefore, +1 is added in the data structure to
   *   ensure support of 32-character string without a NULL terminator.
   */
  uint8_t headerString[EMBER_AF_OTA_MAX_HEADER_STRING_LENGTH + 1];

  /**
   * @brief When reading the header, this will be the complete length of
   *  the file. When writing the header, this must be set to
   *  the length of the MFG image data portion including all tags.
   */
  uint32_t imageSize;

  /**
   * @brief The remaining four fields are optional. Check the field control
   *   to determine if their values are valid.
   */
  uint8_t securityCredentials;
  union {
    uint8_t EUI64[EUI64_SIZE];
    uint8_t UID[UID_SIZE];
  } upgradeFileDestination;
  uint16_t minimumHardwareVersion;
  uint16_t maximumHardwareVersion;
} EmberAfOtaHeader;

/**
 * @brief Contain information about a tag that resides
 *   within an Over-the-air bootload file.
 */
typedef struct {
  uint16_t id;
  uint32_t length;
} EmberAfTagData;

typedef enum {
  NO_APP_MESSAGE               = 0,
  RECEIVED_PARTNER_CERTIFICATE = 1,
  GENERATING_EPHEMERAL_KEYS    = 2,
  GENERATING_SHARED_SECRET     = 3,
  KEY_GENERATION_DONE          = 4,
  GENERATE_SHARED_SECRET_DONE  = 5,
  /**
   * LINK_KEY_ESTABLISHED indicates Success,
   * key establishment done.
   */
  LINK_KEY_ESTABLISHED         = 6,

  /**
   * Error codes:
   * Transient failures where Key Establishment could be retried.
   */
  NO_LOCAL_RESOURCES          = 7,
  PARTNER_NO_RESOURCES        = 8,
  TIMEOUT_OCCURRED            = 9,
  INVALID_APP_COMMAND         = 10,
  MESSAGE_SEND_FAILURE        = 11,
  PARTNER_SENT_TERMINATE      = 12,
  INVALID_PARTNER_MESSAGE     = 13,
  PARTNER_SENT_DEFAULT_RESPONSE_ERROR = 14,

  /**
   * Fatal Errors:
   * These results are not worth retrying because the outcome
   * will not change.
   */
  BAD_CERTIFICATE_ISSUER      = 15,
  KEY_CONFIRM_FAILURE         = 16,
  BAD_KEY_ESTABLISHMENT_SUITE = 17,

  KEY_TABLE_FULL              = 18,

  /**
   * Neither initiator nor responder is an
   * ESP/TC so the key establishment is not
   * allowed per the spec.
   */
  NO_ESTABLISHMENT_ALLOWED    = 19,

  /* 283k1 certificates need to have valid key usage.
   */
  INVALID_CERTIFICATE_KEY_USAGE = 20,
} EmberAfKeyEstablishmentNotifyMessage;

#define APP_NOTIFY_ERROR_CODE_START NO_LOCAL_RESOURCES
#define APP_NOTIFY_MESSAGE_TEXT { \
    "None",                       \
    "Received Cert",              \
    "Generate keys",              \
    "Generate secret",            \
    "Key generate done",          \
    "Generate secret done",       \
    "Link key verified",          \
                                  \
    /* Transient Error codes */   \
    "No local resources",         \
    "Partner no resources",       \
    "Timeout",                    \
    "Invalid app. command",       \
    "Message send failure",       \
    "Partner sent terminate",     \
    "Bad message",                \
    "Partner sent Default Rsp",   \
                                  \
    /* Fatal errors */            \
    "Bad cert issuer",            \
    "Key confirm failure",        \
    "Bad key est. suite",         \
    "Key table full",             \
    "Not allowed",                \
    "Invalid Key Usage",          \
}

/**
 * @brief Indicate the state of an OTA bootload
 *   image undergoing verification.  This is used both for cryptographic
 *   verification and manufacturer-specific verification.
 */
typedef enum {
  EMBER_AF_IMAGE_GOOD                 = 0,
  EMBER_AF_IMAGE_BAD                  = 1,
  EMBER_AF_IMAGE_VERIFY_IN_PROGRESS   = 2,

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // Internal use only.
  EMBER_AF_IMAGE_VERIFY_WAIT          = 3,
  EMBER_AF_IMAGE_VERIFY_ERROR         = 4,
  EMBER_AF_IMAGE_UNKNOWN              = 5,
  EMBER_AF_NO_IMAGE_VERIFY_SUPPORT    = 6,
#endif
} EmberAfImageVerifyStatus;

/**
 * @brief Refer to the tick callback for cluster.
 *
 * Tick function will be called once for each tick for each endpoint in
 * the cluster. The rate of tick is determined by the metadata of the
 * cluster.
 */
typedef void (*EmberAfTickFunction)(uint8_t endpoint);

/**
 * @brief Refer to the init callback for cluster.
 *
 * Init function is called when the application starts up, once for
 * each cluster/endpoint combination.
 */
typedef void (*EmberAfInitFunction)(uint8_t endpoint);

/**
 * @brief Refer to the attribute changed callback function.
 *
 * This function is called just after an attribute changes.
 */
typedef void (*EmberAfClusterAttributeChangedCallback)(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId);

/**
 * @brief Refer to the manufacturer-specific
 *        attribute changed callback function.
 *
 * This function is called just after a manufacturer-specific attribute changes.
 */
typedef void (*EmberAfManufacturerSpecificClusterAttributeChangedCallback)(uint8_t endpoint,
                                                                           EmberAfAttributeId attributeId,
                                                                           uint16_t manufacturerCode);

/**
 * @brief Refer to the pre-attribute changed callback function.
 *
 * This function is called before an attribute changes.
 */
typedef EmberAfStatus (*EmberAfClusterPreAttributeChangedCallback)(uint8_t endpoint,
                                                                   EmberAfAttributeId attributeId,
                                                                   EmberAfAttributeType attributeType,
                                                                   uint8_t size,
                                                                   uint8_t *value);

/**
 * @brief Refer to the default response callback function.
 *
 * This function is called when default response is received, before
 * the global callback. Global callback is called immediately afterwards.
 */
typedef void (*EmberAfDefaultResponseFunction)(uint8_t endpoint,
                                               uint8_t commandId,
                                               EmberAfStatus status);

/**
 * @brief Refer to the message sent callback function.
 *
 * This function is called when a message is sent.
 */
typedef void (*EmberAfMessageSentFunction)(EmberOutgoingMessageType type,
                                           uint16_t indexOrDestination,
                                           EmberApsFrame *apsFrame,
                                           uint16_t msgLen,
                                           uint8_t *message,
                                           EmberStatus status);

/**
 * @brief The EmberAfMessageStruct is a struct wrapper that
 *   contains all the data about a low-level message to be
 *   sent (it may be ZCL or may be some other protocol).
 */
typedef struct {
  EmberAfMessageSentFunction callback;
  EmberApsFrame* apsFrame;
  uint8_t* message;
  uint16_t indexOrDestination;
  uint16_t messageLength;
  EmberOutgoingMessageType type;
  bool broadcast;
} EmberAfMessageStruct;

/**
 * @brief Use for a link key backup.
 *
 * Each entry notes the EUI64 of the device it is paired to and the key data.
 *   This key may be hashed and not the actual link key currently in use.
 */

typedef struct {
  EmberEUI64 deviceId;
  EmberKeyData key;
} EmberAfLinkKeyBackupData;

/**
 * @brief Use for all trust center backup data.
 *
 * The 'keyList' pointer must point to an array and 'maxKeyListLength'
 * must be populated with the maximum number of entries the array can hold.
 *
 * Functions that modify this data structure will populate 'keyListLength'
 * indicating how many keys were actually written into 'keyList'.
 */

typedef struct {
  EmberEUI64 extendedPanId;
  uint8_t keyListLength;
  uint8_t maxKeyListLength;
  EmberAfLinkKeyBackupData* keyList;
} EmberAfTrustCenterBackupData;

/**
 * @brief The length of the hardware tag in the Ember Bootloader Query
 *   Response.
 */
#define EMBER_AF_STANDALONE_BOOTLOADER_HARDWARE_TAG_LENGTH 16u

/**
 * @brief Use for the information retrieved during a response
 *   to an Ember Bootloader over-the-air query.
 */
typedef struct {
  uint8_t hardwareTag[EMBER_AF_STANDALONE_BOOTLOADER_HARDWARE_TAG_LENGTH];
  uint8_t eui64[EUI64_SIZE];
  uint16_t mfgId;
  uint16_t bootloaderVersion;
  uint8_t capabilities;
  uint8_t platform;
  uint8_t micro;
  uint8_t phy;
  bool bootloaderActive;
} EmberAfStandaloneBootloaderQueryResponseData;

/**
 * @brief Keep track of incoming and outgoing
 *   commands for command discovery.
 */
typedef struct {
  uint16_t clusterId;
  uint8_t commandId;
  uint8_t mask;
} EmberAfCommandMetadata;

/**
 * @brief Describe the time in a human
 * understandable format (as opposed to 32-bit UTC).
 */

typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} EmberAfTimeStruct;

/**
 * @brief Describe the ZCL Date data type.
 */

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t dayOfMonth;
  uint8_t dayOfWeek;
} EmberAfDate;

/* Simple Metering Server Test Code */
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_ELECTRIC_METER 0
#define EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_GAS_METER 1

// Functional Notification Flags
// Also #defined in enums.h under slightly different names
#define EMBER_AF_METERING_FNF_NEW_OTA_FIRMWARE                               0x00000001
#define EMBER_AF_METERING_FNF_CBKE_UPDATE_REQUEST                            0x00000002
#define EMBER_AF_METERING_FNF_TIME_SYNC                                      0x00000004
#define EMBER_AF_METERING_FNF_STAY_AWAKE_REQUEST_HAN                         0x00000010
#define EMBER_AF_METERING_FNF_STAY_AWAKE_REQUEST_WAN                         0x00000020
#define EMBER_AF_METERING_FNF_PUSH_HISTORICAL_METERING_DATA_ATTRIBUTE_SET    0x000001C0
#define EMBER_AF_METERING_FNF_PUSH_HISTORICAL_PREPAYMENT_DATA_ATTRIBUTE_SET  0x00000E00
#define EMBER_AF_METERING_FNF_PUSH_ALL_STATIC_DATA_BASIC_CLUSTER             0x00001000
#define EMBER_AF_METERING_FNF_PUSH_ALL_STATIC_DATA_METERING_CLUSTER          0x00002000
#define EMBER_AF_METERING_FNF_PUSH_ALL_STATIC_DATA_PREPAYMENT_CLUSTER        0x00004000
#define EMBER_AF_METERING_FNF_NETWORK_KEY_ACTIVE                             0x00008000
#define EMBER_AF_METERING_FNF_DISPLAY_MESSAGE                                0x00010000
#define EMBER_AF_METERING_FNF_CANCEL_ALL_MESSAGES                            0x00020000
#define EMBER_AF_METERING_FNF_CHANGE_SUPPLY                                  0x00040000
#define EMBER_AF_METERING_FNF_LOCAL_CHANGE_SUPPLY                            0x00080000
#define EMBER_AF_METERING_FNF_SET_UNCONTROLLED_FLOW_THRESHOLD                0x00100000
#define EMBER_AF_METERING_FNF_TUNNEL_MESSAGE_PENDING                         0x00200000
#define EMBER_AF_METERING_FNF_GET_SNAPSHOT                                   0x00400000
#define EMBER_AF_METERING_FNF_GET_SAMPLED_DATA                               0x00800000
#define EMBER_AF_METERING_FNF_NEW_SUB_GHZ_CHANNEL_MASKS_AVAILABLE            0x01000000
#define EMBER_AF_METERING_FNF_ENERGY_SCAN_PENDING                            0x02000000
#define EMBER_AF_METERING_FNF_CHANNEL_CHANGE_PENDING                         0x04000000

// Notification Flags 2
#define EMBER_AF_METERING_NF2_PUBLISH_PRICE                                  0x00000001
#define EMBER_AF_METERING_NF2_PUBLISH_BLOCK_PERIOD                           0x00000002
#define EMBER_AF_METERING_NF2_PUBLISH_TARIFF_INFORMATION                     0x00000004
#define EMBER_AF_METERING_NF2_PUBLISH_CONVERSION_FACTOR                      0x00000008
#define EMBER_AF_METERING_NF2_PUBLISH_CALORIFIC_VALUE                        0x00000010
#define EMBER_AF_METERING_NF2_PUBLISH_CO2_VALUE                              0x00000020
#define EMBER_AF_METERING_NF2_PUBLISH_BILLING_PERIOD                         0x00000040
#define EMBER_AF_METERING_NF2_PUBLISH_CONSOLIDATED_BILL                      0x00000080
#define EMBER_AF_METERING_NF2_PUBLISH_PRICE_MATRIX                           0x00000100
#define EMBER_AF_METERING_NF2_PUBLISH_BLOCK_THRESHOLDS                       0x00000200
#define EMBER_AF_METERING_NF2_PUBLISH_CURRENCY_CONVERSION                    0x00000400
#define EMBER_AF_METERING_NF2_PUBLISH_CREDIT_PAYMENT_INFO                    0x00001000
#define EMBER_AF_METERING_NF2_PUBLISH_CPP_EVENT                              0x00002000
#define EMBER_AF_METERING_NF2_PUBLISH_TIER_LABELS                            0x00004000
#define EMBER_AF_METERING_NF2_CANCEL_TARIFF                                  0x00008000

// Notification Flags 3
#define EMBER_AF_METERING_NF3_PUBLISH_CALENDAR                               0x00000001
#define EMBER_AF_METERING_NF3_PUBLISH_SPECIAL_DAYS                           0x00000002
#define EMBER_AF_METERING_NF3_PUBLISH_SEASONS                                0x00000004
#define EMBER_AF_METERING_NF3_PUBLISH_WEEK                                   0x00000008
#define EMBER_AF_METERING_NF3_PUBLISH_DAY                                    0x00000010
#define EMBER_AF_METERING_NF3_CANCEL_CALENDAR                                0x00000020

// Notification Flags 4
#define EMBER_AF_METERING_NF4_SELECT_AVAILABLE_EMERGENCY_CREDIT              0x00000001
#define EMBER_AF_METERING_NF4_CHANGE_DEBT                                    0x00000002
#define EMBER_AF_METERING_NF4_EMERGENCY_CREDIT_SETUP                         0x00000004
#define EMBER_AF_METERING_NF4_CONSUMER_TOP_UP                                0x00000008
#define EMBER_AF_METERING_NF4_CREDIT_ADJUSTMENT                              0x00000010
#define EMBER_AF_METERING_NF4_CHANGE_PAYMENT_MODE                            0x00000020
#define EMBER_AF_METERING_NF4_GET_PREPAY_SNAPSHOT                            0x00000040
#define EMBER_AF_METERING_NF4_GET_TOP_UP_LOG                                 0x00000080
#define EMBER_AF_METERING_NF4_SET_LOW_CREDIT_WARNING_LEVEL                   0x00000100
#define EMBER_AF_METERING_NF4_GET_DEBT_REPAYMENT_LOG                         0x00000200
#define EMBER_AF_METERING_NF4_SET_MAXIMUM_CREDIT_LIMIT                       0x00000400
#define EMBER_AF_METERING_NF4_SET_OVERALL_DEBT_CAP                           0x00000800

// Notification Flags 5
#define EMBER_AF_METERING_NF5_PUBLISH_CHANGE_OF_TENANCY                      0x00000001
#define EMBER_AF_METERING_NF5_PUBLISH_CHANGE_OF_SUPPLIER                     0x00000002
#define EMBER_AF_METERING_NF5_REQUEST_NEW_PASSWORD_1_RESPONSE                0x00000004
#define EMBER_AF_METERING_NF5_REQUEST_NEW_PASSWORD_2_RESPONSE                0x00000008
#define EMBER_AF_METERING_NF5_REQUEST_NEW_PASSWORD_3_RESPONSE                0x00000010
#define EMBER_AF_METERING_NF5_REQUEST_NEW_PASSWORD_4_RESPONSE                0x00000020
#define EMBER_AF_METERING_NF5_UPDATE_SITE_ID                                 0x00000040
#define EMBER_AF_METERING_NF5_RESET_BATTERY_COUNTER                          0x00000080
#define EMBER_AF_METERING_NF5_UPDATE_CIN                                     0x00000100

/**
 * @brief CBKE Library types
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfCbkeKeyEstablishmentSuite
#else
typedef uint16_t EmberAfCbkeKeyEstablishmentSuite;
enum
#endif
{
  EMBER_AF_INVALID_KEY_ESTABLISHMENT_SUITE      = 0x0000,
  EMBER_AF_CBKE_KEY_ESTABLISHMENT_SUITE_163K1   = 0x0001,
  EMBER_AF_CBKE_KEY_ESTABLISHMENT_SUITE_283K1   = 0x0002,
};

/**
 * @brief Device Management plugin types
 */

#define EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_NAME_LENGTH (16u)
#define EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_CONTACT_DETAILS_LENGTH (18u)
#define EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH (32u)
#define EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_CIN_LENGTH (24u)
#define EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PASSWORD_LENGTH (10u)

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfDeviceManagementPasswordType
#else
typedef uint16_t EmberAfDeviceManagementPasswordType;
enum
#endif
{
  UNUSED_PASSWORD   = 0x00,
  SERVICE_PASSWORD  = 0x01,
  CONSUMER_PASSWORD = 0x02,
};

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfDeviceManagementChangePendingFlags
#else
typedef uint8_t EmberAfDeviceManagementChangePendingFlags;
enum
#endif
{
  EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_TENANCY_PENDING_MASK              = 0x01,
  EMBER_AF_DEVICE_MANAGEMENT_CHANGE_OF_SUPPLIER_PENDING_MASK             = 0x02,
  EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SITE_ID_PENDING_MASK                 = 0x04,
  EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CIN_PENDING_MASK                     = 0x08,
  EMBER_AF_DEVICE_MANAGEMENT_UPDATE_SERVICE_PASSWORD_PENDING_MASK        = 0x10,
  EMBER_AF_DEVICE_MANAGEMENT_UPDATE_CONSUMER_PASSWORD_PENDING_MASK       = 0x20,
};

typedef struct {
  // Optional fields only used by Gas Proxy Function plugin.
  uint32_t providerId;
  uint32_t issuerEventId;
  uint8_t tariffType;

  // always used fields
  uint32_t implementationDateTime;
  uint32_t tenancy;
} EmberAfDeviceManagementTenancy;

typedef struct {
  uint32_t proposedProviderId;
  uint32_t implementationDateTime;
  uint32_t providerChangeControl;
  uint8_t proposedProviderName[EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_NAME_LENGTH + 1];
  uint8_t proposedProviderContactDetails[EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PROPOSED_PROVIDER_CONTACT_DETAILS_LENGTH + 1];
} EmberAfDeviceManagementSupplier;

typedef struct {
  uint32_t requestDateTime;
  uint32_t implementationDateTime;
  uint8_t supplyStatus;
  uint8_t originatorIdSupplyControlBits;
} EmberAfDeviceManagementSupply;

typedef struct {
  uint8_t siteId[EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_SITE_ID_LENGTH + 1];
  uint32_t implementationDateTime;
  uint32_t issuerEventId;
} EmberAfDeviceManagementSiteId;

typedef struct {
  uint8_t cin[EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_CIN_LENGTH + 1];
  uint32_t implementationDateTime;
  uint32_t issuerEventId;
} EmberAfDeviceManagementCIN;

typedef struct {
  bool supplyTamperState;
  bool supplyDepletionState;
  bool supplyUncontrolledFlowState;
  bool loadLimitSupplyState;
} EmberAfDeviceManagementSupplyStatusFlags;

typedef struct {
  uint16_t uncontrolledFlowThreshold;
  uint16_t multiplier;
  uint16_t divisor;
  uint16_t measurementPeriod;
  uint8_t unitOfMeasure;
  uint8_t stabilisationPeriod;
} EmberAfDeviceManagementUncontrolledFlowThreshold;

typedef struct {
  uint32_t implementationDateTime;
  uint8_t supplyStatus;
} EmberAfDeviceManagementSupplyStatus;

typedef struct {
  uint8_t password[EMBER_AF_DEVICE_MANAGEMENT_MAXIMUM_PASSWORD_LENGTH + 1];
  uint32_t implementationDateTime;
  uint16_t durationInMinutes;
  EmberAfDeviceManagementPasswordType passwordType;
} EmberAfDeviceManagementPassword;

typedef struct {
  EmberAfDeviceManagementTenancy tenancy;
  EmberAfDeviceManagementSupplier supplier;
  EmberAfDeviceManagementSupply supply;
  EmberAfDeviceManagementSiteId siteId;
  EmberAfDeviceManagementCIN cin;
  EmberAfDeviceManagementSupplyStatusFlags supplyStatusFlags;
  EmberAfDeviceManagementSupplyStatus supplyStatus;
  //TODO: These passwords ought to be tokenized / hashed
  EmberAfDeviceManagementPassword servicePassword;
  EmberAfDeviceManagementPassword consumerPassword;
  EmberAfDeviceManagementUncontrolledFlowThreshold threshold;
  uint32_t providerId;
  uint32_t issuerEventId;
  uint8_t proposedLocalSupplyStatus;
  EmberAfTariffType tariffType;
  EmberAfDeviceManagementChangePendingFlags pendingUpdates;
} EmberAfDeviceManagementInfo;

typedef struct {
  uint8_t startIndex;
  uint8_t endIndex;
} EmberAfDeviceManagementAttributeRange;

// attrRange is a list of attributeId values in a cluster. It's needed to track contigous
// segments of valid attributeId's with gaps in the middle.
// attributeSetId is the value of the upper byte in the attributeId. It ranges from 0x01(Price)
// to 0x08(OTA Event Configuration)
// Eg. {0x00,0x05} and {0x08,0x0A}
// We're betting that there isn't a list of cluster attributes that has more than 5 gaps.
typedef struct {
  uint8_t attributeSetId;
  EmberAfDeviceManagementAttributeRange attributeRange[7];
} EmberAfDeviceManagementAttributeTable;

typedef struct {
  bool encryption;

  uint8_t * plainPayload;
  uint16_t plainPayloadLength;

  uint8_t * encryptedPayload;
  uint16_t encryptedPayloadLength;
} EmberAfGbzMessageData;

typedef struct {
  uint8_t * gbzCommands;
  uint16_t  gbzCommandsLength;
  uint8_t * gbzCommandsResponse;
  uint16_t  gbzCommandsResponseLength;
  uint16_t  messageCode;
} EmberAfGpfMessage;

/**
 * @brief Zigbee Internet Client/Server Remote Cluster Types
 */
typedef uint16_t EmberAfRemoteClusterType;

#define EMBER_AF_REMOTE_CLUSTER_TYPE_NONE     0x0000u
#define EMBER_AF_REMOTE_CLUSTER_TYPE_SERVER   0x0001u
#define EMBER_AF_REMOTE_CLUSTER_TYPE_CLIENT   0x0002u
#define EMBER_AF_REMOTE_CLUSTER_TYPE_INVALID  0xFFFFu

/**
 * @brief Zigbee Internet Client/Server remote cluster struct.
 */
typedef struct {
  EmberAfClusterId clusterId;
  EmberAfProfileId profileId;
  uint16_t deviceId;
  uint8_t endpoint;
  EmberAfRemoteClusterType type;
} EmberAfRemoteClusterStruct;

/**
 * @brief Zigbee Internet Client/Server Remote Binding struct
 */
typedef struct {
  EmberEUI64 targetEUI64;
  uint8_t sourceEndpoint;
  uint8_t destEndpoint;
  uint16_t clusterId;
  EmberEUI64 destEUI64;
  EmberEUI64 sourceEUI64;
} EmberAfRemoteBindingStruct;

typedef struct {
  EmberAfClusterId clusterId;
  bool server;
} EmberAfClusterInfo;

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_DEVICE_DATABASE_PRESENT
#include "device-database-config.h"
#else
#define EMBER_AF_MAX_CLUSTERS_PER_ENDPOINT 3u
#define EMBER_AF_MAX_ENDPOINTS_PER_DEVICE  1u
#endif
#else // UC_BUILD
#if !defined(EMBER_AF_MAX_CLUSTERS_PER_ENDPOINT)
#define EMBER_AF_MAX_CLUSTERS_PER_ENDPOINT 3u
#define EMBER_AF_MAX_ENDPOINTS_PER_DEVICE  1u
#endif
#endif // UC_BUILD
/**
 * @brief Contain basic information about an endpoint.
 */
typedef struct {
  EmberAfClusterInfo clusters[EMBER_AF_MAX_CLUSTERS_PER_ENDPOINT];
  EmberAfProfileId profileId;
  uint16_t deviceId;
  uint8_t endpoint;
  uint8_t clusterCount;
} EmberAfEndpointInfoStruct;

// Although we treat this like a bitmap, only 1 bit is set at a time.
// We use the bitmap feature to allow us to find all devices
// with any in a set of status codes using
// emberAfPluginDeviceDatabaseFindDeviceByStatus().
typedef enum {
  EMBER_AF_DEVICE_DISCOVERY_STATUS_NONE                 = 0x00,
  EMBER_AF_DEVICE_DISCOVERY_STATUS_NEW                  = 0x01,
  EMBER_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS       = 0x02,
  EMBER_AF_DEVICE_DISCOVERY_STATUS_FIND_CLUSTERS        = 0x04,
  EMBER_AF_DEVICE_DISCOVERY_STATUS_FIND_STACK_REVISION  = 0x08,

  EMBER_AF_DEVICE_DISCOVERY_STATUS_DONE      = 0x40,
  EMBER_AF_DEVICE_DISCOVERY_STATUS_FAILED    = 0x80,
} EmberAfDeviceDiscoveryStatus;

/**
 * @brief Contain endpoint information about a device.
 */
typedef struct {
  EmberEUI64 eui64;
  EmberAfEndpointInfoStruct endpoints[EMBER_AF_MAX_ENDPOINTS_PER_DEVICE];
  EmberAfDeviceDiscoveryStatus status;
  uint8_t discoveryFailures;
  uint8_t capabilities;
  uint8_t endpointCount;
  uint8_t stackRevision;
} EmberAfDeviceInfo;

typedef struct {
  uint16_t deviceIndex;
} EmberAfDeviceDatabaseIterator;

typedef struct {
  EmberNodeId emberNodeId;
  uint32_t timeStamp;
} EmberAfJoiningDevice;

#define EMBER_AF_INVALID_CLUSTER_ID 0xFFFFu

#define EMBER_AF_INVALID_ENDPOINT 0xFFu

#define EMBER_AF_INVALID_PAN_ID 0xFFFFu

/**
 * @brief Permit join times.
 */
#define EMBER_AF_PERMIT_JOIN_FOREVER     0xFFu
#define EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT 0xFEu

/**
 * @brief The overhead of the ZDO response.
 *   1 byte for the sequence and 1 byte for the status code.
 */
#define EMBER_AF_ZDO_RESPONSE_OVERHEAD 2u

/** @} END addtogroup */

#endif // SILABS_AF_API_TYPES
