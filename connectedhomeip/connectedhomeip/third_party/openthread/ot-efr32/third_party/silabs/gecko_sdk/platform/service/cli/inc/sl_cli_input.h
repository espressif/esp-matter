/***************************************************************************//**
 * @file
 * @brief The cli input functions support character input from a user and
 *   creates a command string that can be passed on for execution.
 *   The user may input some control characters like BS, DEL and arrow keys
 *   according to ANSII escape sequences used by VT100 terminals to control
 *   editing and access previous commands stored in the history buffer.
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

#ifndef SL_CLI_INPUT_H
#define SL_CLI_INPUT_H

#include "sl_cli_config.h"
#include "sl_cli_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   MACROS   ************************************
 ******************************************************************************/

/// @brief Define the UP-direction.
#define SL_CLI_INPUT_DIRECTION_UP    (1)
/// @brief Define the DOWN-direction.
#define SL_CLI_INPUT_DIRECTION_DOWN  (-1)

/***************************************************************************//**
 * @brief
 *   Handle new input char from the terminal, both in terms of writing back to
 *   terminal and writing to the input buffer.
 *
 * @details
 *   This function should be called every time new input is detected. The
 *   behavior of the function is highly configurable through settings in
 *   cli_config.h.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in] c
 *   Input char to be processed by the function.
 *
 * @return
 *   Returns true if newline or return is detected, false otherwise.
 ******************************************************************************/
bool sl_cli_input_char(sl_cli_handle_t handle,
                       char c);

/***************************************************************************//**
 * @brief
 *   Empty the input buffer.
 *
 * @details
 *   This function should be called after newline has been detected and the
 *   information in the buffer has been processed.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 ******************************************************************************/
void sl_cli_input_clear(sl_cli_handle_t handle);

#if SL_CLI_ADVANCED_INPUT_HANDLING
/***************************************************************************//**
 * @brief
 *   Auto complete command in the input buffer if a single match is found.
 *   Print a list of matches if several are found.
 *   Do nothing if no matches are found.
 *
 * @details
 *   If a cursor position is not at end of input, the whole input buffer will
 *   still be used for matching. This function uses find_command_matches from
 *   the cli_command.c to find matches and number of matches. Space is used
 *   as a delimiter. If extra space is added at the end of the input buffer, no
 *   commands will be found.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 ******************************************************************************/
void sl_cli_input_autocomplete(sl_cli_handle_t handle);
#endif // SL_CLI_ADVANCED_INPUT_HANDLING

#if SL_CLI_NUM_HISTORY_BYTES
/***************************************************************************//**
 * @brief
 *   Get data from the history buffer and insert it into the input buffer.
 *
 * @details
 *   This function should be called when up or down arrow is pressed.
 *
 * @param[in, out] handle
 *   A handle to a CLI instance.
 *
 * @param[in] direction
 *   An integer indicating the direction pressed. 1 for up and -1 for down.
 ******************************************************************************/
void sl_cli_input_get_history(sl_cli_handle_t handle,
                              int direction);
#endif // SL_CLI_NUM_HISTORY_BYTES

/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_INPUT_H
