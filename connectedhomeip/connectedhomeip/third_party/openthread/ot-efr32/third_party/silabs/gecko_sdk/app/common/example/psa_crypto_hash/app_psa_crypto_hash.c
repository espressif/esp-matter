/***************************************************************************//**
 * @file app_psa_crypto_hash.c
 * @brief PSA crypto hash functions.
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
#include "app_psa_crypto_hash.h"

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
/// The state objects for multi-part hash operations
static psa_hash_operation_t hash_op;
static psa_hash_operation_t verify_hash_op;

/// Message length
static size_t msg_len;

/// Hash length
static size_t hash_len;

/// Pointer to message buffer to generate hash value
static uint8_t *msg_buf_ptr;

/// Hash algorithm to be used
static psa_algorithm_t hash_algo;

/// Buffer for hash value
static uint8_t hash_buf[HASH_BUF_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get hash value buffer pointer.
 ******************************************************************************/
uint8_t * get_hash_buf_ptr(void)
{
  return(hash_buf);
}

/***************************************************************************//**
 * Get size of hash value.
 ******************************************************************************/
uint8_t get_hash_size(void)
{
  return(hash_len);
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
 * Set hash algorithm.
 ******************************************************************************/
void set_hash_algo(psa_algorithm_t type)
{
  hash_algo = type;
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(psa_generate_random(buf, size));
}

/***************************************************************************//**
 * Compute the hash of the message.
 ******************************************************************************/
psa_status_t compute_msg_hash(void)
{
  print_error_cycle(psa_hash_compute(hash_algo,
                                     msg_buf_ptr,
                                     msg_len,
                                     hash_buf,
                                     sizeof(hash_buf),
                                     &hash_len));
}

/***************************************************************************//**
 * Compute the hash of a message and compare it with an expected value.
 ******************************************************************************/
psa_status_t compare_msg_hash(void)
{
  print_error_cycle(psa_hash_compare(hash_algo,
                                     msg_buf_ptr,
                                     msg_len,
                                     hash_buf,
                                     hash_len));
}

/***************************************************************************//**
 * Start a hash stream.
 ******************************************************************************/
psa_status_t start_hash_stream(void)
{
  hash_op = psa_hash_operation_init();
  print_error_cycle(psa_hash_setup(&hash_op, hash_algo));
}

/***************************************************************************//**
 * Update a hash stream.
 ******************************************************************************/
psa_status_t update_hash_stream(uint32_t offset)
{
  print_error_cycle(psa_hash_update(&hash_op, msg_buf_ptr + offset, msg_len));
}

/***************************************************************************//**
 * Finish a hash stream.
 ******************************************************************************/
psa_status_t finish_hash_stream(void)
{
  print_error_cycle(psa_hash_finish(&hash_op,
                                    hash_buf,
                                    sizeof(hash_buf),
                                    &hash_len));
}

/***************************************************************************//**
 * Clone a hash operation.
 ******************************************************************************/
psa_status_t clone_hash_operation(void)
{
  verify_hash_op = psa_hash_operation_init();
  print_error_cycle(psa_hash_clone(&hash_op, &verify_hash_op));
}

/***************************************************************************//**
 * Compare the hash with an expected value.
 ******************************************************************************/
psa_status_t verify_hash_stream(void)
{
  print_error_cycle(psa_hash_verify(&verify_hash_op,
                                    hash_buf,
                                    hash_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
