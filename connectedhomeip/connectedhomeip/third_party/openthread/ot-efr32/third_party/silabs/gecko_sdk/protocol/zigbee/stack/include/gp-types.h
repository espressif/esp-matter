/***************************************************************************//**
 * @file
 * @brief Zigbee Green Power types and defines.
 * See @ref greenpower for documentation.
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

/**
 * @addtogroup gp_types
 *
 * This file defines data types relevant to the Green Power implementation.
 *
 * See gp-types.h for source code.
 * @{
 */

#ifndef SILABS_GP_TYPES_H
#define SILABS_GP_TYPES_H

// Mask used in the Gp Ep Incoming Message Hadler to pass bidirectional information
// Incoming GPDF has rxAfterTx bit set
#define EMBER_GP_BIDIRECTION_INFO_RX_AFTER_TX_MASK        0x01
// A tx queue is available at gp stub to hold an outgoing GPDF
#define EMBER_GP_BIDIRECTION_INFO_TX_QUEUE_AVAILABLE_MASK 0x02
/**
 * @name GP Types
 */
//@{
/** 32-bit source identifier. */
typedef uint32_t EmberGpSourceId;
/** 32-bit security frame counter */
typedef uint32_t EmberGpSecurityFrameCounter;
typedef uint32_t EmberGpMic;

/** @brief Options to use when sending a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberGpSecurityLevel
#else
typedef uint8_t EmberGpSecurityLevel;
enum
#endif
{
  /** None  */
  EMBER_GP_SECURITY_LEVEL_NONE = 0x00,
  /** reserved  */
  EMBER_GP_SECURITY_LEVEL_RESERVED = 0x01,
  /** 4 Byte Frame Counter + 4 Byte MIC */
  EMBER_GP_SECURITY_LEVEL_FC_MIC = 0x02,
  /** 4 Byte Frame Counter + 4 Byte MIC + encryption */
  EMBER_GP_SECURITY_LEVEL_FC_MIC_ENCRYPTED = 0x03,
};

/** @brief Options to use when sending a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberGpKeyType
#else
typedef uint8_t EmberGpKeyType;
enum
#endif
{
  /** None  */
  EMBER_GP_SECURITY_KEY_NONE = 0x00,
  /** reserved  */
  EMBER_GP_SECURITY_KEY_NWK = 0x01,
  EMBER_GP_SECURITY_KEY_GPD_GROUP = 0x02,
  EMBER_GP_SECURITY_KEY_NWK_DERIVED = 0x03,
  EMBER_GP_SECURITY_KEY_GPD_OOB = 0x04,
  EMBER_GP_SECURITY_KEY_GPD_DERIVED = 0x07,
};

/** @brief Options to use when sending a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberGpApplicationId
#else
typedef uint8_t EmberGpApplicationId;
enum
#endif
{
  /** Source identifier. */
  EMBER_GP_APPLICATION_SOURCE_ID = 0x00,
  /** IEEE address. */
  EMBER_GP_APPLICATION_IEEE_ADDRESS = 0x02,
};

#define EMBER_GP_APPLICATION_ID_MASK 0x03
/**
 * @brief GP proxy table entry status.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberGpProxyTableEntryStatus
#else
typedef uint8_t EmberGpProxyTableEntryStatus;
enum
#endif
{
  /**
   * The GP table entry is in use for a Proxy Table Entry.
   */
  EMBER_GP_PROXY_TABLE_ENTRY_STATUS_ACTIVE = 0x01,
  /**
   * The proxy table entry is not in use.
   */
  EMBER_GP_PROXY_TABLE_ENTRY_STATUS_UNUSED = 0xFF,
};

/**
 * @brief GP sink table entry status.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberGpSinkTableEntryStatus
#else
typedef uint8_t EmberGpSinkTableEntryStatus;
enum
#endif
{
  /**
   * The GP table entry is in use for a Sink Table Entry.
   */
  EMBER_GP_SINK_TABLE_ENTRY_STATUS_ACTIVE = 0x01,
  /**
   * The proxy table entry is not in use.
   */
  EMBER_GP_SINK_TABLE_ENTRY_STATUS_UNUSED = 0xFF,
};

