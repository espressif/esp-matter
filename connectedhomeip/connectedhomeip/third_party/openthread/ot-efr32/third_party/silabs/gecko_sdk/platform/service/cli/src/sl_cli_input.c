/***************************************************************************//**
 * @file
 * @brief Functions that implement the cli front end. Characters are appended
          to make a command line string.
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

#include "sl_cli_input.h"
#include "sli_cli_input.h"
#include "sl_cli_command.h"
#include "sli_cli_io.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 ***************************   LOCAL MACROS   **********************************
 ******************************************************************************/

#define SL_CLI_ESCAPE_CHAR       (0x1b)
#define SL_CLI_ARROW_CHAR        (0x5b)
#define SL_CLI_UP_ARROW_CHAR     (0x41)
#define SL_CLI_DOWN_ARROW_CHAR   (0x42)
#define SL_CLI_RIGHT_ARROW_CHAR  (0x43)
#define SL_CLI_LEFT_ARROW_CHAR   (0x44)
#define SL_CLI_DELETE_CHAR       (0x7f)

#if SL_CLI_LOCAL_ECHO
#define input_putchar(c)        sli_cli_io_putchar((c))
#define input_printf(...)       sli_cli_io_printf(__VA_ARGS__)
#else
#define input_putchar(c)        (void)0
#define input_printf(...)       (void)0
#endif

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

#if SL_CLI_ADVANCED_INPUT_HANDLING
/***************************************************************************//**
 * @brief
 *   Move the arrow one position left in the terminal
 ******************************************************************************/
static void write_left_arrow(void)
{
  input_putchar('\b');
}

/***************************************************************************//**
 * @brief
 *   Move the arrow one position right in the terminal
 *
 * @param handle
 *   A handle to a CLI instance.
 *
 * @param position
 *   The current position of the cursor. Needed to figure out which letters to
 *   overwrite.
 ******************************************************************************/
static void write_right_arrow(sl_cli_handle_t handle,
                              int position)
{
#if (SL_CLI_LOCAL_ECHO == 0)
  (void)handle;
  (void)position;
#endif
  // Overwrite the next character to move arrow right
  input_putchar(handle->input_buffer[position]);
}
#endif // SL_CLI_ADVANCED_INUT_HANDLING

#if SL_CLI_NUM_HISTORY_BYTES
/***************************************************************************//**
 * @brief
 *   Get the number of command lines in the history buffer.
 *
 * @param handle
 *   sl_cli_handle_t handle to the CLI.
 *
 * @return
 *   Returns the number of command lines in the history buffer.
 ******************************************************************************/
static size_t history_get_count(sl_cli_handle_t handle)
{
  size_t cnt = 0;

  for (size_t i = 0; i < strlen(handle->history_buf); i++) {
    if (handle->history_buf[i] == '\0') {
      break;
    }
    if (handle->history_buf[i] == '\n') {
      cnt++;
    }
  }

  return cnt;
}

/***************************************************************************//**
 * @brief
 *   Get the begin and end pointers to a given history position given by the
 *   handle variable history_pos.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[out] ofs_begin
 *   A pointer to a variable that shall receive the history begin offset.
 *
 * @param[out] ofs_end
 *   A pointer to a variable that shall receive the history end offset.
 *
 * @return
 *   Returns true if the history buffer contain data false otherwise.
 ******************************************************************************/
static bool history_get_ofs(sl_cli_handle_t handle,
                            size_t *ofs_begin,
                            size_t *ofs_end)
{
  *ofs_begin = 0;
  *ofs_end = 0;

  if (handle->history_pos == 0) {
    return false;
  }

  if (strlen(handle->history_buf) == 0) {
    return false;
  }

  // Find the beginning of the string
  size_t o = 0;
  for (size_t p = 1; p < handle->history_pos; p++) {
    for (; o < sizeof(handle->history_buf); o++) {
      (*ofs_begin)++;
      (*ofs_end)++;
      if (handle->history_buf[o] == '\n') {
        o++;
        break;
      }
    }
  }

  // Find the end of the string
  for (; o < sizeof(handle->history_buf); o++) {
    if ((handle->history_buf[o] == '\n') || (handle->history_buf[o] == '\0')) {
      break;
    }
    (*ofs_end)++;
  }

  return true;
}
#endif // SL_CLI_NUM_HISTORY_BYTES

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void sl_cli_input_clear(sl_cli_handle_t handle)
{
  memset(handle->input_buffer, '\0', handle->input_size);
  handle->input_pos = 0;
  handle->input_len = 0;
}

