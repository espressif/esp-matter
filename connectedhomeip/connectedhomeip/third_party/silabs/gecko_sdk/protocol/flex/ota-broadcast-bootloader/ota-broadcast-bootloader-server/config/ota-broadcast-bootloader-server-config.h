/***************************************************************************//**
 * @brief Connect OTA Broadcast Bootloader Server component configuration header
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

// <h>Connect OTA Broadcast Bootloader Server configuration

// <o EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT> OTA Broadcast Endpoint <0-15>
// <i> Default: 14
// <i> The endpoint used by the ota broadcast bootloader server/client nodes to exchange ota broadcast bootloader-related messages.
#define EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT            (14)

// <q EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_SECURITY_ENABLED> Use security
// <i> Default: 1
// <i> If this option is enabled, the server will encrypt image segments and other related commands. It will also drop all non-encrypted client responses.
#define EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_SECURITY_ENABLED     (1)

// <o EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS> Transmission interval in milliseconds<25-1000>
// <i> Default: 100
// <i> The ota broadcast bootloader server tranmission interval in milliseconds.
#define EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS     (100)

// </h>

// <<< end of configuration section >>>
