/***************************************************************************//**
 * @file
 * @brief SLEEPTIMER hardware abstraction implementation for PRORTC.
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

#include "sl_sleeptimer.h"
#include "sli_sleeptimer_hal.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_bus.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include  <sl_component_catalog.h>
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
#include "sli_power_manager.h"
#endif

#if SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_PRORTC

// Minimum difference between current count value and what the comparator of the timer can be set to.
// 1 tick is added to the minimum diff for the algorithm of compensation for the IRQ handler that
// triggers when CNT == compare_value + 1. For more details refer to sleeptimer_hal_set_compare() function's header.
#define SLEEPTIMER_COMPARE_MIN_DIFF  (2 + 1)
#define PRORTC_CNT_MASK              0xFFFFFFFFUL

#define SLEEPTIMER_TMR_WIDTH (PRORTC_CNT_MASK)

#define TIMER_COMP_REQ                   0U

#define _PRORTC_IF_COMP_SHIFT              1                             /**< Shift value for RTC_COMP */
#define PRORTC_IF_OF                      (0x1UL << 0)                   /**< Overflow Interrupt Flag */

#if (TIMER_COMP_REQ == 0)
#define PRORTC_IF_COMP_BIT RTCC_IF_CC0
#elif  (TIMER_COMP_REQ == 1)
#define PRORTC_IF_COMP_BIT RTCC_IF_CC1
#endif

#ifndef SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER
#define SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER 0
#endif

#if SL_SLEEPTIMER_FREQ_DIVIDER != 1
#warning A value other than 1 for SL_SLEEPTIMER_FREQ_DIVIDER is not supported on Radio Internal RTC (PRORTC)
#endif

static uint32_t get_time_diff(uint32_t a, uint32_t b);

static bool cc_disabled = true;

/******************************************************************************
 * Initializes PRORTC sleep timer.
 *****************************************************************************/
void sleeptimer_hal_init_timer(void)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  uint32_t cmu_status = CMU->STATUS;
  uint32_t lfr_clk_sel = CMU_LFRCLKSEL_LFR_DEFAULT;

  if ((cmu_status & _CMU_STATUS_LFXORDY_MASK) == CMU_STATUS_LFXORDY) {
    lfr_clk_sel = CMU_LFRCLKSEL_LFR_LFXO;
#ifdef PLFRCO_PRESENT
  } else if ((cmu_status & _CMU_STATUS_PLFRCORDY_MASK) == CMU_STATUS_PLFRCORDY) {
    lfr_clk_sel = CMU_LFRCLKSEL_LFR_PLFRCO;
#endif
  } else {
    lfr_clk_sel = CMU_LFRCLKSEL_LFR_LFRCO;
  }

  // Set the Low Frequency R Clock Select Register
  CMU->LFRCLKSEL = (CMU->LFRCLKSEL & ~_CMU_LFRCLKSEL_LFR_MASK)
                   | (lfr_clk_sel << _CMU_LFRCLKSEL_LFR_SHIFT);

  // Enable the PRORTC module
  CMU->LFRCLKEN0 |= 1 << _CMU_LFRCLKEN0_PRORTC_SHIFT;

  PRORTC->IFC = _PRORTC_IF_MASK;

#if (SL_SLEEPTIMER_DEBUGRUN == 1)
  PRORTC->CTRL = _PRORTC_CTRL_EN_MASK | _PRORTC_CTRL_DEBUGRUN_MASK;
#else
  PRORTC->CTRL = _PRORTC_CTRL_EN_MASK;
#endif
#else
  bool use_clk_lfxo  = true;

  // Must enable radio clock branch to clock the radio bus as PRORTC is on this bus.
  CMU->RADIOCLKCTRL = CMU_RADIOCLKCTRL_EN;

#if defined(CMU_CLKEN1_PRORTC)
  CMU->CLKEN1_SET = CMU_CLKEN1_PRORTC;
#endif

#if _SILICON_LABS_32B_SERIES_2_CONFIG > 1
  uint32_t enabled_clocks = CMU->CLKEN0;
  use_clk_lfxo = ((enabled_clocks & _CMU_CLKEN0_LFXO_MASK) == _CMU_CLKEN0_LFXO_MASK);
#endif //_SILICON_LABS_32B_SERIES_2_CONFIG == 2

  // On demand clocking for series 2 chips, make sure that the clock is
  // not force disabled i.e. FORCEEN = 0, DISONDEMAND = 1
  // Cannot rely on ENS bit of STATUS register since that is only set
  // when a module uses the LF clock otherwise the ENS bit will be set to 0.
  use_clk_lfxo = use_clk_lfxo
                 && ((LFXO->CTRL
                      & (_LFXO_CTRL_FORCEEN_MASK | _LFXO_CTRL_DISONDEMAND_MASK))
                     != _LFXO_CTRL_DISONDEMAND_MASK);

  if (use_clk_lfxo) {
    CMU->PRORTCCLKCTRL = CMU_PRORTCCLKCTRL_CLKSEL_LFXO;
  } else {
    CMU->PRORTCCLKCTRL = CMU_PRORTCCLKCTRL_CLKSEL_LFRCO;
  }

