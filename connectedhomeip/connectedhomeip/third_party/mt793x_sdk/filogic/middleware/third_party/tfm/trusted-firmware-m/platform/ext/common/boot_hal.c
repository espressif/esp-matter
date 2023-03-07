/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "target_cfg.h"
#include "cmsis.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/* bootloader platform-specific hw initialization */
__WEAK int32_t boot_platform_init(void)
{
    int32_t result;

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result == ARM_DRIVER_OK) {
        return 0;
    }

    return 1;
}
