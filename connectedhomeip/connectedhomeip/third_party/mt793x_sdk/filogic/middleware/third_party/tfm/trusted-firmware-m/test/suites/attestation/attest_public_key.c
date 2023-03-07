/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attest_public_key.h"
#include "psa/crypto.h"
#include <stdint.h>
#include "attestation.h"

/*!
 * \def ECC_CURVE_SECP256R1_PUBLIC_KEY_LENGTH
 *
 * \brief Calculates the size of ECC public key in bytes based on the bit size
 *        of the curve
 */
#define ECC_CURVE_SECP256R1_PUBLIC_KEY_LENGTH (1 + 2 * PSA_BITS_TO_BYTES(256))

/*!
 *   Byte string representation of ECC public key according to
 *   psa_export_public_key() in interface/include/psa/crypto.h
 */
struct ecc_public_key_t {
    const uint8_t a;
    uint8_t public_key[]; /* X-coordinate || Y-coordinate */
};

/*!
 * \var public_key_registered
 *
 * \brief Indicates whether the public part of the attestation key pair was
 *        registered to Crypto service or not.
 */
static uint32_t public_key_registered = 0;

enum psa_attest_err_t
attest_register_initial_attestation_public_key(psa_key_handle_t *public_key)
{
    psa_status_t res;
    psa_key_attributes_t key_attributes = psa_key_attributes_init();
    uint8_t public_key_buff[ECC_CURVE_SECP256R1_PUBLIC_KEY_LENGTH] = {0};
    size_t public_key_len;
    psa_ecc_curve_t ecc_curve;

    /* Public key should be unregistered at this point */
    if (public_key_registered != 0) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    res = tfm_initial_attest_get_public_key(public_key_buff,
                                            sizeof(public_key_buff),
                                            &public_key_len,
                                            &ecc_curve);
    if (res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Setup the key usage flags, algorithm and key type for public key */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_curve));

    /* Register public key to Crypto service */
    res = psa_import_key(&key_attributes,
                         (const uint8_t *)&public_key_buff,
                         public_key_len,
                         public_key);

    if (res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    public_key_registered = 1;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_unregister_initial_attestation_public_key(psa_key_handle_t public_key)
{
    psa_status_t crypto_res;

    if (public_key_registered != 1) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    crypto_res = psa_destroy_key(public_key);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    public_key_registered = 0;

    return PSA_ATTEST_ERR_SUCCESS;
}
