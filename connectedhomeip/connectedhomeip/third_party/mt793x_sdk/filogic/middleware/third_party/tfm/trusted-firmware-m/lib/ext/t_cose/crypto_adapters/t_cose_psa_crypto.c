/*
 * t_cose_psa_crypto.c
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


/**
 * \file t_cose_psa_crypto.c
 *
 * \brief Crypto Adaptation for t_cose to use ARM's PSA ECDSA and hashes.
 *
 * This connects up the abstract interface in t_cose_crypto.h to the
 * implementations of ECDSA signing and hashing in ARM's PSA crypto
 * library.
 *
 * This adapter layer doesn't bloat the implementation as everything
 * here had to be done anyway -- the mapping of algorithm IDs, the
 * data format rearranging, the error code translation.
 *
 * This code should just work out of the box if compiled and linked
 * against ARM's PSA crypto. No preprocessor #defines are needed.
 *
 * You can disable SHA-384 and SHA-512 to save code and space by
 * defining T_COSE_DISABLE_ES384 or T_COSE_DISABLE_ES512. This saving
 * is most in stack space in the main t_cose implementation. (It seems
 * likely that changes to PSA itself would be needed to remove the
 * SHA-384 and SHA-512 implementations to save that code. Lack of
 * reference and dead stripping the executable won't do it).
 */


#include "t_cose_crypto.h"  /* The interface this implements */
#include "psa/crypto.h"     /* PSA Crypto Interface to mbed crypto or such */


/* Here's the auto-detect and manual override logic for managing PSA
 * Crypto API compatibility.
 *
 * PSA_GENERATOR_UNBRIDLED_CAPACITY happens to be defined in MBed
 * Crypto 1.1 and not in MBed Crypto 2.0 so it is what auto-detect
 * hinges off of.
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20 can be defined to force
 * setting to MBed Crypto 2.0
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 can be defined to force
 * setting to MBed Crypt 1.1. It is also what the code below hinges
 * on.
 */
#if defined(PSA_GENERATOR_UNBRIDLED_CAPACITY) && !defined(T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20)
#define T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
#endif


/* Avoid compiler warning due to unused argument */
#define ARG_UNUSED(arg) (void)(arg)


/**
 * \brief Map a COSE signing algorithm ID to a PSA signing algorithm ID
 *
 * \param[in] cose_alg_id  The COSE algorithm ID.
 *
 * \return The PSA algorithm ID or 0 if this doesn't map the COSE ID.
 */
static psa_algorithm_t cose_alg_id_to_psa_alg_id(int32_t cose_alg_id)
{
    /* The #ifdefs save a little code when algorithms are disabled */

    return cose_alg_id == COSE_ALGORITHM_ES256 ? PSA_ALG_ECDSA(PSA_ALG_SHA_256) :
#ifndef T_COSE_DISABLE_ES384
           cose_alg_id == COSE_ALGORITHM_ES384 ? PSA_ALG_ECDSA(PSA_ALG_SHA_384) :
#endif
#ifndef T_COSE_DISABLE_ES512
           cose_alg_id == COSE_ALGORITHM_ES512 ? PSA_ALG_ECDSA(PSA_ALG_SHA_512) :
#endif
                                                 0;
    /* psa/crypto_values.h doesn't seem to define a "no alg" value,
     * but zero seems OK for that use in the ECDSA context. */
}


/**
 * \brief Map a PSA error into a t_cose error for signing.
 *
 * \param[in] err   The PSA status.
 *
 * \return The \ref t_cose_err_t.
 */
static enum t_cose_err_t psa_status_to_t_cose_error_signing(psa_status_t err)
{
    /* Intentionally keeping to fewer mapped errors to save object code */
    return err == PSA_SUCCESS                   ? T_COSE_SUCCESS :
           err == PSA_ERROR_INVALID_SIGNATURE   ? T_COSE_ERR_SIG_VERIFY :
           err == PSA_ERROR_NOT_SUPPORTED       ? T_COSE_ERR_UNSUPPORTED_SIGNING_ALG:
           err == PSA_ERROR_INSUFFICIENT_MEMORY ? T_COSE_ERR_INSUFFICIENT_MEMORY :
           err == PSA_ERROR_TAMPERING_DETECTED  ? T_COSE_ERR_TAMPERING_DETECTED :
                                                  T_COSE_ERR_SIG_FAIL;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_pub_key_verify(int32_t               cose_algorithm_id,
                             struct t_cose_key     verification_key,
                             struct q_useful_buf_c kid,
                             struct q_useful_buf_c hash_to_verify,
                             struct q_useful_buf_c signature)
{
    psa_algorithm_t   psa_alg_id;
    psa_status_t      psa_result;
    enum t_cose_err_t return_value;
    psa_key_handle_t  verification_key_psa;

