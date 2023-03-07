/***************************************************************************//**
 * @file
 * @brief Hardware driver layer for simple delay on EXX32.
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

#include "btl_driver_delay.h"
#include "btl_driver_util.h"
#include "em_timer.h"

// Cycles used for one loop in sli_delay_loop().
// The Cortex-M33 has a faster execution of the hw loop
// with the same arm instructions.
#if defined(__CORTEX_M) && (__CORTEX_M == 33U)
  #define HW_LOOP_CYCLE  3
#else
  #define HW_LOOP_CYCLE  4
#endif

void sli_delay_loop(uint32_t n);

static uint16_t delayTarget = 0;
static bool expectOverflow;
static uint32_t ticksPerMillisecond = 0;

void delay_microseconds(uint32_t usecs)
{
  uint32_t cyclesPerMicrosecond = util_getClockFreq() / 1000000UL;
  uint32_t loops = (cyclesPerMicrosecond * usecs) / HW_LOOP_CYCLE;
  if (loops > 0U) {
    sli_delay_loop(loops);
  }
}

void delay_init(void)
{
  // Enable clocks to TIMER0
#if defined(CMU_CTRL_HFPERCLKEN)
  CMU->CTRL |= CMU_CTRL_HFPERCLKEN;
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;
#endif
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_SET = CMU_CLKEN0_TIMER0;
#endif

  // Calculate the length of a tick
  ticksPerMillisecond = (util_getClockFreq() / 1000UL) / 1024UL;

  // Initialize timer
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.prescale = timerPrescale1024;
  TIMER_Init(TIMER0, &init);
}

void delay_milliseconds(uint32_t msecs, bool blocking)
{
  uint16_t count = TIMER0->CNT;
  delayTarget = count + (msecs * ticksPerMillisecond);
  expectOverflow = (delayTarget < count);

  if (blocking) {
    while (TIMER0->CNT != delayTarget) {
      // Do nothing
    }
  }
#if defined(TIMER_IFC_OF)
  TIMER0->IFC = TIMER_IFC_OF;
#else
  TIMER0->IF_CLR = TIMER_IF_OF;
#endif
  TIMER0->IEN = TIMER_IEN_OF;
}

bool delay_expired(void)
{
  bool overflow = (TIMER0->IF & TIMER_IF_OF);

  // Expecting overflow, but it hasn't happened yet
  if (expectOverflow && !overflow) {
    return false;
  }
  // Not expecting overflow, but it still happened
  if (!expectOverflow && overflow) {
    return true;
  }

  // Return true if CNT has passed the target
  return TIMER0->CNT >= delayTarget;
}
