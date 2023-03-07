/*
** ###################################################################
**     Processor:           MIMXRT595SFFOA_cm33
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
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
 * @file MIMXRT595S_cm33
 * @version 2.0
 * @date 2019-07-22
 * @brief Device specific configuration file for MIMXRT595S_cm33 (implementation
 *        file)
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

/* Get FRO DIV clock from FRODIVSEL */
static uint32_t getFroDivClk(void)
{
  uint32_t freq = 0;

  switch ((CLKCTL0->FRODIVSEL) & CLKCTL0_FRODIVSEL_SEL_MASK)
  {
    case CLKCTL0_FRODIVSEL_SEL(0):
      freq = CLK_FRO_96MHZ;
      break;
    case CLKCTL0_FRODIVSEL_SEL(1):
      freq = CLK_FRO_48MHZ;
      break;
    case CLKCTL0_FRODIVSEL_SEL(2):
      freq = CLK_FRO_24MHZ;
      break;
    case CLKCTL0_FRODIVSEL_SEL(3):
      freq = CLK_FRO_12MHZ;
      break;
    default:
      break;
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

__attribute__ ((weak)) void SystemInit (void) {
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

  SCB->CPACR |= ((3UL << 0*2) | (3UL << 1*2));    /* set CP0, CP1 Full Access (enable PowerQuad) */

  SCB->NSACR |= ((3UL << 0) | (3UL << 10));   /* enable CP0, CP1, CP10, CP11 Non-secure Access */

  if (CACHE64_POLSEL0->POLSEL == 0) /* set CAHCHE64 if not configured */
  {
      /* set command to invalidate all ways and write GO bit to initiate command */
      CACHE64_CTRL0->CCR = CACHE64_CTRL_CCR_INVW1_MASK | CACHE64_CTRL_CCR_INVW0_MASK;
      CACHE64_CTRL0->CCR |= CACHE64_CTRL_CCR_GO_MASK;
      /* Wait until the command completes */
      while (CACHE64_CTRL0->CCR & CACHE64_CTRL_CCR_GO_MASK)
      {
      }
      /* Enable cache, enable write buffer */
      CACHE64_CTRL0->CCR = (CACHE64_CTRL_CCR_ENWRBUF_MASK | CACHE64_CTRL_CCR_ENCACHE_MASK);

      /* Set whole FlexSPI0 space to write through. */
      CACHE64_POLSEL0->REG0_TOP = 0x07FFFC00U;
      CACHE64_POLSEL0->REG1_TOP = 0x0U;
      CACHE64_POLSEL0->POLSEL = 0x1U;

      __ISB();
      __DSB();
  }

  SystemInitHook();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {

  /* iMXRT5xx systemCoreClockUpdate */
  uint32_t freq = 0U;
  uint64_t freqTmp = 0U;

  switch ((CLKCTL0->MAINCLKSELB) & CLKCTL0_MAINCLKSELB_SEL_MASK)
  {
    case CLKCTL0_MAINCLKSELB_SEL(0): /* MAINCLKSELA clock */
      switch ((CLKCTL0->MAINCLKSELA) & CLKCTL0_MAINCLKSELA_SEL_MASK)
      {
        case CLKCTL0_MAINCLKSELA_SEL(0): /* Low Power Oscillator Clock (1m_lposc) */
          freq = CLK_LPOSC_1MHZ;
          break;
        case CLKCTL0_MAINCLKSELA_SEL(1): /* FRO DIV clock */
          freq = getFroDivClk();
          break;
        case CLKCTL0_MAINCLKSELA_SEL(2): /* OSC clock */
          freq = getOscClk();
          break;
        case CLKCTL0_MAINCLKSELA_SEL(3): /* FFRO 192MHz clock */
          freq = CLK_FRO_192MHZ;
          break;
        default:
          break;
      }
      break;

    case CLKCTL0_MAINCLKSELB_SEL(1): /* Main System PLL clock */
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
        freq = (uint64_t)freq * 18 /
               ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD0_MASK) >> CLKCTL0_SYSPLL0PFD_PFD0_SHIFT);
      }
      break;

    case CLKCTL0_MAINCLKSELB_SEL(2): /* RTC 32KHz clock */
      freq = CLK_RTC_32K_CLK;
      break;

    default:
      break;
  }

  SystemCoreClock = freq / ((CLKCTL0->SYSCPUAHBCLKDIV & CLKCTL0_SYSCPUAHBCLKDIV_DIV_MASK) + 1U);
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInitHook (void) {
  /* Void implementation of the weak function. */
}
