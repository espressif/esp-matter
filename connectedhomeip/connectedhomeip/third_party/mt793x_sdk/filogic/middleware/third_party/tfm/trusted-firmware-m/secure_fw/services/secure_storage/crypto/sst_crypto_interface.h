/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_CRYPTO_INTERFACE_H__
#define __SST_CRYPTO_INTERFACE_H__

#include <stddef.h>
#include <stdint.h>

#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SST_KEY_LEN_BYTES  16
#define SST_TAG_LEN_BYTES  16
#define SST_IV_LEN_BYTES   12

/* Union containing crypto policy implementations. The ref member provides the
 * reference implementation. Further members can be added to the union to
 * provide alternative implementations.
 */
union sst_crypto_t {
    struct {
        uint8_t tag[SST_TAG_LEN_BYTES]; /*!< MAC value of AEAD object */
        uint8_t iv[SST_IV_LEN_BYTES];   /*!< IV value of AEAD object */
    } ref;
};

/**
 * \brief Initializes the crypto engine.
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_init(void);

/**
 * \brief Sets the key to use for crypto operations for the current client.
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_setkey(void);

/**
 * \brief Destroys the transient key used for crypto operations.
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_destroykey(void);

/**
 * \brief Encrypts and tags the given plaintext data.
 *
 * \param[in,out] crypto    Pointer to the crypto union
 * \param[in]     add       Pointer to the associated data
 * \param[in]     add_len   Length of the associated data
 * \param[in]     in        Pointer to the input data
 * \param[in]     in_len    Length of the input data
 * \param[out]    out       Pointer to the output buffer for encrypted data
 * \param[in]     out_size  Size of the output buffer
 * \param[out]    out_len   On success, the length of the output data
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_encrypt_and_tag(union sst_crypto_t *crypto,
                                        const uint8_t *add,
                                        size_t add_len,
                                        const uint8_t *in,
                                        size_t in_len,
                                        uint8_t *out,
                                        size_t out_size,
                                        size_t *out_len);

/**
 * \brief Decrypts and authenticates the given encrypted data.
 *
 * \param[in]  crypto    Pointer to the crypto union
 * \param[in]  add       Pointer to the associated data
 * \param[in]  add_len   Length of the associated data
 * \param[in]  in        Pointer to the input data
 * \param[in]  in_len    Length of the input data
 * \param[out] out       Pointer to the output buffer for decrypted data
 * \param[in]  out_size  Size of the output buffer
 * \param[out] out_len   On success, the length of the output data
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_auth_and_decrypt(const union sst_crypto_t *crypto,
                                         const uint8_t *add,
                                         size_t add_len,
                                         uint8_t *in,
                                         size_t in_len,
                                         uint8_t *out,
                                         size_t out_size,
                                         size_t *out_len);

/**
 * \brief Generates authentication tag for given data.
 *
 * \param[in,out] crypto   Pointer to the crypto union
 * \param[in]     add      Pointer to the data to authenticate
 * \param[in]     add_len  Length of the data to authenticate
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_generate_auth_tag(union sst_crypto_t *crypto,
                                          const uint8_t *add,
                                          uint32_t add_len);

/**
 * \brief Authenticate given data against the tag.
 *
 * \param[in] crypto   Pointer to the crypto union
 * \param[in] add      Pointer to the data to authenticate
 * \param[in] add_len  Length of the data to authenticate
 *
 * \return Returns values as described in \ref psa_status_t
 */
psa_status_t sst_crypto_authenticate(const union sst_crypto_t *crypto,
                                     const uint8_t *add,
                                     uint32_t add_len);

/**
 * \brief Provides current IV value to crypto layer.
 *
 * \param[in] crypto  Pointer to the crypto union
 */
void sst_crypto_set_iv(const union sst_crypto_t *crypto);

/**
 * \brief Gets a new IV value into the crypto union.
 *
 * \param[out] crypto  Pointer to the crypto union
 */
void sst_crypto_get_iv(union sst_crypto_t *crypto);

#ifdef __cplusplus
}
#endif

#endif /* __SST_CRYPTO_INTERFACE_H__ */
