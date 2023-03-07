

#ifndef CRYPTOLIB_TYPES_H_
#define CRYPTOLIB_TYPES_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "cryptolib_types_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>

/**
* @brief Data block type for source & destination
*/
typedef struct block_s
{
   uint8_t *addr;    /**< Start address               */
   uint32_t len;     /**< Total number of data bytes  */
   uint32_t flags;   /**< Flags                       */
} block_t;

/**
 * Get the nth bit value of a uint8_t
 * @param  offset the bit number (LSB)
 * @param var Variable containing the boolean
 * @return        The bit value (on an uint32_t)
 */
#define GETBOOL(var, offset) (((var) >> (offset)) & 1)


/**
 * Swap bytes of a 32-bit word
 * @param  word   The 32-bit word to byte-swap
 * @return        The byte-swapped value
 */
#define SWAPBYTES32(word) ((((word) & 0xFF000000) >> 24) | (((word) & 0x00FF0000) >> 8) | (((word) & 0x0000FF00) << 8) | (((word) & 0x000000FF) << 24))

/**
 * Align on word boundary
 */
// moved to cryptolib_internal.h
// #define ALIGNED __attribute__((aligned(0x4)))


/** @brief value of ::block_s.flags to set addressing in constant mode (pointing to a FIFO) */
#define BLOCK_S_CONST_ADDR             0x10000000
/** @brief value of ::block_s.flags to set addressing in increment mode (pointing to a buffer) */
#define BLOCK_S_INCR_ADDR              0x00000000
/** @brief mask for ::block_s.flags to only get DMA-related options */
#define BLOCK_S_FLAG_MASK_DMA_PROPS    0x70000000



#define BLOCK_T_CONV(array, length) { (uint8_t *) (array), (uint32_t) (length), BLOCK_S_INCR_ADDR}

/**
 * Round-up integer to next 32-bit multiple
 * @param  value The value to round
 * @return       The rounded value
 */
#define roundup_32(value) (((value) + 3) & ~3)


#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_cryptolib_types.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_cryptolib_types.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Convert uint8 array to block_t
 * @param  array  Array
 * @param  length Length of array
 * @return block_t
 */
#ifdef GP_DIVERSITY_JUMPTABLES
block_t block_t_convert(const volatile void * array, uint32_t length);
#else   //def GP_DIVERSITY_JUMPTABLES
static inline block_t block_t_convert(const volatile void * array, uint32_t length)
{
#if defined __cplusplus
    //`compound literals` are not valid in C++
    block_t  blk = BLOCK_T_CONV(array, length);
    return blk;
#else
    //`compound literal` (used below) is valid in C99
    return (block_t)BLOCK_T_CONV(array, length);
#endif
}
#endif  //def GP_DIVERSITY_JUMPTABLES

/**
 * Round-up the length of a block_t to 32-bit if the address is a FIFO
 * @param  blk    A pointer to the block_t to adapt
 */
#ifdef GP_DIVERSITY_JUMPTABLES
void block_t_adapt_len(block_t * blk);
#else   //def GP_DIVERSITY_JUMPTABLES
static inline void block_t_adapt_len(block_t * blk)
{
   if (blk->flags & BLOCK_S_CONST_ADDR)
      blk->len = roundup_32(blk->len);
}
#endif  //def GP_DIVERSITY_JUMPTABLES

/** @brief Inline function for minimum value between 2 numbers */
#ifdef GP_DIVERSITY_JUMPTABLES
uint32_t SX_MIN(uint32_t a, uint32_t b);
#else   //def GP_DIVERSITY_JUMPTABLES
static inline uint32_t SX_MIN(uint32_t a, uint32_t b)
{
   if(a<b) { return a; } else { return b; }
}
#endif  //def GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif /* CRYPTOLIB_TYPES_H_ */
