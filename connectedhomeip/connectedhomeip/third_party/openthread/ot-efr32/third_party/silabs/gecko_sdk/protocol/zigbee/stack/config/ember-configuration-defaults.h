/***************************************************************************//**
 * @file
 * @brief User-configurable stack memory allocation defaults
 *
 * @note Application developers should \b not modify any portion
 * of this file. Doing so may cause mysterious bugs. Allocations should be
 * adjusted only by defining the appropriate macros in the application's
 * CONFIGURATION_HEADER.
 *
 * See @ref configuration for documentation.
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

//  Todo:
//  - explain how to use a configuration header
//  - the documentation of the custom handlers should
//    go in config/ember-configuration.c, not here
//  - the stack profile documentation is out of date

/**
 * @addtogroup configuration
 *
 * All configurations have defaults. Therefore, many applications may not need
 * to do anything custom.  However, these defaults can be overridden
 * by creating a CONFIGURATION_HEADER or within this header,
 * defining the appropriate macro to a different size. For example, to
 * reduce the number of allocated packet buffers from 24 (the default) to 8:
 * @code
 * #define EMBER_PACKET_BUFFER_COUNT 8
 * @endcode
 *
 * The convenience stubs provided in @c config/ember-configuration.c can be overridden
 * by defining the appropriate macro and providing the corresponding callback
 * function. For example, an application with custom debug channel input must
 * implement @c emberDebugHandler() to process it. Along with
 * the function definition, the application should provide the following
 * line in its CONFIGURATION_HEADER:
 * @code
 * #define EMBER_APPLICATION_HAS_DEBUG_HANDLER
 * @endcode
 *
 * See ember-configuration-defaults.h for source code.

 * @{
 */

#ifndef SILABS_EMBER_CONFIGURATION_DEFAULTS_H
#define SILABS_EMBER_CONFIGURATION_DEFAULTS_H

#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_BINDING_TABLE_PRESENT)
#include "sl_zigbee_binding_table_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT)
#include "sl_zigbee_green_power_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_SECURITY_LINK_KEYS_PRESENT)
#include "sl_zigbee_security_link_keys_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_SOURCE_ROUTE_PRESENT)
#include "sl_zigbee_source_route_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_DEVICE_CONFIG_PRESENT)
#include "zigbee_device_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_PRO_STACK_PRESENT)
#include "sl_zigbee_pro_stack_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_PRO_LEAF_STACK_PRESENT) || defined(SL_CATALOG_ZIGBEE_PRO_LEAF_STACK_WITH_CSL_PRESENT)
#include "sl_zigbee_pro_leaf_stack_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_PRO_STACK_ALT_MAC_PRESENT)
#include "sl_zigbee_pro_stack_alt_mac_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT)
#include "sl_zigbee_light_link_config.h"
#endif

#if defined(SL_CATALOG_ZIGBEE_NCP_CONFIGURATION_PRESENT)
#include "ncp-configuration-config.h"
#endif

#ifndef EMBER_API_MAJOR_VERSION
/** @brief The major version number of the Ember stack release
 * that the application is built against.
 */
  #define EMBER_API_MAJOR_VERSION 2
#endif

#ifndef EMBER_API_MINOR_VERSION
/** @brief The minor version number of the Ember stack release
 * that the application is built against.
 */
  #define EMBER_API_MINOR_VERSION 0
#endif

/** @brief Specify the stack profile.  The default is Profile 0.
 *
 * You can set this to Profile 1 (Zigbee) or Profile 2 (Zigbee Pro) in your
 * application's configuration header (.h) file using:
 * @code
 * #define EMBER_STACK_PROFILE 1
 * @endcode
 * or
 * @code
 * #define EMBER_STACK_PROFILE 2
 * @endcode
 */
#ifndef EMBER_STACK_PROFILE
  #define EMBER_STACK_PROFILE 0
#endif

#if (EMBER_STACK_PROFILE == 2)
#define EMBER_MAX_DEPTH                  15
#define EMBER_SECURITY_LEVEL              5
#define EMBER_MIN_DISCOVERY_TABLE_SIZE    4
#define EMBER_INDIRECT_TRANSMISSION_TIMEOUT 7680
#define EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS false
#ifndef EMBER_MIN_ROUTE_TABLE_SIZE // this value is set to zero on leaf stack
#define EMBER_MIN_ROUTE_TABLE_SIZE       10
#endif
#endif

