/***************************************************************************//**
 * @file app_se_manager_ecjpake.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_ecjpake.h"

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
/// Command context
static sl_se_command_context_t cmd_ctx;

/// Client ECJPAKE context
static sl_se_ecjpake_context_t client_ecjpake_ctx;

/// Server ECJPAKE context
static sl_se_ecjpake_context_t server_ecjpake_ctx;

/// Pointer to pre-shared secret
static uint8_t *pre_shared_secret;

/// Pre-shared secret length
static size_t pre_shared_secret_len;

/// Handshake buffer
static uint8_t handshake_buf[HANDSHAKE_BUF_SIZE];

/// Pre-master secret buffer
static uint8_t pre_master_secret_buf[PRE_MASTER_SECRET_SIZE];

/// Pre-master secret length
static size_t pre_master_secret_len;

/// Output message length
static size_t output_msg_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get pre-master secret buffer pointer.
 ******************************************************************************/
uint8_t * get_pre_master_secret_buf_ptr(void)
{
  return(pre_master_secret_buf);
}

/***************************************************************************//**
 * Get pre-master secret length.
 ******************************************************************************/
size_t get_pre_master_secret_len(void)
{
  return(pre_master_secret_len);
}

/***************************************************************************//**
 * Set pointer to pre-shared secret.
 ******************************************************************************/
void set_pre_shared_secret_ptr(uint8_t *ptr)
{
  pre_shared_secret = ptr;
}

/***************************************************************************//**
 * Set pre-shared secret length.
 ******************************************************************************/
