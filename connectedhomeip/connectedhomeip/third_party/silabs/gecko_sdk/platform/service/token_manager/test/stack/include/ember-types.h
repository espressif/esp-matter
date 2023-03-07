/***************************************************************************//**
 * @brief Ember Connect data type definitions.
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

#ifndef __EMBER_TYPES_H__
#define __EMBER_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "ember-configuration-defaults.h"
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Size of an extended PAN identifier in bytes (8).
 */
#define EXTENDED_PAN_ID_SIZE 8

/**
 * @brief Size of EUI64 (an IEEE address) in bytes (8).
 */
#define EUI64_SIZE 8

/**
 * @brief Size of an encryption key in bytes (16).
 */
#define EMBER_ENCRYPTION_KEY_SIZE 16

/**
 * @brief EUI 64-bit ID (an IEEE address).
 */
typedef uint8_t EmberEUI64[EUI64_SIZE];

/**
 * @brief 802.15.4 node ID.
 */
typedef uint16_t EmberNodeId;

/**
 * @brief 802.15.4 PAN ID.
 */
typedef uint16_t EmberPanId;

/**
 * @brief A distinguished network ID that will never be assigned
 * to any node.  Used to indicate the absence of a node ID.
 */
#define EMBER_NULL_NODE_ID                0xFFFFu

/** @brief Broadcast address. */
#define EMBER_BROADCAST_ADDRESS           0xFFFF

/** @brief Special short address indicating the node should use long addressing
 * as source address. */
#define EMBER_USE_LONG_ADDRESS            0xFFFE

/** @brief The coordinator short address. */
#define EMBER_COORDINATOR_ADDRESS         0x0000

/**
 * @brief Defines the possible types of nodes and the roles that a
 * node might play in a network.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberNodeType
#else
typedef uint8_t EmberNodeType;
enum
#endif
{
  /** Device is not joined. */
  EMBER_UNKNOWN_DEVICE = 0,
  /** Star or extended star topology device: will relay messages and can act as
   * a parent to range extender and  end device nodes.
   */
  EMBER_STAR_COORDINATOR = 1,
  /** Star or extended star topology device: will relay messages and can act as
   * a parent to end device nodes.
   */
  EMBER_STAR_RANGE_EXTENDER = 2,
  /** Star or extended star topology device: communicates only with its parent
   * and will not relay messages.
   */
  EMBER_STAR_END_DEVICE = 3,
  /** Star or extended star topology device: an end device whose radio can be
   * turned off to save power. The application must call emberPollForData() to
   * receive messages.
   */
  EMBER_STAR_SLEEPY_END_DEVICE = 4,
  /** A device able to send and receive messages from other devices in range on
   * the same PAN ID, with no star topology restrictions. Such device does not
   * relay messages. A node can be started as a direct device by using the
   * emberJoinCommissioned() API.
   */
  EMBER_DIRECT_DEVICE = 5,
  /** A device able to send and receive MAC-level messages. A node can be
   * started as a MAC mode device by using the emberJoinCommissioned() API or
   * the emberJoinNetworkExtended() API.
   */
  EMBER_MAC_MODE_DEVICE = 6,
  /** A sleepy device able to send and receive MAC-level messages. A node can be
   * started as a sleepy MAC mode device by using the emberJoinCommissioned()
   * API or the emberJoinNetworkExtended() API.
   */
  EMBER_MAC_MODE_SLEEPY_DEVICE = 7,
};

/**
 * @brief Defines the possible join states for a node.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberNetworkStatus
#else
typedef uint8_t EmberNetworkStatus;
enum
#endif
{
  /** The node is not associated with a network in any way. */
  EMBER_NO_NETWORK,
  /** The node is currently attempting to join a network. */
  EMBER_JOINING_NETWORK,
  /** The node is joined to a network. */
  EMBER_JOINED_NETWORK,
};

/** @brief Holds network parameters.
 *
 * For information about power settings and radio channels,
 * see the technical specification for the
 * RF communication module in your Developer Kit.
 */
typedef struct {
  /** The network's PAN identifier.*/
  uint16_t  panId;
  /** A power setting, in dBm.*/
  int8_t   radioTxPower;
  /** A radio channel. Be sure to specify a channel supported by the radio. */
  uint8_t   radioChannel;
} EmberNetworkParameters;