#ifndef EMBER_MAX_END_DEVICE_CHILDREN
/** @brief The maximum number of end device children that a router
 * will support.*/
  #define EMBER_MAX_END_DEVICE_CHILDREN 6
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* Need to put in a compile time check to make sure that too many child devices
 * aren't specified.
 */

#if EMBER_MAX_END_DEVICE_CHILDREN > 64
  #error "EMBER_MAX_END_DEVICE_CHILDREN can not exceed 64."
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifndef EMBER_SECURITY_LEVEL
/** @brief The security level used for security at the MAC and network
 * layers.  The supported values are 0 (no security) and 5 (payload is
 * encrypted and a four-byte MIC is used for authentication).
 */
  #define EMBER_SECURITY_LEVEL 5
#endif

#if !(EMBER_SECURITY_LEVEL == 0 \
      || EMBER_SECURITY_LEVEL == 5)
  #error "Unsupported security level"
#endif

#ifdef EMBER_CHILD_TABLE_SIZE
  #if (EMBER_MAX_END_DEVICE_CHILDREN < EMBER_CHILD_TABLE_SIZE)
    #undef EMBER_CHILD_TABLE_SIZE
  #endif
#endif

#ifndef EMBER_CHILD_TABLE_SIZE
/** @brief The maximum number of children that a node may have.
 *
 * For the mesh stack, this defaults to the value of
 * ::EMBER_MAX_END_DEVICE_CHILDREN.
 * In the mesh stack router children are not stored in the child table.
 *
 * Each child table entry requires 4 bytes of RAM and a 10 byte token.
 *
 * Application definitions for ::EMBER_CHILD_TABLE_SIZE that are larger
 * than the default value
 * are ignored and the default value used instead.
 */
  #define EMBER_CHILD_TABLE_SIZE EMBER_MAX_END_DEVICE_CHILDREN
#endif

/** @brief The maximum number of link and master keys that a node can
 *  store, <b>not</b> including the Trust Center Link Key.
 *  The stack maintains special storage for the Trust Center
 *  Link Key.
 *
 *  For the Trust Center, this controls how many totally unique
 *  Trust Center Link Keys may be stored.  The rest of the devices
 *  in the network will use a global or hashed link key.
 *
 *  For normal nodes, this controls the number of Application
 *  Link Keys it can store. The Trust Center Link Key
 *  is stored separately from this table.
 */
#ifndef EMBER_KEY_TABLE_SIZE
  #define EMBER_KEY_TABLE_SIZE 0
#endif

/** @brief The number of entries for the field upgradeable
 *  certificate table. Normally, certificates, such as SE certs, are stored
 *  in the runtime-unmodifiable MFG area. However, those devices wishing to
 *  add new certificates after manufacturing will have to use the normal
 *  token space. This defines the size of that table. For most devices 0 is
 *  appropriate since there is no need to change certificates in the field.
 *  For those wishing to field upgrade devices with new certificates,
 *  1 is the correct size.  Anything more is wasting SimEEPROM.
 */
#ifndef EMBER_CERTIFICATE_TABLE_SIZE
  #define EMBER_CERTIFICATE_TABLE_SIZE 0
#else
  #if EMBER_CERTIFICATE_TABLE_SIZE > 1
    #error "EMBER_CERTIFICATE_TABLE_SIZE > 1 is not supported!"
  #endif
#endif

/** @brief The maximum depth of the tree in ZigBee 2006.
 *  This implicitly determines the maximum diameter of
 *  the network (::EMBER_MAX_HOPS) if that value is not
 *  overridden.
 */
#ifndef EMBER_MAX_DEPTH
  #define EMBER_MAX_DEPTH                15
#elif (EMBER_MAX_DEPTH > 15)
// Depth is a 4-bit field
  #error "EMBER_MAX_DEPTH cannot be greater than 15"
#endif

/** @brief The maximum number of hops for a message.
 *
 * The default maximum number of hops is two times the maximum depth
 * (::EMBER_MAX_DEPTH). This is the fallback value when the radius is not
 * supplied by the Application (i.e., 0) or the stack is sending a message.
 */
