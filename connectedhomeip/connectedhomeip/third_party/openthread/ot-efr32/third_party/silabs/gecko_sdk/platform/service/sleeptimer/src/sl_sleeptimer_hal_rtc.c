/***************************************************************************//**
 * @file
 * @brief SLEEPTIMER Hardware abstraction implementation for RTC.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_rtc.h"
#include "sl_sleeptimer.h"
#include "sli_sleeptimer_hal.h"
#include "em_core.h"
#include "em_cmu.h"

#if SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_RTC

// Use RTC compare channel B.
#define SLEEPTIMER_RTC_COMP_SHIFT (2u)
#define SLEEPTIMER_RTC_COMP       (1 << SLEEPTIMER_RTC_COMP_SHIFT)

// Minimum difference between current count value and what the comparator of the timer can be set to.
// Almost always current count + 1. However, the RTC on legacy gecko (EFM32G) requires the comparator to be set to at least current cnt + 4.
// 1 tick is added to the minimum diff for the algorithm of compensation for the IRQ handler that
// triggers when CNT == compare_value + 1. For more details refer to sleeptimer_hal_set_compare() function's header.
#ifdef _EFM32_GECKO_FAMILY
#define SLEEPTIMER_COMPARE_MIN_DIFF  (5 + 1)
#else
#define SLEEPTIMER_COMPARE_MIN_DIFF  (2 + 1)
#endif

#define SLEEPTIMER_TMR_WIDTH (_RTC_CNT_MASK)
#define SLEEPTIMER_TMR_BIT_WIDTH 24u

__STATIC_INLINE uint32_t get_time_diff(uint32_t a,
                                       uint32_t b);

static volatile uint8_t rtc_overflow_count = 0;
static volatile uint32_t compare_value_32 = 0;
static bool comp_int_disabled = true;

/******************************************************************************
 * Initializes RTC sleep timer.
 *****************************************************************************/
void sleeptimer_hal_init_timer()
{
  RTC_Init_TypeDef rtc_init = RTC_INIT_DEFAULT;

  CMU_ClockDivSet(cmuClock_RTC, SL_SLEEPTIMER_FREQ_DIVIDER);
  CMU_ClockEnable(cmuClock_RTC, true);

  // Initialize RTC.
  rtc_init.comp0Top = false;
  rtc_init.enable = true;
#if (SL_SLEEPTIMER_DEBUGRUN == 1)
  rtc_init.debugRun = true;
#endif

  RTC_Init(&rtc_init);
  RTC_IntDisable(_RTC_IEN_MASK);
  RTC_IntClear(_RTC_IFC_MASK);
#if !defined(_EFM32_GECKO_FAMILY)
  RTC_CounterSet(0u);
#endif

  // Setup RTC interrupt
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);
}

/******************************************************************************
 * Gets RTC counter.
 *****************************************************************************/
uint32_t sleeptimer_hal_get_counter(void)
{
  uint32_t tick_cnt;
  uint16_t of_cnt;

  tick_cnt = RTC_CounterGet();
  of_cnt = rtc_overflow_count;

  if (RTC_IntGet() & RTC_IF_OF) {
    tick_cnt = RTC_CounterGet();
    of_cnt++;
  }

  return tick_cnt | ((uint32_t)of_cnt << SLEEPTIMER_TMR_BIT_WIDTH);
}

/******************************************************************************
 * Gets RTC compare value
 *****************************************************************************/
uint32_t sleeptimer_hal_get_compare(void)
{
  return compare_value_32;
}

/******************************************************************************
 * Sets RTC compare value
 *
 * @note Compare match value is set to the requested value - 1. This is done
 * to compensate for the fact that the RTC compare match interrupt always
 * triggers at the end of the requested ticks and that the IRQ handler is
 * executed when current tick count == compare_value + 1.
 *****************************************************************************/
void sleeptimer_hal_set_compare(uint32_t value)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t counter;
  uint32_t compare_value = value;

  CORE_ENTER_CRITICAL();
  counter = sleeptimer_hal_get_counter();
  if (((RTC_IntGet() & SLEEPTIMER_RTC_COMP) != 0)
      || get_time_diff(compare_value_32, counter) > SLEEPTIMER_COMPARE_MIN_DIFF
      || compare_value_32 == counter) {
    // Add margin if necessary
    if (get_time_diff(compare_value, counter) < SLEEPTIMER_COMPARE_MIN_DIFF) {
      compare_value = counter + SLEEPTIMER_COMPARE_MIN_DIFF;
    }

    compare_value_32 = compare_value - 1;
    if (compare_value_32 - counter <= SLEEPTIMER_TMR_WIDTH) {
      RTC_CompareSet(1, compare_value_32 % (SLEEPTIMER_TMR_WIDTH + 1));
      sleeptimer_hal_enable_int(SLEEPTIMER_EVENT_COMP);
    } else {
      sleeptimer_hal_disable_int(SLEEPTIMER_EVENT_COMP);
    }
  }
  CORE_EXIT_CRITICAL();
}

