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
#include "app_se_manager_stream_cipher.h"
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
  START_CMAC_STREAM,
  UPDATE_CMAC_STREAM,
  UPDATE_LAST_CMAC_STREAM,
  FINISH_CMAC_STREAM,
  START_GCM_ENCRYPT_STREAM,
  UPDATE_GCM_ENCRYPT_STREAM,
  UPDATE_LAST_GCM_ENCRYPT_STREAM,
  FINISH_GCM_ENCRYPT_STREAM,
  START_GCM_DECRYPT_STREAM,
  UPDATE_GCM_DECRYPT_STREAM,
  UPDATE_LAST_GCM_DECRYPT_STREAM,
  FINISH_GCM_DECRYPT_STREAM,
  START_CHACHA20_ENCRYPT_STREAM,
  UPDATE_CHACHA20_ENCRYPT_STREAM,
  FINISH_CHACHA20_ENCRYPT_STREAM,
  START_CHACHA20_DECRYPT_STREAM,
  UPDATE_CHACHA20_DECRYPT_STREAM,
  FINISH_CHACHA20_DECRYPT_STREAM,
  SE_MANAGER_EXIT
} state_t;

/// CMAC stream block size
#define CMAC_BLCOK_SIZE         (16)

/// CHACHA20 stream block size
#define CHACHA20_BLOCK_SIZE     (64)

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
