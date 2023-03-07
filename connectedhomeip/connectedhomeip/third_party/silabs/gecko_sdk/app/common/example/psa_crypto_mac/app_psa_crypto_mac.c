/***************************************************************************//**
 * @file app_psa_crypto_mac.c
 * @brief PSA Crypto MAC functions.
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
#include "app_psa_crypto_mac.h"

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
/// The state objects for multi-part MAC operations
static psa_mac_operation_t mac_op;

/// Message length
static size_t msg_len;

/// MAC length
static size_t mac_len;

/// Pointer to message buffer to generate MAC value
static uint8_t *msg_buf_ptr;

/// Buffer for MAC value
static uint8_t mac_buf[MAC_BUF_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get MAC value buffer pointer.
 ******************************************************************************/
uint8_t * get_mac_buf_ptr(void)
{
  return(mac_buf);
}

/***************************************************************************//**
 * Get length of MAC value.
 ******************************************************************************/
uint8_t get_mac_len(void)
{
  return(mac_len);
}

/***************************************************************************//**
 * Set pointer to message buffer.
 ******************************************************************************/
void set_msg_buf_ptr(uint8_t *ptr)
{
  msg_buf_ptr = ptr;
}

/***************************************************************************//**
 * Set message length.
 ******************************************************************************/
void set_msg_len(size_t length)
{
  msg_len = length;
}

/***************************************************************************//**
 * Set length of MAC value.
 ******************************************************************************/
void set_mac_len(uint8_t length)
{
  mac_len = length;
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(psa_generate_random(buf, size));
}

/***************************************************************************//**
 * Compute the MAC of a message.
 ******************************************************************************/
psa_status_t compute_mac(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_mac_compute(get_key_id(),
                                    algo,
                                    msg_buf_ptr,
                                    msg_len,
                                    mac_buf,
                                    sizeof(mac_buf),
                                    &mac_len));
}

/***************************************************************************//**
 * Compute the MAC of a message and compare it with an expected value.
 ******************************************************************************/
psa_status_t verify_mac(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_mac_verify(get_key_id(),
                                   algo,
                                   msg_buf_ptr,
                                   msg_len,
                                   mac_buf,
                                   mac_len));
}

/***************************************************************************//**
 * Start a MAC sign stream.
 ******************************************************************************/
psa_status_t start_mac_sign_stream(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  mac_op = psa_mac_operation_init();
  print_error_cycle(psa_mac_sign_setup(&mac_op, get_key_id(), algo));
}

/***************************************************************************//**
 * Start a MAC verify stream.
 ******************************************************************************/
psa_status_t start_mac_verify_stream(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  mac_op = psa_mac_operation_init();
  print_error_cycle(psa_mac_verify_setup(&mac_op, get_key_id(), algo));
}

/***************************************************************************//**
 * Update a MAC stream.
 ******************************************************************************/
psa_status_t update_mac_stream(uint32_t offset)
{
  print_error_cycle(psa_mac_update(&mac_op, msg_buf_ptr + offset, msg_len));
}

/***************************************************************************//**
 * Finish a MAC sign stream.
 ******************************************************************************/
psa_status_t finish_mac_sign_stream(void)
{
  print_error_cycle(psa_mac_sign_finish(&mac_op,
                                        mac_buf,
                                        sizeof(mac_buf),
                                        &mac_len));
}

/***************************************************************************//**
 * Finish a MAC verify stream.
 ******************************************************************************/
psa_status_t finish_mac_verify_stream(void)
{
  print_error_cycle(psa_mac_verify_finish(&mac_op, mac_buf, mac_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
