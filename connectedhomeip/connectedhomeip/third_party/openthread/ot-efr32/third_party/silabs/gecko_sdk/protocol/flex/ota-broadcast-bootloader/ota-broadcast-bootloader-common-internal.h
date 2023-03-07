/***************************************************************************//**
 * @brief Set of macros for ota-broadcast-bootloader (server and client)
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

#ifndef _OTA_BROADCAST_BOOTLOADER_COMMON_INTERNAL_H_
#define _OTA_BROADCAST_BOOTLOADER_COMMON_INTERNAL_H_

#if ((EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_SECURITY_ENABLED > 0) \
  || (EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_SECURITY_ENABLED > 0))
#define SECURITY_ENABLED
#endif

//------------------------------------------------------------------------------
// Image related defines

#if defined(SECURITY_ENABLED)
// Segment size: depends on whether we use security or not (plugin option).
#define MAX_APPLICATION_PAYLOAD_LENGTH \
  EMBER_MAX_SECURED_APPLICATION_PAYLOAD_LENGTH
#define BROADCAST_TX_OPTIONS  (EMBER_OPTIONS_SECURITY_ENABLED)
#define UNICAST_TX_OPTIONS  (EMBER_OPTIONS_ACK_REQUESTED \
                             | EMBER_OPTIONS_SECURITY_ENABLED)
#else
#define MAX_APPLICATION_PAYLOAD_LENGTH \
  EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH
#define BROADCAST_TX_OPTIONS  (0)
#define UNICAST_TX_OPTIONS  (EMBER_OPTIONS_ACK_REQUESTED)
#endif // SECURITY_ENABLED

// Segment size is always set to be a multiple of 4 to facilitate flash writes
// to parts that require 4-bytes alignment.
#define MAX_SEGMENT_PAYLOAD_LENGTH                                         \
  ((MAX_APPLICATION_PAYLOAD_LENGTH                                         \
    - EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH) \
   & 0xFC)

// We define here the maximum image size we support (we use 2 bytes for the
// segment index leaving the index 0xFFFF reserved, which allows us to support
// an image of up to (65534 * ((111-4) & 0xFC)) = 65534 * 104 = 6815536 bytes
// in case we use unsecured messages, an image of up to
// (65534 * ((102-4) & 0xFC) = 65534 * 96 = 6291264 bytes in case we use
// security).
#define MAXIMUM_IMAGE_SIZE (0xFFFE * MAX_SEGMENT_PAYLOAD_LENGTH)

// We break down the broadcast process in blocks, so that when we query the
// targets for missing block, they can provide a bitmask that fits in a single
// message (512 segments in a block requires a 64 bytes bitmask, which fits
// comfortably in a single message, with or without security).
// This should always be a multiple of 8.
#define MAX_SEGMENTS_IN_A_BLOCK           512

// (Max) length of a missing segments bitmask in bytes.
#define MISSING_SEGMENTS_BITMASK_LENGTH   (MAX_SEGMENTS_IN_A_BLOCK / 8)

#define INVALID_SEGMENT_INDEX             0xFFFF

#endif // _OTA_BROADCAST_BOOTLOADER_COMMON_INTERNAL_H_
