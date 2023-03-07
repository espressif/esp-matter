/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Definition of CCM:
 * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
 * RFC 3610 "Counter with CBC-MAC (CCM)"
 *
 * Related:
 * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <wiced_rtos.h>

#if defined(MBEDTLS_CCM_C)
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"

#include <string.h>

#if defined(MBEDTLS_CCM_ALT)

#define seceng_amicr0_adr 0x00440050
#define seceng_amdar_adr 0x00440068
#define cr_seceng_clk_cfg_adr 0x00320228
#define M_SECENG_CLOCK_ON REG32(cr_seceng_clk_cfg_adr) |= 3;
#define M_SECENG_CLOCK_OFF REG32(cr_seceng_clk_cfg_adr) &= ~3;
#define mbedtls_calloc calloc
#define mbedtls_free free

#define CCM_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_CCM_BAD_INPUT)
#define CCM_VALIDATE(cond) MBEDTLS_INTERNAL_VALIDATE(cond)

static wiced_mutex_t *mutex = NULL;

void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
    CCM_VALIDATE(ctx != NULL);

    if (mutex == NULL)
    {
        mutex = wiced_rtos_create_mutex();
        if (mutex == NULL)
        {
            return;
        }
        wiced_rtos_init_mutex(mutex);
    }
}

int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
                       mbedtls_cipher_id_t  cipher,
                       const unsigned char *key,
                       unsigned int         keybits)
{
    (void)cipher;

    ctx->key_len = keybits >> 3;
    memcpy(ctx->key, key, ctx->key_len);
    return (0);
}

void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_ccm_context));
}

/*
 * Use a software method to calculate tag if zero-length aad
 * Refer to the code in COMPONENT_openthread/src/core/crypto/aes_ccm.cpp
 */
static void calc_tag_with_no_add(mbedtls_ccm_context *ctx,
                                 uint8_t *            iv,
                                 size_t               iv_len,
                                 uint8_t *            text,
                                 size_t               length,
                                 uint8_t *            tag,
                                 size_t               tag_len)
{
    uint8_t             L;
    uint16_t            i;
    uint8_t             mBlock[16];
    uint8_t             mCtr[16];
    uint32_t            len;
    uint16_t            mBlockLength = 0;
    mbedtls_aes_context ecb_mContext;

    mbedtls_aes_init(&ecb_mContext);
    if (mbedtls_aes_setkey_enc(&ecb_mContext, ctx->key, ctx->key_len << 3) != 0)
        return;

    L = 0;

    for (len = length; len; len >>= 8)
    {
        L++;
    }

    if (L <= 1)
    {
        L = 2;
    }

    if (iv_len > 13)
    {
        iv_len = 13;
    }

    // increase L to match nonce len
    if (L < (15 - iv_len))
    {
        L = 15 - iv_len;
    }

    // decrease nonceLength to match L
    if ((uint8_t)iv_len > (15 - L))
    {
        iv_len = 15 - L;
    }

    // setup initial block

    // write flags
    mBlock[0] = ((((tag_len - 2) >> 1) << 3) | (L - 1));

    // write nonce
    memcpy(&mBlock[1], iv, iv_len);

    // write len
    len = length;

    for (i = sizeof(mBlock) - 1; i > iv_len; i--)
    {
        mBlock[i] = len & 0xff;
        len >>= 8;
    }

    // encrypt initial block
    mbedtls_aes_crypt_ecb(&ecb_mContext, MBEDTLS_AES_ENCRYPT, mBlock, mBlock);

    // init counter
    mCtr[0] = L - 1;
    memcpy(&mCtr[1], iv, iv_len);
    memset(&mCtr[iv_len + 1], 0, sizeof(mCtr) - iv_len - 1);

    for (i = 0; i < length; i++)
    {
        if (mBlockLength == sizeof(mBlock))
        {
            mbedtls_aes_crypt_ecb(&ecb_mContext, MBEDTLS_AES_ENCRYPT, mBlock, mBlock);
            mBlockLength = 0;
        }

        mBlock[mBlockLength++] ^= text[i];
    }

    if (mBlockLength != 0)
    {
        mbedtls_aes_crypt_ecb(&ecb_mContext, MBEDTLS_AES_ENCRYPT, mBlock, mBlock);
    }
    mbedtls_aes_crypt_ecb(&ecb_mContext, MBEDTLS_AES_ENCRYPT, mCtr, mCtr);

    for (i = 0; i < tag_len; i++)
    {
        tag[i] = mBlock[i] ^ mCtr[i];
    }
}

