/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BTMESH_CONF_TYPES_H
#define BTMESH_CONF_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "sl_status.h"
#include "sl_slist.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_capi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_types BT Mesh Configuration Types
 * @brief BT Mesh Configuration Types provides common definitions and types.
 * @{
 ******************************************************************************/

/// Unknown network key index value
#define BTMESH_CONF_NETKEY_INDEX_UNASSIGNED          0xFFFF

/// Unknown model publication element index value
#define BTMESH_CONF_PUB_ELEM_INDEX_UNKNOWN           0xFF
/// Unknown model publication vendor ID value
#define BTMESH_CONF_PUB_VENDOR_ID_UNKNOWN            0xFFFF
/// Unknown model publication model ID value
#define BTMESH_CONF_PUB_MODEL_ID_UNKNOWN             0xFFFF
/// Unknown model publication address value
#define BTMESH_CONF_PUB_ADDRESS_UNKNOWN              0xFFFF
/// Unknown model publication application key value
#define BTMESH_CONF_PUB_APPKEY_INDEX_UNKNOWN         0xFFFF
/// Unknown model publication friendship credentials flag value
#define BTMESH_CONF_PUB_CREDENTIALS_UNKNOWN          0xFF
/// Unknown model publication TTL value
#define BTMESH_CONF_PUB_TTL_UNKNOWN                  0xFF
/// Unknown model publication period value
#define BTMESH_CONF_PUB_PERIOD_MS_UNKNOWN            0xFFFFFFFF
/// Unknown model publication retransmit count value
#define BTMESH_CONF_PUB_RETRANSMIT_CNT_UNKNOWN       0xFF
/// Unknown model publication retransmit interval value
#define BTMESH_CONF_PUB_RETRANSMIT_INT_MS_UNKNOWN    0xFFFF

/// Unknown node default TTL value
#define BTMESH_CONF_DEFAULT_TTL_UNKNOWN              0xFF
/// Unknown node proxy value
#define BTMESH_CONF_PROXY_VALUE_UNKNOWN              0xFF
/// Unknown node relay value
#define BTMESH_CONF_RELAY_VALUE_UNKNOWN              0xFF
/// Unknown node relay retransmit count
#define BTMESH_CONF_RELAY_RETRANSMIT_CNT_UNKNOWN     0xFF
/// Unknown node relay retransmit interval
#define BTMESH_CONF_RELAY_RETRANSMIT_INT_MS_UNKNOWN  0xFFFF
/// Unknown node network transmit count
#define BTMESH_CONF_NW_TRANSMIT_CNT_UNKNOWN          0xFF
/// Unknown node network transmit interval
#define BTMESH_CONF_NW_TRANSMIT_INT_MS_UNKNOWN       0xFFFF
/// Unknown node friend value
#define BTMESH_CONF_FRIEND_VALUE_UNKNOWN             0xFF

/// Maximum theoretical number of application keys
#define BTMESH_CONF_MAX_APPKEY_COUNT    (1 << 12)
/// Maximum theoretical number of network keys
#define BTMESH_CONF_MAX_NETKEY_COUNT    (1 << 12)
/// @brief Maximum theoretical number of subscription addresses
/// BT Mesh model can subscribe to group addresses (14bit) and to virtual
/// addresses (14bit) and to unicast address of the node element which the
/// model resides on.
#define BTMESH_CONF_MAX_SUB_ADDR_COUNT  ((1 << 14) + (1 << 14) + 1)

/// Variable argument unsigned initializer macro
#define BTMESH_CONF_VARG_U32(u32) (btmesh_conf_varg_t){ .u32 = u32 }
/// Variable argument pointer initializer macro
#define BTMESH_CONF_VARG_PTR(ptr) (btmesh_conf_varg_t){ .ptr = ptr }
/// Variable argument NULL pointer initializer macro
#define BTMESH_CONF_VARG_NULL     (btmesh_conf_varg_t){ .ptr = NULL }

/// Variable argument union type to provide application specific information
typedef union {
  uint32_t u32; ///< Unsigned 32bit variable
  void *ptr;    ///< Pointer variable
} btmesh_conf_varg_t;

/** @} (end addtogroup btmesh_conf_types) */
/** @} (end addtogroup btmesh_conf) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_CONF_TYPES_H */
