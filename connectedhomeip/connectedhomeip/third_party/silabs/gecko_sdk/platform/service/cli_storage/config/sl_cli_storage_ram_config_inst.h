/***************************************************************************//**
 * @file
 * @brief Config file for the cli_storage_ram component
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_CLI_STORAGE_RAM_CONFIG_INSTANCE_H
#define SL_CLI_STORAGE_RAM_CONFIG_INSTANCE_H

// <o SL_CLI_STORAGE_RAM_INSTANCE_CLI_HANDLE> The cli handle used by the cli_storage_ram instance
// <i> Default: sl_cli_default_handle
#define SL_CLI_STORAGE_RAM_INSTANCE_CLI_HANDLE     sl_cli_default_handle

// <s.20 SL_CLI_STORAGE_RAM_INSTANCE_END_STRING> String to indicate end of CLI entry.
// <i> Default: "zzz"
#define SL_CLI_STORAGE_RAM_INSTANCE_END_STRING     "zzz"

// <s.20 SL_CLI_STORAGE_RAM_INSTANCE_PROMPT> Prompt during CLI entry.
// <i> Default: "ram_storage>"
#define SL_CLI_STORAGE_RAM_INSTANCE_PROMPT         "ram_storage>"

// <q SL_CLI_STORAGE_RAM_INSTANCE_EXECUTE> Execute commands upon CLI entry.
// <i> Default: false
#define SL_CLI_STORAGE_RAM_INSTANCE_EXECUTE        false

// <o SL_CLI_STORAGE_RAM_INSTANCE_LINES> The number of command lines that can be stored in RAM. <1-100>
// <i> Default: 10
// <i> Define the size of the RAM buffer in number of lines it can store.
#define SL_CLI_STORAGE_RAM_INSTANCE_LINES          (10)

#endif // SL_CLI_STORAGE_RAM_CONFIG_INSTANCE_H
