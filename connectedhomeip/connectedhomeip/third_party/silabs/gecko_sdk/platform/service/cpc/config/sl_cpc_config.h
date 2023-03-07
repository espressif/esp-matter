/***************************************************************************//**
 * @file
 * @brief CPC configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_CPC_CONFIG_H
#define SL_CPC_CONFIG_H

// <h>CPC Configuration

// <o SL_CPC_RX_PAYLOAD_MAX_LENGTH>Max Rx Payload Length<1-4087>
// <i> Default: 256
// <i> Maximum size of the payload in bytes of each RX buffer
#define SL_CPC_RX_PAYLOAD_MAX_LENGTH      (256)

// <o SL_CPC_TX_QUEUE_ITEM_MAX_COUNT> Tx Queue Size
// <i> Default: 20
// <i> The maximum number of outgoing messages capable of being queued for transmission.
#define SL_CPC_TX_QUEUE_ITEM_MAX_COUNT    (20)

// <o SL_CPC_RX_BUFFER_MAX_COUNT> Total Number of Rx Buffers
// <i> Default: 20
// <i> Total number of CPC RX buffers available across all endpoints
// <i> Multiple RX buffers can be associated with a single endpoint
#define SL_CPC_RX_BUFFER_MAX_COUNT        (20)

// <o SL_CPC_USER_ENDPOINT_MAX_COUNT>Max Number of User Endpoints<0-10>
// <i> Default: 10
// <i> Maximum number of CPC user endpoints that can be opened at once.
#define SL_CPC_USER_ENDPOINT_MAX_COUNT    (10)

// <q SL_CPC_DEBUG_SYSTEM_VIEW_LOG_CORE_EVENT> Enable debug core tracing with system view
// <i> Default: 0
#define SL_CPC_DEBUG_SYSTEM_VIEW_LOG_CORE_EVENT       0

// <q SL_CPC_DEBUG_SYSTEM_VIEW_LOG_ENDPOINT_EVENT> Enable debug endpoint tracing with system view
// <i> Default: 0
#define SL_CPC_DEBUG_SYSTEM_VIEW_LOG_ENDPOINT_EVENT    0

// <q SL_CPC_DEBUG_CORE_EVENT_COUNTERS> Enable debug counters for core events
// <i> Default: 0
#define SL_CPC_DEBUG_CORE_EVENT_COUNTERS     0

// <q SL_CPC_DEBUG_ENDPOINT_EVENT_COUNTERS> Enable debug counters for endpoint events
// <i> Default: 0
#define SL_CPC_DEBUG_ENDPOINT_EVENT_COUNTERS     0

// <q SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS> Enable debug counters for memory allocation
// <i> Default: 0
#define SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS     0

// </h>

// <<< end of configuration section >>>

#endif /* SL_CPC_CONFIG_H */
