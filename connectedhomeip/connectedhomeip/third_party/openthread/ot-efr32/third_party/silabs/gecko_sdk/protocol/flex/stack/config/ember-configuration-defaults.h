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

/*
 * Application developers should \b not modify any portion
 * of this file. Doing so may cause mysterious bugs. Allocations should be
 * adjusted only by defining the appropriate macros in the application's
 * CONFIGURATION_HEADER.
 */

/**
 * @addtogroup configuration
 * @brief User-configurable stack configuration macros and defaults.
 *
 * Connect stack provides various interfaces to apply configuration:
 *  - APIs to change configuration run-time
 *  - Manufacturing tokens to change configuration during device flashing
 *  - Macros to change configuration at compile-time
 *
 * The compile time configuration macros and their default values are listed
 * here.
 *
 * The default values are always chosen to make it usable for most applications,
 * but in some cases, you might need to change it. The recommended way is to
 * either define the macro you need to change in the compilation command
 * (e.g. gcc -D) or you by manipulating the configuration options in the
 * configurator GUI.
 *
 * See ember-configuration-defaults.h for source code.
 * @{
 */

#ifndef __EMBER_CONFIGURATION_DEFAULTS_H__
#define __EMBER_CONFIGURATION_DEFAULTS_H__

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_CONNECT_STACK_COMMON_PRESENT)
#include "stack-common-config.h"
#endif

#if defined(SL_CATALOG_CONNECT_FREQUENCY_HOPPING_PRESENT)
#include "frequency-hopping-config.h"
#endif

#if defined(SL_CATALOG_CONNECT_PARENT_SUPPORT_PRESENT)
#include "parent-support-config.h"
#endif

#if defined(SL_CATALOG_CONNECT_MAC_QUEUE_PRESENT)
#include "mac-queue-config.h"
#endif

// TODO: include here other stack components configuration headers

/**
 * @brief The size in bytes of the Ember heap. See @ref memory_buffer for more
 * details.
 * @warning This should be configured from the Parent Support plugin options.
 */
#ifndef EMBER_HEAP_SIZE
  #define EMBER_HEAP_SIZE                           2000
#endif

/**
 * @brief The maximum number of children supported by the device. Can be
 * configured from 0 to 64. 11B of token space is allocated for each child in
 * the table.
 * @note It's recommended to set it to 64 for @ref EMBER_STAR_COORDINATOR, 32
 * for @ref EMBER_STAR_RANGE_EXTENDER and 0 for anything else.
 * @warning This should be configured from the Parent Support plugin options.
 */
#ifndef EMBER_CHILD_TABLE_SIZE
  #define EMBER_CHILD_TABLE_SIZE                    0
#endif

#define EMBER_CHILD_TABLE_TOKEN_SIZE                EMBER_CHILD_TABLE_SIZE

/**
 * @brief Every child should exchange regularly some sort of traffic with the
 * parent. Eventually, if traffic is not exchanged for a prolonged period of
 * time, the parent may remove the child from the child table. In particular the
 * parent shall remove the oldest stale child whenever the child table is full
 * and there is the need of making room for a new child.
 * Range extenders periodically exchange network-level commands with the
 * coordinator. End devices and sleepy end devices can use
 * @ref emberPollForData() as keep alive mechanism, or use the @ref poll plugin.
 * The maximum allowed timeout value is ::EMBER_CHILD_TABLE_MAX_TIMEOUT_S.
 * Setting the timeout value to ::EMBER_CHILD_TABLE_AGING_DISABLED disables
 * aging of the child table.
 *
 * @warning This should be configured from the Parent Support plugin options.
 */
#ifndef EMBER_CHILD_TIMEOUT_SEC
  #define EMBER_CHILD_TIMEOUT_SEC                   3600
#endif

#ifdef EMBER_CHILD_TABLE_MAX_TIMEOUT_S
  #if (EMBER_CHILD_TIMEOUT_SEC > EMBER_CHILD_TABLE_MAX_TIMEOUT_S)
    #error "Invalid child table timeout value"
  #endif
#endif

/**
 * @brief Indirect queue is used on a parent to store a message intended for a
 * sleepy end device, this configures the size of that queue. Configure it to 0
 * if parent support plugin is not used.
 * @warning This should be configured from the Parent Support plugin options.
 */
#ifndef EMBER_INDIRECT_QUEUE_SIZE
  #define EMBER_INDIRECT_QUEUE_SIZE                 0
#endif

/**
 * @brief MAC Outgoing paclet queue is to store messages until the radio is
 * available to send it (In most cases, the radio is unavailable because it's
 * already transmitting). The configures the size of that queue.
 * @warning This should be configured from the MAC Packet Queue plugin options.
 */
#ifndef EMBER_MAC_OUTGOING_QUEUE_SIZE
  #define EMBER_MAC_OUTGOING_QUEUE_SIZE             0
#endif

/**
 * @brief The maximum amount of time (in milliseconds) that the MAC
 * will hold a message for indirect transmission to a child. The maximum value
 * is 30 seconds (30000 milliseconds).
 * @warning This should be configured from the Parent Support plugin options.
 */
