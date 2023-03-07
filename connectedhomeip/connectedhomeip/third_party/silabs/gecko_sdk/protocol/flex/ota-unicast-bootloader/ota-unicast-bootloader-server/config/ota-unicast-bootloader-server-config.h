/***************************************************************************//**
 * @brief Connect OTA Unicast Bootloader Server component configuration header.
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

// <h>Connect OTA Unicast Bootloader Server configuration

// <o EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT> OTA Unicast Endpoint <0-15>
// <i> Default: 13
// <i> The endpoint used by the ota unicast bootloader server/client nodes to exchange ota unicast bootloader-related messages.
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT              (13)

// <q EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_SECURITY_ENABLED> Use security
// <i> Default: 1
// <i> If this option is enabled, the client will only accept encrypted image segments and other commands. It will also encrypt all the commands sent to the server.
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_SECURITY_ENABLED       (1)

// <o EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS> Transmission interval <25-1000>
// <i> Default: 100
// <i> The ota unicast bootloader server tranmission interval in milliseconds.
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS       (100)

// </h>

// <<< end of configuration section >>>
