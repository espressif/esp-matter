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
#include "app_psa_crypto_cipher.h"
#include "app_psa_crypto_key.h"
#include <string.h>
#include "nvm3.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// State machine states
typedef enum {
  PSA_CRYPTO_IDLE,
  PSA_CRYPTO_INIT,
  SELECT_KEY_STORAGE,
  SELECT_KEY_SIZE,
  SELECT_DATA_SIZE,
  AES_ECB_TEST,
  AES_CBC_TEST,
  AES_CFB_TEST,
  AES_CTR_TEST,
  CHACHA20_TEST,
  BUILTIN_KEY_ENCRYPTION,
  BUILTIN_KEY_DECRYPTION,
  START_CFB_STREAM,
  UPDATE_CFB_STREAM,
  UPDATE_LAST_CFB_STREAM,
  FINISH_CFB_STREAM,
  START_CTR_STREAM,
  UPDATE_CTR_STREAM,
  UPDATE_LAST_CTR_STREAM,
  FINISH_CTR_STREAM,
  START_CHACHA20_STREAM,
  UPDATE_CHACHA20_STREAM,
  UPDATE_LAST_CHACHA20_STREAM,
  FINISH_CHACHA20_STREAM,
  PSA_CRYPTO_EXIT
} state_t;

/// Option selection maximum values
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define KEY_STORAGE_MAX         (PERSISTENT_WRAP_KEY + 1)
#else
#if defined(SEMAILBOX_PRESENT)
#define KEY_STORAGE_MAX         (PERSISTENT_PLAIN_KEY + 1)
#else
#define KEY_STORAGE_MAX         PERSISTENT_PLAIN_KEY
#endif
#endif
#define KEY_SIZE_MAX            (2)
#define MSG_SIZE_MAX            (2)

/// Default key usage is none
#define DEFAULT_KEY_USAGE       (0)

/// Persistent key ID
#define PERSISTENT_KEY_ID       PSA_KEY_ID_USER_MIN

/// Stream block size
#define STREAM_BLOCK_SIZE       (14)

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
