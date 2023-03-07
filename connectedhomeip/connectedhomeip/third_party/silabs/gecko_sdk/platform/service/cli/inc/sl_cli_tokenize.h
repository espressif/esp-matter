/***************************************************************************//**
 * @file
 * @brief Splitting of c-strings by space
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

#ifndef SL_CLI_TOKENIZE_H
#define SL_CLI_TOKENIZE_H

#include "sl_cli_config.h"
#include "sl_cli_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Split a C-string on space, ignoring sections in {} or "".
 *
 * @note
 *   This function modifies the content of the input string,
 *   and points the pointers in tokens to addresses in the input string.
 *   In quote strings ("") or curly braces ({}), the string will not be split,
 *   The quotes will be removed, but curly braces are kept for validation.
 *   To avoid '"' being interpreted as start/end of quote string, it may be
 *   escaped using '\\"', both within and outside of "...". '{' may be escaped
 *   using *   '\\{'. There is no need for escaping '}', since only hex digits
 *   are allowed within {...}. Backslash also needs be be escaped, as '\\'.
 *
 * @param[in,out] input
 *   The C-string to be split. Must be '\0'-terminated. Will be modified,
 *   even if an error occurs.
 *
 * @param[out] token_c
 *   The number of tokens put in the token_v array.
 *
 * @param[out] token_v
 *   An array containing the tokens obtained from splitting.
 *
 * @return
 *   Status of the operation.
 ******************************************************************************/
// Note: doxygen syntax for '\' in output is '\\'. Escape sequences are only
// prefixed with one backslash, as '\"' or '\\'
sl_status_t sl_cli_tokenize(char input[],
                            int *token_c,
                            char *token_v[]);

/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif /* SL_CLI_TOKENIZE_H */