#ifndef EMBER_MAX_HOPS
  #define EMBER_MAX_HOPS (2 * EMBER_MAX_DEPTH)
#endif

/** @brief The number of Packet Buffers available to the Stack.
 * The default is 75.
 *
 * Each buffer requires 36 bytes of RAM (32 for the buffer itself plus 4 bytes
 * of overhead).
 */
#ifndef EMBER_PACKET_BUFFER_COUNT
  #define EMBER_PACKET_BUFFER_COUNT 75
#endif
/** @brief The maximum number of beacons to record when performing an active
 * scan to find networks. Legacy APIs such as emberJoinNetwork and the several
 * emberFindAndRejoinNetwork APIs set the number of beacons to record to 1,
 * while the Network Steering plugin will attempt to record the maximum number of
 * beacons.
 *
 * It is important not to consume all packet buffers in a system, else the
 * transmission and reception of packets will fail, since TX and RX packets are
 * held in packet buffers.
 */
#ifndef EMBER_MAX_BEACONS_TO_STORE
// Packet buffers can be maxed out and we dont necessarily want to cache all those
// many beacons. Cap number of beacons to half the available buffers if they are  <= 75
// and set to a constant of 37 (~half of 75) otherwise
// 75 is the default count for EMBER_PACKET_BUFFER_COUNT, which is set to 255 for NCP
  #define EMBER_MAX_BEACONS_TO_STORE ( (EMBER_PACKET_BUFFER_COUNT <= 75) ? (EMBER_PACKET_BUFFER_COUNT >> 1) : 37)
#endif
/** @brief The amount of time after which any caches beacons are purged from the
 * cache in RAM. Each beacon stored consumes one packet buffer.
 * The timeout for beacons begins once the first beacon is added to the cache.
 */
#ifndef BEACON_STORAGE_TIMEOUT_MS
  #define BEACON_STORAGE_TIMEOUT_MS    (3 * 60 * 1000)  // 3 minutes
#endif
/** @brief The maximum number of router neighbors the stack
 * can keep track of.
 *
 * A neighbor is a node within radio range.
 * The maximum allowed value is 26. End device children are
 * stored in the child table, not the neighbor table.
 * The default is 16. Setting this value lower than 8 is not
 * recommended.
 *
 * Each neighbor table entry consumes 18 bytes of RAM (6 for the table
 * itself and 12 bytes of security data).
 */
#define EMBER_MAX_NEIGHBOR_TABLE_SIZE 26
#ifndef EMBER_NEIGHBOR_TABLE_SIZE
  #define EMBER_NEIGHBOR_TABLE_SIZE 16
#endif
#if (EMBER_NEIGHBOR_TABLE_SIZE != EMBER_MAX_NEIGHBOR_TABLE_SIZE) && (EMBER_NEIGHBOR_TABLE_SIZE != 16) && (EMBER_NEIGHBOR_TABLE_SIZE != 1)
  #error "Unsupported neighbor table size is configured. Supported values are 1, 16 and 26."
#endif
/** @brief The maximum amount of time (in milliseconds) that the MAC
 * will hold a message for indirect transmission to a child.
 *
 * The default is 3000 milliseconds (3 sec).
 * The maximum value is 30 seconds (30000 milliseconds). Larger values
 * will cause rollover confusion.
 */
#ifndef EMBER_INDIRECT_TRANSMISSION_TIMEOUT
  #define EMBER_INDIRECT_TRANSMISSION_TIMEOUT 3000
#endif
#define EMBER_MAX_INDIRECT_TRANSMISSION_TIMEOUT 30000
#if (EMBER_INDIRECT_TRANSMISSION_TIMEOUT \
     > EMBER_MAX_INDIRECT_TRANSMISSION_TIMEOUT)
  #error "Indirect transmission timeout too large."
#endif

/** @brief Define the behavior for the address that the multicasts are sent to.
 *    The normal address is RxOnWhenIdle=true (0xFFFD). However, setting this
 *    to true can change locally generated multicasts to be sent to the sleepy
 *    broadcast address (0xFFFF). Changing the default is NOT Zigbee Pro
 *    compliant and may not be inter-operable.
 */
#ifndef EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS
  #define EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS false
#endif

/* @brief End Device keep alive support mode
 */
