/*
** ###################################################################
**     Processor:           MIMXRT595SFFOA
**     Compiler:            XCC Compiler
**     Reference manual:    RT500 Reference Manual. Rev.0,07/2019
**     Version:             rev. 2.0, 2019-07-22
**     Build:               b190723
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2019 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2019-04-19)
**         Initial version.
**     - rev. 2.0 (2019-07-22)
**         Base on rev 0.5 RM.
**
** ###################################################################
*/

/*!
 * @file MIMXRT595S
 * @version 1.0
 * @date 230719
 * @brief Device specific configuration file for MIMXRT595S (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "fsl_device_registers.h"

/* Get OSC clock from SYSOSC_BYPASS */
static uint32_t getOscClk(void)
{
    return (CLKCTL0->SYSOSC_BYPASS == 0U) ? CLK_XTAL_OSC_CLK : ((CLKCTL0->SYSOSC_BYPASS == 1U) ? CLK_CLK_IN : 0U);
}

static uint32_t getSpllFreq(void)
{
    uint32_t freq    = 0U;
    uint64_t freqTmp = 0U;

    switch ((CLKCTL0->SYSPLL0CLKSEL) & CLKCTL0_SYSPLL0CLKSEL_SEL_MASK)
    {
        case CLKCTL0_SYSPLL0CLKSEL_SEL(0): /* FRO 24MHz clock */
            freq = CLK_FRO_24MHZ;
            break;
        case CLKCTL0_SYSPLL0CLKSEL_SEL(1): /* OSC clock */
            freq = getOscClk();
            break;
        default:
            break;
    }

    if (((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_BYPASS_MASK) == 0U)
    {
        /* PLL output frequency = Fref * (DIV_SELECT + NUM/DENOM). */
        freqTmp = ((uint64_t)freq * ((uint64_t)(CLKCTL0->SYSPLL0NUM))) / ((uint64_t)(CLKCTL0->SYSPLL0DENOM));
        freq *= ((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_MULT_MASK) >> CLKCTL0_SYSPLL0CTL0_MULT_SHIFT;
        freq += (uint32_t)freqTmp;
    }

    return freq;
}

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInit (void)
{
    SystemInitHook();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void)
{
    /* iMXRT5xx systemCoreClockUpdate */
    uint32_t freq = 0U;

    switch ((CLKCTL1->DSPCPUCLKSELB) & CLKCTL1_DSPCPUCLKSELB_SEL_MASK)
    {
        case CLKCTL1_DSPCPUCLKSELB_SEL(0): /* DSPCPUCLKSELA clock */
            switch ((CLKCTL1->DSPCPUCLKSELA) & CLKCTL1_DSPCPUCLKSELA_SEL_MASK)
            {
                case CLKCTL1_DSPCPUCLKSELA_SEL(0): /* FRO clock (FRO192M) */
                    freq = CLK_FRO_192MHZ;
                    break;
                case CLKCTL1_DSPCPUCLKSELA_SEL(1): /* OSC_CLK clock */
                    freq = getOscClk();
                    break;
                case CLKCTL1_DSPCPUCLKSELA_SEL(2): /* Low Power Oscillator Clock (1m_lposc) */
                    freq = CLK_LPOSC_1MHZ;
                    break;
                default:
                    break;
            }
            break;
        case CLKCTL1_DSPCPUCLKSELB_SEL(1): /* Main System PLL clock */
            freq = getSpllFreq();
            if (((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_BYPASS_MASK) == 0U)
            {
                freq = (uint64_t)freq * 18 /
                       ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD0_MASK) >> CLKCTL0_SYSPLL0PFD_PFD0_SHIFT);
            }
            break;
        case CLKCTL1_DSPCPUCLKSELB_SEL(2): /* DSP PLL clock */
            freq = getSpllFreq();
            if (((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_BYPASS_MASK) == 0U)
            {
                freq = (uint64_t)freq * 18 /
                       ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD1_MASK) >> CLKCTL0_SYSPLL0PFD_PFD1_SHIFT);
            }
            break;
        case CLKCTL1_DSPCPUCLKSELB_SEL(3): /* RTC 32KHz clock */
            freq = CLK_RTC_32K_CLK;
            break;
        default:
            break;
    }

    SystemCoreClock = freq / ((CLKCTL1->DSPCPUCLKDIV & 0xffU) + 1U);
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInitHook (void)
{
    /* Void implementation of the weak function. */
}
