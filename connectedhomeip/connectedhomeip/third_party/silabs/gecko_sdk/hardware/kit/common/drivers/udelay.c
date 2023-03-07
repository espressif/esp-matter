/***************************************************************************//**
 * @file
 * @brief Microsecond delay routine.
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_core.h"
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
#define UDELAY_CLOCK_SOURCE_BURTC
#include "em_burtc.h"
#elif defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
#include "em_rtcc.h"
#else
#include "em_rtc.h"
#endif

#include "udelay.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/**************************************************************************//**
* @addtogroup Udelay
* @brief Calibrated busy wait loop.
*
* @details
*  The delay is implemented as a loop coded in assembly. The delay loop must
*  be calibrated by calling @ref UDELAY_Calibrate() once. The calibration
*  algorithm is taken from linux 2.4 sources (bogomips).
*
*  The delay is fairly accurate, the assembly coding will not be optimized
*  by the compiler. The delay function should not be used for longer delays
*  than 1000 us. Calling the delay function with > 1000 will give unpredictable
*  results.
*  Recalibrate the loop when HFCORECLK is changed.
*
*  The calibration uses the RTC clocked by LFRCO to measure time. Better
*  accuracy can be achieved by adding \#define UDELAY_LFXO (i.e. add
*  -DUDELAY_LFXO on the commandline). The LFXO oscillator is then used for
*  delay loop calibration.
*
*  The calibration function will restore RTC upon exit.
* @{
******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* this should be approx 2 BogoMips to start (note initial shift), and will
 * still work even if initially too large, it will just take slightly longer
 */
volatile unsigned long loops_per_jiffy = (1 << 12);

/* This is the number of bits of precision for the loops_per_jiffy.  Each
 * bit takes on average 1.5/HZ seconds.  This (like the original) is a little
 * better than 1%
 */
#define LPS_PREC 8

static void calibrate_delay(void);
__STATIC_INLINE uint32_t clock(void);
static void _delay(uint32_t delay);

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Calibrates the microsecond delay loop.
 ******************************************************************************/
void UDELAY_Calibrate(void)
{
#if (_SILICON_LABS_32B_SERIES >= 2)
  CMU_Clock_TypeDef  rtcClkSrc;
  CMU_Select_TypeDef rtcClkSel;
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  bool lfrcoClkTurnoff = false;
#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  bool burtcClkTurnoff = false;
#endif
#if defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
  bool rtccClkTurnoff  = false;
#endif
#endif
#else
  CMU_Select_TypeDef lfaClkSel;
  CMU_ClkDiv_TypeDef rtcClkDiv;
  bool leClkTurnoff     = false;
  bool rtcClkTurnoff    = false;
  bool lfaClkSrcRestore = false;
  bool lfaClkTurnoff    = false;
#endif
  bool rtcRestore       = false;
#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  BURTC_Init_TypeDef init = BURTC_INIT_DEFAULT;
#elif defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
  RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;
#else
  RTC_Init_TypeDef init = RTC_INIT_DEFAULT;
  uint32_t rtcComp0 = 0;
  uint32_t rtcComp1 = 0;
#endif
  uint32_t rtcCtrl = 0;
  uint32_t rtcIen = 0;
  CORE_DECLARE_IRQ_STATE;

#if (_SILICON_LABS_32B_SERIES < 2)
  /* Ensure LE modules are accessible */
#if defined (_CMU_HFBUSCLKEN0_MASK)
  if ( !(CMU->HFBUSCLKEN0 & CMU_HFBUSCLKEN0_LE) )
#else
  if ( !(CMU->HFCORECLKEN0 & CMU_HFCORECLKEN0_LE) )
#endif
  {
    CMU_ClockEnable(cmuClock_CORELE, true);
    leClkTurnoff = true;
  }

#if defined (CMU_LFECLKEN0_RTCC)
  lfaClkSel = CMU_ClockSelectGet(cmuClock_LFE);
#else
  lfaClkSel = CMU_ClockSelectGet(cmuClock_LFA);
#endif

#if defined(UDELAY_LFXO)
  if ( !(CMU->STATUS & CMU_STATUS_LFXOENS) ) {
    lfaClkTurnoff = true;
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  }

  if ( lfaClkSel != cmuSelect_LFXO ) {
    lfaClkSrcRestore = true;
#if defined (CMU_LFECLKEN0_RTCC)
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
#else
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#endif
  }

#else
  if ( lfaClkSel != cmuSelect_LFRCO ) {
    lfaClkSrcRestore = true;
  }
  if ( !(CMU->STATUS & CMU_STATUS_LFRCOENS) ) {
    lfaClkTurnoff = true;
  }
  /* Enable LFACLK in CMU (will also enable oscillator if not enabled) */
#if defined (CMU_LFECLKEN0_RTCC)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#else
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#endif
#endif

#else /* #if (_SILICON_LABS_32B_SERIES < 2) */
  /* _SILICON_LABS_32B_SERIES_2 or later devices */
#if defined(UDELAY_LFXO)
#error "LFXO udelay calibration not yet supported."

#else

#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  rtcClkSrc = cmuClock_EM4GRPACLK;
#else
  rtcClkSrc = cmuClock_RTCC;
#endif

  /* Remember current clock source selection for RTC. */
  rtcClkSel = CMU_ClockSelectGet(rtcClkSrc);

#if defined(PLFRCO_PRESENT)
  CMU_ClockSelectSet(rtcClkSrc, cmuSelect_PLFRCO);
#elif defined(LFRCO_PRESENT)
  CMU_ClockSelectSet(rtcClkSrc, cmuSelect_LFRCO);
#else
#error Neither LFRCO nor PLFRCO is present.
#endif
#endif

#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  if (!(CMU->CLKEN0 & CMU_CLKEN0_BURTC)) {
    burtcClkTurnoff = true;
  }
  CMU_ClockEnable(cmuClock_BURTC, true);
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  if (!(CMU->CLKEN0 & CMU_CLKEN0_RTCC)) {
    rtccClkTurnoff = true;
  }
  CMU_ClockEnable(cmuClock_RTCC, true);