#ifndef EMBER_END_DEVICE_KEEP_ALIVE_SUPPORT_MODE
  #define EMBER_END_DEVICE_KEEP_ALIVE_SUPPORT_MODE  EMBER_KEEP_ALIVE_SUPPORT_ALL
#endif

/** @brief The default index into the R21 Child Aging timeout
 * The default value is a timeout of 256 minutes
 */
#define SECONDS_10              0
#define MINUTES_2               1
#define MINUTES_4               2
#define MINUTES_8               3
#define MINUTES_16              4
#define MINUTES_32              5
#define MINUTES_64              6
#define MINUTES_128             7
#define MINUTES_256             8
#define MINUTES_512             9
#define MINUTES_1024            10
#define MINUTES_2048            11
#define MINUTES_4096            12
#define MINUTES_8192            13
#define MINUTES_16384           14

#ifndef EMBER_END_DEVICE_POLL_TIMEOUT
  #define EMBER_END_DEVICE_POLL_TIMEOUT MINUTES_256
#endif

/** @brief The temporary, short timeout that is given to a joining or insecurely
 * rejoining child. The child is expected to send a network encrypted packet to
 * set its timeout to the default value.
 */
#ifndef EMBER_SHORT_CHILD_TIMEOUT
  #if (EMBER_END_DEVICE_POLL_TIMEOUT == SECONDS_10)
    #define EMBER_SHORT_CHILD_TIMEOUT SECONDS_10
  #else // (EMBER_END_DEVICE_POLL_TIMEOUT == SECONDS_10)
    #define EMBER_SHORT_CHILD_TIMEOUT MINUTES_2
  #endif // (EMBER_END_DEVICE_POLL_TIMEOUT == SECONDS_10)
#endif // EMBER_SHORT_CHILD_TIMEOUT

#ifndef EMBER_LINK_POWER_DELTA_INTERVAL
  #define EMBER_LINK_POWER_DELTA_INTERVAL 300
#endif
/** @brief The maximum number of APS retried messages that the
 * stack can transmit at any time.
 * Here, transmitting describes
 * the time between the call to ::emberSendUnicast() and the subsequent
 * callback to ::emberMessageSentHandler().
 *
 * @note  A message will
 * typically use one packet buffer for the message header and one or
 * more packet buffers for the payload. The default is 10 messages.
 *
 * Each APS retried message consumes 6 bytes of RAM and
 * two or more packet buffers.
 */
#ifndef EMBER_APS_UNICAST_MESSAGE_COUNT
  #define EMBER_APS_UNICAST_MESSAGE_COUNT 10
#endif
/** @brief The maximum number of bindings supported by the stack.
 * The default is 0 bindings. Each binding consumes 2 bytes of RAM.
 */
#ifndef EMBER_BINDING_TABLE_SIZE
  #define EMBER_BINDING_TABLE_SIZE 0
#endif
/** @brief The maximum number of EUI64<->network address associations
 * that the stack can maintain.  The default value is 8.
 *
 * Address table entries are 10 bytes in size.
 */
#ifndef EMBER_ADDRESS_TABLE_SIZE
  #define EMBER_ADDRESS_TABLE_SIZE 8
#endif
/** @brief The maximum number of destinations to which a node can
 * route messages. This include both messages originating at this node
 * and those relayed for others. The default value is 16.
 *
 * Route table entries are 6 bytes in size.
 */

#ifndef EMBER_ROUTE_TABLE_SIZE //in leaf stack this value is defined to be 0
    #define EMBER_ROUTE_TABLE_SIZE 16
#elif defined(EMBER_MIN_ROUTE_TABLE_SIZE) \
  && EMBER_ROUTE_TABLE_SIZE < EMBER_MIN_ROUTE_TABLE_SIZE
  #error "EMBER_ROUTE_TABLE_SIZE is less than required by stack profile."
#endif

/** @brief The number of simultaneous route discoveries that a node
 * will support.
 *
 * Discovery table entries are 9 bytes in size.
 */
#ifndef EMBER_DISCOVERY_TABLE_SIZE
  #ifdef EMBER_MIN_DISCOVERY_TABLE_SIZE
    #define EMBER_DISCOVERY_TABLE_SIZE EMBER_MIN_DISCOVERY_TABLE_SIZE
  #else
    #define EMBER_DISCOVERY_TABLE_SIZE 8
  #endif
