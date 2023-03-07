/**
 * \file
 * \brief Crypto abstraction functions for external host side cryptography
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "atca_config.h"
#include "atca_status.h"
#include "crypto/atca_crypto_sw.h"

#ifdef ATCA_WOLFSSL

/** \brief Initialize an AES-GCM context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_encrypt_start(
    atcac_aes_gcm_ctx * ctx,     /**< [in] AES-GCM Context */
    const uint8_t *     key,     /**< [in] AES Key */
    const uint8_t       key_len, /**< [in] Length of the AES key - should be 16 or 32*/
    const uint8_t *     iv,      /**< [in] Initialization vector input */
    const uint8_t       iv_len   /**< [in] Length of the initialization vector */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        memset(ctx, 0, sizeof(atcac_aes_gcm_ctx));
        ctx->iv_len = iv_len;

        if (iv)
        {
            memcpy(ctx->iv, iv, ctx->iv_len);
        }

        status = !wc_AesGcmSetKey(&ctx->aes, key, key_len) ? ATCA_SUCCESS : ATCA_GEN_FAIL;
    }

    return status;
}

ATCA_STATUS atcac_aes_gcm_encrypt(
    atcac_aes_gcm_ctx* ctx,
    const uint8_t*     plaintext,
    const size_t       pt_len,
    uint8_t*           ciphertext,
    uint8_t*           tag,
    size_t             tag_len,
    const uint8_t*     aad,
    const size_t       aad_len
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        if (!wc_AesGcmEncrypt(&ctx->aes, ciphertext, plaintext, pt_len, ctx->iv, ctx->iv_len, tag, tag_len, aad, aad_len))
        {
            status = ATCA_SUCCESS;
        }
    }
    return status;
}

/** \brief Initialize an AES-GCM context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_decrypt_start(
    atcac_aes_gcm_ctx* ctx,     /**< [in] AES-GCM Context */
    const uint8_t*     key,     /**< [in] AES Key */
    const uint8_t      key_len, /**< [in] Length of the AES key - should be 16 or 32*/
    const uint8_t*     iv,      /**< [in] Initialization vector input */
    const uint8_t      iv_len   /**< [in] Length of the initialization vector */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        memset(ctx, 0, sizeof(atcac_aes_gcm_ctx));
        ctx->iv_len = iv_len;

        if (iv)
        {
            memcpy(ctx->iv, iv, ctx->iv_len);
        }

        status = !wc_AesGcmSetKey(&ctx->aes, key, key_len) ? ATCA_SUCCESS : ATCA_GEN_FAIL;
    }

    return status;
}

ATCA_STATUS atcac_aes_gcm_decrypt(
    atcac_aes_gcm_ctx* ctx,
    const uint8_t*     ciphertext,
    const size_t       ct_len,
    uint8_t*           plaintext,
    const uint8_t*     tag,
    size_t             tag_len,
    const uint8_t*     aad,
    const size_t       aad_len,
    bool*              is_verified
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx && is_verified)
    {
        if (!wc_AesGcmDecrypt(&ctx->aes, plaintext, ciphertext, ct_len, ctx->iv, ctx->iv_len, tag, tag_len, aad, aad_len))
        {
            *is_verified = true;
            status = ATCA_SUCCESS;
        }
        else
        {
            *is_verified = false;
        }
    }
    return status;
}

