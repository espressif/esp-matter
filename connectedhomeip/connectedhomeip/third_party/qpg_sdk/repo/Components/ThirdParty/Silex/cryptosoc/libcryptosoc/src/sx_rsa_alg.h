/**
 * @file
 * @brief Defines RSA algorithmic functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_RSA_ALG_H
#define SX_RSA_ALG_H


#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "sx_hash.h"
#include "sx_rsa_pad.h"

#if (RSA_ENABLED)

/**
* @brief RSA encryption (block_t as parameters)
* @param padding_type defines the padding to use to encode message
* @param message message to be encrypted
* @param n block_t to rp*rq (multiplication of two random primes)
* @param public_expo pub exponent
* @param result block_t to result buffer
* @param  hashType hash type used in OAEP padding type
* @return 0 if successfull
*/
uint32_t rsa_encrypt_blk(rsa_pad_types_t padding_type,
                    block_t message,
                    block_t n,
                    block_t public_expo,
                    block_t result ,
                    sx_hash_fct_t hashType) CHECK_RESULT;

/**
* @brief RSA decryption (block_t as parameters)
* @param padding_type defines the padding to use to encode message
* @param cipher cipher to be decrypted
* @param n block_t to rp*rq (multiplication of two random primes)
* @param private_key priv key
* @param result block_t to result buffer
* @param crt 1 if crt decryption (in that case, private_key contains priv parameters)
* @param msg_len pointer to the message length (for padded messages)
* @param  hashType hash type used in OAEP padding type
* @return 0 if successfull
*/
uint32_t rsa_decrypt_blk(rsa_pad_types_t padding_type,
                    block_t cipher,
                    block_t n,
                    block_t private_key,
                    block_t result,
                    uint32_t crt,
                    uint32_t *msg_len,
                    sx_hash_fct_t hashType) CHECK_RESULT;

/**
* @brief RSA Signature generation (block_t as parameters)
* @param sha_type Hash type to use for signature
* @param padding_type Padding type to use (see rsa_pad_types_e)
* @param message message to be signed
* @param result block_t to result buffer
* @param n block_t to rp*rq (multiplication of two random primes)
* @param private_key block_t to priv key
* @param crt if equal to 1, uses crt parameters
* @param salt_length   Expected length of the salt used in PSS padding_type
* @return 0 if successfull
*/
uint32_t rsa_signature_generation_blk( sx_hash_fct_t sha_type,
                                  rsa_pad_types_t padding_type,
                                  block_t message,
                                  block_t result,
                                  block_t n,
                                  block_t private_key,
                                  uint32_t crt,
                                  uint32_t salt_length) CHECK_RESULT;

/**
 * @brief RSA Signature verification (on block_t)
 * @param sha_type hash function type used for verification
 * @param padding_type defines the padding to use
 * @param message block_t to the message
 * @param result block_t to the result buffer
 * @param n block_t to rp*rq (multiplication of two random primes)
 * @param public_expo block_t to pub exponent
 * @param signature block_t to signature
 * @param salt_length   Expected length of the salt used in PSS padding_type
 * @return 0 if successfull
 */
uint32_t rsa_signature_verification_blk( sx_hash_fct_t sha_type,
                                    rsa_pad_types_t padding_type,
                                    block_t message,
                                    block_t result,
                                    block_t n,
                                    block_t public_expo,
                                    block_t signature,
                                    uint32_t salt_length) CHECK_RESULT;


/**
* @brief RSA priv key generation using block_t
* @param p block_t to p-component
* @param q block_t to q-component
* @param public_expo block_t to pub exponent
* @param n block_t to rp*rq (multiplication of two random primes)
* @param private_key block_t to the priv key
* @param size number of bytes of the parameters
* @param lambda 1 if lambda(n) should be copied to block N (non-CRT only)
* @return 0 if successfull
*/
uint32_t rsa_private_key_generation_blk( block_t p,
                                   block_t q,
                                   block_t public_expo,
                                   block_t n,
                                   block_t private_key,
                                   uint32_t size,
                                   uint32_t lambda ) CHECK_RESULT;

/**
* Generates CRT parameters based on p and q and output them to output
* @param  p      block_t to the p parameter
* @param  q      block_t to the q parameter
* @param  d      block_t to the priv key
* @param  dp     block_t to the priv evelemnt dp
* @param  dq     block_t to the priv element dq
* @param  inv    block_t to the priv element inv
* @return        returns 0 if no error
*/
uint32_t rsa_crt_key_generation_blk(block_t p, block_t q, block_t d, block_t dp, block_t dq, block_t inv) CHECK_RESULT;


#endif
#endif
