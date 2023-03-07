/***************************************************************************//**
 * @file app_se_manager_hash.h
 * @brief SE manager hash functions.
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
#ifndef APP_SE_MANAGER_HASH_H
#define APP_SE_MANAGER_HASH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_hash.h"
#include "sl_se_manager_entropy.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Hash value buffer size
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define HASH_BUF_SIZE   (64)
#else
#define HASH_BUF_SIZE   (32)
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

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
 * Set Hash algorithm.
 *
 * @param type The Hash algorithm to be used.
 ******************************************************************************/
void set_hash_type(sl_se_hash_type_t type);

/***************************************************************************//**
 * Get size of hash value.
 *
 * @returns Returns size of hash value.
 ******************************************************************************/
uint8_t get_hash_size(void);

/***************************************************************************//**
 * Get hash value buffer pointer.
 *
 * @returns Returns pointer to the hash value buffer.
 ******************************************************************************/
uint8_t * get_hash_buf_ptr(void);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Deinitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Compute a hash value of the message.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t compute_msg_hash(void);

/***************************************************************************//**
 * Start a stream based on the Hash algorithm.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t start_generic_stream(void);

/***************************************************************************//**
 * Start a stream using corresponding Hash streaming context.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t start_shax_stream(void);

/***************************************************************************//**
 * Update a Hash stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_hash_stream(uint32_t offset);

/***************************************************************************//**
 * Finish a Hash stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t finish_hash_stream(void);

#endif  // APP_SE_MANAGER_HASH_H