#elif defined(EMBER_MIN_DISCOVERY_TABLE_SIZE) \
  && EMBER_DISCOVERY_TABLE_SIZE < EMBER_MIN_DISCOVERY_TABLE_SIZE
  #error "EMBER_DISCOVERY_TABLE_SIZE is less than required by stack profile."
#endif

/** @brief The maximum number of multicast groups that the device may be
 * a member of.  The default value is 8.
 *
 * Multicast table entries are 3 bytes in size.
 */
#ifndef EMBER_MULTICAST_TABLE_SIZE
  #define EMBER_MULTICAST_TABLE_SIZE 8
#endif
/** @brief The number of source route table entries if the source route library
 * is not included. The maximum source route table
 * size is 255 entries, since a one-byte index is used, and the index 0xFF is
 * reserved. The default value is 0.
 *
 * Source route table entries are 4 bytes in size.
 */
#ifndef EMBER_SOURCE_ROUTE_TABLE_SIZE
  #define EMBER_SOURCE_ROUTE_TABLE_SIZE 0
#endif

#ifndef EMBER_MAX_SOURCE_ROUTE_RELAY_COUNT
  #define EMBER_MAX_SOURCE_ROUTE_RELAY_COUNT  11
#endif

/** @brief The maximum number of broadcasts during a single broadcast timeout
 * period. The minimum and default value is 15 and can only be changed
 * on compatible Ember stacks. Be very careful when changing the broadcast
 * table size as it affects timing of the broadcasts and the number of
 * possible broadcasts.  Additionally, this value must be universal for
 * all devices in the network. Otherwise, a single router can overwhelm
 * all its neighbors with more broadcasts than they can support.
 * In general, this value should be left alone.
 *
 * Broadcast table entries are 6 bytes in size.
 */

/* The minimum broadcast table size per the Zigbee Pro spec is 9. */
#define EMBER_DEFAULT_BROADCAST_TABLE_SIZE 15

#ifndef EMBER_BROADCAST_TABLE_SIZE
  #define EMBER_BROADCAST_TABLE_SIZE EMBER_DEFAULT_BROADCAST_TABLE_SIZE
#elif EMBER_BROADCAST_TABLE_SIZE < EMBER_DEFAULT_BROADCAST_TABLE_SIZE
  #error "EMBER_BROADCAST_TABLE_SIZE is less than the minimum value of 15."
#elif 254 < EMBER_BROADCAST_TABLE_SIZE
  #error "EMBER_BROADCAST_TABLE_SIZE is larger than the maximum value of 254."
#endif

#ifndef EMBER_RETRY_QUEUE_SIZE
  #define EMBER_RETRY_QUEUE_SIZE 16
#endif

#ifndef EMBER_STORE_AND_FORWARD_QUEUE_SIZE
    #define EMBER_STORE_AND_FORWARD_QUEUE_SIZE 5
#endif

/** @brief Settings to control if and where assert information will
 * be printed.
 *
 * The output can be suppressed by defining @c EMBER_ASSERT_OUTPUT_DISABLED.
 * The serial port to which the output is sent
 * can be changed by defining ::EMBER_ASSERT_SERIAL_PORT as the desired port.
 *
 * The default is to have the assert output on and sent to serial port 1.
 */
#if !defined(EMBER_ASSERT_OUTPUT_DISABLED) \
  && !defined(EMBER_ASSERT_SERIAL_PORT)
  #define EMBER_ASSERT_SERIAL_PORT 1
#endif

/** @brief The time the stack will wait (in milliseconds) between sending blocks
 * of a fragmented message. The default value is 0.
 */
#ifndef EMBER_FRAGMENT_DELAY_MS
  #define EMBER_FRAGMENT_DELAY_MS 0
#endif

/** @brief The maximum number of blocks of a fragmented message that can be sent
 * in a single window is defined to be 8.
 */
#define EMBER_FRAGMENT_MAX_WINDOW_SIZE 8

/** @brief The number of blocks of a fragmented message that can be sent in a
 * single window. The maximum is ::EMBER_FRAGMENT_MAX_WINDOW_SIZE. The default
 * value is 1.
 */
