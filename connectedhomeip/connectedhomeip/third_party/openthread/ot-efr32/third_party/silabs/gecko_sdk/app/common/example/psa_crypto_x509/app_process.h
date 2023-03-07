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
#include "app_mbedtls_x509.h"
#include "app_psa_crypto_key.h"
#include "nvm3.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// State machine states
typedef enum {
  PSA_CRYPTO_IDLE,
  PSA_CRYPTO_INIT,
  SELECT_KEY_STORAGE,
  SELECT_SECPR1_SIZE,
  SELECT_HASH_ALGO,
  INIT_ROOT_CSR,
  CREATE_ROOT_KEY,
  WRAP_ROOT_PSA_KEY,
  WRITE_ROOT_CSR_PEM,
  LOAD_ROOT_CSR,
  INIT_ROOT_CERT,
  SET_ROOT_ISSUER,
  SET_ROOT_SUBJECT,
  SET_ROOT_PARAMETER,
  SET_ROOT_SERIAL,
  SET_ROOT_VALIDITY,
  SET_ROOT_BASIC_CONSTRAINT,
  SET_ROOT_KEY_USAGE,
  SET_ROOT_CERT_TYPE,
  SET_ROOT_SUBJECT_KEY_EXT,
  SET_ROOT_AUTH_KEY_EXT,
  WRITE_ROOT_CRT_PEM,
  PARSE_ROOT_CERT,
  STORE_ROOT_DN,
  INIT_DEVICE_CSR,
  CREATE_DEVICE_KEY,
  WRAP_DEVICE_PSA_KEY,
  WRITE_DEVICE_CSR_PEM,
  LOAD_DEVICE_CSR,
  INIT_DEVICE_CERT,
  SET_DEVICE_ISSUER,
  SET_DEVICE_SUBJECT,
  SET_DEVICE_PARAMETER,
  SET_DEVICE_SERIAL,
  SET_DEVICE_VALIDITY,
  SET_DEVICE_BASIC_CONSTRAINT,
  SET_DEVICE_KEY_USAGE,
  SET_DEVICE_CERT_TYPE,
  SET_DEVICE_SUBJECT_KEY_EXT,
  SET_DEVICE_AUTH_KEY_EXT,
  WRITE_DEVICE_CRT_PEM,
  VERIFY_CERT_CHAIN,
  DESTROY_ROOT_KEY,
  DESTROY_DEVICE_KEY,
  PSA_CRYPTO_EXIT
} state_t;

/// Option selection maximum values
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define KEY_STORAGE_MAX         (PERSISTENT_WRAP_KEY + 4)
#else
#define KEY_STORAGE_MAX         PERSISTENT_PLAIN_KEY
#endif
#define SECPR1_SIZE_MAX         (3)
#define HASH_ALGO_MAX           (3)

/// Default key usage is ECDSA sigan and verify hash
#define DEFAULT_KEY_USAGE       (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH)

/// Persistent root certificate key ID
#define ROOT_KEY_ID             PSA_KEY_ID_USER_MIN

/// Persistent device certificate key ID
#define DEVICE_KEY_ID           (PSA_KEY_ID_USER_MIN + 1)

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
