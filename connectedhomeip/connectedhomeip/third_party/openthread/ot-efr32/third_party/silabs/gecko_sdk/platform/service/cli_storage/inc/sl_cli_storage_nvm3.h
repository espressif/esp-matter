/***************************************************************************//**
 * @file sl_cli_storage_nvm3.h
 * @brief Implement storing of CLI command lines in NVM3.
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

#ifndef SL_CLI_STORAGE_NVM3_H
#define SL_CLI_STORAGE_NVM3_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "nvm3.h"
#include "sl_cli_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup cli_storage_nvm3 CLI Commands in NVM3
 * @brief Support for CLI commands stored in and executed from NVM3
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/// Defined in the "NVM3 Default Instance Key Space Regions"
/// Note: These definitions cannot be changed!
#define SL_CLI_STORAGE_NVM3_KEY_COUNT    SL_CLI_NVM3_KEY_COUNT
#define SL_CLI_STORAGE_NVM3_KEY_BEGIN    SL_CLI_NVM3_KEY_BEGIN
#define SL_CLI_STORAGE_NVM3_KEY_END      (SL_CLI_STORAGE_NVM3_KEY_BEGIN + SL_CLI_STORAGE_NVM3_KEY_COUNT)
/// @endcond

/// @brief Struct representing an instance of the CLI storage NVM3.
typedef struct {
  sl_cli_handle_t cli_handle;   ///< The CLI instance associated with this instance.
  char *prompt;                 ///< The command prompt.
  char *end_string;             ///< The string that define the end of definition.
  size_t key_offset;            ///< The NVM3 key offset for this instance.
  size_t key_count;             ///< The NVM3 key count for this instance.
  size_t key_next;              ///< The next key value that will be used.
  bool execute_while_define;    ///< True if the command shell execute when defined.
  bool define_in_progress;      ///< True when definition is in progress.
  bool execute_in_progress;     ///< True when execute is in progress.
  bool init_tick;               ///< True after the initial (first) tick has been executed.
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  size_t exec_offset;           ///< The script execution offset.
#endif
} cli_storage_nvm3_t;

/// @brief Definition of the CLI storage NVM3 handle.
typedef cli_storage_nvm3_t *cli_storage_nvm3_handle_t;

/***************************************************************************//**
 * @brief
 *  Clear the command lines stored in nvm3.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_nvm3_clear(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Print the contet of the command lines stored in nvm3.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_nvm3_list(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Start defining new command lines that will be stored in nvm3.
 *  This function can be used directly as a cli command handler.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_nvm3_define(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Execute command lines stored in nvm3.
 *  This function can be used directly as a cli command handler.
 *
 * @note
 *  The function has a different implementation in a bare-metal vs. kernel
 *  configurations.
 *  In a bare-metal configuration, this function will signal that stored commands
 *  shall be run by the sl_cli_storage_nvm3_tick function. One stored command is
 *  executed for each call to the sl_cli_storage_nvm3_tick function.
 *  In a kernel configuration, all stored commands will be executed before this
 *  function returns.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_storage_nvm3_execute(sl_cli_command_arg_t *arguments);

/***************************************************************************//**
 * @brief
 *  Get the number of command lines stored in nvm3.
 *  Note: The number of limits may be limited by the instance configuration.
 *
 * @param[in] cli_handle
 *   Handle to the CLI where the cli_storage_ram component is connected.
 *
 * @return
 *   The number of command lines stored in nvm3.
 ******************************************************************************/
size_t sl_cli_storage_nvm3_count(sl_cli_handle_t cli_handle);

/***************************************************************************//**
 * @brief
 *  Initialize the cli_storage_nvm3 instance.
 *
 * @param[in] handle
 *   A handle to the cli_storage_nvm3 instance.
 ******************************************************************************/
void sl_cli_storage_nvm3_init(cli_storage_nvm3_handle_t handle);

/***************************************************************************//**
 * @brief
 *  Execute the command lines stored in nvm3 at program startup and complete
 *  execution of commands started with a call to sl_cli_storage_nvm3_execute
 *  when running in bare-metal configuation.
 *
 * @param[in] cli_handle
 *   A handle to the cli instance.
 *
 * @return
 *   A boolean that is true if there are more stored commands to execute, else
 *   false.
 ******************************************************************************/
bool sl_cli_storage_nvm3_tick(sl_cli_handle_t cli_handle);

/** @} (end addtogroup cli_storage_nvm3) */
/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_STORAGE_NVM3_H
