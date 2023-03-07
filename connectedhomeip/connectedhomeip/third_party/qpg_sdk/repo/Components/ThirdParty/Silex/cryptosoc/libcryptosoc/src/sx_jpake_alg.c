#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Implements the procedures to make JPAKE operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "sx_jpake_alg.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "memcmp.h"
#include "ba414e_config.h"
#include "padding.h"
#include "sx_hash.h"
#include "sx_primitives.h"
#include "sx_rng.h"
#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#endif

#if (JPAKE_ENABLED)

/************************************************
 *  Private functions
 ***********************************************/
STATIC_FUNC uint32_t jpake_gen_seskey_blk(block_t curve,
                         block_t X4,
                         block_t B,
                         block_t x2,
                         block_t x2s,
                         block_t T,
                         uint32_t curve_flags,
                         uint32_t size)
{
   /* Generate session key */
   ba414e_set_command(BA414E_OPTYPE_JPAKE_SESSION_KEY, size, BA414E_BIGEND, curve_flags);
   ba414e_set_config(BA414E_MEMLOC_6, BA414E_MEMLOC_8, BA414E_MEMLOC_13, BA414E_MEMLOC_0);

   if(ba414e_load_curve(curve, size, BA414E_BIGEND, 0)) return CRYPTOLIB_CRYPTO_ERR;

   mem2CryptoRAM_rev(x2, size, BA414E_MEMLOC_10); // x2 & x2s set before 6-9 because fetched first
   mem2CryptoRAM_rev(x2s, size, BA414E_MEMLOC_11);
   point2CryptoRAM_rev(X4, size, BA414E_MEMLOC_6);
   point2CryptoRAM_rev(B, size, BA414E_MEMLOC_8);

   /* Start BA414 */
   if (ba414e_start_wait_status()) return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2mem_rev(T, size, BA414E_MEMLOC_13); //Only need T.x

   return CRYPTOLIB_SUCCESS;
}

#include "silexCryptoSoc_Defs.h"

STATIC_FUNC uint32_t jpake_hash_GVXU(block_t G, block_t V, block_t X, block_t ii, uint8_t *h, const size_t h_size)
{



   silexCryptoSoc_GlobalVars_t *globals = (void *)SILEX_CRYPTOSOC_GET_GLOBALS();


   uint8_t *ii_preamble = globals->ii_preamble;
   uint8_t *G_preamble = globals->G_preamble;
   uint8_t *X_preamble = globals->X_preamble;
   uint8_t *V_preamble = globals->V_preamble;



   ii_preamble[0] = 0;
   ii_preamble[1] = 0;
   ii_preamble[2] = 0;
   ii_preamble[3] = ii.len;

   // case2 x9.62 preamble for 2 octet strings. Encoded data = length (32-bit) | 0x4 (8-bit) | scalar1 | scalar2

   G_preamble[0] = 0;
   G_preamble[1] = 0;
   G_preamble[2] = 0;
   G_preamble[3] = G.len+1;
   G_preamble[4] = 0x04;


   X_preamble[0] = 0;
   X_preamble[1] = 0;
   X_preamble[2] = 0;
   X_preamble[3] = X.len+1;
   X_preamble[4] = 0x04;


   V_preamble[0] = 0;
   V_preamble[1] = 0;
   V_preamble[2] = 0;
   V_preamble[3] = V.len+1;
   V_preamble[4] = 0x04;

   //todo: check. Somehow stack protector issue warnings if size are set to smaller value?

   block_t array[8];
   array[0] = block_t_convert(G_preamble, 5);
   array[1] = G;
   array[2] = block_t_convert(V_preamble, 5);
   array[3] = V;
   array[4] = block_t_convert(X_preamble, 5);
   array[5] = X;
   array[6] = block_t_convert(ii_preamble, 4);
   array[7] = ii;
   return sx_hash_array_blk(e_SHA256, &array[0], 8, block_t_convert(h,h_size));
}