#if SL_CLI_ADVANCED_INPUT_HANDLING
void sl_cli_input_autocomplete(sl_cli_handle_t handle)
{
  char possible_matches[SL_CLI_INPUT_BUFFER_SIZE] = { '\0' };
  int input_position;
  int input_length;

  int number_of_matches = sl_cli_command_find_matches(handle,
                                                      possible_matches, sizeof(possible_matches),
                                                      &input_length, &input_position);

  // Number of matches is one, write match to buffer and terminal
  if (number_of_matches == 1) {
    int length = strlen(possible_matches);
    int remaining_length = length - input_length;
    char *buf_dest = &handle->input_buffer[input_length + input_position];
    char *buf_src = &possible_matches[input_length];

    while (remaining_length > 0) {
      *buf_dest = *buf_src;
      buf_dest++;
      buf_src++;
      remaining_length--;
    }

    *buf_dest = '\0';

    for (int j = handle->input_pos; j < input_position + input_length; j++) {
      write_right_arrow(handle, handle->input_pos);
      handle->input_pos++;
    }
    handle->input_len += length - input_length;
    handle->input_pos += length - input_length;
    buf_src = &possible_matches[input_length];
    remaining_length = length - input_length;
    while (remaining_length > 0) {
      input_putchar(*buf_src);
      buf_src++;
      remaining_length--;
    }

    // Number of matches is larger than one, print list of matches and write
    // longest common prefix of these matches to buffer and terminal
  } else if (number_of_matches > 1) {
    // Find all the start indexes in possible matches
    int i = 0;
    bool match;
    char longest_common_prefix[SL_CLI_INPUT_BUFFER_SIZE];
    memset(longest_common_prefix, 0, sizeof(longest_common_prefix));
    int possible_matches_index = 1;
    int possible_matches_indexes[SL_CLI_INPUT_BUFFER_SIZE] = { 0 };
    while (possible_matches[i] != '\0') {
      if (possible_matches[i] == ' ') {
        possible_matches_indexes[possible_matches_index] = i + 1;
        possible_matches_index++;
      }
      i++;
    }
    possible_matches_indexes[possible_matches_index] = i;
    possible_matches_index++;

    // Find the longest common prefix of all the matches
    i = 0;
    char current_match;
    int possible_matches_length = strlen(possible_matches);
    for (int j = 0; j < possible_matches_index; j++) {
      match = true;
      current_match = possible_matches[j];
      while (possible_matches_indexes[i] != possible_matches_length) {
        if (possible_matches[possible_matches_indexes[i] + j] != current_match) {
          match = false;
          break;
        }
        i++;
      }
      if (match) {
        longest_common_prefix[j] = possible_matches[j];
      } else {
        longest_common_prefix[j] = '\0';
        break;
      }
      i = 0;
    }

    // If position does not match input length, move cursor to end of input
    for (int j = handle->input_pos; j < input_length; j++) {
      write_right_arrow(handle, handle->input_pos);
      handle->input_pos++;
    }

    // Print out the list of possible matches
    input_printf("\n%s\n%s%s", possible_matches, SL_CLI_PROMPT_STRING,
                 handle->input_buffer);

    // Update input buffer to longest common prefix
    int prefix_length = strlen(longest_common_prefix);
    char *buf_dest = &handle->input_buffer[input_length + input_position];
    char *buf_src = &longest_common_prefix[input_length];
    int  remain_length = prefix_length - input_length;
    while (remain_length > 0) {
      *buf_dest = *buf_src;
      buf_dest++;
      buf_src++;
      handle->input_pos++;
      handle->input_len++;
      remain_length--;
    }

    *buf_dest = '\0';
    buf_src = &longest_common_prefix[input_length];
    remain_length = prefix_length - input_length;
    // Write longest common prefix to terminal
    while (remain_length > 0) {
      input_putchar(*buf_src);
      buf_src++;
      remain_length--;
    }
  }
}
#endif // SL_CLI_ADVANCED_INPUT_HANDLING

