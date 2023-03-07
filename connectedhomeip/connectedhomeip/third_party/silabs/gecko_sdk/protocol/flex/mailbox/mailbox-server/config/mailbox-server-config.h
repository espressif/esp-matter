/***************************************************************************//**
 * @brief Connect Mailbox Server component configuration header.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect Mailbox Server configuration

// <o EMBER_AF_PLUGIN_MAILBOX_SERVER_MAILBOX_ENDPOINT> The mailbox protocol endpoint <0-15>
// <i> Default: 15
// <i> The endpoint used by the mailbox server/client nodes to exchange mailbox-related messages.
#define EMBER_AF_PLUGIN_MAILBOX_SERVER_MAILBOX_ENDPOINT         (15)

// <o EMBER_AF_PLUGIN_MAILBOX_SERVER_PACKET_TABLE_SIZE> Maximum number of packets <1-254>
// <i> Default: 25
// <i> The maximum number of packets that can be stored at the mailbox server.
#define EMBER_AF_PLUGIN_MAILBOX_SERVER_PACKET_TABLE_SIZE        (25)

// <o EMBER_AF_PLUGIN_MAILBOX_SERVER_PACKET_TIMEOUT_S> Packet timeout in seconds
// <i> Default: 3600
// <i> The time in seconds after which a packet is dropped if not retrieved by its destination.
#define EMBER_AF_PLUGIN_MAILBOX_SERVER_PACKET_TIMEOUT_S         (3600)

// </h>

// <<< end of configuration section >>>
