/***************************************************************************//**
 * @file sl_cli_storage_ram.h
 * @brief Implement storing of CLI command lines in RAM.
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

#ifndef SL_CLI_STORAGE_RAM_H
#define SL_CLI_STORAGE_RAM_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_cli_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup cli_storage_ram CLI Commands in RAM
 * @brief Support for CLI commands stored in and executed from RAM
 * @{
 ******************************************************************************/

/// @brief Struct representing an instance of the CLI storage RAM.
typedef struct {
  sl_cli_handle_t cli_handle;   ///< The CLI instance associated with this instance.
  char *prompt;                 ///< The command prompt.
  char *end_string;             ///< The string that define the end of definition.
  size_t ram_size;              ///< The number of command lines that can be stored in RAM.
  size_t ram_next;              ///< The next free line in RAM.
  char   *ram_buffer;           ///< Pointer to the RAM buffer.
  bool execute_while_define;    ///< True if the command shell execute when defined.
  bool define_in_progress;      ///< True when definition is in progress.
  bool execute_in_progress;     ///< True when execute is in progress.
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  size_t exec_offset;           ///< The script execution offset.
#endif
} cli_storage_ram_t;

/// @brief Definition of the CLI storage RAM handle.
typedef cli_storage_ram_t *cli_storage_ram_handle_t;

/***************************************************************************//**
 * @brief
 *  Clear the command lines stored in ram.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_ram_clear(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Print the contet of the command lines stored in ram.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_ram_list(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Start defining new command lines that will be stored in ram.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_ram_define(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Execute command lines stored in ram.
 *  This function can be used directly as a cli command handler.
 *
 * @note
 *  The function has a different implementation in a bare-metal vs. kernel
 *  configurations.
 *  In a bare-metal configuration, this function will signal that stored commands
 *  shall be run by the sl_cli_storage_ram_tick function. One stored command is
 *  executed for each call to the sl_cli_storage_ram_tick function.
 *  In a kernel configuration, all stored commands will be executed before this
 *  function returns.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_ram_execute(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Get the number of command lines stored in ram.
 *
 * @param[in] cli_handle
 *   Handle to the CLI where the cli_storage_ram component is connected.
 *
 * @return
 *   The number of command lines stored in ram.
 ******************************************************************************/
size_t sl_cli_storage_ram_count(sl_cli_handle_t cli_handle);

/***************************************************************************//**
 * @brief
 *  Initialize the cli_storage_ram instance.
 *
 * @param[in] handle
 *   A handle to the cli_storage_ram instance.
 ******************************************************************************/
void sl_cli_storage_ram_init(cli_storage_ram_handle_t handle);

#if !defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * @brief
 *  Complete execution of commands after a call to sl_cli_storage_ram_execute
 *  when running in bare-metal configuation.
 *
 * @param[in] cli_handle
 *   A handle to the cli instance.
 *
 * @return
 *   A boolean that is true if there are more stored commands to execute, else
 *   false.
 ******************************************************************************/
bool sl_cli_storage_ram_tick(sl_cli_handle_t cli_handle);
#endif

/** @} (end addtogroup cli_storage_ram) */
/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_STORAGE_RAM_H
