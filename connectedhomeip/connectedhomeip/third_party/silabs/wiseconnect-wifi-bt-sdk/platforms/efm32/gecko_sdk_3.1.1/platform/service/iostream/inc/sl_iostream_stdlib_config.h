/***************************************************************************//**
 * @file
 * @brief IO Stream STDIO Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_IOSTREAM_STDLIB_CONFIG_H
#define SL_IOSTREAM_STDLIB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_stdlib_cfg IO Stream STDLIB Configuration
 * @brief IO Stream STDLIB Configuration
 * @details
 * ## Overview
 *
 *   The buffering mode is enabled by default in nano-lib meaning that the low-level
 *   write function is called only when a '\n' is seen by the standard library. This
 *   module allows to disable the buffering feature and output the data as soon as
 *   possible.
 *
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Disable standard library buffering
 ******************************************************************************/
void sl_iostream_stdlib_disable_buffering(void);

/** @} (end addtogroup iostream_stdlib_cfg) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_STDLIB_CONFIG_H
