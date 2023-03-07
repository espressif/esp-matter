/***************************************************************************//**
 * @file
 * @brief Functions to find and execute cli commands.
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

#include "sl_cli.h"
#include "sl_cli_command.h"
#include "sl_cli_tokenize.h"
#include "sli_cli_io.h"
#include "sli_cli_arguments.h"
#include "sl_string.h"
#include "sl_common.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#define SL_CLI_TERMINAL_LINE_LENGTH  (80)

/***************************************************************************//**
 * @brief
 *   Hook executed before the command. Unless specifically redefined to
 *   something, this hook does nothing.
 *
 * @param[in] arguments     The arguments passed to the command handler.
 *
 ******************************************************************************/
SL_WEAK void sli_cli_pre_cmd_hook(sl_cli_command_arg_t* arguments)
{
  (void) arguments;
}

/***************************************************************************//**
 * @brief
 *   Hook executed after the command. Unless specifically redefined to
 *   something, this hook does nothing.
 *
 * @param[in] arguments     The arguments passed to the command handler.
 *
 ******************************************************************************/
SL_WEAK void sli_cli_post_cmd_hook(sl_cli_command_arg_t* arguments)
{
  (void) arguments;
}

/***************************************************************************//**
 * @brief
 *   Compare two command strings. If the comparison shall be case sensitive or
 *   not can be configured with SL_CLI_IGNORE_COMMAND_CASE.
 *
 * @param[in] a         String to compare.
 *
 * @param[in] b         String to compare.
 *
 * @return              An integer greater than, or less than 0 if the strings
 *                      are not equal. 0 if the strings are equal.
 ******************************************************************************/
static int cmd_strcmp(const char *a, const char *b)
{
#if SL_CLI_IGNORE_COMMAND_CASE
  return sl_strcasecmp(a, b);
#else
  return strcmp(a, b);
#endif
}

#if SL_CLI_HELP_DESCRIPTION_ENABLED
/***************************************************************************//**
 * @brief
 *   Print a string and append spaces until the total length is reached.
 *
 * @param[in] string
 *   A pointer to the string that shall be printed.
 *
 * @param[in] total_lenght
 *   The total length of the string. If the length of the string is equal or
 *  larger than total_length, no spaces are appended.
 ******************************************************************************/
static void print_and_pad(const char *string,
                          size_t total_length)
{
  size_t string_length = strlen(string);
  sli_cli_io_printf(string);
  for (; string_length < total_length; string_length++) {
    sli_cli_io_printf(" ");
  }
}

/***************************************************************************//**
 * @brief
 *   Get a string representation of an argument type
 *
 * @param[in] type
 *   The argument type to be translated.
 *
 * @return
 *   A pointer to the string.
 ******************************************************************************/
static const char *get_arg_type_string(sl_cli_argument_type_t type)
{
  const char *str;

  switch (type) {
    case SL_CLI_ARG_UINT8:
      str = "uint8";
      break;
    case SL_CLI_ARG_UINT16:
      str = "uint16";
      break;
    case SL_CLI_ARG_UINT32:
      str = "uint32";
      break;
    case SL_CLI_ARG_INT8:
      str = "int8";
      break;
    case SL_CLI_ARG_INT16:
      str = "int16";
      break;
    case SL_CLI_ARG_INT32:
      str = "int32";
      break;
    case SL_CLI_ARG_STRING:
      str = "string";
      break;
    case SL_CLI_ARG_HEX:
      str = "hex";
      break;
    case SL_CLI_ARG_UINT8OPT:
      str = "uint8opt";
      break;
    case SL_CLI_ARG_UINT16OPT:
      str = "uint16opt";
      break;
    case SL_CLI_ARG_UINT32OPT:
      str = "uint32opt";
      break;
    case SL_CLI_ARG_INT8OPT:
      str = "int8opt";
      break;
    case SL_CLI_ARG_INT16OPT:
      str = "int16opt";
      break;
    case SL_CLI_ARG_INT32OPT:
      str = "int32opt";
      break;
    case SL_CLI_ARG_STRINGOPT:
      str = "stringopt";
      break;
    case SL_CLI_ARG_HEXOPT:
      str = "hexopt";
      break;
    case SL_CLI_ARG_ADDITIONAL:
      str = "additional";
      break;
    case SL_CLI_ARG_WILDCARD:
      str = "wildcard";
      break;
    case SL_CLI_ARG_GROUP:
      str = "group";
      break;
    case SL_CLI_ARG_END:
      str = "end";
      break;
    default:
      str = "unknown";
      break;
  }

  return str;
}

