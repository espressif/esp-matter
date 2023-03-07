#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the procedures to check BA414EP integrity
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "sx_pk_integrity.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba414e_config.h"
#include "sx_hash.h"

uint32_t ba414ep_ucode_integrity_check(void)
{
   BA414E_ucode_t *ucode = (BA414E_ucode_t *) ADDR_BA414E_UCODE;

   uint32_t wl;
   block_t blk_i;
   block_t blk_h;
   uint8_t digest[SHA256_DIGESTSIZE];
   uint8_t refval[4];

   // get BA414EP microcode length in words
   // note: ucode word are 18-bit values stored on 32-bit words)
   wl = ucode->info.ucodesize;
   if(wl > BA414E_UCODE_MAX_LENGTH)   // check wl is valid
      return CRYPTOLIB_INVALID_SIGN_ERR;

   // hash ba414 microcode content
   blk_i = block_t_convert((uint8_t*) ADDR_BA414E_UCODE, 4*wl);
   blk_h = block_t_convert(digest, sizeof(digest));

   if(sx_hash_blk(e_SHA256, blk_i, blk_h))
      return CRYPTOLIB_CRYPTO_ERR;

   // check that reference value match computed value
   refval[0] = (ucode->content[wl]>>8   )&0xFF;
   refval[1] = (ucode->content[wl]>>0   )&0xFF;
   refval[2] = (ucode->content[wl+1]>>8 )&0xFF;
   refval[3] = (ucode->content[wl+1]>>0 )&0xFF;

   if( refval[0] == digest[0] && refval[1] == digest[1] && refval[2] == digest[2] && refval[3] == digest[3] )
      return CRYPTOLIB_SUCCESS;
   else
      return CRYPTOLIB_INVALID_SIGN_ERR;

}
