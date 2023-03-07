/***************************************************************************//**
 * @file app_psa_crypto_kdf.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_kdf.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// The state object for key derivation operations
static psa_key_derivation_operation_t kdf_op;

/// Pointer to input data buffer
static uint8_t *input_buf_ptr;

/// Input data length in bytes
static size_t input_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get key derivation operation object pointer.
 ******************************************************************************/
psa_key_derivation_operation_t * get_kdf_op_ptr(void)
{
  return(&kdf_op);
}

/***************************************************************************//**
 * Set pointer to input data buffer.
 ******************************************************************************/
void set_input_buf_ptr(uint8_t *ptr)
{
  input_buf_ptr = ptr;
}

/***************************************************************************//**
 * Set input data length.
 ******************************************************************************/
void set_input_len(uint8_t length)
{
  input_len = length;
}

/***************************************************************************//**
 * Setup a key derivation operation.
 ******************************************************************************/
psa_status_t setup_key_derivation(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  kdf_op = psa_key_derivation_operation_init();
  print_error_cycle(psa_key_derivation_setup(&kdf_op, algo));
}

/***************************************************************************//**
 * Abort a key derivation operation.
 ******************************************************************************/
psa_status_t abort_key_derivation(void)
{
  print_error_cycle(psa_key_derivation_abort(&kdf_op));
}

/***************************************************************************//**
 * Set the maximum capacity of a key derivation operation.
 ******************************************************************************/
psa_status_t set_key_derivation_capacity(size_t capacity)
{
  print_error_cycle(psa_key_derivation_set_capacity(&kdf_op, capacity));
}

/***************************************************************************//**
 * Provide an input for key derivation.
 ******************************************************************************/
psa_status_t provide_key_derivation_input(psa_key_derivation_step_t step)
{
  print_error_cycle(psa_key_derivation_input_bytes(&kdf_op,
                                                   step,
                                                   input_buf_ptr,
                                                   input_len));
}

/***************************************************************************//**
 * Provide a key for key derivation.
 ******************************************************************************/
psa_status_t provide_key_derivation_input_key(psa_key_derivation_step_t step)
{
  print_error_cycle(psa_key_derivation_input_key(&kdf_op,
                                                 step,
                                                 get_key_id()));
}

/***************************************************************************//**
 * Perform a ECDH and use the shared secret as input to a key derivation.
 ******************************************************************************/
psa_status_t derive_key_agreement(psa_key_derivation_step_t step)
{
  print_error_cycle(psa_key_derivation_key_agreement(&kdf_op,
                                                     step,
                                                     get_key_id(),
                                                     input_buf_ptr,
                                                     input_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