#ifndef EMBER_FRAGMENT_WINDOW_SIZE
  #define EMBER_FRAGMENT_WINDOW_SIZE 1
#elif EMBER_FRAGMENT_MAX_WINDOW_SIZE < EMBER_FRAGMENT_WINDOW_SIZE
  #error "EMBER_FRAGMENT_WINDOW_SIZE is too large."
#endif

/** @brief The length of time that the device will wait for an answer
 *  to its Application Key Request. For the Trust Center, this is the time
 *  it will hold the first request and wait for a second matching request.
 *  If both arrive within this time period, the Trust Center will reply to
 *  both with the new key. If both requests are not received, the
 *  Trust Center will discard the request. The time is in minutes,
 *  with the maximum time being 10 minutes. A value of 0 minutes indicates that the
 *  Trust Center will not buffer the request but instead respond immediately.
 *  Only 1 outstanding request is supported at a time.
 *
 *  The Zigbee Pro Compliant value is 0.
 */
#ifndef EMBER_REQUEST_KEY_TIMEOUT
  #define EMBER_REQUEST_KEY_TIMEOUT 0
#elif EMBER_REQUEST_KEY_TIMEOUT > 10
  #error "EMBER_REQUEST_KEY_TIMEOUT is too large."
#endif

/** @brief The length of time, in seconds, that a trust center will store
 *  a transient link key that a device can use to join its network. A
 *  transient key is added with a call to emberAddTransientLinkKey. After the
 *  transient key is added, it will be removed once this amount of time has
 *  passed. A joining device will not be able to use that key to join until
 *  it is added again on the trust center. The default value is 300 seconds,
 *  i.e., 5 minutes.
 */
#ifndef EMBER_TRANSIENT_KEY_TIMEOUT_S
  #define EMBER_TRANSIENT_KEY_TIMEOUT_S (300)
#endif

/** @brief The time the coordinator will wait (in seconds) for a second end
 *  device bind request to arrive. The default value is 60.
 */
#ifndef EMBER_END_DEVICE_BIND_TIMEOUT
  #define EMBER_END_DEVICE_BIND_TIMEOUT 60
#endif

/** @brief The number of PAN ID conflict reports that must be received by
 * the network manager within one minute to trigger a PAN ID change.
 * Very rarely, a corrupt beacon can pass the CRC check and trigger a
 * false PAN id conflict. This is more likely to happen in very large
 * dense networks. Setting this value to 2 or 3 dramatically reduces
 * the chances of a spurious PAN ID change. The maximum value is 63.
 * The default value is 2.
 */
#ifndef EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD
  #define EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD 2
#endif

/** @brief The number of network level retries before network layer gives up
 *  sending a message, and return with an error. This could be seen as an
 *  extension to mac retries, and can take a range between 0..15. Setting
 *  the value to 0 disables network retries.
 */
#define EMBER_MAX_NETWORK_RETRIES 15
#ifndef EMBER_ZIGBEE_NUM_NETWORK_RETRIES_DEFAULT
  #define EMBER_ZIGBEE_NUM_NETWORK_RETRIES_DEFAULT 15
#endif
#if (EMBER_ZIGBEE_NUM_NETWORK_RETRIES_DEFAULT > EMBER_MAX_NETWORK_RETRIES)
  #error "Unsupported network retry attempts is configured. Supported values are 0..15"
#endif

/** @brief The number of event tasks that can be tracked for the purpose of
 *  processor idling. The Ember Zigbee Pro Stack requires two tasks,
 *  an application and associated libraries may use additional tasks,
 *  though typically no more than 5 are needed for most applications.
 */
#ifndef EMBER_TASK_COUNT
 #define EMBER_TASK_COUNT (5)
#endif

/** @brief The number of networks supported by the stack.
 */
#define EMBER_MAX_SUPPORTED_NETWORKS 2
#ifdef UC_BUILD
#if (SLI_ZIGBEE_SECONDARY_NETWORK_ENABLED == 0)
#define EMBER_SUPPORTED_NETWORKS 1
#else
#define EMBER_SUPPORTED_NETWORKS 2
#endif
#if defined(SL_CATALOG_ZIGBEE_NCP_FRAMEWORK_PRESENT) && defined(SL_CATALOG_ZIGBEE_MULTI_PAN_PRESENT)
#undef EMBER_SUPPORTED_NETWORKS
#define EMBER_SUPPORTED_NETWORKS 2
#endif
#else // !UC_BUILD
#ifndef EMBER_SUPPORTED_NETWORKS
#ifdef EMBER_TEST
 #define EMBER_SUPPORTED_NETWORKS 2
