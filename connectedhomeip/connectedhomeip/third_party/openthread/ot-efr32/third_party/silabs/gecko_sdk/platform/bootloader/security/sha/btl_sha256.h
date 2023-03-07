/***************************************************************************//**
 * @file
 * @brief Generic SHA-256 functionality
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/
#ifndef BTL_SHA256_H
#define BTL_SHA256_H

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup SHA256
 * @brief SHA-256 Cryptography Library
 * @details
 *   This file includes an alternative implementation of the standard
 *   mbed TLS SHA using hardware accelerator incorporated in MCU devices from
 *   Silicon Labs.
 * @{
 ******************************************************************************/
/// Context Variable type for SHA-256 Cryptography
typedef struct {
  uint32_t total[2];              /*!< number of bytes processed  */
  uint32_t state[8];              /*!< intermediate digest state  */
  unsigned char buffer[64];       /*!< data block being processed */
}
btl_sha256_context;

/// Type variable for SHA-256 Cryptography
typedef enum {
  SHA256                          /*!< SHA-256 type */
} SHA_Type_t;



/**
 * \brief          Initialize SHA-256 context.
 *
 * \param ctx      SHA-256 context to be initialized
 */
void btl_sha256_init(btl_sha256_context *ctx);

/**
 * \brief          Set up SHA-256 context.
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 *
 * \return         \c 0 if successful
 *
 */
int btl_sha256_starts_ret(btl_sha256_context *ctx, int is224);

/**
 * \brief          SHA-256 process buffer.
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 *
 * \return         \c 0 if successful
 *
 */
int btl_sha256_update_ret(btl_sha256_context *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          SHA-256 final digest.
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 *
 * \return         \c 0 if successful
 *
 */
int btl_sha256_finish_ret(btl_sha256_context *ctx, unsigned char output[32]);

/**
 * \brief                   Process (a) block(s) of data to be hashed.
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 *
 * \param algo              Which hashing algorithm to use
 * \param[in] state_in      Previous state of the hashing algorithm
 * \param[in] blockdata     Pointer to the block(s) of data
 * \param[out] state_out    Pointer to block of memory to store state
 * \param num_blocks        Number of SHA blocks in data block
 * \returns                 Zero on success. Negative error code on failure.
 */
int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks);

/**
 * \brief                   Process an arbitrary number of bytes to be hashed.
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 * \note                    Watch the counter size! Counter is 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 *
 * \param algo              Which hashing algorithm to use
 * \param[inout] state      Pointer to the hashing algorithm's state buffer
 * \param[in] data          Pointer to the data to add to the hash
 * \param[inout] buffer     Pointer to a block buffer to retrieve/store a partial block
 *                          in between calls to this function. Needs to have a size equal
 *                          to the block size.
 * \param[inout] counter    Counter variable keeping track of the amount of bytes hashed,
 *                          to later be used for hash finalization. For first use, initialize
 *                          with zeroes.
 * \param[in] data_len      Length to data to add to hash
 * \returns                 Zero on success. Negative error code on failure.
 */
int sha_x_update(SHA_Type_t algo,
                 const unsigned char *data,
                 size_t data_len,
                 uint8_t* state,
                 unsigned char *buffer,
                 uint32_t* counter);

/**
 * \brief                   Process an arbitrary number of bytes to be hashed.
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 * \note                    Watch the counter size! Counter is 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 *
 * \param algo              Which hashing algorithm to use
 * \param[in] state         Pointer to the hashing algorithm's state buffer
 * \param[in] buffer        Pointer to a block buffer to retrieve/store a partial block
 *                          in between calls to this function. Needs to have a size equal
 *                          to the block size.
 * \param[inout] counter    Counter variable keeping track of the amount of bytes hashed,
 *                          to later be used for hash finalization. For first use, initialize
 *                          with zeroes.
 * \param[out] output       Pointer to the destination of the hash.
 * \returns                 Zero on success. Negative error code on failure.
 */
int sha_x_finish(SHA_Type_t algo,
                 uint8_t* state,
                 uint8_t* buffer,
                 uint32_t* counter,
                 uint8_t* output);


/** @} (end addtogroup SHA256) */
/** @} (end addtogroup Security) */
/** @} (end addtogroup Components) */

#endif // BTL_SHA256_H
