/***************************************************************************//**
 * @file app_process.h
 * @brief Top level application functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "app_se_manager_ecdh.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// State machine states
typedef enum {
  SE_MANAGER_IDLE,
  SE_MANAGER_INIT,
  SELECT_KEY_TYPE,
  SELECT_KEY_ALGO,
  SELECT_MONTGOMERY_KEY,
  SELECT_WEIERSTRASS_KEY,
  CREATE_CLIENT_PLAIN_KEY,
  EXPORT_CLIENT_PLAIN_PUBLIC_KEY,
  CREATE_CLIENT_WRAP_KEY,
  EXPORT_CLIENT_WRAP_PUBLIC_KEY,
  CREATE_CLIENT_VOLATILE_KEY,
  EXPORT_CLIENT_VOLATILE_PUBLIC_KEY,
  DELETE_CLIENT_VOLATILE_KEY,
  CREATE_SERVER_PLAIN_KEY,
  EXPORT_SERVER_PLAIN_PUBLIC_KEY,
  CREATE_SERVER_WRAP_KEY,
  EXPORT_SERVER_WRAP_PUBLIC_KEY,
  CREATE_SERVER_VOLATILE_KEY,
  EXPORT_SERVER_VOLATILE_PUBLIC_KEY,
  DELETE_SERVER_VOLATILE_KEY,
  COMPUTE_CLIENT_SHARED_SECRET,
  COMPUTE_SERVER_SHARED_SECRET,
  SE_MANAGER_EXIT
} state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void);

#endif  // APP_PROCESS_H
