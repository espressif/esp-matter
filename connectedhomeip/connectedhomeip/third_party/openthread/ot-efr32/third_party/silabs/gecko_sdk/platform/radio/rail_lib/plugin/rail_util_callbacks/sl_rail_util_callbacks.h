/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SL_RAIL_UTIL_CALLBACKS_H
#define SL_RAIL_UTIL_CALLBACKS_H

#include "rail.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A callback available to the application, called on RAIL asserts.
 *
 * @param[in] rail_handle The RAIL handle associated with the assert.
 * @param[in] error_code The assertion error code.
 */
void sl_rail_util_on_assert_failed(RAIL_Handle_t rail_handle,
                                   RAIL_AssertErrorCodes_t error_code);

/**
 * A callback available to the application, called on RAIL init completion.
 *
 * @param[in] rail_handle The RAIL handle associated with the RAIL init
 * completion notification.
 */
void sl_rail_util_on_rf_ready(RAIL_Handle_t rail_handle);

/**
 * A callback available to the application, called on a channel configuration
 * change.
 *
 * @param[in] rail_handle The RAIL handle associated with the channel config
 * change notification.
 * @param[in] entry The channel configuration being changed to.
 */
void sl_rail_util_on_channel_config_change(RAIL_Handle_t rail_handle,
                                           const RAIL_ChannelConfigEntry_t *entry);

/**
 * A callback available to the application, called on registered RAIL events.
 *
 * @param[in] rail_handle The RAIL handle associated with the RAIL event
 * notification.
 * @param[in] events The RAIL events having occurred.
 */
void sl_rail_util_on_event(RAIL_Handle_t rail_handle,
                           RAIL_Events_t events);

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_UTIL_CALLBACKS_H