/** @brief Address for sending and receiving a message. */
typedef struct {
  union {
    /** The IEEE address is used when the application identifier is
     *  ::EMBER_GP_APPLICATION_IEEE_ADDRESS.
     */
    EmberEUI64 gpdIeeeAddress;
    /** The 32-bit source identifier is used when the application identifier is
     *  ::EMBER_GP_APPLICATION_SOURCE_ID.
     *
     */
    EmberGpSourceId sourceId;
  } id;
  /** Application identifier of the GPD. */
  EmberGpApplicationId applicationId;
  uint8_t endpoint;
} EmberGpAddress;

typedef enum {
  EMBER_GP_SINK_TYPE_FULL_UNICAST,
  EMBER_GP_SINK_TYPE_D_GROUPCAST,
  EMBER_GP_SINK_TYPE_GROUPCAST,
  EMBER_GP_SINK_TYPE_LW_UNICAST,
  EMBER_GP_SINK_TYPE_SINK_GROUPLIST,    // Sink Group List

  EMBER_GP_SINK_TYPE_UNUSED = 0xFF
} EmberGpSinkType;

typedef struct {
  EmberEUI64 sinkEUI;
  EmberNodeId sinkNodeId;
} EmberGpSinkAddress;

typedef struct {
  uint16_t groupID;
  uint16_t alias;
} EmberGpSinkGroup;

typedef struct {
  EmberGpSinkType type;

  union {
    EmberGpSinkAddress unicast;
    EmberGpSinkGroup groupcast;
    EmberGpSinkGroup groupList;   // Entry for Sink Group List
  } target;
} EmberGpSinkListEntry;

//minimum required by the spec is 2
#define GP_SINK_LIST_ENTRIES 2

/** @brief The internal representation of a proxy or sink table entry.
 */
typedef struct {
  /**
   * Internal status. Defines if the entry is unused or used as a proxy entry
   */
  EmberGpProxyTableEntryStatus status;
  /**
   * The tunneling options (this contains both options and extendedOptions from the spec)
   */
  uint32_t options;
  /**
   * The addressing info of the GPD
   */
  EmberGpAddress gpd;
  /**
   * The assigned alias for the GPD
   */
  EmberNodeId assignedAlias;
  /**
   * The security options field
   */
  uint8_t securityOptions;
  /**
   * The SFC of the GPD
   */
  EmberGpSecurityFrameCounter gpdSecurityFrameCounter;
  /**
   * The key
   */
  EmberKeyData gpdKey;
  /**
   * The list of sinks  (hardcoded to 2 which is the spec minimum)  (maybe we should indirect this?)
   */
  EmberGpSinkListEntry sinkList[GP_SINK_LIST_ENTRIES];
  /**
   * The groupcast radius
   */
  uint8_t groupcastRadius;
  /**
   * The search counter
   */
  uint8_t searchCounter;
} EmberGpProxyTableEntry;

/** @brief The internal representation of a proxy or sink table entry.
 */
typedef struct {
  /**
   * Internal status. Defines if the entry is unused or used as a sink table entry
   */
  EmberGpSinkTableEntryStatus status;
  /**
   * The tunneling options (this contains both options and extendedOptions from the spec)
   */
  uint16_t options;
  /**
   * The addressing info of the GPD
   */
  EmberGpAddress gpd;
  /**
   * The device id for the GPD
   */
  uint8_t deviceId;
  /**
   * The list of sinks  (hardcoded to 2 which is the spec minimum)  (maybe we should indirect this?)
   */
  EmberGpSinkListEntry sinkList[GP_SINK_LIST_ENTRIES];
  /**
   * The assigned alias for the GPD
   */
  EmberNodeId assignedAlias;
  /**
   * The groupcast radius
   */
  uint8_t groupcastRadius;
  /**
   * The security options field
   */
  uint8_t securityOptions;
  /**
   * The SFC of the GPD
   */
  EmberGpSecurityFrameCounter gpdSecurityFrameCounter;
  /**
   * The key
   */
  EmberKeyData gpdKey;
} EmberGpSinkTableEntry;

/**
 * @brief GP sink table entry status.
 */
//typedef  EmberGpProxyTableEntry EmberGpSinkTableEntry;

/**
 * @brief GP sink table entry status. TODO
 *    R E M O V E
 */

