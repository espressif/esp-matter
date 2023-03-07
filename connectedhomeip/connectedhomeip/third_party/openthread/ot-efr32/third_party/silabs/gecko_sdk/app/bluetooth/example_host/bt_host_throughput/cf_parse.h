/***************************************************************************//**
 * @file
 * @brief Configuration parser.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef CF_PARSE_H
#define CF_PARSE_H

#include "sl_status.h"

/**************************************************************************//**
 * Load file into memory.
 *
 * @param[in] filename File to be loaded.
 * @return Pointer where the file has been loaded into.
 * @retval NULL Failed to load the file.
 *
 * @note Allocated memory has to bee freed after use.
 *****************************************************************************/
char* cf_parse_load_file(const char *filename);

/**************************************************************************//**
 * Initialise parser module.
 *
 * @param[in] config Buffer that holds the configuration data.
 *****************************************************************************/
sl_status_t cf_parse_init(char *config);

/**************************************************************************//**
 * Parse next item from the allowlist.
 *
 * @param[out] address the address of the item.
 * @param[out] address_type the address type of the item
 *
 * @retval SL_STATUS_NOT_FOUND No more item found, use it for iteration.
 *****************************************************************************/
sl_status_t cf_parse_allowlist(uint8_t *address, uint8_t *address_type);

/**************************************************************************//**
 * Deinitialize parser module.
 *****************************************************************************/
sl_status_t cf_parse_deinit(void);

#endif // CF_PARSE_H
