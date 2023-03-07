/**
 * @file
 * @brief Defines the procedures to make JPAKE operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_JPAKE_ALG_H_
#define SX_JPAKE_ALG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_jpake_alg_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptodma.h"
#include "sx_hash.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_jpake_alg.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_jpake_alg.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief J-Pake protocol round 1 generate zero-knowledge proof
 * @details Alice selects two random numbers x1 and x2 and generates a zkp for both number
 * The input is an ID "my_id" and the output is X1 (=g^x1), X2 (=g^x2),
 * zkp1 (zero-knowledge proof for x1) and zkp2 (zero-knowledge proof for x2)
 * @param  curve       input descriptor pointing to the curve parameters
 * @param  user_id     input descriptor pointing to the id of the user
 * @param  x2          output descriptor pointing for the scalar x2
 * @param  zkp1        output descriptor pointing to zkp1
 * @param  zkp2        output descriptor pointing to zkp2
 * @param  curve_flags flag for curve acceleration (see PK datasheet)
 * @param  size        size of the curve parameters
 * @return 0 if finished successfully
 */

 uint32_t jpake_round1_generate_blk(block_t curve, block_t user_id, block_t x2, block_t zkp1, block_t zkp2, uint32_t curve_flags, uint32_t size) CHECK_RESULT;

/**
 * @brief J-Pake protocol round 1 verify the zero knowledge proof
 * @details Alice receives zkp's from Bob for x3 and x4. Alice needs to verify these zkp:
 * The input (from Bob) is: "his_id", X3 (=X1 from Bob), X4 (=X2 from Bob),
 * zkp1 (zero-knowledge proof for x1) and zkp2 (zero-knowledge proof for x2)
 * @param  curve       input descriptor pointing to the curve parameters
 * @param  my_user_id  input descriptor pointing to the id of the user
 * @param  his_user_id input descriptor pointing to the id of the other user
 * @param  zkp3        input descriptor pointing to the point X3
 * @param  zkp4        input descriptor pointing to the point X4
 * @param  curve_flags flag for curve acceleration (see PK datasheet)
 * @param  size        Operand size
 * @return 0 if zkp is correct, otherwise 1.
 */

 uint32_t jpake_round1_verify_blk(block_t curve, block_t my_user_id, block_t his_user_id, block_t zkp3, block_t zkp4, uint32_t curve_flags, uint32_t size) CHECK_RESULT;

/**
 * @brief J-Pake round 2 generate
 * @details Input is a password. Output is A (g^((x1+x3+x4)*x2*s) and
 * zkp1 (zero-knowledge proof for x2s)
 * @param  curve       input descriptor pointing to the curve parameters
 * @param  user_id     input descriptor pointing to the id of the user
 * @param  pwd_blk     input descriptor pointing to the user password
 * @param  x2          input descriptor pointing to the scalar x2
 * @param  X1_in       input descriptor pointing to the point X1
 * @param  X3_in       input descriptor pointing to the points X3
 * @param  X4_in       input descriptor pointing to the points X4
 * @param  data_out    output descriptor for A and its ZKP
 * @param  curve_flags flag for curve acceleration (see PK datasheet)
 * @param  size        size of the operands
 * @return 0 if finished successfully
 */

 uint32_t jpake_round2_generate_blk(block_t curve, block_t user_id, block_t pwd_blk, block_t x2, block_t X1_in, block_t X3_in, block_t X4_in, block_t data_out, uint32_t curve_flags, uint32_t size) CHECK_RESULT;

/**
 * @brief Verify J-pake round 2 data
 * @details input is B (g^((x1+x2+x3)*x4*s)) received from Bob and
 * zkp2 (zero-knowledge proof for x4s)
 * @param  curve         input descriptor pointing to the curve parameters
 * @param  user_id     input descriptor pointing to the id of the user
 * @param  X1       input descriptor pointing to the point X1
 * @param  X2       input descriptor pointing to the point X2
 * @param  X3       input descriptor pointing to the point X3
 * @param  Bzkp       input descriptor pointing to the point B and zkp
 * @param  curve_flags flag for curve acceleration (see PK datasheet)
 * @param size      size of the operands
 * @return 0 if data is correct, otherwise 1
 */

 uint32_t jpake_round2_verify_blk(block_t curve, block_t user_id, block_t X1, block_t X2, block_t X3, block_t Bzkp, uint32_t curve_flags, uint32_t size) CHECK_RESULT;

/**
 * @brief J-pake generate session key
 * @details output key = hashed session key to be used by alice and bob
 * @param p_data J-pake data structure
 * @param  curve       input descriptor pointing to the curve parameters
 * @param  pwd_blk     input descriptor pointing to the user password
 * @param  x2_blk      input descriptor pointing to the scalar x2
 * @param  X4_blk      input descriptor pointing to the point X4
 * @param  B_blk       input descriptor pointing to the point B
 * @param  sha_type    function type to use to hash the session key
 * @param  session_key output descriptor for the generated session key
 * @param  curve_flags flag for curve acceleration (see PK datasheet)
 * @param  size        Size of the operands
 * @return 0 if finished
 */
uint32_t jpake_generate_session_key_blk(block_t curve, block_t pwd_blk, block_t x2_blk, block_t X4_blk, block_t B_blk, sx_hash_fct_t  sha_type, block_t session_key, uint32_t curve_flags, uint32_t size) CHECK_RESULT;

/**
 * @brief Compute GB = X1 + X2 + X3
 * @param  curve        curve parameters
 * @param  X1           point X1
 * @param  X2           point X2
 * @param  X3           point X3
 * @param  GB           output
 * @param  curve_flags  hardware curve command flags
 * @param  size         curve operands size in bytes
 * @return 0 on success
 */
