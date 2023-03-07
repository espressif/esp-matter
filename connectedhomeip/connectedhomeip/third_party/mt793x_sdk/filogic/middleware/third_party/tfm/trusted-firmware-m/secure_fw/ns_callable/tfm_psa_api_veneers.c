/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include "psa/client.h"
#include "psa/service.h"
#include "secure_utilities.h"
#include "tfm_arch.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "tfm_svcalls.h"

/* Veneer implementation */

/*
 * SVC to core directly before touch stack due to:
 * - Re-entrant detection bases on stack information.
 * - SVC here stores the current xPSR into stack and recover it back while
 *   exception returns, no leakage of secure state information and no
 *   interference between two sides.
 */

__tfm_psa_secure_gateway_attributes__
uint32_t tfm_psa_framework_version_veneer(void)
{
    __ASM volatile("SVC %0           \n"
                   "BXNS LR          \n"
                    : : "I" (TFM_SVC_PSA_FRAMEWORK_VERSION));
}

__tfm_psa_secure_gateway_attributes__
uint32_t tfm_psa_version_veneer(uint32_t sid)
{
    __ASM volatile("SVC %0           \n"
                   "BXNS LR          \n"
                    : : "I" (TFM_SVC_PSA_VERSION));
}

__tfm_psa_secure_gateway_attributes__
psa_handle_t tfm_psa_connect_veneer(uint32_t sid, uint32_t version)
{
    __ASM volatile("SVC %0           \n"
                   "BXNS LR          \n"
                    : : "I" (TFM_SVC_PSA_CONNECT));
}

__tfm_psa_secure_gateway_attributes__
psa_status_t tfm_psa_call_veneer(psa_handle_t handle,
                               const struct tfm_control_parameter_t *ctrl_param,
                               const psa_invec *in_vec,
                               psa_outvec *out_vec)
{
    __ASM volatile("SVC %0           \n"
                   "BXNS LR          \n"
                    : : "I" (TFM_SVC_PSA_CALL));
}

__tfm_psa_secure_gateway_attributes__
void tfm_psa_close_veneer(psa_handle_t handle)
{
    __ASM volatile("SVC %0           \n"
                   "BXNS LR          \n"
                    : : "I" (TFM_SVC_PSA_CLOSE));
}
