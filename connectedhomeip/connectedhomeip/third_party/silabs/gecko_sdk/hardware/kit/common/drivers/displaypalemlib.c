/***************************************************************************//**
 * @file
 * @brief Platform Abstraction Layer (PAL) for DISPLAY driver on EMLIB based
 *        platforms.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "bsp.h"
#include "udelay.h"

/* DISPLAY driver inclustions */
#include "displayconfigall.h"
#include "displaypal.h"

#ifdef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE

#if !defined(RTCC_PRESENT) && !defined(RTC_PRESENT) && defined(BURTC_PRESENT)
#define PAL_CLOCK_BURTC
#include "em_burtc.h"
#elif defined(RTCC_PRESENT) && (RTCC_COUNT > 0) && !defined(PAL_CLOCK_RTC)
#define PAL_CLOCK_RTCC
#include "em_rtcc.h"
#else
#define PAL_CLOCK_RTC
#include "em_rtc.h"
#endif

#ifdef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
#include "em_prs.h"
#endif

#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/*******************************************************************************
 ********************************  STATICS  ************************************
 ******************************************************************************/

#ifdef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE
#ifndef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
/* GPIO port and pin used for the PAL_GpioPinAutoToggle function. */
static unsigned int gpioPortNo;
static unsigned int gpioPinNo;
#endif

static void palClockSetup(CMU_Clock_TypeDef clock);

#if defined(PAL_CLOCK_BURTC)
static void burtcSetup(unsigned int frequency);
#elif defined(PAL_CLOCK_RTCC)
static void rtccSetup(unsigned int frequency);
#else
static void rtcSetup(unsigned int frequency);
#endif

#endif

/*******************************************************************************
 **************************     GLOBAL FUNCTIONS      **************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief   Initialize the PAL SPI interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL SPI inteface functions for the textdisplay example on
 *          EFM32GG_STK3700.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiInit(void)
{
  EMSTATUS                status    = PAL_EMSTATUS_OK;
  USART_InitSync_TypeDef  usartInit = USART_INITSYNC_DEFAULT;

  /* Initialize USART for SPI transaction */
  CMU_ClockEnable(PAL_SPI_USART_CLOCK, true);
  usartInit.baudrate = PAL_SPI_BAUDRATE;

  USART_InitSync(PAL_SPI_USART_UNIT, &usartInit);

#if defined(GPIO_USART_ROUTEEN_TXPEN)
  GPIO->USARTROUTE[PAL_SPI_USART_INDEX].ROUTEEN =
    GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_CLKPEN;
  GPIO->USARTROUTE[PAL_SPI_USART_INDEX].TXROUTE =
    (LCD_PORT_SI << _GPIO_USART_TXROUTE_PORT_SHIFT)
    | (LCD_PIN_SI << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[PAL_SPI_USART_INDEX].CLKROUTE =
    (LCD_PORT_SCLK << _GPIO_USART_CLKROUTE_PORT_SHIFT)
    | (LCD_PIN_SCLK << _GPIO_USART_CLKROUTE_PIN_SHIFT);

#elif defined(USART_ROUTEPEN_TXPEN)
  PAL_SPI_USART_UNIT->ROUTEPEN = USART_ROUTEPEN_TXPEN
                                 | USART_ROUTEPEN_CLKPEN;
  PAL_SPI_USART_UNIT->ROUTELOC0 = (PAL_SPI_USART_UNIT->ROUTELOC0
                                   & ~(_USART_ROUTELOC0_TXLOC_MASK
                                       | _USART_ROUTELOC0_CLKLOC_MASK) )
                                  | (PAL_SPI_USART_LOCATION_TX
                                     << _USART_ROUTELOC0_TXLOC_SHIFT)
                                  | (PAL_SPI_USART_LOCATION_SCLK
                                     << _USART_ROUTELOC0_CLKLOC_SHIFT);

#else
  PAL_SPI_USART_UNIT->ROUTE = (USART_ROUTE_CLKPEN | USART_ROUTE_TXPEN | PAL_SPI_USART_LOCATION);
#endif

  return status;
}

