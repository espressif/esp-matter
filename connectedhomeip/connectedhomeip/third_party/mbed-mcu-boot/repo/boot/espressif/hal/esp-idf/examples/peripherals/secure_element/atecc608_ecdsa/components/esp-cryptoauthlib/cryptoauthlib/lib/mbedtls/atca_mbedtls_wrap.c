/**
 * \file
 * \brief Wrapper functions to replace cryptoauthlib software crypto functions
 *        with the mbedTLS equivalent
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

/* mbedTLS boilerplate includes */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#include "mbedtls/cmac.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/bignum.h"
#include "mbedtls/x509_crt.h"


/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "atca_mbedtls_wrap.h"

#include "crypto/atca_crypto_sw.h"
#if ATCA_CA_SUPPORT
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_def.h"
#endif

/** \brief Return Random Bytes
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_random(uint8_t* data, size_t data_size)
{
    return mbedtls_ctr_drbg_random(mbedtls_entropy_func, data, data_size);
}


/** \brief Update the GCM context with additional authentication data (AAD)
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_aad_update(
    atcac_aes_gcm_ctx* ctx,     /**< [in] AES-GCM Context */
    const uint8_t*     aad,     /**< [in] Additional Authentication Data */
    const size_t       aad_len  /**< [in] Length of AAD */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = mbedtls_cipher_update_ad(ctx, aad, aad_len);
        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Initialize an AES-GCM context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_encrypt_start(
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
        int ret;
        mbedtls_cipher_init(ctx);

        ret = mbedtls_cipher_setup(ctx, mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_GCM));

        if (!ret)
        {
            ret = mbedtls_cipher_setkey(ctx, key, key_len * 8, MBEDTLS_ENCRYPT);
        }

        if (!ret)
        {
            ret = mbedtls_cipher_set_iv(ctx, iv, iv_len);
        }

        if (!ret)
        {
            ret = mbedtls_cipher_reset(ctx);
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
}

/** \brief Encrypt a data using the initialized context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_encrypt_update(
    atcac_aes_gcm_ctx* ctx,        /**< [in] AES-GCM Context */
    const uint8_t*     plaintext,  /**< [in] Input buffer to encrypt */
    const size_t       pt_len,     /**< [in] Length of the input */
    uint8_t*           ciphertext, /**< [out] Output buffer */
    size_t*            ct_len      /**< [inout] Length of the ciphertext buffer */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = mbedtls_cipher_update(ctx, plaintext, pt_len, ciphertext, ct_len);
        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
}

/** \brief Get the AES-GCM tag and free the context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_encrypt_finish(
    atcac_aes_gcm_ctx* ctx,     /**< [in] AES-GCM Context */
    uint8_t*           tag,     /**< [out] GCM Tag Result */
    size_t             tag_len  /**< [in] Length of the GCM tag */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = mbedtls_cipher_write_tag(ctx, tag, tag_len);

        mbedtls_cipher_free(ctx);

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Initialize an AES-GCM context for decryption
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
        int ret;
        mbedtls_cipher_init(ctx);

        ret = mbedtls_cipher_setup(ctx, mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_ID_AES, key_len * 8, MBEDTLS_MODE_GCM));

        if (!ret)
        {
            ret = mbedtls_cipher_setkey(ctx, key, key_len * 8, MBEDTLS_DECRYPT);
        }

        if (!ret)
        {
            ret = mbedtls_cipher_set_iv(ctx, iv, iv_len);
        }

        if (!ret)
        {
            ret = mbedtls_cipher_reset(ctx);
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
}

/** \brief Decrypt ciphertext using the initialized context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_decrypt_update(
    atcac_aes_gcm_ctx* ctx,        /**< [in] AES-GCM Context */
    const uint8_t*     ciphertext, /**< [in] Ciphertext to decrypt */
    const size_t       ct_len,     /**< [in] Length of the ciphertext */
    uint8_t*           plaintext,  /**< [out] Resulting decrypted plaintext */
    size_t*            pt_len      /**< [inout] Length of the plaintext buffer */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = mbedtls_cipher_update(ctx, ciphertext, ct_len, plaintext, pt_len);
        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
}

