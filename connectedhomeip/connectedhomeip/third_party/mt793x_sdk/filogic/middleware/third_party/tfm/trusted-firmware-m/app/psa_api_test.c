/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_api_test.h"
#include "tfm_nspm_api.h"
#include "tfm_integ_test.h"

/**
 * \brief This symbol is the entry point provided by the PSA API compliance
 *        test libraries
 */
extern void val_entry(void);

__attribute__((noreturn))
void psa_api_test(void *arg)
{
    UNUSED_VARIABLE(arg);

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    val_entry();

    for (;;) {
    }
}
