#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Implements the procedures to make ECDSA operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "sx_ecdsa_alg.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba414e_config.h"
#include "sx_hash.h"
#include "sx_rng.h"

uint32_t ecdsa_domain_parameter_validation (const sx_ecc_curve_t *curve)
{
   uint32_t status;
   uint32_t size;
   size        = sx_ecc_curve_bytesize(curve);
   ba414e_set_command(BA414E_OPTYPE_ECDSA_PARAM_EVAL, size, BA414E_BIGEND, curve->command);
   status = ba414e_load_curve(curve->params, size, BA414E_BIGEND, 1);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;
   status = ba414e_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_ecdsa_signature_configure(const sx_ecc_curve_t *curve,
                                      block_t formatted_digest,
                                      block_t key)
{
   uint32_t status;
   uint32_t size = sx_ecc_curve_bytesize(curve);

   // Set command to enable byte-swap
   ba414e_set_command(BA414E_OPTYPE_ECDSA_SIGN_GEN, size, BA414E_BIGEND, curve->command);

   // Load parameters
   status = ba414e_load_curve(curve->params, size, BA414E_BIGEND, 1);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   /* Load ECDSA parameters */
   mem2CryptoRAM_rev(key, size, BA414E_MEMLOC_6);
   mem2CryptoRAM_rev(formatted_digest, size, BA414E_MEMLOC_12);

   return CRYPTOLIB_SUCCESS;
}

uint32_t ecdsa_signature_generation_digest(  const sx_ecc_curve_t *curve,
                                             block_t formatted_digest,
                                             block_t key,
                                             block_t signature)
{
   uint8_t  kbuff[ECC_MAX_KEY_SIZE];
   uint32_t status;
   uint32_t size;
   uint32_t ctr =0;

   size = sx_ecc_curve_bytesize(curve);
   block_t k = block_t_convert(kbuff, size); /*random for signature generation*/
   block_t n = block_t_convert(curve->params.addr + size, size); /*curve order*/

   status = sx_ecdsa_signature_configure(curve, formatted_digest, key);
   if (status != CRYPTOLIB_SUCCESS)
      return status;

   do {
      status = sx_rng_get_rand_lt_n_blk(k, n);

      if (status)
         return status;
      mem2CryptoRAM_rev(k, k.len, BA414E_MEMLOC_7);

      /* ECDSA signature generation */
      status = ba414e_start_wait_status();
      ctr++;

      // If signature not valid => try to generate a valid signature with a new random
   } while((status & BA414E_STS_SINV_MASK) && (ctr < 10));
   if (status)
      return CRYPTOLIB_INVALID_SIGN_ERR;

   // Fetch the results
   CryptoRAM2point_rev(signature, size, BA414E_MEMLOC_10);


   return CRYPTOLIB_SUCCESS;
}




uint32_t ecdsa_signature_verification_digest(   const sx_ecc_curve_t *curve,
                                                block_t formatted_digest,
                                                block_t key,
                                                block_t signature)
{
   uint32_t status;
   uint32_t size;

   size = sx_ecc_curve_bytesize(curve);

   ba414e_set_command(BA414E_OPTYPE_ECDSA_SIGN_VERIF, size, BA414E_BIGEND, curve->command);
   status = ba414e_load_curve(curve->params, size, 1, 1);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   /* Load ECDSA parameters */
   point2CryptoRAM_rev(key, size, BA414E_MEMLOC_8);
   mem2CryptoRAM_rev(formatted_digest, size, BA414E_MEMLOC_12);

   // Fetch the signature
   point2CryptoRAM_rev(signature, size, BA414E_MEMLOC_10);

   /* ECDSA signature verification */
   if(ba414e_start_wait_status())
      return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}




/**
 * @brief perform a bit shift to the right of a large value stored in a byte array
 * @param array value to shift (input and output)
 * @param len length of the \p array
 * @param shift size of the bit shift (between 0 and 7)
 */
STATIC_FUNC void sx_bitshift(uint8_t *array, uint8_t len, uint8_t shift)
{
   if(shift)
   {
      uint8_t prev, val;
      int i;
      prev = 0;
      for(i = 0; i < len; i++)
      {
         val = ((array[i] >> shift)&0xFF) | prev;
         prev = array[i] << (8-shift);
         array[i] = val;
      }
   }
}


/**
 * @brief perform an ECDSA operation (sign or verify)
 * @param curve ECDSA curve to use
 * @param message message to sign or verify
 * @param key private or public key
 * @param signature result output or signature input
 * @param hash_fct algorithm to use for hash
 * @param verify select the type of ECDSA operation. (0: sign, others: verify)
 */
STATIC_FUNC uint32_t ecdsa_operation(const sx_ecc_curve_t *curve,
                                      block_t message,
                                      block_t key,
                                      block_t signature,
                                      sx_hash_fct_t hash_fct,
                                      uint8_t verify )
{
   uint32_t status;
   uint8_t digest[ECC_MAX_KEY_SIZE];
   uint32_t dgst_local_len;
   block_t digest_blk;
   uint8_t extra_bits;


   // 1. Hash message

   // 1.2 Call hash fct to get digest
   digest_blk = block_t_convert(digest, sizeof(digest));
   status = sx_hash_blk(hash_fct, message, digest_blk);
   if (status)
      return status;

   // 1.1 Define digest size. This only take the most significant bytes when curve
   // is smaller than hash. If it's greater, leading zeroes will be inserted
   // within ecdsa_signature_* functions
   dgst_local_len = SX_MIN(sx_ecc_curve_bytesize(curve), sx_hash_get_digest_size(hash_fct) );
   CRYPTOLIB_ASSERT_NM(dgst_local_len <= digest_blk.len);

   /* Shorten the digest to match the expected length */
   digest_blk.len = dgst_local_len;

   // 1.3 Bitshift if needed, for curve smaller than digest and with order N not on bytes boundaries
   extra_bits = (sx_ecc_curve_bitsize(curve)&0x7);
   if(extra_bits && sx_hash_get_digest_size(hash_fct)*8 > sx_ecc_curve_bitsize(curve))
      sx_bitshift(digest, dgst_local_len,  8-extra_bits);

   // 2. perform signature gen/ver operation
   if(verify)
      status =  ecdsa_signature_verification_digest(curve, digest_blk, key, signature);
   else
      status =  ecdsa_signature_generation_digest(curve, digest_blk, key, signature);

   return status;
}





uint32_t ecdsa_signature_generation(const sx_ecc_curve_t *curve,
                                      block_t message,
                                      block_t key,
                                      block_t signature,
                                      sx_hash_fct_t hash_fct )
{

   return ecdsa_operation(curve, message, key, signature, hash_fct, 0);

}

uint32_t ecdsa_signature_verification(const sx_ecc_curve_t *curve,
                                      block_t message,
                                      block_t key,
                                      block_t signature,
                                      sx_hash_fct_t hash_fct )
{
   return ecdsa_operation(curve, message, key, signature, hash_fct, 1);
}