/** @brief Options to use when sending a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberCGpTxOption
#else
typedef uint8_t EmberCGpTxOption;
enum
#endif
{
  /** No options. */
  EMBER_CGP_TX_OPTION_NONE = 0x00,
  /** Use CSMA/CA. */
  EMBER_CGP_TX_OPTION_USE_CSMA_CA = 0x01,
  /** Use MAC ACK. */
  EMBER_CGP_TX_OPTION_USE_MAC_ACK = 0x02,
  /** Reserved. */
  EMBER_CGP_TX_OPTION_RESERVED = 0xFC,
};

/** @brief Addressing modes for sending and receiving a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberCGpAddressMode
#else
typedef uint8_t EmberCGpAddressMode;
enum
#endif
{
  /** No address (PAN identifier and address omitted). */
  EMBER_CGP_ADDRESS_MODE_NONE = 0x00,
  /** Reserved. */
  EMBER_CGP_ADDRESS_MODE_RESERVED = 0x01,
  /** 16-bit short address. */
  EMBER_CGP_ADDRESS_MODE_SHORT = 0x02,
  /** 64-bit extended address. */
  EMBER_CGP_ADDRESS_MODE_EXTENDED = 0x03,
};

/** @brief Address for sending and receiving a message. */
typedef struct {
  /** The address. */
  union {
    /** The 16-bit short address is used when the mode is
     *  ::EMBER_CGP_ADDRESS_MODE_SHORT.
     */
    EmberNodeId shortId;
    /** The 64-bit extended address is used when the mode is
     *  ::EMBER_CGP_ADDRESS_MODE_EXTENDED.
     */
    EmberEUI64 extendedId;
  } address;
  /** The PAN identifier is used when the mode is not
   *  ::EMBER_CGP_ADDRESS_MODE_NONE.
   */
  EmberPanId panId;
  /** The addressing mode. */
  EmberCGpAddressMode mode;
} EmberCGpAddress;

/*S @brief Options to use when sending a message. */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberDGpTxOption
#else
typedef uint8_t EmberDGpTxOption;
enum
#endif
{
  /** No options. */
  EMBER_DGP_TX_OPTION_NONE = 0x00,
  /** Use gpTxQueue. */
  EMBER_DGP_TX_OPTION_USE_GP_TX_QUEUE = 0x01,
  /** Use CSMA/CA. */
  EMBER_DGP_TX_OPTION_USE_CSMA_CA = 0x02,
  /** Use MAC ACK. */
  EMBER_DGP_TX_OPTION_USE_MAC_ACK = 0x04,
  /** Data frame. */
  EMBER_DGP_TX_OPTION_FRAME_TYPE_DATA = 0x00,
  /** Maintenance frame. */
  EMBER_DGP_TX_OPTION_FRAME_TYPE_MAINTENANCE = 0x08,
  /** Reserved. */
  EMBER_DGP_TX_OPTION_RESERVED = 0xE0,
};

typedef struct {
  uint8_t foo;
} EmberGpProxyClusterAttributes;

typedef struct {
  bool inUse;
  bool useCca;
  EmberGpAddress addr;
  uint8_t gpdCommandId;
  EmberMessageBuffer asdu;
  uint8_t gpepHandle;
  uint16_t queueEntryLifetimeMs;
} EmberGpTxQueueEntry;

//these defines are ugly, but there are a bunch of callbacks that take the same set of arguments
//while the design is still in flux, it makes sense to only have one place to change them

#define GP_PARAMS                         \
  EmberStatus status,                     \
  uint8_t gpdLink,                        \
  uint8_t sequenceNumber,                 \
  EmberGpAddress * addr,                  \
  EmberGpSecurityLevel gpdfSecurityLevel, \
  EmberGpKeyType gpdfSecurityKeyType,     \
  bool autoCommissioning,                 \
  uint8_t bidirectionalInfo,              \
  uint32_t gpdSecurityFrameCounter,       \
  uint8_t gpdCommandId,                   \
  uint32_t mic,                           \
  uint8_t proxyTableIndex,                \
  uint8_t gpdCommandPayloadLength,        \
  uint8_t * gpdCommandPayload

