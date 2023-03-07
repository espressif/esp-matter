/***************************************************************************//**
 * @brief Connect OTA Unicast Bootloader Client component configuration header.
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

// <h>Connect OTA Unicast Bootloader Client configuration

// <o EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT> OTA Unicast Endpoint <0-15>
// <i> Default: 13
// <i> The endpoint used by the ota unicast bootloader server/client nodes to exchange ota unicast bootloader-related messages.
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT                  (13)

// <q EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_SECURITY_ENABLED> Use security
// <i> Default: 1
// <i> If this option is enabled, the client will only accept encrypted image segments and other commands. It will also encrypt all the commands sent to the server.
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_SECURITY_ENABLED          (1)

// <o EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_IMAGE_DOWNLOAD_TIMEOUT_S> Download Timeout <1-10>
// <i> Default: 5
// <i> The time in seconds after which the client shall fail an ongoing image download process in case no message is received from the server(s).
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_IMAGE_DOWNLOAD_TIMEOUT_S  (5)

// </h>

// <<< end of configuration section >>>
