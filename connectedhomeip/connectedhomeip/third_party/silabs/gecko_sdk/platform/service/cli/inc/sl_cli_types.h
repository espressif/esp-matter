/***************************************************************************//**
 * @file
 * @brief CLI type definitions.
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

#ifndef SL_CLI_TYPES_H
#define SL_CLI_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_cli_config.h"
#include "sl_status.h"
#include "sl_slist.h"
#include "sl_iostream.h"

#if !defined(SL_CATALOG_KERNEL_PRESENT)
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
#include "sl_sleeptimer.h"
#endif
#else
#include "cmsis_os2.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

#if ((!defined(SL_CATALOG_KERNEL_PRESENT))       \
  && (defined(SL_CATALOG_CLI_DELAY_PRESENT)      \
  || defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT) \
  || defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)))
#define SL_CLI_ACTIVE_FLAG_EN   1
#endif

#define SL_CLI_NVM3_KEY_COUNT    (0x100)
#define SL_CLI_NVM3_KEY_BEGIN    (0x3000)
#define SL_CLI_NVM3_KEY_END      (SL_CLI_NVM3_KEY_BEGIN + SL_CLI_NVM3_KEY_COUNT)

// Defines for return types
#define SL_CLI_INPUT_ORDINARY   (0U)   // Ordinary text or non-special characters
#define SL_CLI_INPUT_RETURN     (1U)   // RETURN ('\r')
#define SL_CLI_INPUT_ESCAPE     (2U)   // escape character (0xb)
#define SL_CLI_INPUT_ARROW      (3U)   // Must follow escape character. (0x5b)

#if !defined(SL_CATALOG_KERNEL_PRESENT)
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
typedef struct {
  struct sl_sleeptimer_timer_handle sleeptimer; ///< The CLI delay is using Sleep Timer for bare metal.
} cli_delay_t;
#endif
#endif

/// @brief Argument type typedef
typedef uint8_t sl_cli_argument_type_t;

/// @brief Command arguments data type
typedef struct {
  struct sl_cli *handle;                        ///< The current cli handle.
  int argc;                                     ///< The total number of input strings (command group names, if any + command + corresponding arguments).
  void **argv;                                  ///< A pointer to the input string array.
  int arg_ofs;                                  ///< The offset in the input string array where the arguments start (after the command group names, if any and command).
  const sl_cli_argument_type_t* arg_type_list;  ///< List of argument types
} sl_cli_command_arg_t;

/// @brief Command handler data type
typedef void (*sl_cli_command_func_t)(sl_cli_command_arg_t *arguments);

/// @brief Redirect command function
typedef void (*sl_cli_command_function_t)(char *arg_str, void *user);

/// @brief Command struct representing a command and its variable types.
/// Represents both a group and a single command. A group is characterized
/// by the its argument list being { SL_CLI_ARG_GROUP }. The handler will then
/// point to the first element in the group array (sl_cli_command_entry_t[]).
/// To initialize, use macros SL_CLI_COMMAND or SL_CLI_COMMAND_GROUP.
/// arg_help is string where 0x1f (unit separator) separates the messages.
typedef struct {
  sl_cli_command_func_t function;             ///< Command function
    #if SL_CLI_HELP_DESCRIPTION_ENABLED
  char                   *help;               ///< Info displayed by "help" command
  char                   *arg_help;           ///< Info for arguments.
    #endif // SL_CLI_HELP_DESCRIPTION_ENABLED
  sl_cli_argument_type_t arg_type_list[];     ///< List of argument types
} sl_cli_command_info_t;

/// @brief Struct associating a command with a string.
/// The command table should be an array of these kinds of structs.
typedef struct {
  const char                  *name;        ///< String associated with command/group
  const sl_cli_command_info_t *command;     ///< Pointer to command information
  const bool                  is_shortcut;  ///< Indicating if the entry is a shortcut
} sl_cli_command_entry_t;

/// @brief Struct representing a command group.
typedef struct {
  sl_slist_node_t node;                         ///< Command group list node.
  bool in_use;                                  ///< Node in use indicator.
  const sl_cli_command_entry_t *command_table;  ///< Command table pointer.
} sl_cli_command_group_t;

// Distinguishing different input types
typedef uint8_t sl_cli_input_type_t;

/// @brief Struct representing an instance of the CLI.
typedef struct sl_cli {
  bool tick_in_progress;                       ///< True when a tick is in progress.
  const char *prompt_string;                   ///< The current command prompt.
  bool req_prompt;                             ///< The next tick shall write a prompt.
  char input_buffer[SL_CLI_INPUT_BUFFER_SIZE]; ///< The input buffer.
  int input_size;                              ///< Length of input buffer.
  int input_pos;                               ///< The position the user is currently typing to.
  int input_len;                               ///< The total length of the current input.
  sl_cli_input_type_t last_input_type;         ///< Keeps track of last input.
  sl_slist_node_t *command_group;              ///< Base for the command group list.
  sl_cli_command_function_t command_function;  ///< Function pointer to an alternate command function.
  void *aux_argument;                          ///< User defined command argument.
  void *session_data;                          ///< Instance session data; own by submodule
#if SL_CLI_NUM_HISTORY_BYTES
  char history_buf[SL_CLI_NUM_HISTORY_BYTES];  ///< The history buffer.
  size_t history_pos;                          ///< Position in history, if enabled.
#endif
  sl_iostream_t  *iostream_handle;             ///< The iostream used by the CLI.
#if defined(SL_CLI_ACTIVE_FLAG_EN)
  bool active;                                 ///< A boolean indicating that the CLI is processing input.
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  uint32_t start_delay_tick;                   ///< A delay after the CLI task has started before any actions in ticks.
  uint32_t loop_delay_tick;                    ///< A delay in the CLI task loop in ticks.
#else
  int input_char;                              ///< A buffer that may contain the last input character.
  bool block_sleep;
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  cli_delay_t cli_delay;                       ///< Instance data for the CLI delay function.
#endif
#endif
} sl_cli_t;

typedef sl_cli_t *sl_cli_handle_t;

/// @brief The structure defining the parameters for creating a CLI instance.
typedef struct {
  const char *task_name;                         ///< Task name.
  sl_iostream_t  *iostream_handle;               ///< IOstream handle.
  sl_cli_command_group_t *default_command_group; ///< Command_group.
#if defined(SL_CATALOG_KERNEL_PRESENT)
  osThreadId_t thread_id;                        ///< Thread ID.
  void *thread_cb;                               ///< Thread control block.
                                                 ///  Can be set to NULL for dynamic allocation.
  void *stack;                                   ///< Pointer to the stack.
                                                 ///  Can be set to NULL for dynamic allocation.
  uint32_t stack_size;                           ///< Stack size.
  osPriority_t prio;                             ///< Task priority.
  uint32_t start_delay_ms;                       ///< Task start delay.
  uint32_t loop_delay_ms;                        ///< Task loop delay.
#endif
} sl_cli_instance_parameters_t;

/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_TYPES_H
