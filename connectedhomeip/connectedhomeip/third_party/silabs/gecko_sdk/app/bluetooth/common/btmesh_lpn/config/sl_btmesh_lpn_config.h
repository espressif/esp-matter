/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_BTMESH_LPN_CONFIG_H
#define SL_BTMESH_LPN_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> LPN configuration

// <o SL_BTMESH_LPN_MIN_QUEUE_LENGTH_CFG_VAL> Minimum queue length the friend must support <2-128>
// <i> Default: 2
// <i> Minimum queue length the friend must support. Choose an appropriate based on the expected message
// frequency and LPN sleep period, because messages that do not fit into the friend queue are dropped.
// Note that the given value is rounded up to the nearest power of 2
#define SL_BTMESH_LPN_MIN_QUEUE_LENGTH_CFG_VAL   (2)

// <o SL_BTMESH_LPN_POLL_TIMEOUT_CFG_VAL> Poll timeout in milliseconds <1000-345599900:100>
// <i> Default: 5000
// <i> Poll timeout in milliseconds, which is the longest time that LPN sleeps in between querying its friend
// for queued messages. Long poll timeout allows the LPN to sleep for longer periods, at the expense of increased
// latency for receiving messages. Note that the given value is rounded up to the nearest 100 ms
#define SL_BTMESH_LPN_POLL_TIMEOUT_CFG_VAL   (5000)

// <o SL_BTMESH_LPN_RECEIVE_DELAY_CFG_VAL> Receive delay in milliseconds <10-255>
// <i> Default: 50
// <i> Receive delay in milliseconds. Receive delay is the time between the LPN sending a request and listening
// for a response. Receive delay allows the friend node time to prepare the message and LPN to sleep
#define SL_BTMESH_LPN_RECEIVE_DELAY_CFG_VAL   (50)

// <o SL_BTMESH_LPN_REQUEST_RETRIES_CFG_VAL> The number of retry attempts to repeat <0-10>
// <i> Default: 8
// <i> Request retry is the number of retry attempts to repeat e.g., the friend poll message
// if the friend update was not received by the LPN
#define SL_BTMESH_LPN_REQUEST_RETRIES_CFG_VAL   (8)

// <o SL_BTMESH_LPN_RETRY_INTERVAL_CFG_VAL> Time interval between retry attempts in milliseconds <0-100>
// <i> Default: 100
// <i> Time interval between retry attempts in milliseconds
#define SL_BTMESH_LPN_RETRY_INTERVAL_CFG_VAL   (100)

// <h> Initialization timeouts

// <o SL_BTMESH_LPN_TIMEOUT_AFTER_PROVISIONED_CFG_VAL> Timeout for initializing LPN after an already provisioned Node is initialized
// <i> Default: 30000
// <i> Timeout for initializing LPN after an already provisioned Node is initialized. It can delay friend
// establishment to wait for possible Configuration Messages
#define SL_BTMESH_LPN_TIMEOUT_AFTER_PROVISIONED_CFG_VAL   (30000)

// <o SL_BTMESH_LPN_TIMEOUT_AFTER_KEY_CFG_VAL> Timeout for initializing LPN after Security Key was added
// <i> Default: 5000
// <i> Timeout for initializing LPN after Security Key was added. It can delay friend establishment
//  to wait for possible other Configuration Messages
#define SL_BTMESH_LPN_TIMEOUT_AFTER_KEY_CFG_VAL   (5000)

// <o SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_MODEL_CHANGED_CFG_VAL> Timeout for initializing LPN after the Configuration Model changed
// <i> Default: 5000
// <i> Timeout for initializing LPN after the Configuration Model changed. It can delay friend establishment
//  to wait for possible other Configuration Messages
#define SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_MODEL_CHANGED_CFG_VAL   (5000)

// <o SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_SET_CFG_VAL> Timeout for initializing LPN after the Configuration Model Set Message
// <i> Default: 5000
// <i> Timeout for initializing LPN after the Configuration Model Set Message. It can delay friend establishment
//  to wait for possible other Configuration Messages
#define SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_SET_CFG_VAL   (5000)

// </h>

// <o SL_BTMESH_LPN_FRIEND_FIND_TIMEOUT_CFG_VAL> Timeout between retries to find a friend
// <i> Default: 2000
// <i> Timeout between retries to find a friend
#define SL_BTMESH_LPN_FRIEND_FIND_TIMEOUT_CFG_VAL   (2000)

// <e SL_BTMESH_LPN_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable or disable Logging for LPN specific messages for this component.
#define SL_BTMESH_LPN_LOGGING_CFG_VAL   (1)

// <s.32 SL_BTMESH_LPN_FRIEND_NOT_FOUND_LOG_TEXT_CFG_VAL> Log text when no friend was found.
// <i> Log text when no friend was found
#define SL_BTMESH_LPN_FRIEND_NOT_FOUND_LOG_TEXT_CFG_VAL "Friend not found - Ret.code 0x%lx\r\n"

// <s.32 SL_BTMESH_LPN_START_INIT_LOG_TEXT_CFG_VAL> Log text when LPN initialization starts.
// <i> Log text when LPN initialization starts
#define SL_BTMESH_LPN_START_INIT_LOG_TEXT_CFG_VAL "Trying to initialize lpn...\r\n"

// <s.32 SL_BTMESH_LPN_START_FRIEND_SEARCH_LOG_TEXT_CFG_VAL> Log text when friend establishment is attempted.
// <i> Log text when friend establishment is attempted
#define SL_BTMESH_LPN_START_FRIEND_SEARCH_LOG_TEXT_CFG_VAL "Trying to find a friend...\r\n"

// </e>

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_LPN_CONFIG_H
