/***************************************************************************//**
 * @brief User-configurable stack memory allocation and convenience stubs
 * for little-used callbacks.
 *
 *
 * \b Note: Application developers should \b not modify any portion
 * of this file. Doing so may lead to mysterious bugs. Allocations should be
 * adjusted only with macros in the Connect Stack common configuration header.
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

#include "em_common.h"

#include "stack/include/ember.h"
#include "stack/include/error.h"

#include "stack/config/ember-configuration-defaults.h"

PGM uint8_t emTaskCount = EMBER_TASK_COUNT;
EmberTaskControl emTasks[EMBER_TASK_COUNT];

// Configurable stack parameters.
uint8_t emberMacAckTimeoutMS = EMBER_MAC_ACK_TIMEOUT_MS;
int8_t emRadioCcaThreshold = EMBER_RADIO_CCA_THRESHOLD;

#ifdef SL_CATALOG_CONNECT_FREQUENCY_HOPPING_PRESENT

uint16_t emberFrequencyHoppingSeed = EMBER_FREQUENCY_HOPPING_SEED;
uint16_t emberFrequencyHoppingStartChannel = EMBER_FREQUENCY_HOPPING_START_CHANNEL;
uint16_t emberFrequencyHoppingEndChannel = EMBER_FREQUENCY_HOPPING_END_CHANNEL;
uint16_t emberFrequencyHoppingChannelDurationMs = EMBER_FREQUENCY_HOPPING_CHANNEL_DURATION_MS;
uint16_t emberFrequencyHoppingChannelGuardDurationMs = EMBER_FREQUENCY_HOPPING_CHANNEL_GUARD_DURATION_MS;
uint16_t emberFrequencyHoppingServerFreqInfoBroadcastPeriodS = EMBER_FREQUENCY_HOPPING_SERVER_FREQ_INFO_BROADCAST_PERIOD_S;
uint16_t emberFrequencyHoppingSleepyClientResyncPeriodS = EMBER_FREQUENCY_HOPPING_SLEEPY_CLIENT_RESYNC_PERIOD_S;
uint16_t emberFrequencyHoppingAlwaysOnClientSyncTimeoutS = EMBER_FREQUENCY_HOPPING_ALWAYS_ON_CLIENT_SYNC_TIMEOUT_S;
uint8_t  emberFrequencyHoppingServerAdvertisingIterationCount = EMBER_FREQUENCY_HOPPING_SERVER_ADVERTISING_ITERATION_COUNT;

#endif // SL_CATALOG_CONNECT_FREQUENCY_HOPPING_PRESENT

#if defined(CORTEXM3)
// This declaration is performed only to ensure that the size of the memory heap
// is *at least* EMBER_HEAP_SIZE (if not, the linker would return an error).
// The buffer management code uses the full heap memory segment.
SL_ALIGN(4)
uint16_t heapMemory[(EMBER_HEAP_SIZE + 1) / 2] SL_ATTRIBUTE_ALIGN(4);
#else
uint16_t heapMemory[(EMBER_HEAP_SIZE + 1) / 2];
#endif
const uint32_t heapMemorySize = EMBER_HEAP_SIZE;

#ifdef SL_CATALOG_CONNECT_MAC_QUEUE_PRESENT

uint8_t emberMacOutgoingQueueSize = EMBER_MAC_OUTGOING_QUEUE_SIZE;
static EmOutgoingPacket macOutgoingQueue[EMBER_MAC_OUTGOING_QUEUE_SIZE];
EmOutgoingPacket *emMacOutgoingQueue = macOutgoingQueue;

#endif // SL_CATALOG_CONNECT_MAC_QUEUE_PRESENT

#ifdef SL_CATALOG_CONNECT_PARENT_SUPPORT_PRESENT

uint32_t emberChildTimeoutSec = EMBER_CHILD_TIMEOUT_SEC;
uint8_t emberChildTableSize = EMBER_CHILD_TABLE_SIZE;
static EmberChildEntry childTable[EMBER_CHILD_TABLE_SIZE];
EmberChildEntry *emChildTable = childTable;

uint8_t emberIndirectPacketQueueSize = EMBER_INDIRECT_QUEUE_SIZE;
static EmOutgoingPacket indirectPacketQueue[EMBER_INDIRECT_QUEUE_SIZE];
static uint32_t indirectQueueTimeouts[EMBER_INDIRECT_QUEUE_SIZE];
static EmberMacAddress indirectQueueSourceAddresses[EMBER_INDIRECT_QUEUE_SIZE];
EmOutgoingPacket *emIndirectPacketQueue = indirectPacketQueue;
EmberMacAddress *emIndirectQueueSourceAddresses = indirectQueueSourceAddresses;
uint32_t *emIndirectQueueTimeouts = indirectQueueTimeouts;
uint32_t emberMacIndirectTimeoutMs = EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS;

uint16_t emberCoordinatorFirstShortIdToBeAssigned = EMBER_COORDINATOR_FIRST_SHORT_ID_TO_BE_ASSIGNED;

#endif // SL_CATALOG_CONNECT_PARENT_SUPPORT_PRESENT