/**
 * @brief Child flags.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberChildFlags
#else
typedef uint8_t EmberChildFlags;
enum
#endif
{
  /** The child is a range extender. */
  EMBER_CHILD_FLAGS_DEVICE_IS_RANGE_EXTENDER_BIT       = 0x02,
  /** The child is a sleepy end device. */
  EMBER_CHILD_FLAGS_DEVICE_IS_SLEEPY_BIT               = 0x04,
  /** The node has data pending for the child. */
  EMBER_CHILD_FLAGS_HAVE_PENDING_DATA_BIT              = 0x08,
  /** The child supports AES-based security. */
  EMBER_CHILD_FLAGS_AES_SECURITY_CAPABLE_BIT           = 0x10,
  /** The child supports XXTEA-based security. */
  EMBER_CHILD_FLAGS_XXTEA_SECURITY_CAPABLE_BIT         = 0x20,
};

/**
 * @brief Message length in bytes.
 */
typedef uint8_t EmberMessageLength;

/**
 * @brief Message options.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberMessageOptions
#else
typedef uint8_t EmberMessageOptions;
enum
#endif
{
  /** No options. */
  EMBER_OPTIONS_NONE                     = 0x00,
  /** The packet should be sent out encrypted. */
  EMBER_OPTIONS_SECURITY_ENABLED         = 0x01,
  /** An acknowledgment should be requested for the outgoing packet. */
  EMBER_OPTIONS_ACK_REQUESTED            = 0x02,
  /** The packet should be sent with high priority. */
  EMBER_OPTIONS_HIGH_PRIORITY            = 0x04,
  /** The packet should be sent via the indirect queue. This option only applies
     to nodes with EMBER_MAC_MODE_DEVICE or EMBER_MAC_MODE_SLEEPY_DEVICE
     node type. */
  EMBER_OPTIONS_INDIRECT                 = 0x08,
};

/**
 * @brief An instance of this structure is passed to
 * emberIncomingMessageHandler(). It describes the incoming message.
 */