/**************************************************************************//**
 * @brief   Shutdown the PAL SPI interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL SPI interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiShutdown(void)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  /* Disable the USART device used for SPI. */
  USART_Enable(PAL_SPI_USART_UNIT, usartDisable);

  /* Disable the USART clock. */
  CMU_ClockEnable(PAL_SPI_USART_CLOCK, false);

  return status;
}

/**************************************************************************//**
 * @brief      Transmit data on the SPI interface.
 *
 * @param[in]  data    Pointer to the data to be transmitted.
 * @param[in]  len     Length of data to transmit.
 *
 * @return     EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiTransmit(uint8_t* data, unsigned int len)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  while (len > 0) {
    /* Send only one byte if len==1 or data pointer is not aligned at a 16 bit
       word location in memory. */
    if ((len == 1) || ((unsigned int)data & 0x1)) {
      USART_Tx(PAL_SPI_USART_UNIT, *(uint8_t*)data);
      len--;
      data++;
    } else {
      USART_TxDouble(PAL_SPI_USART_UNIT, *(uint16_t*)data);
      len  -= 2;
      data += 2;
    }
  }

  /* Wait for transfer to finish */
  while (!(PAL_SPI_USART_UNIT->STATUS & USART_STATUS_TXC)) ;

  return status;
}

/**************************************************************************//**
 * @brief   Initialize the PAL Timer interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL Timer interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerInit(void)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  UDELAY_Calibrate();

  return status;
}

/**************************************************************************//**
 * @brief   Shutdown the PAL Timer interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL Timer interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerShutdown(void)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  /* Nothing to do since the UDELAY_Delay does not use any resources after
     the UDELAY_Calibrate has been called. The UDELAY_Calibrate uses the
     RTC to calibrate the delay loop, and restores the RTC after use. */

  return status;
}

/**************************************************************************//**
 * @brief   Delay for the specified number of micro seconds.
 *
 * @param[in] usecs   Number of micro seconds to delay.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerMicroSecondsDelay(unsigned int usecs)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  UDELAY_Delay(usecs);

  return status;
}

#ifdef PAL_TIMER_REPEAT_FUNCTION
/**************************************************************************//**
 * @brief   Call a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerRepeat(void(*pFunction)(void*),
                         void* argument,
                         unsigned int frequency)
{
  if (0 != PAL_TIMER_REPEAT_FUNCTION(pFunction, argument, frequency)) {
    return PAL_EMSTATUS_REPEAT_FAILED;
  } else {
    return EMSTATUS_OK;
  }
}
#endif

/**************************************************************************//**
 * @brief   Initialize the PAL GPIO interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL GPIO interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioInit(void)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  /* Enable the GPIO clock in order to access the GPIO module. */
  CMU_ClockEnable(cmuClock_GPIO, true);

  return status;
}

/**************************************************************************//**
 * @brief   Shutdown the PAL GPIO interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL GPIO interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioShutdown(void)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  /* Enable the GPIO clock in order to access the GPIO module. */
  CMU_ClockEnable(cmuClock_GPIO, false);

  return status;
}

/***************************************************************************//**
 * @brief
 *   Set the mode for a GPIO pin.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin number in the port.
 *
 * @param[in] mode
 *   The desired pin mode.
 *
 * @param[in] platformSpecific
 *   Platform specific value which may need to be set.
 *   For EFM32:
 *   Value to set for pin in DOUT register. The DOUT setting is important for
 *   even some input mode configurations, determining pull-up/down direction.
 *
 * @return  EMSTATUS code of the operation.
 ******************************************************************************/
EMSTATUS PAL_GpioPinModeSet(unsigned int   port,
                            unsigned int   pin,
                            PAL_GpioMode_t mode,
                            unsigned int   platformSpecific)
{
  EMSTATUS status = PAL_EMSTATUS_OK;
  GPIO_Mode_TypeDef   emGpioMode;

  /* Convert PAL pin mode to GPIO_Mode_TypeDef defined in em_gpio.h.  */
  switch (mode) {
    case palGpioModePushPull:
      emGpioMode = gpioModePushPull;
      break;
    default:
      return PAL_EMSTATUS_INVALID_PARAM;
  }

  GPIO_PinModeSet((GPIO_Port_TypeDef) port, pin, emGpioMode, platformSpecific);

  return status;
}

