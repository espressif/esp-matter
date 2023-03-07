/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

void tfm_mtk_ci_tfm_excp_handl_test(void)
{
    tfm_ns_interface_dispatch(
            (veneer_fn)tfm_tfm_ci_exception_handle_test_veneer,
            (uint32_t)NULL, 0,
            (uint32_t)NULL, 0);
}

int32_t tfm_mtk_ci_plat_iso_test(void)
{
    psa_status_t status;

    status = tfm_ns_interface_dispatch(
            (veneer_fn)tfm_tfm_ci_platform_isolation_test_veneer,
            (uint32_t)NULL, 0,
            (uint32_t)NULL, 0);

    return status;
}
