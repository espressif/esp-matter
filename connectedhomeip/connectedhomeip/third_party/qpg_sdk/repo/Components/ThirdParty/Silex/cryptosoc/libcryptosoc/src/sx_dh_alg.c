#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the common key generation for DH protocoles
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "sx_dh_alg.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba414e_config.h"
#include "sx_primitives.h"


#if (DH_MODP_ENABLED)

uint32_t dh_common_key_modp(block_t mod, block_t priv, block_t pub, block_t common, uint32_t size)
{
   return modular_exponentiation_blk(pub, priv, mod, common, size);
}

#endif

uint32_t dh_common_key_ecdh(block_t curve, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

   // Set command to enable byte-swap
   ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);

   /* Set Configuration register */
   ba414e_set_config(BA414E_MEMLOC_12, BA414E_MEMLOC_14, BA414E_MEMLOC_6, 0x0);
   // Load parameters
   error = ba414e_load_curve(curve, size, BA414E_BIGEND, 1);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Location 14 -> Private key
   mem2CryptoRAM_rev(priv, size, BA414E_MEMLOC_14);
   // Location 12 -> Public key x, Location 13 -> Public key y
   point2CryptoRAM_rev(pub, size, BA414E_MEMLOC_12);

   // Start BA414E
   error = ba414e_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(common, size, BA414E_MEMLOC_6);

   return CRYPTOLIB_SUCCESS;
}

/* Qorvo added: Calculate ECDH common key but return only the X coordinate of the common key. */
uint32_t dh_common_key_ecdh_short(block_t curve, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

   // Set command to enable byte-swap
   ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);

   /* Set Configuration register */
   ba414e_set_config(BA414E_MEMLOC_12, BA414E_MEMLOC_14, BA414E_MEMLOC_6, 0x0);
   // Load parameters
   error = ba414e_load_curve(curve, size, BA414E_BIGEND, 1);
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Location 14 -> Private key
   mem2CryptoRAM_rev(priv, size, BA414E_MEMLOC_14);
   // Location 12 -> Public key x, Location 13 -> Public key y
   point2CryptoRAM_rev(pub, size, BA414E_MEMLOC_12);

   // Start BA414E
   error = ba414e_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   // Copy only the X coordinate of the point.
   CRYPTOLIB_ASSERT(common.len == size, "common key length differs from curve size");
   CryptoRAM2mem_rev(common, size, BA414E_MEMLOC_6);

   return CRYPTOLIB_SUCCESS;
}



uint32_t dh_montgomery_key_ecdh(block_t curve, block_t priv, block_t pub, block_t common, uint32_t size, uint32_t curve_flags)
{
   return ecc_montgomery_mult(curve, curve_flags, size, priv, pub, common);
}