static int ccm_auth_crypt(mbedtls_ccm_context *ctx,
                          int                  mode,
                          size_t               length,
                          const unsigned char *iv,
                          size_t               iv_len,
                          const unsigned char *add,
                          size_t               add_len,
                          const unsigned char *input,
                          unsigned char *      output,
                          unsigned char *      tag,
                          size_t               tag_len)
{
    tHW_AES  aes;
    uint8_t  status;
    uint8_t *hw_aes_out, *hw_aes_in;
    uint8_t  aes_mode = HW_AES_MODE_CCM;

    /*
     * Check length requirements: SP800-38C A.1
     * Additional requirement: a < 2^16 - 2^8 to simplify the code.
     * 'length' checked later (when writing it to the first block)
     *
     * Also, loosen the requirements to enable support for CCM* (IEEE 802.15.4).
     */
    if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0)
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    /* Also implies q is within bounds */
    if (iv_len < 7 || iv_len > 13)
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    if (add_len > 0xFF00)
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    if ((mode != HW_AES_ENC) && (mode != HW_AES_DEC))
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    hw_aes_in  = mbedtls_calloc(1, add_len + length + tag_len);
    hw_aes_out = mbedtls_calloc(1, add_len + length + tag_len);
    if ((hw_aes_in == NULL) || (hw_aes_out == NULL))
    {
        if (hw_aes_in)
            mbedtls_free(hw_aes_in);
        if (hw_aes_out)
            mbedtls_free(hw_aes_out);
        return (MBEDTLS_ERR_CCM_BAD_INPUT);
    }

    if (mode == HW_AES_ENC)
    {
        memcpy(hw_aes_in, input, length);
    }
    else
    {
        memcpy(hw_aes_in, input, length);
        memcpy(hw_aes_in + length, tag, tag_len);
    }

    if (length == 0)
        aes_mode = HW_AES_MODE_CBCMAC;
    if (add_len == 0)
        aes_mode = HW_AES_MODE_CTR;

    /* nonce aka iv ; tag aka mic */
    aes.enc_flag    = mode;
    aes.aes_mode    = aes_mode;
    aes.mic_len     = tag_len;
    aes.in_auth_ptr = (uint8_t *)add;
    if (add_len == 0)
        /* In hw_aes_engine, aes.auth_len=0 will be treaded as invalid directly. Add a fake 1-byte here */
        aes.auth_len = 1;
    else
        aes.auth_len = add_len;
    aes.in_msg_ptr   = (uint8_t *)hw_aes_in;
    aes.msg_len      = length;
    aes.key_ptr      = ctx->key;
    aes.key_len      = ctx->key_len;
    aes.nonce_ptr    = (uint8_t *)iv;
    aes.nonce_len    = iv_len;
    aes.out_ptr      = hw_aes_out;
    aes.polling_flag = HW_SECENG_POLLING;

    wiced_rtos_lock_mutex(mutex);

    status = hw_aes_engine(&aes);

    if (mode == HW_AES_ENC)
    {
        /* Use aes.auth_len instead of add_len here. When add_len=0, there is a fake 1-byte aad */
        memcpy(output, hw_aes_out + aes.auth_len, length);
        if (add_len == 0)
            calc_tag_with_no_add(ctx, (uint8_t *)iv, iv_len, hw_aes_in, length, tag, tag_len);
        else
            memcpy(tag, hw_aes_out + add_len + length, tag_len);
    }
    if (mode == HW_AES_DEC)
    {
        /* Use aes.auth_len instead of add_len here. When add_len=0, there is a fake 1-byte aad */
        memcpy(output, hw_aes_out + aes.auth_len, length);
        if (add_len == 0)
        {
            calc_tag_with_no_add(ctx, (uint8_t *)iv, iv_len, output, length, tag, tag_len);
        }
        else if (status == HW_AES_MIC_ERROR)
        {
            /*
             * for decryption, openthread caller does not provide received tag value to this function.
             * It causes hw_aes_engine return mic error. (hw_aes_engine will compare the calculated tag and received
             * tag) The current workaround is read out data and tag directly.
             */
            uint8_t *out_from_reg;
            uint32_t out_mic[4];

            M_SECENG_CLOCK_ON
            out_from_reg = (uint8_t *)REG32(seceng_amdar_adr);
            memcpy(output, out_from_reg, length);
            for (uint8_t i = 0; i<tag_len>> 2; i++)
            {
                out_mic[i] = REG32(seceng_amicr0_adr + i * 4);
            }
            memcpy(tag, out_mic, tag_len);
            M_SECENG_CLOCK_OFF

            status = HW_AES_COMPLETE;
        }
    }

    wiced_rtos_unlock_mutex(mutex);

    mbedtls_free(hw_aes_in);
    mbedtls_free(hw_aes_out);

    if (status != HW_AES_COMPLETE)
    {
        printf("hw_aes fail %d\n", status);
        return (MBEDTLS_ERR_CCM_AUTH_FAILED);
    }

    return (0);
}

