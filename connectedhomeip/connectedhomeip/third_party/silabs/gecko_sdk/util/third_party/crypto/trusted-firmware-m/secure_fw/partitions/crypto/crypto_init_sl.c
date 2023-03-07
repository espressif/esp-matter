/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/crypto.h"
#include "tfm_crypto_defs.h"

#define SL_SCRATCH_ID (0x11223344)

psa_status_t tfm_crypto_get_caller_id(int32_t *id)
{
    *id = SL_SCRATCH_ID;
    return PSA_SUCCESS;
}