uint32_t jpake_hash_verify_zkp(block_t G, uint32_t Gloc, block_t X_b,
   block_t V_b, block_t z_b, block_t his_id, uint32_t curve_flags, uint32_t size)
{
   uint8_t  h[SHA256_DIGESTSIZE];
   uint32_t status;

   /* 1. Hash concatenation of generator point | V | X | ID */
   status = jpake_hash_GVXU(G, V_b, X_b, his_id, h, sizeof(h));
   if(status) return CRYPTOLIB_CRYPTO_ERR;
   //h = h mod N -> load result to LOC12
   status = ba414e_load_and_modN(BA414E_MEMLOC_12, BA414E_MEMLOC_1, size,
               block_t_convert(h, SHA256_DIGESTSIZE), sx_getNULL_blk(), curve_flags);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   /* 2. Verify zero knowledge proof */
   ba414e_set_command(BA414E_OPTYPE_JPAKE_ZKP_VERIF, size, BA414E_BIGEND, curve_flags);

   // Set input pointers
   if (Gloc == ~0) {
      Gloc = BA414E_MEMLOC_13;
      point2CryptoRAM_rev(G, size, Gloc);
   }
   ba414e_set_config(Gloc, BA414E_MEMLOC_0, BA414E_MEMLOC_0, BA414E_MEMLOC_0);
   point2CryptoRAM_rev(X_b, size, BA414E_MEMLOC_8);
   point2CryptoRAM_rev(V_b, size, BA414E_MEMLOC_6);
   mem2CryptoRAM_rev(z_b, size, BA414E_MEMLOC_10);
   // h already loaded in LOC12
   status = ba414e_start_wait_status();
   if (status == BA414E_STS_PNCU_MASK)
        return CRYPTOLIB_INVALID_POINT;
   if (status)
        return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}


STATIC_FUNC uint32_t jpake_hash_verify_zkp_blk(block_t G, uint8_t Gloc, block_t zkp, block_t his_id, uint32_t curve_flags, uint32_t size)

{
   uint8_t  X[2*ECC_MAX_KEY_SIZE];
   uint8_t  V[2*ECC_MAX_KEY_SIZE];
   uint8_t  Z[ECC_MAX_KEY_SIZE];
   uint32_t status;

   block_t X_b = block_t_convert(X, 2*size);
   block_t V_b = block_t_convert(V, 2*size);
   block_t Z_b = block_t_convert(Z, size);

   /* Split & copy the different elements of zkp since they have to be used several times, and separately*/
   memcpy_blk(X_b, zkp, 2*size);
   if(!(zkp.flags & BLOCK_S_CONST_ADDR)) zkp.addr += 2*size;
   memcpy_blk(V_b, zkp, 2*size);
   if(!(zkp.flags & BLOCK_S_CONST_ADDR)) zkp.addr += 2*size;
   memcpy_blk(Z_b, zkp, size);

   status = jpake_hash_verify_zkp(G, Gloc, X_b, V_b, Z_b, his_id, curve_flags, size);

   return status;
}

uint32_t jpake_hash_create_zkp(block_t G,
    block_t x, block_t X,
    block_t v, block_t V,
    block_t my_id,
    block_t r,
    uint32_t curve_flags, uint32_t opsize)
{
   uint8_t hmem[SHA256_DIGESTSIZE];
   uint32_t status;
   MEMSET(hmem, 0, sizeof(hmem));
   block_t h = BLOCK_T_CONV(hmem, SHA256_DIGESTSIZE);


   status = jpake_hash_GVXU(G, V, X, my_id, hmem, SHA256_DIGESTSIZE);
   if(status)
        return CRYPTOLIB_CRYPTO_ERR;

   status = ba414e_load_and_modN(BA414E_MEMLOC_12, BA414E_MEMLOC_1, opsize, h, sx_getNULL_blk(),
        curve_flags);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   ba414e_set_command(BA414E_OPTYPE_JPAKE_ZKP_GEN, opsize, BA414E_BIGEND, curve_flags);
   ba414e_set_config(BA414E_MEMLOC_8, BA414E_MEMLOC_11, BA414E_MEMLOC_10, BA414E_MEMLOC_0);
   if (v.len)
      mem2CryptoRAM_rev(v, opsize, BA414E_MEMLOC_8);
   if (x.len)
      mem2CryptoRAM_rev(x, opsize, BA414E_MEMLOC_11);

   if(ba414e_start_wait_status())
        return CRYPTOLIB_CRYPTO_ERR;

   // Fetch results
   CryptoRAM2mem_rev(r, opsize, BA414E_MEMLOC_10);

   return 0;
}

