/***************************************************************************//**
 * @file
 * @brief Functions to convert and validate cli arguments.
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

#include "sli_cli_arguments.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*******************************************************************************
 ****************************   LOCAL FUNCTIONS   ******************************
 ******************************************************************************/
static bool is_argument_optional(sl_cli_argument_type_t arg_type)
{
  switch (arg_type) {
    case SL_CLI_ARG_UINT8OPT:
    case SL_CLI_ARG_UINT16OPT:
    case SL_CLI_ARG_UINT32OPT:
    case SL_CLI_ARG_INT8OPT:
    case SL_CLI_ARG_INT16OPT:
    case SL_CLI_ARG_INT32OPT:
    case SL_CLI_ARG_STRINGOPT:
    case SL_CLI_ARG_HEXOPT:
      return true;
    default:
      break;
  }
  return false;
}

static sl_cli_argument_type_t optional_to_mandatory(sl_cli_argument_type_t arg_type)
{
  switch (arg_type) {
    case SL_CLI_ARG_UINT8OPT:
    case SL_CLI_ARG_UINT16OPT:
    case SL_CLI_ARG_UINT32OPT:
    case SL_CLI_ARG_INT8OPT:
    case SL_CLI_ARG_INT16OPT:
    case SL_CLI_ARG_INT32OPT:
    case SL_CLI_ARG_STRINGOPT:
    case SL_CLI_ARG_HEXOPT:
      return arg_type - 0x10;
    default:
      break;
  }
  return arg_type;
}

/*******************************************************************************
 ****************************   GLOBAL FUNCTIONS   *****************************
 ******************************************************************************/
bool sli_cli_arguments_validate_uint(int size,
                                     const char *argument,
                                     bool neg)
{
  int i;
  // Argument is hexadecimal
  if (argument[0] == '0' && (argument[1] == 'x' || argument[1] == 'X')) {
    i = 2;
    while (argument[i] != '\0') {
      int value = (int)((unsigned char)argument[i]);
      if ((value < 0) || (value > 255) || !isxdigit(value)) {
        return false;
      }
      i++;
    }
    // Check if number of digits is smaller than int size divided by four
    if (size % 4 == 3) {
      if ((int)(strlen(argument) - 2) == ((size + 1) / 4)) {
        return ((int)argument[2] - (int) '0' <= 7);
      }
      return ((int)(strlen(argument) - 2) < ((size + 1) / 4));
    }
    return (int)(strlen(argument) - 2) <= size / 4;
  }
  // Argument is decimal
  i = 0;
  while (argument[i] != '\0') {
    int value = (int)((unsigned char)argument[i]);
    if ((value < 0) || (value > 255) || !isdigit(value)) {
      return false;
    }
    i++;
  }
  // Check if only the size-last bits are set
  unsigned long long value = strtoll(argument, NULL, 10);
  if (size == 32) {
    return value == (value & 0xFFFFFFFF);
  }
  if (neg && (value > 0)) {
    // Negative numbers can have one higher magnitude than the positive value.
    value--;
  }
  return value == (value & ((1U << size) - 1));
}

bool sli_cli_arguments_validate_int(int size,
                                    const char *argument)
{
  if (argument[0] == '-') {
    const char *sub_array = argument + 1;
    return sli_cli_arguments_validate_uint(size - 1, sub_array, true);
  }
  return sli_cli_arguments_validate_uint(size - 1, argument, false);
}

bool sli_cli_arguments_validate_hex_string(const char *argument)
{
  if (argument[0] != '{') {
    return false;
  }
  int i = 1;
  int digit_count = 0;
  while (argument[i] != '}') {
    int value = (int)((unsigned char)argument[i]);
    if ((value < 0) || (value > 255)) {
      return false;
    }
    if (isxdigit(value)) {
      digit_count++;
    } else if (argument[i] == ' ') {
      i++;
      continue;
    } else if (argument[i] == '\0') {
      return false;
    } else {
      return false;
    }
    i++;
  }
  if (argument[i + 1] != '\0') {
    return false;
  }
  // Check if number of hex digits is an even number
  if (digit_count % 2 != 0) {
    return false;
  }
  return true;
}

