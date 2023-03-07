/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_wdt.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mw_wdt"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
static WDT_Type *const s_wdtBases[] = WDT_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of WDT clock name. */
static const clock_ip_name_t s_wdtClock[] = WDT_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t WDT_GetInstance(WDT_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_wdtBases); instance++)
    {
        if (s_wdtBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_wdtBases));

    return instance;
}

/*!
 * brief Prepares an available pre-defined setting for module's configuration.
 * This function initializes the WDT configuration structure to default values.
 *   config->timeoutValue = kWDT_TimeoutVal2ToThePowerOf16;
 *   config->timeoutMode =kWDT_TimeoutModeReset;
 *   config->resetWidth = kWDT_ResetWidthLen2;
 *   config->enableWDT = false;
 * @param config Pointer to the WDT configuration structure, @ref wdt_config_t.
 */
void WDT_GetDefaultConfig(wdt_config_t *config)
{
    config->timeoutValue = kWDT_TimeoutVal2ToThePowerOf16;
    config->timeoutMode  = kWDT_ModeTimeoutReset;
    config->resetWidth   = kWDT_ResetPulseLen8;
    config->enableWDT    = false;
}

/*!
 * brief Initializes the WDT module with input configuration
 * Call this function to do initialization configuration for WDT module.
 * The configurations are:
 * - WDT counter timeout value
 * - Timeout response mode select
 * - Reset assert pulse width configuration
 * - WDT enable/disable
 * param base   WDT peripheral base address.
 * param config The pointer to WDT configuration structure, @ref wdt_config_t.
 */
void WDT_Init(WDT_Type *base, const wdt_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Set configuration */
    CLOCK_EnableClock(s_wdtClock[WDT_GetInstance(base)]);
#endif

    uint32_t u32ConfigVal = base->WDT_TORR;
    u32ConfigVal &= ~(WDT_WDT_TORR_TOP_INIT_MASK | WDT_WDT_TORR_TOP_MASK);
    u32ConfigVal |=
        WDT_WDT_TORR_TOP_INIT((uint32_t)config->timeoutValue) | WDT_WDT_TORR_TOP((uint32_t)config->timeoutValue);

    base->WDT_TORR = u32ConfigVal;

    u32ConfigVal = base->WDT_CR;
    u32ConfigVal &= ~(WDT_WDT_CR_RMOD_MASK | WDT_WDT_CR_RPL_MASK | WDT_WDT_CR_WDT_EN_MASK);
    u32ConfigVal |= (WDT_WDT_CR_RMOD((uint32_t)config->timeoutMode) | WDT_WDT_CR_RPL((uint32_t)config->resetWidth) |
                     WDT_WDT_CR_WDT_EN(config->enableWDT));

    base->WDT_CR = u32ConfigVal;
}

void WDT_Refresh(WDT_Type *base)
{
    base->WDT_CRR = WDT_REFRESH_PATTERN;
}
