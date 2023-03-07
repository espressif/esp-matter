/***************************************************************************//**
 * @brief Macros defined for mailbox protocols
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

#ifndef _MAILBOX_PROTOCOL_H_
#define _MAILBOX_PROTOCOL_H_

#define EMBER_MAILBOX_PROTOCOL_MAX_HEADER_LENGTH                            5

#define EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_LENGTH                         1
#define EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_OFFSET                         0
#define EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_COMMAND_ID_MASK                0x0F
#define EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET              0
#define EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_MESSAGE_PENDING_BIT            0x10

// Command IDs
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_ADD_MESSAGE                       0x00
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_ADD_MESSAGE_RESPONSE              0x01
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_GET_MESSAGE                       0x02
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_NULL_MESSAGE                      0x03
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_DATA_MESSAGE                      0x04
#define EMBER_MAILBOX_PROTOCOL_COMMAND_ID_MESSAGE_STATUS                    0x05

// add message ->  frame control (1) + tag(1) + destination (2) + payload (n)
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_HEADER_LENGTH                    4
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_TAG_LENGTH                       1
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_TAG_OFFSET                       1
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_DESTINATION_LENGTH               2
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_DESTINATION_OFFSET               2
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_PAYLOAD_OFFSET                   4

// add message response -> frame control (1) + tag (1) + destination (2)
//                         + status (1)
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_HEADER_LENGTH           5
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_TAG_LENGTH              1
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_TAG_OFFSET              1
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_DESTINATION_LENGTH      2
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_DESTINATION_OFFSET      2
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_STATUS_LENGTH           1
#define EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_STATUS_OFFSET           4

// get message -> frame control (1)
#define EMBER_MAILBOX_PROTOCOL_GET_MESSAGE_HEADER_LENGTH                    1

// null message -> frame control (1)
#define EMBER_MAILBOX_PROTOCOL_NULL_MESSAGE_HEADER_LENGTH                   1

// data message -> frame control (1) + tag (1) + source (2) + payload (n)
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_HEADER_LENGTH                   4
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_TAG_LENGTH                      1
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_TAG_OFFSET                      1
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_SOURCE_LENGTH                   2
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_SOURCE_OFFSET                   2
#define EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_PAYLOAD_OFFSET                  4

// message status -> frame control (1) + tag (1) + destination (2)
//                      + result (1)
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_HEADER_LENGTH                 5
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_TAG_LENGTH                    1
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_TAG_OFFSET                    1
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_DESTINATION_LENGTH            2
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_DESTINATION_OFFSET            2
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_RESULT_LENGTH                 1
#define EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_RESULT_OFFSET                 4

//------------------------------------------------------------------------------
// Utility macros

#define mailboxProtocolCommandId(message)                   \
  ((message[EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_OFFSET]    \
    & EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_COMMAND_ID_MASK) \
   >> EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)

#define mailboxProtocolMessagePendingFlag(message)       \
  ((message[EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_OFFSET] \
    & EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_MESSAGE_PENDING_BIT) > 0)

#endif //_MAILBOX_PROTOCOL_H_