#define GP_ARGS            \
  status,                  \
  gpdLink,                 \
  sequenceNumber,          \
  addr,                    \
  gpdfSecurityLevel,       \
  gpdfSecurityKeyType,     \
  autoCommissioning,       \
  bidirectionalInfo,       \
  gpdSecurityFrameCounter, \
  gpdCommandId,            \
  mic,                     \
  proxyTableIndex,         \
  gpdCommandPayloadLength, \
  gpdCommandPayload

#define GP_UNUSED_ARGS           \
  (void)status;                  \
  (void)gpdLink;                 \
  (void)sequenceNumber;          \
  (void)addr;                    \
  (void)gpdfSecurityLevel;       \
  (void)gpdfSecurityKeyType;     \
  (void)autoCommissioning;       \
  (void)rxAfterTx;               \
  (void)gpdSecurityFrameCounter; \
  (void)gpdCommandId;            \
  (void)mic;                     \
  (void)proxyTableIndex;         \
  (void)gpdCommandPayloadLength; \
  (void)gpdCommandPayload;

#define GP_PROXY_TABLE_OPTIONS_IN_RANGE (BIT(10))

//3 consumed by device id, options, and app info byte
#define GP_COMMISSIONING_MAX_BYTES (55 - 3)

typedef struct {
  uint8_t deviceId;
  uint8_t applInfoBitmap;
  uint16_t manufacturerId;
  uint16_t modelId;
  uint8_t numberOfGpdCommands;
  uint8_t gpdCommands[GP_COMMISSIONING_MAX_BYTES - 1];
  uint8_t numberOfPairedEndpoints;
  uint8_t pairedEndpoints[GP_COMMISSIONING_MAX_BYTES - 1];
  uint8_t numberOfGpdClientCluster;
  uint8_t numberOfGpdServerCluster;
  uint16_t serverClusters[15];
  uint16_t clientClusters[15];
} EmberGpApplicationInfo;

// GP types for the Multisensor Extn. and CAR
// Switch Information and Sensor type
typedef struct {
  uint16_t              attributeID;
  uint8_t               attributeDataType;
  // careful it is "N-1", total number of octet -1 of following attrib record field
  // thus is there is 0b0000 indicates that 1 octet will follows, this allow sink
  // to skip octets base on AttributID
  // after read AttributID the sink could go directly to next attribut if it doesn't support this one
  uint8_t               remainingAttributRecordLength; // on 4 bits
  uint8_t               reported;
  uint8_t               attrValuePresent;

  uint8_t               attrOffsetWithinReport; // demarre a 0
  // TODO change "attrValue" from u32 to a pointer, as the data could be up to a string
  // the field "attributeDataType" tell with format it is, among  more than 55 possibilities)
  // so a add a private field which just tell the lenght of attrValue to easier
  // process it
  uint32_t              attrValue;
  uint8_t               attrValueSize;
} EmberGpAttributeRecord;

typedef struct {
  // careful it is "N-1", if there is 2 attr record then nbAttrRecord=1 and not 2 !
  uint8_t               nbAttrRecord;
  uint8_t               clientServer;
  uint8_t               ManIdPresent;
  uint8_t               datapointOptions;
  uint16_t              clusterID;
  uint16_t              manufacturerID;
  EmberGpAttributeRecord attrRecord;
} EmberGpDataPointDescriptor;

typedef struct {
  uint8_t               repId;
  uint8_t               reportOptions;
  uint16_t              timeoutPeriod;
  uint8_t               totalLength;
  EmberGpDataPointDescriptor  dataPoint;
} EmberGpReportDescriptor;

typedef struct {
  uint8_t switchInfoLength;
  uint8_t nbOfContacts;
  uint8_t switchType;
  uint8_t currentContact;
  uint8_t savedContact;
}EmberGpSwitchInformation;

#define TT_NB_MAX_OPTION_RECORD                                         (8)
typedef struct {
  // button information of the Generic switch,
  // tell the contact saved
  uint8_t       currentStatus;
  // tell how to evaluate contact received, thus bitmask only depend of NbContact of this GPD
  uint8_t       contactBitmask;
} EmberGpGenericSwitchData;

