/***************************************************************************//**
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

/**
 * All configurations have defaults. Therefore, many applications may not need
 * to do anything special.  However, you can override these defaults by creating
 * a configuration header and, within this header, defining the appropriate
 * macro to a different size.
 *
 * See ember-configuration-defaults.h for source code.
 */

#ifndef __EMBER_CONFIGURATION_DEFAULTS_H__
#define __EMBER_CONFIGURATION_DEFAULTS_H__

#if defined(CONNECT_STACK_COMMON)
#include "stack-common-config.h"
#endif

#if defined(CONNECT_STACK_FREQUENCY_HOPPING)
#include "frequency-hopping-config.h"
#endif

#if defined(CONNECT_STACK_PARENT_SUPPORT)
#include "parent-support-config.h"
#endif

#if defined(CONNECT_STACK_MAC_QUEUE)
#include "mac-queue-config.h"
#endif

// TODO: include here other stack components configuration headers

/** @brief The number of event tasks that can be tracked for the purpose of
 *  processor idling.
 */
#ifndef EMBER_TASK_COUNT
 #define EMBER_TASK_COUNT (3)
#endif

/** @brief The size in bytes of the Ember heap.
 */
#ifndef EMBER_HEAP_SIZE
  #define EMBER_HEAP_SIZE                           2000
#endif

/** @brief The maximum number of children that a node may have set by
 * default to 0. If the application includes the optional parent support stack
 * feature, this should be adjusted accordingly.
 */
#ifndef EMBER_CHILD_TABLE_SIZE
  #define EMBER_CHILD_TABLE_SIZE                    0
#endif

#define EMBER_CHILD_TABLE_TOKEN_SIZE                EMBER_CHILD_TABLE_SIZE

/** @brief Every child should exchange regularly some sort of traffic with the
 * parent. Eventually, if traffic is not exchanged for a prolonged period of
 * time, the parent may remove the child from the child table. In particular the
 * parent shall remove the oldest stale child whenever the child table is full
 * and there is the need of making room for a new child.
 * Range extenders periodically exchange network-level commands with the
 * coordinator. End devices and sleepy end devices can use ::emberPollForData()
 * as keep alive mechanism.
 */
#ifndef EMBER_CHILD_TIMEOUT_SEC
  #define EMBER_CHILD_TIMEOUT_SEC                   3600
#endif

/** @brief The maximum number of pending indirect packets set by
 * default to 0. If the application includes the optional parent support stack
 * feature, this should be adjusted accordingly.
 */
#ifndef EMBER_INDIRECT_QUEUE_SIZE
  #define EMBER_INDIRECT_QUEUE_SIZE                 0
#endif

/** @brief The maximum number of packets in the outgoing MAC queue set
 * by default to 0. If the application includes the optional MAC queue stack
 * feature, this should be adjusted accordingly.
 */
#ifndef EMBER_MAC_OUTGOING_QUEUE_SIZE
  #define EMBER_MAC_OUTGOING_QUEUE_SIZE             0
#endif

/** @brief The maximum amount of time (in milliseconds) that the MAC
 * will hold a message for indirect transmission to a child.
 *
 * The default is 8000 milliseconds (8 sec).
 * The maximum value is 30 seconds (30000 milliseconds). Larger values
 * will cause rollover confusion.
 */
#ifndef EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS
  #define EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS 8000
#endif

/** @brief The period in seconds a range extender sends an update command to the
 * coordinator containing the list of its children.
 */
#ifndef EMBER_NWK_RANGE_EXTENDER_UPDATE_PERIOD_SEC
  #define EMBER_NWK_RANGE_EXTENDER_UPDATE_PERIOD_SEC     60
#endif

/** @brief The ACK timeout in milliseconds. This parameter should be fine-tuned to
 * reduce energy consumption for sleepy devices and depends on the data rate
 * of the PHY configuration used.
 */
#ifndef EMBER_MAC_ACK_TIMEOUT_MS
#define EMBER_MAC_ACK_TIMEOUT_MS 25
#endif // EMBER_MAC_ACK_TIMEOUT_MS

/** @brief The CCA threshold used at the MAC layer.
 */
#ifndef EMBER_RADIO_CCA_THRESHOLD
#define EMBER_RADIO_CCA_THRESHOLD -65
#endif // EMBER_RADIO_CCA_THRESHOLD

/** @brief The frequency hopping channel sequence generation seed.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SEED
#define EMBER_FREQUENCY_HOPPING_SEED 0
#endif // EMBER_FREQUENCY_HOPPING_SEED

/** @brief The lowest channel on the frequency hopping list.
 */
#ifndef EMBER_FREQUENCY_HOPPING_START_CHANNEL
#define EMBER_FREQUENCY_HOPPING_START_CHANNEL 0
#endif // EMBER_FREQUENCY_HOPPING_START_CHANNEL

/** @brief The highest channel on the frequency hopping list.
 */
#ifndef EMBER_FREQUENCY_HOPPING_END_CHANNEL
#define EMBER_FREQUENCY_HOPPING_END_CHANNEL 24
#endif // EMBER_FREQUENCY_HOPPING_END_CHANNEL

/** @brief The time in milliseconds to stay on each channel for frequency
 *  hopping.
 */
#ifndef EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS
#define EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS 400
#endif // EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS

/** @brief The time in milliseconds to guard each channel while frequency
 *  hopping. No MAC activity is allowed when entering or exiting the slot.
 */
#ifndef EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS
#define EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS 20
#endif // EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS

/** @brief The duration in seconds after which the server should broadcast
 * its frequency hopping information to allow clients to realign.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S
#define EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S 15
#endif // EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S

/** @brief The duration in seconds after which the client should resync
 * with the server if the last resync happened more than this duration ago.
 */
#ifndef EMBER_FREQUENCY_HOPPING_CLIENT_RESYNC_PERIOD_S
#define EMBER_FREQUENCY_HOPPING_CLIENT_RESYNC_PERIOD_S 60
#endif // EMBER_FREQUENCY_HOPPING_CLIENT_RESYNC_PERIOD_S

/** @brief A star coordinator assigns short IDs to other nodes in the star
 * network sequentially starting from this short ID. This option provides a
 * simple effective way to reserve an interval of short addresses.
 */
#ifndef EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED
#define EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED 1
#endif // EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED

/** @brief The size of the short-to-long address mapping table.
 */
#ifndef EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE
#define EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE 10
#endif // EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE

#endif //__EMBER_CONFIGURATION_DEFAULTS_H__
