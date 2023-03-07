/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MBEDTLS_CCM_INTERNAL_H
#define _MBEDTLS_CCM_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ccm.h"

int mbedtls_ccm_get_security_level(uint8_t sizeOfT, uint8_t *pSecurityLevel);

void mbedtls_ccm_init_int(mbedtls_ccm_context *ctx);

int mbedtls_ccm_setkey_int(mbedtls_ccm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key, unsigned int keybits);

void mbedtls_ccm_free_int(mbedtls_ccm_context *ctx);

int mbedtls_ccm_encrypt_and_tag_int(mbedtls_ccm_context *ctx,
                                    size_t length,
                                    const unsigned char *iv,
                                    size_t iv_len,
                                    const unsigned char *add,
                                    size_t add_len,
                                    const unsigned char *input,
                                    unsigned char *output,
                                    unsigned char *tag,
                                    size_t tag_len,
                                    uint32_t ccmMode);

int mbedtls_ccm_auth_decrypt_int(mbedtls_ccm_context *ctx,
                                 size_t length,
                                 const unsigned char *iv,
                                 size_t iv_len,
                                 const unsigned char *add,
                                 size_t add_len,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 const unsigned char *tag,
                                 size_t tag_len,
                                 uint32_t ccmMode);
#ifdef __cplusplus
}
#endif

#endif /* _MBEDTLS_CCM_INTERNAL_H */
