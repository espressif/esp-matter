/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_KEY_H__
#define __ATTESTATION_KEY_H__

#include "psa/initial_attestation.h"
#include "psa/crypto.h"
#include "q_useful_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register the initial attestation private key to Crypto service. Loads
 *        the public key if the key has not already been loaded.
 *
 * \note  Private key MUST be present on the device, otherwise initial
 *        attestation token cannot be signed.
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key(s) was registered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key(s) could not be registered.
 */
enum psa_attest_err_t
attest_register_initial_attestation_key();

/**
 * \brief Unregister the initial attestation private key from Crypto service
 *        to do not occupy key slot.
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key(s) was unregistered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key(s) could not be unregistered.
 */
enum psa_attest_err_t
attest_unregister_initial_attestation_key();

/**
 * \brief Get a handle to the attestion private key.
 *
 * \param[out] key_handle Key handle for private key
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Private key was successfully returned.
 * \retval  PSA_ATTEST_ERR_GENERAL   Private key could not be returned.
 */

enum psa_attest_err_t
attest_get_initial_attestation_private_key_handle(psa_key_handle_t *key_handle);

/**
 * \brief Get the public key derived from the initial attestation private key.
 *
 * \param[out] public_key       Pointer to public key buffer.
 * \param[out] public_key_len   Size of public key in bytes.
 * \param[out] public_key_curve Type of the curve that is used in the public
 *                              key.
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Public key was successfully returned.
 * \retval  PSA_ATTEST_ERR_GENERAL   Public key could not be returned.
 */

enum psa_attest_err_t
attest_get_initial_attestation_public_key(uint8_t **public_key,
                                          size_t *public_key_len,
                                          psa_ecc_curve_t *public_key_curve);

/**
 * \brief Get the attestation key ID. It is the hash (SHA256) of the COSE_Key
 *        encoded attestation public key.
 *
 * \param[out] attest_key_id  Pointer and length of the key id.
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key id calculated successfully.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key id calculation failed.

 */
enum psa_attest_err_t
attest_get_initial_attestation_key_id(struct q_useful_buf_c *attest_key_id);

#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_KEY_H__ */
