/***************************************************************************//**
 * @file app_psa_crypto_kdf.h
 * @brief PSA Crypto key derivation functions.
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
#ifndef APP_PSA_CRYPTO_KDF_H
#define APP_PSA_CRYPTO_KDF_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get key derivation operation object pointer.
 *
 * @returns Returns pointer to key derivation operation object.
 ******************************************************************************/
psa_key_derivation_operation_t * get_kdf_op_ptr(void);

/***************************************************************************//**
 * Set pointer to input data buffer.
 *
 * @param ptr The pointer to input data buffer.
 ******************************************************************************/
void set_input_buf_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set input data length.
 *
 * @param length The length of input data.
 ******************************************************************************/
void set_input_len(uint8_t length);

/***************************************************************************//**
 * Setup a key derivation operation.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t setup_key_derivation(void);

/***************************************************************************//**
 * Abort a key derivation operation.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t abort_key_derivation(void);

/***************************************************************************//**
 * Set the maximum capacity of a key derivation operation.
 *
 * @param capacity The capacity for key derivation.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t set_key_derivation_capacity(size_t capacity);

/***************************************************************************//**
 * Provide an input for key derivation.
 *
 * @param step The step of input data.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t provide_key_derivation_input(psa_key_derivation_step_t step);

/***************************************************************************//**
 * Provide a key for key derivation.
 *
 * @param step The step of input data.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t provide_key_derivation_input_key(psa_key_derivation_step_t step);

/***************************************************************************//**
 * Perform a ECDH and use the shared secret as input to a key derivation.
 *
 * @param step The step of input data.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t derive_key_agreement(psa_key_derivation_step_t step);

#endif  // APP_PSA_CRYPTO_KDF_H