/***************************************************************************//**
 * @brief
 *   Print out the available commands in the given command table and their
 *   argument list and help description. Line length on terminal is assumed to
 *   be given by the SL_CLI_TERMINAL_LINE_LENGTH #define or larger.
 *
 * @param[in] command_table
 *   sl_cli_command_entry_t array that is {NULL, NULL}-terminated. Can be both
 *   root command table and group command table
 ******************************************************************************/
static void cmd_help_command(const sl_cli_command_entry_t *cmd_entry)
{
  print_and_pad("", SL_CLI_HELP_CMD_PRE);
  print_and_pad(cmd_entry->name, SL_CLI_HELP_CMD_SIZE);
  sli_cli_io_printf(cmd_entry->command->help);
  sli_cli_io_printf("\n");

  // Print argument list
  const sl_cli_argument_type_t *arg_type_list;
  arg_type_list = cmd_entry->command->arg_type_list;
  if (arg_type_list[0] == SL_CLI_ARG_GROUP ||  arg_type_list[0] == SL_CLI_ARG_END) {
    // Do not do anything. No arguments to print
  } else {
    char *arg_help = cmd_entry->command->arg_help;
    int arg_no = 0;
    int arg_help_ofs = -1;
    int arg_help_length = strlen(arg_help);
    while (arg_type_list[arg_no] != SL_CLI_ARG_END) {
      const char *arg_str;
      arg_str = get_arg_type_string(arg_type_list[arg_no]);
      if (arg_help_length > 0) {
        arg_help_ofs++;
      }
      if (arg_type_list[arg_no] != SL_CLI_ARG_ADDITIONAL) {
        print_and_pad("", SL_CLI_HELP_CMD_PRE);
        print_and_pad("", SL_CLI_HELP_CMD_SIZE);
        sli_cli_io_printf("[");
        sli_cli_io_printf("%s", (strcmp(arg_str, "wildcard") == 0) ? "*" : arg_str);
        if (arg_type_list[arg_no + 1] == SL_CLI_ARG_ADDITIONAL) {
          sli_cli_io_putchar('+');
        }
        sli_cli_io_printf("] ");
        if ((arg_help_length > 0) && (arg_help[arg_help_ofs] != SL_CLI_UNIT_SEPARATOR_CHAR)) {
          while ((arg_help[arg_help_ofs] != SL_CLI_UNIT_SEPARATOR_CHAR) &&  (arg_help_ofs < arg_help_length)) {
            sli_cli_io_putchar(arg_help[arg_help_ofs]);
            arg_help_ofs++;
          }
        }
        sli_cli_io_printf("\n");
      }
      arg_no++;
    }
  }
}

#else
/***************************************************************************//**
 * @brief
 *   Print out the available commands in the given command table.
 *
 * @param[in] command_table
 *   sl_cli_command_entry_t array that is {NULL, NULL}-terminated. Can be both
 *   root command table and group command table
 ******************************************************************************/
static void cmd_help_command(const sl_cli_command_entry_t *cmd_entry)
{
  sli_cli_io_printf("  %s\n", cmd_entry->name);
}

#endif // SL_CLI_HELP_DESCRIPTION_ENABLED

static void cmd_help_group(const sl_cli_command_entry_t command_table[])
{
  int i = 0;

  //printf("---------------\n");
  while (command_table[i].name) {
    if (command_table[i].is_shortcut) {
      i++;
      continue;
    }
    cmd_help_command(&command_table[i]);
    i++;
  }
}

static void cmd_help(sl_cli_handle_t handle, const sl_cli_command_entry_t *cmd_table, bool single)
{
  if (cmd_table != NULL) {
    if (single) {
      cmd_help_command(cmd_table);
    } else {
      cmd_help_group(cmd_table);
    }
  } else {
    sl_cli_command_group_t *cmd_group;
    const sl_cli_command_entry_t *table;

    SL_SLIST_FOR_EACH_ENTRY(handle->command_group, cmd_group, sl_cli_command_group_t, node) {
      table = cmd_group->command_table;
      if (table != NULL) {
        cmd_help_group(table);
      }
    }
  }
}

/*******************************************************************************
 ****************************   GLOBAL FUNCTIONS   *****************************
 ******************************************************************************/
