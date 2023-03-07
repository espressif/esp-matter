/***************************************************************************//**
 * @file app_psa_crypto_mac.h
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
#ifndef APP_PSA_CRYPTO_MAC_H
#define APP_PSA_CRYPTO_MAC_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// MAC value buffer size
#define MAC_BUF_SIZE   (64)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get MAC value buffer pointer.
 *
 * @returns Returns pointer to the MAC value buffer.
 ******************************************************************************/
uint8_t * get_mac_buf_ptr(void);

/***************************************************************************//**
 * Get length of MAC value.
 *
 * @returns Returns length of MAC value.
 ******************************************************************************/
uint8_t get_mac_len(void);

/***************************************************************************//**
 * Set pointer to message buffer.
 *
 * @param ptr The pointer to message buffer.
 ******************************************************************************/
void set_msg_buf_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set message length.
 *
 * @param length The length of the message.
 ******************************************************************************/
void set_msg_len(size_t length);

/***************************************************************************//**
 * Set length of MAC value.
 *
 * @param length The length of the MAC value.
 ******************************************************************************/
void set_mac_len(uint8_t length);

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Compute the MAC of the message.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t compute_mac(void);

/***************************************************************************//**
 * Compute the MAC of a message and compare it with an expected value.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t verify_mac(void);

/***************************************************************************//**
 * Start a MAC sign stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t start_mac_sign_stream(void);

/***************************************************************************//**
 * Start a MAC verify stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t start_mac_verify_stream(void);

/***************************************************************************//**
 * Update a MAC stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t update_mac_stream(uint32_t offset);

/***************************************************************************//**
 * Finish a MAC sign stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t finish_mac_sign_stream(void);

/***************************************************************************//**
 * Finish a MAC verify stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t finish_mac_verify_stream(void);

#endif  // APP_PSA_CRYPTO_MAC_H