void set_pre_shared_secret_len(size_t length)
{
  pre_shared_secret_len = length;
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Denitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/**************************************************************************//**
 * Initialize an ECJPAKE client context.
 *****************************************************************************/
sl_status_t init_ecjpake_client(void)
{
  print_error_cycle(sl_se_ecjpake_init(&client_ecjpake_ctx, &cmd_ctx),
                    &cmd_ctx);
}

/**************************************************************************//**
 * Set up an ECJPAKE client context.
 *****************************************************************************/
sl_status_t setup_ecjpake_client(void)
{
  // Currently the only values for hash and curve allowed by the standard are
  // SL_SE_HASH_SHA256 and SL_SE_KEY_TYPE_ECC_P256.
  print_error_cycle(sl_se_ecjpake_setup(&client_ecjpake_ctx,
                                        SL_SE_ECJPAKE_CLIENT,
                                        SL_SE_HASH_SHA256,
                                        SL_SE_KEY_TYPE_ECC_P256,
                                        pre_shared_secret,
                                        pre_shared_secret_len), NULL);
}

/**************************************************************************//**
 * Check an ECJPAKE client context.
 *****************************************************************************/
sl_status_t check_client_ctx(void)
{
  print_error_cycle(sl_se_ecjpake_check(&client_ecjpake_ctx), NULL);
}

/**************************************************************************//**
 * Generate and write the first round client message.
 *****************************************************************************/
sl_status_t write_client_round_one(void)
{
  print_error_cycle(sl_se_ecjpake_write_round_one(&client_ecjpake_ctx,
                                                  handshake_buf,
                                                  sizeof(handshake_buf),
                                                  &output_msg_len), NULL);
}

/**************************************************************************//**
 * Generate and write the second round client message.
 *****************************************************************************/
sl_status_t write_client_round_two(void)
{
  print_error_cycle(sl_se_ecjpake_write_round_two(&client_ecjpake_ctx,
                                                  handshake_buf,
                                                  sizeof(handshake_buf),
                                                  &output_msg_len), NULL);
}

/**************************************************************************//**
 * Read and process the first round client message.
 *****************************************************************************/
sl_status_t read_client_round_one(void)
{
  print_error_cycle(sl_se_ecjpake_read_round_one(&client_ecjpake_ctx,
                                                 handshake_buf,
                                                 output_msg_len), NULL);
}

/**************************************************************************//**
 * Read and process the second round client message.
 *****************************************************************************/
sl_status_t read_client_round_two(void)
{
  print_error_cycle(sl_se_ecjpake_read_round_two(&client_ecjpake_ctx,
                                                 handshake_buf,
                                                 output_msg_len), NULL);
}

/**************************************************************************//**
 * Derive the client secret.
 *****************************************************************************/
sl_status_t derive_client_secret(void)
{
  print_error_cycle(sl_se_ecjpake_derive_secret(&client_ecjpake_ctx,
                                                pre_master_secret_buf,
                                                sizeof(pre_master_secret_buf),
                                                &pre_master_secret_len), NULL);
}

/**************************************************************************//**
 * Clear an ECJPAKE client context.
 *****************************************************************************/
sl_status_t clear_client_ctx(void)
{
  print_error_cycle(sl_se_ecjpake_free(&client_ecjpake_ctx), NULL);
}

/**************************************************************************//**
 * Initialize an ECJPAKE server context.
 *****************************************************************************/
sl_status_t init_ecjpake_server(void)
{
  print_error_cycle(sl_se_ecjpake_init(&server_ecjpake_ctx, &cmd_ctx),
                    &cmd_ctx);
}

/**************************************************************************//**
 * Set up an ECJPAKE server context.
 *****************************************************************************/
sl_status_t setup_ecjpake_server(void)
{
  // Currently the only values for hash and curve allowed by the standard are
  // SL_SE_HASH_SHA256 and SL_SE_KEY_TYPE_ECC_P256.
  print_error_cycle(sl_se_ecjpake_setup(&server_ecjpake_ctx,
                                        SL_SE_ECJPAKE_SERVER,
                                        SL_SE_HASH_SHA256,
                                        SL_SE_KEY_TYPE_ECC_P256,
                                        pre_shared_secret,
                                        pre_shared_secret_len), NULL);
}

/**************************************************************************//**
 * Check an ECJPAKE server context.
 *****************************************************************************/
sl_status_t check_server_ctx(void)
{
  print_error_cycle(sl_se_ecjpake_check(&server_ecjpake_ctx), NULL);
}

/**************************************************************************//**
 * Generate and write the first round server message.
 *****************************************************************************/
sl_status_t write_server_round_one(void)
{
  print_error_cycle(sl_se_ecjpake_write_round_one(&server_ecjpake_ctx,
                                                  handshake_buf,
                                                  sizeof(handshake_buf),
                                                  &output_msg_len), NULL);
}

/**************************************************************************//**
 * Generate and write the second round server message.
 *****************************************************************************/
sl_status_t write_server_round_two(void)
{
  print_error_cycle(sl_se_ecjpake_write_round_two(&server_ecjpake_ctx,
                                                  handshake_buf,
                                                  sizeof(handshake_buf),
                                                  &output_msg_len), NULL);
}

/**************************************************************************//**
 * Read and process the first round server message.
 *****************************************************************************/
sl_status_t read_server_round_one(void)
{
  print_error_cycle(sl_se_ecjpake_read_round_one(&server_ecjpake_ctx,
                                                 handshake_buf,
                                                 output_msg_len), NULL);
}

/**************************************************************************//**
 * Read and process the second round server message.
 *****************************************************************************/
sl_status_t read_server_round_two(void)
{
  print_error_cycle(sl_se_ecjpake_read_round_two(&server_ecjpake_ctx,
                                                 handshake_buf,
                                                 output_msg_len), NULL);
}

/**************************************************************************//**
 * Derive the server secret.
 *****************************************************************************/
sl_status_t derive_server_secret(void)
{
  print_error_cycle(sl_se_ecjpake_derive_secret(&server_ecjpake_ctx,
                                                pre_master_secret_buf,
                                                sizeof(pre_master_secret_buf),
                                                &pre_master_secret_len), NULL);
}

/**************************************************************************//**
 * Clear an ECJPAKE server context.
 *****************************************************************************/
sl_status_t clear_server_ctx(void)
{
  print_error_cycle(sl_se_ecjpake_free(&server_ecjpake_ctx), NULL);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
