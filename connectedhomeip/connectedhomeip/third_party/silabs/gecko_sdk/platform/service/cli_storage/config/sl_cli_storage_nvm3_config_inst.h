/***************************************************************************//**
 * @file
 * @brief Config file for the cli_storage_nvm3 component
 * @version x.x.x
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
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

#ifndef SL_CLI_STORAGE_NVM3_CONFIG_INSTANCE_H
#define SL_CLI_STORAGE_NVM3_CONFIG_INSTANCE_H

// <o SL_CLI_STORAGE_NVM3_INSTANCE_CLI_HANDLE> The cli handle used by the cli_storage_ram instance
// <i> Default: sl_cli_default_handle
#define SL_CLI_STORAGE_NVM3_INSTANCE_CLI_HANDLE     sl_cli_default_handle

// <s.20 SL_CLI_STORAGE_NVM3_INSTANCE_END_STRING> String to indicate end of CLI entry.
// <i> Default: "zzz"
#define SL_CLI_STORAGE_NVM3_INSTANCE_END_STRING     "zzz"

// <s.20 SL_CLI_STORAGE_NVM3_INSTANCE_PROMPT> Prompt during CLI entry.
// <i> Default: "nvm3|_storage>"
#define SL_CLI_STORAGE_NVM3_INSTANCE_PROMPT         "nvm3_storage>"

// <q SL_CLI_STORAGE_NVM3_INSTANCE_EXECUTE> Execute commands upon CLI entry.
// <i> Default: false
#define SL_CLI_STORAGE_NVM3_INSTANCE_EXECUTE        false

// <o SL_CLI_STORAGE_NVM3_INSTANCE_KEY_OFFSET> The NVM3 key offset for the instance <0-255>
// <i> Default: 0
// <i> Define the nvm3 key offset for the instance. The offset is relative to the defined storage area.
#define SL_CLI_STORAGE_NVM3_INSTANCE_KEY_OFFSET     (0)

// <o SL_CLI_STORAGE_NVM3_INSTANCE_KEY_COUNT> The number of nvm3 keys that shall be available for the instance. <1-256>
// <i> Default: SL_CLI_STORAGE_NVM3_KEY_COUNT
// <i> Define the number of nvm3 keys that are available for the instance.
#define SL_CLI_STORAGE_NVM3_INSTANCE_KEY_COUNT      (SL_CLI_STORAGE_NVM3_KEY_COUNT)

#endif // SL_CLI_STORAGE_NVM3_CONFIG_INSTANCE_H
