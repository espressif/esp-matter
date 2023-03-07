/**
 * @file
 * @brief Defines the procedures to generate random numbers
 *          as described in NIST 800-90A
 * @copyright Copyright (c) 2017-2018 Silex Inside. All Rights reserved
 */

#ifndef SX_DRBG_HASH_H_
#define SX_DRBG_HASH_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_drbg_hash_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdbool.h>
#include <stdint.h>
#include "cryptolib_def.h"
#include "sx_hash.h"

#define DRBG_HASH_MAX_SEEDLEN (888 / 8)

/**
 * @brief Context for DRBG Hash operations
 *
 * This is a private structure and should never be directly manipulated by
 * sx_drbg_hash users.
 */
typedef struct sx_drbg_hash_ctx_s
{
   bool     is_instantiated;
   uint16_t security_strength;
   sx_hash_fct_t hash_fct;
   size_t hash_block_size;
   uint32_t seedlen;

   uint8_t V[DRBG_HASH_MAX_SEEDLEN];
   uint8_t C[DRBG_HASH_MAX_SEEDLEN];
   uint32_t reseed_counter;
} sx_drbg_hash_ctx_t;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_drbg_hash.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_drbg_hash.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Instantiate a Hash_DRBG
 *
 * @param ctx                  The context describing the state of this Hash_DRBG
 * @param security_strength    The desired security strength, { 112, 128, 192, 256 }
 * @param entropy              An array of random bytes
 * @param entropy_size         The size of the entropy array
 * @param personalization      Personalization data, non-secret
 * @param personalization_size The size of the personalization array
 * @param nonce                Nonce data, secret
 * @param nonce_size           The size of the nonce array
 *
 * @return OKAY when execution was successful
 */
uint32_t sx_drbg_hash_instantiate(sx_drbg_hash_ctx_t *ctx, uint32_t security_strength, uint8_t *entropy, size_t entropy_size, uint8_t *personalization, size_t personalization_size, uint8_t *nonce, size_t nonce_size);

/**
 * @brief Re-seed the Hash_DRBG with the supplied entropy.
 *
 * @param ctx                  The context describing the state of this Hash_DRBG
 * @param entropy              An array of random bytes
 * @param entropy_size         The size of the entropy array
 *
 * @return OKAY when execution was successful
 */
uint32_t sx_drbg_hash_reseed(sx_drbg_hash_ctx_t *ctx, uint8_t *entropy, size_t entropy_size);

/**
 * @brief Generate random bits using the instantiated Hash_DRBG
 *
 * @param ctx  The context describing the state of this Hash_DRBG
 * @param out  Block_t with pointer to result, expected length and flags
 *             (constant address or not)
 * @return OKAY when execution was successful
 */
uint32_t sx_drbg_hash_generate(sx_drbg_hash_ctx_t *ctx, block_t out);

/**
 * @brief Uninstantiate a previously instantiated Hash_DRBG
 *
 * @param ctx                  The context describing the state of this Hash_DRBG
 *
 * @return OKAY when execution was successful
 */
uint32_t sx_drbg_hash_uninstantiate(sx_drbg_hash_ctx_t *ctx);

#if defined(GP_DIVERSITY_JUMPTABLES)
void     _sx_drbg_hash_hasgen(sx_drbg_hash_ctx_t *ctx, block_t out);
uint32_t _sx_drbg_hash_df(sx_drbg_hash_ctx_t *ctx, block_t *input, const size_t input_count, block_t output);
#endif // GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
