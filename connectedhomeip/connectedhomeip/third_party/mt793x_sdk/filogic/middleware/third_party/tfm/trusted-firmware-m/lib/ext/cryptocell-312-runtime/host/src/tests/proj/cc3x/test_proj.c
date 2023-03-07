/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "test_proj_defs.h"
#include "test_proj.h"
#include "dx_env.h"

uint32_t Test_ProjInit(void)
{
    uint32_t error = 0;

    error = Test_ProjMap();
    if (error != 0) {
        return error;
    }
    TEST_WRITE_TEE_ENV_REG(DX_ENV_APB_PPROT_OVERRIDE_REG_OFFSET, 0x9);

    return error;

}

void Test_ProjFree(void)
{
    Test_ProjUnmap();
    return;
}