bool sl_cli_command_add_command_group(sl_cli_handle_t handle, sl_cli_command_group_t *command_group)
{
  bool status = false;

  if (command_group != NULL) {
    if (!command_group->in_use) {
      command_group->in_use = true;
      sl_slist_push(&handle->command_group, &command_group->node);
      status = true;
    }
  }
  return status;
}

bool sl_cli_command_remove_command_group(sl_cli_handle_t handle, sl_cli_command_group_t *command_group)
{
  bool status = false;

  if (command_group != NULL) {
    if (command_group->in_use) {
      command_group->in_use = false;
      sl_slist_remove(&handle->command_group, &command_group->node);
      command_group->node.node = NULL;
      status = true;
    }
  }
  return status;
}

// The following function is only needed if advanced input handling is enabled
#if !defined(SL_CLI_DUT) && (SL_CLI_ADVANCED_INPUT_HANDLING == 1)
int sl_cli_command_find_matches(sl_cli_handle_t handle,
                                char *possible_matches,
                                size_t possible_matches_size,
                                int *input_length,
                                int *input_position)
{
  int len;
  int original_input_buffer_len = strlen(handle->input_buffer);
  char *token_v[SL_CLI_MAX_INPUT_ARGUMENTS];
  char *buf;
  int token_c;

  *input_length = 0;
  *input_position = 0;
  sl_status_t status = sl_cli_tokenize(handle->input_buffer, &token_c, token_v);
  if (status != SL_STATUS_OK) {
    return 0;
  }

  int i = 0;
  int arg_ofs = 0;
  int number_of_matches = 0;
  *input_length = strlen(token_v[0]);

  sl_cli_command_group_t *cmd_group;
  const sl_cli_command_entry_t *table;
  SL_SLIST_FOR_EACH_ENTRY(handle->command_group, cmd_group, sl_cli_command_group_t, node) {
    table = cmd_group->command_table;
    if (table == NULL) {
      continue;
    }

    // If input buffer begins with spaces, increment input position to match
    while (handle->input_buffer[i] == ' ') {
      *input_position = *input_position + 1;
      i++;
    }

    // Look for possible command matches in command table and within groups.
    // Add all possible matches to possible_matches string
    i = 0;
    while ((table[i].name != NULL) && (arg_ofs <= token_c)) {
      if (table[i].is_shortcut) {
        // Ignore shortcuts
        i++;
        continue;
      }
      *input_length = handle->input_len - *input_position;
      if (strncmp(token_v[arg_ofs], table[i].name, *input_length) == 0) {
        int table_entry_length = strlen(table[i].name);
        if ((table[i].command->arg_type_list[0] == SL_CLI_ARG_GROUP) && (arg_ofs != (token_c - 1))) {
          table = (sl_cli_command_entry_t *)(table[i].command->function);
          i = 0;
          *input_position += strlen(token_v[arg_ofs]) + 1;
          while (handle->input_buffer[*input_position] == '\0') {
            *input_position = *input_position + 1;
          }
          arg_ofs++;
          continue;
        } else if (handle->input_len - *input_position <= table_entry_length) {
          // Will only show the total possible matches up to length of the
          // input buffer, and add "..." if there are more possible matches
          if (strlen(possible_matches) + strlen(table[i].name)
              >= SL_CLI_INPUT_BUFFER_SIZE - 4) {
            sl_strcat_s(possible_matches, possible_matches_size, "... ");
            break;
          }
          number_of_matches++;
          sl_strcat_s(possible_matches, possible_matches_size, table[i].name);
          sl_strcat_s(possible_matches, possible_matches_size, " ");
        } else if ((table[i].command->arg_type_list[0] == SL_CLI_ARG_GROUP) && (arg_ofs == (token_c - 1))) {
          int k = 0;
          table = (sl_cli_command_entry_t *)(table[i].command->function);
          while (table[k].name) {
            sl_strcat_s(possible_matches, possible_matches_size, table[k].name);
            sl_strcat_s(possible_matches, possible_matches_size, " ");
            number_of_matches++;
            k++;
          }
          *input_position = original_input_buffer_len;
          *input_length = 0;
          break;
        }
      }
      i++;
    }

    // The tokenizer will replace all spaces with '\0' in the input buffer,
    // these need to be converted back to spaces
    buf = handle->input_buffer;
    len = original_input_buffer_len;
    while (len > 0) {
      if (*buf == '\0') {
        *buf = ' ';
      }
      buf++;
      len--;
    }
  }

  return number_of_matches;
}
#endif // SL_CLI_ADVANCED_INPUT_HANDLING