typedef struct {
  uint8_t       totalLengthOfAddInfoBlock;
  uint8_t       optionSelector;
  union {
    struct {
      uint8_t       reportIdentifier;
      uint8_t       attrOffsetWithinReport;
      uint16_t      clusterID;
      uint16_t      attributeID;
      uint8_t       attributeDataType;
      uint8_t       attributeOptions; //clientServer bit0 manufacturerIdPresent bit1
      uint16_t      manufacturerID;
    } compactAttr;
    struct {
      // button information of the Generic switch,
      // tell the contact saved
      uint8_t       contactStatus;
      // tell how to evaluate contact received, thus bitmask only depend of NbContact of this GPD
      uint8_t       contactBitmask;
    } genericSwitch;
  } optionData;//EmberGpTranslationTableAdditionalInfoBlockOptionRecordOptionDataField;
} EmberGpTranslationTableAdditionalInfoBlockOptionRecordField;

//@} \\END GP Types

// GPD SrcID reserved values
#define GP_GPD_SRC_ID_RESERVED_0 0x00000000
#define GP_GPD_SRC_ID_RESERVED_FFFFFF9 0xFFFFFFF9
#define GP_GPD_SRC_ID_RESERVED_FFFFFFA 0xFFFFFFFA
#define GP_GPD_SRC_ID_RESERVED_FFFFFFB 0xFFFFFFFB
#define GP_GPD_SRC_ID_RESERVED_FFFFFFC 0xFFFFFFFC
#define GP_GPD_SRC_ID_RESERVED_FFFFFFD 0xFFFFFFFD
#define GP_GPD_SRC_ID_RESERVED_FFFFFFE 0xFFFFFFFE

#define GP_DERIVED_GROUP_ALIAS_NOT_USED 0xffff

#define GP_DMIN_B 32
#define GP_DMIN_U 5
#define GP_DMAX 100

EmberStatus emberDGpSend(bool action,
                         bool useCca,
                         EmberGpAddress *addr,
                         uint8_t gpdCommandId,
                         uint8_t gpdAsduLength,
                         uint8_t const *gpdAsdu,
                         uint8_t gpepHandle,
                         uint16_t gpTxQueueEntryLifetimeMs);

bool emberGpProxyTableProcessGpPairing(uint32_t options,
                                       EmberGpAddress* addr,
                                       uint8_t commMode,
                                       uint16_t sinkNwkAddress,
                                       uint16_t sinkGroupId,
                                       uint16_t assignedAlias,
                                       uint8_t* sinkIeeeAddress,
                                       EmberKeyData *gpdKey,
                                       uint32_t gpdSecurityFrameCounter,
                                       uint8_t forwardingRadius);

uint8_t emberGpProxyTableLookup(EmberGpAddress *addr);
EmberStatus emberGpProxyTableGetEntry(uint8_t proxyIndex,
                                      EmberGpProxyTableEntry *entry);

EmberStatus emberGpSinkTableGetEntry(uint8_t sinkIndex,
                                     EmberGpSinkTableEntry *entry);
uint8_t emberGpSinkTableLookup(EmberGpAddress *addr);
void emberGpSinkTableClearAll(void);

EmberStatus emberGpSinkTableSetEntry(uint8_t sinkTableIndex,
                                     EmberGpSinkTableEntry *entry);
uint8_t emberGpSinkTableFindOrAllocateEntry(EmberGpAddress *addr);
void emberGpSinkTableRemoveEntry(uint8_t index);
void emberGpSinkTableInit(void);
void emberGpSinkTableSetSecurityFrameCounter(uint8_t index,
                                             uint32_t sfc);
// Tx Queue APIs
MessageBufferQueue * emberGpGetTxQueueHead(void);
void emberGpSetMaxTxQueueEntry(uint16_t maxEntries);
uint16_t emberGetGpMaxTxQListCount(void);
uint16_t emberGetGpTxQListCount(void);
EmberMessageBuffer emberGpGetTxQueueEntryFromQueue(EmberGpTxQueueEntry *txQueue,
                                                   uint8_t *data,
                                                   uint16_t *dataLength,
                                                   uint16_t allocatedDataLength);
EmberMessageBuffer emberGpAddGpTxQueueEntryWithPayload(EmberGpTxQueueEntry *txQueue,
                                                       uint8_t *data,
                                                       uint16_t dataLength);
bool emberGpRemoveFromTxQueue(EmberGpTxQueueEntry *txQueue);
void emberGpClearTxQueue(void);

#endif // SILABS_GP_TYPES_H

/** @} END addtogroup
 */
