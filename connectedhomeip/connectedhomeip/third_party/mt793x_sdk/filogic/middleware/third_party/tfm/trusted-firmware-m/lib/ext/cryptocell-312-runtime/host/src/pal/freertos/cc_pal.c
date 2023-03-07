/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CC_IOT

/************* Include Files ****************/
#include "cc_pal_init.h"
#include "cc_pal_dma_plat.h"
#include "cc_pal_log.h"
#include "dx_reg_base_host.h"
#include "cc_pal_mutex.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_pal_pm.h"
#include "cc_pal_interrupt_ctrl_plat.h"

extern CC_PalMutex CCSymCryptoMutex;
extern CC_PalMutex CCAsymCryptoMutex;
extern CC_PalMutex CCRndCryptoMutex;
extern CC_PalMutex *pCCRndCryptoMutex;
extern CC_PalMutex CCApbFilteringRegMutex;

#define PAL_WORKSPACE_MEM_BASE_ADDR     0
#define PAL_WORKSPACE_MEM_SIZE          0


/**
 * @brief   PAL layer entry point.
 *          The function initializes customer platform sub components,
 *           such as memory mapping used later by CRYS to get physical contiguous memory.
 *
 *
 * @return Returns a non-zero value in case of failure
 */
int CC_PalInit(void)
{

    CCError_t rc = CC_FAIL;

    /* Currently in FreeRtos palDma is not needed - and therefore is implemented as empty. */
    rc = CC_PalDmaInit(PAL_WORKSPACE_MEM_SIZE, PAL_WORKSPACE_MEM_BASE_ADDR);
    if (rc != CC_SUCCESS)
        {
            return 1;
        }

    /* Initialize power management module */
    CC_PalPowerSaveModeInit();

    /* Initialize mutex that protects shared memory and crypto access */
    rc = CC_PalMutexCreate(&CCSymCryptoMutex);
    if (rc != CC_SUCCESS)
        {
            CC_PalAbort("Fail to create SYM mutex\n");
        }
    /* Initialize mutex that protects shared memory and crypto access */
    rc = CC_PalMutexCreate(&CCAsymCryptoMutex);
    if (rc != CC_SUCCESS)
        {
            CC_PalAbort("Fail to create ASYM mutex\n");
        }
    /* Initialize mutex that protects shared memory and crypto access */
    rc = CC_PalMutexCreate(&CCRndCryptoMutex);
    if (rc != CC_SUCCESS) {
            CC_PalAbort("Fail to create RND mutex\n");
    }
    pCCRndCryptoMutex = &CCRndCryptoMutex;

    /* Initialize mutex that protects APBC access */
    rc = CC_PalMutexCreate(&CCApbFilteringRegMutex);
    if (rc != 0) {
            CC_PalAbort("Fail to create APBC mutex\n");
    }

    CC_PalInitIrq();

    return 0;
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
    CCError_t err = CC_FAIL;

    CC_PalDmaTerminate();
    CC_PalFinishIrq();

    err = CC_PalMutexDestroy(&CCSymCryptoMutex);
    if (err != CC_SUCCESS)
        {
            CC_PAL_LOG_DEBUG("failed to destroy mutex CCSymCryptoMutex\n");
        }

    CC_PalMemSetZero(&CCSymCryptoMutex, sizeof(CC_PalMutex));

    err = CC_PalMutexDestroy(&CCAsymCryptoMutex);
    if (err != CC_SUCCESS)
        {
            CC_PAL_LOG_DEBUG("failed to destroy mutex CCAsymCryptoMutex\n");
        }

    CC_PalMemSetZero(&CCAsymCryptoMutex, sizeof(CC_PalMutex));

    err = CC_PalMutexDestroy(&CCRndCryptoMutex);
    if (err != CC_SUCCESS)
        {
            CC_PAL_LOG_DEBUG("failed to destroy mutex CCRndCryptoMutex\n");
        }
    CC_PalMemSetZero(&CCRndCryptoMutex, sizeof(CC_PalMutex));


    err = CC_PalMutexDestroy(&CCApbFilteringRegMutex);
    if (err != 0){
            CC_PAL_LOG_DEBUG("failed to destroy mutex CCApbFilteringRegMutex\n");
    }
    CC_PalMemSetZero(&CCApbFilteringRegMutex, sizeof(CC_PalMutex));
}

#endif

