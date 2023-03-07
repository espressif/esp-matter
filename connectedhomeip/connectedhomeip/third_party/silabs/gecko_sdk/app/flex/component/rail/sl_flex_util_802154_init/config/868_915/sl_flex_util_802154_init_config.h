/***************************************************************************//**
 * @file
 * @brief sl_flex_802154_init_config.h
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

#ifndef SL_FLEX_UTIL_802154_INIT_CONFIG_H
#define SL_FLEX_UTIL_802154_INIT_CONFIG_H

#include "sl_flex_util_802154_protocol_types.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Available IEEE 802.15.4 standards
// <o SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT> Default Radio Configuration
// <SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ=> IEEE 802.15.4 GB868 915MHz
// <SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ=> IEEE 802.15.4 GB868 863MHz
// <i> Default: SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ
#define SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT  SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ
// </h>

// <<< end of configuration section >>>

#endif // SL_FLEX_UTIL_802154_INIT_CONFIG_H
