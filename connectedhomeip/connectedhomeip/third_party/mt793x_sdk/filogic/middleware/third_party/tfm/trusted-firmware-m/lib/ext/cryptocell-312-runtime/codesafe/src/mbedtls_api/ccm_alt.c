/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Definition of CCM:
 * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
 * RFC 3610 "Counter with CBC-MAC (CCM)"
 *
 * Related:
 * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
 */
#include "ccm_alt.h"

#if defined(MBEDTLS_CCM_C) && defined (MBEDTLS_CCM_ALT)

#include "cc_pal_types.h"
#include "mbedtls_common.h"

#include "mbedtls_ccm_internal.h"
#include "mbedtls_ccm_common.h"


/************************ Public Functions **********************/
/*
 * Initialize context
 */
void mbedtls_ccm_init( mbedtls_ccm_context *ctx )
{
    mbedtls_ccm_init_int(ctx);
}

int mbedtls_ccm_setkey( mbedtls_ccm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits)
{
    return mbedtls_ccm_setkey_int(ctx, cipher, key, keybits);
}

/*
 * Free context
 */
void mbedtls_ccm_free( mbedtls_ccm_context *ctx )
{
    mbedtls_ccm_free_int(ctx);
}

/*
 * Authenticated encryption
 */
int mbedtls_ccm_encrypt_and_tag( mbedtls_ccm_context *ctx,
                                 size_t length,
                                 const unsigned char *iv,
                                 size_t iv_len,
                                 const unsigned char *add,
                                 size_t add_len,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 unsigned char *tag,
                                 size_t tag_len )
{
    return mbedtls_ccm_encrypt_and_tag_int(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len, MBEDTLS_AESCCM_MODE_CCM);
}

/*
 * Authenticated decryption
 */
int mbedtls_ccm_auth_decrypt( mbedtls_ccm_context *ctx,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *input,
                              unsigned char *output,
                              const unsigned char *tag,
                              size_t tag_len )
{


    return mbedtls_ccm_auth_decrypt_int( ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len, MBEDTLS_AESCCM_MODE_CCM);
}

int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx,
                                     size_t length,
                                     const unsigned char *iv,
                                     size_t iv_len,
                                     const unsigned char *add,
                                     size_t add_len,
                                     const unsigned char *input,
                                     unsigned char *output,
                                     unsigned char *tag,
                                     size_t tag_len)
{
    return mbedtls_ccm_encrypt_and_tag_int(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len, MBEDTLS_AESCCM_MODE_STAR);
}

int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx,
                                  size_t length,
                                  const unsigned char *iv,
                                  size_t iv_len,
                                  const unsigned char *add,
                                  size_t add_len,
                                  const unsigned char *input,
                                  unsigned char *output,
                                  const unsigned char *tag,
                                  size_t tag_len)
{
    return mbedtls_ccm_auth_decrypt_int(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len, MBEDTLS_AESCCM_MODE_STAR);

}



#endif /* defined(MBEDTLS_CCM_C) && defined (MBEDTLS_CCM_ALT) */
