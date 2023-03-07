/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CRYPTO_HW_H__
#define __CRYPTO_HW_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The CC312 uses some the MBEDTLS heap buffer, so it needs increasing in size
 * to allow attest crypto operations to still function.
 */
#ifndef TFM_CRYPTO_ENGINE_BUF_SIZE
#define TFM_CRYPTO_ENGINE_BUF_SIZE (0x4000) /* 16KB for EC signing in attest */
#endif

/* Offsets are specified in 32-bit words */
#define CC_OTP_ATTESTATION_KEY_OFFSET               (0x30UL)
#define CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS        (8)
#define CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET    (0x2FUL)

/**
 * \brief Initialize the CC312 crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_init(void);

/**
 * \brief Deallocate the CC312 crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_finish(void);

/*
 * \brief  This function performs key derivation
 *
 * \param[in]  label         Label for KDF
 * \param[in]  label_size    Size of the label
 * \param[in]  context       Context for KDF
 * \param[in]  context_size  Size of the context
 * \param[out] key           Buffer to output the derived key material
 * \param[in]  key_size      Requested size of the derived key material and
 *                           minimum size of the key buffer
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_huk_derive_key(const uint8_t *label,
                                         size_t label_size,
                                         const uint8_t *context,
                                         size_t context_size,
                                         uint8_t *key,
                                         size_t key_size);

/**
 * \brief Write the crypto keys to One-Time-Programmable memory
 *
 * The following keys will be provisioned:
 *  - Hardware Unique Key (HUK)
 *  - Hash of ROTPK
 *  - Attestation private key
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_otp_provisioning(void);

/**
 * \brief Retrieve the hash of ROTPK from OTP
 *
 * \param[in]      image_id         The identifier of firmware image
 * \param[out]     rotpk_hash       Buffer to store the key-hash in
 * \param[in,out]  rotpk_hash_size  As input the size of the buffer. As output
 *                                  the actual key-hash length.
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_get_rotpk_hash(uint8_t image_id,
                                         uint8_t *rotpk_hash,
                                         uint32_t *rotpk_hash_size);

/**
 * \brief Retrieve the attestation private key from OTP
 *
 * \param[out]     buf   Buffer to store the key in
 * \param[in,out]  size  As input the size of the buffer. As output the actual
 *                       size of the key in bytes.
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_get_attestation_private_key(uint8_t *buf,
                                                      uint32_t *size);

/**
 * \brief Retrieve the device lifecycle
 *
 * \param[out]  lcs  Pointer to store lifecycle state
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_get_lcs(uint32_t *lcs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRYPTO_HW_H__ */
