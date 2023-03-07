/***************************************************************************//**
 * @file
 * @brief Radio coexistence directional priority priority(DP)
 * @details Use the coexistence priority GPIO to communicate to the coexistence
 *   master device whether the request is for an RX or TX. In the case of an
 *   RX, the priority GPIO will be pulsed for a configurable period of time.
 *   In the case of a TX, the priority GPIO will be held through out the
 *   duration of the request.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_cmu.h"
#include "em_timer.h"
#include "em_prs.h"
#include "coexistence-hal.h"

#if SL_RAIL_UTIL_COEX_DP_ENABLED
#ifdef PLATFORM_HEADER
#include PLATFORM_HEADER
#endif //PLATFORM_HEADER
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT

#ifdef _SILICON_LABS_32B_SERIES_1
_Static_assert(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT == SL_RAIL_UTIL_COEX_PWM_REQ_PORT, "SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT must match SL_RAIL_UTIL_COEX_PWM_REQ_PORT");
_Static_assert(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN == SL_RAIL_UTIL_COEX_PWM_REQ_PIN, "SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN must match SL_RAIL_UTIL_COEX_PWM_REQ_PIN");
#else //!_SILICON_LABS_32B_SERIES_1
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT (SL_RAIL_UTIL_COEX_PWM_REQ_PORT)
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN (SL_RAIL_UTIL_COEX_PWM_REQ_PIN)
#endif //_SILICON_LABS_32B_SERIES_1
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT

#ifdef _SILICON_LABS_32B_SERIES_1
_Static_assert(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT == SL_RAIL_UTIL_COEX_REQ_PORT, "SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT must match SL_RAIL_UTIL_COEX_REQ_PORT");
_Static_assert(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN == SL_RAIL_UTIL_COEX_REQ_PIN, "SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN must match SL_RAIL_UTIL_COEX_REQ_PIN");
#else //!_SILICON_LABS_32B_SERIES_1
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT (SL_RAIL_UTIL_COEX_REQ_PORT)
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN (SL_RAIL_UTIL_COEX_REQ_PIN)
#endif //_SILICON_LABS_32B_SERIES_1
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#if SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL == 0
#error "Directional priority does not support active low priority(SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL == 0)"
#endif //SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL == 0
#if SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL == 0
#error "Directional priority does not support active low request(SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL == 0)"
#endif //SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL == 0

#define cmuClock_TIMER_DP GET_TIMER_REG(cmuClock, _SL_RAIL_UTIL_COEX_DP_TIMER)
#ifdef _SILICON_LABS_32B_SERIES_1
#define TIMER_DIVISOR 16
#define TIMER_PRESC_DIV_PREFIX TIMER_CTRL_PRESC_DIV
#define PRS_CH_CTRL_SOURCESEL_TIMER_DP GET_TIMER_REG(PRS_CH_CTRL_SOURCESEL, _SL_RAIL_UTIL_COEX_DP_TIMER)
#define PRS_CH_CTRL_SIGSEL_TIMER_DP GET_TIMER_REG(PRS_CH_CTRL_SIGSEL, _SL_RAIL_UTIL_COEX_DP_TIMER)
#define PRS_CH_CTRL_SIGSEL_TIMERCC0_DP GET_TIMER_REG(PRS_CH_CTRL_SIGSEL_TIMER_DP, CC0)
#else //!_SILICON_LABS_32B_SERIES_1
#define TIMER_DIVISOR 2
#define TIMER_PRESC_DIV_PREFIX TIMER_CFG_PRESC_DIV
#define PRS_CH_CTRL_SIGSEL_TIMER_DP GET_TIMER_REG(PRS_ASYNC, _SL_RAIL_UTIL_COEX_DP_TIMER)
#define PRS_CH_CTRL_SIGSEL_TIMERCC0_DP GET_TIMER_REG(PRS_CH_CTRL_SIGSEL_TIMER_DP, _CC0)
#endif //_SILICON_LABS_32B_SERIES_1
#define GET_TIMER_REG(reg, timer) GET_TIMER_REG_(reg, timer)
#define GET_TIMER_REG_(reg, timer) reg ## timer

#if SL_RAIL_UTIL_COEX_PRI_SHARED
#define SL_RAIL_UTIL_COEX_DP_MODE gpioModeWiredOr
#else //!SL_RAIL_UTIL_COEX_PRI_SHARED
#define SL_RAIL_UTIL_COEX_DP_MODE gpioModePushPull
#endif //SL_RAIL_UTIL_COEX_PRI_SHARED

#ifdef HAL_CONFIG
#if defined(TIMER0) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER0
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER0
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER0)
#elif defined(TIMER1) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER1
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER1
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER1)
#elif defined(TIMER2) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER2
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER2
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER2)
#elif defined(TIMER3) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER3
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER3
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER3)
#elif defined(TIMER4) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER4
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER4
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER4)
#elif defined(TIMER5) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER5
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER5
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER5)
#elif defined(TIMER6) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_TIMER6
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER6
#define SL_RAIL_UTIL_COEX_DP_TIMER (TIMER6)
#elif defined(WTIMER0) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_WTIMER0
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER0
#define SL_RAIL_UTIL_COEX_DP_TIMER (WTIMER0)
#elif defined(WTIMER1) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_WTIMER1
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER1
#define SL_RAIL_UTIL_COEX_DP_TIMER (WTIMER1)
#elif defined(WTIMER2) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_WTIMER2
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER2
#define SL_RAIL_UTIL_COEX_DP_TIMER (WTIMER2)
#elif defined(WTIMER3) && SL_RAIL_UTIL_COEX_DP_TIMER == HAL_TIMER_WTIMER3
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER3
#define SL_RAIL_UTIL_COEX_DP_TIMER (WTIMER3)
#else
#error "Unrecognized timer selection!"
#endif
#else //!HAL_CONFIG
#define __STRIP_TYPECAST(x)
#define _STRIP_TYPECAST(x)              __STRIP_TYPECAST x
#define STRIP_TYPECAST(x)               _STRIP_TYPECAST x
#define SL_RAIL_UTIL_COEX_DP_TIMER_ADDR STRIP_TYPECAST(SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL)

#if defined(TIMER0_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER0_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER0
#elif defined(TIMER1_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER1_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER1
#elif defined(TIMER2_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER2_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER2
#elif defined(TIMER3_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER3_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER3
#elif defined(TIMER4_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER4_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER4
#elif defined(TIMER5_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER5_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER5
#elif defined(TIMER6_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == TIMER6_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _TIMER6
#elif defined(WTIMER0_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == WTIMER0_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER0
#elif defined(WTIMER1_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == WTIMER1_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER1
#elif defined(WTIMER2_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == WTIMER2_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER2
#elif defined(WTIMER3_BASE) && SL_RAIL_UTIL_COEX_DP_TIMER_ADDR == WTIMER3_BASE
#define _SL_RAIL_UTIL_COEX_DP_TIMER _WTIMER3
#else
#error "Unrecognized timer selection!"
#endif
#define SL_RAIL_UTIL_COEX_DP_TIMER SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL
#endif //HAL_CONFIG

static uint8_t directionalPriorityPulseWidthUs;
static bool directionalPriorityInitialized = false;

typedef struct PRS_ChannelConfig {
  union {
    uint32_t source;
    uint32_t *sourcePtr;
  };
  union {
    uint32_t signal;
    uint32_t *signalPtr;
  };
  uint32_t ctrl;
  uint32_t channel;
  bool needToDereference;
} PRS_ChannelConfig_t;

#define TIMER_PRESC_DIV GET_TIMER_REG(TIMER_PRESC_DIV_PREFIX, TIMER_DIVISOR)
#define TIMER_FREQUENCY (CMU_ClockFreqGet(cmuClock_TIMER_DP) / TIMER_DIVISOR)
#define MICROSECONDS_PER_SECOND (1000000UL)

/** Map TIMER reference to index of device. */
#define TIMER_DEVICE_ID(timer) ( \
    (timer) == TIMER0   ? 0      \
    : (timer) == TIMER1 ? 1      \
    : (timer) == TIMER2 ? 2      \
    : (timer) == TIMER3 ? 3      \
    : -1)

