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
#include "app_se_manager_key_provisioning.h"
#include "sli_se_manager_internal.h"
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
  READ_SE_VERSION,
  GET_OTP_CONF,
  CHECK_EXECUTED_COMMAND,
  PRE_INIT_AES_KEY,
  INIT_AES_KEY,
  CHECK_AES_KEY,
  PRE_INIT_PUB_SIGN_KEY,
  INIT_PUB_SIGN_KEY,
  POST_INIT_PUB_SIGN_KEY,
  GET_PUB_SIGN_KEY,
  POST_GET_PUB_SIGN_KEY,
  PRE_INIT_PUB_CMD_KEY,
  INIT_PUB_CMD_KEY,
  POST_INIT_PUB_CMD_KEY,
  GET_PUB_CMD_KEY,
  POST_GET_PUB_CMD_KEY,
  PRE_INIT_OTP,
  INIT_OTP,
  POST_INIT_OTP,
  SE_MANAGER_EXIT
} state_t;

/// Response buffer size
#define RESP_BUF_SIZE   (128)

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