static const sl_cli_command_entry_t *scan_entry(const sl_cli_command_entry_t *cmd_entry_in,
                                                bool group,
                                                bool *found,
                                                int *token_c,
                                                char *token_v[],
                                                int *arg_ofs,
                                                bool *single_flag,
                                                bool *help_flag)
{
  const sl_cli_command_entry_t *cmd_entry = cmd_entry_in;

  while ((cmd_entry->name != NULL) && (*arg_ofs < *token_c)) {
    if (cmd_strcmp(cmd_entry->name, token_v[*arg_ofs]) == 0) {
      // Command or group found
      (*arg_ofs)++;
      if (cmd_entry->command->arg_type_list[0] == SL_CLI_ARG_GROUP) {
        // Group found, continue search
        cmd_entry = (sl_cli_command_entry_t *)(cmd_entry->command->function);
        cmd_entry = scan_entry(cmd_entry, true, found, token_c, token_v, arg_ofs, single_flag, help_flag);
        break;
      } else {
        // Command found, stop search
        *single_flag = true;
        *found = true;
        break;
      }
    }
    cmd_entry++;
  }

  if (!(*found) && (!(*help_flag))) {
    if (group) {
      *help_flag = true;
      cmd_entry = cmd_entry_in;
      *found = true;
    } else {
      cmd_entry = NULL;
    }
  }

  return cmd_entry;
}

const sl_cli_command_entry_t *sl_cli_command_find(sl_cli_handle_t handle,
                                                  int *token_c,
                                                  char *token_v[],
                                                  int *arg_ofs,
                                                  bool *single_flag,
                                                  bool *help_flag)
{
  const sl_cli_command_entry_t *cmd_entry = NULL;
  bool help = false;
  bool found = false;

  *arg_ofs = 0;
  *single_flag = false;
  *help_flag = false;

  if (cmd_strcmp("help", token_v[*arg_ofs]) == 0) {
    help = true;
    (*arg_ofs)++;
  }

  sl_cli_command_group_t *cmd_group;
  SL_SLIST_FOR_EACH_ENTRY(handle->command_group, cmd_group, sl_cli_command_group_t, node) {
    cmd_entry = cmd_group->command_table;
    if (cmd_entry == NULL) {
      continue;
    }
    cmd_entry = scan_entry(cmd_entry, false, &found, token_c, token_v, arg_ofs, single_flag, help_flag);
    if (found) {
      break;
    }
  }
  if (help) {
    *help_flag = true;
  }

  return cmd_entry;
}

sl_status_t sl_cli_command_execute(sl_cli_handle_t handle,
                                   char *input)
{
  char *token_v[SL_CLI_MAX_INPUT_ARGUMENTS];
  int token_c;
  uint32_t memory_array[SL_CLI_MAX_INPUT_ARGUMENTS];
  void *argv[SL_CLI_MAX_INPUT_ARGUMENTS];
  sl_cli_command_arg_t arguments;
  int arg_ofs;

  // Split input string
  sl_status_t status = sl_cli_tokenize(input, &token_c, token_v);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Find command
  bool help_flag = false;
  bool single_flag;
  const sl_cli_command_entry_t *cmd_table = sl_cli_command_find(handle,
                                                                &token_c,
                                                                token_v,
                                                                &arg_ofs,
                                                                &single_flag,
                                                                &help_flag);
  if (help_flag) {
    cmd_help(handle, cmd_table, single_flag);
    return SL_STATUS_OK;
  } else if (cmd_table == NULL) {
    return SL_STATUS_NOT_FOUND;
  }

  // Convert arguments to correct type
  const sl_cli_command_info_t *cmd_info = cmd_table->command;
  status = sli_cli_arguments_convert_multiple(cmd_info->arg_type_list,
                                              token_c,
                                              token_v,
                                              arg_ofs,
                                              argv,
                                              memory_array);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Call function
  arguments.handle = handle;
  arguments.argc = token_c;
  arguments.argv = argv;
  arguments.arg_ofs = arg_ofs;
  arguments.arg_type_list = cmd_info->arg_type_list;

  sli_cli_pre_cmd_hook(&arguments);
  cmd_info->function(&arguments);
  sli_cli_post_cmd_hook(&arguments);

  // Command executed, return status that in this case is success
  return status;
}
