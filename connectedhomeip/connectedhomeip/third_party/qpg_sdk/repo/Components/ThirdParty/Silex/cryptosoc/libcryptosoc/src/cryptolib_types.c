

#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

#include "cryptolib_types.h"

/**
 * @brief Convert uint8 array to block_t
 * @param  array  Array
 * @param  length Length of array
 * @return block_t
 */

#ifdef GP_DIVERSITY_JUMPTABLES

STATIC_FUNC block_t block_t_convert(const volatile void * array, uint32_t length)
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

/**
 * Round-up the length of a block_t to 32-bit if the address is a FIFO
 * @param  blk    A pointer to the block_t to adapt
 */
STATIC_FUNC void block_t_adapt_len(block_t * blk)
{
   if (blk->flags & BLOCK_S_CONST_ADDR)
      blk->len = roundup_32(blk->len);
}

/** @brief Inline function for minimum value between 2 numbers */
STATIC_FUNC uint32_t SX_MIN(uint32_t a, uint32_t b)
{
   if(a<b) { return a; } else { return b; }
}

#endif  //def GP_DIVERSITY_JUMPTABLES