#if (SL_SLEEPTIMER_DEBUGRUN == 1)
  PRORTC->CFG |= _RTCC_CFG_DEBUGRUN_MASK;
#endif

  PRORTC->EN_SET = RTCC_EN_EN;

  PRORTC->CC[TIMER_COMP_REQ].CTRL = (_RTCC_CC_CTRL_MODE_OFF << _RTCC_CC_CTRL_MODE_SHIFT)
                                    | (_RTCC_CC_CTRL_CMOA_PULSE << _RTCC_CC_CTRL_CMOA_SHIFT)
                                    | (_RTCC_CC_CTRL_ICEDGE_NONE << _RTCC_CC_CTRL_ICEDGE_SHIFT)
                                    | (_RTCC_CC_CTRL_COMPBASE_CNT << _RTCC_CC_CTRL_COMPBASE_SHIFT);

  // Write the start bit until it syncs to the low frequency domain
  do {
    PRORTC->CMD = RTCC_CMD_START;
    while ((PRORTC->SYNCBUSY & _RTCC_SYNCBUSY_MASK) != 0U) ;
  } while ((PRORTC->STATUS & _RTCC_STATUS_RUNNING_MASK) != RTCC_STATUS_RUNNING);

  // Disable ALL PRORTC interrupts
  PRORTC->IEN &= ~_RTCC_IEN_MASK;

  // Clear any pending interrupts
#if defined (RTCC_HAS_SET_CLEAR)
  PRORTC->IF_CLR = _RTCC_IF_MASK;
#else
  PRORTC->IFC = _RTCC_IF_MASK;
#endif
#endif

  NVIC_ClearPendingIRQ(PRORTC_IRQn);
  NVIC_EnableIRQ(PRORTC_IRQn);
}

/******************************************************************************
 * Gets PRORTC counter value.
 *****************************************************************************/
uint32_t sleeptimer_hal_get_counter(void)
{
  return PRORTC->CNT;
}

/******************************************************************************
 * Gets PRORTC compare value.
 *****************************************************************************/
uint32_t sleeptimer_hal_get_compare(void)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  return PRORTC->COMP[TIMER_COMP_REQ].COMP;
#else
  return PRORTC->CC[TIMER_COMP_REQ].OCVALUE;
#endif
}

/******************************************************************************
 * Sets PRORTC compare value.
 *
 * @note Compare match value is set to the requested value - 1. This is done
 * to compensate for the fact that the PRORTC compare match interrupt always
 * triggers at the end of the requested ticks and that the IRQ handler is
 * executed when current tick count == compare_value + 1.
 *****************************************************************************/
void sleeptimer_hal_set_compare(uint32_t value)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t counter;
  uint32_t compare;
  uint32_t compare_value = value;

  CORE_ENTER_CRITICAL();
  counter = sleeptimer_hal_get_counter();
  compare = sleeptimer_hal_get_compare();

  if (((PRORTC->IF & PRORTC_IF_COMP_BIT) != 0)
      || get_time_diff(compare, counter) > SLEEPTIMER_COMPARE_MIN_DIFF
      || compare == counter) {
    // Add margin if necessary
    if (get_time_diff(compare_value, counter) < SLEEPTIMER_COMPARE_MIN_DIFF) {
      compare_value = counter + SLEEPTIMER_COMPARE_MIN_DIFF;
    }
    compare_value %= SLEEPTIMER_TMR_WIDTH;

#if defined(_SILICON_LABS_32B_SERIES_1)
    PRORTC->COMP[TIMER_COMP_REQ].COMP = compare_value - 1;
#else
    PRORTC->CC[TIMER_COMP_REQ].OCVALUE = compare_value - 1;
#endif

    sleeptimer_hal_enable_int(SLEEPTIMER_EVENT_COMP);
  }
  CORE_EXIT_CRITICAL();

#if defined(_SILICON_LABS_32B_SERIES_2)
  if (cc_disabled) {
    PRORTC->CC[TIMER_COMP_REQ].CTRL |= RTCC_CC_CTRL_MODE_OUTPUTCOMPARE;
    cc_disabled = false;
  }
#endif
}

