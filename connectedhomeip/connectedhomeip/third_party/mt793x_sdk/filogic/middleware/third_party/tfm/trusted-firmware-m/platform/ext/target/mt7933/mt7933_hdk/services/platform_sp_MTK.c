/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_sp_MTK.h"

#include "psa_manifest/pid.h"
#include "hal_aes.h"
#include "hal_trng.h"

enum tfm_platform_err_t
tfm_hal_aes_ecb_encrypt(psa_invec  *in_vec,  uint32_t num_invec,
                        psa_outvec *out_vec, uint32_t num_outvec)
{
    hal_aes_buffer_t *enc_text, *plain_text, *key;

    if ((num_invec != 2) || (in_vec[0].base == NULL) || (in_vec[1].base == NULL)
     || (num_outvec != 1) || (out_vec[0].base == NULL))
    {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    plain_text = *(hal_aes_buffer_t **)(in_vec[0].base);
    key = *(hal_aes_buffer_t **)(in_vec[1].base);
    enc_text = *(hal_aes_buffer_t **)(out_vec[0].base);

    return hal_aes_ecb_encrypt(enc_text, plain_text, key);
}

enum tfm_platform_err_t platform_sp_MTK_init(void)
{
#ifdef TFM_PSA_API
    #error "TFM_PSA_API not supported yet in mt7933."
#else
    return TFM_PLATFORM_ERR_SUCCESS;
#endif /* TFM_PSA_API */
}

enum tfm_platform_err_t
tfm_hal_trng_random_generate(psa_invec  *in_vec,  uint32_t num_invec,
                            psa_outvec *out_vec, uint32_t num_outvec)
{
    uint32_t *random;
    uint32_t len;
    hal_trng_status_t ret = HAL_TRNG_STATUS_ERROR;

    if ((num_invec != 0) || (in_vec[0].base != NULL)
     || (num_outvec != 1) || (out_vec[0].base == NULL))
    {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
    }

    random = *(uint32_t **)(out_vec[0].base);
    len = out_vec[0].len;

    if( len < sizeof(uint32_t) ) {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
    }

    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    ret = hal_trng_get_generated_random_number(random);
    hal_trng_deinit();
    if (HAL_TRNG_STATUS_OK != ret) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    return TFM_PLATFORM_ERR_SUCCESS;
}
