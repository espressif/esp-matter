/***************************************************************************//**
 * @file
 * @brief Functions to split a command line into tokens.
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

#include "sl_cli_tokenize.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 *****************************   LOCAL FUNCTIONS   *****************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Function escaping the next character by shifting the input one char down,
 *   if the next char matches one of the escaped sequences
 *
 * @param[in,out] input
 *   The C-string to be splitted. Must be '\0'-terminated. May be modified.
 *
 * @param[in] i
 *   Position in input where escape character was found ('\').
 *
 * @param[in] size
 *   Length if input.
 *
 * @return
 *   true for success, false for error.
 ******************************************************************************/
static bool escape(char input[],
                   int i,
                   int size)
{
  // Check if next character is one of the escaped ones
  if (input[i + 1] == '"'
      || input[i + 1] == '{'
      || input[i + 1] == '\\') {
    // Next input matches, so shift array
    memmove(input + i, input + i + 1, size - i);
    return true;
  } else {
    // Unknown sequence
    return false;
  }
}

/*******************************************************************************
 ****************************   GLOBAL FUNCTIONS   *****************************
 ******************************************************************************/
sl_status_t sl_cli_tokenize(char input[],
                            int *token_c,
                            char *token_v[])
{
  // Get size of string and declare variables
  uint32_t input_size = strlen(input);
  uint32_t i = 0;

  // Ignore preceding space
  while (input[i] == ' ') {
    i++;
  }
  token_v[0] = input + i;
  *token_c = 1;
  while (i < input_size) {
    if (input[i] == '{' || input[i] == '"') {
      // Start of segment where splitting should not be performed
      char end_char;
      if (input[i] == '{') {
        end_char = '}';
      } else {
        end_char = '"';
        // Token ignores " character
        token_v[*token_c - 1] = input + i + 1;
      }
      // Verify that preceding character is space or end of string
      if (i > 1 && (input[i - 1] != ' ' && input[i - 1] != '\0')) {
        return SL_STATUS_INVALID_PARAMETER;
      }

      // Loop through input to find end character
      i++;
      while (input[i] != end_char) {
        if (input[i] == '\\') {
          // Next character should be escaped
          if (!escape(input, i, input_size)) {
            // Escape error
            return SL_STATUS_INVALID_PARAMETER;
          }
          input_size--;
        }
        i++;
        if (i >= input_size) {
          // End character not found, return error
          return SL_STATUS_INVALID_PARAMETER;
        }
      }
      // Verify that next character is space or end of string
      if (input[i + 1] != ' ' && input[i + 1] != '\0') {
        return SL_STATUS_INVALID_PARAMETER;
      }
      // Remove " from string end
      if (end_char == '"') {
        input[i] = '\0';
      }

      // Escape symbol encountered
    } else if (input[i] == '\\') {
      if (!escape(input, i, input_size)) {
        // Escape error
        return SL_STATUS_INVALID_PARAMETER;
      }
      input_size--;

      // Ordinary splitting
    } else if (input[i] == ' ') {
      // Turn space into '\0' to indicate end of string
      input[i] = '\0';
      // Add token if next is character is meaningful
      if (i + 1 < input_size && input[i + 1] != ' ') {
        // Only add argument if number of max arguments is not exceeded
        if (*token_c < SL_CLI_MAX_INPUT_ARGUMENTS) {
          // Set next token to start at the (i+1)th element in input
          token_v[*token_c] = input + i + 1;
        } else {
          // There are too many arguments, error needs to be signaled
          return SL_STATUS_HAS_OVERFLOWED;
        }
        (*token_c)++;
      }
    }
    i++;
  }

  return SL_STATUS_OK;
}
