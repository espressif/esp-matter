/***************************************************************************//**
 * @file
 * @brief The command interpreter has functions to interpret a command string
 *   and execute the correct command function with decoded parameters.
 * @version x.y.z
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

#ifndef SL_CLI_COMMAND_H
#define SL_CLI_COMMAND_H

#include <stdint.h>
#include <stdbool.h>

#include "sl_cli_config.h"
#include "sl_cli_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

#if SL_CLI_HELP_DESCRIPTION_ENABLED
// Macros requiring help text

/***************************************************************************//**
 *  @brief A macro, which is used to create commands.
 ******************************************************************************/
  #define SL_CLI_COMMAND(function, help_text, arg_help, ...) \
  {                                                          \
    (function),     /* Pointer to command function */        \
    (help_text),    /* Help text */                          \
    (arg_help),     /* Argument help text */                 \
    __VA_ARGS__     /* Argument list*/                       \
  }

/***************************************************************************//**
 *  @brief A macro, which is used to create command groups.
 ******************************************************************************/
  #define SL_CLI_COMMAND_GROUP(group_table, help_text)                       \
  {                                                                          \
    ((sl_cli_command_func_t)(group_table)),   /* Group pointer */            \
    (help_text),                              /* Help text */                \
    (""),                                     /* Empty argument help text */ \
    { SL_CLI_ARG_GROUP, }                     /* Group indicator */          \
  }
#else
// Macros, which do not allow help text.

/***************************************************************************//**
 *  @brief A macro, which is used to create commands.
 ******************************************************************************/
  #define SL_CLI_COMMAND(function, help_text, arg_help, ...) \
  {                                                          \
    (function),    /* Pointer to command function */         \
    __VA_ARGS__    /* Argument list*/                        \
  }

/***************************************************************************//**
 *  @brief A macro, which is used to create command groups.
 ******************************************************************************/
  #define SL_CLI_COMMAND_GROUP(group_table, help_text)              \
  {                                                                 \
    ((sl_cli_command_func_t)(group_table)),   /* Group pointer */   \
    { SL_CLI_ARG_GROUP, }                     /* Group indicator */ \
  }
#endif // SL_CLI_HELP_DESCRIPTION_ENABLED

/***************************************************************************//**
 * @brief
 *   Add a new command group.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in] command_group
 *   A pointer to a command group structure.
 *   Note that the structure must initially have NULL in all elements except
 *   the command_table.
 *
 * @return
 *   Returns true if the command_group could be added, false otherwise.
 ******************************************************************************/
bool sl_cli_command_add_command_group(sl_cli_handle_t handle, sl_cli_command_group_t *command_group);

/***************************************************************************//**
 * @brief
 *   Remove a command group.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in] command_group
 *   A pointer to a command group structure.
 *
 * @return
 *   Returns true if the command_group could be removed, false otherwise.
 ******************************************************************************/
bool sl_cli_command_remove_command_group(sl_cli_handle_t handle, sl_cli_command_group_t *command_group);

/***************************************************************************//**
 * @brief
 *   Find a command given the command groups and input arguments.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in] token_c
 *   A number of arguments given.
 *
 * @param[in] token_v
 *   An array containing the token_v obtained from tokenization.
 *
 * @param[out] arg_ofs
 *   An integer that will get the index to the first command argument.
 *   Whether the command is in a group or not will affect the arg_ofs value.
 *
 * @param[out] single_flag
 *   Boolean that is set to true if a specific command is detected.
 *   For help, the function may return a pointer to a command entry table
 *   or NULL, and in these cases the single_flag is set to false.
 *
 * @param[out] help_flag
 *   Boolean that is set to true if "help" is detected, else set to false.
 *
 * @return
 *   A pointer to a command entry for the given command.
 *   If the command is not found, the returned value can be NULL. If a
 *   help command is issued, the returned value may point to a command entry
 *   table.
 ******************************************************************************/
const sl_cli_command_entry_t *sl_cli_command_find(sl_cli_handle_t handle,
                                                  int *token_c,
                                                  char *token_v[],
                                                  int *arg_ofs,
                                                  bool *single_flag,
                                                  bool *help_flag);

/***************************************************************************//**
 * @brief
 *   Find and execute a command given the string input and the command table.
 *
 * @note
 *   The input string will be modified in-place.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in, out] input
 *   C-string containing the user input. Must be '\0'-terminated.
 *
 * @return
 *   Status of the execution.
 ******************************************************************************/
sl_status_t sl_cli_command_execute(sl_cli_handle_t handle,
                                   char *input);

/***************************************************************************//**
 * @brief
 *   Find a number of matches and command matches from CLI's input buffer.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[out] possible_matches
 *   A string containing all possible command matches for what is currently in
 *   the input buffer.
 *
 * @param[in] possible_matches_size
 *   The size of the possible_matches string buffer.
 *
 * @param[out] input_length
 *   An integer value indicating the length of the last word in the input buffer.
 *
 * @param[out] input_position
 *   An integer value indicating where the last word in the input buffer begins.
 *
 * @return
 *   A number of possible matches found in the command table.
 ******************************************************************************/
#if SL_CLI_ADVANCED_INPUT_HANDLING
int sl_cli_command_find_matches(sl_cli_handle_t handle,
                                char *possible_matches,
                                size_t possible_matches_size,
                                int *input_length,
                                int *input_position);
#endif

/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_COMMAND_H