typedef struct {
  /**
   * An EmberMessageOptions value indicating the options used for the incoming
   * packet.
   */
  EmberMessageOptions options;
  /**
   * An EmberNodeId value indicating source node ID.
   */
  EmberNodeId source;
  /**
   * The endpoint the message is destined to.
   */
  uint8_t endpoint;
  /**
   * The RSSI in dBm the packet was received with.
   */
  int8_t rssi;
  /**
   * An EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
} EmberIncomingMessage;

/**
 * @brief An instance of this structure is passed to emberMessageSentHandler().
 * It describes the outgoing packet.
 */
typedef struct {
  /**
   * An EmberMessageOptions value indicating the options used for transmitting
   * the outgoing message.
   */
  EmberMessageOptions options;
  /**
   * An EmberNodeId value indicating the destination short ID.
   */
  EmberNodeId destination;
  /**
   * The endpoint the message is destined to.
   */
  uint8_t endpoint;
  /**
   * A tag value the application can use to match emberMessageSend() calls to
   * the corresponding emberMessageSentHandler() calls.
   */
  uint8_t tag;
  /**
   * An EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The RSSI in dBm of the ACK corresponding to this message. This field is
   * meaningful only if EMBER_OPTIONS_ACK_REQUESTED flag is set in the options
   * field.
   */
  int8_t ackRssi;
} EmberOutgoingMessage;

/**
 * @brief 802.15.4 addressing mode.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberMacAddressMode
#else
typedef uint8_t EmberMacAddressMode;
enum
#endif
{
  EMBER_MAC_ADDRESS_MODE_NONE  = 0x00,
  EMBER_MAC_ADDRESS_MODE_SHORT = 0x02,
  EMBER_MAC_ADDRESS_MODE_LONG  = 0x03,
};

/**
 * @brief A structure that stores an 802.15.4 address.
 */
typedef struct {
  union {
    uint8_t longAddress[EUI64_SIZE];
    uint16_t shortAddress;
  } addr;

  EmberMacAddressMode mode;
} EmberMacAddress;

/**
 * @brief A structure that describes the addressing fields of a 802.15.4
 * frame.
 */
typedef struct {
  /**
   * An EmberMacAddress structure indicating the source address of a MAC frame.
   */
  EmberMacAddress srcAddress;
  /**
   * An EmberMacAddress structure indicating the destination address of a MAC
   * frame.
   */
  EmberMacAddress dstAddress;
  /**
   * An EmberPanId struct indicating the source PAN ID of a MAC frame. This
   * field is meaningful only if srcPanIdSpecified is set to true.
   */
  EmberPanId srcPanId;
  /**
   * An EmberPanId struct indicating the destination PAN ID of a MAC frame.
   * This field is meaningful only if dstPanIdSpecified is set to true.
   */
  EmberPanId dstPanId;
  /**
   * True if the srcPanId field is set, false otherwise.
   */
  bool srcPanIdSpecified;
  /**
   * True if the dstPanId field is set, false otherwise.
   */
  bool dstPanIdSpecified;
} EmberMacFrame;

/**
 * @brief An instance of this structure is passed to
 * emberIncomingMacMessageHandler(). It describes the incoming MAC frame.
 */
typedef struct {
  /**
   * An EmberMessageOptions value indicating the options used for the incoming
   * packet.
   */
  EmberMessageOptions options;
  /**
   * An EmberMacFrame structure indicating the source and destination
   * addresses and source and destination PAN IDs.
   */
  EmberMacFrame macFrame;
  /**
   * The RSSI in dBm the packet was received with.
   */
  int8_t rssi;
  /**
   * The LQI the packet was received with.
   */
  int8_t lqi;
  /**
   * The security MAC frame counter (if any).
   */
  uint32_t frameCounter;
  /**
   * An EmberMessageLength value indicating the length in bytes of the
   * MAC payload of the incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message MAC payload.
   */
  uint8_t *payload;
} EmberIncomingMacMessage;

/**
 * @brief An instance of this structure is passed to
 * emberMacMessageSentHandler(). It describes the outgoing MAC frame.
 */
typedef struct {
  /**
   * An EmberMessageOptions value indicating the options used for transmitting
   * the outgoing message.
   */
  EmberMessageOptions options;
  /**
   * An EmberMacFrame struct indicating the source and destination addresses
   * and source and destination PAN IDs of the outgoing MAC frame.
   */
  EmberMacFrame macFrame;
  /**
   * A tag value the application can use to match emberMacMessageSend() calls
   * to the corresponding emberMacMessageSentHandler() calls.
   */
  uint8_t tag;
  /**
   * The security frame counter of the outgoing MAC frame (if any).
   */
  uint32_t frameCounter;
  /**
   * An EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The RSSI in dBm of the ACK corresponding to this message. This field is
   * meaningful only if EMBER_OPTIONS_ACK_REQUESTED flag is set in the options
   * field.
   */
  int8_t ackRssi;
} EmberOutgoingMacMessage;

/** @brief This data structure contains the key data that is passed
 *   into various other functions. */
typedef struct {
  /** This is the key byte data. */
  uint8_t contents[EMBER_ENCRYPTION_KEY_SIZE];
} EmberKeyData;

/** @brief This function allows the programmer to gain access
 *  to the key data bytes of the EmberKeyData structure.
 *
 * @param[in] key  A Pointer to an EmberKeyData structure.
 *
 * @return uint8_t* Returns a pointer to the first byte of the key data.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint8_t* emberKeyContents(EmberKeyData* key);
#else
#define emberKeyContents(key) ((key)->contents)
#endif

/**
 * @brief Either marks an event as inactive or specifies the units for the
 * event execution time.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberEventUnits
#else
typedef uint8_t EmberEventUnits;
enum
#endif
{
  /** The event is not scheduled to run. */
  EMBER_EVENT_INACTIVE = 0,
  /** The execution time is in approximate milliseconds.  */
  EMBER_EVENT_MS_TIME,
  /** The execution time is in 'binary' quarter seconds (256 approximate
      milliseconds each). */
  EMBER_EVENT_QS_TIME,
  /** The execution time is in 'binary' minutes (65536 approximate milliseconds
      each). */
  EMBER_EVENT_MINUTE_TIME,
  /** The event is scheduled to run at the earliest opportunity. */
  EMBER_EVENT_ZERO_DELAY
};

/** brief An identifier for a task */
typedef uint8_t EmberTaskId;

//----------------------------------------------------------------
// Events and event queues.

// Forward declarations to make up for C's one-pass type checking.
struct Event_s;
struct EventQueue_s;

/** @brief The static part of an event.  Each event can be used with only one
 * event queue.
 */

typedef const struct {
  struct EventQueue_s *queue;           // the queue this event goes on
  void (*handler)(struct Event_s *);    // called when the event fires
  void (*marker)(struct Event_s *);     // marking fuction, can be NULL
  const char *name;                     // event name for debugging purposes
} EventActions;

typedef struct Event_s {
  EventActions *actions;                // static data

  // For internal use only, but the 'next' field must be initialized
  // to NULL.
  struct Event_s *next;
  uint32_t timeToExecute;
} Event;

