/*
 *  t_cose_make_psa_test_key.c
 *
 * Copyright 2019-2020, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


#include "t_cose_make_test_pub_key.h" /* The interface implemented here */
#include "t_cose_standard_constants.h"
#include "psa/crypto.h"


/* Here's the auto-detect and manual override logic for managing PSA
 * Crypto API compatibility.
 *
 * PSA_GENERATOR_UNBRIDLED_CAPACITY happens to be defined in MBed
 * Crypto 1.1 and not in MBed Crypto 2.0 so it is what auto-detect
 * hinges off of.
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20 can be defined to force
 * setting to MBed Crypto 2.0
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 can be defined to force
 * setting to MBed Crypt 1.1. It is also what the code below hinges
 * on.
 */
#if defined(PSA_GENERATOR_UNBRIDLED_CAPACITY) && !defined(T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20)
#define T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
#endif


/*
 * Some hard coded keys for the test cases here.
 */
#define PRIVATE_KEY_prime256v1 \
0xf1, 0xb7, 0x14, 0x23, 0x43, 0x40, 0x2f, 0x3b, 0x5d, 0xe7, 0x31, 0x5e, 0xa8, \
0x94, 0xf9, 0xda, 0x5c, 0xf5, 0x03, 0xff, 0x79, 0x38, 0xa3, 0x7c, 0xa1, 0x4e, \
0xb0, 0x32, 0x86, 0x98, 0x84, 0x50

#define PRIVATE_KEY_secp384r1 \
0x03, 0xdf, 0x14, 0xf4, 0xb8, 0xa4, 0x3f, 0xd8, 0xab, 0x75, 0xa6, 0x04, 0x6b, \
0xd2, 0xb5, 0xea, 0xa6, 0xfd, 0x10, 0xb2, 0xb2, 0x03, 0xfd, 0x8a, 0x78, 0xd7, \
0x91, 0x6d, 0xe2, 0x0a, 0xa2, 0x41, 0xeb, 0x37, 0xec, 0x3d, 0x4c, 0x69, 0x3d, \
0x23, 0xba, 0x2b, 0x4f, 0x6e, 0x5b, 0x66, 0xf5, 0x7f

#define PRIVATE_KEY_secp521r1 \
0x00, 0x45, 0xd2, 0xd1, 0x43, 0x94, 0x35, 0xfa, 0xb3, 0x33, 0xb1, 0xc6, 0xc8, \
0xb5, 0x34, 0xf0, 0x96, 0x93, 0x96, 0xad, 0x64, 0xd5, 0xf5, 0x35, 0xd6, 0x5f, \
0x68, 0xf2, 0xa1, 0x60, 0x65, 0x90, 0xbb, 0x15, 0xfd, 0x53, 0x22, 0xfc, 0x97, \
0xa4, 0x16, 0xc3, 0x95, 0x74, 0x5e, 0x72, 0xc7, 0xc8, 0x51, 0x98, 0xc0, 0x92, \
0x1a, 0xb3, 0xb8, 0xe9, 0x2d, 0xd9, 0x01, 0xb5, 0xa4, 0x21, 0x59, 0xad, 0xac, \
0x6d


/*
 * Public function, see t_cose_make_test_pub_key.h
 */
enum t_cose_err_t make_ecdsa_key_pair(int32_t            cose_algorithm_id,
                                      struct t_cose_key *key_pair)
{
    psa_key_type_t      key_type;
    psa_status_t        crypto_result;
    psa_key_handle_t    key_handle;
    psa_algorithm_t     key_alg;
    const uint8_t      *private_key;
    size_t              private_key_len;

    static const uint8_t private_key_256[] = {PRIVATE_KEY_prime256v1};
    static const uint8_t private_key_384[] = {PRIVATE_KEY_secp384r1};
    static const uint8_t private_key_521[] = {PRIVATE_KEY_secp521r1};

    /* There is not a 1:1 mapping from alg to key type, but
     * there is usually an obvious curve for an algorithm. That
     * is what this does.
     */

