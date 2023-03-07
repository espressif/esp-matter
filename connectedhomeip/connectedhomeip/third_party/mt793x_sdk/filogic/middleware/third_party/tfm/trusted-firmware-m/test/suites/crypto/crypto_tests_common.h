/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CRYPTO_TESTS_COMMON_H__
#define __CRYPTO_TESTS_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "test/framework/test_framework_helpers.h"

/**
 * \brief Size of the key to use in tests in bits
 *
 */
#define BIT_SIZE_TEST_KEY (128)

/**
 * \brief Size of the long key to use in tests in bits
 *
 */
#define BIT_SIZE_TEST_LONG_KEY (256)

/**
 * \brief Macro to extract the size of the key in bytes
 *
 */
#define BYTE_SIZE_TEST_KEY (BIT_SIZE_TEST_KEY/8)

/**
 * \brief Size in bytes of a chunk of data to process
 *
 */
#define BYTE_SIZE_CHUNK (16)

/**
 * \brief Size in bytes of the encryption/decryption buffers
 *
 */
#define ENC_DEC_BUFFER_SIZE (32)

/**
 * \brief Size in bytes of the associated data to authenticate
 *        in AEAD tests
 *
 */
#define ASSOCIATED_DATA_SIZE (24)

/**
 * \brief The maximum allowed key length in bytes used in the
 *        tests
 */
#define TEST_MAX_KEY_LENGTH (64)

/**
 * \brief Tests the key interfaces with different key types
 *
 * \param[in]  key_type PSA key type
 * \param[out] ret      Test result
 *
 */
void psa_key_interface_test(const psa_key_type_t key_type,
                            struct test_result_t *ret);
/**
 * \brief Run block ciphering tests with different algorithms and key types
 *
 * \param[in]  key_type PSA key type
 * \param[in]  alg      PSA algorithm
 * \param[out] ret      Test result
 *
 */
void psa_cipher_test(const psa_key_type_t key_type,
                     const psa_algorithm_t alg,
                     struct test_result_t *ret);
/**
 * \brief Tests invalid key type and algorithm combinations for block ciphers
 *
 * \param[in]  key_type  PSA key type
 * \param[in]  alg       PSA algorithm
 * \param[in]  key_size  Key size
 * \param[out] ret       Test result
 *
 */
void psa_invalid_cipher_test(const psa_key_type_t key_type,
                             const psa_algorithm_t alg,
                             const size_t key_size,
                             struct test_result_t *ret);
/**
 * \brief Tests setup of an unsupported hash algorithm
 *
 * \param[in]  alg PSA algorithm
 * \param[out] ret Test result
 *
 */
void psa_unsupported_hash_test(const psa_algorithm_t alg,
                               struct test_result_t *ret);
/**
 * \brief Tests different hashing algorithms
 *
 * \param[in]  alg PSA algorithm
 * \param[out] ret Test result
 *
 */
void psa_hash_test(const psa_algorithm_t alg,
                   struct test_result_t *ret);
/**
 * \brief Tests setup of an unsupported MAC algorithm
 *
 * \param[in]  key_type PSA key type
 * \param[in]  alg      PSA algorithm
 * \param[out] ret      Test result
 *
 */
void psa_unsupported_mac_test(const psa_key_type_t key_type,
                              const psa_algorithm_t alg,
                              struct test_result_t *ret);
/**
 * \brief Tests different MAC algorithms
 *
 * \param[in]  alg          PSA algorithm
 * \param[in]  use_long_key Flag used to indicate to use the long test key
 * \param[out] ret          Test result
 *
 */
void psa_mac_test(const psa_algorithm_t alg,
                  uint8_t use_long_key,
                  struct test_result_t *ret);
/**
 * \brief Run AEAD tests with different algorithms and key types
 *
 * \param[in]  key_type PSA key type
 * \param[in]  alg      PSA algorithm
 * \param[out] ret      Test result
 *
 */
void psa_aead_test(const psa_key_type_t key_type,
                   const psa_algorithm_t alg,
                   struct test_result_t *ret);
/**
 * \brief Tests invalid key length
 *
 * \param[out] ret Test result
 *
 */
void psa_invalid_key_length_test(struct test_result_t *ret);

/**
 * \brief Tests the policy key interface
 *
 * \param[out] ret Test result
 *
 */
void psa_policy_key_interface_test(struct test_result_t *ret);

/**
 * \brief Tests invalid policy usage
 *
 * \param[out] ret Test result
 *
 */
void psa_policy_invalid_policy_usage_test(struct test_result_t *ret);

/**
 * \brief Tests persistent keys
 *
 * \param[out] ret Test result
 *
 */
void psa_persistent_key_test(psa_key_id_t key_id, struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_TESTS_COMMON__ */