bool sli_cli_validate_argument(sl_cli_argument_type_t type,
                               const char *argument)
{
  switch (type) {
    case SL_CLI_ARG_UINT8:
      return sli_cli_arguments_validate_uint(8, argument, false);
    case SL_CLI_ARG_UINT16:
      return sli_cli_arguments_validate_uint(16, argument, false);
    case SL_CLI_ARG_UINT32:
      return sli_cli_arguments_validate_uint(32, argument, false);
    case SL_CLI_ARG_INT8:
      return sli_cli_arguments_validate_int(8, argument);
    case SL_CLI_ARG_INT16:
      return sli_cli_arguments_validate_int(16, argument);
    case SL_CLI_ARG_INT32:
      return sli_cli_arguments_validate_int(32, argument);
    case SL_CLI_ARG_STRING:
      return true;
    case SL_CLI_ARG_HEX:
      return sli_cli_arguments_validate_hex_string(argument);
    default:
      return false;
  }
}

void sli_cli_arguments_convert_single(sl_cli_argument_type_t type,
                                      char *argument,
                                      void *argv[],
                                      uint32_t *memory_array,
                                      int *arg_index,
                                      int *mem_index)
{
  switch (type) {
    case SL_CLI_ARG_UINT8:
    {
      // Convert and store in memory array
      uint8_t uint8_argument = (uint8_t)strtoul(argument, NULL, 0);
      memory_array[*mem_index] = uint8_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_UINT16:
    {
      // Convert and store in memory array
      uint16_t uint16_argument = (uint16_t)strtoul(argument, NULL, 0);
      memory_array[*mem_index] = uint16_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_UINT32:
    {
      // Convert and store in memory array
      uint32_t uint32_argument = (uint32_t)strtoul(argument, NULL, 0);
      memory_array[*mem_index] = uint32_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_INT8:
    {
      // Convert and store in memory array
      int8_t int8_argument = (int8_t)strtol(argument, NULL, 0);
      memory_array[*mem_index] = int8_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_INT16:
    {
      // Convert and store in memory array
      int16_t int16_argument = (int16_t)strtol(argument, NULL, 0);
      memory_array[*mem_index] = int16_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_INT32:
    {
      // Convert and store in memory array
      int32_t int32_argument = (int32_t)strtol(argument, NULL, 0);
      memory_array[*mem_index] = int32_argument;
      argv[*arg_index] = &memory_array[*mem_index];
      *mem_index = *mem_index + 1;
    }
    break;
    case SL_CLI_ARG_STRING:
    {
      // Argument is already a string, so just point to it
      argv[*arg_index] = argument;
    }
    break;
    case SL_CLI_ARG_HEX:
    {
      // Hex strings are converted byte by byte
      int j = 1;
      uint16_t byte_count = 0;
      int digit_count = 0;
      char current_hex_number[3];

      current_hex_number[2] = '\0';
      while (argument[j] != '}') {
        if (argument[j] == ' ') {
          // Ignore space
          j++;
          continue;
        } else if (digit_count % 2 == 0) {
          // Interpret input in pairs, so just ignore every other character
          current_hex_number[0] = argument[j];
          digit_count++;
        } else {
          // Convert and store in the argument buffer.
          current_hex_number[1] = argument[j];
          digit_count++;
          argument[byte_count + 2] = (char)strtol(current_hex_number, NULL, 16);
          byte_count++;
        }
        j++;
      }
      // Byte 0 and 1 shall contain the length, while byte 2..n shall contain the data.
      argument[0] = (char)byte_count;
      argument[1] = (char)(byte_count >> 8);
      // Point argv to the decoded argument buffer.
      argv[*arg_index] = argument;
    }
    break;
    case SL_CLI_ARG_WILDCARD:
    {
      // Wildcard argument. Do no parsing
      argv[*arg_index] = argument;
    }
    break;
    default:
      // Unknown argument, do nothing and don't increment counter
      return;
  }
  *arg_index = *arg_index + 1;
}

sl_status_t sli_cli_arguments_convert_multiple(const sl_cli_argument_type_t *arg_type_list,
                                               const int token_c,
                                               char *token_v[],
                                               int arg_ofs,
                                               void *argv[],
                                               uint32_t *memory_array)
{
  int arg_index = 0;
  int mem_index = 0;
  sl_cli_argument_type_t argument_type = SL_CLI_ARG_END;
  bool additional_or_wildcard = false;
  bool optional = false;
  bool wildcard = false;
  int arg_type_c = 0;
  int arg_total_c;

  // Sanity check of input parameters
  if (arg_ofs > token_c) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if there is a wildcard or additional argument type in list
  while (arg_type_list[arg_type_c] != SL_CLI_ARG_END) {
    if (arg_type_list[arg_type_c] == SL_CLI_ARG_ADDITIONAL) {
      additional_or_wildcard = true;
    } else if (arg_type_list[arg_type_c] == SL_CLI_ARG_WILDCARD) {
      additional_or_wildcard = true;
    } else if (is_argument_optional(arg_type_list[arg_type_c])) {
      optional = true;
    }
    arg_type_c++;
  }
  arg_total_c = arg_type_c + arg_ofs;

  // Check the number of arguments
  if (additional_or_wildcard) {
    if (token_c < (arg_total_c - 1)) {
      return SL_STATUS_INVALID_COUNT;
    }
  } else if (optional) {
    if (token_c < (arg_total_c - 1)) {
      return SL_STATUS_INVALID_COUNT;
    }
  } else {
    if (token_c != arg_total_c) {
      return SL_STATUS_INVALID_COUNT;
    }
  }

  // Convert and validate arguments with correct argument type
  int type_o = 0;
  for (int i = 0; i < token_c; i++) {
    // Set correct argument type of variables until the additional or wildcard
    // type is reached, if there is one
    if (i < arg_ofs) {
      // commands are strings
      argument_type = SL_CLI_ARG_STRING;
    } else {
      if (arg_type_list[type_o] == SL_CLI_ARG_ADDITIONAL) {
        // If argument type is additional, argument_type variable should be unchanged
        if (arg_type_list[type_o + 1] != SL_CLI_ARG_END) {
          return SL_STATUS_INVALID_COUNT;
        }
      } else if (arg_type_list[type_o] == SL_CLI_ARG_WILDCARD) {
        // If argument type is wildcard, wildcard variable is to true so that
        // no validation is conducted
        wildcard = true;
        if (arg_type_list[type_o + 1] != SL_CLI_ARG_END) {
          return SL_STATUS_INVALID_COUNT;
        }
      } else if (is_argument_optional(arg_type_list[type_o])) {
        // If argument type is optional
        if (arg_type_list[type_o + 1] != SL_CLI_ARG_END) {
          return SL_STATUS_INVALID_COUNT;
        }
        argument_type = optional_to_mandatory(arg_type_list[type_o]);
      } else {
        // If argument type is something else, argument_type variable should be
        // set to the argument type in index i of arg_type_list array
        argument_type = arg_type_list[type_o];
        type_o++;
      }
    }

    // Validate and convert argument type unless it is a wildcard in which case
    // it will only be converted, not validated
    if (!wildcard) {
      if (sli_cli_validate_argument(argument_type, token_v[i])) {
        sli_cli_arguments_convert_single(argument_type, token_v[i], argv, memory_array,
                                         &arg_index, &mem_index);
      } else {
        return SL_STATUS_INVALID_TYPE;
      }
    } else {
      sli_cli_arguments_convert_single(SL_CLI_ARG_WILDCARD, token_v[i], argv, memory_array,
                                       &arg_index, &mem_index);
    }
  }

  return SL_STATUS_OK;
}
