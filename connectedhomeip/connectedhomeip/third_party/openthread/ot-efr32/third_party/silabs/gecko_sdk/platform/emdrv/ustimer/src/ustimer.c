/***************************************************************************//**
 * @file
 * @brief Microsecond delay functions.
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
#include "sl_common.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_core.h"
#include "em_timer.h"

#include "ustimer.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define USTIMER_TIMER0 0
#define USTIMER_TIMER1 1
#define USTIMER_TIMER2 2
#define USTIMER_TIMER3 3
#define USTIMER_TIMER4 4
#define USTIMER_TIMER5 5
#define USTIMER_TIMER6 6

#define USTIMER_WTIMER0 10
#define USTIMER_WTIMER1 11
#define USTIMER_WTIMER2 12
#define USTIMER_WTIMER3 13

#ifndef USTIMER_TIMER
#define USTIMER_TIMER USTIMER_TIMER0
#endif

#if (USTIMER_TIMER == USTIMER_TIMER0) && (TIMER_COUNT >= 1)
  #define TIMER_DEV         TIMER0
  #define TIMER_CLK         cmuClock_TIMER0
  #define TIMER_IRQ         TIMER0_IRQn
  #define TIMER_IRQHandler  TIMER0_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER1) && (TIMER_COUNT >= 2)
  #define TIMER_DEV         TIMER1
  #define TIMER_CLK         cmuClock_TIMER1
  #define TIMER_IRQ         TIMER1_IRQn
  #define TIMER_IRQHandler  TIMER1_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER2) && (TIMER_COUNT >= 3)
  #define TIMER_DEV         TIMER2
  #define TIMER_CLK         cmuClock_TIMER2
  #define TIMER_IRQ         TIMER2_IRQn
  #define TIMER_IRQHandler  TIMER2_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER3) && (TIMER_COUNT >= 4)
  #define TIMER_DEV         TIMER3
  #define TIMER_CLK         cmuClock_TIMER3
  #define TIMER_IRQ         TIMER3_IRQn
  #define TIMER_IRQHandler  TIMER3_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER4) && (TIMER_COUNT >= 5)
  #define TIMER_DEV         TIMER4
  #define TIMER_CLK         cmuClock_TIMER4
  #define TIMER_IRQ         TIMER4_IRQn
  #define TIMER_IRQHandler  TIMER4_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER5) && (TIMER_COUNT >= 6)
  #define TIMER_DEV         TIMER5
  #define TIMER_CLK         cmuClock_TIMER5
  #define TIMER_IRQ         TIMER5_IRQn
  #define TIMER_IRQHandler  TIMER5_IRQHandler

#elif (USTIMER_TIMER == USTIMER_TIMER6) && (TIMER_COUNT >= 7)
  #define TIMER_DEV         TIMER6
  #define TIMER_CLK         cmuClock_TIMER6
  #define TIMER_IRQ         TIMER6_IRQn
  #define TIMER_IRQHandler  TIMER6_IRQHandler

#elif (USTIMER_TIMER == USTIMER_WTIMER0) && (WTIMER_COUNT >= 1)
  #define TIMER_DEV         WTIMER0
  #define TIMER_CLK         cmuClock_WTIMER0
  #define TIMER_IRQ         WTIMER0_IRQn
  #define TIMER_IRQHandler  WTIMER0_IRQHandler

#elif (USTIMER_TIMER == USTIMER_WTIMER1) && (WTIMER_COUNT >= 2)
  #define TIMER_DEV         WTIMER1
  #define TIMER_CLK         cmuClock_WTIMER1
  #define TIMER_IRQ         WTIMER1_IRQn
  #define TIMER_IRQHandler  WTIMER1_IRQHandler

#elif (USTIMER_TIMER == USTIMER_WTIMER2) && (WTIMER_COUNT >= 3)
  #define TIMER_DEV         WTIMER2
  #define TIMER_CLK         cmuClock_WTIMER2
  #define TIMER_IRQ         WTIMER2_IRQn
  #define TIMER_IRQHandler  WTIMER2_IRQHandler

#elif (USTIMER_TIMER == USTIMER_WTIMER3) && (WTIMER_COUNT >= 4)
  #define TIMER_DEV         WTIMER3
  #define TIMER_CLK         cmuClock_WTIMER3
  #define TIMER_IRQ         WTIMER3_IRQn
  #define TIMER_IRQHandler  WTIMER3_IRQHandler

#else
#error "Illegal USTIMER TIMER selection"
#endif

// Use 16-bit TOP value for timer, independent of the width of the timer
#define TIMER_MAX   0xFFFF

static uint32_t freq;
static uint32_t minTicks;
static volatile bool timeElapsed = false;

static void DelayTicksEM1(uint16_t ticks);
static void DelayTicksPolled(uint16_t ticks);

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Activate and initialize the hardware timer used to pace the 1 microsecond
 *   delay functions.
 *
 * @note
 *   Call this function whenever the HFPERCLK and/or HFCORECLK frequency is
 *   changed.
 *
 * @return
 *    @ref ECODE_EMDRV_USTIMER_OK.
 ******************************************************************************/