#else
 #define EMBER_SUPPORTED_NETWORKS 1
#endif
#endif
#endif // UC_BUILD

#ifndef EMBER_ZLL_GROUP_ADDRESSES
/** @brief The number of unique group identifiers that this device requires.
 */
  #define EMBER_ZLL_GROUP_ADDRESSES 1
#endif

#ifndef EMBER_ZLL_RSSI_THRESHOLD
/** @brief The RSSI threshold applied to incoming scan requests.
 */
  #define EMBER_ZLL_RSSI_THRESHOLD -128
#endif

#ifndef EMBER_GP_PROXY_TABLE_SIZE
/** @brief The number of proxy table entries supported.
 */
  #define EMBER_GP_PROXY_TABLE_SIZE 5
#endif

#ifndef EMBER_GP_SINK_TABLE_SIZE
/** @brief The number of sink table entries supported.
 */
  #define EMBER_GP_SINK_TABLE_SIZE 0
#endif

#ifndef EMBER_GP_INCOMING_FC_TOKEN_TIMEOUT
/** @brief The timeout (seconds) to restore the GPD incoming security frame counter in the Flash (0 being never).
 */
#define EMBER_GP_INCOMING_FC_TOKEN_TIMEOUT 7 // between 0 seconds (never) to 60 seconds
#endif

#ifndef EMBER_GP_INCOMING_FC_TOKEN_TABLE_SIZE
/** @brief The maximum table size to restore the GPD incoming security frame counter in the Flash (being either 0 or EMBER_GP_PROXY_TABLE_SIZE).
 */
#define EMBER_GP_INCOMING_FC_TOKEN_TABLE_SIZE 0 //EMBER_GP_PROXY_TABLE_SIZE
#endif

#ifndef EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TIMEOUT
/** @brief The timeout (seconds) to restore the GPD incoming security frame counter in the Flash (0 being never).
 */
#define EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TIMEOUT 7 // between 0 seconds (never) to 60 seconds
#endif

#ifndef EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TABLE_SIZE
/** @brief The maximum table size to restore the GPD incoming security frame counter in the Flash (being either 0 or EMBER_GP_SINK_TABLE_SIZE).
 */
#define EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TABLE_SIZE 0 //EMBER_GP_SINK_TABLE_SIZE
#endif

/** @brief The maximum number of ZC and ZR devices can be supported in a multi network mode.
 * All the ZC and ZR devices are expected to be on the same channel with different PAN IDs.
 * Multi network with multiple ZC and ZR devices are supported only on EFR platform.
 */
#define EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED 2
#if EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED > EMBER_SUPPORTED_NETWORKS
  #undef EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED
  #define EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED EMBER_SUPPORTED_NETWORKS
#endif

/** @brief The number of ZC and ZR device types are present in the application.
 * This configuration is used to expand the various stack tables and events
 * required to support the multi PAN application.
 * Note that for SoC, this define is generated by app builder based on the
 * device type(s) present in the application.
 */
#ifdef UC_BUILD

#if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_COORDINATOR_OR_ROUTER \
     || SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER)
// Primary network is coordinator or router
  #define NETWORK_0_ZC_AND_ZR_COUNT (1)
#else
// Primary network is end device
  #define NETWORK_0_ZC_AND_ZR_COUNT (0)
#endif

#if (SLI_ZIGBEE_SECONDARY_NETWORK_ENABLED > 0                                                             \
     && (SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_COORDINATOR_OR_ROUTER \
         || SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER))
// Secondary network is coordinator or router
  #define NETWORK_1_ZC_AND_ZR_COUNT (1)
#else
// Secondary network not enabled or it is end device
  #define NETWORK_1_ZC_AND_ZR_COUNT (0)