/***************************************************************************//**
 * @brief
 *   Set a single pin in GPIO data out register to 1.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to set.
 *
 * @return  EMSTATUS code of the operation.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutSet(unsigned int port, unsigned int pin)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  GPIO_PinOutSet((GPIO_Port_TypeDef) port, pin);

  return status;
}

/***************************************************************************//**
 * @brief
 *   Set a single pin in GPIO data out port register to 0.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to set.
 *
 * @return  EMSTATUS code of the operation.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutClear(unsigned int port, unsigned int pin)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  GPIO_PinOutClear((GPIO_Port_TypeDef) port, pin);

  return status;
}

/***************************************************************************//**
 * @brief
 *   Toggle a single pin in GPIO port data out register.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to toggle.
 *
 * @return  EMSTATUS code of the operation.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutToggle(unsigned int port, unsigned int pin)
{
  EMSTATUS status = PAL_EMSTATUS_OK;

  GPIO_PinOutToggle((GPIO_Port_TypeDef) port, pin);

  return status;
}

#ifdef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE
/**************************************************************************//**
 * @brief   Toggle a GPIO pin automatically at the given frequency.
 *
 * @param[in] gpioPort  GPIO port number of GPIO ping to toggle.
 * @param[in] gpioPin   GPIO pin number.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioPinAutoToggle(unsigned int gpioPort,
                               unsigned int gpioPin,
                               unsigned int frequency)
{
  EMSTATUS status = EMSTATUS_OK;

#ifdef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY

  /* Setup PRS to drive the GPIO pin which is connected to the
     display com inversion pin (EXTCOMIN) using the RTC COMP0 signal or
     RTCC CCV1 signal as source. */
#if defined(PAL_CLOCK_BURTC)
  uint32_t  source = PRS_ASYNC_CH_CTRL_SOURCESEL_BURTC;
  uint32_t  signal = PRS_ASYNC_CH_CTRL_SIGSEL_BURTCCOMP;
#elif defined(PAL_CLOCK_RTCC)
#if defined(PRS_ASYNC_CH_CTRL_SIGSEL_DEFAULT)
  uint32_t  source  = PRS_ASYNC_CH_CTRL_SOURCESEL_RTCC;
  uint32_t  signal  = PRS_ASYNC_CH_CTRL_SIGSEL_RTCCCCV1;
#else
  uint32_t  source  = PRS_CH_CTRL_SOURCESEL_RTCC;
  uint32_t  signal  = PRS_CH_CTRL_SIGSEL_RTCCCCV1;
#endif
#else
  uint32_t  source  = PRS_CH_CTRL_SOURCESEL_RTC;
  uint32_t  signal  = PRS_CH_CTRL_SIGSEL_RTCCOMP0;
#endif

  /* Make sure frequency is non-zero */
  EFM_ASSERT(frequency);

  /* Enable PRS clock */
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up PRS to trigger from an RTC compare match */
  PRS_SourceAsyncSignalSet(LCD_AUTO_TOGGLE_PRS_CH, source, signal);

  /* This outputs the PRS pulse on the EXTCOMIN pin */
#if defined(_SILICON_LABS_32B_SERIES_2)
  PRS_PinOutput(LCD_AUTO_TOGGLE_PRS_CH, prsTypeAsync, (GPIO_Port_TypeDef)gpioPort, gpioPin);
#elif defined(_SILICON_LABS_32B_SERIES_1)
  LCD_AUTO_TOGGLE_PRS_ROUTELOC();
  PRS->ROUTEPEN |= LCD_AUTO_TOGGLE_PRS_ROUTEPEN;
#else
  PRS->ROUTE = (PRS->ROUTE & ~_PRS_ROUTE_LOCATION_MASK)
               | LCD_AUTO_TOGGLE_PRS_ROUTE_LOC;
  PRS->ROUTE |= LCD_AUTO_TOGGLE_PRS_ROUTE_PEN;
