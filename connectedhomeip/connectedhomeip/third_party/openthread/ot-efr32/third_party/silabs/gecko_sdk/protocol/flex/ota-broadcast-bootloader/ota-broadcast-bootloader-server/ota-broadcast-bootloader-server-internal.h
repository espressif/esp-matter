/***************************************************************************//**
 * @brief Marcos, includes, and typedefs for ota-broadcast-bootloader-server.
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

#ifndef _OTA_BROADCAST_BOOTLOADER_SERVER_INTERNAL_H_
#define _OTA_BROADCAST_BOOTLOADER_SERVER_INTERNAL_H_

#include "ota-broadcast-bootloader/ota-broadcast-bootloader-protocol.h"
#include "ota-broadcast-bootloader/ota-broadcast-bootloader-common-internal.h"

//------------------------------------------------------------------------------
// Internal states

#define STATE_OTA_SERVER_IDLE                                               0x00
#define STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL                         0x01
#define STATE_OTA_SERVER_SEGMENT_BROADCAST_PENDING                          0x02
#define STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_INTERVAL                  0x03
#define STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_PENDING                   0x04
#define STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE                  0x05
#define STATE_OTA_SERVER_IMAGE_DISTRIBUTION_COMPLETED                       0x06
#define STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_INTERVAL             0x07
#define STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_PENDING              0x08
#define STATE_OTA_SERVER_TARGET_STATUS_REQUEST_WAITING_RESPONSE             0x09
#define STATE_OTA_SERVER_TARGET_STATUS_REQUEST_COMPLETED                    0x0A
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL                  0x0B
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING                   0x0C
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE                  0x0D
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_COMPLETED                         0x0E

#define serverIsIdle()  (internalState == STATE_OTA_SERVER_IDLE)

#define serverIsCompletingProcess() \
  (processCompleteState != STATE_OTA_SERVER_IDLE)

#define serverInImageDistributionProcess()                      \
  (internalState >= STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL \
   && internalState <= STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE)

#define serverCompletingDistributionProcess() \
  (processCompleteState == STATE_OTA_SERVER_IMAGE_DISTRIBUTION_COMPLETED)

#define serverInTargetStatusRequestProcess()                                \
  (internalState >= STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_INTERVAL \
   && internalState <= STATE_OTA_SERVER_TARGET_STATUS_REQUEST_WAITING_RESPONSE)

#define serverCompletingTargetStatusRequestProcess() \
  (processCompleteState == STATE_OTA_SERVER_TARGET_STATUS_REQUEST_COMPLETED)

#define serverInBootloadRequestProcess()                               \
  (internalState >= STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL \
   && internalState <= STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE)

#define serverCompletingBootloadRequestProcess() \
  (processCompleteState == STATE_OTA_SERVER_BOOTLOAD_REQUEST_COMPLETED)

//------------------------------------------------------------------------------
// Target list related defines

#define INVALID_TARGET_LIST_INDEX            0xFFFF

// We allocate 4 bytes for each target, 2 bytes for the short ID, 1 byte for
// storing the application target status and 1 byte for misc target information.
#define TARGET_LIST_ENTRY_LENGTH                                   4
#define TARGET_LIST_ENTRY_SHORT_ID_LENGTH                          2
#define TARGET_LIST_ENTRY_SHORT_ID_OFFSET                          0
#define TARGET_LIST_ENTRY_APP_STATUS_LENGHT                        1
#define TARGET_LIST_ENTRY_APP_STATUS_OFFSET                        2
#define TARGET_LIST_ENTRY_MISC_INFO_LENGHT                         1
#define TARGET_LIST_ENTRY_MISC_INFO_OFFSET                         3
#define TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_MASK                0x0F
#define TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_OFFSET              0
#define TARGET_LIST_ENTRY_MISC_INFO_UNREACHABLE_BIT                0x10

#define TARGET_LIST_ENTRY_MISC_INFO_INIT_VALUE                               \
  (EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING \
    << TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_OFFSET)

uint16_t emTargetLookup(EmberNodeId targetShortId);

uint8_t emGetOrSetTargetApplicationStatus(uint16_t targetIndex,
                                          bool isSet,
                                          uint8_t appStatus);

#define emGetTargetApplicationStatus(targetIndex) \
  (emGetOrSetTargetApplicationStatus((targetIndex), false, 0))

#define emSetTargetApplicationStatus(targetIndex, appStatus) \
  (emGetOrSetTargetApplicationStatus((targetIndex), true, (appStatus)))

uint8_t emGetOrSetTargetMiscInfo(uint16_t targetIndex,
                                 bool isSet,
                                 uint8_t info);

#define emGetTargetMiscInfo(targetIndex) \
  (emGetOrSetTargetMiscInfo((targetIndex), false, 0))

#define emSetTargetMiscInfo(targetIndex, info) \
  (emGetOrSetTargetMiscInfo((targetIndex), true, (info)))

#define emTargetIsUnreachable(targetIndex) \
  ((emGetTargetMiscInfo(targetIndex)       \
    & TARGET_LIST_ENTRY_MISC_INFO_UNREACHABLE_BIT))

#define emGetTargetOtaStatus(targetIndex)          \
  ((emGetTargetMiscInfo(targetIndex)               \
    & TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_MASK) \
   >> TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_OFFSET)

#define emSetTargetUnreachable(targetIndex)              \
  (emSetTargetMiscInfo(targetIndex,                      \
                       (emGetTargetMiscInfo(targetIndex) \
                        | TARGET_LIST_ENTRY_MISC_INFO_UNREACHABLE_BIT)))

#define emSetTargetOtaStatus(targetIndex, otaStatus)                   \
  do {                                                                 \
    uint8_t info = emGetTargetMiscInfo(targetIndex);                   \
    info &= ~TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_MASK;              \
    info |= ((otaStatus & TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_MASK) \
             << TARGET_LIST_ENTRY_MISC_INFO_OTA_STATUS_OFFSET);        \
    emSetTargetMiscInfo(targetIndex, info);                            \
  } while (0)

#endif // _OTA_BROADCAST_BOOTLOADER_SERVER_INTERNAL_H_
