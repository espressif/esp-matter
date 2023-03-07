/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 * Copyright (c) 2021, Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_private.h"


/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_generate_random(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
#ifdef TFM_CRYPTO_RNG_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;

    return psa_generate_random(output, output_size);
#endif /* TFM_CRYPTO_RNG_MODULE_DISABLED */
}


/*!@}*/