/******************************************************************************
 * Enables PRORTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_enable_int(uint8_t local_flag)
{
  uint32_t prortc_ien = 0u;

  if (local_flag & SLEEPTIMER_EVENT_OF) {
    prortc_ien |= PRORTC_IF_OF;
  }

  if (local_flag & SLEEPTIMER_EVENT_COMP) {
#if defined(_SILICON_LABS_32B_SERIES_1)
    if (cc_disabled == true) {
#if defined (RTCC_HAS_SET_CLEAR)
      PRORTC->IF_CLR = PRORTC_IF_COMP_BIT;
#else
      PRORTC->IFC = PRORTC_IF_COMP_BIT;
#endif

      cc_disabled = false;
    }
#endif

    prortc_ien |= PRORTC_IF_COMP_BIT;
  }

  BUS_RegMaskedSet(&PRORTC->IEN, prortc_ien);
}

/******************************************************************************
 * Disables PRORTC interrupts.
 *****************************************************************************/
void sleeptimer_hal_disable_int(uint8_t local_flag)
{
  uint32_t prortc_int_dis = 0u;

  if (local_flag & SLEEPTIMER_EVENT_OF) {
    prortc_int_dis |= PRORTC_IF_OF;
  }

  if (local_flag & SLEEPTIMER_EVENT_COMP) {
    prortc_int_dis |= PRORTC_IF_COMP_BIT;
    cc_disabled = true;

#if defined(_SILICON_LABS_32B_SERIES_2)
    PRORTC->CC[TIMER_COMP_REQ].CTRL &= ~_RTCC_CC_CTRL_MODE_MASK;
#endif
  }

  BUS_RegMaskedClear(&PRORTC->IEN, prortc_int_dis);
}

/*******************************************************************************
 * Hardware Abstraction Layer to set timer interrupts.
 ******************************************************************************/
void sleeptimer_hal_set_int(uint8_t local_flag)
{
  if (local_flag & SLEEPTIMER_EVENT_COMP) {
#if defined (RTCC_HAS_SET_CLEAR)
    PRORTC->IF_SET = PRORTC_IF_COMP_BIT;
#else
    PRORTC->IFS = PRORTC_IF_COMP_BIT;
#endif
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
  uint32_t irq_flag = PRORTC->IF;;

  switch (local_flag) {
    case SLEEPTIMER_EVENT_COMP:
      int_is_set = ((irq_flag & PRORTC_IF_COMP_BIT) == PRORTC_IF_COMP_BIT);
      break;

    case SLEEPTIMER_EVENT_OF:
      int_is_set = ((irq_flag & PRORTC_IF_OF) == PRORTC_IF_OF);
      break;

    default:
      break;
  }

  return int_is_set;
}

/*******************************************************************************
 * PRORTC interrupt handler.
 ******************************************************************************/
#if (SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER == 1)
void PRORTC_IRQHandler(void)
#else
void PRORTC_IRQHandlerOverride(void)
#endif
{
  CORE_DECLARE_IRQ_STATE;
  uint8_t local_flag = 0;
  uint32_t irq_flag;

  CORE_ENTER_ATOMIC();
  irq_flag = PRORTC->IF;

  if (irq_flag & PRORTC_IF_OF) {
    local_flag |= SLEEPTIMER_EVENT_OF;
  }
  if (irq_flag & PRORTC_IF_COMP_BIT) {
    local_flag |= SLEEPTIMER_EVENT_COMP;
  }

  /* Clear interrupt source. */
#if defined (RTCC_HAS_SET_CLEAR)
  PRORTC->IF_CLR = irq_flag;
#else
  PRORTC->IFC = irq_flag;
#endif

  process_timer_irq(local_flag);
  CORE_EXIT_ATOMIC();
}

/*******************************************************************************
 * Gets PRORTC timer frequency.
 ******************************************************************************/
uint32_t sleeptimer_hal_get_timer_frequency(void)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  uint32_t lfr_clk_sel;
#endif
  uint32_t freq;

#if defined(_SILICON_LABS_32B_SERIES_1)
  lfr_clk_sel = (CMU->LFRCLKSEL & _CMU_LFRCLKSEL_LFR_MASK) << _CMU_LFRCLKSEL_LFR_SHIFT;

  switch (lfr_clk_sel) {
    case _CMU_LFRCLKSEL_LFR_LFXO:
      freq = SystemLFXOClockGet();
      break;

#if defined(PLFRCO_PRESENT)
    case _CMU_LFRCLKSEL_LFR_PLFRCO:
      freq = SystemLFRCOClockGet();
      break;
#endif

    case _CMU_LFRCLKSEL_LFR_LFRCO:
    default:
      freq = SystemLFRCOClockGet();
      break;
  }

  freq >>= (CMU->LFRPRESC0 & _CMU_LFRPRESC0_PRORTC_MASK)
           >> _CMU_LFRPRESC0_PRORTC_SHIFT;
#elif defined(_SILICON_LABS_32B_SERIES_2)
  if (CMU->PRORTCCLKCTRL == CMU_PRORTCCLKCTRL_CLKSEL_LFXO) {
    freq = SystemLFXOClockGet();
  } else {
    freq = SystemLFRCOClockGet();
  }
#endif

  return freq;
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
