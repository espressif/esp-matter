/***************************************************************************//**
 * @file
 * @brief API for enabling SWO and ETM trace.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdbool.h>
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "bsp_trace.h"
#include "bsp.h"

#if (defined(BSP_ETM_TRACE) && defined(ETM_PRESENT)) \
  || defined(GPIO_TRACECLK_PORT)

#if !defined(BSP_TRACE_ETM_LOC)
#define BSP_TRACE_ETM_LOC      0
#endif
#if !defined(BSP_TRACE_ETM_CLKLOC)
#define BSP_TRACE_ETM_CLKLOC   BSP_TRACE_ETM_LOC
#endif
#if !defined(BSP_TRACE_ETM_TD0LOC)
#define BSP_TRACE_ETM_TD0LOC   BSP_TRACE_ETM_LOC
#endif
#if !defined(BSP_TRACE_ETM_TD1LOC)
#define BSP_TRACE_ETM_TD1LOC   BSP_TRACE_ETM_LOC
#endif
#if !defined(BSP_TRACE_ETM_TD2LOC)
#define BSP_TRACE_ETM_TD2LOC   BSP_TRACE_ETM_LOC
#endif
#if !defined(BSP_TRACE_ETM_TD3LOC)
#define BSP_TRACE_ETM_TD3LOC   BSP_TRACE_ETM_LOC
#endif

/**************************************************************************//**
 * @brief Configure EFM32 for ETM trace output.
 * @note  For DK kits: Configure ETM trace on kit configuration menu as well.
 *****************************************************************************/
void BSP_TraceEtmSetup(void)
{
#if defined(GPIO_TRACECLK_PORT)

  /* Setup the GPIO for trace clock and data lines. */
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet((GPIO_Port_TypeDef)GPIO_TRACEDATA0_PORT,
                  GPIO_TRACEDATA0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)GPIO_TRACECLK_PORT,
                  GPIO_TRACECLK_PIN, gpioModePushPull, 0);
  GPIO->TRACEROUTEPEN = GPIO_TRACEROUTEPEN_TRACEDATA0PEN
                        | GPIO_TRACEROUTEPEN_TRACECLKPEN;

  /* Additional settings to counter for incomplete trace initialization
     in debugger tools. */

  /* Recommended to be set according to D1.2.36 of ArmV8-M ARM. */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /* Set TPIU formatting to Parallel Trace Port mode. */
  TPI->SPPR = 0;

#else
  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);

  /* Configure trace output */
#if defined(_GPIO_ROUTE_TCLKPEN_MASK)
  /* Enable Port D, pins 3,4,5,6 for ETM Trace Data output */
  GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE3_MASK) | GPIO_P_MODEL_MODE3_PUSHPULL;
  GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE4_MASK) | GPIO_P_MODEL_MODE4_PUSHPULL;
  GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE5_MASK) | GPIO_P_MODEL_MODE5_PUSHPULL;
  GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE6_MASK) | GPIO_P_MODEL_MODE6_PUSHPULL;

  /* Enable Port D, pin 7 for DBG_TCLK */
  GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE7_MASK) | GPIO_P_MODEL_MODE7_PUSHPULL;

  GPIO->ROUTE = GPIO->ROUTE | GPIO_ROUTE_TCLKPEN | GPIO_ROUTE_TD0PEN | GPIO_ROUTE_TD1PEN
                | GPIO_ROUTE_TD2PEN | GPIO_ROUTE_TD3PEN
                | GPIO_ROUTE_ETMLOCATION_LOC0;
#else

  /* Enable GPIO Pins for ETM Trace Data output and ETM Clock */
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETM_TCLK_PORT(BSP_TRACE_ETM_CLKLOC), AF_ETM_TCLK_PIN(BSP_TRACE_ETM_CLKLOC), gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETM_TD0_PORT(BSP_TRACE_ETM_TD0LOC), AF_ETM_TD0_PIN(BSP_TRACE_ETM_TD0LOC), gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETM_TD1_PORT(BSP_TRACE_ETM_TD1LOC), AF_ETM_TD1_PIN(BSP_TRACE_ETM_TD1LOC), gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETM_TD2_PORT(BSP_TRACE_ETM_TD2LOC), AF_ETM_TD2_PIN(BSP_TRACE_ETM_TD2LOC), gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETM_TD3_PORT(BSP_TRACE_ETM_TD3LOC), AF_ETM_TD3_PIN(BSP_TRACE_ETM_TD3LOC), gpioModePushPull, 0);
#if defined(_GPIO_ROUTELOC0_ETMLOC_MASK)
  GPIO->ROUTELOC0 = (GPIO->ROUTELOC0 & ~_GPIO_ROUTELOC0_ETMLOC_MASK)
                    | (BSP_TRACE_ETM_LOC << _GPIO_ROUTELOC0_ETMLOC_SHIFT);