/** \brief Compare the AES-GCM tag and free the context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_gcm_decrypt_finish(
    atcac_aes_gcm_ctx* ctx,        /**< [in] AES-GCM Context */
    const uint8_t*     tag,        /**< [in] GCM Tag to Verify */
    size_t             tag_len,    /**< [in] Length of the GCM tag */
    bool*              is_verified /**< [out] Tag verified as matching */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx && is_verified)
    {
        int ret;
        *is_verified = false;

        ret = mbedtls_cipher_check_tag(ctx, tag, tag_len);

        if (!ret)
        {
            *is_verified = true;
        }

        mbedtls_cipher_free(ctx);

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief MBedTLS Message Digest Abstraction - Init
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS _atca_mbedtls_md_init(mbedtls_md_context_t* ctx, const mbedtls_md_info_t* md_info)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret;
        mbedtls_md_init(ctx);

        ret = mbedtls_md_setup(ctx,  md_info, false);

        if (!ret)
        {
            ret = mbedtls_md_starts(ctx);
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief MbedTLS Message Digest Abstraction - Update
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS _atca_mbedtls_md_update(mbedtls_md_context_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        status = (!mbedtls_md_update(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief MbedTLS Message Digest Abstraction - Finish
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS _atca_mbedtls_md_finish(mbedtls_md_context_t* ctx, uint8_t* digest, unsigned int* outlen)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    (void)outlen;

    if (ctx)
    {
        int ret = mbedtls_md_finish(ctx, digest);

        mbedtls_md_free(ctx);

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
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
    return _atca_mbedtls_md_init(ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1));
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
    return _atca_mbedtls_md_update(ctx, data, data_size);
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
    return _atca_mbedtls_md_finish(ctx, digest, NULL);
}

/** \brief Initialize context for performing SHA256 hash in software.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atcac_sw_sha2_256_init(
    atcac_sha2_256_ctx* ctx                 /**< [in] pointer to a hash context */
    )
{
    return _atca_mbedtls_md_init(ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256));
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
    return _atca_mbedtls_md_update(ctx, data, data_size);
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
    return _atca_mbedtls_md_finish(ctx, digest, NULL);
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
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = 0;
        mbedtls_cipher_init(ctx);

        ret = mbedtls_cipher_setup(ctx, mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB));

        if (!ret)
        {
            ret = mbedtls_cipher_cmac_starts(ctx, key, (size_t)key_len * 8);
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }

    return status;
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
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        status = (!mbedtls_cipher_cmac_update(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Finish CMAC calculation and clear the CMAC context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_aes_cmac_finish(
    atcac_aes_cmac_ctx* ctx,           /**< [in] pointer to a aes-cmac context */
    uint8_t*            cmac,          /**< [out] cmac value */
    size_t*             cmac_size      /**< [inout] length of cmac */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    (void)cmac_size;

    if (ctx)
    {
        int ret = mbedtls_cipher_cmac_finish(ctx, cmac);

        mbedtls_cipher_free(ctx);

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
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
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret;
        mbedtls_md_init(ctx);

        ret = mbedtls_md_setup(ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), true);

        if (!ret)
        {
            ret = mbedtls_md_hmac_starts(ctx, key, key_len);
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
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
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        status = (!mbedtls_md_hmac_update(ctx, data, data_size)) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Finish CMAC calculation and clear the HMAC context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_sha256_hmac_finish(
    atcac_hmac_sha256_ctx* ctx,                /**< [in] pointer to a sha256-hmac context */
    uint8_t*               digest,             /**< [out] hmac value */
    size_t*                digest_len          /**< [inout] length of hmac */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    (void)digest_len;

    if (ctx)
    {
        int ret = mbedtls_md_hmac_finish(ctx, digest);

        mbedtls_md_free(ctx);

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
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

    (void)key_type;

    if (ctx)
    {
        int ret;
        uint8_t temp = 1;
        mbedtls_ecp_keypair* ecp = NULL;

        mbedtls_pk_init(ctx);
        ret = mbedtls_pk_setup(ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));

        if (!ret)
        {
            ecp = mbedtls_pk_ec(*ctx);
            ret = mbedtls_ecp_group_load(&ecp->grp, MBEDTLS_ECP_DP_SECP256R1);
        }

        if (pubkey)
        {
            if (!ret)
            {
                ret = mbedtls_mpi_read_binary(&(ecp->Q.X), buf, buflen / 2);
            }

            if (!ret)
            {
                ret = mbedtls_mpi_read_binary(&(ecp->Q.Y), &buf[buflen / 2], buflen / 2);
            }

            if (!ret)
            {
                ret = mbedtls_mpi_read_binary(&(ecp->Q.Z), &temp, 1);
            }
        }
        else
        {
            if (!ret)
            {
                ret = mbedtls_mpi_read_binary(&(ecp->d), buf, buflen);
            }
        }

        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Set up a public/private key structure for use in asymmetric cryptographic functions
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_init_pem(
    atcac_pk_ctx* ctx,                         /**< [in] pointer to a pk context */
    uint8_t*      buf,                         /**< [in] buffer containing a pem encoded key */
    size_t        buflen,                      /**< [in] length of the input buffer */
    bool          pubkey                       /**< [in] buffer is a public key */
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret;
        mbedtls_pk_init(ctx);

        if (pubkey)
        {
            ret = mbedtls_pk_parse_public_key(ctx, buf, buflen);
        }
        else
        {
            ret = mbedtls_pk_parse_key(ctx, buf, buflen, NULL, 0);
        }
        status = (!ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
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
        mbedtls_pk_init(ctx);
        status = ATCA_SUCCESS;
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

    if (ctx)
    {
        int ret = -1;
        switch (mbedtls_pk_get_type(ctx))
        {
        case MBEDTLS_PK_ECKEY:
        /* fallthrough */
        case MBEDTLS_PK_ECDSA:
        {
            (void)mbedtls_mpi_write_binary(&mbedtls_pk_ec(*ctx)->Q.X, buf, 32);
            ret = mbedtls_mpi_write_binary(&mbedtls_pk_ec(*ctx)->Q.Y, &buf[32], 32);
            *buflen = 64;
            break;
        }
        default:
            break;
        }
        status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
    }
    return status;
}

/** \brief Perform a signature with the private key in the context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcac_pk_sign(
    atcac_pk_ctx* ctx,
    uint8_t*      digest,
    size_t        dig_len,
    uint8_t*      signature,
    size_t*       sig_len
    )
{
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (ctx)
    {
        int ret = -1;
        switch (mbedtls_pk_get_type(ctx))
        {
        case MBEDTLS_PK_ECKEY:
        /* fallthrough */
        case MBEDTLS_PK_ECDSA:
        {
            mbedtls_mpi r;
            mbedtls_mpi s;

            mbedtls_mpi_init(&r);
            mbedtls_mpi_init(&s);

            //ret = mbedtls_ecdsa_sign(&mbedtls_pk_ec(*ctx)->grp, &r, &s, &mbedtls_pk_ec(*ctx)->d, digest, dig_len, NULL, NULL);
            ret = mbedtls_ecdsa_sign_det(&mbedtls_pk_ec(*ctx)->grp, &r, &s, &mbedtls_pk_ec(*ctx)->d, digest, dig_len, MBEDTLS_MD_SHA256);

            if (!ret)
            {
                ret = mbedtls_mpi_write_binary(&r, signature, 32);
            }

            if (!ret)
            {
                ret = mbedtls_mpi_write_binary(&s, &signature[32], 32);
            }

            mbedtls_mpi_free(&r);
            mbedtls_mpi_free(&s);

            *sig_len = 64;
            break;
        }
        case MBEDTLS_PK_RSA:
            ret = mbedtls_pk_sign(ctx, MBEDTLS_MD_SHA256, digest, dig_len, signature, sig_len, NULL, NULL);
            break;
        default:
            break;
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

    if (ctx)
    {
        int ret = -1;
        switch (mbedtls_pk_get_type(ctx))
        {
        case MBEDTLS_PK_ECKEY:
        /* fallthrough */
        case MBEDTLS_PK_ECDSA:
        {
            mbedtls_mpi r;
            mbedtls_mpi s;

            mbedtls_mpi_init(&r);
            mbedtls_mpi_init(&s);

            mbedtls_mpi_read_binary(&r, signature, sig_len / 2);
            mbedtls_mpi_read_binary(&s, &signature[sig_len / 2], sig_len / 2);

            ret = mbedtls_ecdsa_verify(&mbedtls_pk_ec(*ctx)->grp, digest, dig_len, &mbedtls_pk_ec(*ctx)->Q, &r, &s);

            mbedtls_mpi_free(&r);
            mbedtls_mpi_free(&s);
            break;
        }
        case MBEDTLS_PK_RSA:
            ret = mbedtls_pk_verify(ctx, MBEDTLS_MD_SHA256, digest, dig_len, signature, sig_len);
            break;
        default:
            break;
        }
        status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
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

    if (private_ctx && public_ctx)
    {
        mbedtls_pk_type_t keytype = mbedtls_pk_get_type(private_ctx);

        if (mbedtls_pk_get_type(public_ctx) == keytype)
        {
            int ret = -1;
            switch (keytype)
            {
            case MBEDTLS_PK_ECKEY:
            /* fallthrough */
            case MBEDTLS_PK_ECDSA:
            {
                mbedtls_mpi result;

                mbedtls_mpi_init(&result);

                ret = mbedtls_ecdh_compute_shared(&mbedtls_pk_ec(*private_ctx)->grp, &result, &mbedtls_pk_ec(*public_ctx)->Q, &mbedtls_pk_ec(*private_ctx)->d, NULL, NULL);

                mbedtls_mpi_write_binary(&result, buf, *buflen);
                mbedtls_mpi_free(&result);
                break;
            }
            default:
                break;
            }
            status = (0 == ret) ? ATCA_SUCCESS : ATCA_FUNC_FAIL;
        }
    }
    return status;
}


#ifndef MBEDTLS_ECDSA_SIGN_ALT
#include "mbedtls/pk_internal.h"
#include "atcacert/atcacert_der.h"

static size_t atca_mbedtls_eckey_get_bitlen(const void * ctx)
{
    return mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->get_bitlen(ctx);
}

static int atca_mbedtls_eckey_can_do(mbedtls_pk_type_t type)
{
    return mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->can_do(type);
}

static int atca_mbedtls_eckey_verify(void *ctx, mbedtls_md_type_t md_alg,
                                     const unsigned char *hash, size_t hash_len,
                                     const unsigned char *sig, size_t sig_len)
{
#ifdef MBEDTLS_ECDSA_VERIFY_ALT
    return mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->verify_func(ctx, md_alg, hash, hash_len, sig, sig_len);
#else
    int ret = -1;
    mbedtls_ecp_keypair *ecp = (mbedtls_ecp_keypair*)ctx;

    (void)md_alg;
    (void)hash_len;

    if (ecp && hash && sig)
    {
        mbedtls_mpi r, s;
        atca_mbedtls_eckey_t key_info;
        uint8_t signature[ATCA_ECCP256_SIG_SIZE];

        /* Signature is in ASN.1 format so we have to parse it out manually */
        size_t len;
        unsigned char *ptr = (unsigned char*)sig;
        const unsigned char *end = sig + sig_len;

        mbedtls_mpi_init(&r);
        mbedtls_mpi_init(&s);

        ret = mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));

        if (!ret)
        {
            ret = mbedtls_asn1_get_tag(&ptr, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);

            if (ptr + len != end)
            {
                /* Some sort of parsing error */
                ret = -1;
            }
        }

        if (!ret)
        {
            ret = mbedtls_asn1_get_mpi(&ptr, end, &r);
        }

        if (!ret)
        {
            ret = mbedtls_asn1_get_mpi(&ptr, end, &s);
        }

        if (!ret)
        {
            ret = mbedtls_mpi_write_binary(&r, signature, 32);
        }

        if (!ret)
        {
            ret = mbedtls_mpi_write_binary(&s, &signature[32], 32);
        }

        if (!ret)
        {
            bool is_verified = false;

            //           if (0x01 & key_info.flags)
            {
                uint8_t public_key[ATCA_ECCP256_PUBKEY_SIZE];
                if (0 == (ret = mbedtls_mpi_write_binary(&ecp->Q.X, public_key, ATCA_ECCP256_PUBKEY_SIZE / 2)))
                {
                    if (0 == (ret = mbedtls_mpi_write_binary(&ecp->Q.Y, &public_key[ATCA_ECCP256_PUBKEY_SIZE / 2], ATCA_ECCP256_PUBKEY_SIZE / 2)))
                    {
                        ret = atcab_verify_extern_ext(key_info.device, hash, signature, public_key, &is_verified);
                    }
                }
            }
//            else
//            {
//                ret = atcab_verify_stored_ext(key_info.device, hash, signature, key_info.handle, &is_verified);
//            }

            if (ATCA_SUCCESS == ret)
            {
                ret = is_verified ? 0 : -1;
            }
        }

        mbedtls_mpi_free(&r);
        mbedtls_mpi_free(&s);
    }

    return ret;
#endif
}

static int atca_mbedtls_eckey_sign(void *ctx, mbedtls_md_type_t md_alg,
                                   const unsigned char *hash, size_t hash_len,
                                   unsigned char *sig, size_t *sig_len,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng)
{
    int ret = -1;
    mbedtls_ecp_keypair *ecp = (mbedtls_ecp_keypair*)ctx;

    ((void)md_alg);
    ((void)f_rng);
    ((void)p_rng);

    if (ecp && hash && sig && sig_len)
    {
        mbedtls_mpi r, s;

        mbedtls_mpi_init(&r);
        mbedtls_mpi_init(&s);

        ret = atca_mbedtls_ecdsa_sign(&ecp->d, &r, &s, hash, hash_len);

        if (!ret)
        {
            ret = mbedtls_ecdsa_signature_to_asn1(&r, &s, sig, sig_len);
        }

        mbedtls_mpi_free(&r);
        mbedtls_mpi_free(&s);
    }
    return ret;
}

static int atca_mbedtls_eckey_check_pair(const void *pub, const void *prv)
{
    return mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->check_pair_func(pub, prv);
}

static void * atca_mbedtls_eckey_alloc(void)
{
    return mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->ctx_alloc_func();
}

static void atca_mbedtls_eckey_free(void * ctx)
{
    mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->ctx_free_func(ctx);
}

static void atca_mbedtls_eckey_debug(const void *ctx, mbedtls_pk_debug_item *items)
{
    mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)->debug_func(ctx, items);
}

const mbedtls_pk_info_t atca_mbedtls_eckey_info = {
    MBEDTLS_PK_ECKEY,
    "EC",
    atca_mbedtls_eckey_get_bitlen,
    atca_mbedtls_eckey_can_do,
    atca_mbedtls_eckey_verify,
    atca_mbedtls_eckey_sign,
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    /* Required to maintain structure alignment */
    NULL,
    NULL,
#endif
    NULL,
    NULL,
    atca_mbedtls_eckey_check_pair,
    atca_mbedtls_eckey_alloc,
    atca_mbedtls_eckey_free,
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    /* Required to maintain structure alignment */
    NULL,
    NULL,
#endif
    atca_mbedtls_eckey_debug,
};

#endif


/** \brief Initializes an mbedtls pk context for use with EC operations
 * \param[in,out] pkey ptr to space to receive version string
 * \param[in] slotid Associated with this key
 * \return 0 on success, otherwise an error code.
 */
int atca_mbedtls_pk_init_ext(ATCADevice device, mbedtls_pk_context * pkey, const uint16_t slotid)
{
    int ret = 0;
    uint8_t public_key[ATCA_ECCP256_SIG_SIZE];
    mbedtls_ecp_keypair * ecp = NULL;
    uint8_t temp = 1;
    bool is_private = false;

    if (!pkey)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (!ret)
    {
        mbedtls_pk_init(pkey);
#ifdef MBEDTLS_ECDSA_SIGN_ALT
        ret = mbedtls_pk_setup(pkey, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
#else
        ret = mbedtls_pk_setup(pkey, &atca_mbedtls_eckey_info);
#endif
    }


    if (!ret)
    {
        ecp = mbedtls_pk_ec(*pkey);
        ret = mbedtls_ecp_group_load(&ecp->grp, MBEDTLS_ECP_DP_SECP256R1);
    }

    if (!ret)
    {
        ret = atcab_is_private_ext(device, slotid, &is_private);
    }

    if (!ret)
    {
        if (is_private)
        {
            ret = atcab_get_pubkey_ext(device, slotid, public_key);
        }
        else
        {
            ret = atcab_read_pubkey_ext(device, slotid, public_key);
        }
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(ecp->Q.X), public_key, ATCA_ECCP256_SIG_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(ecp->Q.Y), &public_key[ATCA_ECCP256_SIG_SIZE / 2], ATCA_ECCP256_SIG_SIZE / 2);
    }

    if (!ret)
    {
        atca_mbedtls_eckey_t key_info = { device, slotid };

        /* This is a bit of a hack to force a context into the mbedtls keypair structure but it should
           work on any platform as it is in essence directly copying memory exactly as it appears in the
           structure */

#ifndef MBEDTLS_ECDSA_VERIFY_ALT
        if (0 == (ret = mbedtls_mpi_read_binary(&(ecp->Q.Z), &temp, 1)))
        {
            ret = mbedtls_mpi_read_binary(&ecp->d, (const unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));
        }
#else
        if (is_private)
        {
            if (0 == (ret = mbedtls_mpi_read_binary(&(ecp->Q.Z), &temp, 1)))
            {
                ret = mbedtls_mpi_read_binary(&ecp->d, (const unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));
            }
        }
        else
        {
            ret = mbedtls_mpi_read_binary(&ecp->Q.Z, (const unsigned char*)&key_info, sizeof(atca_mbedtls_eckey_t));
        }
#endif
    }

    return ret;
}

/** \brief Initializes an mbedtls pk context for use with EC operations
 * \param[in,out] pkey ptr to space to receive version string
 * \param[in] slotid Associated with this key
 * \return 0 on success, otherwise an error code.
 */
int atca_mbedtls_pk_init(mbedtls_pk_context * pkey, const uint16_t slotid)
{
    return atca_mbedtls_pk_init_ext(atcab_get_device(), pkey, slotid);
}

#if ATCA_CA_SUPPORT
/** \brief Rebuild a certificate from an atcacert_def_t structure, and then add
 * it to an mbedtls cert chain.
 * \param[in,out] cert mbedtls cert chain. Must have already been initialized
 * \param[in] cert_def Certificate definition that will be rebuilt and added
 * \return 0 on success, otherwise an error code.
 */
int atca_mbedtls_cert_add(mbedtls_x509_crt * cert, const atcacert_def_t * cert_def)
{
    uint8_t ca_key[64];
    int ret = ATCA_SUCCESS;
    size_t cert_len;
    uint8_t * cert_buf = NULL;

    if (cert_def->ca_cert_def)
    {
        const atcacert_device_loc_t * ca_key_cfg = &cert_def->ca_cert_def->public_key_dev_loc;

        if (ca_key_cfg->is_genkey)
        {
            ret = atcab_get_pubkey(ca_key_cfg->slot, ca_key);
        }
        else
        {
            ret = atcab_read_pubkey(ca_key_cfg->slot, ca_key);
        }
    }

    cert_len = cert_def->cert_template_size + 8;
    if (NULL == (cert_buf = mbedtls_calloc(1, cert_len)))
    {
        ret = -1;
    }

    if (0 == ret)
    {
        ret = atcacert_read_cert(cert_def, cert_def->ca_cert_def ? ca_key : NULL, cert_buf, &cert_len);
    }

    if (0 == ret)
    {
        ret = mbedtls_x509_crt_parse(cert, (const unsigned char*)cert_buf, cert_len);
    }

    if (cert_buf)
    {
        mbedtls_free(cert_buf);
    }

    return ret;
}
#endif