STATIC_FUNC uint32_t jpake_multiply_generate_zkp_blk(block_t curve, block_t G, uint8_t Gloc, block_t x, block_t my_id, block_t zkp, uint32_t curve_flags, uint32_t size)
{
   uint8_t v[ECC_MAX_KEY_SIZE];
   uint8_t zkpBuff[3*ECC_MAX_KEY_SIZE];
   uint8_t XBuff[2*ECC_MAX_KEY_SIZE];

   block_t v_blk = block_t_convert(v, size);
   block_t zkpT = block_t_convert(zkpBuff, 3*size);
   block_t X = block_t_convert(XBuff, 2*size);
   uint32_t status;

   /* 1. ECC Point multiplication X = x . G (G = curve generator point)*/
   ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);
   ba414e_set_config(Gloc, BA414E_MEMLOC_11, BA414E_MEMLOC_6, BA414E_MEMLOC_0);
   mem2CryptoRAM_rev(x, size, BA414E_MEMLOC_11);
   if(ba414e_start_wait_status()) return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(X, size, BA414E_MEMLOC_6);

   /* 2. Generate random number v < n for zero knowledge proof, and load it to LOC 8 */
   block_t n = block_t_convert(curve.addr + size, size);
   status = sx_rng_get_rand_lt_n_blk(v_blk, n);
   if (status)
      return status;
   mem2CryptoRAM_rev(v_blk, v_blk.len, BA414E_MEMLOC_8);

   /* 3. Generate zero-knowledge proof V component (V (LOC6) = v (loc8) . G (loc2))) */
   // v and G are already loaded
   ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);
   ba414e_set_config(Gloc, BA414E_MEMLOC_8, BA414E_MEMLOC_6, BA414E_MEMLOC_0);
   if(ba414e_start_wait_status()) return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(block_t_convert(zkpT.addr, size*2), size, BA414E_MEMLOC_6);

   block_t V_blk = block_t_convert(zkpT.addr, 2*size);
   block_t r_blk = block_t_convert(zkpT.addr + 2*size, size);
   status = jpake_hash_create_zkp(
                   G,
                   sx_getNULL_blk() /* x already in loc11 */, X,
                   sx_getNULL_blk() /* v preloaded in loc8 */, V_blk,
                   my_id,
                   r_blk, curve_flags, size);
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   /* Transfer results */
   memcpy_blk(zkp, X, X.len);
   if(!(zkp.flags & BLOCK_S_CONST_ADDR)) zkp.addr += X.len;
   memcpy_blk(zkp, zkpT, zkpT.len);

   return CRYPTOLIB_SUCCESS;
}




/************************************************
 *  Public functions
 ***********************************************/

uint32_t jpake_round1_generate_blk(block_t curve, block_t user_id, block_t x2, block_t zkp1, block_t zkp2, uint32_t curve_flags, uint32_t size)
{
   uint32_t status;
   uint8_t x1Buff[ECC_MAX_KEY_SIZE];
   uint8_t x2Buff[ECC_MAX_KEY_SIZE];
   uint8_t temp_id[JPAKE_MAX_ID_SIZE];

   block_t x1b = block_t_convert(x1Buff, size);
   block_t x2b = block_t_convert(x2Buff, size);
   block_t generator = block_t_convert(curve.addr + 2*size, 2*size);

   // Store the fetched user_id in a buffer
   memcpy_blkIn(temp_id, user_id, user_id.len);

   // load curve
   ba414e_set_command(0, size, BA414E_BIGEND, curve_flags);
   if(ba414e_load_curve(curve, size, BA414E_BIGEND, 1)) return CRYPTOLIB_CRYPTO_ERR;

   /* selects secret value x1 < n */
   block_t n = block_t_convert(curve.addr + size, size);
   status =  sx_rng_get_rand_lt_n_blk(x1b, n);
   if (status)
      return status;

   /* selects secret value x2 < n */
   status = sx_rng_get_rand_lt_n_blk(x2b, n);
   if (status)
      return status;

   /* Transfer x2 */
   memcpy_blk(x2, x2b, size);

   /* Generate zero-knowledge proof for x1 */
   status = jpake_multiply_generate_zkp_blk(curve, generator, BA414E_MEMLOC_2,
                                            x1b, block_t_convert(temp_id, user_id.len),
                                            zkp1, curve_flags, size);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   /* Generate zero-knowledge proof for x2 */
   status = jpake_multiply_generate_zkp_blk(curve, generator, BA414E_MEMLOC_2,
                                            x2b, block_t_convert(temp_id, user_id.len),
                                            zkp2, curve_flags, size);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}


