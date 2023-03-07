/***************************************************************************//**
 * @brief Macro defined for ota-unicast-bootloader protocols.
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

#ifndef _OTA_UNICAST_BOOTLOADER_PROTOCOL_H_
#define _OTA_UNICAST_BOOTLOADER_PROTOCOL_H_

#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_MAX_HEADER_LENGTH                     7

// Frame control
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_LENGTH                  1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET                  0
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_MASK         0x0F
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET       0
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_MASK          0x10
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_OFFSET        4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_RESERVED_MASK       0xE0
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_MASK        0x70
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET      4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_RESERVED_MASK      0x80

// Request flags
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAG_RESUME_SUPPORT 0x01

// Generic response status values
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED   0x00
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING     0x01
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED      0x02
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED     0x03
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ABORTED     0x04
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OUT_OF_SEQ  0x05

// Command IDs
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT              0x00
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT_RESPONSE     0x01
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST           0x02
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_RESPONSE          0x03
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE                  0x04
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE_RESPONSE         0x05
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_EXT_HANDSHAKE_RESPONSE     0x06

// Handshake -> frame control (1) + image tag (1) + image size (4)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_HEADER_LENGTH               6
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_TAG_LENGTH                  1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_TAG_OFFSET                  1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_LENGTH_LENGTH               4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_LENGTH_OFFSET               2

// Handshake response -> frame control (1) + image tag (1) + application status (1)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_HEADER_LENGTH          3
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_TAG_LENGTH             1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_TAG_OFFSET             1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_STATUS_LENGTH          1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_STATUS_OFFSET          2

// Extended handshake response -> frame control (1) + image tag (1)
//                                + segment index (4)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_HEADER_LENGTH         6
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_TAG_LENGTH            1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_TAG_OFFSET            1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_SEGMENT_INDEX_LENGTH  4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_SEGMENT_INDEX_OFFSET  2

// image segment -> frame control (1) + image tag (1) + segment index (4)
//                  + payload (n)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH           6
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_LENGTH              1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_OFFSET              1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_LENGTH            4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_OFFSET            2
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_PAYLOAD_OFFSET          6

// image segment response -> frame control (1) + image tag (1) + segment index (4)
//                  + application status (1)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_HEADER_LENGTH      7
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_TAG_LENGTH         1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_TAG_OFFSET         1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_INDEX_LENGTH       4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_INDEX_OFFSET       2
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_STATUS_LENGTH      1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_STATUS_OFFSET      6

// bootload request -> frame control (1) + image tag (1) + delay (4)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH            6
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_LENGTH               1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_OFFSET               1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_LENGTH             4
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_OFFSET             2

// bootload response -> frame control (1) + image tag (1)
//                      + application status (1)
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH           3
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_TAG_LENGTH              1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_TAG_OFFSET              1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_APP_STATUS_LENGTH       1
#define EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_APP_STATUS_OFFSET       2

#define emOtaUnicastBootloaderProtocolCommandId(message)                   \
  ((message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET]    \
    & EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_MASK) \
   >> EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)

#define emOtaUnicastBootloaderProtocolResponseStatus(message)               \
  ((message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET]     \
    & EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_MASK) \
   >> EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET)

#define emOtaUnicastBootloaderProtocolRequestFlags(message)               \
  ((message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET]   \
    & EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_MASK) \
   >> EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_OFFSET)

#define emOtaUnicastBootloaderProtocolResumeDownloadSupported(message) \
  ((emOtaUnicastBootloaderProtocolRequestFlags(message)                \
    & EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAG_RESUME_SUPPORT) > 0)

#endif // _OTA_UNICAST_BOOTLOADER_PROTOCOL_H_