#ifndef EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS
  #define EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS 8000
#endif

/**
 * @brief The period in seconds a range extender sends an update command to the
 * coordinator containing the list of its children.
 * @attention This option is only used on @ref EMBER_STAR_RANGE_EXTENDER device.
 */
#ifndef EMBER_NWK_RANGE_EXTENDER_UPDATE_PERIOD_SEC
  #define EMBER_NWK_RANGE_EXTENDER_UPDATE_PERIOD_SEC     60
#endif

/**
 * @brief The ACK timeout in milliseconds. This parameter should be fine-tuned
 * to reduce energy consumption for sleepy devices and depends on the data rate
 * of the PHY configuration used. The maximum allowed value is 65.
 */
#ifndef EMBER_MAC_ACK_TIMEOUT_MS
#define EMBER_MAC_ACK_TIMEOUT_MS 25
#endif // EMBER_MAC_ACK_TIMEOUT_MS

/**
 * @brief The CCA threshold used at the MAC layer for CSMA/CA, in dBm.
 * @warning This should be configured from the Connect Stack plugin options.
 */
#ifndef EMBER_RADIO_CCA_THRESHOLD
#define EMBER_RADIO_CCA_THRESHOLD -65
#endif // EMBER_RADIO_CCA_THRESHOLD

/**
 * @brief The frequency hopping channel sequence generation seed. Can be
 * configured between 0 and 65535. See the Frequency Hopping chapter of UG235.03
 * for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SEED
#define EMBER_FREQUENCY_HOPPING_SEED 0
#endif // EMBER_FREQUENCY_HOPPING_SEED

/**
 * @brief The lowest channel on the frequency hopping list. See the Frequency
 * Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_START_CHANNEL
#define EMBER_FREQUENCY_HOPPING_START_CHANNEL 0
#endif // EMBER_FREQUENCY_HOPPING_START_CHANNEL

/**
 * @brief The highest channel on the frequency hopping list. See the Frequency
 * Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_END_CHANNEL
#define EMBER_FREQUENCY_HOPPING_END_CHANNEL 24
#endif // EMBER_FREQUENCY_HOPPING_END_CHANNEL

/**
 * @brief The time in milliseconds to stay on each channel for frequency
 * hopping. See the Frequency Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS
#define EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS 400
#endif // EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS

/**
 * @brief The time in milliseconds to guard each channel while frequency
 * hopping. No MAC activity is allowed when entering or exiting the slot. See
 * the Frequency Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS
#define EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS 20
#endif // EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS

/**
 * @brief The duration in seconds after which the server should broadcast
 * its frequency hopping information to allow clients to realign. See the
 * Frequency Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S
#define EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S 15
#endif // EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S

/**
 * @brief The duration in seconds after which a sleepy client should resync
 * with the server if the last resync happened more than this duration ago.
 * See the Frequency Hopping chapter of UG235.03 for more details.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SLEEPY_CLIENT_RESYNC_PERIOD_S
#define EMBER_FREQUENCY_HOPPING_SLEEPY_CLIENT_RESYNC_PERIOD_S 60
#endif // EMBER_FREQUENCY_HOPPING_SLEEPY_CLIENT_RESYNC_PERIOD_S

/**
 * @brief The maximum duration in seconds a non sleepy client would keep hopping
 * without receiving frequency hopping information from the server, after which
 * the synchronization with the server is deemed lost. A special value of
 * ::EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_DISABLE_TIMEOUT disables
 * this timeout.
 */
#ifndef EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_TIMEOUT_S
#define EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_TIMEOUT_S (100)
#endif // EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_TIMEOUT_S

/**
 * @brief When a node is started a frequency hopping server, it will first
 * advertise on all channels to resynchronize all existing clients in case the
 * server was started as result of a reboot. This parameter defines the number
 * of iterations over the entire hopping sequence.
 */
#ifndef EMBER_FREQUENCY_HOPPING_SERVER_ADVERTISING_ITERATION_COUNT
#define EMBER_FREQUENCY_HOPPING_SERVER_ADVERTISING_ITERATION_COUNT (3)
#endif // EMBER_FREQUENCY_HOPPING_SERVER_ADVERTISING_ITERATION_COUNT

/**
 * @brief An @ref EMBER_STAR_COORDINATOR assigns short IDs to other nodes in
 * the star network sequentially starting from this short ID. This option
 * provides a simple effective way to reserve an pool of short addresses for
 * commissioning.
 */
#ifndef EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED
#define EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED 1
#endif // EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED

/**
 * @brief The size of the short-to-long address mapping table.
 * See @ref emberMacAddShortToLongAddressMapping for more details.
 * @attention This table is only used for @ref EMBER_MAC_MODE_DEVICE and
 * @ref EMBER_MAC_MODE_SLEEPY_DEVICE, but the memory will be allocated on all
 * device types.
 */
#ifndef EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE
#define EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE 10
#endif // EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE

/** @} END addtogroup */

#endif //__EMBER_CONFIGURATION_DEFAULTS_H__
