#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the procedures to generate random numbers
 *          as described in NIST 800-90A
 * @copyright Copyright (c) 2017-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "sx_drbg_hash.h"
#include <string.h>
#include "sx_math.h"
#include <inttypes.h>

#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

/* NIST SP 800-90A defines the maximum number of requests between reseeds to be
 * 2^48. We're lazy and don't want to deal with large numbers, so we set that
 * to 2^32 - 1 instead.
 * Users are allowed to request a reseed at any point, so there's no security
 * impact of enforcing more reseeds than required by the standard. */
#define RESEED_INTERVAL  0xffffffff
#define MAX_REQUEST_SIZE ((1 << 19) / 8)

/**
 * @brief Hash derivation function (private)
 *
 * @param ctx   The context describing the state of this Hash_DRBG
 * @param input Array of hash input block_ts
 * @param input_count The number of entries in \p input
 * @param output The block_t where to write the hash output
 *
 * @return CRYPTOLIB_SUCCESS when execution was successful
 */
STATIC_FUNC uint32_t _sx_drbg_hash_df(sx_drbg_hash_ctx_t *ctx,
      block_t *input, const size_t input_count,
      block_t output)
{
   CRYPTOLIB_ASSERT_NM(ctx);
   CRYPTOLIB_ASSERT_NM(input);
   CRYPTOLIB_ASSERT_NM(input_count > 0);
   CRYPTOLIB_ASSERT_NM(output.len >= ctx->seedlen);

   /* Hash_df Process:
    * 2. len = ceil(no_of_bits_to_return / outlen).
    * Note that we write more data than seedlen here!
    */
   size_t len = (ctx->seedlen + ctx->hash_block_size - 1)/ ctx->hash_block_size;

   /* 3. counter = 0x01; */
   uint8_t counter = 0x01;
   uint32_t no_of_bits = SWAPBYTES32(ctx->seedlen * 8);

   /* 4. For i = 1 to len do */
   for (int i = 0; i < len; i++) {
      /* 4.1 temp = temp || Hash(counter || no_of_bits_to_return || input_string) */
      block_t hash_input[SX_HASH_ARRAY_MAX_ENTRIES];
      MEMSET(hash_input, 0, sizeof(hash_input));
      hash_input[0] = block_t_convert(&counter, sizeof(counter));
      hash_input[1] = block_t_convert(&no_of_bits, sizeof(no_of_bits));

      size_t hash_input_size = 2;
      CRYPTOLIB_ASSERT_NM((input_count + hash_input_size) <= COUNT_OF(hash_input));

      for (int i = 0; i < input_count; i++) {
         hash_input[hash_input_size] = input[i];
         hash_input_size++;
      }

      uint32_t status = sx_hash_array_blk(ctx->hash_fct, hash_input, hash_input_size,
              output);
      if (status != CRYPTOLIB_SUCCESS)
          return status;

      /* 5. requested_bits = leftmost(temp, no_of_bits_to_return)
       * We write the output as we go */
      output.len -= ctx->hash_block_size;
      if (! (output.flags & BLOCK_S_CONST_ADDR))
          output.addr += ctx->hash_block_size;

      /* 4.2. counter = counter + 1 */
      counter++;
   }

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_drbg_hash_instantiate(sx_drbg_hash_ctx_t *ctx,
      uint32_t security_strength,
      uint8_t *entropy, size_t entropy_size,
      uint8_t *personalization, size_t personalization_size,
      uint8_t *nonce, size_t nonce_size)
{
   CRYPTOLIB_ASSERT_NM(ctx);
   CRYPTOLIB_ASSERT_NM(entropy);

   /* Can't re-instantiate without uninstantiating first */
   if (ctx->is_instantiated) {
      return CRYPTOLIB_INVALID_PARAM;
   }

   /* Minimal seed lengths and output block size as defined in NIST SP 800-90 A
    * Table 2. */
   switch (security_strength) {
   case 112:
      ctx->hash_fct = e_SHA224;
      ctx->hash_block_size = 224 / 8;
      ctx->seedlen = 440 / 8;
      break;
   case 128:
      ctx->hash_fct = e_SHA256;
      ctx->hash_block_size = 256 / 8;
      ctx->seedlen = 440 / 8;
      break;
   case 192:
      ctx->hash_fct = e_SHA384;
      ctx->hash_block_size = 384 / 8;
      ctx->seedlen = 888 / 8;
      break;
   case 256:
      ctx->hash_fct = e_SHA512;
      ctx->hash_block_size = 512 / 8;
      ctx->seedlen = 888 / 8;
      break;
   default:
      CRYPTOLIB_ASSERT(false, "Unsupported security strength");
      return CRYPTOLIB_INVALID_PARAM;
   }

   if (entropy_size < (security_strength / 8)) {
      CRYPTOLIB_PRINTF("Insufficient entropy to seed DRBG Hash\n");
      return CRYPTOLIB_INVALID_PARAM;
   }

   /* 5. reseed_counter = 1 */
   ctx->reseed_counter = 1;

   /* 1. seed_material = entropy_input || nonce || personalization) */
   if(!nonce) {
      nonce_size = 0;
   }
   if(!personalization) {
      personalization_size = 0;
   }
   block_t array_enp[3];
   MEMSET(array_enp, 0, sizeof(array_enp));
   array_enp[0] = (block_t) BLOCK_T_CONV(entropy, entropy_size);
   array_enp[1] = (block_t) BLOCK_T_CONV(nonce, nonce_size);
   array_enp[2] = (block_t) BLOCK_T_CONV(personalization, personalization_size);

   /* 2. seed = Hash_df(seed_material, seedlen)
    * 3. V = seed */
   uint32_t status = _sx_drbg_hash_df(ctx, array_enp,
           COUNT_OF(array_enp),
           block_t_convert(ctx->V, ctx->seedlen));
   if (status != CRYPTOLIB_SUCCESS)
       return status;

   /* Step 4: C = Hash_df(0x00 || V) */
   uint8_t zero = 0;
   block_t array_zv[] = {
       block_t_convert(&zero, sizeof(zero)),
       block_t_convert(ctx->V, ctx->seedlen)
   };
   status = _sx_drbg_hash_df(ctx, array_zv,
           COUNT_OF(array_zv),
           block_t_convert(ctx->C, ctx->seedlen));
   if (status != CRYPTOLIB_SUCCESS)
       return status;

   ctx->security_strength = security_strength;
   ctx->is_instantiated = true;

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_drbg_hash_reseed(sx_drbg_hash_ctx_t *ctx,
        uint8_t *entropy, size_t entropy_size)
{
   CRYPTOLIB_ASSERT_NM(ctx);
   CRYPTOLIB_ASSERT_NM(entropy);

   if (! ctx->is_instantiated)
      return CRYPTOLIB_INVALID_PARAM;

   /* min_length is security_strength */
   if ((entropy_size * 8) < ctx->security_strength)
      return CRYPTOLIB_INVALID_PARAM;

   /* 1. seed_material = 0x01 || V || entropy_input */
   uint8_t one = 0x01;
   block_t seed_material[] = {
       block_t_convert(&one, sizeof(one)),
       block_t_convert(ctx->V, ctx->seedlen),
       block_t_convert(entropy, entropy_size)
   };

   /* 2. seed = Hash_df(seed_material) */
   uint8_t seed[DRBG_HASH_MAX_SEEDLEN];
   CRYPTOLIB_ASSERT_NM(ctx->seedlen <= sizeof(seed));

   uint32_t status = _sx_drbg_hash_df(ctx, seed_material,
           COUNT_OF(seed_material),
           block_t_convert(seed, ctx->seedlen));
   /* If this or any of the following operations fail our DRBG_Hash is broken,
    * because V might be in an undefined state. */
   CRYPTOLIB_ASSERT_NM(status == CRYPTOLIB_SUCCESS);

   /* 3. V = seed */
   memcpy_array(ctx->V, seed, ctx->seedlen);

   /* 4. C = Hash_df(0x00 || V) */
   uint8_t zero = 0x00;
   block_t input[] = {
       block_t_convert(&zero, sizeof(zero)),
       block_t_convert(ctx->V, ctx->seedlen)
   };
   status = _sx_drbg_hash_df(ctx, input, COUNT_OF(input),
           block_t_convert(ctx->C, ctx->seedlen));
   CRYPTOLIB_ASSERT_NM(status == CRYPTOLIB_SUCCESS);

   /* 5. ressed_counter = 1 */
   ctx->reseed_counter = 1;

   return CRYPTOLIB_SUCCESS;
}

/**
 * @brief Hash generation function (private)
 *
 * @param ctx The context describing the state of this Hash_DRBG
 * @param out The block_t where to write the hash output
 *
 * @return CRYPTOLIB_SUCCESS when execution was successful
 */
STATIC_FUNC void _sx_drbg_hash_hasgen(sx_drbg_hash_ctx_t *ctx, block_t out)
{
   CRYPTOLIB_ASSERT_NM(ctx);

   /* Hashgen 1: m = ceil(requested_no_of_bits / outlen) */
   int m = out.len / ctx->hash_block_size;
   if (out.len % ctx->hash_block_size)
       m++;

   /* Hashgen 2: data = V */
   uint8_t data[DRBG_HASH_MAX_SEEDLEN];
   block_t data_block = BLOCK_T_CONV(data, ctx->seedlen);
   CRYPTOLIB_ASSERT_NM(ctx->seedlen <= DRBG_HASH_MAX_SEEDLEN);

   memcpy_array(data, ctx->V, ctx->seedlen);

   /* Hashgen 4: for i = 1 to m */
   for (int i = 0; i < m; i++) {
      uint32_t ret = sx_hash_blk(ctx->hash_fct, data_block, out);
      CRYPTOLIB_ASSERT_NM(ret == CRYPTOLIB_SUCCESS);

      out.len -= ctx->hash_block_size;
      if (! (out.flags & BLOCK_S_CONST_ADDR))
          out.addr += ctx->hash_block_size;

      /* Hashgen 4.3: data = (data + 1) mod 2^seedlen */
      sx_math_array_increment(data, ctx->seedlen, 1);
   }
}

uint32_t sx_drbg_hash_generate(sx_drbg_hash_ctx_t *ctx, block_t out)
{
   CRYPTOLIB_ASSERT_NM(ctx);
   CRYPTOLIB_ASSERT_NM(ctx->seedlen <= DRBG_HASH_MAX_SEEDLEN);

   if (! ctx->is_instantiated) {
      CRYPTOLIB_PRINTF("DRBG_Hash: Not instantiated\n");
      return CRYPTOLIB_INVALID_PARAM;
   }

   /* 1. if reseed_count == reseed_interval then return an indication that a
    * reseed is required. */
   if (ctx->reseed_counter == RESEED_INTERVAL) {
      CRYPTOLIB_PRINTF("DRBG_Hash: reseed required\n");
      return CRYPTOLIB_RESEED_REQUIRED;
   }

   if (out.len > MAX_REQUEST_SIZE) {
      CRYPTOLIB_PRINTF("DRBG_Hash: Requested %"PRIu32" bytes, but max size is %d\n",
            out.len, MAX_REQUEST_SIZE);
      return CRYPTOLIB_INVALID_PARAM;
   }

   /* 3. returned_bits = Hashgen(requested_number_of_bits, V) */
   _sx_drbg_hash_hasgen(ctx, out);

   /* 4. H = Hash(0x03 || V) */
   uint8_t H[MAX_DIGESTSIZE];
   uint8_t three = 0x03;
   block_t input[] = {
      block_t_convert(&three, sizeof(three)),
      block_t_convert(ctx->V, ctx->seedlen)
   };

   CRYPTOLIB_ASSERT_NM(ctx->hash_block_size <= MAX_DIGESTSIZE);
   uint32_t status = sx_hash_array_blk(ctx->hash_fct, input,
           COUNT_OF(input),
           block_t_convert(H, ctx->hash_block_size));
   CRYPTOLIB_ASSERT_NM(status == CRYPTOLIB_SUCCESS);

   /* 5. V = (V + H + C + reseed_counter) mod 2^seedlen */
   sx_math_array_add(ctx->V, ctx->seedlen, H, ctx->hash_block_size);
   sx_math_array_add(ctx->V, ctx->seedlen, ctx->C, ctx->seedlen);
   sx_math_array_increment(ctx->V, ctx->seedlen, ctx->reseed_counter);

   /* 6. reseed_counter = reseed_counter + 1 */
   ctx->reseed_counter++;

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_drbg_hash_uninstantiate(sx_drbg_hash_ctx_t *ctx)
{
   CRYPTOLIB_ASSERT_NM(ctx);

   if (! ctx->is_instantiated)
      return CRYPTOLIB_INVALID_PARAM;

   /* Delete all internal state */
   MEMSET(ctx, 0, sizeof(sx_drbg_hash_ctx_t));

   return CRYPTOLIB_SUCCESS;
}
