/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "target_cfg.h"
#include "platform/include/tfm_spm_hal.h"
#include "uart_stdout.h"

/* platform-specific hw initialization */
__WEAK enum tfm_plat_err_t tfm_spm_hal_post_init_platform(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_post_init(void)
{
    if (tfm_spm_hal_post_init_platform() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    __enable_irq();
    stdio_init();

    return TFM_PLAT_ERR_SUCCESS;
}

__WEAK void tfm_spm_hal_system_reset(void)
{
    NVIC_SystemReset();
}
