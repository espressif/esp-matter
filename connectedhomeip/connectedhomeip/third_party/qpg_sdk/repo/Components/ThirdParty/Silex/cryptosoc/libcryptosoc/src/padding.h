/**
 * @file
 * @brief Handles zero padding
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_PAD_H_
#define SX_PAD_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "padding_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include <stddef.h>
#include "cryptodma.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_padding.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_padding.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

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

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
