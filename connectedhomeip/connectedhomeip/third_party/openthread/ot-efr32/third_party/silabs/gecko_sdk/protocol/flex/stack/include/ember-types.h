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
 * @addtogroup ember_types
 * @brief Definitions of Connect data types used by various Connect API
 * functions.
 *
 * See ember-types.h for source code.
 * @{
 */

#ifndef __EMBER_TYPES_H__
#define __EMBER_TYPES_H__

#include PLATFORM_HEADER

#include <stdint.h>
#include <stdbool.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "stack/config/ember-configuration-defaults.h"
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
 * @brief EUI 64-bit ID (IEEE 802.15.4 long address).
 */
typedef uint8_t EmberEUI64[EUI64_SIZE];

/**
 * @brief IEEE 802.15.4 node ID. Also known as short address.
 */
typedef uint16_t EmberNodeId;

/**
 * @brief IEEE 802.15.4 PAN ID.
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
 * @brief Define the possible types of nodes and the roles that a
 * node might play in a network.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberNodeType
#else
typedef uint8_t EmberNodeType;
enum
#endif
{
  /** Device is not in network. */
  EMBER_UNKNOWN_DEVICE = 0,
  /** Extended star mode device: Will relay messages and can act as
   * a parent to range extender and  end device nodes. A node can be
   * started as a coordinator using the ::emberFormNetwork() API.
   */
  EMBER_STAR_COORDINATOR = 1,
  /** Extended star mode device: Will relay messages and can act as
   * a parent to end device nodes. Joins to a coordinator. A node can be
   * started as a range extender using the ::emberJoinNetwork() or
   * ::emberJoinNetworkExtended() API.
   */
  EMBER_STAR_RANGE_EXTENDER = 2,
  /** Extended star mode device: Communicates only with its parent
   * and will not relay messages. A node can be started as an end device using
   * the ::emberJoinNetwork() or ::emberJoinNetworkExtended() API.
   */
  EMBER_STAR_END_DEVICE = 3,
  /** Extended star mode device: An end device whose radio is
   * turned off when not communicating to save power. The application must call
   * ::emberPollForData() to receive messages.  A node can be started as a
   * sleepy end device using the ::emberJoinNetwork() or
   * ::emberJoinNetworkExtended() API.
   */
  EMBER_STAR_SLEEPY_END_DEVICE = 4,
  /** Direct mode device: A device able to send and receive messages
   * from other devices in range on the same PAN ID, with no star topology
   * restrictions. Such device does not relay messages. A node can be started
   * as a direct device by using the ::emberJoinCommissioned() API.
   */
  EMBER_DIRECT_DEVICE = 5,
  /** MAC mode device: A device able to send and receive MAC-level messages.
   * A node can be started as a MAC mode device by using the
   * ::emberJoinCommissioned(), the ::emberJoinNetworkExtended() or the
   * ::emberMacFormNetwork() API.
   */
  EMBER_MAC_MODE_DEVICE = 6,
  /** MAC mode device: A sleepy device able to send and receive MAC-level
   * messages. The radio on the device is turned off when not communicating.
   * The application must call ::emberPollForData() to receive messages. A node
   * can be started as a sleepy MAC mode device by using the
   * ::emberJoinCommissioned() API or the ::emberJoinNetworkExtended() API.
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
  /** The node is currently using its radio for test */
  EMBER_RADIO_TEST
};

/** @brief Hold network parameters.
 *
 * For information about power settings and radio channels,
 * see the technical specification for the
 * RF communication module in your Developer Kit and the
 * Radio Configurator Guide (AN971).
 */
typedef struct {
  /** The network's PAN identifier.*/
  uint16_t  panId;
  /** The transmit power setting, in deci-dBm.*/
  int16_t   radioTxPower;
  /** The radio channel. Be sure to specify a channel supported by the radio. */
  uint16_t   radioChannel;
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
  /** The child is connected through range extender. */
  EMBER_CHILD_FLAG_DEVICE_IS_EXTENDED_BIT              = 0x20,
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
  /** The packet should be sent with high priority. High priority messages are
   * added to the head of transmit queue, other messages are added to the tail.
   */
  EMBER_OPTIONS_HIGH_PRIORITY            = 0x04,
  /** The packet should be sent via the indirect queue. This option only applies
     to nodes with ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE
     node type. */
  EMBER_OPTIONS_INDIRECT                 = 0x08,
};