uint32_t jpake_round1_verify_blk(block_t curve, block_t my_user_id, block_t his_user_id, block_t zkp3, block_t zkp4, uint32_t curve_flags, uint32_t size)
{
   uint8_t     my_id[JPAKE_MAX_ID_SIZE];
   uint8_t     his_id[JPAKE_MAX_ID_SIZE];

   block_t generator = block_t_convert(curve.addr + 2*size, 2*size);
   uint32_t status;

   /* 1. Check that ID's are different */
   // Copy user_ids
   memcpy_blkIn(my_id, my_user_id, my_user_id.len);
   my_user_id = block_t_convert(my_id, my_user_id.len);

   memcpy_blkIn(his_id, his_user_id, his_user_id.len);
   his_user_id = block_t_convert(his_id, his_user_id.len);

   // Chech if ID is not equal
   if((my_user_id.len == his_user_id.len) && !(memcmp_time_cst(my_user_id.addr, his_user_id.addr, my_user_id.len)))
      return CRYPTOLIB_INVALID_SIGN_ERR;

   /* 2. Verify ZKP */
   ba414e_set_command(0, size, BA414E_BIGEND, curve_flags);
   if(ba414e_load_curve(curve, size, BA414E_BIGEND, 1)) return CRYPTOLIB_CRYPTO_ERR;

   status = jpake_hash_verify_zkp_blk(generator, BA414E_MEMLOC_2, zkp3, his_user_id, curve_flags, size); //verif zkp3
   if(status) return CRYPTOLIB_INVALID_SIGN_ERR;

   status = jpake_hash_verify_zkp_blk(generator, BA414E_MEMLOC_2, zkp4, his_user_id, curve_flags, size); //verif zkp4
   if(status) return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}

uint32_t jpake_round2_compute_points(block_t curve, block_t x2,
   block_t X1_in, block_t X3_in, block_t X4_in,
   block_t x2s_blk, block_t GA, block_t A, uint32_t curve_flags, uint32_t size)
{
   uint32_t status;
   if (!((X3_in.len - X4_in.len) ? (X3_in.len - X4_in.len) : MEMCMP_ROM(X3_in.addr, X4_in.addr, X3_in.len)))
   {
      // If X3 and X4 are equal the crypto engine returns an error code
      // This is, however, not a protocol requirement
      return CRYPTOLIB_INVALID_PARAM;
   }

   ba414e_set_command(BA414E_OPTYPE_JPAKE_STEP2, size, BA414E_BIGEND, curve_flags);

   // Load curve params
   status = ba414e_load_curve(curve, size, BA414E_BIGEND, 0);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   mem2CryptoRAM_rev(x2s_blk, size, BA414E_MEMLOC_13); // s (pwd)
   mem2CryptoRAM_rev(x2, size, BA414E_MEMLOC_10);
   point2CryptoRAM_rev(X1_in, size, BA414E_MEMLOC_11);
   point2CryptoRAM_rev(X3_in, size, BA414E_MEMLOC_8);
   point2CryptoRAM_rev(X4_in, size, BA414E_MEMLOC_6);

   /* Start BA414 */
   if(ba414e_start_wait_status()) return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(x2s_blk, size, BA414E_MEMLOC_11);
   CryptoRAM2point_rev(GA, size, BA414E_MEMLOC_13);
   if (A.len)
      CryptoRAM2point_rev(A, size, BA414E_MEMLOC_8);

   return CRYPTOLIB_SUCCESS;
}

