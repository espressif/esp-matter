/***************************************************************************//**
 * @file
 * @brief LCD driver implementation file
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

#include <stdio.h>
#include <string.h>
#include "sl_btmesh_wstk_graphics.h"
#include "sl_btmesh_wstk_lcd.h"
#include "sl_btmesh_wstk_lcd_config.h"

/***************************************************************************//**
 * @addtogroup disp_interface
 * @{
 ******************************************************************************/

/// 2D array for storing the LCD content
static char LCD_data[LCD_ROW_MAX][LCD_ROW_LEN];

/***************************************************************************//**
 * Call a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  Status code of the operation.
 *
 * @note This is needed by the LCD driver
 ******************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void *),
                           void *argument,
                           unsigned int frequency)
{
  (void)pFunction;
  (void)argument;
  (void)frequency;

  return 0;
}

/*******************************************************************************
 * LCD initialization, called once at startup.
 ******************************************************************************/
sl_status_t sl_btmesh_LCD_init(void)
{
  memset(&LCD_data, 0, sizeof(LCD_data));

  graphInit(SL_BTMESH_WSTK_LCD_GRAPH_INIT_TEXT_CFG_VAL);

  return sl_btmesh_LCD_write(SL_BTMESH_WSTK_LCD_INIT_TEXT_CFG_VAL, SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
}

/*******************************************************************************
 * This function is used to write one line in the LCD.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of LCD display is written,
 *                 possible values are defined as LCD_ROW_xxx.
 *  @returns Status of the write command.
 *  @retval SL_STATUS_OK if successful
 *  @retval SL_STATUS_FAIL otherwise
 ******************************************************************************/
sl_status_t sl_btmesh_LCD_write(char *str, uint8_t row)
{
  char LCD_message[LCD_ROW_MAX * LCD_ROW_LEN];
  char *pRow;
  int i;

  if (row > LCD_ROW_MAX) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  pRow  = &(LCD_data[row - 1][0]);

  strcpy(pRow, str);

  LCD_message[0] = 0;

  for (i = 0; i < LCD_ROW_MAX; i++) {
    pRow  = &(LCD_data[i][0]);
    strcat(LCD_message, pRow);
    strcat(LCD_message, "\n"); // add newline at end of reach row
  }

  sl_status_t status = graphWriteString(LCD_message);
  return status;
}

/** @} (end addtogroup lcd_driver) */
/** @} (end addtogroup disp_interface) */