#endif

#else
  /* Store GPIO pin data. */
  gpioPortNo = gpioPort;
  gpioPinNo  = gpioPin;
#endif

  /* Setup GPIO pin. */
  GPIO_PinModeSet((GPIO_Port_TypeDef)gpioPort, gpioPin, gpioModePushPull, 0);

#if defined(PAL_CLOCK_BURTC)
  /* Setup BURTC to toggle PRS or generate interrupts at given frequency */
  burtcSetup(frequency);
#elif defined(PAL_CLOCK_RTCC)
  /* Setup RTCC to toggle PRS or generate interrupts at given frequency. */
  rtccSetup(frequency);
#else
  /* Setup RTC to toggle PRS or generate interrupts at given frequency. */
  rtcSetup(frequency);
#endif

  return status;
}

#ifndef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
#if defined(PAL_CLOCK_RTC)
/**************************************************************************//**
 * @brief   RTC Interrupt handler which toggles GPIO pin.
 *
 * @return  N/A
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  /* Clear interrupt source */
  RTC_IntClear(RTC_IF_COMP0);

  /* Toggle GPIO pin. */
  GPIO_PinOutToggle((GPIO_Port_TypeDef)gpioPortNo, gpioPinNo);
}
#endif /* PAL_CLOCK_RTC */

#if defined(PAL_CLOCK_RTCC)
/**************************************************************************//**
 * @brief   RTCC Interrupt handler which toggles GPIO pin.
 *
 * @return  N/A
 *****************************************************************************/
void RTCC_IRQHandler(void)
{
  /* Clear interrupt source */
  RTCC_IntClear(RTCC_IF_CC1);

  /* Toggle GPIO pin. */
  GPIO_PinOutToggle((GPIO_Port_TypeDef)gpioPortNo, gpioPinNo);
}
#endif /* PAL_CLOCK_RTCC */

#if defined(PAL_CLOCK_BURTC)
/**************************************************************************//**
 * @brief   BURTC Interrupt handler which toggles GPIO pin.
 *
 * @return  N/A
 *****************************************************************************/
void BURTC_IRQHandler(void)
{
  /* Clear interrupt source */
  BURTC_IntClear(BURTC_IF_COMPIF);

  /* Toggle GPIO pin. */
  GPIO_PinOutToggle((GPIO_Port_TypeDef)gpioPortNo, gpioPinNo);
}
#endif /* PAL_CLOCK_BURTC */
#endif /* INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY */

/**************************************************************************//**
 * @brief   Setup clocks necessary to drive RTC/RTCC for EXTCOM GPIO pin.
 *
 * @return  N/A
 *****************************************************************************/
static void palClockSetup(CMU_Clock_TypeDef clock)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  /* Enable LE domain registers */
  CMU_ClockEnable(cmuClock_CORELE, true);
#endif

#if (defined(PAL_CLOCK_RTC) && defined(PAL_RTC_CLOCK_LFXO) )    \
  || (defined(PAL_CLOCK_RTCC) && defined(PAL_RTCC_CLOCK_LFXO) ) \
  || (defined(PAL_CLOCK_BURTC) && defined(PAL_BURTC_CLOCK_LFXO) )
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_ClockSelectSet(clock, cmuSelect_LFXO);
#elif (defined(PAL_CLOCK_RTC) && defined(PAL_RTC_CLOCK_LFRCO) )  \
  || (defined(PAL_CLOCK_RTCC) && defined(PAL_RTCC_CLOCK_LFRCO) ) \
  || (defined(PAL_CLOCK_BURTC) && defined(PAL_BURTC_CLOCK_LFRCO) )
  CMU_ClockSelectSet(clock, cmuSelect_LFRCO);
#elif (defined(PAL_CLOCK_RTC) && defined(PAL_RTC_CLOCK_ULFRCO) )  \
  || (defined(PAL_CLOCK_RTCC) && defined(PAL_RTCC_CLOCK_ULFRCO) ) \
  || (defined(PAL_CLOCK_BURTC) && defined(PAL_BURTC_CLOCK_ULFRCO) )
  CMU_ClockSelectSet(clock, cmuSelect_ULFRCO);