/** \brief Initialize context for performing SHA1 hash in software.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha1_init(
    atcac_sha1_ctx* ctx         /**< [in] pointer to a hash context */
    )
{
    return (!wc_InitSha(ctx)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Add data to a SHA1 hash.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha1_update(
    atcac_sha1_ctx* ctx,        /**< [in] pointer to a hash context */
    const uint8_t*  data,       /**< [in] input data buffer */
    size_t          data_size   /**< [in] input data length */
    )
{
    return (!wc_ShaUpdate(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Complete the SHA1 hash in software and return the digest.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha1_finish(
    atcac_sha1_ctx* ctx,                          /**< [in] pointer to a hash context */
    uint8_t         digest[ATCA_SHA1_DIGEST_SIZE] /**< [out] output buffer (20 bytes) */
    )
{
    return (!wc_ShaFinal(ctx, digest)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Initialize context for performing SHA256 hash in software.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha2_256_init(
    atcac_sha2_256_ctx* ctx                 /**< [in] pointer to a hash context */
    )
{
    return (!wc_InitSha256(ctx)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Add data to a SHA256 hash.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha2_256_update(
    atcac_sha2_256_ctx* ctx,                /**< [in] pointer to a hash context */
    const uint8_t*      data,               /**< [in] input data buffer */
    size_t              data_size           /**< [in] input data length */
    )
{
    return (!wc_Sha256Update(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Complete the SHA256 hash in software and return the digest.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha2_256_finish(
    atcac_sha2_256_ctx* ctx,                              /**< [in] pointer to a hash context */
    uint8_t             digest[ATCA_SHA2_256_DIGEST_SIZE] /**< [out] output buffer (32 bytes) */
    )
{
    return (!wc_Sha256Final(ctx, digest)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Initialize context for performing CMAC in software.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_cmac_init(
    atcac_aes_cmac_ctx* ctx,                    /**< [in] pointer to a aes-cmac context */
    const uint8_t*      key,                    /**< [in] key value to use */
    const uint8_t       key_len                 /**< [in] length of the key */
    )
{
    return (!wc_InitCmac(ctx, key, key_len, WC_CMAC_AES, NULL)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Update CMAC context with input data
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_cmac_update(
    atcac_aes_cmac_ctx* ctx,                /**< [in] pointer to a aes-cmac context */
    const uint8_t*      data,               /**< [in] input data */
    const size_t        data_size           /**< [in] length of input data */
    )
{
    return (!wc_CmacUpdate(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Finish CMAC calculation and clear the CMAC context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_cmac_finish(
    atcac_aes_cmac_ctx* ctx,            /**< [in] pointer to a aes-cmac context */
    uint8_t*            cmac,           /**< [out] cmac value */
    size_t*             cmac_size       /**< [inout] length of cmac */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (cmac_size)
    {
        word32 out_len = *cmac_size;
        status = (!wc_CmacFinal(ctx, cmac, &out_len)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
        *cmac_size = out_len;
    }
    return status;
}

/** \brief Initialize context for performing HMAC (sha256) in software.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_sha256_hmac_init(
    atcac_hmac_sha256_ctx* ctx,                 /**< [in] pointer to a sha256-hmac context */
    const uint8_t*         key,                 /**< [in] key value to use */
    const uint8_t          key_len              /**< [in] length of the key */
    )
{
    int ret = wc_HmacInit(ctx, NULL, 0);

    if (!ret)
    {
        ret = wc_HmacSetKey(ctx, SHA256, key, key_len);
    }

    return (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Update HMAC context with input data
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_sha256_hmac_update(
    atcac_hmac_sha256_ctx* ctx,                 /**< [in] pointer to a sha256-hmac context */
    const uint8_t*         data,                /**< [in] input data */
    size_t                 data_size            /**< [in] length of input data */
    )
{
    return (!wc_HmacUpdate(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Finish HMAC calculation and clear the HMAC context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_sha256_hmac_finish(
    atcac_hmac_sha256_ctx* ctx,                /**< [in] pointer to a sha256-hmac context */
    uint8_t*               digest,             /**< [out] hmac value */
    size_t*                digest_len          /**< [inout] length of hmac */
    )
{
    int ret = wc_HmacFinal(ctx, digest);

    wc_HmacFree(ctx);

    return (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
}

/** \brief Set up a public/private key structure for use in asymmetric cryptographic functions
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_init(
    atcac_pk_ctx* ctx,                          /**< [in] pointer to a pk context */
    uint8_t*      buf,                          /**< [in] buffer containing a pem encoded key */
    size_t        buflen,                       /**< [in] length of the input buffer */
    uint8_t       key_type,
    bool          pubkey                        /**< [in] buffer is a public key */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        if (!key_type)
        {
            (ecc_key*)ctx->ptr = wc_ecc_key_new(NULL);

            if (ctx->ptr)
            {
                int ret = wc_ecc_set_curve((ecc_key*)ctx->ptr, 32, ECC_SECP256R1);

                if (!ret)
                {
                    if (pubkey)
                    {
                        /* Configure the public key */
                        ret = wc_ecc_import_unsigned((ecc_key*)ctx->ptr, buf, &buf[32], NULL, ECC_SECP256R1);
                    }
                    else
                    {
                        /* Configure a private key */
                        ret = wc_ecc_import_private_key((byte*)buf, 32, NULL, 0, (ecc_key*)ctx->ptr);
                    }

                    if (!ret)
                    {
                        status = ATCA_SUCCESS;
                    }
                    else
                    {
                        wc_ecc_key_free((ecc_key*)(ctx->ptr));
                        status = ATCA_GEN_FAIL;
                    }
                }
            }
        }
    }
    return status;
}

/** \brief Set up a public/private key structure for use in asymmetric cryptographic functions
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_init_pem(
    atcac_pk_ctx* ctx,                          /**< [in] pointer to a pk context */
    uint8_t *     buf,                          /**< [in] buffer containing a pem encoded key */
    size_t        buflen,                       /**< [in] length of the input buffer */
    bool          pubkey                        /**< [in] buffer is a public key */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx && buf)
    {
        int ret = -1;
        int ecckey = 0;
        int type = ECC_PRIVATEKEY_TYPE;
        word32 inOutIdx = 0;
        DerBuffer* der = NULL;
        status = ATCA_FUNC_FAIL;

        if (pubkey)
        {
            type = ECC_PUBLICKEY_TYPE;
        }

        ret = PemToDer((char*)buf, (long)buflen, type, &der, NULL, NULL, &ecckey);

        if ((ret >= 0) && (der != NULL))
        {
            (ecc_key*)ctx->ptr = wc_ecc_key_new(NULL);

            if (ctx->ptr)
            {
                ret = wc_ecc_set_curve((ecc_key*)ctx->ptr, 32, ECC_SECP256R1);

                if (!ret)
                {
                    if (pubkey)
                    {
                        ret = wc_EccPublicKeyDecode(der->buffer, &inOutIdx, (ecc_key*)ctx->ptr, der->length);
                    }
                    else
                    {
                        ret = wc_EccPrivateKeyDecode(der->buffer, &inOutIdx, (ecc_key*)ctx->ptr, der->length);
                    }
                    status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
                }
            }
        }
    }
    return status;
}

/** \brief Get the public key from the context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_public(
    atcac_pk_ctx* ctx,
    uint8_t*      buf,
    size_t*       buflen
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx && ctx->ptr && buf)
    {
        if (buflen)
        {
            (void)*buflen;
        }

        int ret = -1;

        if (!wc_ecc_check_key((ecc_key*)ctx->ptr))
        {
            word32 xlen = 32;
            word32 ylen = 32;

            ret = wc_ecc_export_public_raw((ecc_key*)ctx->ptr, (byte*)buf, &xlen, (byte*)&buf[32], &ylen);
        }

        status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Free a public/private key structure
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_free(
    atcac_pk_ctx* ctx                           /**< [in] pointer to a pk context */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        if (ctx->ptr)
        {
            wc_ecc_key_free((ecc_key*)(ctx->ptr));
        }
        status = ATCA_SUCCESS;
    }
    return status;
}

/** \brief Perform a signature with the private key in the context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_sign(
    atcac_pk_ctx* ctx,
    uint8_t *     digest,
    size_t        dig_len,
    uint8_t*      signature,
    size_t*       sig_len
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    int ret = 0;
    WC_RNG rng;

    if (ctx && ctx->ptr && signature && digest && sig_len)
    {
        int ret = wc_InitRng(&rng);

        if (!ret)
        {
            if (!wc_ecc_check_key((ecc_key*)ctx->ptr))
            {
                uint8_t sig[72];
                word32 siglen = sizeof(sig);
                word32 rlen = 32;
                word32 slen = 32;

                ret = wc_ecc_sign_hash((byte*)digest, (word32)dig_len, (byte*)sig, &siglen, &rng, (ecc_key*)ctx->ptr);
                if (!ret)
                {
                    ret = wc_ecc_sig_to_rs((byte*)sig, siglen, (byte*)signature, &rlen, (byte*)&signature[32], &slen);
                }

                if (!ret)
                {
                    *sig_len = 64;
                }
            }
            else
            {
                // ret = wc_SignatureGenerateHash(WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_RSA, digest, dig_len, signature,
                //                                *sig_len, (RsaKey*)ctx->ptr, 32, &rng);
            }
            wc_FreeRng(&rng);
        }
        status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Perform a verify using the public key in the provided context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_verify(
    atcac_pk_ctx* ctx,
    uint8_t*      digest,
    size_t        dig_len,
    uint8_t*      signature,
    size_t        sig_len
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx && ctx->ptr && signature && digest)
    {
        int ret = -1;
        int res = 0;
        if (!wc_ecc_check_key((ecc_key*)ctx->ptr))
        {
            uint8_t sig[72];
            sig_len = sizeof(sig);

            ret = wc_ecc_rs_raw_to_sig(signature, 32, &signature[32], 32, (byte*)sig, (word32*)&sig_len);

            if (!ret)
            {
                ret = wc_ecc_verify_hash((byte*)sig, (word32)sig_len, (byte*)digest, (word32)dig_len, &res, (ecc_key*)ctx->ptr);
            }
        }
        else
        {
            // ret = wc_SignatureVerifyHash(WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_RSA, digest, 32, signature,
            //                              &sig_len, ctx->ptr, 64);
        }

        status = ATCA_FUNC_FAIL;
        if (!ret)
        {
            if (res)
            {
                status = ATCA_SUCCESS;
            }
        }
    }

    return status;

}

/** \brief Execute the key agreement protocol for the provided keys (if they can)
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_derive(
    atcac_pk_ctx* private_ctx,
    atcac_pk_ctx* public_ctx,
    uint8_t*      buf,
    size_t*       buflen
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if ((private_ctx != NULL) && (public_ctx != NULL) && (buf != NULL) && (buflen != NULL))
    {
        int ret = -1;

        if (!wc_ecc_check_key((ecc_key*)public_ctx->ptr))
        {
            ret = wc_ecc_shared_secret((ecc_key*)private_ctx->ptr, (ecc_key*)public_ctx->ptr, (byte*)buf, (word32*)buflen);
        }
        status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
}

#endif /* ATCA_WOLFSSL */