static bool configDpTimer(uint8_t pulseWidthUs)
{
  uint32_t ticks = (pulseWidthUs * TIMER_FREQUENCY) / MICROSECONDS_PER_SECOND;
  // Setup TIMER for ONE-SHOT Triggers by REQUEST rising edge-----------------
  CMU_ClockEnable(cmuClock_TIMER_DP, true);     // turn on clock to TIMER

  // Reset compare output at start, run off HFPERCLK / 16, run in debug, count up,
  // Reload on rising edge, use one-shot mode
  // set PRS to track CC out level, set on start, clear on compare, PWM
  #ifdef _SILICON_LABS_32B_SERIES_1
  TIMER_Enable(SL_RAIL_UTIL_COEX_DP_TIMER, false);    // stop TIMER

  SL_RAIL_UTIL_COEX_DP_TIMER->CTRL = TIMER_CTRL_OSMEN
                                     | TIMER_CTRL_DEBUGRUN
                                     | TIMER_CTRL_RISEA_RELOADSTART
                                     | TIMER_PRESC_DIV
                                     | TIMER_CTRL_RSSCOIST;
  SL_RAIL_UTIL_COEX_DP_TIMER->CC[0].CTRL = TIMER_CC_CTRL_MODE_PWM
                                           | TIMER_CC_CTRL_OUTINV
                                           | TIMER_CC_CTRL_CMOA_SET
                                           | TIMER_CC_CTRL_PRSCONF;

  // Setup REQUEST GPIO as TIMx_CC0 trigger input
  // Configure TX/RX PRS output to selected channel and location
  // The TIMER_CC0 GPIO selected in HWCONF must match the selected
  // SL_RAIL_UTIL_COEX_REQ GPIO(or SL_RAIL_UTIL_COEX_PWM_REQ GPIO if available)
  BUS_RegMaskedClear(&SL_RAIL_UTIL_COEX_DP_TIMER->ROUTELOC0, _TIMER_ROUTELOC0_CC0LOC_MASK);
  BUS_RegMaskedSet(&SL_RAIL_UTIL_COEX_DP_TIMER->ROUTELOC0, SL_RAIL_UTIL_COEX_DP_TIMER_CC0_LOC);
  #else // !_SILICON_LABS_32B_SERIES_1
  TIMER_SyncWait(SL_RAIL_UTIL_COEX_DP_TIMER);
  SL_RAIL_UTIL_COEX_DP_TIMER->EN_CLR = TIMER_EN_EN;
#if defined(_TIMER_EN_DISABLING_MASK)
  while ((SL_RAIL_UTIL_COEX_DP_TIMER->EN & TIMER_EN_DISABLING) != 0U) {
  }
#endif
  SL_RAIL_UTIL_COEX_DP_TIMER->CFG = TIMER_CFG_OSMEN
                                    | TIMER_CFG_DEBUGRUN
                                    | TIMER_PRESC_DIV
                                    | TIMER_CFG_RSSCOIST;
  SL_RAIL_UTIL_COEX_DP_TIMER->CC[0].CFG = TIMER_CC_CFG_MODE_PWM
                                          | TIMER_CC_CFG_PRSCONF;
  SL_RAIL_UTIL_COEX_DP_TIMER->EN_SET = TIMER_EN_EN;

  SL_RAIL_UTIL_COEX_DP_TIMER->CTRL = TIMER_CTRL_RISEA_RELOADSTART;
  SL_RAIL_UTIL_COEX_DP_TIMER->CC[0].CTRL = TIMER_CC_CTRL_OUTINV
                                           | TIMER_CC_CTRL_CMOA_SET;
  GPIO->TIMERROUTE[TIMER_DEVICE_ID(SL_RAIL_UTIL_COEX_DP_TIMER)].CC0ROUTE = (SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                                                                           | (SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN
                                                                              << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
  #endif // _SILICON_LABS_32B_SERIES_1
  // pulse => CEIL(1200kHz*PULSE-1)
  TIMER_TopBufSet(SL_RAIL_UTIL_COEX_DP_TIMER, ticks);
  TIMER_TopSet(SL_RAIL_UTIL_COEX_DP_TIMER, ticks);
  // reset count
  TIMER_CounterSet(SL_RAIL_UTIL_COEX_DP_TIMER, 0);

  // pulse => CEIL(1200kHz*PULSE-1)
  TIMER_CompareBufSet(SL_RAIL_UTIL_COEX_DP_TIMER, 0, ticks);
  TIMER_CompareSet(SL_RAIL_UTIL_COEX_DP_TIMER, 0, ticks);
  TIMER_Enable(SL_RAIL_UTIL_COEX_DP_TIMER, true);   // start TIMER

  return true;
}

__STATIC_INLINE void configPrsChain(PRS_ChannelConfig_t *prsConfig,
                                    unsigned int channelCount)
{
#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  // enable ptaPriCfg interrupt if not already enabled
  if (sli_coex_ptaPriCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaPriCfg), true, false);
  }
#endif //SL_RAIL_UTIL_COEX_PRI_PORT

#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  // enable ptaPwmReqCfg interrupt if not already enabled
  if (sli_coex_ptaPwmReqCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaPwmReqCfg), true, false);
  }
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  // enable ptaReqCfg interrupt if not already enabled
  if (sli_coex_ptaReqCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaReqCfg), true, false);
  }
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT

  for (unsigned int ch = 0; ch < channelCount; ++ch) {
#ifdef _SILICON_LABS_32B_SERIES_1
    PRS->CH[prsConfig[ch].channel].CTRL = 0U;
#endif //_SILICON_LABS_32B_SERIES_1
    if (prsConfig[ch].needToDereference) {
      PRS_SourceAsyncSignalSet(prsConfig[ch].channel,
                               *(prsConfig[ch].sourcePtr),
                               *(prsConfig[ch].signalPtr));
    } else {
      PRS_SourceAsyncSignalSet(prsConfig[ch].channel,
                               prsConfig[ch].source,
                               prsConfig[ch].signal);
    }
#ifdef _SILICON_LABS_32B_SERIES_1
    PRS->CH[prsConfig[ch].channel].CTRL |= prsConfig[ch].ctrl;
#else //!_SILICON_LABS_32B_SERIES_1
    PRS_Combine(prsConfig[ch].channel,
                WRAP_PRS_ASYNC(prsConfig[ch].channel - 1),
                (PRS_Logic_t)prsConfig[ch].ctrl);
#endif //_SILICON_LABS_32B_SERIES_1
  }
}