/**
 * @brief An instance of this structure is passed to
 * emberIncomingMessageHandler(). It describes the incoming message.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for the incoming
   * packet.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberNodeId value indicating source node ID.
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
   * An ::EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The millisecond system time returned by ::halCommonGetInt32uMillisecondTick()
   * at the time the sync word was detected.
   */
  uint32_t timestamp;
  /**
   * The LQI the packet was received with.
   */
  uint8_t lqi;
} EmberIncomingMessage;

/**
 * @brief An instance of this structure is passed to emberMessageSentHandler().
 * It describes the outgoing packet.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for transmitting
   * the outgoing message.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberNodeId value indicating the destination short ID.
   */
  EmberNodeId destination;
  /**
   * The endpoint the message is destined to.
   */
  uint8_t endpoint;
  /**
   * A tag value the application can use to match ::emberMessageSend() calls to
   * the corresponding ::emberMessageSentHandler() calls.
   */
  uint8_t tag;
  /**
   * An ::EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The RSSI in dBm of the ACK corresponding to this message. This field is
   * meaningful only if ::EMBER_OPTIONS_ACK_REQUESTED flag is set in the options
   * field.
   */
  int8_t ackRssi;
  /**
   * The millisecond system time returned by ::halCommonGetInt32uMillisecondTick()
   * at the time the sync word was transmitted.
   */
  uint32_t timestamp;
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
  /** No address on this field. */
  EMBER_MAC_ADDRESS_MODE_NONE  = 0x00,
  /** Short addressing mode. */
  EMBER_MAC_ADDRESS_MODE_SHORT = 0x02,
  /** Long addressing mode. */
  EMBER_MAC_ADDRESS_MODE_LONG  = 0x03,
};

/**
 * @brief A structure that stores an 802.15.4 address.
 */
typedef struct {
  union {
    /**
     * @brief Long (EUI-64) address. Valid if @p mode is @ref
     * EMBER_MAC_ADDRESS_MODE_LONG
     */
    uint8_t longAddress[EUI64_SIZE];
    /**
     * @brief Short address (node ID). Valid if @p mode is @ref
     * EMBER_MAC_ADDRESS_MODE_SHORT
     */
    uint16_t shortAddress;
  } addr;

  EmberMacAddressMode mode; /**< Addressing mode*/
} EmberMacAddress;

/**
 * @brief A structure that describes the addressing fields of a 802.15.4
 * frame.
 */
typedef struct {
  /**
   * An ::EmberMacAddress structure indicating the source address of a MAC frame.
   */
  EmberMacAddress srcAddress;
  /**
   * An ::EmberMacAddress structure indicating the destination address of a MAC
   * frame.
   */
  EmberMacAddress dstAddress;
  /**
   * An ::EmberPanId struct indicating the source PAN ID of a MAC frame. This
   * field is meaningful only if srcPanIdSpecified is set to true.
   */
  EmberPanId srcPanId;
  /**
   * An ::EmberPanId struct indicating the destination PAN ID of a MAC frame.
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
 * ::emberIncomingMacMessageHandler(). It describes the incoming MAC frame.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for the incoming
   * packet.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberMacFrame structure indicating the source and destination
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
  uint8_t lqi;
  /**
   * The security MAC frame counter (if any).
   */
  uint32_t frameCounter;
  /**
   * An ::EmberMessageLength value indicating the length in bytes of the
   * MAC payload of the incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message MAC payload.
   */
  uint8_t *payload;
  /**
   * The millisecond system time returned by ::halCommonGetInt32uMillisecondTick()
   * at the time the sync word was detected.
   */
  uint32_t timestamp;
} EmberIncomingMacMessage;

/**
 * @brief An instance of this structure is passed to
 * emberMacMessageSentHandler(). It describes the outgoing MAC frame.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for transmitting
   * the outgoing message.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberMacFrame struct indicating the source and destination addresses
   * and source and destination PAN IDs of the outgoing MAC frame.
   */
  EmberMacFrame macFrame;
  /**
   * A tag value the application can use to match ::emberMacMessageSend() calls
   * to the corresponding ::emberMacMessageSentHandler() calls.
   */
  uint8_t tag;
  /**
   * The security frame counter of the outgoing MAC frame (if any).
   */
  uint32_t frameCounter;
  /**
   * An ::EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The RSSI in dBm of the ACK corresponding to this message. This field is
   * meaningful only if ::EMBER_OPTIONS_ACK_REQUESTED flag is set in the options
   * field.
   */
  int8_t ackRssi;
  /**
   * The millisecond system time returned by ::halCommonGetInt32uMillisecondTick()
   * at the time the sync word was transmitted.
   */
  uint32_t timestamp;
} EmberOutgoingMacMessage;

/** @brief This data structure contains the security key, most prominently used
 * by ::emberSetSecurityKey.
 */