uint32_t jpake_round2_generate_blk(block_t curve, block_t user_id, block_t pwd_blk, block_t x2, block_t X1_in, block_t X3_in, block_t X4_in, block_t data_out, uint32_t curve_flags, uint32_t size)
{
   uint8_t  id[JPAKE_MAX_ID_SIZE];
   uint8_t  x2s[ECC_MAX_KEY_SIZE];
   uint8_t  GABuff[2*ECC_MAX_KEY_SIZE];
   uint32_t status;

   block_t GA = block_t_convert(GABuff, 2*size);
   block_t x2s_blk = block_t_convert(x2s, size);

   // Copy password
   pad_zeros_blk(x2s_blk, pwd_blk);

   // Copy userid
   memcpy_blkIn(id, user_id, user_id.len);
   user_id = block_t_convert(id, user_id.len);

   status = jpake_round2_compute_points(curve, x2, X1_in, X3_in, X4_in, x2s_blk,
      GA, sx_getNULL_blk(), curve_flags, size);
   if (status)
      return status;

   // /* Generate zero knowledge proof for x2s */
   status = jpake_multiply_generate_zkp_blk(curve, GA, BA414E_MEMLOC_13, x2s_blk,
                                            user_id, data_out, curve_flags, size);
   if(status) return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}

uint32_t jpake_3point_add(block_t curve, block_t X2, block_t X3, block_t X1, block_t GB, uint32_t curve_flags, uint32_t size)
{
   // Set command to enable byte-swap
   ba414e_set_command(BA414E_OPTYPE_JPAKE_3POINT_ADD, size, BA414E_BIGEND, curve_flags);

   // Load curve params
   if(ba414e_load_curve(curve, size, BA414E_BIGEND, 0)) return CRYPTOLIB_CRYPTO_ERR;

   // Set input parameters
   point2CryptoRAM_rev(X2, size, BA414E_MEMLOC_6);
   point2CryptoRAM_rev(X3, size, BA414E_MEMLOC_8);
   point2CryptoRAM_rev(X1, size, BA414E_MEMLOC_11);

   /* Start BA414 */
   if(ba414e_start_wait_status()) return CRYPTOLIB_CRYPTO_ERR;

   // Fetch results
   CryptoRAM2point_rev(GB, size, BA414E_MEMLOC_13);

   return CRYPTOLIB_SUCCESS;
}

uint32_t jpake_round2_verify_blk(block_t curve, block_t user_id, block_t X1, block_t X2, block_t X3, block_t Bzkp, uint32_t curve_flags, uint32_t size)
{
   uint8_t  id[JPAKE_MAX_ID_SIZE];
   uint8_t GBBuff[2*ECC_MAX_KEY_SIZE];

   block_t GB = block_t_convert(GBBuff, 2*size);
   uint32_t status;

   // Copy userid
   memcpy_blkIn(id, user_id, user_id.len);
   user_id = block_t_convert(id, user_id.len);

   status = jpake_3point_add(curve, X2, X3, X1, GB, curve_flags, size);
   if (status)
      return status;

   status = jpake_hash_verify_zkp_blk(GB, BA414E_MEMLOC_13, Bzkp, user_id, curve_flags, size);
   if(status) return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}


uint32_t jpake_generate_session_key_blk(  block_t        key,
                                          block_t        pwd_blk,
                                          block_t        x2_blk,
                                          block_t        X4_blk,
                                          block_t        B_blk,
                                          sx_hash_fct_t  sha_type,
                                          block_t        session_key,
                                          uint32_t       curve_flags,
                                          uint32_t       size)
{
   uint8_t T[ECC_MAX_KEY_SIZE];
   uint8_t x2s[ECC_MAX_KEY_SIZE];
   uint8_t x2[ECC_MAX_KEY_SIZE];
   uint32_t status;

   block_t T_b = block_t_convert(T, size);
   block_t x2_b = block_t_convert(x2, size);
   block_t x2s_b = block_t_convert(x2s, size);

   pad_zeros_blk(x2s_b, pwd_blk);
   pad_zeros_blk(x2_b, x2_blk);

   /* 'x2s = x2 * pwd Mod N' where pwd was stored into x2s */
   status = modular_multiplication(x2, x2s, key.addr+size, x2s, size);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   status = jpake_gen_seskey_blk(key, X4_blk, B_blk, x2_b, x2s_b, T_b, curve_flags, size);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   status = sx_hash_blk(sha_type, T_b, session_key);
   if(status) return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}

#endif