#endif
#endif // #if (_SILICON_LABS_32B_SERIES < 2)

  /* Set up a reasonable prescaler. */
#if !defined(UDELAY_CLOCK_SOURCE_BURTC)
#if defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
#if !defined(RTCC_EN_EN)
  rtcClkDiv = CMU_ClockDivGet(cmuClock_RTCC);
  if ( !(CMU->LFECLKEN0 & CMU_LFECLKEN0_RTCC) ) {
    /* Enable clock to RTCC module */
    CMU_ClockEnable(cmuClock_RTCC, true);
    rtcClkTurnoff = true;
  }
#endif
#else
  rtcClkDiv = CMU_ClockDivGet(cmuClock_RTC);
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_256);
  if ( !(CMU->LFACLKEN0 & CMU_LFACLKEN0_RTC) ) {
    /* Enable clock to RTC module */
    CMU_ClockEnable(cmuClock_RTC, true);
    rtcClkTurnoff = true;
  }
#endif
#endif /* !defined(UDELAY_CLOCK_SOURCE_BURTC) */

  CORE_ENTER_ATOMIC();

#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  if ((BURTC->EN & BURTC_EN_EN) != 0U) {
    BURTC_SyncWait();
    BURTC->CMD    = BURTC_CMD_STOP;
    rtcCtrl       = BURTC->CFG;
    rtcIen        = BURTC->IEN;
    BURTC->IEN    = 0;
    BURTC->IF_CLR = _BURTC_IF_MASK;

    NVIC_ClearPendingIRQ(BURTC_IRQn);
    rtcRestore = true;
  }

  BURTC_Init(&init);

