/***************************************************************************//**
 * @file
 * @brief SLEEPTIMER Hardware abstraction implementation for BURTC.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_sleeptimer.h"
#include "sli_sleeptimer_hal.h"
#include "em_burtc.h"
#include "em_core.h"
#include "em_cmu.h"

#if SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_BURTC

#if defined(_SILICON_LABS_32B_SERIES_0)
#error BURTC implementation of the sleeptimer not available on Series 0 chips
#endif

// Minimum difference between current count value and what the comparator of the timer can be set to.
// 1 tick is added to the minimum diff for the algorithm of compensation for the IRQ handler that
// triggers when CNT == compare_value + 1. For more details refer to sleeptimer_hal_set_compare() function's header.
#define SLEEPTIMER_COMPARE_MIN_DIFF  (4 + 1)

#define SLEEPTIMER_TMR_WIDTH (_BURTC_CNT_MASK)

static uint32_t get_time_diff(uint32_t a, uint32_t b);

/******************************************************************************
 * Convert HAL interrupt flag BURTC-interrupt-enable bitmask
 *****************************************************************************/
static uint32_t irqien_hal2burtc(uint8_t hal_flag)
{
  uint32_t burtc_if = 0u;

  if (hal_flag & SLEEPTIMER_EVENT_OF) {
    burtc_if |= BURTC_IEN_OF;
  }

  if (hal_flag & SLEEPTIMER_EVENT_COMP) {
    burtc_if |= BURTC_IEN_COMP;
  }

  return burtc_if;
}

/******************************************************************************
 * Convert BURTC interrupt flags to HAL events
 *****************************************************************************/
static uint8_t irqflags_burtc2hal(uint32_t burtc_flag)
{
  uint8_t hal_if = 0u;

  if (burtc_flag & BURTC_IF_OF) {
    hal_if |= SLEEPTIMER_EVENT_OF;
  }

  if (burtc_flag & BURTC_IF_COMP) {
    hal_if |= SLEEPTIMER_EVENT_COMP;
  }

  return hal_if;
}

/******************************************************************************
 * Initializes BURTC sleep timer.
 *****************************************************************************/
void sleeptimer_hal_init_timer()
{
  BURTC_Init_TypeDef burtc_init = BURTC_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_BURTC, true);

  burtc_init.start  = false;
  burtc_init.clkDiv = SL_SLEEPTIMER_FREQ_DIVIDER;
#if (SL_SLEEPTIMER_DEBUGRUN == 1)
  burtc_init.debugRun = true;
#endif

  BURTC_Init(&burtc_init);
  BURTC_IntDisable(_BURTC_IEN_MASK);
  BURTC_IntClear(_BURTC_IF_MASK);
  BURTC_CounterReset();

  BURTC_Start();
  BURTC_SyncWait();

  // Setup BURTC interrupt
  NVIC_ClearPendingIRQ(BURTC_IRQn);
  NVIC_EnableIRQ(BURTC_IRQn);
}

/******************************************************************************
 * Gets BURTC counter.
 *****************************************************************************/
uint32_t sleeptimer_hal_get_counter(void)
{
  return BURTC_CounterGet();
}

/******************************************************************************
 * Gets BURTC compare value
 *****************************************************************************/
uint32_t sleeptimer_hal_get_compare(void)
{
  return BURTC_CompareGet(0U);
}

/******************************************************************************
 * Sets BURTC compare value
 *
 * @note Compare match value is set to the requested value - 1. This is done
 * to compensate for the fact that the BURTC compare match interrupt always
 * triggers at the end of the requested ticks and that the IRQ handler is
 * executed when current tick count == compare_value + 1.
 *****************************************************************************/
void sleeptimer_hal_set_compare(uint32_t value)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t counter;
  uint32_t compare_current;
  uint32_t compare_new = value;

  CORE_ENTER_CRITICAL();
  counter = sleeptimer_hal_get_counter();
  compare_current = sleeptimer_hal_get_compare();

  if (((BURTC_IntGet() & _BURTC_IF_COMP_MASK) != 0)
      || get_time_diff(compare_current, counter) > SLEEPTIMER_COMPARE_MIN_DIFF
      || compare_current == counter) {
    // Add margin if necessary
    if (get_time_diff(compare_new, counter) < SLEEPTIMER_COMPARE_MIN_DIFF) {
      compare_new = counter + SLEEPTIMER_COMPARE_MIN_DIFF;
    }

    // wrap around if necessary
    compare_new %= SLEEPTIMER_TMR_WIDTH;

    BURTC_CompareSet(0U, compare_new - 1);
    sleeptimer_hal_enable_int(SLEEPTIMER_EVENT_COMP);
  }
  CORE_EXIT_CRITICAL();
}

/******************************************************************************
 * Enables BURTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_enable_int(uint8_t local_flag)
{
  BURTC_IntEnable(irqien_hal2burtc(local_flag));
}

/******************************************************************************
 * Disables BURTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_disable_int(uint8_t local_flag)
{
  BURTC_IntDisable(irqien_hal2burtc(local_flag));
}

/*******************************************************************************
 * Hardware Abstraction Layer to set timer interrupts.
 ******************************************************************************/
void sleeptimer_hal_set_int(uint8_t local_flag)
{
  BURTC_IntSet(irqien_hal2burtc(local_flag));
}

/******************************************************************************
 * Gets status of specified interrupt.
 *
 * Note: This function must be called with interrupts disabled.
 *****************************************************************************/
bool sli_sleeptimer_hal_is_int_status_set(uint8_t local_flag)
{
  bool int_is_set = false;
  uint32_t irq_flag = BURTC_IntGet();

  switch (local_flag) {
    case SLEEPTIMER_EVENT_COMP:
      int_is_set = (irq_flag & BURTC_IF_COMP);
      break;

    case SLEEPTIMER_EVENT_OF:
      int_is_set = (irq_flag & BURTC_IF_OF);
      break;

    default:
      break;
  }

  return int_is_set;
}

/*******************************************************************************
 * Gets BURTC timer frequency.
 ******************************************************************************/
uint32_t sleeptimer_hal_get_timer_frequency(void)
{
  return (CMU_ClockFreqGet(cmuClock_BURTC) >> (CMU_PrescToLog2(SL_SLEEPTIMER_FREQ_DIVIDER - 1)));
}

/*******************************************************************************
 * BURTC interrupt handler.
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
  CORE_DECLARE_IRQ_STATE;
  uint8_t local_flag = 0;
  uint32_t irq_flag;

  CORE_ENTER_ATOMIC();
  irq_flag = BURTC_IntGet();
  local_flag = irqflags_burtc2hal(irq_flag);

  BURTC_IntClear(irq_flag & (BURTC_IF_OF | BURTC_IF_COMP));

  process_timer_irq(local_flag);

  CORE_EXIT_ATOMIC();
}

/*******************************************************************************
 * Computes difference between two times taking into account timer wrap-around.
 *
 * @param a Time.
 * @param b Time to substract from a.
 *
 * @return Time difference.
 ******************************************************************************/
static uint32_t get_time_diff(uint32_t a, uint32_t b)
{
  return (a - b);
}

#endif
