/***************************************************************************//**
 * @file
 * @brief Functions for argument parsing in the CLI framework.
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

#ifndef SLI_CLI_ARGUMENTS_H
#define SLI_CLI_ARGUMENTS_H

#include <stdint.h>
#include <stdbool.h>

#include "sl_cli_config.h"
#include "sl_cli_types.h"
#include "sl_cli_arguments.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @brief
 *   Validate unsigned integer arguments of different sizes.
 *
 * @param[in] size
 *   Size of uint variable in bits, should be multiple of four or (multiple of
 *   four) minus one.
 *
 * @param[in] argument
 *   Argument string to be validated, can be decimal or hex.
 *
 * @param[in] neg
 *   A bool that should be true if the original number was a negative number.
 *
 * @return
 *   bool indicating whether argument is a valid unsigned integer of given size.
 ******************************************************************************/
bool sli_cli_arguments_validate_uint(int size,
                                     const char *argument,
                                     bool neg);

/***************************************************************************//**
 * @brief
 *   Validate signed integer arguments of different sizes.
 *
 * @note
 *   Uses sli_cli_arguments_validate_uint function called with size - 1.
 *
 * @param[in] size
 *   Size of int variable in bits, should be multiple of four.
 *
 * @param[in] argument
 *   Argument string to be validated, can be decimal or hex.
 *
 * @return
 *   bool indicating whether the argument is a valid signed integer of given size.
 ******************************************************************************/
bool sli_cli_arguments_validate_int(int size,
                                    const char *argument);

/***************************************************************************//**
 * @brief
 *   Validate hex string argument, check if it is wrapped in {} and whether it contains
 *   an even number of valid hex digits and arbitrary number of spaces only.
 *
 * @param[in] argument
 *   Argument string to be validated.
 *
 * @return
 *   bool indicating whether argument is a valid hex string.
 ******************************************************************************/
bool sli_cli_arguments_validate_hex_string(const char *argument);

/***************************************************************************//**
 * @brief
 *   Validate argument of any type and choose the right validation function.
 *
 * @param[in] type
 *   sl_cli_argument_type_t used to pick right function.
 *
 * @param[in] argument
 *   Argument string to be validated.
 *
 * @return
 *   bool indicating whether argument is a valid instance of the argument type.
 ******************************************************************************/
bool sli_cli_validate_argument(sl_cli_argument_type_t type,
                               const char *argument);

/***************************************************************************//**
 * @brief
 *   Convert and store argument in memory_array, if necessary, and store
 *   pointer to it in argv.
 *
 * @note
 *   Function assumes little-endian byte format.
 *
 * @param[in] type
 *   sl_cli_argument_type_t to choose right conversion.
 *
 * @param[in, out] argument
 *   Argument string to be converted.
 *
 * @param[out] argv
 *   Array where pointer to the argument should be stored.
 *
 * @param[out] memory_array
 *   Array where argument should be stored byte for byte.
 *
 * @param[in, out] arg_index
 *   Keeps track of where in argv next argument should be stored.
 *
 * @param[in, out] mem_index
 *   Keeps track of where in memory_array next argument should be stored.
 ******************************************************************************/
void sli_cli_arguments_convert_single(sl_cli_argument_type_t type,
                                      char *argument,
                                      void *argv[],
                                      uint32_t *memory_array,
                                      int *arg_index,
                                      int *mem_index);

/***************************************************************************//**
 * @brief
 *   Convert and store arguments in memory_array and store pointers in argv.
 *
 * @param[in] argument_type_list
 *   Array describing the argument types of the arguments in token_v.
 *
 * @param[in] token_c
 *   Length of how many arguments in input.
 *
 * @param[in, out] token_v
 *   Array of argument as strings to be converted.
 *
 * @param[out] arg_ofs
 *   The offset in the token_v array where command argument starts.
 *   The first elements in the token_v array will contain pointers to the
 *   command string(s).
 *
 * @param[out] argv
 *   Array where pointers to arguments should be stored.
 *
 * @param[out] memory_array
 *   Array where numerical arguments should be stored byte for byte.
 *
 * @return
 *   Return SL_STATUS_OK if converting of arguments was successful,
 *   SL_STATUS_INVALID_TYPE if one or more arguments are invalid and
 *   SL_STATUS_INVALID_COUNT if there is an incorrect number of arguments.
 *   The SL_STATUS_INVALID_PARAMETER error is indicating an internal error.
 ******************************************************************************/
sl_status_t sli_cli_arguments_convert_multiple(const sl_cli_argument_type_t *argument_type_list,
                                               const int token_c,
                                               char *token_v[],
                                               int arg_ofs,
                                               void *argv[],
                                               uint32_t *memory_array);

#ifdef __cplusplus
}
#endif

#endif // SLI_CLI_ARGUMENTS_H
