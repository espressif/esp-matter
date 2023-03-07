/***************************************************************************//**
 * @brief Marcos, includes, and typedefs for ota-broadcast-bootloader-client.
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

#ifndef _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_
#define _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_

#include "ota-broadcast-bootloader/ota-broadcast-bootloader-protocol.h"
#include "ota-broadcast-bootloader/ota-broadcast-bootloader-common-internal.h"

//------------------------------------------------------------------------------
// Internal states

#define STATE_OTA_CLIENT_IDLE                                               0x00
#define STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING                             0x01

#define clientIsIdle()  (internalState == STATE_OTA_CLIENT_IDLE)
#define clientInImageDownload() \
  (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING)

//------------------------------------------------------------------------------
// Download history types and definitions.

#define HISTORY_TABLE_SIZE        5

typedef struct {
  uint8_t imageTag;
  uint8_t otaProtocolStatus;
  uint8_t applicationStatus;
  EmberNodeId serverId;
  EmberNodeId alternateServerId;
} EmHistoryEntry;

//------------------------------------------------------------------------------
// Test-related macros

#if defined(EMBER_SCRIPTED_TEST)
#include "core/ember-stack.h"
#include "core/parcel.h"
#include "scripted_test_framework.h"
#define scriptCheckHistoryTableAdd(index) \
  postTestCheck("history table add", "history table add", "u", (index))
#define scriptCheckHistoryTableRemove(index, oldestIndex) \
  postTestCheck("history table remove", "history table remove", "uu", (index), (oldestIndex))
#else
#define scriptCheckHistoryTableAdd(index)
#define scriptCheckHistoryTableRemove(index, oldestIndex)
#endif

#endif // _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_