    #ifdef T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
    #define PSA_KEY_TYPE_ECC_KEY_PAIR PSA_KEY_TYPE_ECC_KEYPAIR
    #endif /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */

    switch(cose_algorithm_id) {
    case COSE_ALGORITHM_ES256:
        private_key     = private_key_256;
        private_key_len = sizeof(private_key_256);
        key_type        = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP256R1);
        key_alg         = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
        break;

    case COSE_ALGORITHM_ES384:
        private_key     = private_key_384;
        private_key_len = sizeof(private_key_384);
        key_type        = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP384R1);
        key_alg         = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
        break;

    case COSE_ALGORITHM_ES512:
        private_key     = private_key_521;
        private_key_len = sizeof(private_key_521);
        key_type        = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP521R1);
        key_alg         = PSA_ALG_ECDSA(PSA_ALG_SHA_512);
        break;

    default:
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }


    /* OK to call this multiple times */
    crypto_result = psa_crypto_init();
    if(crypto_result != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }


    /* When importing a key with the PSA API there are two main
     * things to do.
     *
     * First you must tell it what type of key it is as this
     * cannot be discovered from the raw data. The variable
     * key_type contains that information including the EC curve. This is sufficient
     * for psa_import_key() to succeed, but you probably want
     * actually use the key.
     *
     * Second, you must say what algorithm(s) and operations
     * the key can be used as the PSA Crypto Library has
     * policy enforcement.
     *
     * How this is done varies quite a lot in the newer
     * PSA Crypto API compared to the older.
     */

#ifdef T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
    /* Allocate for the key pair in the Crypto service */
    crypto_result = psa_allocate_key(&key_handle);
    if (crypto_result != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    /* Say what algorithm and operations the key can be used with / for */
    psa_key_policy_t policy = psa_key_policy_init();
    psa_key_policy_set_usage(&policy,
                             PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY,
                             key_alg);
    crypto_result = psa_set_key_policy(key_handle, &policy);
    if (crypto_result != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    /* Import the private key. psa_import_key() automatically
     * generates the public key from the private so no need to import
     * more than the private key. (With ECDSA the public key is always
     * deterministically derivable from the private key).
     */
    /* key_type has the type of key including the EC curve */
    crypto_result = psa_import_key(key_handle,
                                   key_type,
                                   private_key,
                                   private_key_len);

#else /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */
    psa_key_attributes_t key_attributes;

    key_attributes = psa_key_attributes_init();

    /* Say what algorithm and operations the key can be used with / for */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, key_alg);

    /* The type of key including the EC curve */
    psa_set_key_type(&key_attributes, key_type);

    /* Import the private key. psa_import_key() automatically
     * generates the public key from the private so no need to import
     * more than the private key. (With ECDSA the public key is always
     * deterministically derivable from the private key).
     */
    crypto_result = psa_import_key(&key_attributes,
                                    private_key,
                                    private_key_len,
                                   &key_handle);

#endif /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */

    if (crypto_result != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    key_pair->k.key_handle = key_handle;
    key_pair->crypto_lib   = T_COSE_CRYPTO_LIB_PSA;

    return T_COSE_SUCCESS;
}


/*
 * Public function, see t_cose_make_test_pub_key.h
 */
void free_ecdsa_key_pair(struct t_cose_key key_pair)
{
   psa_close_key((psa_key_handle_t)key_pair.k.key_handle);
}


/*
 * Public function, see t_cose_make_test_pub_key.h
 */
int check_for_key_pair_leaks()
{
#if defined(T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11) || defined(T_COSE_USE_PSA_CRYPTO_FROM_TFM)
    /* No way to check for leaks with:
     *   - MBED Crypto 1.1
     *   - TF-M Crypto service does not expose this interface
     */
    return 0;

#else
    mbedtls_psa_stats_t stats;

    mbedtls_psa_get_stats(&stats);

    return (int)(stats.volatile_slots +
           stats.persistent_slots +
           stats.external_slots +
           stats.half_filled_slots +
           stats.cache_slots);

#endif /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */
}