#else
#error No clock source for RTC defined.
#endif
}

#if defined(PAL_CLOCK_RTC)
/**************************************************************************//**
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every second.
 *****************************************************************************/
static void rtcSetup(unsigned int frequency)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  palClockSetup(cmuClock_LFA);

  /* Set the prescaler. */
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_2);

  /* Enable RTC clock */
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Initialize RTC */
  rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtcInit.debugRun = false;  /* Halt RTC when debugging. */
  rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */

  RTC_Init(&rtcInit);

  /* Interrupt at given frequency. */
  RTC_CompareSet(0, ((CMU_ClockFreqGet(cmuClock_RTC) / frequency) - 1) & _RTC_COMP0_MASK);

#ifndef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
  /* Enable interrupt */
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_IntEnable(RTC_IEN_COMP0);
#endif
  RTC_CounterReset();
  /* Start Counter */
  RTC_Enable(true);
}
#endif /* PAL_CLOCK_RTC */

#if defined(PAL_CLOCK_RTCC)
/**************************************************************************//**
 * @brief Enables LFECLK and selects clock source for RTCC
 *        Sets up the RTCC to generate an interrupt every second.
 *****************************************************************************/
static void rtccSetup(unsigned int frequency)
{
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;

#if defined(_CMU_LFECLKEN0_MASK)
  palClockSetup(cmuClock_LFE);
#else
  palClockSetup(cmuClock_RTCC);
#endif
  /* Enable RTCC clock */
  CMU_ClockEnable(cmuClock_RTCC, true);

  /* Initialize RTC */
  rtccInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtccInit.debugRun = false;  /* Halt RTC when debugging. */
  rtccInit.cntWrapOnCCV1 = true;   /* Wrap around on CCV1 match. */
  RTCC_Init(&rtccInit);

  /* Interrupt at given frequency. */
  RTCC_CCChConf_TypeDef ccchConf = RTCC_CH_INIT_COMPARE_DEFAULT;
  ccchConf.compMatchOutAction = rtccCompMatchOutActionToggle;
  RTCC_ChannelInit(1, &ccchConf);
  RTCC_ChannelCCVSet(1, (CMU_ClockFreqGet(cmuClock_RTCC) / frequency) - 1);

#ifndef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
  /* Enable interrupt */
  NVIC_EnableIRQ(RTCC_IRQn);
  RTCC_IntEnable(RTCC_IEN_CC1);
#endif

  RTCC->CNT = _RTCC_CNT_RESETVALUE;
  /* Start Counter */
  RTCC_Enable(true);
}
#endif /* PAL_CLOCK_RTCC */

#if defined(PAL_CLOCK_BURTC)
/**************************************************************************//**
 * @brief Enables LFECLK and selects clock source for BURTC
 *        Sets up the BURTC to generate an interrupt every second.
 *****************************************************************************/
static void burtcSetup(unsigned int frequency)
{
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

  palClockSetup(cmuClock_EM4GRPACLK);

  CMU_ClockEnable(cmuClock_BURTC, true);

  /* Initialize BURTC */
  burtcInit.start = false;          /* Do not start BURTC on initialization. */
  burtcInit.compare0Top = true;     /* Wrap around on match. */
  burtcInit.clkDiv = burtcClkDiv_1; /* set prescaler to 1 */
  BURTC_Init(&burtcInit);

  uint32_t freq = CMU_ClockFreqGet(cmuClock_EM4GRPACLK);
  BURTC_CompareSet(0, (freq / frequency) - 1);

#ifndef INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY
  BURTC_IntClear(_BURTC_IF_MASK);
  BURTC_IntEnable(BURTC_IF_COMPIF);

  NVIC_ClearPendingIRQ(BURTC_IRQn);
  NVIC_EnableIRQ(BURTC_IRQn);
#endif

  /* Reset Counter and start */
  BURTC_CounterReset();
  BURTC_Start();
}
#endif /* PAL_CLOCK_BURTC */
#endif /* INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE */

/** @endcond */
