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

#ifndef SL_RAIL_UTIL_PROTOCOL_TYPES_H
#define SL_RAIL_UTIL_PROTOCOL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sl_rail_util_protocol_type{
  SL_RAIL_UTIL_PROTOCOL_PROPRIETARY, /* use rail_config.c/h */
  SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ,
  SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_ANZ,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_CN,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_EU,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_HK,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_IN,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_IL,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_JP,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_KR,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_MY,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_RU,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_US,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR1,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR2,
  SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR_END_DEVICE,
} sl_rail_util_protocol_type_t;

#define SL_RAIL_UTIL_PROTOCOL_IS_BLE(x)              \
  ((x == SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS)            \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS)         \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS) \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS))

#define SL_RAIL_UTIL_PROTOCOL_IS_IEEE802154_2G4(x)          \
  ((x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ)           \
   || (x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV) \
   || (x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX)   \
   || (x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX))

#define SL_RAIL_UTIL_PROTOCOL_IS_IEEE802154_GB868(x)    \
  ((x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ) \
   || (x == SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ))

#define SL_RAIL_UTIL_PROTOCOL_IS_ZWAVE(x)       \
  ((x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_ANZ)       \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_CN)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_EU)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_HK)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_IN)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_IL)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_JP)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_KR)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_MY)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_RU)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_US)     \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR1) \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR2) \
   || (x == SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR_END_DEVICE))

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_UTIL_PROTOCOL_TYPES_H
