/***************************************************************************//**
 * @file sl_wisun_api.h
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

#ifndef SL_WISUN_TRACE_API_H
#define SL_WISUN_TRACE_API_H

#include "sl_wisun_types.h"
#include "sl_wisun_events.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @addtogroup SL_WISUN_TRACE_API Wi-SUN Stack Traces and Debug API
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @brief Set the trace level.
 * @param[in] group_count Number of groups to configure.
 *                        If 0, enable all levels for all groups.
 *                        Maximum SL_WISUN_TRACE_GROUP_COUNT
 * @param[in] trace_config  Table with \p group_count element filled. It indicates the
 *                          trace level to be displayed for each group.
 * @return SL_STATUS_NOT_AVAILABLE if the Wi-SUN Debug & Traces component is not installed
 *         SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_trace_level(uint8_t group_count, sl_wisun_trace_group_config_t *trace_config);

/**************************************************************************//**
 * @brief Set the trace filter. Indicate which trace group will be displayed.
 * @param[in] filter Bitfield. Each 1 indicates the trace group ID corresponding
 *                   is selected for tracing. 0 means the trace group ID corresponding
 *                   should be filtered out. It follows enum sl_wisun_trace_group_t
 *                   filter[0] contains ID 0 to 7
 *                   filter[1] contains ID 8 to 15...
 * @return SL_STATUS_NOT_AVAILABLE if the Wi-SUN Debug & Traces component is not installed
 *         SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_set_trace_filter(uint8_t filter[SL_WISUN_FILTER_BITFIELD_SIZE]);

/** @} (end SL_WISUN_TRACE_API) */

#ifdef __cplusplus
}
#endif

#endif  // SL_WISUN_TRACE_API_H
