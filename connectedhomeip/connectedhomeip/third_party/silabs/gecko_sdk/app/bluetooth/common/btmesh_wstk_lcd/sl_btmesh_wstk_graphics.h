/***************************************************************************//**
 * @file
 * @brief Displays text on the LCD
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BTMESH_WSTK_GRAPHICS_H
#define SL_BTMESH_WSTK_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"

// -----------------------------------------------------------------------------
// Public Function Declarations

/***************************************************************************//**
 *  @brief  Initializes the graphics stack
 *  @param[in]  header  Header Text on display
 ******************************************************************************/
void graphInit(char *header);

/***************************************************************************//**
 *  @brief  display a string on the LCD center aligned
 *  @param[in]  string  String to be displayed
 *  @returns Status of the command. SL_STATUS_OK if successful, an error code
 *           otherwise.
 ******************************************************************************/
sl_status_t graphWriteString(char *string);

#ifdef __cplusplus
}
#endif

#endif /* SL_BTMESH_WSTK_GRAPHICS_H */