#elif (defined(RTCC_PRESENT) && (RTCC_COUNT == 1))
#if defined(RTCC_EN_EN)
  if ((RTCC->EN & RTCC_EN_EN) != 0U) {
    /* Stash away current RTC settings. */
    RTCC_SyncWait();
    RTCC->EN_CLR = RTCC_EN_EN;
    rtcCtrl      = RTCC->CFG;
    rtcIen       = RTCC->IEN;
    RTCC->CFG    = _RTCC_CFG_RESETVALUE;
    RTCC->IEN    = 0;
    RTCC->IF_CLR = _RTCC_IF_MASK;
#else
  if ( RTCC->CTRL & RTCC_CTRL_ENABLE ) {
    /* Stash away current RTC settings. */
    rtcCtrl      = RTCC->CTRL;
    rtcIen       = RTCC->IEN;
    RTCC->CTRL   = _RTCC_CTRL_RESETVALUE;
    RTCC->IEN    = 0;
    RTCC->IFC    = _RTCC_IEN_MASK;
#endif
    NVIC_ClearPendingIRQ(RTCC_IRQn);
    rtcRestore = true;
  }
  init.precntWrapOnCCV0 = false;  /* Count to max before wrapping */
  init.cntWrapOnCCV1 = false;  /* Count to max before wrapping */
  init.presc = rtccCntPresc_256; /* Setup prescaler */

  RTCC_Init(&init);        /* Start RTC counter. */

#else /* #if defined(RTCC_PRESENT) && (RTCC_COUNT == 1) */
  if ( RTC->CTRL & RTC_CTRL_EN ) {
    /* Stash away current RTC settings. */
    rtcCtrl   = RTC->CTRL;
    rtcComp0  = RTC->COMP0;
    rtcComp1  = RTC->COMP1;
    rtcIen    = RTC->IEN;

    RTC->CTRL = _RTC_CTRL_RESETVALUE;
    RTC->IEN  = 0;
    RTC->IFC  = _RTC_IEN_MASK;

    NVIC_ClearPendingIRQ(RTC_IRQn);

    rtcRestore = true;
  }
  init.comp0Top = false;  /* Count to max before wrapping */

  RTC_Init(&init);        /* Start RTC counter. */

#endif /* #if defined(RTCC_PRESENT) && (RTCC_COUNT == 1) */

  /*
   * This fixup is needed only for Series2 chip or above, and only
   * if the BURTC or the RTCC is used.
   */
#if (_SILICON_LABS_32B_SERIES >= 2) \
  && (defined(UDELAY_CLOCK_SOURCE_BURTC) || (defined(RTCC_PRESENT) && (RTCC_COUNT == 1)))

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  if (!(CMU->CLKEN0 & CMU_CLKEN0_LFRCO)) {
    lfrcoClkTurnoff = true;
  }
  CMU_ClockEnable(cmuClock_LFRCO, true);
#endif
#if defined(LFRCO_PRESENT) || defined(PLFRCO_PRESENT)
  while ((LFRCO->STATUS & (LFRCO_STATUS_ENS | LFRCO_STATUS_RDY))
         != (LFRCO_STATUS_ENS | LFRCO_STATUS_RDY)) {
  }
#else
#error Neither LFRCO nor PLFRCO is present.
#endif
#endif /* Series >= 2 && (BURTC || RTCC) */

  calibrate_delay();      /* Calibrate the micro second delay loop. */

  CORE_EXIT_ATOMIC();

  /* Restore all RTC related settings to how they were previously set. */
  if ( rtcRestore ) {
#if defined(UDELAY_CLOCK_SOURCE_BURTC)
    /*
     * CFG must be set while disabled, other registers when enabled. The safest
     * way to achieve that is through BURTC_Enable which takes care of synchronization
     */
    BURTC_Enable(false);
    BURTC->CFG    = rtcCtrl;
    BURTC_Enable(true);
    BURTC->IEN    = rtcIen;
    BURTC->IF_CLR = _BURTC_IF_MASK;
    BURTC->CMD    = BURTC_CMD_START;
#elif defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
#if defined(RTCC_EN_EN)
    RTCC_SyncWait();
    RTCC->EN_CLR = RTCC_EN_EN;
    RTCC->CFG    = rtcCtrl;
    RTCC->IEN    = rtcIen;
    RTCC->IF_CLR = _RTCC_IF_MASK;
    RTCC->EN_SET = RTCC_EN_EN;
    RTCC->CMD    = RTCC_CMD_START;
#else
    CMU_ClockDivSet(cmuClock_RTCC, rtcClkDiv);
    RTCC->CTRL  = rtcCtrl;
    RTCC->IEN   = rtcIen;
#endif
#else
    CMU_ClockDivSet(cmuClock_RTC, rtcClkDiv);
    RTC_FreezeEnable(true);
    #if defined(_EFM32_GECKO_FAMILY)
    RTC_Sync(RTC_SYNCBUSY_COMP0 | RTC_SYNCBUSY_COMP1 | RTC_SYNCBUSY_CTRL);
    #endif
    RTC->COMP0 = rtcComp0;
    RTC->COMP1 = rtcComp1;
    RTC->CTRL  = rtcCtrl;
    RTC->IEN   = rtcIen;
    RTC_FreezeEnable(false);
#endif
  } else {
#if defined(UDELAY_CLOCK_SOURCE_BURTC)
    BURTC_Enable(false);
#elif defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
    RTCC_Enable(false);
#else
    RTC_Enable(false);
#endif
  }

#if (_SILICON_LABS_32B_SERIES < 2)
  if ( rtcClkTurnoff ) {
#if defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
    CMU_ClockEnable(cmuClock_RTCC, false);
#else
    CMU_ClockEnable(cmuClock_RTC, false);
#endif
  }

  if ( lfaClkSrcRestore ) {
#if defined (CMU_LFECLKEN0_RTCC)
    CMU_ClockSelectSet(cmuClock_LFE, lfaClkSel);
#else
    CMU_ClockSelectSet(cmuClock_LFA, lfaClkSel);
#endif
  }

  if ( lfaClkTurnoff ) {
    #if defined(UDELAY_LFXO)
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
    #else
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
    #endif
  }

  if ( leClkTurnoff ) {
    CMU_ClockEnable(cmuClock_CORELE, false);
  }

#else /* #if (_SILICON_LABS_32B_SERIES < 2) */
  /* Restore original clock source selection. */
  CMU_ClockSelectSet(rtcClkSrc, rtcClkSel);
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  if (lfrcoClkTurnoff == true) {
    CMU_ClockEnable(cmuClock_LFRCO, false);
  }

#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  if (burtcClkTurnoff == true) {
    CMU_ClockEnable(cmuClock_BURTC, false);
  }