Ecode_t USTIMER_Init(void)
{
  TIMER_Init_TypeDef timerInit     = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  uint32_t coreClockScale;

  timerCCInit.mode = timerCCModeCompare;
  CMU_ClockEnable(TIMER_CLK, true);
  TIMER_InitCC(TIMER_DEV, 0, &timerCCInit);
  TIMER_TopSet(TIMER_DEV, TIMER_MAX);

  /* Run timer at slowest frequency that still gives less than 1 us per tick */
#if defined (_TIMER_CTRL_PRESC_DIV1)
  timerInit.prescale = (TIMER_Prescale_TypeDef)_TIMER_CTRL_PRESC_DIV1;
#endif

#if defined (_TIMER_CFG_PRESC_DIV1)
  timerInit.prescale = (TIMER_Prescale_TypeDef)_TIMER_CFG_PRESC_DIV1;
#endif

  do {
    TIMER_Init(TIMER_DEV, &timerInit);

#if defined(_CMU_HFPERCLKEN0_MASK)
    freq = CMU_ClockFreqGet(cmuClock_HFPER);
#else
    freq = CMU_ClockFreqGet(cmuClock_EM01GRPACLK);
#endif

#if defined (_TIMER_CFG_PRESC_DIV1)
    freq /= (timerInit.prescale + 1);
#endif
#if defined (_TIMER_CTRL_PRESC_DIV1)
    freq /= 1 << timerInit.prescale;
#endif
    timerInit.prescale++;
  } while ( (timerInit.prescale <= timerPrescale1024)
            && (freq > 2000000) );

  /* Figure out the minimum delay we can have when using timer interrupt
   * to avoid that actual delay become a full timer counter lap.
   * We are assuming that this number scales with mcu core clock.
   * The number is found by trial and err on a GG running at 14MHz.
   */
  coreClockScale = (4 * 48000000) / CMU_ClockFreqGet(cmuClock_CORE);
  minTicks = ( ( (uint64_t)freq * coreClockScale) + 500000) / 1000000;
  timeElapsed = false;

  TIMER_IntDisable(TIMER_DEV, TIMER_IEN_CC0);
  NVIC_ClearPendingIRQ(TIMER_IRQ);
  NVIC_EnableIRQ(TIMER_IRQ);

  return ECODE_EMDRV_USTIMER_OK;
}

/***************************************************************************//**
 * @brief
 *    Deinitialize USTIMER driver.
 *
 * @details
 *    Will disable interrupts and turn off the clock to the underlying hardware
 *    timer.
 *
 * @return
 *    @ref ECODE_EMDRV_USTIMER_OK.
 ******************************************************************************/
Ecode_t USTIMER_DeInit(void)
{
  NVIC_DisableIRQ(TIMER_IRQ);
  TIMER_IntDisable(TIMER_DEV, TIMER_IEN_CC0);

  TIMER_IntClear(TIMER_DEV, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER_IRQ);

  TIMER_Enable(TIMER_DEV, false);

  CMU_ClockEnable(TIMER_CLK, false);

  return ECODE_EMDRV_USTIMER_OK;
}

/***************************************************************************//**
 * @brief
 *   Delay a given number of microseconds.
 *
 * @details
 *   The mcu is put in EM1 during the delay.
 *
 * @note
 *   This function assumes that the timer interrupt needed to wake the mcu
 *   up from EM1 is not blocked. This function is not thread safe.
 *   Due to the overhead of setting up and serving TIMER/WTIMER interrupts,
 *   timing will be inaccurate for delays shorter than 200us. If higher accuracy
 *   is needed for short delays, consider using USTIMER_DelayIntSafe() or
 *   UDELAY_Delay().
 *
 * @param[in] usec
 *   Number of microseconds to delay.
 *
 * @return
 *    @ref ECODE_EMDRV_USTIMER_OK.
 ******************************************************************************/
Ecode_t USTIMER_Delay(uint32_t usec)
{
  uint64_t totalTicks;

  totalTicks = ( ( (uint64_t)freq * usec) + 500000) / 1000000;

  /* The timer counter is 16 bits wide, split the total wait up in chunks */
  /* of a little less than 2^16.                                          */
  while ( totalTicks > 65000 ) {
    DelayTicksEM1(65000);
    totalTicks -= 65000;
  }
  DelayTicksEM1( (uint16_t)totalTicks);

  return ECODE_EMDRV_USTIMER_OK;
}

/***************************************************************************//**
 * @brief
 *   Delay a given number of microseconds.
 *
 * @details
 *   This is a busy wait delay not using energy modes to conserve power.
 *
 * @note
 *   This function can be used in any context (interrupts can be disabled).
 *   This function is thread safe.
 *
 * @param[in] usec
 *   Number of microseconds to delay.
 *
 * @return
 *    @ref ECODE_EMDRV_USTIMER_OK.
 ******************************************************************************/
