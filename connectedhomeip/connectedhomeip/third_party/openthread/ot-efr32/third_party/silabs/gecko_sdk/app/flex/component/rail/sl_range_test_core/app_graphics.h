/***************************************************************************//**
 * @file
 * @brief app_graphics.h
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
#ifndef APP_GRAPHICS_H
#define APP_GRAPHICS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Display-specific constants
#define GRAPHICS_MENU_DISP_SIZE        (12U)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays).
 ******************************************************************************/
void graphics_init(void);

/*******************************************************************************
 * @brief  Function to display the menu.
 * @param  None
 * @return None.
 ******************************************************************************/
void graphics_draw_menu(void);

/*******************************************************************************
 * @brief  Function that draws the starting screen.
 * @param  None.
 * @return None.
 ******************************************************************************/
void graphics_draw_init_screen(void);

/*******************************************************************************
 * @brief  Displays the Range Test in RX mode.
 * @param  None.
 * @return None.
 ******************************************************************************/
void graphics_draw_rx_screen(void);

/*******************************************************************************
 * @brief  Displays the Range Test in TX mode.
 * @param  None.
 * @return None.
 ******************************************************************************/
void graphics_draw_tx_screen(void);

/*******************************************************************************
 * @brief  Function to clear the RSSI history.
 * @param  None.
 * @return None.
 ******************************************************************************/
void graphics_clear_rssi_buffer();

/*******************************************************************************
 * @brief  This function adds a given RSSI value to the RSSI history buffer.
 * @param  rssi: Current RSSI value to add to the RSSI history.
 * @return None.
 ******************************************************************************/
void graphics_add_to_rssi_buffer(uint8_t rssi);

/*******************************************************************************
 * @brief  This function checks and modifies a given RSSI value to be able to
 * add to the RSSI history buffer.
 * @param  rssi: Current RSSI value to add to the RSSI history.
 * @return display_value: modified value able to be displayed
 ******************************************************************************/
int8_t rssi_value_for_display(int8_t original_value);

#endif  // APP_GRAPHICS_H