#define  CONFIG_PRS_CHAIN(prsChain) (configPrsChain(prsChain, \
                                                    sizeof(prsChain) / sizeof(prsChain[0])))

PRS_ChannelConfig_t prsChainOff[] = {
#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .sourcePtr = &(sli_coex_ptaPriCfg.source),
    .signalPtr = &(sli_coex_ptaPriCfg.signal),
#ifndef _SILICON_LABS_32B_SERIES_1
    .ctrl = prsLogic_A,
#endif
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = true
  }
#elif defined(_SILICON_LABS_32B_SERIES_1) //!defined(SL_RAIL_UTIL_COEX_PRI_PORT)
  {
    .source = PRS_CHANNEL_SOURCE(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .signal = PRS_CHANNEL_SIGNAL(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .ctrl = PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = false
  },
  {
    .source = PRS_CHANNEL_SOURCE(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .signal = PRS_CHANNEL_SIGNAL(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .ctrl = PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = false
  },
  {
    .source = PRS_CHANNEL_SOURCE(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .signal = PRS_CHANNEL_SIGNAL(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .ctrl = PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = false
  },
#else //!(defined(_SILICON_LABS_32B_SERIES_1) || defined(SL_RAIL_UTIL_COEX_PRI_PORT))
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaPwmReqCfg.source),
    .signalPtr = &(sli_coex_ptaPwmReqCfg.signal),
    .ctrl = prsLogic_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = true
  },
  {
    .sourcePtr = &(sli_coex_ptaPwmReqCfg.source),
    .signalPtr = &(sli_coex_ptaPwmReqCfg.signal),
    .ctrl = prsLogic_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = true
  },
  {
    .sourcePtr = &(sli_coex_ptaPwmReqCfg.source),
    .signalPtr = &(sli_coex_ptaPwmReqCfg.signal),
    .ctrl = prsLogic_A_AND_NOT_B,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = true
  }
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaReqCfg.source),
    .signalPtr = &(sli_coex_ptaReqCfg.signal),
    .ctrl = prsLogic_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = true
  },
  {
    .sourcePtr = &(sli_coex_ptaReqCfg.source),
    .signalPtr = &(sli_coex_ptaReqCfg.signal),
    .ctrl = prsLogic_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = true
  },
  {
    .sourcePtr = &(sli_coex_ptaReqCfg.source),
    .signalPtr = &(sli_coex_ptaReqCfg.signal),
    .ctrl = prsLogic_A_AND_NOT_B,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = true
  }
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
};

#ifdef PRS_RACL_PAEN
#define PRS_RAC_PAEN PRS_RACL_PAEN
#endif

#ifndef _SILICON_LABS_32B_SERIES_1
PRS_ChannelConfig_t prsChainOn[] = {
#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .sourcePtr = &(sli_coex_ptaPriCfg.source),
    .signalPtr = &(sli_coex_ptaPriCfg.signal),
    .ctrl = prsLogic_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 3),
    .needToDereference = true
  },
  {
    .signal = PRS_CH_CTRL_SIGSEL_TIMERCC0_DP,
    .ctrl = prsLogic_NOT_A_AND_B,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = false
  },
  {
    .signal = PRS_RAC_PAEN,
    .ctrl = prsLogic_A_NOR_B,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = false
  },
