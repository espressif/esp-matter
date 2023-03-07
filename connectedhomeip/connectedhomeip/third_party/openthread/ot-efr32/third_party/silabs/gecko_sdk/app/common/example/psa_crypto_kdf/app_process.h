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
#include "app_psa_crypto_kdf.h"
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
  SELECT_HASH_ALGO,
  IMPORT_MASTER_KEY,
  SETUP_HKDF,
  SET_CAPACITY,
  INPUT_SALT,
  INPUT_KEY,
  INPUT_INFO,
  DERIVE_HKDF_KEY,
  SL_HKDF_SINGLE,
  EXPORT_HKDF_KEY,
  COMPARE_HKDF_KEY,
  DESTROY_MASTER_KEY,
  DESTROY_HKDF_KEY,
  PSA_CRYPTO_EXIT
} state_t;

/// Option selection maximum values
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define KEY_STORAGE_MAX         PERSISTENT_WRAP_KEY
#else
#define KEY_STORAGE_MAX         PERSISTENT_PLAIN_KEY
#endif
#define HASH_ALGO_MAX           (4)
#define KEY_SIZE_MAX            (2)

/// Default key usage is none
#define DEFAULT_KEY_USAGE       (0)

/// Derive key algorithm
#define DERIVE_KEY_ALGO         PSA_ALG_CTR

/// Master key ID
#define MASTER_KEY_ID           PSA_KEY_ID_USER_MIN

/// Derive key ID
#define DERIVE_KEY_ID           (PSA_KEY_ID_USER_MIN + 1)

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
