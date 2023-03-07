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

/* FIXME: Functions in this file should be implemented by platform vendor. For
 * the security of the storage system, it is critical to use a hardware unique
 * key. For the security of the attestation, it is critical to use a unique key
 * pair and keep the private key is secret.
 */

#define TFM_KEY_LEN_BYTES  16

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

    rc = crypto_hw_accelerator_get_lcs(&lcs);
    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (lcs != CC_MNG_LCS_SEC_ENABLED) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    rc = crypto_hw_accelerator_huk_derive_key(label, label_size, context,
                                              context_size, key, key_size);
    if (rc) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t
tfm_plat_get_initial_attest_key(uint8_t          *key_buf,
                                uint32_t          size,
                                struct ecc_key_t *ecc_key,
                                psa_ecc_curve_t  *curve_type)
{
    uint32_t key_size = initial_attestation_private_key_size;
    int rc;

    if (size < key_size) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Set the EC curve type which the key belongs to */
    *curve_type = initial_attestation_curve_type;

    /* Copy the private key to the buffer, it MUST be present */
    rc = crypto_hw_accelerator_get_attestation_private_key(key_buf, &size);
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