/** @brief An event queue is currently just a list of events ordered by
 * execution time.
 */
typedef struct EventQueue_s {
  Event *isrEvents;
  Event *events;
} EventQueue;

/** @brief Control structure for events.
 *
 * This structure should not be accessed directly.
 * It holds the event status (one of the @e EMBER_EVENT_ values)
 * and the time left before the event fires.
 */
typedef struct {
  /** The event's status, either inactive or the units for timeToExecute. */
  EmberEventUnits status;
  /** The task ID this event belongs to. */
  EmberTaskId taskid;
  /** How long before the event fires.
   *  Units are always in milliseconds.
   */
  uint32_t timeToExecute;
} EmberEventControl;

// Testing the token_manager has shown the following two defines
// conflict with the compilations and are not relevant to the
// tests.  Disable them with an #if 0 but leave them here for
// completeness of information.
#if 0
/** @brief Complete events with a control and a handler procedure.
 *
 * An application typically creates an array of events
 * along with their handlers.
 * The main loop passes the array to emberRunEvents() to call
 * the handlers of any events whose time has arrived.
 */
typedef PGM struct EmberEventData_S {
  /** The control structure for the event. */
  EmberEventControl *control;
  /** The procedure to call when the event fires. */
  void (*handler)(void);
} EmberEventData;

/** @brief Control structure for tasks.
 *
 * This structure should not be accessed directly.
 */
typedef struct {
  // The time when the next event associated with this task will fire
  uint32_t nextEventTime;
  // The list of events associated with this task
  EmberEventData *events;
  // A flag that indicates the task has something to do other than events
  bool busy;
} EmberTaskControl;
#endif // 0

/** @brief Defines tasks that prevent the stack from sleeping.
 */
enum {
  /** Messages are waiting for transmission. */
  EMBER_OUTGOING_MESSAGES = 0x0001,
  /** One or more incoming messages are processed. */
  EMBER_INCOMING_MESSAGES = 0x0002,
  /** The radio is currently powered on.  On sleepy devices, the radio is
   *  turned off when not in use.  On non-sleepy devices, i.e.,
   *  EMBER_STAR_COORDINATOR, EMBER_STAR_RANGE_EXTENDER,
   *  or EMBER_STAR_END_DEVICE, the radio is always on.
   */
  EMBER_RADIO_IS_ON = 0x0004,
  /** The node is currently trying to associate to a Connect network. */
  EMBER_ASSOCIATING = 0x0008,
  /** The node is currently performing a MAC-level scanning procedure. */
  EMBER_SCANNING = 0x0010,
};