#if SL_CLI_NUM_HISTORY_BYTES
void sli_cli_input_update_history(sl_cli_handle_t handle)
{
  while ((strlen(handle->input_buffer) + 1) > (sizeof(handle->history_buf) - strlen(handle->history_buf))) {
    // Remove the oldest history string(s) to make space for the last
    size_t history_cnt = history_get_count(handle);
    size_t ofs_begin, ofs_end;
    handle->history_pos = history_cnt;
    history_get_ofs(handle, &ofs_begin, &ofs_end);
    handle->history_buf[ofs_begin] = '\0';
  }
  // Move the history data to make space for the last
  memmove(handle->history_buf + strlen(handle->input_buffer) + 1, handle->history_buf, strlen(handle->history_buf) + 1);

  // Copy input to history buffer
  memcpy(handle->history_buf, handle->input_buffer, strlen(handle->input_buffer));
  handle->history_buf[strlen(handle->input_buffer)] = '\n';

  handle->history_pos = 0;
}

void sl_cli_input_get_history(sl_cli_handle_t handle,
                              int direction)
{
  size_t history_cnt = history_get_count(handle);
  bool clr_previous = false;

  if (direction == SL_CLI_INPUT_DIRECTION_DOWN) {
    if (handle->history_pos > 0) {
      (handle->history_pos)--;
      clr_previous = true;
    }
  } else if (direction == SL_CLI_INPUT_DIRECTION_UP) {
    if (handle->history_pos < history_cnt) {
      (handle->history_pos)++;
      clr_previous = true;
    } else {
      return;
    }
  } else {
    // Invalid input
    return;
  }

  if (handle->history_pos == 0) {
    clr_previous = true;
  }

  if (clr_previous) {
    for (size_t i = 0; i < strlen(handle->input_buffer); i++) {
      input_putchar('\b');
      input_putchar(' ');
      input_putchar('\b');
    }
    sl_cli_input_clear(handle);
  }

  if (handle->history_pos > 0) {
    size_t ofs_begin, ofs_end;
    bool found = history_get_ofs(handle, &ofs_begin, &ofs_end);
    if (found) {
      for (size_t o = ofs_begin; o < ofs_end; o++) {
        sl_cli_input_char(handle, handle->history_buf[o]);
      }
    }
  }
}
#endif // SL_CLI_NUM_HISTORY_BYTES

