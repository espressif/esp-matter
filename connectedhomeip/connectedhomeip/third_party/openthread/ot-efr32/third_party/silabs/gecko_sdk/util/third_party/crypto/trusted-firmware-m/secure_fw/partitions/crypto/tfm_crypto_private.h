/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_PRIVATE_H__
#define __TFM_CRYPTO_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "sli_tz_iovec_check.h"
#endif

#ifdef TFM_PSA_API
/*
 * Validate the IOVEC[] lengths for IPC model. The tfm_crypto_call_sfn()
 * reduces the entries in IOVEC[] which are empty from `in_len` and `out_len`.
 * This means that Crypto service APIs need to ensure that the `in_len`
 * and `out_len` are within the expected range.
 *
 * Also tfm_crypto_call_sfn() ensures that all entries in IOVEC[] are
 * initialised. Hence all entries in IOVEC[] can be accessed to
 * initialize internal variables even if they are outside `in_len`
 * and `out_len`.
 */
#define CRYPTO_IN_OUT_LEN_VALIDATE(in_len, in_min, in_max, out_len, out_min, out_max)   \
            if (!(((in_len) >= (in_min)) && ((in_len) <= (in_max))) ||      \
                !(((out_len) >= (out_min)) && ((out_len) <= (out_max)))) {  \
                    return PSA_ERROR_PROGRAMMER_ERROR;                      \
            }
#else
/*
 * Validate the IOVEC[] lengths for Library model.  Unlike the IPC model, the
 * service APIs expects to receive the exact of `in_len` and `out_len`
 * as expected by the API.
 */
#define CRYPTO_IN_OUT_LEN_VALIDATE(in_len, in_min, in_max, out_len, out_min, out_max)    \
            if (((in_len) != (in_max)) || ((out_len) != (out_max))) {     \
                return PSA_ERROR_PROGRAMMER_ERROR;                        \
            }
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_PRIVATE_H__ */