#endif
// Handle the NCP Multi pan case and set the EMBER_ZC_AND_ZR_DEVICE_COUNT to 2
#if defined(SL_CATALOG_ZIGBEE_NCP_FRAMEWORK_PRESENT) && defined(SL_CATALOG_ZIGBEE_MULTI_PAN_PRESENT)
  #undef EMBER_ZC_AND_ZR_DEVICE_COUNT
  #define EMBER_ZC_AND_ZR_DEVICE_COUNT 2
#elif (defined(EMBER_NO_STACK) && !defined(EMBER_ZC_AND_ZR_DEVICE_COUNT))
// For no stack application such nodetest/sniffer.
#define EMBER_ZC_AND_ZR_DEVICE_COUNT (1)
#else
#define EMBER_ZC_AND_ZR_DEVICE_COUNT (NETWORK_0_ZC_AND_ZR_COUNT + NETWORK_1_ZC_AND_ZR_COUNT)
#endif

#if (EMBER_ZC_AND_ZR_DEVICE_COUNT > EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED)
  #error "Exceeded the maximum number of supported ZC and ZR devices."
#endif

#else // !UC_BUILD

#if defined(EMBER_AF_NCP) && defined(EMBER_AF_PLUGIN_MULTI_PAN_LIBRARY)
  #undef EMBER_AF_ZC_AND_ZR_DEVICE_COUNT    // ToDo: Remove #if part, would not need it with UC apps.
  #define EMBER_AF_ZC_AND_ZR_DEVICE_COUNT 2 // For Multi-PAN NCP
#elif defined(EMBER_NO_STACK) && !defined(EMBER_AF_ZC_AND_ZR_DEVICE_COUNT)
// For no stack application such nodetest/sniffer.
  #define EMBER_AF_ZC_AND_ZR_DEVICE_COUNT 1
#endif

#if (EMBER_AF_ZC_AND_ZR_DEVICE_COUNT > EMBER_MULTI_NETWORK_MAX_ZC_AND_ZR_DEVICES_SUPPORTED)
  #error "Exceeded the maximum number of supported ZC and ZR devices."
#endif

#endif // UC_BUILD

// This table temporarily keeps track of devices that are joining
// To allow "n" devices to join simulataneously, this table needs to be capable of
// holding n entries. Since this space is only used when necessary and times out fairly
// quickly, we set it to 64 (same as max child table size) to prevent all the additional
// code to allow configuration of this table size
#ifndef SL_ZIGBEE_TRANSIENT_DEVICE_MGMT_MAX_CAPACITY
  #define SL_ZIGBEE_TRANSIENT_DEVICE_MGMT_MAX_CAPACITY 64
#endif

#ifndef SL_ZIGBEE_TRANSIENT_DEVICE_DEFAULT_TIMEOUT_MS
  #define SL_ZIGBEE_TRANSIENT_DEVICE_DEFAULT_TIMEOUT_MS (10000u)
#endif

#ifndef EMBER_CUSTOM_MAC_FILTER_TABLE_SIZE
#if !defined(EMBER_TEST) && !defined(ZIGBEE_STACK_ON_HOST)
  #define EMBER_CUSTOM_MAC_FILTER_TABLE_SIZE 2
#else
  #define EMBER_CUSTOM_MAC_FILTER_TABLE_SIZE 64
#endif
#endif

/** @brief The 802.15.4 CCA mode that should be used at startup. The default
 * value is RAIL_IEEE802154_CCA_MODE_RSSI.
 * This value is configured in the radio during initialization. The user may
 * override this value at runtime using ::emberSetRadioIeee802154CcaMode.
 *
 * @note The value of this macro is used at startup for SoC and NCP
 * applications only. A host application does not use this value, nor does it
 * set the CCA mode at startup by default.
 */
#ifdef SL_CATALOG_RAIL_LIB_PRESENT
  #include "rail_ieee802154.h"
  #ifndef EMBER_RADIO_802154_CCA_MODE
    #define EMBER_RADIO_802154_CCA_MODE RAIL_IEEE802154_CCA_MODE_RSSI
  #endif // EMBER_RADIO_802154_CCA_MODE
#endif // SL_CATALOG_RAIL_LIB_PRESENT

#ifdef EMBER_TEST
  #undef EMBER_RADIO_802154_CCA_MODE
  #define EMBER_RADIO_802154_CCA_MODE 0
#endif

/** @} END addtogroup */

#endif //__EMBER_CONFIGURATION_DEFAULTS_H__
