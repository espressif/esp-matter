/***************************************************************************//**
 * @file app_se_manager_ecjpake.h
 * @brief SE manager ECJPAKE functions.
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
#ifndef APP_SE_MANAGER_ECJPAKE_H
#define APP_SE_MANAGER_ECJPAKE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_key_derivation.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Handshake buffer size
#define HANDSHAKE_BUF_SIZE      (512)

/// Pre-master secret size
#define PRE_MASTER_SECRET_SIZE  (32)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get pre-master secret buffer pointer.
 *
 * @returns Returns pointer to pre-master secret buffer.
 ******************************************************************************/
uint8_t * get_pre_master_secret_buf_ptr(void);

/***************************************************************************//**
 * Get pre-master secret length.
 *
 * @returns Returns pre-master secret length.
 ******************************************************************************/
size_t get_pre_master_secret_len(void);

/***************************************************************************//**
 * Set pointer to pre-shared secret.
 *
 * @param ptr The pointer to pre-shared secret.
 ******************************************************************************/
void set_pre_shared_secret_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set pre-shared secret length.
 *
 * @param length The length of the pre-shared secret.
 ******************************************************************************/
void set_pre_shared_secret_len(size_t length);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Denitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/**************************************************************************//**
 * Initialize an ECJPAKE client context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t init_ecjpake_client(void);

/**************************************************************************//**
 * Set up an ECJPAKE client context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t setup_ecjpake_client(void);

/**************************************************************************//**
 * Check an ECJPAKE client context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t check_client_ctx(void);

/**************************************************************************//**
 * Generate and write the first round client message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t write_client_round_one(void);

/**************************************************************************//**
 * Generate and write the second round client message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t write_client_round_two(void);

/**************************************************************************//**
 * Read and process the first round client message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t read_client_round_one(void);

/**************************************************************************//**
 * Read and process the second round client message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t read_client_round_two(void);

/**************************************************************************//**
 * Derive the client secret.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t derive_client_secret(void);

/**************************************************************************//**
 * Clear an ECJPAKE client context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t clear_client_ctx(void);

/**************************************************************************//**
 * Initialize an ECJPAKE server context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t init_ecjpake_server(void);

/**************************************************************************//**
 * Set up an ECJPAKE server context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t setup_ecjpake_server(void);

/**************************************************************************//**
 * Check an ECJPAKE server context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t check_server_ctx(void);

/**************************************************************************//**
 * Generate and write the first round server message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t write_server_round_one(void);

/**************************************************************************//**
 * Generate and write the second round server message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t write_server_round_two(void);

/**************************************************************************//**
 * Read and process the first round server message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t read_server_round_one(void);

/**************************************************************************//**
 * Read and process the second round server message.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t read_server_round_two(void);

/**************************************************************************//**
 * Derive the server secret.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t derive_server_secret(void);

/**************************************************************************//**
 * Clear an ECJPAKE server context.
 *
 * @returns Returns status code, @ref sl_status.h.
 *****************************************************************************/
sl_status_t clear_server_ctx(void);

#endif  // APP_SE_MANAGER_ECJPAKE_H
