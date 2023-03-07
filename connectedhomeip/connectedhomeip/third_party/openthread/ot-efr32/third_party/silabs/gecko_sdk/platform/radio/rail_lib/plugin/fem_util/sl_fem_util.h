/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SL_FEM_UTIL_H
#define SL_FEM_UTIL_H

#include "sl_fem_util_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initialize FEM
 ******************************************************************************/
void sl_fem_util_init(void);

/***************************************************************************//**
 * Ready FEM after waking from selep
 ******************************************************************************/
void sl_fem_util_wakeup(void);

/***************************************************************************//**
 * Prepare FEM for sleep
 ******************************************************************************/
void sl_fem_util_shutdown(void);

/***************************************************************************//**
 * Get whether or not FEM optimized adio configuration is selected
 ******************************************************************************/
bool sl_fem_util_get_phy_select(void);

/***************************************************************************//**
 * Enable/disable FEM optimized radio configuration
 ******************************************************************************/
void sl_fem_util_enable_phy(bool enable);

#ifdef __cplusplus
}
#endif

#endif // SL_FEM_UTIL_H
