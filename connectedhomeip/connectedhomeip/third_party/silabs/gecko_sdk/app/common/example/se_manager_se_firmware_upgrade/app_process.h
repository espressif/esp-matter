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
#include "app_se_manager_se_firmware_upgrade.h"
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
  CHECK_EXECUTED_COMMAND,
  GET_SE_UPGRADE_STATUS,
  POST_SE_UPGRADE_STATUS,
  VALIDATE_SE_IMAGE,
  POST_VALIDATE_SE_IMAGE,
  UPGRADE_SE_IMAGE,
  POST_UPGRADE_SE_IMAGE,
  SE_MANAGER_EXIT
} state_t;

/// Response buffer size
#define RESP_BUF_SIZE   (128)

/// String for SE
#if defined(CRYPTOACC_PRESENT)
#define S2      "VSE"
#else
#define S2      "HSE"
#endif

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
