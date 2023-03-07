/***************************************************************************//**
 * @file
 * @brief app_process.h
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
#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/** State machine states */
typedef enum {
  /** Show the info or starting screen */
  INFO_SCREEN,
  /** Show the menu screen */
  MENU_SCREEN,
  /** Start the range test, init radio for defined config */
  START_MEASURMENT,
  /** Run RX range test and show it on display */
  RECEIVE_MEASURMENT,
  /** Run TX range test and show it on display */
  SEND_MEASURMENT
} state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * The function is used for Application logic.
 * @brief Application state machine, called infinitely
 *
 * @param RAIL_Handle_t Null pointer to keep stlye with other sample apps
 * @returns None
 ******************************************************************************/
void app_process_action(RAIL_Handle_t pointer);

/*******************************************************************************
 * State set for other part of the application.
 *
 * @param next_state What should be the next state in the state machine
 * @returns None
 ******************************************************************************/
void set_next_state(state_t next_state);

/*******************************************************************************
 * LCD screen needs update.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void request_refresh_screen(void);

#endif  // APP_PROCESS_H
