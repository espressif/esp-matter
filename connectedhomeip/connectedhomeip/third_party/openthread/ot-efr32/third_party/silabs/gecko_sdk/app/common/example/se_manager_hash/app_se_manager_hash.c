/***************************************************************************//**
 * @file app_se_manager_hash.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_hash.h"

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

/// Streaming contexts for different Hash algorithms
static sl_se_sha1_multipart_context_t sha1_stream_ctx;
static sl_se_sha224_multipart_context_t sha224_stream_ctx;
static sl_se_sha256_multipart_context_t sha256_stream_ctx;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
static sl_se_sha384_multipart_context_t sha384_stream_ctx;
static sl_se_sha512_multipart_context_t sha512_stream_ctx;
#endif

/// Message length
static size_t msg_len;

/// Pointer to message buffer to generate hash value
static uint8_t *msg_buf_ptr;

/// Hash algorithm to be used
static sl_se_hash_type_t hash_type;

/// Buffer for hash value
static uint8_t hash_buf[HASH_BUF_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

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
 * Set Hash algorithm.
 ******************************************************************************/
void set_hash_type(sl_se_hash_type_t type)
{
  hash_type = type;
}

/***************************************************************************//**
 * Get size of hash value.
 ******************************************************************************/
uint8_t get_hash_size(void)
{
  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      return(20);

    case SL_SE_HASH_SHA224:
      return(28);

    case SL_SE_HASH_SHA256:
      return(32);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      return(48);

    case SL_SE_HASH_SHA512:
      return(64);
#endif

    default:
      return 0;
  }
}

/***************************************************************************//**
 * Get hash value buffer pointer.
 ******************************************************************************/
uint8_t * get_hash_buf_ptr(void)
{
  return(hash_buf);
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(sl_se_get_random(&cmd_ctx, buf, size), &cmd_ctx);
}

/***************************************************************************//**
 * Compute a hash value of the message.
 ******************************************************************************/
sl_status_t compute_msg_hash(void)
{
  print_error_cycle(sl_se_hash(&cmd_ctx,
                               hash_type,
                               msg_buf_ptr,
                               msg_len,
                               hash_buf,
                               sizeof(hash_buf)), &cmd_ctx);
}

/***************************************************************************//**
 * Start a stream based on the Hash algorithm.
 ******************************************************************************/
sl_status_t start_generic_stream(void)
{
  // The command context (cmd_ctx) for streaming cannot be reused for other
  // mailbox operations until the streaming operation is finished (i.e. the
  // sl_se_hash_multipart_finish is called).
  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      print_error_cycle(sl_se_hash_multipart_starts(&sha1_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_type), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA224:
      print_error_cycle(sl_se_hash_multipart_starts(&sha224_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_type), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA256:
      print_error_cycle(sl_se_hash_multipart_starts(&sha256_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_type), &cmd_ctx);
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      print_error_cycle(sl_se_hash_multipart_starts(&sha384_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_type), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA512:
      print_error_cycle(sl_se_hash_multipart_starts(&sha512_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_type), &cmd_ctx);
      break;
#endif

    default:
      return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Start a stream using corresponding Hash streaming context.
 ******************************************************************************/
sl_status_t start_shax_stream(void)
{
  // The command context (cmd_ctx) for streaming cannot be reused for other
  // mailbox operations until the streaming operation is finished (i.e. the
  // sl_se_hash_multipart_finish is called).
  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      print_error_cycle(sl_se_hash_sha1_multipart_starts(&sha1_stream_ctx,
                                                         &cmd_ctx), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA224:
      print_error_cycle(sl_se_hash_sha224_multipart_starts(&sha224_stream_ctx,
                                                           &cmd_ctx), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA256:
      print_error_cycle(sl_se_hash_sha256_multipart_starts(&sha256_stream_ctx,
                                                           &cmd_ctx), &cmd_ctx);
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      print_error_cycle(sl_se_hash_sha384_multipart_starts(&sha384_stream_ctx,
                                                           &cmd_ctx), &cmd_ctx);
      break;

    case SL_SE_HASH_SHA512:
      print_error_cycle(sl_se_hash_sha512_multipart_starts(&sha512_stream_ctx,
                                                           &cmd_ctx), &cmd_ctx);
      break;
#endif

    default:
      return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Update a Hash stream.
 ******************************************************************************/
sl_status_t update_hash_stream(uint32_t offset)
{
  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      print_error_cycle(sl_se_hash_multipart_update(&sha1_stream_ctx,
                                                    &cmd_ctx,
                                                    msg_buf_ptr + offset,
                                                    msg_len), NULL);
      break;

    case SL_SE_HASH_SHA224:
      print_error_cycle(sl_se_hash_multipart_update(&sha224_stream_ctx,
                                                    &cmd_ctx,
                                                    msg_buf_ptr + offset,
                                                    msg_len), NULL);
      break;

    case SL_SE_HASH_SHA256:
      print_error_cycle(sl_se_hash_multipart_update(&sha256_stream_ctx,
                                                    &cmd_ctx,
                                                    msg_buf_ptr + offset,
                                                    msg_len), NULL);
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      print_error_cycle(sl_se_hash_multipart_update(&sha384_stream_ctx,
                                                    &cmd_ctx,
                                                    msg_buf_ptr + offset,
                                                    msg_len), NULL);
      break;

    case SL_SE_HASH_SHA512:
      print_error_cycle(sl_se_hash_multipart_update(&sha512_stream_ctx,
                                                    &cmd_ctx,
                                                    msg_buf_ptr + offset,
                                                    msg_len), NULL);
      break;
#endif

    default:
      return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Finish a Hash stream.
 ******************************************************************************/
sl_status_t finish_hash_stream(void)
{
  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      print_error_cycle(sl_se_hash_multipart_finish(&sha1_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_buf,
                                                    sizeof(hash_buf)), NULL);
      break;

    case SL_SE_HASH_SHA224:
      print_error_cycle(sl_se_hash_multipart_finish(&sha224_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_buf,
                                                    sizeof(hash_buf)), NULL);
      break;

    case SL_SE_HASH_SHA256:
      print_error_cycle(sl_se_hash_multipart_finish(&sha256_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_buf,
                                                    sizeof(hash_buf)), NULL);
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      print_error_cycle(sl_se_hash_multipart_finish(&sha384_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_buf,
                                                    sizeof(hash_buf)), NULL);
      break;

    case SL_SE_HASH_SHA512:
      print_error_cycle(sl_se_hash_multipart_finish(&sha512_stream_ctx,
                                                    &cmd_ctx,
                                                    hash_buf,
                                                    sizeof(hash_buf)), NULL);
      break;
#endif

    default:
      return SL_STATUS_FAIL;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