typedef struct {
  /** This is the key byte data. */
  uint8_t contents[EMBER_ENCRYPTION_KEY_SIZE];
} EmberKeyData;

/** @brief This macro allows the programmer to gain access
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
  /** The execution time is in 'binary' quarter seconds (256 timer ticks). */
  EMBER_EVENT_QS_TIME,
  /** The execution time is in 'binary' minutes (65536 timer ticks). */
  EMBER_EVENT_MINUTE_TIME,
  /** The event is scheduled to run at the earliest opportunity. */
  EMBER_EVENT_ZERO_DELAY
};

/** @brief An identifier for a task */
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
  void (*marker)(struct Event_s *);     // marking function, can be NULL
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

/** @brief Complete events with a control and a handler procedure.
 *
 * An application typically creates an array of events
 * along with their handlers.
 * The main loop passes the array to ::emberRunEvents() to call
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
  /** The time when the next event associated with this task will fire */
  uint32_t nextEventTime;
  /** The list of events associated with this task */
  EmberEventData *events;
  /** A flag that indicates the task has something to do other than events */
  bool busy;
} EmberTaskControl;

/** @brief Define tasks that prevent the stack from sleeping.
 */
enum {
  /** Messages are waiting for transmission. */
  EMBER_OUTGOING_MESSAGES = 0x0001,
  /** One or more incoming messages are processed. */
  EMBER_INCOMING_MESSAGES = 0x0002,
  /** The radio is currently powered on.  On sleepy devices, the radio is
   *  turned off when not in use.  On non-sleepy devices, i.e.,
   *  ::EMBER_STAR_COORDINATOR, ::EMBER_STAR_RANGE_EXTENDER,
   *  ::EMBER_STAR_END_DEVICE, or ::EMBER_MAC_MODE_DEVICE, the radio is
   *  always on.
   */
  EMBER_RADIO_IS_ON = 0x0004,
  /** The node is currently trying to associate to a Connect network. */
  EMBER_ASSOCIATING = 0x0008,
  /** The node is currently performing a MAC-level scanning procedure. */
  EMBER_SCANNING = 0x0010,
};

/**
 * @brief Define the event counters that can be requested from the application
 * using ::emberGetCounter()
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

  /** Incoming MAC layer unicasts. */
  EMBER_COUNTER_MAC_IN_UNICAST,

  /** Incoming MAC layer broadcasts. */
  EMBER_COUNTER_MAC_IN_BROADCAST,

  /** Outgoing MAC layer unicasts that do not require an ACK. */
  EMBER_COUNTER_MAC_OUT_UNICAST_NO_ACK,

  /** Outgoing MAC layer unicasts that required an ACK, for which an ACK was
   * received (possibly after retrying). */
  EMBER_COUNTER_MAC_OUT_UNICAST_ACK_SUCCESS,

  /** Outgoing MAC layer unicasts that required an ACK, for which an ACK was
   * never received (even after retrying). */
  EMBER_COUNTER_MAC_OUT_UNICAST_ACK_FAIL,

  /** Outgoing MAC layer packets which were never transmitted because
   * clear channel assessment always returned busy. */
  EMBER_COUNTER_MAC_OUT_UNICAST_CCA_FAIL,

  /** Outgoing MAC layer unicast retries.  This does not count the initial
   * transmission.  Note a single MAC transmission can result in
   * multiple retries. */
  EMBER_COUNTER_MAC_OUT_UNICAST_RETRY,

  /** Outgoing MAC layer broadcasts */
  EMBER_COUNTER_MAC_OUT_BROADCAST,

  /** Outgoing MAC layer broadcasts which were never transmitted because
   * clear channel assessment always returned busy. */
  EMBER_COUNTER_MAC_OUT_BROADCAST_CCA_FAIL,

  /** Outgoing MAC layer messages which were not transmitted due to security
   * error e.g. because security key was not set. */
  EMBER_COUNTER_MAC_OUT_ENCRYPT_FAIL,

  /** Dropped incoming MAC layer packets (out of memory) */
  EMBER_COUNTER_MAC_DROP_IN_MEMORY,

  /** Dropped incoming MAC layer packets (invalid frame counter) */
  EMBER_COUNTER_MAC_DROP_IN_FRAME_COUNTER,

  /** Dropped incoming MAC layer packets (can't decrypt) */
  EMBER_COUNTER_MAC_DROP_IN_DECRYPT,

  /** Outgoing Network layer forwarded packets */
  EMBER_COUNTER_NWK_OUT_FORWARDING,

  /** Incoming Network layer data frames correctly processed */
  EMBER_COUNTER_NWK_IN_SUCCESS,

  /** Dropped incoming Network layer packets (wrong source node) */
  EMBER_COUNTER_NWK_DROP_IN_WRONG_SOURCE,

  /** Dropped incoming Network layer packets (can't forward) */
  EMBER_COUNTER_NWK_DROP_IN_FORWARDING,

  /** NCP UART in data packet */
  EMBER_COUNTER_UART_IN_DATA,
  /** NCP UART in management packet */
  EMBER_COUNTER_UART_IN_MANAGEMENT,
  /** NCP UART in frame errors */
  EMBER_COUNTER_UART_IN_FAIL,
  /** NCP UART out data packet */
  EMBER_COUNTER_UART_OUT_DATA,
  /** NCP UART out management packet */
  EMBER_COUNTER_UART_OUT_MANAGEMENT,
  /** NCP UART out frame errors */
  EMBER_COUNTER_UART_OUT_FAIL,

  // Counters for non-packet events below.
  EMBER_COUNTER_ROUTE_2_HOP_LOOP,
  EMBER_COUNTER_BUFFER_ALLOCATION_FAIL,

  /** ASHv3 ACK frame sent */
  EMBER_ASH_V3_ACK_SENT,
  /** ASHv3 ACK frame received */
  EMBER_ASH_V3_ACK_RECEIVED,
  /** ASHv3 NACK frame sent */
  EMBER_ASH_V3_NACK_SENT,
  /** ASHv3 NACK frame received */
  EMBER_ASH_V3_NACK_RECEIVED,
  /** ASHv3 resends of frames that were not acked */
  EMBER_ASH_V3_RESEND,
  /** ASHv3 number of bytes sent */
  EMBER_ASH_V3_BYTES_SENT,
  /** ASHv3 number of bytes received */
  EMBER_ASH_V3_TOTAL_BYTES_RECEIVED,
  /** ASHv3 number of valid bytes received */
  EMBER_ASH_V3_VALID_BYTES_RECEIVED,
  /** ASHv3 number of payload bytes sent */
  EMBER_ASH_V3_PAYLOAD_BYTES_SENT,

  /** A placeholder giving the number of Ember counter types. */
  EMBER_COUNTER_TYPE_COUNT,
};

