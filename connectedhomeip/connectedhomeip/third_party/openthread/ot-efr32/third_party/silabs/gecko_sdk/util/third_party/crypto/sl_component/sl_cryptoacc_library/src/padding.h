/**
 * @file
 * @brief Handles zero padding
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_PAD_H
#define SX_PAD_H

#include <stdint.h>
#include <stddef.h>
#include "cryptolib_types.h"

/**
 * Pads the hash of hashLen to EM of emLen. MSBs are set to 0
 * @param EM      Destination buffer (pointer)
 * @param emLen   Length of the destination buffer (bytes)
 * @param hash    Input to pad
 * @param hashLen Length of the input
 */
void pad_zeros(uint8_t *EM, size_t emLen, uint8_t *hash, size_t hashLen);

/**
 * Pads with zeroes for blk
 * @param out     block_t to the output buffer
 * @param in      inside block_t
 */
void pad_zeros_blk(block_t out, block_t in);

#endif