#endif

#if defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
  if (rtccClkTurnoff == true) {
    CMU_ClockEnable(cmuClock_RTCC, false);
  }
#endif
#endif /* _SILICON_LABS_32B_SERIES_2_CONFIG_2 || _SILICON_LABS_32B_SERIES_2_CONFIG_3 || _SILICON_LABS_32B_SERIES_2_CONFIG_4 */

#endif /* #if (_SILICON_LABS_32B_SERIES < 2) */
}

#if defined(__GNUC__) /* GCC */
/***************************************************************************//**
 * @brief
 *   Microsecond active wait delay routine.
 *
 * @note
 *   The delay function should not be used for longer delays than 1000 us.
 *   Calling the delay function with > 1000 will give unpredictable results.
 *
 * @param[in] usecs
 *   Number of microseconds to delay.
 ******************************************************************************/
/* *INDENT-OFF* */
void UDELAY_Delay(uint32_t usecs)
{
  __ASM volatile (
#if (__CORTEX_M == 0x00)
    "        .syntax unified           \n"
    "        .arch armv6-m             \n"
#endif
    "        cmp     %0, #0            \n" /* Return if 0 delay. */
    "        beq.n   2f                \n"
    "        subs    %0, #1            \n" /* Correct for off by one error. */
    "        movs    r2, #0x88         \n"
    "        lsls    r2, r2, #8        \n"
    "        adds    r2, #0x00         \n"
    "        muls    %0, r2            \n"
    "                                  \n"
    "        ldr     r2, [%1]          \n"
    "        movs    r0, %0, lsr #11   \n"
    "        movs    r2, r2, lsr #11   \n"
    "                                  \n"
    "        muls    r0, r2            \n"
    "        movs    r0, r0, lsr #6    \n"
    "                                  \n"
    "        beq.n   2f                \n"
    "                                  \n"
    "1:      subs    r0, #1            \n"
    "        bhi     1b                \n"
#if (__CORTEX_M == 0x00)
    "2:                                \n"
    "        .syntax divided           \n" : : "r" (usecs), "r" (&loops_per_jiffy) : "r0", "r2", "cc");
#else
    "2:                                \n" : : "r" (usecs), "r" (&loops_per_jiffy) : "r0", "r2", "cc");