#else //!SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .signal = PRS_CH_CTRL_SIGSEL_TIMERCC0_DP,
    .ctrl = prsLogic_NOT_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = false
  },
  {
    .signal = PRS_RAC_PAEN,
    .ctrl = prsLogic_NOT_A,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = false
  },
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaPwmReqCfg.source),
    .signalPtr = &(sli_coex_ptaPwmReqCfg.signal),
    .ctrl = prsLogic_A_AND_NOT_B,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = true
  }
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaReqCfg.source),
    .signalPtr = &(sli_coex_ptaReqCfg.signal),
    .ctrl = prsLogic_A_AND_NOT_B,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = true
  }
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
};
#else
PRS_ChannelConfig_t prsChainOn[] = {
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaPwmReqCfg.source),
    .signalPtr = &(sli_coex_ptaPwmReqCfg.signal),
    .ctrl = PRS_CH_CTRL_INV,
    .channel = SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL,
    .needToDereference = true
  },
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  {
    .sourcePtr = &(sli_coex_ptaReqCfg.source),
    .signalPtr = &(sli_coex_ptaReqCfg.signal),
    .ctrl = PRS_CH_CTRL_INV,
    .channel = SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL,
    .needToDereference = true
  },
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .sourcePtr = &(sli_coex_ptaPriCfg.source),
    .signalPtr = &(sli_coex_ptaPriCfg.signal),
    .ctrl = PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 3),
    .needToDereference = true
  },
  {
    .source = PRS_CH_CTRL_SOURCESEL_TIMER_DP,
    .signal = PRS_CH_CTRL_SIGSEL_TIMERCC0_DP,
    .ctrl = PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = false
  },
  {
    .signal = PRS_RAC_PAEN,
    .ctrl = PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = false
  },
