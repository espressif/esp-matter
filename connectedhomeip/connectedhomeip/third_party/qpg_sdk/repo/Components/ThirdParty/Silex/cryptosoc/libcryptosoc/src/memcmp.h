#ifndef MEMCMP_H_
#define MEMCMP_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "memcmp_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "cryptodma.h"
#include "cryptolib_types.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_memcmp.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_memcmp.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * Compares two buffer in time constant
 * @param  in1  pointer to the first buffer
 * @param  in2  pointer to the second buffer
 * @param  size Size to compare
 * @return      0 if the two buffers are the same 1 otherwize
 */
uint32_t memcmp_time_cst(uint8_t *in1, uint8_t *in2, uint32_t size);

/**
 * Compares two block_t in time constant on 32-bit words. Supports
 * constant addresses (FIFO)
 * @param  in1  block_t of first buffer
 * @param  in2  block_t of second buffer
 * @param  size Size to compare in bytes
 * @return      0 if the two buffers are the same 1 otherwize
 */
uint32_t memcmp32_blk_time_cst(block_t in1, block_t in2, uint32_t size);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)
#endif
