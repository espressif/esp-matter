/*
 * Copyright (c) 2017-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/include/tfm_plat_crypto_keys.h"
#include "platform/include/tfm_attest_hal.h"
#include <stddef.h>
#include "psa/crypto_types.h"
#include "crypto_hw.h"
#include "mbedtls_cc_mng_int.h"
#include "mbedtls_cc_util_key_derivation.h"
#include "cc_otp_defs.h"

/* FIXME: Functions in this file should be implemented by platform vendor. For
 * the security of the storage system, it is critical to use a hardware unique
 * key. For the security of the attestation, it is critical to use a unique key
 * pair and keep the private key is secret.
 */

#define TFM_KEY_LEN_BYTES  16

#define CC312_NULL_CONTEXT "NO SALT!"
#define CC_BITS_IN_32BIT_WORD 32

extern const psa_ecc_curve_t initial_attestation_curve_type;
extern const uint8_t  initial_attestation_private_key[];
extern const uint32_t initial_attestation_private_key_size;

extern const struct tfm_plat_rotpk_t device_rotpk[];
extern const uint32_t rotpk_key_cnt;

enum tfm_plat_err_t tfm_plat_get_huk_derived_key(const uint8_t *label,
                                                 size_t label_size,
                                                 const uint8_t *context,
                                                 size_t context_size,
                                                 uint8_t *key,
                                                 size_t key_size)
{
    (void)label;
    (void)label_size;
    (void)context;
    (void)context_size;

    int rc;
    uint32_t lcs;
    struct mbedtls_util_keydata mram_key = { 0 };
    uint32_t huk[CC_OTP_HUK_SIZE_IN_WORDS] = { 0 };
    int i = 0;

    rc = crypto_hw_accelerator_get_lcs(&lcs);
    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (lcs != CC_MNG_LCS_SEC_ENABLED) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    for (i = 0; i < CC_OTP_HUK_SIZE_IN_WORDS; i++) {
        CC_READ_MRAM_WORD((CC_OTP_HUK_OFFSET + i) * sizeof(uint32_t), huk[i]);
    }

    mram_key.pKey = (uint8_t*)huk;

    mram_key.keySize = CC_OTP_HUK_SIZE_IN_WORDS * sizeof(uint32_t);

    if (context == NULL || context_size == 0) {
        /* The CC312 requires the context to not be null, so a default
         * is given.
         */
        context = (const uint8_t *)CC312_NULL_CONTEXT;
        context_size = sizeof(CC312_NULL_CONTEXT);
    }

    rc = mbedtls_util_key_derivation_cmac(CC_UTIL_USER_KEY, &mram_key,
                                          label, label_size,
                                          context, context_size,
                                          key, key_size);

    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

/*
 * Count number of zero bits in 32-bit word.
 * Copied from:
 * lib/ext/cryptocell-312-runtime/host/src/ \
 * cc3x_productionlib/common/prod_util.c: CC_PROD_GetZeroCount(..)
 */
static int get_zero_bits_count(uint32_t *buf,
                               uint32_t  buf_word_size,
                               uint32_t *zero_count)
{
    uint32_t val;
    uint32_t index = 0;

    *zero_count = 0;
    for (index = 0; index < buf_word_size; index++) {
        val = buf[index];
        val = val - ((val >> 1) & 0x55555555);
        val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
        val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);
        *zero_count += (32 - val);
    }
    /* All 0's and all 1's is forbidden */
    if ((*zero_count == 0)
        || (*zero_count == buf_word_size*CC_BITS_IN_32BIT_WORD)) {
        *zero_count = 0;
        return -1;
    }

    return 0;
}

enum tfm_plat_err_t
tfm_plat_get_initial_attest_key(uint8_t          *key_buf,
                                uint32_t          size,
                                struct ecc_key_t *ecc_key,
                                psa_ecc_curve_t  *curve_type)
{
    uint32_t key_size = initial_attestation_private_key_size;
    int rc;
    uint32_t *key = (uint32_t *)key_buf;
    uint32_t zero_count;
    uint32_t otp_zero_count;
    int i;

    if (size < key_size) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Set the EC curve type which the key belongs to */
    *curve_type = initial_attestation_curve_type;

    /* Copy the private key to the buffer, it MUST be present */
    if (key == NULL ||
        size < CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS * sizeof(uint32_t)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    size = CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS * sizeof(uint32_t);

    for (i = 0; i < CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS; i++) {
        CC_READ_MRAM_WORD((CC_OTP_ATTESTATION_KEY_OFFSET + i) *
                          sizeof(uint32_t), key[i]);
    }

    /* Verify the zero number of private key */
    rc = get_zero_bits_count((uint32_t *)key_buf,
                             CC_OTP_ATTESTATION_KEY_SIZE_IN_WORDS,
                             &zero_count);
    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    CC_READ_MRAM_WORD(CC_OTP_ATTESTATION_KEY_ZERO_COUNT_OFFSET *
                      sizeof(uint32_t), otp_zero_count);

    if (otp_zero_count != zero_count) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    key_size = size;

    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    ecc_key->priv_key = key_buf;
    ecc_key->priv_key_size = key_size;

    ecc_key->pubx_key = NULL;
    ecc_key->pubx_key_size = 0;
    ecc_key->puby_key = NULL;
    ecc_key->puby_key_size = 0;

    return TFM_PLAT_ERR_SUCCESS;
}

#ifdef BL2
enum tfm_plat_err_t
tfm_plat_get_rotpk_hash(uint8_t image_id,
                        uint8_t *rotpk_hash,
                        uint32_t *rotpk_hash_size)
{
    int rc = 0;

    rc = crypto_hw_accelerator_get_rotpk_hash(image_id, rotpk_hash,
                                              rotpk_hash_size);

    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* BL2 */
