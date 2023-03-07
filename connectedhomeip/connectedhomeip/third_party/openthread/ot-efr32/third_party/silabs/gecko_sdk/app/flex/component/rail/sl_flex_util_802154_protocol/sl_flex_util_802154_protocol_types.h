/***************************************************************************//**
 * @file
 * @brief sl_flex_util_802154_protocol_types.h
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

#ifndef SL_FLEX_UTIL_802154_PROTOCOL_TYPES_H
#define SL_FLEX_UTIL_802154_PROTOCOL_TYPES_H

/// Enum for all IEEE 802.15.4 protocols
typedef enum sl_flex_util_802154_protocol_type{
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ,
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV,
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX,
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX,
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ,
  SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ,
} sl_flex_util_802154_protocol_type_t;

#define SL_FLEX_UTIL_PROTOCOL_IS_IEEE802154_2G4(x)          \
  ((x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ)           \
   || (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV) \
   || (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX)   \
   || (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX))

#define SL_FLEX_UTIL_PROTOCOL_IS_IEEE802154_GB868(x)   \
  (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ) \
  || (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ)

#define SL_FLEX_UTIL_PROTOCOL_IS_IEEE802154_GB868_863MHZ(x) \
  (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ)

#define SL_FLEX_UTIL_PROTOCOL_IS_IEEE802154_GB868_915MHZ(x) \
  (x == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ)

#endif // SL_FLEX_UTIL_802154_PROTOCOL_TYPES_H
