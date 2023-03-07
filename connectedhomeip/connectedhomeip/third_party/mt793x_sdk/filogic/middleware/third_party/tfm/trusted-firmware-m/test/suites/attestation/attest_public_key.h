/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTEST_PUBLIC_KEY_H__
#define __ATTEST_PUBLIC_KEY_H__

#include "psa/initial_attestation.h"
#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register the initial attestation public key to Crypto service to
 *        verify the signature of the token.
 *
 * \param[out] key_handle_public  Pointer to the key handle allocated for the
 *                                public key
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key was registered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key could not be registered.
 */
enum psa_attest_err_t attest_register_initial_attestation_public_key(
                                          psa_key_handle_t *key_handle_public);

/**
 * \brief Unregister the initial attestation public key from Crypto service
 *        to do not occupy key slot.
 *
 * \param[in] key_handle_public  Key handle associated to the public key
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key was unregistered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key could not be unregistered.
 */
enum psa_attest_err_t attest_unregister_initial_attestation_public_key(
                                           psa_key_handle_t key_handle_public);
#ifdef __cplusplus
}
#endif

#endif /* __ATTEST_PUBLIC_KEY_H__ */