#endif
}
/* *INDENT-ON* */
#endif /* defined(__GNUC__) */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void calibrate_delay(void)
{
  /* From linux 2.4 source. */
  unsigned long loopbit;
  unsigned long ticks;
  int lps_precision = LPS_PREC;

  loops_per_jiffy = (1 << 12);

  while (loops_per_jiffy <<= 1) {
    /* wait for "start of" clock tick */
    ticks = clock();
    while (ticks == clock())
      /* nothing */;
    /* Go .. */
    ticks = clock();
    _delay(loops_per_jiffy);
    ticks = clock() - ticks;
    if (ticks) {
      break;
    }
  }

  /* Do a binary approximation to get loops_per_jiffy set to equal one clock
     (up to lps_precision bits) */

  loops_per_jiffy >>= 1;
  loopbit = loops_per_jiffy;
  while ( lps_precision-- && (loopbit >>= 1) ) {
    loops_per_jiffy |= loopbit;
    ticks = clock();
    while (ticks == clock()) ;
    ticks = clock();
    _delay(loops_per_jiffy);
    if (clock() != ticks) { /* longer than 1 tick */
      loops_per_jiffy &= ~loopbit;
    }
  }
}

__STATIC_INLINE uint32_t clock(void)
{
#if defined(UDELAY_CLOCK_SOURCE_BURTC)
  return BURTC_CounterGet();
#elif defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
  return RTCC_CounterGet();
#else
  return RTC_CounterGet();
#endif
}

#if defined(__ICCARM__) /* IAR */
/* *INDENT-OFF* */
static void _delay(uint32_t delay)
{
  __ASM volatile (
    "_delay_1:                         \n"
    "        subs    r0, #1            \n"
    "        bhi.n   _delay_1          \n");
}

void UDELAY_Delay(uint32_t usecs)
{
  __ASM volatile (
    "        cmp     %0, #0            \n" /* Return if 0 delay. */
    "        beq.n   udelay_2          \n"
    "        subs    %0, #1            \n" /* Correct for off by one error. */
    "        movs    r2, #0x88         \n"
    "        lsls    r2, r2, #8        \n"
    "        adds    r2, #0x00         \n"
    "        muls    %0, r2            \n"
    "                                  \n"
    "        ldr     r2, [%1]          \n"
    "        movs    r0, %0, lsr #11   \n"
    "        movs    r2, r2, lsr #11   \n"
    "                                  \n"
    "        muls    r0, r2            \n"
    "        movs    r0, r0, lsr #6    \n"
    "                                  \n"
    "        beq.n   udelay_2          \n"
    "                                  \n"
    "udelay_1:                         \n"
    "        subs    r0, #1            \n"
    "        bhi.n   udelay_1          \n"
    "udelay_2:                         \n" : : "r" (usecs), "r" (&loops_per_jiffy) : "r0", "r2", "cc");
}
/* *INDENT-ON* */
#endif /* defined(__ICCARM__) */

#if defined(__GNUC__) /* GCC */
/* *INDENT-OFF* */
static void _delay(uint32_t delay)
{
  __ASM volatile (
#if (__CORTEX_M == 0x00)
    "        .syntax unified           \n"
    "        .arch armv6-m             \n"
#endif
    "1:      subs    %0, #1            \n"
#if (__CORTEX_M == 0x00)
    "        bhi.n   1b                \n"
    "        .syntax divided           \n" : : "r" (delay) );
#else
    "        bhi.n   1b                \n" : : "r" (delay) );
#endif
}
/* *INDENT-ON* */
#endif /* defined(__GNUC__) */

#if defined(__CC_ARM) /* Keil */
/* *INDENT-OFF* */
static __ASM void _delay(uint32_t delay)
{
_delay_1
  subs    r0, #1
  bhi     _delay_1
  bx      lr
}

__ASM void UDELAY_Delay(uint32_t usecs __attribute__ ((unused)) )
{
  IMPORT  loops_per_jiffy

  cmp     r0, #0                        /* Return if 0 delay. */
  beq.n   udelay_2
  subs    r0, #1                        /* Correct for off by one error. */
  movs    r2, #0x88
  lsls    r2, r2, #8
  adds    r2, #0x00
  muls    r0, r2, r0

  ldr     r2, = loops_per_jiffy
                ldr     r2, [r2]
  movs    r0, r0, lsr #11
  movs    r2, r2, lsr #11

  muls    r0, r2, r0
  movs    r0, r0, lsr #6
  beq     udelay_2
udelay_1
  subs    r0, #1
  bhi     udelay_1
udelay_2
  bx      lr
}
/* *INDENT-ON* */
#endif /* defined(__CC_ARM) */

/** @endcond */

/** @} (end group Udelay) */
/** @} (end group kitdrv) */
