/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include "cc_pal_init.h"
#include "cc_pal_dma_plat.h"
#include "cc_pal_log.h"
#include "cc_pal_mutex.h"
#include "cc_pal_pm.h"

/**
 * @brief   PAL layer entry point.
 *          The function initializes customer platform sub components,
 *           such as memory mapping used later by CRYS to get physical contiguous memory.
 *
 *
 * @return Returns a non-zero value in case of failure
 */
int CC_PalInit(void)
{  // IG - need to use palInit of cc_linux for all PALs
    uint32_t rc = CC_OK;

    CC_PalLogInit();

    rc =    CC_PalDmaInit(0, 0);
    if (rc != CC_OK) {
            return rc;
    }

#ifdef CC_IOT
/* Initialize power management module */
    CC_PalPowerSaveModeInit();
#endif

    return rc;
}


/**
 * @brief   PAL layer entry point.
 *          The function initializes customer platform sub components,
 *           such as memory mapping used later by CRYS to get physical contiguous memory.
 *
 *
 * @return None
 */
void CC_PalTerminate(void)
{
    CC_PalDmaTerminate();
}

