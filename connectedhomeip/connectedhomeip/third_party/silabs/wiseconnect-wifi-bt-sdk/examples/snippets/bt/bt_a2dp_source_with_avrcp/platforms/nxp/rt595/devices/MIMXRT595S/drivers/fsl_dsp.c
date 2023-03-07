/*
 * Copyright 2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dsp.h"
#include "fsl_reset.h"
#include "fsl_common.h"
#include "fsl_power.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Initializing DSP core.
 *
 * Power up DSP
 * Enable DSP clock
 * Reset DSP peripheral
 */
void DSP_Init(void)
{
    POWER_DisablePD(kPDRUNCFG_PD_DSP);
    POWER_ApplyPD();

    CLOCK_EnableClock(kCLOCK_Dsp);
    RESET_PeripheralReset(kDSP_RST_SHIFT_RSTn);
}

/*!
 * @brief Deinitializing DSP core.
 */
void DSP_Deinit(void)
{
    DSP_Stop();

    CLOCK_DisableClock(kCLOCK_Dsp);
    POWER_EnablePD(kPDRUNCFG_PD_DSP);
    POWER_ApplyPD();
}

/*!
 * @brief Copy DSP image to destination address.
 *
 * Copy DSP image from source address to destination address with given size.
 *
 * @param dspCopyImage Structure contains information for DSP copy image to destination address.
 */
void DSP_CopyImage(dsp_copy_image_t *dspCopyImage)
{
    assert(dspCopyImage != NULL);
    assert(dspCopyImage->srcAddr != NULL);
    assert(dspCopyImage->destAddr != NULL);

    uint32_t *srcAddr  = dspCopyImage->srcAddr;
    uint32_t *destAddr = dspCopyImage->destAddr;
    uint32_t size      = dspCopyImage->size;

    while (size > 0)
    {
        *destAddr++ = *srcAddr++;
        size -= 4;
    }
}