#else //!SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .source = PRS_CH_CTRL_SOURCESEL_TIMER_DP,
    .signal = PRS_CH_CTRL_SIGSEL_TIMERCC0_DP,
    .ctrl = PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 2),
    .needToDereference = false
  },
  {
    .signal = PRS_RAC_PAEN,
    .ctrl = PRS_CH_CTRL_INV,
    .channel = WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1),
    .needToDereference = false
  },
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
  {
    .source = PRS_CHANNEL_SOURCE(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .signal = PRS_CHANNEL_SIGNAL(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL),
    .ctrl = PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV,
    .channel = SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL,
    .needToDereference = false
  }
};
#endif

bool COEX_HAL_ConfigDp(uint8_t pulseWidthUs)
{
  // Common PRS setup (clock enable, REQUEST and PRIORITY GPIO INT PRS sources)
  // enable clock to PRS
  CMU_ClockEnable(cmuClock_PRS, true);

#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  if (sli_coex_ptaPriCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaPriCfg), true, false);
  }
  // Disable priority and request interrupts
  GPIO_ExtIntConfig(SL_RAIL_UTIL_COEX_PRI_PORT,
                    SL_RAIL_UTIL_COEX_PRI_PIN,
                    sli_coex_ptaPriCfg.intNo,
                    false,
                    false,
                    false);
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  if (sli_coex_ptaPwmReqCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaPwmReqCfg), true, false);
  }
  GPIO_ExtIntConfig(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT,
                    SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN,
                    sli_coex_ptaPwmReqCfg.intNo,
                    false,
                    false,
                    false);
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  if (sli_coex_ptaReqCfg.intNo == INVALID_INTERRUPT) {
    sli_coex_enableGpioInt(&(sli_coex_ptaReqCfg), true, false);
  }
  GPIO_ExtIntConfig(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT,
                    SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN,
                    sli_coex_ptaReqCfg.intNo,
                    false,
                    false,
                    false);
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  // Common PRS wrap-up (enable PRIORITY GPIO, route PRS output to GPIO)
  // enable PRIORITY output pin with initial value of 0
  GPIO_PinModeSet(SL_RAIL_UTIL_COEX_DP_OUT_PORT,
                  SL_RAIL_UTIL_COEX_DP_OUT_PIN,
                  SL_RAIL_UTIL_COEX_DP_MODE,
                  0);