/**
 * @brief  Buffers used by the memory buffer system.
 */
typedef uint16_t EmberBuffer;

/**
 * @brief Define the PHY configuration of connect stack
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberPhyType
#else
typedef uint8_t EmberPhyType;
enum
#endif
{
  /** Connect uses radio configurator PHY */
  EMBER_RADIO_CONFIGURATOR,

  /** Connect is configured to use only 2.4GHz standard PHY */
  EMBER_STANDARD_PHY_2_4GHZ,

  /** Connect is configured to use only 915MHz standard PHY */
  EMBER_STANDARD_PHY_915MHZ,

  /** Connect is configured to use only 863MHz standard PHY */
  EMBER_STANDARD_PHY_863MHZ
};

/**
 * @brief Define the type of calibration requested
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberCalType
#else
typedef uint32_t EmberCalType;
enum
#endif
{
  /** Temperature calibration */
  EMBER_CAL_TEMP_VCO = 0x00000001,

  /** Image Rejection calibration */
  EMBER_CAL_IRCAL    = 0x00010000,

  /** Both calibrations */
  EMBER_CAL_ALL      = 0x00010001
};

#define EMBER_CAL_INVALID_VALUE    (0xFFFFFFFF)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//doxygen should see this in error-def.h

/**
 * @brief  Return type for Ember functions.
 */
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__

#endif //DOXYGEN_SHOULD_SKIP_THIS

//------------------------------------------------------------------------------
// INTERNAL TYPES AND DEFINES
//------------------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef struct {
  EmberNodeId destination;
  uint8_t endpoint;
  uint8_t tag;
  EmberBuffer payload;
  EmberMessageOptions txOptions;
} EmOutgoingPacket;

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

typedef struct {
  EmberBuffer* buffer_addr;
  uint16_t buffer_length;
} EmberBufferDesc;

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

#define emberDebugPrintf(...)
#define emLog(type, ...)
#define emLogLine(type, ...)
#define emLogBytes(type, format, bytes, count, ...)

#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @enum EmberTxStreamParameters
 * @brief Radio Stream mode
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberTxStreamParameters
#else
typedef uint8_t EmberTxStreamParameters;
enum
#endif
{
  TX_STREAM_PN9, /**< Pseudo random PN9 byte sequence */
  TX_STREAM_CW   /**< An unmodulated carrier wave */
};

#endif // __EMBER_TYPES_H__

/** @} // END addtogroup
 */
