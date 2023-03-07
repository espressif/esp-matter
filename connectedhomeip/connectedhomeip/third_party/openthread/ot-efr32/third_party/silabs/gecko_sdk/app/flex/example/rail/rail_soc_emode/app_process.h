/***************************************************************************//**
 * @file
 * @brief app_tick.h
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
#include <stdint.h>
#include "rail.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// RAIL channel number
#define CHANNEL ((uint8_t) 0)

/// Emode state machine states
typedef enum {
  S_IDLE,            //!< Idle state
  S_CW,              //!< CW set
  S_SET_IDLE,        //!< Set power level in idle
  S_RX,              //!< RX state
  S_SET_POWER_LEVEL, //!< Set the power level for the radio
  S_GET_POWER_LEVEL, //!< Print out the power level of the radio
  S_PERIODIC_RX,     //!< Periodic RX state
  S_PERIODIC_TX,     //!< Periodic TX state
  S_PACKET_RECEIVED  //!< Packet print out state
} state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Sleep level variable
extern volatile uint8_t sleep_mode;

/// TX power settings
extern volatile RAIL_TxPowerLevel_t power_raw;
extern volatile RAIL_TxPower_t power_deci_dbm;
extern volatile bool is_raw;

/// Scheduled TX/RX variables
extern volatile uint32_t sleep_period;
extern volatile uint32_t rx_on_period;

/// Schedule state first run
extern volatile bool init_needed;

/// Schedule TX/RX flags to not to run again accidently
extern volatile bool packet_sending;
extern volatile bool rx_ended;
// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Application state machine, called infinitely, manages radio settings
 *
 * @param rail_handle: which rail instance to use
 * @return None.
 ******************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Set state machine state out of this file
 *
 * @param next_state: the next state the machine will be in
 * @return None.
 ******************************************************************************/
void set_next_state(state_t next_state);

/*******************************************************************************
 * Set the flag for proper sleep level.
 * @param None.
 * @return None.
 ******************************************************************************/
void init_em1_mode(void);

#endif // APP_PROCESS_H
