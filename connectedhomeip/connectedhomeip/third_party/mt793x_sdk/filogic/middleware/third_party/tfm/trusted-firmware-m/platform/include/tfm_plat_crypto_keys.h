/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_CRYPTO_KEYS_H__
#define __TFM_PLAT_CRYPTO_KEYS_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       SoC.
 */

#include <stddef.h>
#include <stdint.h>
#include "tfm_plat_defs.h"
#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure definition to carry pointer and size information about an Elliptic
 * curve key which is stored in a buffer(key_buf) in raw format (without
 * encoding):
 *   - priv_key       Base address of the private key in key_buf. It must be
 *                    present on the device.
 *   - priv_key_size  Size of the private key in bytes.
 *   - pubx_key       Base address of x-coordinate of the public key in key_buf.
 *                    It can be empty, because it can be recomputed based on
 *                    private key.
 *   - pubx_key_size  Length of x-coordinate of the public key in key_buf.
 *                    It can be empty, because it can be recomputed based on
 *                    private key.
 *   - puby_key       Base address of y-coordinate of the public key in key_buf.
 *                    It can be empty, because either it can be recomputed based
 *                    on private key or some curve type works without it.
 *   - puby_key_size  Length of y-coordinate of the public key in key_buf.
 */
struct ecc_key_t {
    uint8_t  *priv_key;
    uint32_t  priv_key_size;
    uint8_t  *pubx_key;
    uint32_t  pubx_key_size;
    uint8_t  *puby_key;
    uint32_t  puby_key_size;
};

#define ROTPK_HASH_LEN (32u) /* SHA256 */

/**
 * Structure to store the hard-coded (embedded in secure firmware) hash of ROTPK
 * for firmware authentication.
 *
 * \note Just temporary solution, hard-coded key-hash values in firmware is not
 *       suited for use in production!
 */
struct tfm_plat_rotpk_t {
    const uint8_t *key_hash;
    const uint8_t  hash_len;
};

/**
 * \brief Gets key material derived from the hardware unique key.
 *
 * \param[in]  label         Label for KDF
 * \param[in]  label_size    Size of the label
 * \param[in]  context       Context for KDF
 * \param[in]  context_size  Size of the context
 * \param[out] key           Buffer to output the derived key material
 * \param[in]  key_size      Requested size of the derived key material and
 *                           minimum size of the key buffer
 *
 * \return Returns error code specified in \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_plat_get_huk_derived_key(const uint8_t *label,
                                                 size_t label_size,
                                                 const uint8_t *context,
                                                 size_t context_size,
                                                 uint8_t *key,
                                                 size_t key_size);

/**
 * \brief Get the initial attestation key
 *
 * The device MUST contain an initial attestation key, which is used to sign the
 * token. Initial attestation service supports elliptic curve signing
 * algorithms. Device maker can decide whether store only the private key on the
 * device or store both (public and private) key. Public key can be recomputed
 * based on private key. Keys must be provided in raw format, just binary data
 * without any encoding (DER, COSE). Caller provides a buffer to copy all the
 * available key components to there. Key components must be copied after
 * each other to the buffer. The base address and the length of each key
 * component must be indicating in the corresponding field of ecc_key
 * (\ref struct ecc_key_t).
 * Curve_type indicates to which curve belongs the key.
 *
 *
 * Keys must be provided in
 *
 * \param[in/out]  key_buf     Buffer to store the initial attestation key.
 * \param[in]      size        Size of the buffer.
 * \param[out]     ecc_key     A structure to carry pointer and size information
 *                             about the initial attestation key, which is
 *                             stored in key_buf.
 * \param[out]     curve_type  The type of the EC curve, which the key belongs
 *                             to according to \ref psa_ecc_curve_t
 *
 * \return Returns error code specified in \ref tfm_plat_err_t
 */
enum tfm_plat_err_t
tfm_plat_get_initial_attest_key(uint8_t          *key_buf,
                                uint32_t          size,
                                struct ecc_key_t *ecc_key,
                                psa_ecc_curve_t  *curve_type);

/**
 * \brief Get the hash of the corresponding Root of Trust Public Key for
 *        firmware authentication.
 *
 * \param[in]      image_id         The identifier of firmware image
 * \param[out]     rotpk_hash       Buffer to store the key-hash in
 * \param[in,out]  rotpk_hash_size  As input the size of the buffer. As output
 *                                  the actual key-hash length.
 */
enum tfm_plat_err_t
tfm_plat_get_rotpk_hash(uint8_t image_id,
                        uint8_t *rotpk_hash,
                        uint32_t *rotpk_hash_size);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_CRYPTO_KEYS_H__ */
