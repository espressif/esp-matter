/***************************************************************************//**
 * @file network_management.h
 * @brief network_management.h
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef NETWORK_MANAGEMENT_H
#define NETWORK_MANAGEMENT_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef enum
{
  NETWORK_MANAGEMENT_STATE_IDLE,
  NETWORK_MANAGEMENT_STATE_START_INCLUSION,
  NETWORK_MANAGEMENT_STATE_START_EXCLUSION,
  NETWORK_MANAGEMENT_STATE_ADD_GROUP,
  NETWORK_MANAGEMENT_STATE_REMOVE_GROUP,
  NETWORK_MANAGEMENT_STATE_LEARNMODE,
  NETWORK_MANAGEMENT_STATE_SECURITY_PROBE,
  NETWORK_MANAGEMENT_STATE_S2_PROBE,
  NETWORK_MANAGEMENT_STATE_S0_PROBE
} sl_network_management_states_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

bool key_fob_start_inclusion();

bool key_fob_start_exclusion();

bool key_fob_stop_exclusion();

bool key_fob_stop_inclusion();

bool key_fob_start_network_learnmode();

bool key_fob_stop_network_learnmode();

bool set_new_network_management_state( sl_network_management_states_t new_state);

sl_network_management_states_t get_current_network_management_state();


#endif  // NETWORK_MANAGEMENT_H