    /* This implementation does no look up keys by kid in the key
     * store */
    ARG_UNUSED(kid);

    /* Convert to PSA algorithm ID scheme */
    psa_alg_id = cose_alg_id_to_psa_alg_id(cose_algorithm_id);

    /* This implementation supports ECDSA and only ECDSA. The
     * interface allows it to support other, but none are implemented.
     * This implementation works for different keys lengths and
     * curves. That is the curve and key length as associated with the
     * signing_key passed in, not the cose_algorithm_id This check
     * looks for ECDSA signing as indicated by COSE and rejects what
     * is not. (Perhaps this check can be removed to save object code
     * if it is the case that psa_asymmetric_verify() does the right
     * checks).
     */
    if(!PSA_ALG_IS_ECDSA(psa_alg_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    verification_key_psa = (psa_key_handle_t)verification_key.k.key_handle;

    psa_result = psa_verify_hash(verification_key_psa,
                                 psa_alg_id,
                                 hash_to_verify.ptr,
                                 hash_to_verify.len,
                                 signature.ptr,
                                 signature.len);

    return_value = psa_status_to_t_cose_error_signing(psa_result);

  Done:
    return return_value;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_pub_key_sign(int32_t                cose_algorithm_id,
                           struct t_cose_key      signing_key,
                           struct q_useful_buf_c  hash_to_sign,
                           struct q_useful_buf    signature_buffer,
                           struct q_useful_buf_c *signature)
{
    enum t_cose_err_t return_value;
    psa_status_t      psa_result;
    psa_algorithm_t   psa_alg_id;
    psa_key_handle_t  signing_key_psa;
    size_t            signature_len;

    psa_alg_id = cose_alg_id_to_psa_alg_id(cose_algorithm_id);

    /* This implementation supports ECDSA and only ECDSA. The
     * interface allows it to support other, but none are implemented.
     * This implementation works for different keys lengths and
     * curves. That is the curve and key length as associated with the
     * signing_key passed in, not the cose_algorithm_id This check
     * looks for ECDSA signing as indicated by COSE and rejects what
     * is not. (Perhaps this check can be removed to save object code
     * if it is the case that psa_asymmetric_verify() does the right
     * checks).
     */
    if(!PSA_ALG_IS_ECDSA(psa_alg_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    signing_key_psa = (psa_key_handle_t)signing_key.k.key_handle;

    /* It is assumed that this call is checking the signature_buffer
     * length and won't write off the end of it.
     */
    psa_result = psa_sign_hash(signing_key_psa,
                               psa_alg_id,
                               hash_to_sign.ptr,
                               hash_to_sign.len,
                               signature_buffer.ptr, /* Sig buf */
                               signature_buffer.len, /* Sig buf size */
                               &signature_len);      /* Sig length */

    return_value = psa_status_to_t_cose_error_signing(psa_result);

    if(return_value == T_COSE_SUCCESS) {
        /* Success, fill in the return useful_buf */
        signature->ptr = signature_buffer.ptr;
        signature->len = signature_len;
    }

  Done:
     return return_value;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t t_cose_crypto_sig_size(int32_t           cose_algorithm_id,
                                         struct t_cose_key signing_key,
                                         size_t           *sig_size)
{
    enum t_cose_err_t return_value;
    psa_key_handle_t  signing_key_psa;
    size_t            key_len_bits;
    size_t            key_len_bytes;

    /* If desperate to save code, this can return the constant
     * T_COSE_MAX_SIG_SIZE instead of doing an exact calculation.  The
     * buffer size calculation will return too large of a value and
     * waste a little heap / stack, but everything will still work
     * (except the tests that test for exact values will fail). This
     * will save 100 bytes or so of obejct code.
     */

    if(!t_cose_algorithm_is_ecdsa(cose_algorithm_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    signing_key_psa = (psa_key_handle_t)signing_key.k.key_handle;

#ifdef T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
    /* This code is for MBed Crypto 1.1. It uses an older version of
     * the PSA Crypto API that is not compatible with the new
     * versions. When all environments (particularly TF-M) are on the
     * latest API, this code will no longer be necessary.
     */

    psa_key_type_t    key_type;

    psa_status_t status = psa_get_key_information(signing_key_psa,
                                                  &key_type,
                                                  &key_len_bits);

    (void)key_type; /* Avoid unused parameter error */

#else /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */
    /* This code is for Mbed Crypto 2.0 circa 2019. The PSA Crypto API
     * is supposed to be offically locked down in 2020 and should be
     * very close to this, so this is likely the code to use with MBed
     * Crypto going forward.
     */

    psa_key_attributes_t key_attributes = psa_key_attributes_init();

    psa_status_t status = psa_get_key_attributes(signing_key_psa, &key_attributes);

    key_len_bits = psa_get_key_bits(&key_attributes);

#endif /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */

    return_value = psa_status_to_t_cose_error_signing(status);
    if(return_value == T_COSE_SUCCESS) {
        /* Calculation of size per RFC 8152 section 8.1 -- round up to
         * number of bytes. */
        key_len_bytes = key_len_bits / 8;
        if(key_len_bits % 8) {
            key_len_bytes++;
        }
        /* Double because signature is made of up r and s values */
        *sig_size = key_len_bytes * 2;
    }

    return_value = T_COSE_SUCCESS;
Done:
    return return_value;
}




/**
 * \brief Convert COSE hash algorithm ID to a PSA hash algorithm ID
 *
 * \param[in] cose_hash_alg_id   The COSE-based ID for the
 *
 * \return PSA-based hash algorithm ID, or USHRT_MAX on error.
 *
 */
static inline psa_algorithm_t
cose_hash_alg_id_to_psa(int32_t cose_hash_alg_id)
{
    return cose_hash_alg_id == COSE_ALGORITHM_SHA_256 ? PSA_ALG_SHA_256 :
#ifndef T_COSE_DISABLE_ES384
           cose_hash_alg_id == COSE_ALGORITHM_SHA_384 ? PSA_ALG_SHA_384 :
#endif
#ifndef T_COSE_DISABLE_ES512
           cose_hash_alg_id == COSE_ALGORITHM_SHA_512 ? PSA_ALG_SHA_512 :
#endif
                                                        UINT16_MAX;
}


/**
 * \brief Map a PSA error into a t_cose error for hashes.
 *
 * \param[in] status   The PSA status.
 *
 * \return The \ref t_cose_err_t.
 */
static enum t_cose_err_t
psa_status_to_t_cose_error_hash(psa_status_t status)
{
    /* Intentionally limited to just this minimum set of errors to
     * save object code as hashes don't really fail much
     */
    return status == PSA_SUCCESS                ? T_COSE_SUCCESS :
           status == PSA_ERROR_NOT_SUPPORTED    ? T_COSE_ERR_UNSUPPORTED_HASH :
           status == PSA_ERROR_BUFFER_TOO_SMALL ? T_COSE_ERR_HASH_BUFFER_SIZE :
                                                  T_COSE_ERR_HASH_GENERAL_FAIL;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t t_cose_crypto_hash_start(struct t_cose_crypto_hash *hash_ctx,
                                           int32_t cose_hash_alg_id)
{
    psa_algorithm_t      psa_alg;

    /* Map the algorithm ID */
    psa_alg = cose_hash_alg_id_to_psa(cose_hash_alg_id);

    /* initialize PSA hash context */
    hash_ctx->ctx = psa_hash_operation_init();

    /* Actually do the hash set up */
    hash_ctx->status = psa_hash_setup(&(hash_ctx->ctx), psa_alg);

    /* Map errors and return */
    return psa_status_to_t_cose_error_hash((psa_status_t)hash_ctx->status);
}


/*
 * See documentation in t_cose_crypto.h
 */
void t_cose_crypto_hash_update(struct t_cose_crypto_hash *hash_ctx,
                               struct q_useful_buf_c      data_to_hash)
{
    if(hash_ctx->status != PSA_SUCCESS) {
        /* In error state. Nothing to do. */
        return;
    }

    if(data_to_hash.ptr == NULL) {
        /* This allows for NULL buffers to be passed in all the way at
         * the top of signer or message creator when all that is
         * happening is the size of the result is being computed.
         */
        return;
    }

    /* Actually hash the data */
    hash_ctx->status = psa_hash_update(&(hash_ctx->ctx),
                                       data_to_hash.ptr,
                                       data_to_hash.len);
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hash_finish(struct t_cose_crypto_hash *hash_ctx,
                          struct q_useful_buf        buffer_to_hold_result,
                          struct q_useful_buf_c     *hash_result)
{
    if(hash_ctx->status != PSA_SUCCESS) {
        /* Error state. Nothing to do */
        goto Done;
    }

    /* Actually finish up the hash */
    hash_ctx->status = psa_hash_finish(&(hash_ctx->ctx),
                                         buffer_to_hold_result.ptr,
                                         buffer_to_hold_result.len,
                                       &(hash_result->len));

    hash_result->ptr = buffer_to_hold_result.ptr;

Done:
    return psa_status_to_t_cose_error_hash(hash_ctx->status);
}
