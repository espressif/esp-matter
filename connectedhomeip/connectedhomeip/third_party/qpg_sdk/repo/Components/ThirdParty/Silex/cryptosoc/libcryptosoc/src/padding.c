#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

#include "cryptolib_internal.h"
#include "padding.h"
#include <string.h>
#include "cryptodma.h"


void pad_zeros(uint8_t *EM, size_t emLen, uint8_t *hash, size_t hashLen)
{
   MEMSET(EM, 0x00, emLen - hashLen);
   memcpy_array(EM + emLen - hashLen, hash, hashLen);
}

void pad_zeros_blk(block_t out, block_t in)
{
   MEMSET(out.addr, 0x00, out.len - in.len);
   if(!(out.flags & BLOCK_S_CONST_ADDR)) out.addr += (out.len - in.len);
   memcpy_blk(out, in, in.len);
}
