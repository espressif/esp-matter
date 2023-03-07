/***************************************************************************//**
 * @file
 * @brief BT Mesh WSTK LCD driver header file
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

#ifndef SL_BTMESH_WSTK_LCD_H
#define SL_BTMESH_WSTK_LCD_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_btmesh_wstk_lcd_config.h"

/***************************************************************************//**
 * \defgroup lcd_driver LCD Driver
 * \brief Driver for SPI LCD Display.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup disp_interface
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup lcd_driver
 * @{
 ******************************************************************************/

/*******************************************************************************
 * LCD content can be updated one row at a time using function LCD_write().
 * Row number is passed as parameter, the possible values are defined below.
 ******************************************************************************/
/** up to 9 rows available on screen */
#define LCD_ROW_MAX          9
/** up to 21 characters, plus line break each row */
#define LCD_ROW_LEN         22

/***************************************************************************//**
 * LCD initialization, called once at startup.
 ******************************************************************************/
sl_status_t sl_btmesh_LCD_init(void);

/***************************************************************************//**
 * This function is used to write one line in the LCD.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of LCD display is written,
 *                 possible values are defined as LCD_ROW_xxx.
 * @returns Status of the write command.
 * @retval SL_STATUS_OK if successful
 * @retval SL_STATUS_FAIL otherwise
 ******************************************************************************/
sl_status_t sl_btmesh_LCD_write(char *str, uint8_t row);

/** @} (end addtogroup lcd_driver) */
/** @} (end addtogroup disp_interface) */

#endif /* SL_BTMESH_WSTK_LCD_H */
