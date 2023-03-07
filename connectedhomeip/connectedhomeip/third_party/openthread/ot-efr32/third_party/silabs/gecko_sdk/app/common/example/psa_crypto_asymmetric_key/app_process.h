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
  SELECT_KEY_CURVE,
  SELECT_SECPR1_SIZE,
  SELECT_MONTGOMERY_SIZE,
  SELECT_KEY_USAGE,
  CREATE_KEY,
  IMPORT_PRIVATE_KEY,
  EXPORT_PRIVATE_KEY,
  EXPORT_PUBLIC_KEY,
  COPY_KEY,
  IMPORT_PUBLIC_KEY,
  DESTROY_KEY,
  PSA_CRYPTO_EXIT
} state_t;

/// Option selection maximum values
#if defined(SEMAILBOX_PRESENT)
#define KEY_CURVE_MAX           (2)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define KEY_STORAGE_MAX         PERSISTENT_WRAP_KEY
#else
#define KEY_STORAGE_MAX         PERSISTENT_PLAIN_KEY
#endif
#else
#define KEY_STORAGE_MAX         PERSISTENT_PLAIN_KEY
#define KEY_CURVE_MAX           (1)
#endif
#define SECPR1_SIZE_MAX         (3)
#define KEY_USAGE_MAX           (2)
#define MONTGOMERY_SIZE_MAX     (1)

/// Default key usage is none
#define DEFAULT_KEY_USAGE       (0)

/// Persistent key ID
#define PERSISTENT_KEY_ID       PSA_KEY_ID_USER_MIN

/// Persistent copied key ID
#define PERSISTENT_COPY_KEY_ID  (PSA_KEY_ID_USER_MIN + 1)

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
