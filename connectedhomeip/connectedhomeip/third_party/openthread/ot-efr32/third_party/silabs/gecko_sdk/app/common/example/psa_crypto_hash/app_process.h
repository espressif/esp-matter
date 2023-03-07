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
#include "app_psa_crypto_hash.h"
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// State machine states
typedef enum {
  PSA_CRYPTO_IDLE,
  PSA_CRYPTO_INIT,
  SELECT_DATA_SIZE,
  SELECT_HASH_ALGO,
  COMPUTE_HASH,
  COMPARE_HASH,
  START_HASH_STREAM,
  UPDATE_HASH_STREAM,
  UPDATE_LAST_HASH_STREAM,
  FINISH_HASH_STREAM,
  VERIFY_HASH_STREAM,
  PSA_CRYPTO_EXIT
} state_t;

/// Option selection maximum values
#define HASH_ALGO_MAX           (4)
#define MSG_SIZE_MAX            (2)

/// Message buffer size
#define MSG_SIZE                (4096)

/// Stream block size
#define STREAM_BLOCK_SIZE       (16)

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