bool sl_cli_input_char(sl_cli_handle_t handle,
                       char c)
{
  int position = handle->input_pos;
  int length = handle->input_len;
  bool write_to_buffer = true;
  char *input_buffer = handle->input_buffer;

  // Interpret the new character based on what the last one was
  // First, check if last input was return, and look for a possible trailing
  // \n in case of \r\n line endings
  if (handle->last_input_type == SL_CLI_INPUT_RETURN && c == '\n') {
    // Set handle last input to ordinary, and return false, even though the
    // input actually was newline, because the command has already been executed
    handle->last_input_type = SL_CLI_INPUT_ORDINARY;
    return false;
  }

  if (handle->last_input_type == SL_CLI_INPUT_ORDINARY
      || handle->last_input_type == SL_CLI_INPUT_RETURN) {
    // Handle cases where the last input was ordinary, or return and above block
    // did not exectue
    // Start to set new last_input_type
    if (c == SL_CLI_ESCAPE_CHAR) {
      // Escape character. Change handle variable and exit function
      handle->last_input_type = SL_CLI_INPUT_ESCAPE;
      return false;
    } else if (c == '\r') {
      // Return character
      handle->last_input_type = SL_CLI_INPUT_RETURN;
    } else {
      // All other cases:
      handle->last_input_type = SL_CLI_INPUT_ORDINARY;
    }

    if (c == '\b') {
      // Handle backspace
      // Position is at end of non-zero input, write backspace and space and
      // another backspace to terminal to remove last character.
      // Remove last character from input buffer and decrease handle's length
      if (position > 0 && length == position) {
        input_putchar('\b');
        input_putchar(' ');
        input_putchar('\b');
        position--;
        input_buffer[position] = '\0';
        length = length - 1;

      #if SL_CLI_ADVANCED_INPUT_HANDLING
      } else if (position > 0 && length > position) {
        // Position is in the middle of the input, so terminal line needs to be
        // cleared, and line needs to be correctly rewritten and arrow correctly
        // positioned
        position--;
        input_putchar('\b');
        // Shift input down
        memmove(input_buffer + position, input_buffer + position + 1,
                length - position);
        input_buffer[length] = '\0';
        // Write out rest of buffer, extra space and backspace
        input_printf("%s \b", input_buffer + position);
        for (int i = length - 1; i > position; i--) {
          write_left_arrow();
        }
        length -= 1;
      #endif // SL_CLI_ADVANCED_INPUT_HANDLING
      } else if (position == 0) {
        // Position is at beginning of input, so do nothing
        return false;
      }

      // Update handle's position variable and return
      handle->input_pos = position;
      handle->input_len = length;
      return false;

    #if SL_CLI_ADVANCED_INPUT_HANDLING
    } else if (c == SL_CLI_DELETE_CHAR) {
      // Handle delete
      if (length > 0 && position < length) {
        // Remove a char from buffer, and print out new buffer
        memmove(input_buffer + position, input_buffer + position + 1,
                length - position);
        input_buffer[length] = '\0';
        input_printf("%s ", input_buffer + position);
        // Backspace to current pos
        for (int i = length; i > position; i--) {
          input_putchar('\b');
        }
        handle->input_len--;
      }
      return false;
    #endif // SL_CLI_ADVANCED_INPUT_HANDLING
    } else if (c == '\r' || c == '\n') {
      // Line ending detected
      // Print newline and reset handle's postion and length
      input_putchar('\r');
      input_putchar('\n');
      handle->input_pos = 0;
      handle->input_len = 0;
      write_to_buffer = false;
      return true;

    #if SL_CLI_ADVANCED_INPUT_HANDLING
    } else if (c == '\t') {
      // Autocompletion
      sl_cli_input_autocomplete(handle);
      return false;
    #endif // SL_CLI_ADVANCED_INPUT_HANDLING
    }

    // If the input buffer is full, do nothing, unless input is backspace
    // which is already handled
    if (length >= handle->input_size - 1) {
      return false;
    }

    #if SL_CLI_ADVANCED_INPUT_HANDLING
    // If position is not at end of buffer, all characters positioned after
    // current position must be moved one step towards right in buffer and
    // terminal
    if (length > position && write_to_buffer) {
      // Shift buffer up by one index
      memmove(input_buffer + position + 1, input_buffer + position,
              length - position);
      // Print out buffer end
      input_printf(input_buffer + position);
      for (int i = length; i >= position; i--) {
        write_left_arrow();
      }
    }
    #endif

    // Write character to the end of buffer if specified to do so
    if (write_to_buffer) {
      input_buffer[position] = c;
      handle->input_pos = position + 1;
      handle->input_len = handle->input_len + 1;
    }

    // Echo character to terminal
    input_putchar(c);
    return false;

#if SL_CLI_ADVANCED_INPUT_HANDLING
  } else if (handle->last_input_type == SL_CLI_INPUT_ESCAPE) {
    // Last input was escape symbol
    // Check if new input is one of the handled cases, and act accordingly
    if (c == SL_CLI_ARROW_CHAR) {
      handle->last_input_type = SL_CLI_INPUT_ARROW;
      return false;
    } else {
      // Not a recognized case. Recursive call with c as input,
      // but this time with last_input_type as ordinary, so this block
      // will not be executed again
      handle->last_input_type = SL_CLI_INPUT_ORDINARY;
      return sl_cli_input_char(handle, c);
    }
  } else if (handle->last_input_type == SL_CLI_INPUT_ARROW) {
    // If last input was the arrow character, treat new input as an arrow
    // character in a specific direction
    // Set last_input_type to ordinary, so next charcater is treated right
    handle->last_input_type = SL_CLI_INPUT_ORDINARY;
    switch (c) {
      case SL_CLI_UP_ARROW_CHAR:
        // Reqest history, if enabled
#if SL_CLI_NUM_HISTORY_BYTES
        sl_cli_input_get_history(handle, SL_CLI_INPUT_DIRECTION_UP);
#endif
        break;
      case SL_CLI_DOWN_ARROW_CHAR:
        // Reqest history, if enabled
#if SL_CLI_NUM_HISTORY_BYTES
        sl_cli_input_get_history(handle, SL_CLI_INPUT_DIRECTION_DOWN);
#endif
        break;
      #if SL_CLI_ADVANCED_INPUT_HANDLING
      case SL_CLI_RIGHT_ARROW_CHAR:
        // Move cursor right if possible
        if (position < length) {
          write_right_arrow(handle, position);
          handle->input_pos++;
        }
        break;
      case SL_CLI_LEFT_ARROW_CHAR:
        // Move cursor left if not already at the left end
        if (position > 0) {
          handle->input_pos--;
          write_left_arrow();
        }
        break;
      #endif
      default:
        // unkown input, do nothing
        return false;
    }
    #endif // SL_CLI_ADVANCED_INPUT_HANDLING
  }
  // Character is not newline
  return false;
}
