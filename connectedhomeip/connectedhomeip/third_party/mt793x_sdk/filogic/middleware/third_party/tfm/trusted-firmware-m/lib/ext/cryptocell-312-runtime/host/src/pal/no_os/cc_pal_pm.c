/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"


void CC_PalPowerSaveModeInit(void)
{
    return;
}

void CC_PalPowerSaveModeStatus(void)
{
    return;
}

CCError_t CC_PalPowerSaveModeSelect(CCBool isPowerSaveMode)
{
    CC_UNUSED_PARAM(isPowerSaveMode);

    return 0;
}