Ecode_t USTIMER_DelayIntSafe(uint32_t usec)
{
  uint64_t totalTicks;

  totalTicks = ( ( (uint64_t)freq * usec) + 500000) / 1000000;

  /* The timer counter is 16 bits wide, split the total wait up in chunks */
  /* of a little less than 2^16.                                          */
  while ( totalTicks > 65000 ) {
    DelayTicksPolled(65000);
    totalTicks -= 65000;
  }
  DelayTicksPolled( (uint16_t)totalTicks);

  return ECODE_EMDRV_USTIMER_OK;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

void TIMER_IRQHandler(void)
{
  uint32_t flags;

  flags = TIMER_IntGet(TIMER_DEV);

  if ( flags & TIMER_IF_CC0 ) {
    TIMER_IntClear(TIMER_DEV, TIMER_IF_CC0);
    timeElapsed = true;
  }
}

static void DelayTicksPolled(uint16_t ticks)
{
  uint16_t startTime;
  volatile uint16_t now;

  if ( ticks ) {
    startTime = TIMER_CounterGet(TIMER_DEV);
    do {
      now = TIMER_CounterGet(TIMER_DEV);
    } while ( (uint16_t)(now - startTime) < ticks );
  }
}

static void DelayTicksEM1(uint16_t ticks)
{
  uint32_t cmp;

  if ( ticks ) {
    /* Arm TIMER compare interrupt */

    CORE_ATOMIC_SECTION(
      /* The following lines costs 2.7us@48MHz and 7.5us@14MHz (measured with GG)*/
      cmp = (TIMER_CounterGet(TIMER_DEV) + SL_MAX(minTicks, ticks)) & TIMER_MAX;
      TIMER_CompareSet(TIMER_DEV, 0, cmp);
      TIMER_IntClear(TIMER_DEV, TIMER_IF_CC0);
      TIMER_IntEnable(TIMER_DEV, TIMER_IEN_CC0);
      )

    while ( !timeElapsed ) {
      EMU_EnterEM1();
    }
    timeElapsed = false;

    TIMER_IntDisable(TIMER_DEV, TIMER_IEN_CC0);
  }
}

/** @endcond */

/* *INDENT-OFF* */
// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup ustimer USTIMER - Microsecond Timer
/// @brief Microsecond Delay Timer Driver
/// @{
///
///@details
///  Implements microsecond delays.
///
///  The delay is implemented using a hardware timer. @ref USTIMER_Init() must
///  be called prior to using the delay functions. @ref USTIMER_Init() must
///  also be called if HFCORECLK and/or HFPERCLK is changed.
///
///  The source files for the USTIMER driver library resides in the
///  emdrv/ustimer folder, and are named ustimer.c and ustimer.h.
///
///  @li @ref ustimer_intro
///  @li @ref ustimer_conf
///  @li @ref ustimer_api
///  @li @ref ustimer_example
///
///@n @section ustimer_intro Introduction
///
///  The USTIMER driver implements microsecond delay functions. The delay is timed
///  using a hardware TIMER or WTIMER resource. Two delay functions are available,
///  one which uses energy mode EM1 to preserve energy while waiting, and one which
///  performs busy wait.
///
///  @note Due to the overhead of setting up and serving TIMER/WTIMER interrupts,
///  the USTIMER_Delay() function will be inaccurate for delays shorter than 200us.
///  If higher accuracy is needed for short delays, consider using
///  USTIMER_DelayIntSafe() or UDELAY_Delay().
///
///@n @section ustimer_conf Configuration Options
///
///  By default the module use TIMER0. Timer resource selection is stored in a
///  file named ustimer_config.h. A template for this file, containing default
///  value, resides in the emdrv/config folder.
///
///  To configure USTIMER, provide your own configuration file. Here is a
///  sample ustimer_config.h file:
///  @code{.c}
///#ifndef __SILICON_LABS_USTIMER_CONFIG_H__
///#define __SILICON_LABS_USTIMER_CONFIG_H__
///
////// USTIMER configuration option. Use this define to select a TIMER resource.
///#define USTIMER_TIMER USTIMER_TIMER3
///
///#endif
///  @endcode
///
///@n @section ustimer_api The API
///
///  This section contain brief descriptions of the functions in the API. You will
///  find detailed information on input and output parameters and return values by
///  clicking on the hyperlinked function names. Most functions return an error
///  code, @ref ECODE_EMDRV_USTIMER_OK is returned on success,
///  see \ref ecode for other error codes.
///
///  Your application code must include one header file: @em ustimer.h.
///
///  @ref USTIMER_Init(), @ref USTIMER_DeInit() @n
///    These functions initialize and deinitialize the USTIMER driver. Typically
///    @htmlonly USTIMER_Init() @endhtmlonly is called once in your startup code.
///
///  @ref USTIMER_Delay()
///    Delay a given number of microseconds. The MCU enters EM1 during the delay.
///
///  @ref USTIMER_DelayIntSafe()
///    Delay a given number of microseconds. The MCU stays in EM0 during the delay.
///    This function can be called in any context and is also thread safe.
///
///@n @section ustimer_example Example
///  @code{.c}
///#include "ustimer.h"
///
///int main( void )
///{
///  // Initialization of USTIMER driver
///  USTIMER_Init();
///
///  // Wait for 250 microseconds
///  USTIMER_Delay(250);
///}
///  @endcode
///
/// @} end group ustimer *******************************************************
