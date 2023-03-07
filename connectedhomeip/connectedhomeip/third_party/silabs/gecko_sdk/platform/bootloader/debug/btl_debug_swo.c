/***************************************************************************//**
 * @file
 * @brief SWO debug component for Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "btl_debug.h"

#include "em_device.h"
#include "em_cmu.h"
#include "btl_debug_cfg.h"

void btl_debugInit(void)
{
//Below variable is supported for Cortex-M4,M33
#if ((__CORTEX_M == 4) || (__CORTEX_M == 33))
  uint32_t tpiu_prescaler_val;
#endif

#if defined(_CMU_HFBUSCLKEN0_GPIO_MASK)
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
#endif
#if defined(_CMU_HFPERCLKEN0_GPIO_MASK)
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
#endif

//Check for Series
#if defined(_SILICON_LABS_32B_SERIES_1)
#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_103)
#if defined (_GPIO_ROUTEPEN_MASK)
  GPIO->ROUTEPEN |= GPIO_ROUTEPEN_SWVPEN;
#endif
  // Set location 0
  GPIO->ROUTELOC0 = (GPIO->ROUTELOC0 & ~(_GPIO_ROUTELOC0_SWVLOC_MASK))
                    | SL_DEBUG_SWV_LOC;

  // Set TPIU prescaler to 22 (19 MHz / 22 = 863.63 kHz SWO speed)
  tpiu_prescaler_val = 22 - 1;
#endif
  // Enable output on pin
#if (SL_DEBUG_SWV_PIN > 7U)
  GPIO->P[SL_DEBUG_SWV_PORT].MODEH &= ~(_GPIO_P_MODEL_MODE0_MASK << (SL_DEBUG_SWV_PIN * 4U));
  GPIO->P[SL_DEBUG_SWV_PORT].MODEH |= _GPIO_P_MODEL_MODE0_PUSHPULL << (SL_DEBUG_SWV_PIN * 4U);
#else
  GPIO->P[SL_DEBUG_SWV_PORT].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (SL_DEBUG_SWV_PIN * 4U));
  GPIO->P[SL_DEBUG_SWV_PORT].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULL << (SL_DEBUG_SWV_PIN * 4U);
#endif

#elif defined(_SILICON_LABS_32B_SERIES_2)
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;
#endif

  /* Enable output on pin */
  GPIO->P[GPIO_SWV_PORT].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (GPIO_SWV_PIN * 4));
  GPIO->P[GPIO_SWV_PORT].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULL << (GPIO_SWV_PIN * 4);
  GPIO->TRACEROUTEPEN |= GPIO_TRACEROUTEPEN_SWVPEN;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_215) || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_220)
#if defined(_CMU_TRACECLKCTRL_CLKSEL_MASK) && defined(_CMU_TRACECLKCTRL_CLKSEL_HFRCOEM23)
#if defined(CMU_CLKEN0_HFRCOEM23)
  CMU->CLKEN0_SET = CMU_CLKEN0_HFRCOEM23;
#endif
  // Select HFRCOEM23 as source for TRACECLK
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_HFRCOEM23);
#endif
#else
#if defined(_CMU_TRACECLKCTRL_CLKSEL_MASK)
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_230)
  /* Select SYSCLK as source for TRACECLK */
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_SYSCLK);
#else
  /* Select HFRCOEM23 as source for TRACECLK */
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_HFRCOEM23);
#endif
#endif
#endif

  /* Set TPIU prescaler to get a 863.63 kHz SWO speed */
  tpiu_prescaler_val = CMU_ClockFreqGet(cmuClock_TRACECLK) / 863630 - 1;

#else
#error Unknown device family!
#endif

#if (_SILICON_LABS_32B_SERIES < 2)
  // Enable debug clock AUXHFRCO
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;
  while ((CMU->STATUS & CMU_STATUS_AUXHFRCORDY) == 0UL) {
  }
#endif
//Below registers are supported for Cortex-M3,M4,M33
#if ((__CORTEX_M == 4) || (__CORTEX_M == 33))
  // Enable trace in core debug
  CoreDebug->DHCSR |= 1UL;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  // Enable PC and IRQ sampling output
  DWT->CTRL = 0x400113FFUL;

  // Set TPIU Prescaler
  TPI->ACPR = tpiu_prescaler_val;

  // Set protocol to NRZ
  TPI->SPPR = 2;

  // Disable continuous formatting
  TPI->FFCR = 0x100;

  // Unlock ITM and output data
  ITM->LAR = 0xC5ACCE55UL;
  ITM->TCR = 0x10009UL;

  // ITM Channel 0 is used for UART output
  ITM->TER |= (1UL << 0);
#endif
}

void btl_debugWriteChar(char c)
{
  (void) ITM_SendChar((uint32_t)c);
}