/*
 * Authenticated encryption
 */
int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx,
                                     size_t               length,
                                     const unsigned char *iv,
                                     size_t               iv_len,
                                     const unsigned char *add,
                                     size_t               add_len,
                                     const unsigned char *input,
                                     unsigned char *      output,
                                     unsigned char *      tag,
                                     size_t               tag_len)
{
    return (ccm_auth_crypt(ctx, HW_AES_ENC, length, iv, iv_len, add, add_len, input, output, tag, tag_len));
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx,
                                size_t               length,
                                const unsigned char *iv,
                                size_t               iv_len,
                                const unsigned char *add,
                                size_t               add_len,
                                const unsigned char *input,
                                unsigned char *      output,
                                unsigned char *      tag,
                                size_t               tag_len)
{
    if (tag_len == 0)
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    return (mbedtls_ccm_star_encrypt_and_tag(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len));
}

/*
 * Authenticated decryption
 */
int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx,
                                  size_t               length,
                                  const unsigned char *iv,
                                  size_t               iv_len,
                                  const unsigned char *add,
                                  size_t               add_len,
                                  const unsigned char *input,
                                  unsigned char *      output,
                                  const unsigned char *tag,
                                  size_t               tag_len)
{
    int           ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char i;
    int           diff;

    if ((ret = ccm_auth_crypt(ctx, HW_AES_DEC, length, iv, iv_len, add, add_len, input, output, ctx->decrypt_tag,
                              tag_len)) != 0)
    {
        return ret;
    }

    /* input tag NULL means upper layer does not provide authenticated data to compare */
    if (tag == NULL)
    {
        return (0);
    }

    /* Check tag in "constant-time" */
    for (diff = 0, i = 0; i < tag_len; i++)
    {
        diff |= tag[i] ^ ctx->decrypt_tag[i];
    }
    if (diff != 0)
    {
        mbedtls_platform_zeroize(output, length);
        return (MBEDTLS_ERR_CCM_AUTH_FAILED);
    }

    return (0);
}

int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx,
                             size_t               length,
                             const unsigned char *iv,
                             size_t               iv_len,
                             const unsigned char *add,
                             size_t               add_len,
                             const unsigned char *input,
                             unsigned char *      output,
                             const unsigned char *tag,
                             size_t               tag_len)
{
    if (tag_len == 0)
        return (MBEDTLS_ERR_CCM_BAD_INPUT);

    return (mbedtls_ccm_star_auth_decrypt(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len));
}

void mbedtls_ccm_get_decrypt_tag(mbedtls_ccm_context *ctx, const unsigned char *tag, size_t tag_len)
{
    memcpy((void *)tag, ctx->decrypt_tag, tag_len);
}

#endif /* !MBEDTLS_CCM_ALT */

#endif /* MBEDTLS_CCM_C */
