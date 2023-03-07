/***************************************************************************//**
 * @file
 * @brief Board Init
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
#ifndef SL_BOARD_INIT_H
#define SL_BOARD_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup board_init Board Init
 * @brief Initialization of Silicon Labs board features
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief Initialize board.
 * @details
 * Initialize a Silicon Labs board by enabling available and configured board
 * features, in addition to performing necessary board errata fixes and setting
 * default pin states.
 ******************************************************************************/
void sl_board_init(void);

/***************************************************************************//**
 * @brief Initialize board features that are required at early boot.
 * @details
 * Certain board features such as external oscillators may need to be powered
 * before core device features, such as the clock tree, are configured.
 ******************************************************************************/
void sl_board_preinit(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif // SL_BOARD_INIT_H