#else
  GPIO->ROUTELOC1 = (BSP_TRACE_ETM_CLKLOC << _GPIO_ROUTELOC1_ETMTCLKLOC_SHIFT)
                    | (BSP_TRACE_ETM_TD0LOC << _GPIO_ROUTELOC1_ETMTD0LOC_SHIFT)
                    | (BSP_TRACE_ETM_TD1LOC << _GPIO_ROUTELOC1_ETMTD1LOC_SHIFT)
                    | (BSP_TRACE_ETM_TD2LOC << _GPIO_ROUTELOC1_ETMTD2LOC_SHIFT)
                    | (BSP_TRACE_ETM_TD3LOC << _GPIO_ROUTELOC1_ETMTD3LOC_SHIFT);
#endif
  GPIO->ROUTEPEN = GPIO->ROUTEPEN
                   | GPIO_ROUTEPEN_ETMTCLKPEN
                   | GPIO_ROUTEPEN_ETMTD0PEN
                   | GPIO_ROUTEPEN_ETMTD1PEN
                   | GPIO_ROUTEPEN_ETMTD2PEN
                   | GPIO_ROUTEPEN_ETMTD3PEN;
#endif
#endif /* defined(GPIO_TRACECLK_PORT) */
}
#endif

#if defined(_GPIO_ROUTE_SWOPEN_MASK) || defined(_GPIO_ROUTEPEN_SWVPEN_MASK) \
  || defined(GPIO_TRACEROUTEPEN_SWVPEN)
/**************************************************************************//**
 * @brief  Configure SWO trace output for Energy Profiler.
 * @note   Enabling SWO trace will add ~150uA current consumption in EM0.
 * @return Always true
 *****************************************************************************/
bool BSP_TraceProfilerSetup(void)
{
  uint32_t freq;
  uint32_t div;
  /* Enable GPIO clock */
#if defined(_CMU_HFPERCLKEN0_GPIO_MASK)
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
#elif defined(_CMU_HFBUSCLKEN0_GPIO_MASK)
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
#endif

  /* Enable Serial wire output pin */
#if defined(_GPIO_ROUTE_SWOPEN_MASK)
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;
#elif defined(_GPIO_ROUTEPEN_SWVPEN_MASK)
  GPIO->ROUTEPEN |= GPIO_ROUTEPEN_SWVPEN;
#elif defined(GPIO_TRACEROUTEPEN_SWVPEN)
  GPIO->TRACEROUTEPEN |= GPIO_TRACEROUTEPEN_SWVPEN;
#endif

  /* Set correct location */
#if defined(_GPIO_ROUTE_SWOPEN_MASK)
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | BSP_TRACE_SWO_LOCATION;
#elif defined(_GPIO_ROUTEPEN_SWVPEN_MASK)
  GPIO->ROUTELOC0 = (GPIO->ROUTELOC0 & ~(_GPIO_ROUTELOC0_SWVLOC_MASK)) | BSP_TRACE_SWO_LOCATION;
#endif

  /* Enable output on correct pin. */
  TRACE_ENABLE_PINS();

#if (_SILICON_LABS_32B_SERIES < 2)
  /* Enable debug clock AUXHFRCO */
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

  /* Wait until clock is ready */
  while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY)) ;
#endif

  /* Enable trace in core debug */
  CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* Enable PC and IRQ sampling output */
  DWT->CTRL = 0x400113FF;

  /* Set TPIU prescaler for the current debug clock frequency. Target frequency
     is 875 kHz so we choose a divider that gives us the closest match.
     Actual divider is TPI->ACPR + 1. */
#if (_SILICON_LABS_32B_SERIES < 2)
  freq = CMU_ClockFreqGet(cmuClock_DBG) + (875000 / 2);
#else //_SILICON_LABS_32B_SERIES
  freq = CMU_ClockFreqGet(cmuClock_TRACECLK) + (875000 / 2);
#endif //_SILICON_LABS_32B_SERIES
  div  = freq / 875000;
  TPI->ACPR = div - 1;

  /* Set protocol to NRZ */
  TPI->SPPR = 2;

  /* Disable continuous formatting */
  TPI->FFCR = 0x100;

  /* Unlock ITM and output data */
  ITM->LAR = 0xC5ACCE55;
  ITM->TCR = 0x10009;

  /* ITM Channel 0 is used for UART output */
  ITM->TER |= (1UL << 0);

  return true;
}

/**************************************************************************//**
 * @brief  Energy Profiler SWO setup.
 * @deprecated
 *         Deprecated function. New code should call @ref BSP_TraceProfilerSetup().
 *****************************************************************************/
void BSP_TraceSwoSetup(void)
{
  (void)BSP_TraceProfilerSetup();
}
#endif