/******************************************************************************
 * Enables RTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_enable_int(uint8_t local_flag)
{
  uint32_t rtc_ien = 0u;

  if (local_flag & SLEEPTIMER_EVENT_OF) {
    rtc_ien |= RTC_IEN_OF;
  }

  if (local_flag & SLEEPTIMER_EVENT_COMP) {
    if (comp_int_disabled == true) {
      RTC_IntClear(SLEEPTIMER_RTC_COMP);
      comp_int_disabled = false;
    }

    rtc_ien |= SLEEPTIMER_RTC_COMP;
  }

  RTC_IntEnable(rtc_ien);
}

/******************************************************************************
 * Disables RTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_disable_int(uint8_t local_flag)
{
  uint32_t rtc_int_clr = 0u;

  if (local_flag & SLEEPTIMER_EVENT_OF) {
    rtc_int_clr |= RTC_IEN_OF;
  }

  if (local_flag & SLEEPTIMER_EVENT_COMP) {
    rtc_int_clr |= SLEEPTIMER_RTC_COMP;
    comp_int_disabled = true;
  }

  RTC_IntDisable(rtc_int_clr);
}

/*******************************************************************************
 * Hardware Abstraction Layer to set timer interrupts.
 ******************************************************************************/
void sleeptimer_hal_set_int(uint8_t local_flag)
{
  if (local_flag & SLEEPTIMER_EVENT_COMP) {
    RTC_IntSet(SLEEPTIMER_RTC_COMP);
  }
}

/******************************************************************************
 * Gets status of specified interrupt.
 *
 * Note: This function must be called with interrupts disabled.
 *****************************************************************************/
bool sli_sleeptimer_hal_is_int_status_set(uint8_t local_flag)
{
  bool int_is_set = false;
  uint32_t irq_flag = RTC_IntGet();

  switch (local_flag) {
    case SLEEPTIMER_EVENT_COMP:
      int_is_set = ((irq_flag & SLEEPTIMER_RTC_COMP) == SLEEPTIMER_RTC_COMP);
      break;

    case SLEEPTIMER_EVENT_OF:
      int_is_set = ((irq_flag & RTC_IF_OF) == RTC_IF_OF)
                   && (((rtc_overflow_count << SLEEPTIMER_TMR_BIT_WIDTH) + SLEEPTIMER_TMR_WIDTH) == UINT32_MAX);
      break;

    default:
      break;
  }

  return int_is_set;
}

/*******************************************************************************
 * Gets RTC timer frequency.
 ******************************************************************************/
uint32_t sleeptimer_hal_get_timer_frequency(void)
{
  return CMU_ClockFreqGet(cmuClock_RTC);
}

/*******************************************************************************
 * RTC interrupt handler.
 ******************************************************************************/
void RTC_IRQHandler(void)
{
  CORE_DECLARE_IRQ_STATE;
  uint8_t local_flag = 0;
  uint32_t irq_flag;
  uint32_t compare_value_24 = 0;

  CORE_ENTER_ATOMIC();
  irq_flag = RTC_IntGet();

  if (irq_flag & RTC_IF_OF) {
    if (((rtc_overflow_count << SLEEPTIMER_TMR_BIT_WIDTH) + SLEEPTIMER_TMR_WIDTH) == UINT32_MAX) {
      local_flag |= SLEEPTIMER_EVENT_OF;
    }
    rtc_overflow_count++;
    compare_value_24 = compare_value_32;
    compare_value_24 -= (rtc_overflow_count << SLEEPTIMER_TMR_BIT_WIDTH);
    if (compare_value_24 <= SLEEPTIMER_TMR_WIDTH) {
      RTC_CompareSet(1, compare_value_24);
      sleeptimer_hal_enable_int(SLEEPTIMER_EVENT_COMP);
    }
  }
  if (irq_flag & SLEEPTIMER_RTC_COMP) {
    local_flag |= SLEEPTIMER_EVENT_COMP;
  }
  RTC_IntClear(irq_flag & (RTC_IFC_OF | SLEEPTIMER_RTC_COMP));

  if (local_flag != 0) {
    process_timer_irq(local_flag);
  }
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
__STATIC_INLINE uint32_t get_time_diff(uint32_t a,
                                       uint32_t b)
{
  return (a - b);
}

#endif
