#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines ECC keygen functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "sx_ecc_keygen_alg.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba414e_config.h"
#include "sx_rng.h"
#include "sx_primitives.h"


uint32_t ecc_validate_key(block_t domain, block_t pub,uint32_t size, uint32_t curve_flags)
{

   uint32_t status;
   ba414e_set_command(BA414E_OPTYPE_ECC_CHECK_POINTONCURVE, size, BA414E_BIGEND, curve_flags);
   status = ba414e_load_curve(domain, size, BA414E_BIGEND, 1);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;
   point2CryptoRAM_rev(pub, size , BA414E_MEMLOC_6);

   ba414e_set_config(BA414E_MEMLOC_6, BA414E_MEMLOC_6, BA414E_MEMLOC_6, 0x0);
   status = ba414e_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_genkey(block_t domain, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags)
{
   // Get random number < n -> private key
   block_t n = block_t_convert(domain.addr + size, size);
   uint32_t status = sx_rng_get_rand_lt_n_blk(priv, n);
   if (status)
      return status;

   //Point mult for Public key
   return ecc_generate_pub_key(domain, pub, priv, size, curve_flags);
}


uint32_t ecc_generate_pub_key(block_t curve, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags)
{
   uint32_t status;

   // Set command to enable byte-swap
   ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);
   // Load parameters
   status = ba414e_load_curve(curve, size, BA414E_BIGEND, 1);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   // Location 14 -> Private key
   mem2CryptoRAM_rev(priv, priv.len, BA414E_MEMLOC_14);

   /* Set Configuration register */
   ba414e_set_config(BA414E_MEMLOC_2, BA414E_MEMLOC_14, BA414E_MEMLOC_6, 0x0);

   /* Start ECC Point Mult */
   status = ba414e_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(pub, size, BA414E_MEMLOC_6);

   return CRYPTOLIB_SUCCESS;
}



uint32_t ecc_montgomery_genkey(block_t curve, block_t priv, block_t pub, uint32_t size, uint32_t curve_flags)
{
   // Get random private key
   sx_rng_get_rand_blk(priv);
   // Point mult for Public key
   return ecc_montgomery_mult(curve, curve_flags, size, priv, sx_getNULL_blk(), pub);
}


uint32_t ecc_montgomery_get_public_key_blk(block_t curve, block_t priv, block_t pub, uint32_t size, uint32_t curve_flags)
{
   return ecc_montgomery_mult(curve, curve_flags, size, priv, sx_getNULL_blk(), pub);
}
