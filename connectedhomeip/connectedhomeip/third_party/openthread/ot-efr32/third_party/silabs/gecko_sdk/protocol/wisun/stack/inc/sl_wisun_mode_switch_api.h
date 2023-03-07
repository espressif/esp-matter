/***************************************************************************//**
 * @file sl_wisun_mode_switch_api.h
 * @brief Wi-SUN API
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

#ifndef SL_WISUN_MODE_SWITCH_API_H
#define SL_WISUN_MODE_SWITCH_API_H

#include "sl_wisun_types.h"
#include "sl_wisun_events.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Wi-SUN Mode switch Message API indication IDs
typedef enum {
  /// This event is sent when the mode switch is disabled
  SL_WISUN_MSG_MODE_SWITCH_FALLBACK_IND_ID        = 0xFE // WARNING should not overlap sl_wisun_msg_ind_id_t
} sl_wisun_mode_switch_msg_ind_id_t;

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_MODE_SWITCH_FALLBACK_IND sl_wisun_mode_switch_fallback_level_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_mode_switch_fallback_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_mode_switch_fallback_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_mode_switch_fallback_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MODE_SWITCH_FALLBACK_IND) */

/**************************************************************************//**
 * @addtogroup SL_WISUN_API Wi-SUN Stack API
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @brief Set the mode switch configuration
 * @param[in] mode Indicate how the device uses mode switch feature
 * @param[in] phy_mode_id PhyModeId to use on mode switch when enabled
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_mode_switch(uint8_t mode, uint8_t phy_mode_id);

/** @} (end SL_WISUN_API) */

#ifdef __cplusplus
}
#endif

#endif  // SL_WISUN_MODE_SWITCH_API_H
