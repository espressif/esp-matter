
#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Math functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "sx_math.h"
#include "cryptolib_def.h"

uint32_t sx_math_array_is_not_null(uint8_t *in, uint32_t length)
{
   uint32_t i;
   uint32_t is_null = 1;

   for(i = 0; i < length-1; i++) {
      is_null &= (in[i] == 0);
   }

   return !is_null;
}



uint32_t sx_math_array_nbits(uint8_t *a, const size_t length)
{
   CRYPTOLIB_ASSERT_NM(a);

   uint32_t i,j;
   uint32_t nbits = 8*length;

   for(i=0;i<length;i++) {
      if(a[i]==0) {
         nbits -= 8;
      } else {
         for(j=7;j>0;j--) {
            if(a[i]>>j)
               break;
            else
               nbits--;
         }
         break;
      }
   }
   return nbits;
}


void sx_math_array_increment(uint8_t *a, const size_t length, uint8_t value)
{
   CRYPTOLIB_ASSERT_NM(a);
   CRYPTOLIB_ASSERT_NM(length >= 1);
   int i = length - 1;
   for (; i >= 0; i--) {
      uint16_t newval = a[i] + value;
      if (newval > 0xff) {
         a[i] = newval & 0xff;
         value = newval >> 8;
      }
      else {
         a[i] += value;
         break;
      }
   }
}

void sx_math_array_add(uint8_t *a, const size_t alen, const uint8_t *b, const size_t blen)
{
   CRYPTOLIB_ASSERT_NM(a);
   CRYPTOLIB_ASSERT_NM(alen >= 1);
   CRYPTOLIB_ASSERT_NM(b);
   CRYPTOLIB_ASSERT_NM(blen >= 1);

   uint32_t tmp = 0;
   int i = 0;

   for(; i < blen; i++) {
      tmp += a[alen - i - 1] + b[blen - i - 1];
      a[alen - i - 1] = tmp & 0xFF;
      tmp >>= 8;
   }

   for (; tmp != 0 && i < alen; i++) {
      tmp += a[alen - i - 1] ;
      a[alen - i - 1] = tmp & 0xFF;
      tmp >>= 8;
   }
}