uint32_t jpake_3point_add(block_t curve, block_t X2, block_t X3, block_t X1, block_t GB, uint32_t curve_flags, uint32_t size);

/**
 * @brief Compute points and x2 * s for ECJPAKE round 2
 * @details Computes: GA = X1 + X2 + X3; x2s = x2 * s; A = x2s * GA
 * @param  curve        curve parameters
 * @param  x2           big number x2 in big endian format
 * @param  X1_in        point X1
 * @param  X3_in        point X3
 * @param  X4_in        point X4
 * @param  x2s_blk      input password; output = x2 * s
 * @param  GA           output point X1 + X2 + X3
 * @param  A            output point x2s * GA
 * @param  curve_flags  hardware curve command flags
 * @param  size         curve operands size in bytes
 * @return 0 on success
 */
uint32_t jpake_round2_compute_points(block_t curve, block_t x2, block_t X1_in, block_t X3_in, block_t X4_in, block_t x2s_blk, block_t GA, block_t A, uint32_t curve_flags, uint32_t size);

/**
 * @brief Verify a ECKPAKE Zero Knowledge Proof
 * @param  G            point X1 + X2 + X3
 * @param  Gloc		CryptoRAM location where G is preloaded or ~0 if not.
 * @param  X_b          Point X
 * @param  V_b          Point V base of the ZKP
 * @param  z_b          Value in big endian part of ZKP
 * @param  his_id	User ID of the creator of the ZKP
 * @param  curve_flags  curve command flags
 * @param  size         curve operands size in bytes
 * @return 0 on success
 */
uint32_t jpake_hash_verify_zkp(block_t G, uint32_t Gloc, block_t X_b, block_t V_b, block_t z_b, block_t his_id, uint32_t curve_flags, uint32_t size);

/**
 * @brief Compute ZKP from the 'user_id' and 2 key pairs (x, X) and (v, V) for
 * a given 'curve'.
 *
 * Steps in the computation:
 *  1. Compute h = hash(G, V, X, my_id)
 *  2. Compute r = v - x.h mod n
 *
 * The zkp is the point V and the value r.
 *
 * @param G  Point G
 * @param x  ZKP private key x big endian value.
 *           If empty block, it should be preloaded in location 11.
 * @param X  ZKP Point X from private key x.
 * @param v  ZKP private key v big endian value
 *           If empty block, it should be preloaded in location 8.
 * @param V  ZKP Point V from private key v.
 * @param my_id User ID of the creator of the ZKP
 * @param r  big endian value proof in the ZKP
 * @param curve_flags curve command flags
 * @param opsize  curve operands size in bytes
 */
uint32_t jpake_hash_create_zkp(block_t G, block_t x, block_t X, block_t v, block_t V, block_t my_id, block_t r, uint32_t curve_flags, uint32_t opsize);

#if defined(GP_DIVERSITY_JUMPTABLES)
/**
 * @brief subroutine calling the crypto layer (sx_jpake) generate session key function
 * @details T = (B - x2s*X4)*x2
 * @param x2            Alice's exponent x2
 * @param x2s           Alice's exponent x2.s
 * @param X4            Bob's g^x2
 * @param B             Bob's g^((x1+x2+x3)*x4*s)
 * @param T             output point
 * @param curve_flags   flag for curve acceleration (see PK datasheet)
 * @param curve         curve parameters
 * @param size          size of the operands
 * @return 0 if finished successfull
 */
uint32_t jpake_gen_seskey_blk(block_t curve, block_t X4, block_t B, block_t x2, block_t x2s, block_t T, uint32_t curve_flags, uint32_t size);


/**
 * @brief Subroutine perform sha256(G'|V'|X'|i'), where x' is the X9.62 encoding of x and | is concatenation
 * @param G  input of hash
 * @param V  input of hash
 * @param X  input of hash
 * @param i  input of hash
 * @param h  ouput digest
 * @param h_size size of \p h buffer
 * @return CRYPTOLIB_SUCCESS if successful
 */
uint32_t jpake_hash_GVXU(block_t G, block_t V, block_t X, block_t i, uint8_t *h, const size_t h_size);


/**
 * @brief Subroutine to hash and verify zkp
 * @details V =?= G*r + X*H(G, V, X, id)
 * @param G             Generator to check the hash
 * @param Gloc          BA414EP Location were the generator is pre-loaded
 * @param zkp           zero-knowledge proof and X
 * @param his_id        Bob's id
 * @param curve_flags   flag for curve acceleration (see PK datasheet)
 * @param size          operand size
 * @return 0 if finsihed successfull
 *
 */
uint32_t jpake_hash_verify_zkp_blk(block_t G, uint8_t Gloc, block_t zkp, block_t his_id, uint32_t curve_flags, uint32_t size);

/**
 * @brief Execute point multiplication and generate zero-knowledge proof
 * @details X = x*G
 * @param curve         curve parameter
 * @param G             Generator to check the hash
 * @param Gloc          BA414EP Location were the generator is pre-loaded
 * @param x             x exponent (private)
 * @param my_id         Alice's id
 * @param zkp           zero knowledge proof to generate
 * @param curve_flags   flag for curve acceleration (see PK datasheet)
 * @param size          size of the curve parameters
 * @return 0 if finished successfull
 */
uint32_t jpake_multiply_generate_zkp_blk(block_t curve, block_t G, uint8_t Gloc, block_t x, block_t my_id, block_t zkp, uint32_t curve_flags, uint32_t size);

#endif // GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#define BA414E_JPAKE_DATA_SIZE   32
#define BA414E_JPAKE_NB_DATA_PT  2
#define BA414E_JPAKE_NB_DATA_ZKP 3

#define JPAKE_MAX_ID_SIZE 32

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