/**
 * @brief Defines the events reported to the application
 * by the emberCounterHandler().
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberCounterType
#else
typedef uint8_t EmberCounterType;
enum
#endif
{
  /** Every packet that comes in over the radio (except MAC ACKs). */
  EMBER_COUNTER_PHY_IN_PACKETS,

  /** Every packet that goes out over the radio (except MAC ACKs). */
  EMBER_COUNTER_PHY_OUT_PACKETS,

  /** Incoming MAC unicasts. */
  EMBER_COUNTER_MAC_IN_UNICAST,

  /** Incoming MAC broadcasts. */
  EMBER_COUNTER_MAC_IN_BROADCAST,

  /** Outgoing MAC unicasts that do not require an ACK. */
  EMBER_COUNTER_MAC_OUT_UNICAST_NO_ACK,

  /** Outgoing MAC unicasts that require an ACK for which an ACK
   * was received possibly after retrying. */
  EMBER_COUNTER_MAC_OUT_UNICAST_ACK_SUCCESS,

  /** Outgoing unicasts for which an ACK was never received even
   * after retrying. */
  EMBER_COUNTER_MAC_OUT_UNICAST_ACK_FAIL,

  /** Outgoing MAC packets which were never transmitted because
   * clear channel assessment always returned busy. */
  EMBER_COUNTER_MAC_OUT_UNICAST_CCA_FAIL,

  /** Outgoing unicast retries.  This does not count the initial
   * transmission.  Note a single MAC transmission can result in
   * multiple retries. */
  EMBER_COUNTER_MAC_OUT_UNICAST_RETRY,

  EMBER_COUNTER_MAC_OUT_BROADCAST,

  EMBER_COUNTER_MAC_OUT_BROADCAST_CCA_FAIL,

  EMBER_COUNTER_MAC_OUT_ENCRYPT_FAIL,

  /** Dropped incoming MAC packets (out of memory) */
  EMBER_COUNTER_MAC_DROP_IN_MEMORY,

  /** Dropped incoming MAC packets (invalid frame counter) */
  EMBER_COUNTER_MAC_DROP_IN_FRAME_COUNTER,

  /** Dropped incoming MAC packets (can't decrypt) */
  EMBER_COUNTER_MAC_DROP_IN_DECRYPT,

  /** Outgoing NWK forwarded packets */
  EMBER_COUNTER_NWK_OUT_FORWARDING,

  /** Incoming NWK data frames correctly processed */
  EMBER_COUNTER_NWK_IN_SUCCESS,

  /** Dropped incoming NWK packets (wrong source node) */
  EMBER_COUNTER_NWK_DROP_IN_WRONG_SOURCE,

  /** Dropped incoming NWK packets (can't forward) */
  EMBER_COUNTER_NWK_DROP_IN_FORWARDING,

  /** UART in and out data */
  EMBER_COUNTER_UART_IN_DATA,
  EMBER_COUNTER_UART_IN_MANAGEMENT,
  EMBER_COUNTER_UART_IN_FAIL,
  EMBER_COUNTER_UART_OUT_DATA,
  EMBER_COUNTER_UART_OUT_MANAGEMENT,
  EMBER_COUNTER_UART_OUT_FAIL,

  // Counters for non-packet events below.
  EMBER_COUNTER_ROUTE_2_HOP_LOOP,
  EMBER_COUNTER_BUFFER_ALLOCATION_FAIL,

  /** ASHv3 */
  EMBER_ASH_V3_ACK_SENT,
  EMBER_ASH_V3_ACK_RECEIVED,
  EMBER_ASH_V3_NACK_SENT,
  EMBER_ASH_V3_NACK_RECEIVED,
  EMBER_ASH_V3_RESEND,
  EMBER_ASH_V3_BYTES_SENT,
  EMBER_ASH_V3_TOTAL_BYTES_RECEIVED,
  EMBER_ASH_V3_VALID_BYTES_RECEIVED,
  EMBER_ASH_V3_PAYLOAD_BYTES_SENT,

  /** A placeholder giving the number of Ember counter types. */
  EMBER_COUNTER_TYPE_COUNT,
};

/**
 * @brief  Buffers used by the memory buffer system.
 */
typedef uint16_t EmberBuffer;

/**
 * @brief  Return type for Ember functions.
 */
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__

//------------------------------------------------------------------------------
// INTERNAL TYPES AND DEFINES
//------------------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef uint16_t Buffer;
typedef uint16_t EmberMessageBuffer;

#define NULL_BUFFER 0x0000u

typedef struct {
  EmberNodeId destination;
  uint8_t endpoint;
  uint8_t tag;
  Buffer payload;
  EmberMessageOptions txOptions;
} EmOutgoingPacket;

enum {
  /* RX status codes */
  EMBER_PACKET_TRACE_RX_STATUS_SUCCESS      = 0x00,
  EMBER_PACKET_TRACE_RX_STATUS_CRC_FAILED   = 0x01,
  EMBER_PACKET_TRACE_RX_STATUS_MAC_FILTERED = 0x02,
  /* TX status codes */
  EMBER_PACKET_TRACE_TX_STATUS_SUCCESS      = 0x80
};

typedef struct {
  uint8_t status;
  int8_t rssi;
  uint8_t channel;
  EmberMessageLength packetLength;
  uint8_t *packet;
} EmberMessageTrace;

typedef struct {
  uint32_t lastSeen;
  uint32_t frameCounter;
  EmberNodeId shortId;
  EmberEUI64 longId;
  uint8_t flags;
  uint8_t leaveCount;
} EmberChildEntry;

typedef uint8_t EmberLibraryStatus;

typedef void (*ClientResyncCallback)(EmberStatus status);

// A library's status is an 8-bit value with information about it.
// The high bit indicates whether the library is present (1), or if it is a
// stub (0).  The lower 7-bits can be used for codes specific to the library.
// This allows a library, like the security library, to specify what additional
// features are present.
// A value of 0xFF is reserved, it indicates an error in retrieving the
// library status.
#define EMBER_LIBRARY_PRESENT_MASK          0x80
#define EMBER_LIBRARY_IS_STUB               0x00
#define EMBER_LIBRARY_ERROR                 0xFF

#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif // __EMBER_TYPES_H__