#ifdef _SILICON_LABS_32B_SERIES_1
  volatile uint32_t * routeRegister;
  // Configure directional priority PRS output to selected channel and location
  if (SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    return false; // error
  }
  // Route PRS CH/LOC to PRIORITY GPIO output
  BUS_RegMaskedClear(routeRegister, 0xFFU << ((SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL & 3) * 8));
  BUS_RegMaskedSet(routeRegister, SL_RAIL_UTIL_COEX_DP_OUT_LOC << ((SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL & 3) * 8));
  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_PinOutput(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL, prsTypeAsync, SL_RAIL_UTIL_COEX_DP_OUT_PORT, SL_RAIL_UTIL_COEX_DP_OUT_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
  return COEX_HAL_SetDpPulseWidth(pulseWidthUs);
}

bool COEX_HAL_SetDpPulseWidth(uint8_t pulseWidthUs)
{
  if (directionalPriorityInitialized
      && directionalPriorityPulseWidthUs == pulseWidthUs) {
    return true;
  }
  directionalPriorityInitialized = true;
  directionalPriorityPulseWidthUs = pulseWidthUs;
  if (pulseWidthUs == 0) {
    #ifndef _SILICON_LABS_32B_SERIES_1
    SL_RAIL_UTIL_COEX_DP_TIMER->EN_SET = TIMER_EN_EN;
    #endif //!_SILICON_LABS_32B_SERIES_1
    TIMER_Enable(SL_RAIL_UTIL_COEX_DP_TIMER, false);
    CONFIG_PRS_CHAIN(prsChainOff);
    return true;
  }
  if (!configDpTimer(directionalPriorityPulseWidthUs)) {
    return false;
  }
  CONFIG_PRS_CHAIN(prsChainOn);
  return true;
}

uint8_t COEX_HAL_GetDpPulseWidth(void)
{
  return directionalPriorityPulseWidthUs;
}

#else //!SL_RAIL_UTIL_COEX_DP_ENABLED

bool COEX_HAL_ConfigDp(uint8_t pulseWidthUs)
{
  (void)pulseWidthUs;
  return false;
}

uint8_t COEX_HAL_GetDpPulseWidth(void)
{
  return 0;
}

bool COEX_HAL_SetDpPulseWidth(uint8_t pulseWidthUs)
{
  (void)pulseWidthUs;
  return false;
}
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED
