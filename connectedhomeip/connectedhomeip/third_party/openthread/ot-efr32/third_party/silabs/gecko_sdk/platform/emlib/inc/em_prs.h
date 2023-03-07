/***************************************************************************//**
 * @file
 * @brief Peripheral Reflex System (PRS) peripheral API
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

#ifndef EM_PRS_H
#define EM_PRS_H

#include "em_device.h"
#include "em_gpio.h"

#include <stdbool.h>
#include <stddef.h>

#if defined(PRS_COUNT) && (PRS_COUNT > 0)

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup prs
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_2)
/** PRS Synchronous channel count. */
  #define PRS_SYNC_CHAN_COUNT    PRS_SYNC_CH_NUM
/** PRS Asynchronous channel count. */
  #define PRS_ASYNC_CHAN_COUNT   PRS_ASYNC_CH_NUM
#elif defined(_EFM32_GECKO_FAMILY)
/** PRS Synchronous channel count. */
  #define PRS_SYNC_CHAN_COUNT    PRS_CHAN_COUNT
/** PRS Asynchronous channel count. */
  #define PRS_ASYNC_CHAN_COUNT   0
#else
/** PRS Synchronous channel count. */
  #define PRS_SYNC_CHAN_COUNT    PRS_CHAN_COUNT
/** PRS Asynchronous channel count. */
  #define PRS_ASYNC_CHAN_COUNT   PRS_CHAN_COUNT
#endif

#if !defined(_EFM32_GECKO_FAMILY)
/** PRS asynchronous support */
#define PRS_ASYNC_SUPPORTED      1
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Some devices have renamed signals so some of these signals are mapped to
   common names. */
#if defined(PRS_USART0_RXDATAV)
#define PRS_USART0_RXDATA        PRS_USART0_RXDATAV
#endif
#if defined(PRS_USART1_RXDATAV)
#define PRS_USART1_RXDATA        PRS_USART1_RXDATAV
#endif
#if defined(PRS_USART2_RXDATAV)
#define PRS_USART2_RXDATA        PRS_USART2_RXDATAV
#endif
#if defined(PRS_BURTC_OVERFLOW)
#define PRS_BURTC_OF             PRS_BURTC_OVERFLOW
#endif
#if defined(PRS_BURTC_COMP0)
#define PRS_BURTC_COMP           PRS_BURTC_COMP0
#endif
/** @endcond */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** PRS Channel type. */
typedef enum {
  prsTypeAsync,  /**< Asynchronous channel type. */
  prsTypeSync    /**< Synchronous channel type.*/
} PRS_ChType_t;

/** Edge detection type. */
typedef enum {
  prsEdgeOff,  /**< Leave signal as is. */
  prsEdgePos,  /**< Generate pulses on positive edge. */
  prsEdgeNeg,  /**< Generate pulses on negative edge. */
  prsEdgeBoth  /**< Generate pulses on both edges. */
} PRS_Edge_TypeDef;

#if defined(_PRS_ASYNC_CH_CTRL_FNSEL_MASK)
/** Logic functions that can be used when combining two PRS channels. */
typedef enum {
  prsLogic_Zero        = _PRS_ASYNC_CH_CTRL_FNSEL_LOGICAL_ZERO, /**< Logical 0. */
  prsLogic_A_NOR_B     = _PRS_ASYNC_CH_CTRL_FNSEL_A_NOR_B,      /**< A NOR B. */
  prsLogic_NOT_A_AND_B = _PRS_ASYNC_CH_CTRL_FNSEL_NOT_A_AND_B,  /**< (!A) NOR B. */
  prsLogic_NOT_A       = _PRS_ASYNC_CH_CTRL_FNSEL_NOT_A,        /**< !A. */
  prsLogic_A_AND_NOT_B = _PRS_ASYNC_CH_CTRL_FNSEL_A_AND_NOT_B,  /**< A AND (!B). */
  prsLogic_NOT_B       = _PRS_ASYNC_CH_CTRL_FNSEL_NOT_B,        /**< !B. */
  prsLogic_A_XOR_B     = _PRS_ASYNC_CH_CTRL_FNSEL_A_XOR_B,      /**< A XOR B. */
  prsLogic_A_NAND_B    = _PRS_ASYNC_CH_CTRL_FNSEL_A_NAND_B,     /**< A NAND B. */
  prsLogic_A_AND_B     = _PRS_ASYNC_CH_CTRL_FNSEL_A_AND_B,      /**< A AND B. */
  prsLogic_A_XNOR_B    = _PRS_ASYNC_CH_CTRL_FNSEL_A_XNOR_B,     /**< A XNOR B. */
  prsLogic_B           = _PRS_ASYNC_CH_CTRL_FNSEL_B,            /**< B. */
  prsLogic_NOT_A_OR_B  = _PRS_ASYNC_CH_CTRL_FNSEL_NOT_A_OR_B,   /**< (!A) OR B. */
  prsLogic_A           = _PRS_ASYNC_CH_CTRL_FNSEL_A,            /**< A. */
  prsLogic_A_OR_NOT_B  = _PRS_ASYNC_CH_CTRL_FNSEL_A_OR_NOT_B,   /**< A OR (!B). */
  prsLogic_A_OR_B      = _PRS_ASYNC_CH_CTRL_FNSEL_A_OR_B,       /**< A OR B. */
  prsLogic_One         = _PRS_ASYNC_CH_CTRL_FNSEL_LOGICAL_ONE,  /**< Logical 1. */
} PRS_Logic_t;
#endif

/** PRS Signal. */
typedef enum {
#if defined(_PRS_SYNC_CH_CTRL_SOURCESEL_MASK)
  prsSignalNone       = PRS_SYNC_CH_CTRL_SOURCESEL_DEFAULT | (0x0 << _PRS_SYNC_CH_CTRL_SIGSEL_SHIFT), /**< No Signal. */
  prsSignalSW         = PRS_SYNC_CH_CTRL_SOURCESEL_DEFAULT | (0x1 << _PRS_SYNC_CH_CTRL_SIGSEL_SHIFT), /**< Software-reserved Signal. */
#else
  prsSignalNone       = PRS_CH_CTRL_SOURCESEL_NONE | (0x0 << _PRS_CH_CTRL_SIGSEL_SHIFT),  /**< No Signal. */
  prsSignalSW         = PRS_CH_CTRL_SOURCESEL_NONE | (0x1 << _PRS_CH_CTRL_SIGSEL_SHIFT),  /**< Software-reserved Signal. */
#endif

#if defined(PRS_PRS_CH11)
  prsSignalPRS_CH0  = PRS_PRS_CH0,  /**< PRS_CH0  signal */
  prsSignalPRS_CH1  = PRS_PRS_CH1,  /**< PRS_CH1  signal */
  prsSignalPRS_CH2  = PRS_PRS_CH2,  /**< PRS_CH2  signal */
  prsSignalPRS_CH3  = PRS_PRS_CH3,  /**< PRS_CH3  signal */
  prsSignalPRS_CH4  = PRS_PRS_CH4,  /**< PRS_CH4  signal */
  prsSignalPRS_CH5  = PRS_PRS_CH5,  /**< PRS_CH5  signal */
  prsSignalPRS_CH6  = PRS_PRS_CH6,  /**< PRS_CH6  signal */
  prsSignalPRS_CH7  = PRS_PRS_CH7,  /**< PRS_CH7  signal */
  prsSignalPRS_CH8  = PRS_PRS_CH8,  /**< PRS_CH8  signal */
  prsSignalPRS_CH9  = PRS_PRS_CH9,  /**< PRS_CH9  signal */
  prsSignalPRS_CH10 = PRS_PRS_CH10, /**< PRS_CH10 signal */
  prsSignalPRS_CH11 = PRS_PRS_CH11, /**< PRS_CH11 signal */
#endif
#if defined(PRS_PRS_CH23)
  prsSignalPRS_CH12 = PRS_PRS_CH12, /**< PRS_CH12 signal */
  prsSignalPRS_CH13 = PRS_PRS_CH13, /**< PRS_CH13 signal */
  prsSignalPRS_CH14 = PRS_PRS_CH14, /**< PRS_CH14 signal */
  prsSignalPRS_CH15 = PRS_PRS_CH15, /**< PRS_CH15 signal */
  prsSignalPRS_CH16 = PRS_PRS_CH16, /**< PRS_CH16 signal */
  prsSignalPRS_CH17 = PRS_PRS_CH17, /**< PRS_CH17 signal */
  prsSignalPRS_CH18 = PRS_PRS_CH18, /**< PRS_CH18 signal */
  prsSignalPRS_CH19 = PRS_PRS_CH19, /**< PRS_CH19 signal */
  prsSignalPRS_CH20 = PRS_PRS_CH20, /**< PRS_CH20 signal */
  prsSignalPRS_CH21 = PRS_PRS_CH21, /**< PRS_CH21 signal */
  prsSignalPRS_CH22 = PRS_PRS_CH22, /**< PRS_CH22 signal */
  prsSignalPRS_CH23 = PRS_PRS_CH23, /**< PRS_CH23 signal */
#endif

#if defined(PRS_ADC0_SINGLE)
  prsSignalADC0_SINGLE = PRS_ADC0_SINGLE, /**< ADC0_SINGLE  signal */
  prsSignalADC0_SCAN   = PRS_ADC0_SCAN,   /**< ADC0_SCAN  signal */
#endif
#if defined(PRS_ADC1_SINGLE)
  prsSignalADC1_SINGLE = PRS_ADC1_SINGLE, /**< ADC1_SINGLE  signal */
  prsSignalADC1_SCAN   = PRS_ADC1_SCAN,   /**< ADC1_SCAN  signal */
#endif

  /* Timer Signals */
#if defined(TIMER0)
  prsSignalTIMER0_UF  = PRS_TIMER0_UF,  /**< TIMER0 underflow Signal. */
  prsSignalTIMER0_OF  = PRS_TIMER0_OF,  /**< TIMER0 overflow Signal. */
  prsSignalTIMER0_CC0 = PRS_TIMER0_CC0, /**< TIMER0 capture/compare channel 0 Signal. */
  prsSignalTIMER0_CC1 = PRS_TIMER0_CC1, /**< TIMER0 capture/compare channel 1 Signal. */
  prsSignalTIMER0_CC2 = PRS_TIMER0_CC2, /**< TIMER0 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER1)
  prsSignalTIMER1_UF  = PRS_TIMER1_UF,  /**< TIMER1 underflow Signal. */
  prsSignalTIMER1_OF  = PRS_TIMER1_OF,  /**< TIMER1 overflow Signal. */
  prsSignalTIMER1_CC0 = PRS_TIMER1_CC0, /**< TIMER1 capture/compare channel 0 Signal. */
  prsSignalTIMER1_CC1 = PRS_TIMER1_CC1, /**< TIMER1 capture/compare channel 1 Signal. */
  prsSignalTIMER1_CC2 = PRS_TIMER1_CC2, /**< TIMER1 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER2)
  prsSignalTIMER2_UF  = PRS_TIMER2_UF,  /**< TIMER2 underflow Signal. */
  prsSignalTIMER2_OF  = PRS_TIMER2_OF,  /**< TIMER2 overflow Signal. */
  prsSignalTIMER2_CC0 = PRS_TIMER2_CC0, /**< TIMER2 capture/compare channel 0 Signal. */
  prsSignalTIMER2_CC1 = PRS_TIMER2_CC1, /**< TIMER2 capture/compare channel 1 Signal. */
  prsSignalTIMER2_CC2 = PRS_TIMER2_CC2, /**< TIMER2 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER3)
  prsSignalTIMER3_UF  = PRS_TIMER3_UF,  /**< TIMER3 underflow Signal. */
  prsSignalTIMER3_OF  = PRS_TIMER3_OF,  /**< TIMER3 overflow Signal. */
  prsSignalTIMER3_CC0 = PRS_TIMER3_CC0, /**< TIMER3 capture/compare channel 0 Signal. */
  prsSignalTIMER3_CC1 = PRS_TIMER3_CC1, /**< TIMER3 capture/compare channel 1 Signal. */
  prsSignalTIMER3_CC2 = PRS_TIMER3_CC2, /**< TIMER3 capture/compare channel 2 Signal. */
#if defined(PRS_TIMER1_CC3)
  prsSignalTIMER1_CC3 = PRS_TIMER1_CC3, /**< TIMER3 capture/compare channel 3 Signal. */
#endif
#endif
#if defined(TIMER4)
  prsSignalTIMER4_UF  = PRS_TIMER4_UF,  /**< TIMER4 underflow Signal. */
  prsSignalTIMER4_OF  = PRS_TIMER4_OF,  /**< TIMER4 overflow Signal. */
  prsSignalTIMER4_CC0 = PRS_TIMER4_CC0, /**< TIMER4 capture/compare channel 0 Signal. */
  prsSignalTIMER4_CC1 = PRS_TIMER4_CC1, /**< TIMER4 capture/compare channel 1 Signal. */
  prsSignalTIMER4_CC2 = PRS_TIMER4_CC2, /**< TIMER4 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER5)
  prsSignalTIMER5_UF  = PRS_TIMER5_UF,  /**< TIMER5 underflow Signal. */
  prsSignalTIMER5_OF  = PRS_TIMER5_OF,  /**< TIMER5 overflow Signal. */
  prsSignalTIMER5_CC0 = PRS_TIMER5_CC0, /**< TIMER5 capture/compare channel 0 Signal. */
  prsSignalTIMER5_CC1 = PRS_TIMER5_CC1, /**< TIMER5 capture/compare channel 1 Signal. */
  prsSignalTIMER5_CC2 = PRS_TIMER5_CC2, /**< TIMER5 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER6)
  prsSignalTIMER6_UF  = PRS_TIMER6_UF,  /**< TIMER6 underflow Signal. */
  prsSignalTIMER6_OF  = PRS_TIMER6_OF,  /**< TIMER6 overflow Signal. */
  prsSignalTIMER6_CC0 = PRS_TIMER6_CC0, /**< TIMER6 capture/compare channel 0 Signal. */
  prsSignalTIMER6_CC1 = PRS_TIMER6_CC1, /**< TIMER6 capture/compare channel 1 Signal. */
  prsSignalTIMER6_CC2 = PRS_TIMER6_CC2, /**< TIMER6 capture/compare channel 2 Signal. */
#endif
#if defined(TIMER7)
  prsSignalTIMER7_UF  = PRS_TIMER7_UF,  /**< TIMER7 underflow Signal. */
  prsSignalTIMER7_OF  = PRS_TIMER7_OF,  /**< TIMER7 overflow Signal. */
  prsSignalTIMER7_CC0 = PRS_TIMER7_CC0, /**< TIMER7 capture/compare channel 0 Signal. */
  prsSignalTIMER7_CC1 = PRS_TIMER7_CC1, /**< TIMER7 capture/compare channel 1 Signal. */
  prsSignalTIMER7_CC2 = PRS_TIMER7_CC2, /**< TIMER7 capture/compare channel 2 Signal. */
#endif
#if defined(PRS_LETIMER0_CH0)
  prsSignalLETIMER0_CH0  = PRS_LETIMER0_CH0, /**< LETIMER0 channel 0 Signal. */
  prsSignalLETIMER0_CH1  = PRS_LETIMER0_CH1, /**< LETIMER0 channel 1 Signal. */
#endif
#if defined(PRS_LETIMER1_CH0)
  prsSignalLETIMER1_CH0  = PRS_LETIMER1_CH0, /**< LETIMER1 channel 0 Signal. */
  prsSignalLETIMER1_CH1  = PRS_LETIMER1_CH1, /**< LETIMER1 channel 1 Signal. */
#endif
#if defined(PRS_PCNT0_TCC)
  prsSignalPCNT0_TCC  = PRS_PCNT0_TCC,  /**< PCNT0_TCC Signal. */
#endif
#if defined(PRS_PCNT0_UFOF)
  prsSignalPCNT0_UFOF = PRS_PCNT0_UFOF, /**< PCNT0_TCC Signal. */
  prsSignalPCNT0_DIR  = PRS_PCNT0_DIR,  /**< PCNT0_TCC Signal. */
#endif
#if defined(PRS_PCNT1_TCC)
  prsSignalPCNT1_TCC  = PRS_PCNT1_TCC,  /**< PCNT1_TCC Signal. */
  prsSignalPCNT1_UFOF = PRS_PCNT1_UFOF, /**< PCNT1_TCC Signal. */
  prsSignalPCNT1_DIR  = PRS_PCNT1_DIR,  /**< PCNT1_TCC Signal. */
#endif
#if defined(PRS_PCNT2_TCC)
  prsSignalPCNT2_TCC  = PRS_PCNT2_TCC,  /**< PCNT2_TCC Signal. */
  prsSignalPCNT2_UFOF = PRS_PCNT2_UFOF, /**< PCNT2_TCC Signal. */
  prsSignalPCNT2_DIR  = PRS_PCNT2_DIR,  /**< PCNT2_TCC Signal. */
#endif
#if defined(PRS_CRYOTIMER_PERIOD)
  prsSignalCRYOTIMER_PERIOD = PRS_CRYOTIMER_PERIOD, /**< CRYOTIMER_PERIOD Signal. */
#endif
#if defined(PRS_CORE_CTIOUT0)
  prsSignalCORE_CTIOUT0 = PRS_CORE_CTIOUT0, /**< CORE CTIOUT0 Signal. */
  prsSignalCORE_CTIOUT1 = PRS_CORE_CTIOUT1, /**< CORE CTIOUT1 Signal. */
  prsSignalCORE_CTIOUT2 = PRS_CORE_CTIOUT2, /**< CORE CTIOUT2 Signal. */
  prsSignalCORE_CTIOUT3 = PRS_CORE_CTIOUT3, /**< CORE CTIOUT3 Signal. */
#endif
#if defined(PRS_CMUL_CLKOUT0)
  prsSignalCMUL_CLKOUT0       = PRS_CMUL_CLKOUT0,       /**< CMU CLKOUT0 Signal. */
  prsSignalCMUL_CLKOUT1       = PRS_CMUL_CLKOUT1,       /**< CMU CLKOUT1 Signal. */
  prsSignalCMUL_CLKOUT2       = PRS_CMUL_CLKOUT2,       /**< CMU CLKOUT2 Signal. */
#endif
#if defined(PRS_PRSL_ASYNCH0)
  prsSignalPRSL_ASYNCH0 = PRS_PRSL_ASYNCH0, /**< PRS channel 0 Signal. */
  prsSignalPRSL_ASYNCH1 = PRS_PRSL_ASYNCH1, /**< PRS channel 1 Signal. */
  prsSignalPRSL_ASYNCH2 = PRS_PRSL_ASYNCH2, /**< PRS channel 2 Signal. */
  prsSignalPRSL_ASYNCH3 = PRS_PRSL_ASYNCH3, /**< PRS channel 3 Signal. */
  prsSignalPRSL_ASYNCH4 = PRS_PRSL_ASYNCH4, /**< PRS channel 4 Signal. */
  prsSignalPRSL_ASYNCH5 = PRS_PRSL_ASYNCH5, /**< PRS channel 5 Signal. */
  prsSignalPRSL_ASYNCH6 = PRS_PRSL_ASYNCH6, /**< PRS channel 6 Signal. */
  prsSignalPRSL_ASYNCH7 = PRS_PRSL_ASYNCH7, /**< PRS channel 7 Signal. */
  prsSignalPRS_ASYNCH8  = PRS_PRS_ASYNCH8,  /**< PRS channel 8 Signal. */
  prsSignalPRS_ASYNCH9  = PRS_PRS_ASYNCH9,  /**< PRS channel 9 Signal. */
  prsSignalPRS_ASYNCH10 = PRS_PRS_ASYNCH10, /**< PRS channel 10 Signal. */
  prsSignalPRS_ASYNCH11 = PRS_PRS_ASYNCH11, /**< PRS channel 11 Signal. */
#endif

  /* RTC/RTCC/SYSRTC/BURTC Signals */
#if defined(PRS_RTC_OF)
  prsSignalRTC_OF    = PRS_RTC_OF,    /**< RTC_OF    signal. */
  prsSignalRTC_COMP0 = PRS_RTC_COMP0, /**< RTC_COMP0 signal. */
  prsSignalRTC_COMP1 = PRS_RTC_COMP1, /**< RTC_COMP1 signal. */
#if defined(PRS_RTC_COMP5)
  prsSignalRTC_COMP2 = PRS_RTC_COMP2, /**< RTC_COMP2 signal. */
  prsSignalRTC_COMP3 = PRS_RTC_COMP3, /**< RTC_COMP3 signal. */
  prsSignalRTC_COMP4 = PRS_RTC_COMP4, /**< RTC_COMP4 signal. */
  prsSignalRTC_COMP5 = PRS_RTC_COMP5, /**< RTC_COMP5 signal. */
#endif
#endif
#if defined(RTCC)
  prsSignalRTCC_CCV0  = PRS_RTCC_CCV0, /**< RTCC capture/compare channel 0 Signal. */
  prsSignalRTCC_CCV1  = PRS_RTCC_CCV1, /**< RTCC capture/compare channel 1 Signal. */
  prsSignalRTCC_CCV2  = PRS_RTCC_CCV2, /**< RTCC capture/compare channel 2 Signal. */
#endif
#if defined(BURTC)
  prsSignalBURTC_COMP = PRS_BURTC_COMP, /**< BURTC compare Signal. */
  prsSignalBURTC_OF   = PRS_BURTC_OF,   /**< BURTC overflow Signal. */
#endif
#if defined(SYSRTC0)
  prsSignalSYSRTC0_GRP0OUT0 = PRS_SYSRTC0_GRP0OUT0, /**< SYSRTC GRP0OUT0 Signal. */
  prsSignalSYSRTC0_GRP0OUT1 = PRS_SYSRTC0_GRP0OUT1, /**< SYSRTC GRP0OUT1 Signal. */
  prsSignalSYSRTC0_GRP1OUT0 = PRS_SYSRTC0_GRP1OUT0, /**< SYSRTC GRP1OUT0 Signal. */
  prsSignalSYSRTC0_GRP1OUT1 = PRS_SYSRTC0_GRP1OUT1, /**< SYSRTC GRP1OUT1 Signal. */
#endif
#if defined(PRS_HFXO0L_STATUS)
  prsSignalHFXO0L_STATUS            = PRS_HFXO0L_STATUS,            /**< HFXO0L_STATUS  Signal. */
  prsSignalHFXO0L_STATUS1           = PRS_HFXO0L_STATUS1,           /**< HFXO0L_STATUS1  Signal. */
#endif
#if defined(PRS_HFRCO0_COREEN)
  prsSignalHFRCO0_COREEN = PRS_HFRCO0_COREEN, /**< HFRCO0_COREEN  Signal. */
  prsSignalHFRCO0_STATE0 = PRS_HFRCO0_STATE0, /**< HFRCO0_STATE0  Signal. */
  prsSignalHFRCO0_STATE1 = PRS_HFRCO0_STATE1, /**< HFRCO0_STATE1  Signal. */
  prsSignalHFRCO0_STATE2 = PRS_HFRCO0_STATE2, /**< HFRCO0_STATE2  Signal. */
#endif
#if defined(PRS_HFRCOEM23_COREEN)
  prsSignalHFRCOEM23_COREEN = PRS_HFRCOEM23_COREEN, /**< HFRCOEM23_COREEN  Signal. */
  prsSignalHFRCOEM23_STATE0 = PRS_HFRCOEM23_STATE0, /**< HFRCOEM23_STATE0  Signal. */
  prsSignalHFRCOEM23_STATE1 = PRS_HFRCOEM23_STATE1, /**< HFRCOEM23_STATE1  Signal. */
  prsSignalHFRCOEM23_STATE2 = PRS_HFRCOEM23_STATE2, /**< HFRCOEM23_STATE2  Signal. */
#endif

  /* ACMP Signals */
#if defined(ACMP0)
  prsSignalACMP0_OUT  = PRS_ACMP0_OUT, /**< ACMP0 Signal. */
#endif
#if defined(ACMP1)
  prsSignalACMP1_OUT  = PRS_ACMP1_OUT, /**< ACMP1 output Signal. */
#endif
#if defined(ACMP2)
  prsSignalACMP2_OUT  = PRS_ACMP2_OUT, /**< ACMP2 output Signal. */
#endif
#if defined(ACMP3)
  prsSignalACMP3_OUT  = PRS_ACMP3_OUT, /**< ACMP3 output Signal. */
#endif

  /* VDAC Signals */
#if defined(VDAC0) & (_SILICON_LABS_32B_SERIES >= 2)
  prsSignalVDAC0_CH0WARM          = PRS_VDAC0L_CH0WARM,         /**< VDAC0 channel 0 warmed Signal. */
  prsSignalVDAC0_CH1WARM          = PRS_VDAC0L_CH1WARM,         /**< VDAC0 channel 1 warmed Signal. */
  prsSignalVDAC0_CH0DONE          = PRS_VDAC0L_CH0DONEASYNC,    /**< VDAC0 channel 0 conversion done Signal. */
  prsSignalVDAC0_CH1DONE          = PRS_VDAC0L_CH1DONEASYNC,    /**< VDAC0 channel 1 conversion done Signal. */
  prsSignalVDAC0_INTERNALTIMEROF  = PRS_VDAC0L_INTERNALTIMEROF, /**< VDAC0 internal timer overflow Signal. */
  prsSignalVDAC0_REFRESHTIMEROF   = PRS_VDAC0L_REFRESHTIMEROF,  /**< VDAC0 internal timer overflow Signal. */
#endif
#if defined(PRS_VDAC0_OPA3)
  prsSignalVDAC0_CH0  = PRS_VDAC0_CH0,  /**< VDAC0_CH0  Signal. */
  prsSignalVDAC0_CH1  = PRS_VDAC0_CH1,  /**< VDAC0_CH1  Signal. */
  prsSignalVDAC0_OPA0 = PRS_VDAC0_OPA0, /**< VDAC0_OPA0 Signal. */
  prsSignalVDAC0_OPA1 = PRS_VDAC0_OPA1, /**< VDAC0_OPA1 Signal. */
  prsSignalVDAC0_OPA2 = PRS_VDAC0_OPA2, /**< VDAC0_OPA2 Signal. */
  prsSignalVDAC0_OPA3 = PRS_VDAC0_OPA3, /**< VDAC0_OPA3 Signal. */
#endif
#if defined(VDAC1) & (_SILICON_LABS_32B_SERIES >= 2)
  prsSignalVDAC1_CH0WARM          = PRS_VDAC1L_CH0WARM,         /**< VDAC1 channel 0 warmed Signal. */
  prsSignalVDAC1_CH1WARM          = PRS_VDAC1L_CH1WARM,         /**< VDAC1 channel 1 warmed Signal. */
  prsSignalVDAC1_CH0DONE          = PRS_VDAC1L_CH0DONEASYNC,    /**< VDAC1 channel 0 conversion done Signal. */
  prsSignalVDAC1_CH1DONE          = PRS_VDAC1L_CH1DONEASYNC,    /**< VDAC1 channel 1 conversion done Signal. */
  prsSignalVDAC1_INTERNALTIMEROF  = PRS_VDAC1L_INTERNALTIMEROF, /**< VDAC1 internal timer overflow Signal. */
  prsSignalVDAC1_REFRESHTIMEROF   = PRS_VDAC1L_REFRESHTIMEROF,  /**< VDAC1 internal timer overflow Signal. */
#endif

#if defined(PRS_LESENSE_SCANRES15)
  prsSignalLESENSE_SCANRES0  = PRS_LESENSE_SCANRES0,  /**< LESENSE_SCANRES0  Signal. */
  prsSignalLESENSE_SCANRES1  = PRS_LESENSE_SCANRES1,  /**< LESENSE_SCANRES1  Signal. */
  prsSignalLESENSE_SCANRES2  = PRS_LESENSE_SCANRES2,  /**< LESENSE_SCANRES2  Signal. */
  prsSignalLESENSE_SCANRES3  = PRS_LESENSE_SCANRES3,  /**< LESENSE_SCANRES3  Signal. */
  prsSignalLESENSE_SCANRES4  = PRS_LESENSE_SCANRES4,  /**< LESENSE_SCANRES4  Signal. */
  prsSignalLESENSE_SCANRES5  = PRS_LESENSE_SCANRES5,  /**< LESENSE_SCANRES5  Signal. */
  prsSignalLESENSE_SCANRES6  = PRS_LESENSE_SCANRES6,  /**< LESENSE_SCANRES6  Signal. */
  prsSignalLESENSE_SCANRES7  = PRS_LESENSE_SCANRES7,  /**< LESENSE_SCANRES7  Signal. */
  prsSignalLESENSE_SCANRES8  = PRS_LESENSE_SCANRES8,  /**< LESENSE_SCANRES8  Signal. */
  prsSignalLESENSE_SCANRES9  = PRS_LESENSE_SCANRES9,  /**< LESENSE_SCANRES9  Signal. */
  prsSignalLESENSE_SCANRES10 = PRS_LESENSE_SCANRES10, /**< LESENSE_SCANRES10 Signal. */
  prsSignalLESENSE_SCANRES11 = PRS_LESENSE_SCANRES11, /**< LESENSE_SCANRES11 Signal. */
  prsSignalLESENSE_SCANRES12 = PRS_LESENSE_SCANRES12, /**< LESENSE_SCANRES12 Signal. */
  prsSignalLESENSE_SCANRES13 = PRS_LESENSE_SCANRES13, /**< LESENSE_SCANRES13 Signal. */
  prsSignalLESENSE_SCANRES14 = PRS_LESENSE_SCANRES14, /**< LESENSE_SCANRES14 Signal. */
  prsSignalLESENSE_SCANRES15 = PRS_LESENSE_SCANRES15, /**< LESENSE_SCANRES15 Signal. */
#endif
#if defined(PRS_LESENSE_DEC2)
  prsSignalLESENSE_DEC0    = PRS_LESENSE_DEC0,    /**< LESENSE_DEC0    Signal. */
  prsSignalLESENSE_DEC1    = PRS_LESENSE_DEC1,    /**< LESENSE_DEC1    Signal. */
  prsSignalLESENSE_DEC2    = PRS_LESENSE_DEC2,    /**< LESENSE_DEC2    Signal. */
#endif
#if defined(PRS_LESENSE_DECOUT2)
  prsSignalLESENSE_DECOUT0    = PRS_LESENSE_DECOUT0,    /**< LESENSE_DECOUT0    Signal. */
  prsSignalLESENSE_DECOUT1    = PRS_LESENSE_DECOUT1,    /**< LESENSE_DECOUT1    Signal. */
  prsSignalLESENSE_DECOUT2    = PRS_LESENSE_DECOUT2,    /**< LESENSE_DECOUT2    Signal. */
#endif
#if defined(PRS_LESENSE_DECCMP)
  prsSignalLESENSE_DECCMP  = PRS_LESENSE_DECCMP,  /**< LESENSE_DECCMP  Signal. */
#endif
#if defined(PRS_LESENSE_MEASACT)
  prsSignalLESENSE_MEASACT = PRS_LESENSE_MEASACT, /**< LESENSE_MEASACT Signal. */
#endif

  /* USART Signals */
#if defined(USART0)
  prsSignalUSART0_TXC     = PRS_USART0_TXC,    /**< USART0 TX complete Signal. */
#if defined(PRS_USART0_RXDATA)
  prsSignalUSART0_RXDATA  = PRS_USART0_RXDATA, /**< USART0 RX data available Signal. */
#endif
#if defined(PRS_USART0_RXDATAV)
  prsSignalUSART0_RXDATAV = PRS_USART0_RXDATAV, /**< USART0 RX data available Signal. */
#endif
#if defined(PRS_USART0_IRTX)
  prsSignalUSART0_IRTX    = PRS_USART0_IRTX,    /**< USART0 IR TX Signal. */
#endif
#if defined(PRS_USART0_RTS)
  prsSignalUSART0_RTS     = PRS_USART0_RTS,    /**< USART0 RTS Signal. */
  prsSignalUSART0_TX      = PRS_USART0_TX,     /**< USART0 TX Signal. */
  prsSignalUSART0_CS      = PRS_USART0_CS,     /**< USART0 chip select Signal. */
#endif
#endif
#if defined(USART1)
  prsSignalUSART1_TXC     = PRS_USART1_TXC,    /**< USART1 TX complete Signal. */
#if defined(PRS_USART1_RXDATA)
  prsSignalUSART1_RXDATA  = PRS_USART1_RXDATA, /**< USART1 RX data available Signal. */
#endif
#if defined(PRS_USART1_RXDATAV)
  prsSignalUSART1_RXDATAV = PRS_USART1_RXDATAV, /**< USART1 RX data available Signal. */
#endif
#if defined(PRS_USART1_IRTX)
  prsSignalUSART1_IRTX    = PRS_USART1_IRTX,   /**< USART1 IR TX Signal. */
#endif
#if defined(PRS_USART1_RTS)
  prsSignalUSART1_RTS     = PRS_USART1_RTS,    /**< USART1 RTS Signal. */
  prsSignalUSART1_TX      = PRS_USART1_TX,     /**< USART1 TX Signal. */
  prsSignalUSART1_CS      = PRS_USART1_CS,     /**< USART1 chip select Signal. */
#endif
#endif
#if defined(USART2)
  prsSignalUSART2_TXC     = PRS_USART2_TXC,    /**< USART2 TX complete Signal. */
#if defined(PRS_USART2_RXDATA)
  prsSignalUSART2_RXDATA  = PRS_USART2_RXDATA, /**< USART2 RX data available Signal. */
#endif
#if defined(PRS_USART2_RXDATAV)
  prsSignalUSART2_RXDATAV = PRS_USART2_RXDATAV, /**< USART2 RX data available Signal. */
#endif
#if defined(PRS_USART2_IRTX)
  prsSignalUSART2_IRTX    = PRS_USART2_IRTX,   /**< USART2 IR TX Signal. */
#endif
#if defined(PRS_USART2_RTS)
  prsSignalUSART2_RTS     = PRS_USART2_RTS,    /**< USART2 RTS Signal. */
  prsSignalUSART2_TX      = PRS_USART2_TX,     /**< USART2 TX Signal. */
  prsSignalUSART2_CS      = PRS_USART2_CS,     /**< USART2 chip select Signal. */
#endif
#endif
#if defined(PRS_USART3_TXC)
  prsSignalUSART3_TXC     = PRS_USART3_TXC,       /**< USART3 TX complete Signal. */
  prsSignalUSART3_RXDATAV = PRS_USART3_RXDATAV,   /**< USART3 RX data available Signal. */
  prsSignalUSART3_RTS     = PRS_USART3_RTS,       /**< USART3 RTS Signal. */
  prsSignalUSART3_TX      = PRS_USART3_TX,        /**< USART3 TX Signal. */
  prsSignalUSART3_CS      = PRS_USART3_CS,        /**< USART3 chip select Signal. */
#endif
#if defined(PRS_USART4_TXC)
  prsSignalUSART4_TXC     = PRS_USART4_TXC,       /**< USART4 TX complete Signal. */
  prsSignalUSART4_RXDATAV = PRS_USART4_RXDATAV,   /**< USART4 RX data available Signal. */
  prsSignalUSART4_RTS     = PRS_USART4_RTS,       /**< USART4 RTS Signal. */
  prsSignalUSART4_TX      = PRS_USART4_TX,        /**< USART4 TX Signal. */
  prsSignalUSART4_CS      = PRS_USART4_CS,        /**< USART4 chip select Signal. */
#endif
#if defined(PRS_USART5_TXC)
  prsSignalUSART5_TXC     = PRS_USART5_TXC,       /**< USART5 TX complete Signal. */
  prsSignalUSART5_RXDATAV = PRS_USART5_RXDATAV,   /**< USART5 RX data available Signal. */
  prsSignalUSART5_RTS     = PRS_USART5_RTS,       /**< USART5 RTS Signal. */
  prsSignalUSART5_TX      = PRS_USART5_TX,        /**< USART5 TX Signal. */
  prsSignalUSART5_CS      = PRS_USART5_CS,        /**< USART5 chip select Signal. */
#endif

#if defined(UART0)
  prsSignalUART0_TXC     = PRS_UART0_TXC,       /**< UART0 TX complete Signal. */
  prsSignalUART0_RXDATAV = PRS_UART0_RXDATAV,   /**< UART0 RX data available Signal. */
#if defined(PRS_UART1_IRTX)
  prsSignalUART0_IRTX    = PRS_UART0_IRTX,      /**< UART0 IR TX Signal. */
#endif
#if defined(PRS_UART0_RTS)
  prsSignalUART0_RTS     = PRS_UART0_RTS,       /**< UART0 RTS Signal. */
  prsSignalUART0_TX      = PRS_UART0_TX,        /**< UART0 TX Signal. */
  prsSignalUART0_CS      = PRS_UART0_CS,        /**< UART0 chip select Signal. */
#endif
#endif

#if defined(UART1)
  prsSignalUART1_TXC     = PRS_UART1_TXC,       /**< UART1 TX complete Signal. */
  prsSignalUART1_RXDATAV = PRS_UART1_RXDATAV,   /**< UART1 RX data available Signal. */
#if defined(PRS_UART1_IRTX)
  prsSignalUART1_IRTX    = PRS_UART1_IRTX,      /**< UART1 IR RX Signal. */
#endif
#if defined(PRS_UART1_RTS)
  prsSignalUART1_RTS     = PRS_UART1_RTS,       /**< UART1 RTS Signal. */
  prsSignalUART1_TX      = PRS_UART1_TX,        /**< UART1 RX Signal. */
  prsSignalUART1_CS      = PRS_UART1_CS,        /**< UART1 chip select Signal. */
#endif
#endif

#if defined(PRS_USB_SOF)
  prsSignalUSB_SOF   = PRS_USB_SOF,     /**< USB_SOF   Signal. */
  prsSignalUSB_SOFSR = PRS_USB_SOFSR,   /**< USB_SOFSR Signal. */
#endif

#if defined(PRS_CM4_TXEV)
  prsSignalCM4_TXEV             = PRS_CM4_TXEV,               /**< TXEV             Signal. */
#endif
#if defined(PRS_CM4_ICACHEPCHITSOF)
  prsSignalCM4_ICACHEPCHITSOF   = PRS_CM4_ICACHEPCHITSOF,     /**< ICACHEPCHITSOF   Signal. */
  prsSignalCM4_ICACHEPCMISSESOF = PRS_CM4_ICACHEPCMISSESOF,   /**< ICACHEPCMISSESOF Signal. */
#endif

#if defined(PRS_WTIMER0_UF)
  prsSignalWTIMER0_UF  = PRS_WTIMER0_UF,    /**< WTIMER0_UF  Signal. */
  prsSignalWTIMER0_OF  = PRS_WTIMER0_OF,    /**< WTIMER0_OF  Signal. */
  prsSignalWTIMER0_CC0 = PRS_WTIMER0_CC0,   /**< WTIMER0_CC0 Signal. */
  prsSignalWTIMER0_CC1 = PRS_WTIMER0_CC1,   /**< WTIMER0_CC1 Signal. */
  prsSignalWTIMER0_CC2 = PRS_WTIMER0_CC2,   /**< WTIMER0_CC2 Signal. */
#endif
#if defined(PRS_WTIMER1_UF)
  prsSignalWTIMER1_UF  = PRS_WTIMER1_UF,    /**< WTIMER1_UF  Signal. */
  prsSignalWTIMER1_OF  = PRS_WTIMER1_OF,    /**< WTIMER1_OF  Signal. */
  prsSignalWTIMER1_CC0 = PRS_WTIMER1_CC0,   /**< WTIMER1_CC0 Signal. */
  prsSignalWTIMER1_CC1 = PRS_WTIMER1_CC1,   /**< WTIMER1_CC1 Signal. */
  prsSignalWTIMER1_CC2 = PRS_WTIMER1_CC2,   /**< WTIMER1_CC2 Signal. */
  prsSignalWTIMER1_CC3 = PRS_WTIMER1_CC3,   /**< WTIMER1_CC3 Signal. */
#endif
#if defined(PRS_WTIMER2_UF)
  prsSignalWTIMER2_UF  = PRS_WTIMER2_UF,    /**< WTIMER2_UF  Signal. */
  prsSignalWTIMER2_OF  = PRS_WTIMER2_OF,    /**< WTIMER2_OF  Signal. */
  prsSignalWTIMER2_CC0 = PRS_WTIMER2_CC0,   /**< WTIMER2_CC0 Signal. */
  prsSignalWTIMER2_CC1 = PRS_WTIMER2_CC1,   /**< WTIMER2_CC1 Signal. */
  prsSignalWTIMER2_CC2 = PRS_WTIMER2_CC2,   /**< WTIMER2_CC2 Signal. */
#endif
#if defined(PRS_WTIMER3_UF)
  prsSignalWTIMER3_UF  = PRS_WTIMER3_UF,    /**< WTIMER3_UF  Signal. */
  prsSignalWTIMER3_OF  = PRS_WTIMER3_OF,    /**< WTIMER3_OF  Signal. */
  prsSignalWTIMER3_CC0 = PRS_WTIMER3_CC0,   /**< WTIMER3_CC0 Signal. */
  prsSignalWTIMER3_CC1 = PRS_WTIMER3_CC1,   /**< WTIMER3_CC1 Signal. */
  prsSignalWTIMER3_CC2 = PRS_WTIMER3_CC2,   /**< WTIMER3_CC2 Signal. */
#endif

/* EUSART Signals */
#if defined(EUSART0)
  prsSignalEUSART0_CS         = PRS_EUSART0L_CS,         /**< EUSART0 chip select Signal. */
  prsSignalEUSART0_IRTX       = PRS_EUSART0L_IRDATX,     /**< EUSART0 IR RX Signal. */
  prsSignalEUSART0_RTS        = PRS_EUSART0L_RTS,        /**< EUSART0 RTS Signal. */
  prsSignalEUSART0_RXDATA     = PRS_EUSART0L_RXDATAV,    /**< EUSART0 RX data available Signal. */
  prsSignalEUSART0_TX         = PRS_EUSART0L_TX,         /**< EUSART0 TX Signal. */
  prsSignalEUSART0_TXC        = PRS_EUSART0L_TXC,        /**< EUSART0 TX complete Signal. */
  prsSignalEUSART0_RXFL       = PRS_EUSART0L_RXFL,       /**< EUSART0 rxfl Signal. */
  prsSignalEUSART0_TXFL       = PRS_EUSART0L_TXFL,       /**< EUSART0 txfl Signal. */
#endif
#if defined(EUSART1)
  prsSignalEUSART1_CS         = PRS_EUSART1L_CS,         /**< EUSART1 chip select Signal. */
  prsSignalEUSART1_IRTX       = PRS_EUSART1L_IRDATX,     /**< EUSART1 IR TX Signal. */
  prsSignalEUSART1_RTS        = PRS_EUSART1L_RTS,        /**< EUSART1 RTS Signal. */
  prsSignalEUSART1_RXDATA     = PRS_EUSART1L_RXDATAV,    /**< EUSART1 RX data available Signal. */
  prsSignalEUSART1_TX         = PRS_EUSART1L_TX,         /**< EUSART1 TX Signal. */
  prsSignalEUSART1_TXC        = PRS_EUSART1L_TXC,        /**< EUSART1 TX complete Signal. */
  prsSignalEUSART1_RXFL       = PRS_EUSART1L_RXFL,       /**< EUSART1 rxfl Signal. */
  prsSignalEUSART1_TXFL       = PRS_EUSART1L_TXFL,       /**< EUSART1 txfl Signal. */
#endif
#if defined(EUSART2)
  prsSignalEUSART2_CS         = PRS_EUSART2L_CS,         /**< EUSART2 chip select Signal. */
  prsSignalEUSART2_IRTX       = PRS_EUSART2L_IRDATX,     /**< EUSART2 IR TX Signal. */
  prsSignalEUSART2_RTS        = PRS_EUSART2L_RTS,        /**< EUSART2 RTS Signal. */
  prsSignalEUSART2_RXDATA     = PRS_EUSART2L_RXDATAV,    /**< EUSART2 RX data available Signal. */
  prsSignalEUSART2_TX         = PRS_EUSART2L_TX,         /**< EUSART2 TX Signal. */
  prsSignalEUSART2_TXC        = PRS_EUSART2L_TXC,        /**< EUSART2 TX complete Signal. */
  prsSignalEUSART2_RXFL       = PRS_EUSART2L_RXFL,       /**< EUSART2 rxfl Signal. */
  prsSignalEUSART2_TXFL       = PRS_EUSART2L_TXFL,       /**< EUSART2 txfl Signal. */
#endif
#if defined(EUSART3)
  prsSignalEUSART3_CS         = PRS_EUSART3L_CS,         /**< EUSART3 chip select Signal. */
  prsSignalEUSART3_IRTX       = PRS_EUSART3L_IRDATX,     /**< EUSART3 IR TX Signal. */
  prsSignalEUSART3_RTS        = PRS_EUSART3L_RTS,        /**< EUSART3 RTS Signal. */
  prsSignalEUSART3_RXDATA     = PRS_EUSART3L_RXDATAV,    /**< EUSART3 RX data available Signal. */
  prsSignalEUSART3_TX         = PRS_EUSART3L_TX,         /**< EUSART3 TX Signal. */
  prsSignalEUSART3_TXC        = PRS_EUSART3L_TXC,        /**< EUSART3 TX complete Signal. */
  prsSignalEUSART3_RXFL       = PRS_EUSART3L_RXFL,       /**< EUSART3 rxfl Signal. */
  prsSignalEUSART3_TXFL       = PRS_EUSART3L_TXFL,       /**< EUSART3 txfl Signal. */
#endif
#if defined(EUSART4)
  prsSignalEUSART4_CS         = PRS_EUSART4L_CS,         /**< EUSART4 chip select Signal. */
  prsSignalEUSART4_IRTX       = PRS_EUSART4L_IRDATX,     /**< EUSART4 IR TX Signal. */
  prsSignalEUSART4_RTS        = PRS_EUSART4L_RTS,        /**< EUSART4 RTS Signal. */
  prsSignalEUSART4_RXDATA     = PRS_EUSART4L_RXDATAV,    /**< EUSART4 RX data available Signal. */
  prsSignalEUSART4_TX         = PRS_EUSART4L_TX,         /**< EUSART4 TX Signal. */
  prsSignalEUSART4_TXC        = PRS_EUSART4L_TXC,        /**< EUSART4 TX complete Signal. */
  prsSignalEUSART4_RXFL       = PRS_EUSART4L_RXFL,       /**< EUSART4 rxfl Signal. */
  prsSignalEUSART4_TXFL       = PRS_EUSART4L_TXFL,       /**< EUSART4 txfl Signal. */
#endif
  /* ADC Signals */
#if defined(IADC0)
  prsSignalIADC0_SCANENTRY  = PRS_IADC0_SCANENTRYDONE, /**< IADC0 scan entry Signal. */
  prsSignalIADC0_SCANTABLE  = PRS_IADC0_SCANTABLEDONE, /**< IADC0 scan table Signal. */
  prsSignalIADC0_SINGLE     = PRS_IADC0_SINGLEDONE,    /**< IADC0 single Signal. */
#endif

  /* GPIO pin Signals */
  prsSignalGPIO_PIN0  = PRS_GPIO_PIN0,   /**< GPIO Pin 0 Signal. */
  prsSignalGPIO_PIN1  = PRS_GPIO_PIN1,   /**< GPIO Pin 1 Signal. */
  prsSignalGPIO_PIN2  = PRS_GPIO_PIN2,   /**< GPIO Pin 2 Signal. */
  prsSignalGPIO_PIN3  = PRS_GPIO_PIN3,   /**< GPIO Pin 3 Signal. */
  prsSignalGPIO_PIN4  = PRS_GPIO_PIN4,   /**< GPIO Pin 4 Signal. */
  prsSignalGPIO_PIN5  = PRS_GPIO_PIN5,   /**< GPIO Pin 5 Signal. */
  prsSignalGPIO_PIN6  = PRS_GPIO_PIN6,   /**< GPIO Pin 6 Signal. */
  prsSignalGPIO_PIN7  = PRS_GPIO_PIN7,   /**< GPIO Pin 7 Signal. */
#if defined(PRS_GPIO_PIN15)
  prsSignalGPIO_PIN8  = PRS_GPIO_PIN8,    /**< GPIO Pin 8 Signal. */
  prsSignalGPIO_PIN9  = PRS_GPIO_PIN9,    /**< GPIO Pin 9 Signal. */
  prsSignalGPIO_PIN10 = PRS_GPIO_PIN10,   /**< GPIO Pin 10 Signal. */
  prsSignalGPIO_PIN11 = PRS_GPIO_PIN11,   /**< GPIO Pin 11 Signal. */
  prsSignalGPIO_PIN12 = PRS_GPIO_PIN12,   /**< GPIO Pin 12 Signal. */
  prsSignalGPIO_PIN13 = PRS_GPIO_PIN13,   /**< GPIO Pin 13 Signal. */
  prsSignalGPIO_PIN14 = PRS_GPIO_PIN14,   /**< GPIO Pin 14 Signal. */
  prsSignalGPIO_PIN15 = PRS_GPIO_PIN15,   /**< GPIO Pin 15 Signal. */
#endif
#if defined(PRS_AGCL_CCA)
  prsSignalAGCL_CCA                  = PRS_AGCL_CCA,            /**< AGCL_CCA Signal. */
  prsSignalAGCL_CCAREQ               = PRS_AGCL_CCAREQ,         /**< AGCL_CCAREQ Signal. */
  prsSignalAGCL_GAINADJUST           = PRS_AGCL_GAINADJUST,     /**< AGCL_GAINADJUST Signal. */
  prsSignalAGCL_GAINOK               = PRS_AGCL_GAINOK,         /**< AGCL_GAINOK Signal. */
  prsSignalAGCL_GAINREDUCED          = PRS_AGCL_GAINREDUCED,    /**< AGCL_GAINREDUCED Signal. */
  prsSignalAGCL_IFPKI1               = PRS_AGCL_IFPKI1,         /**< AGCL_IFPKI1 Signal. */
  prsSignalAGCL_IFPKQ2               = PRS_AGCL_IFPKQ2,         /**< AGCL_IFPKQ2 Signal. */
  prsSignalAGCL_IFPKRST              = PRS_AGCL_IFPKRST,        /**< AGCL_IFPKRST Signal. */
#endif
#if defined(PRS_AGC_PEAKDET)
  prsSignalAGC_PEAKDET               = PRS_AGC_PEAKDET,         /**< AGC_PEAKDET Signal. */
  prsSignalAGC_PROPAGATED            = PRS_AGC_PROPAGATED,      /**< AGC_PROPAGATED Signal. */
  prsSignalAGC_RSSIDONE              = PRS_AGC_RSSIDONE,        /**< AGC_RSSIDONE Signal. */
#endif
#if defined(PRS_BUFC_THR0)
  prsSignalBUFC_THR0                 = PRS_BUFC_THR0,           /**< BUFC_THR0 Signal. */
  prsSignalBUFC_THR1                 = PRS_BUFC_THR1,           /**< BUFC_THR1 Signal. */
  prsSignalBUFC_THR2                 = PRS_BUFC_THR2,           /**< BUFC_THR2 Signal. */
  prsSignalBUFC_THR3                 = PRS_BUFC_THR3,           /**< BUFC_THR3 Signal. */
#endif
#if defined(PRS_BUFC_CNT0)
  prsSignalBUFC_CNT0                 = PRS_BUFC_CNT0,           /**< BUFC_CNT0 Signal. */
  prsSignalBUFC_CNT1                 = PRS_BUFC_CNT1,           /**< BUFC_CNT1 Signal. */
  prsSignalBUFC_FULL                 = PRS_BUFC_FULL,           /**< BUFC_FULL Signal. */
#endif
#if defined(PRS_MODEML_ADVANCE)
  prsSignalMODEML_ADVANCE            = PRS_MODEML_ADVANCE,      /**< MODEML_ADVANCE Signal. */
  prsSignalMODEML_ANT0               = PRS_MODEML_ANT0,         /**< MODEML_ANT0 Signal. */
  prsSignalMODEML_ANT1               = PRS_MODEML_ANT1,         /**< MODEML_ANT1 Signal. */
  prsSignalMODEML_COHDSADET          = PRS_MODEML_COHDSADET,    /**< MODEML_COHDSADET Signal. */
  prsSignalMODEML_COHDSALIVE         = PRS_MODEML_COHDSALIVE,   /**< MODEML_COHDSALIVE Signal. */
  prsSignalMODEML_DCLK               = PRS_MODEML_DCLK,         /**< MODEML_DCLK Signal. */
  prsSignalMODEML_DOUT               = PRS_MODEML_DOUT,         /**< MODEML_DOUT Signal. */
  prsSignalMODEML_FRAMEDET           = PRS_MODEML_FRAMEDET,     /**< MODEML_FRAMEDET Signal. */
#endif
#if defined(PRS_MODEM_FRAMEDET)
  prsSignalMODEM_FRAMEDET            = PRS_MODEM_FRAMEDET,       /**< MODEM_FRAMEDET Signal. */
  prsSignalMODEM_TIMDET              = PRS_MODEM_TIMDET,         /**< MODEM_TIMDET Signal. */
  prsSignalMODEM_SYNCSENT            = PRS_MODEM_SYNCSENT,       /**< MODEM_SYNCSENT Signal. */
  prsSignalMODEM_PRESENT             = PRS_MODEM_PRESENT,        /**< MODEM_PRESENT Signal. */
  prsSignalMODEM_ANT0                = PRS_MODEM_ANT0,           /**< MODEM_ANT0 Signal. */
  prsSignalMODEM_ANT1                = PRS_MODEM_ANT1,           /**< MODEM_ANT1 Signal. */
#endif
#if defined(PRS_MODEM_FRAMESENT)
  prsSignalMODEM_FRAMESENT           = PRS_MODEM_FRAMESENT,     /**< MODEM_FRAMESENT Signal. */
#endif
#if defined(PRS_MODEM_PREDET)
  prsSignalMODEM_PREDET              = PRS_MODEM_PREDET,         /**< MODEM_PREDET Signal. */
#endif
#if defined(PRS_MODEM_LRDSADET)
  prsSignalMODEM_LRDSADET            = PRS_MODEM_LRDSADET,      /**< MODEM_LRDSADET Signal. */
  prsSignalMODEM_LRDSALIVE           = PRS_MODEM_LRDSALIVE,     /**< MODEM_LRDSALIVE Signal. */
#endif
#if defined(PRS_MODEM_LOWCORR)
  prsSignalMODEM_LOWCORR             = PRS_MODEM_LOWCORR,       /**< MODEM_LOWCORR Signal. */
  prsSignalMODEM_NEWSYMBOL           = PRS_MODEM_NEWSYMBOL,     /**< MODEM_NEWSYMBOL Signal. */
  prsSignalMODEM_NEWWND              = PRS_MODEM_NEWWND,        /**< MODEM_NEWWND Signal. */
  prsSignalMODEM_POSTPONE            = PRS_MODEM_POSTPONE,      /**< MODEM_POSTPONE Signal. */
#endif
#if defined(PRS_MODEMH_PRESENT)
  prsSignalMODEMH_PRESENT            = PRS_MODEMH_PRESENT,      /**< MODEMH_PRESENT Signal. */
  prsSignalMODEMH_RSSIJUMP           = PRS_MODEMH_RSSIJUMP,     /**< MODEMH_RSSIJUMP Signal. */
  prsSignalMODEMH_SYNCSENT           = PRS_MODEMH_SYNCSENT,     /**< MODEMH_SYNCSENT Signal. */
  prsSignalMODEMH_TIMDET             = PRS_MODEMH_TIMDET,       /**< MODEMH_TIMDET Signal. */
  prsSignalMODEMH_WEAK               = PRS_MODEMH_WEAK,         /**< MODEMH_WEAK Signal. */
  prsSignalMODEMH_EOF                = PRS_MODEMH_EOF,          /**< MODEMH_EOF Signal. */
#endif
#if defined(PRS_MODEMH_SI)
  prsSignalMODEMH_SI                 = PRS_MODEMH_SI,           /**< MODEMH_SI Signal. */
#endif
#if defined(PRS_FRC_DCLK)
  prsSignalFRC_DCLK                  = PRS_FRC_DCLK,            /**< FRC_DCLK Signal. */
  prsSignalFRC_DOUT                  = PRS_FRC_DOUT,            /**< FRC_DOUT Signal. */
#endif
#if defined(PRS_PROTIMERL_BOF)
  prsSignalPROTIMERL_BOF             = PRS_PROTIMERL_BOF,       /**< PROTIMERL_BOF Signal. */
  prsSignalPROTIMERL_CC0             = PRS_PROTIMERL_CC0,       /**< PROTIMERL_CC0 Signal. */
  prsSignalPROTIMERL_CC1             = PRS_PROTIMERL_CC1,       /**< PROTIMERL_CC1 Signal. */
  prsSignalPROTIMERL_CC2             = PRS_PROTIMERL_CC2,       /**< PROTIMERL_CC2 Signal. */
  prsSignalPROTIMERL_CC3             = PRS_PROTIMERL_CC3,       /**< PROTIMERL_CC3 Signal. */
  prsSignalPROTIMERL_CC4             = PRS_PROTIMERL_CC4,       /**< PROTIMERL_CC4 Signal. */
  prsSignalPROTIMERL_LBTF            = PRS_PROTIMERL_LBTF,      /**< PROTIMERL_LBTF Signal. */
  prsSignalPROTIMERL_LBTR            = PRS_PROTIMERL_LBTR,      /**< PROTIMERL_LBTR Signal. */
#endif
#if defined(PRS_PROTIMER_LBTR)
  prsSignalPROTIMER_LBTR             = PRS_PROTIMER_LBTR,        /**< PROTIMER_LBTR Signal. */
  prsSignalPROTIMER_LBTF             = PRS_PROTIMER_LBTF,        /**< PROTIMER_LBTF Signal. */
#endif
#if defined(PRS_PROTIMER_LBTS)
  prsSignalPROTIMER_LBTS             = PRS_PROTIMER_LBTS,        /**< PROTIMER_LBTS Signal. */
#endif
#if defined(PRS_PROTIMER_POF)
  prsSignalPROTIMER_POF              = PRS_PROTIMER_POF,        /**< PROTIMER_POF Signal. */
  prsSignalPROTIMER_T0MATCH          = PRS_PROTIMER_T0MATCH,    /**< PROTIMER_T0MATCH Signal. */
  prsSignalPROTIMER_T0UF             = PRS_PROTIMER_T0UF,       /**< PROTIMER_T0UF Signal. */
  prsSignalPROTIMER_T1MATCH          = PRS_PROTIMER_T1MATCH,    /**< PROTIMER_T1MATCH Signal. */
  prsSignalPROTIMER_T1UF             = PRS_PROTIMER_T1UF,       /**< PROTIMER_T1UF Signal. */
  prsSignalPROTIMER_WOF              = PRS_PROTIMER_WOF,        /**< PROTIMER_WOF Signal. */
#endif
#if defined(PRS_RAC_ACTIVE)
  prsSignalRAC_ACTIVE                = PRS_RAC_ACTIVE,          /**< RAC_ACTIVE Signal. */
  prsSignalRAC_LNAEN                 = PRS_RAC_LNAEN,           /**< RAC_LNAEN Signal. */
  prsSignalRAC_PAEN                  = PRS_RAC_PAEN,            /**< RAC_PAEN Signal. */
  prsSignalRAC_RX                    = PRS_RAC_RX,              /**< RAC_RX Signal. */
  prsSignalRAC_TX                    = PRS_RAC_TX,              /**< RAC_TX Signal. */
#endif
#if defined(PRS_RACL_ACTIVE)
  prsSignalRACL_ACTIVE               = PRS_RACL_ACTIVE,          /**< RACL_ACTIVE Signal. */
  prsSignalRACL_LNAEN                = PRS_RACL_LNAEN,           /**< RACL_LNAEN Signal. */
  prsSignalRACL_PAEN                 = PRS_RACL_PAEN,            /**< RACL_PAEN Signal. */
  prsSignalRACL_RX                   = PRS_RACL_RX,              /**< RACL_RX Signal. */
  prsSignalRACL_TX                   = PRS_RACL_TX,              /**< RACL_TX Signal. */
  prsSignalRACL_CTIOUT0              = PRS_RACL_CTIOUT0,        /**< RACL_CTIOUT0 Signal. */
  prsSignalRACL_CTIOUT1              = PRS_RACL_CTIOUT1,        /**< RACL_CTIOUT1 Signal. */
  prsSignalRACL_CTIOUT2              = PRS_RACL_CTIOUT2,        /**< RACL_CTIOUT2 Signal. */
#endif
#if defined(PRS_RAC_CTIOUT3)
  prsSignalRAC_CTIOUT3               = PRS_RAC_CTIOUT3,         /**< RAC_CTIOUT3 Signal. */
#endif
#if defined(PRS_RAC_AUXADCDATA)
  prsSignalRAC_AUXADCDATA            = PRS_RAC_AUXADCDATA,      /**< RAC_AUXADCDATA Signal. */
  prsSignalRAC_AUXADCDATAVALID       = PRS_RAC_AUXADCDATAVALID, /**< RAC_AUXADCDATAVALID Signal. */
#endif
#if defined(PRS_SYNTH_MUX0)
  prsSignalSYNTH_MUX0                = PRS_SYNTH_MUX0,          /**< SYNTH_MUX0 Signal. */
  prsSignalSYNTH_MUX1                = PRS_SYNTH_MUX1,          /**< SYNTH_MUX1 Signal. */
#endif
#if defined(PRS_PRORTC_CCV0)
  prsSignalPRORTC_CCV0               = PRS_PRORTC_CCV0,         /**< PRORTC_CCV0 Signal. */
  prsSignalPRORTC_CCV1               = PRS_PRORTC_CCV1,         /**< PRORTC_CCV1 Signal. */
#endif
#if defined(PRS_LVGD_LVGDFALLDETECTED)
  prsSignalLVGD_LVGDFALLDETECTED     = PRS_LVGD_LVGDFALLDETECTED,    /**< LVGD_LVGDFALLDETECTED Signal. */
  prsSignalLVGD_LVGDFALLDETECTEDRAW  = PRS_LVGD_LVGDFALLDETECTEDRAW, /**< LVGD_LVGDFALLDETECTEDRAW Signal. */
  prsSignalLVGD_LVGDRISEDETECTED     = PRS_LVGD_LVGDRISEDETECTED,    /**< LVGD_LVGDRISEDETECTED Signal. */
  prsSignalLVGD_LVGDRISEDETECTEDRAW  = PRS_LVGD_LVGDRISEDETECTEDRAW, /**< LVGD_LVGDRISEDETECTEDRAW Signal. */
#endif
#if defined(PRS_SE_COREENGATED)
  prsSignalSE_COREENGATED            = PRS_SE_COREENGATED,  /**< SE_COREENGATED Signal. */
  prsSignalSE_STATE0GATED            = PRS_SE_STATE0GATED,  /**< SE_STATE0GATED Signal. */
  prsSignalSE_STATE1GATED            = PRS_SE_STATE1GATED,  /**< SE_STATE1GATED Signal. */
  prsSignalSE_STATE2GATED            = PRS_SE_STATE2GATED,  /**< SE_STATE2GATED Signal. */
#endif
#if defined(RFFPLL0)
  prsSignalRFFPLL0L_CLKDIGDIV4      = PRS_RFFPLL0L_CLKDIGDIV4,      /**< RFFPLL0L CLKDIGDIV4 Signal. */
  prsSignalRFFPLL0L_CLKMODEMDIV4    = PRS_RFFPLL0L_CLKMODEMDIV4,    /**< RFFPLL0L CLKMODEMDIV4 Signal. */
#if defined(PRS_RFFPLL0L_RFBIASFSMSTATE0)
  prsSignalRFFPLL0L_RFBIASFSMSTATE0 = PRS_RFFPLL0L_RFBIASFSMSTATE0, /**< RFFPLL0L RFBIASFSMSTATE0 Signal. */
  prsSignalRFFPLL0L_RFBIASFSMSTATE1 = PRS_RFFPLL0L_RFBIASFSMSTATE1, /**< RFFPLL0L RFBIASFSMSTATE1 Signal. */
  prsSignalRFFPLL0L_RFFPLLFSMSTATE0 = PRS_RFFPLL0L_RFFPLLFSMSTATE0, /**< RFFPLL0L RFFPLLFSMSTATE0 Signal. */
  prsSignalRFFPLL0L_RFFPLLFSMSTATE1 = PRS_RFFPLL0L_RFFPLLFSMSTATE1, /**< RFFPLL0L RFFPLLFSMSTATE1 Signal. */
  prsSignalRFFPLL0L_RFFPLLFSMSTATE2 = PRS_RFFPLL0L_RFFPLLFSMSTATE2, /**< RFFPLL0L RFFPLLFSMSTATE2 Signal. */
  prsSignalRFFPLL0L_RFBIASFSMSTATE2 = PRS_RFFPLL0L_RFBIASFSMSTATE2, /**< RFFPLL0L RFBIASFSMSTATE2 Signal. */
  prsSignalRFFPLL0_RFFPLLFSMSTATE3  = PRS_RFFPLL0_RFFPLLFSMSTATE3,  /**< RFFPLL0 RFFPLLFSMSTATE3 Signal. */
#endif
#endif
#if defined(FEFILT0)
  prsSignalFEFILT0_REMPTY = PRS_FEFILT0_REMPTY, /**< FEFILT0 REMPTY Signal. */
  prsSignalFEFILT0_WFULL  = PRS_FEFILT0_WFULL,  /**< FEFILT0 WFULL Signal. */
#endif
#if defined(FEFILT0)
  prsSignalFEFILT1_REMPTY = PRS_FEFILT1_REMPTY, /**< FEFILT1 REMPTY Signal. */
  prsSignalFEFILT1_WFULL  = PRS_FEFILT1_WFULL,  /**< FEFILT1 WFULL Signal. */
#endif
#if defined(ETAMPDET)
  prsSignalETAMPDET_TAMPERSRCETAMPDET = PRS_ETAMPDET_TAMPERSRCETAMPDET, /**< ETAMPDET TAMPERSRCETAMPDET Signal. */
#endif
#if defined(SMCTRL)
  prsSignalSMCTRLL_SOFTM0 = PRS_SMCTRLL_SOFTM0, /**< SMCTRLL SOFTM0 Signal. */
  prsSignalSMCTRLL_SOFTM1 = PRS_SMCTRLL_SOFTM1, /**< SMCTRLL SOFTM1 Signal. */
  prsSignalSMCTRLL_SOFTM2 = PRS_SMCTRLL_SOFTM2, /**< SMCTRLL SOFTM2 Signal. */
  prsSignalSMCTRLL_SOFTM3 = PRS_SMCTRLL_SOFTM3, /**< SMCTRLL SOFTM3 Signal. */
  prsSignalSMCTRLL_SOFTM4 = PRS_SMCTRLL_SOFTM4, /**< SMCTRLL SOFTM4 Signal. */
  prsSignalSMCTRLL_SOFTM5 = PRS_SMCTRLL_SOFTM5, /**< SMCTRLL SOFTM5 Signal. */
  prsSignalSMCTRLL_SOFTM6 = PRS_SMCTRLL_SOFTM6, /**< SMCTRLL SOFTM6 Signal. */
  prsSignalSMCTRLL_SOFTM7 = PRS_SMCTRLL_SOFTM7, /**< SMCTRLL SOFTM7 Signal. */
  prsSignalSMCTRL_SOFTM8  = PRS_SMCTRL_SOFTM8,  /**< SMCTRL SOFTM8 Signal. */
  prsSignalSMCTRL_SOFTM9  = PRS_SMCTRL_SOFTM9,  /**< SMCTRL SOFTM9 Signal. */
  prsSignalSMCTRL_SOFTM10 = PRS_SMCTRL_SOFTM10, /**< SMCTRL SOFTM10 Signal. */
  prsSignalSMCTRL_SOFTM11 = PRS_SMCTRL_SOFTM11, /**< SMCTRL SOFTM11 Signal. */
#endif
#if defined(PRS_SEHFRCO_COREEN)
  prsSignalSEHFRCO_COREEN = PRS_SEHFRCO_COREEN, /**< SEHFRCO COREEN Signal. */
  prsSignalSEHFRCO_STATE0 = PRS_SEHFRCO_STATE0, /**< SEHFRCO STATE0 Signal. */
  prsSignalSEHFRCO_STATE1 = PRS_SEHFRCO_STATE1, /**< SEHFRCO STATE1 Signal. */
  prsSignalSEHFRCO_STATE2 = PRS_SEHFRCO_STATE2, /**< SEHFRCO STATE2 Signal. */
#endif
#if defined(PRS_SEATAMPDET_SEATAMPDETSUPTPDELAY)
  prsSignalSEATAMPDET_SEATAMPDETSUPTPDELAY = PRS_SEATAMPDET_SEATAMPDETSUPTPDELAY, /**< SEATAMPDET SEATAMPDETSUPTPDELAY Signal. */
#endif
#if defined(PRS_DCDC_MONO70NSANA)
  prsSignalDCDC_MONO70NSANA = PRS_DCDC_MONO70NSANA, /** DCDC Pulses for Coulomb Counter Calibration Signal. */
#endif
#if defined(PRS_LFRCO_CALMEAS)
  prsSignalLFRCO_CALMEAS = PRS_LFRCO_CALMEAS, /** LFRCO Calibration Measure Signal. */
  prsSignalLFRCO_SDM     = PRS_LFRCO_SDM,     /** LFRCO Sigma Delta Modulator output Signal. */
  prsSignalLFRCO_TCMEAS  = PRS_LFRCO_TCMEAS,  /** LFRCO Temperature Check Measure Signal. */
#endif
} PRS_Signal_t;

#if defined(_SILICON_LABS_32B_SERIES_2)
/** PRS Consumers. */
typedef enum {
  prsConsumerNone                = 0x000,                                               /**< No PRS consumer */
  prsConsumerCMU_CALDN           = offsetof(PRS_TypeDef, CONSUMER_CMU_CALDN),           /**< CMU calibration down consumer. */
  prsConsumerCMU_CALUP           = offsetof(PRS_TypeDef, CONSUMER_CMU_CALUP),           /**< CMU calibration up consumer. */
#if defined(IADC_PRESENT)
  prsConsumerIADC0_SCANTRIGGER   = offsetof(PRS_TypeDef, CONSUMER_IADC0_SCANTRIGGER),   /**< IADC0 scan trigger consumer. */
  prsConsumerIADC0_SINGLETRIGGER = offsetof(PRS_TypeDef, CONSUMER_IADC0_SINGLETRIGGER), /**< IADC0 single trigger consumer. */
#endif
  prsConsumerLDMA_REQUEST0       = offsetof(PRS_TypeDef, CONSUMER_LDMAXBAR_DMAREQ0),    /**< LDMA Request 0 consumer. */
  prsConsumerLDMA_REQUEST1       = offsetof(PRS_TypeDef, CONSUMER_LDMAXBAR_DMAREQ1),    /**< LDMA Request 1 consumer. */
#if defined(LETIMER0)
  prsConsumerLETIMER0_CLEAR      = offsetof(PRS_TypeDef, CONSUMER_LETIMER0_CLEAR),      /**< LETIMER0 clear consumer. */
  prsConsumerLETIMER0_START      = offsetof(PRS_TypeDef, CONSUMER_LETIMER0_START),      /**< LETIMER0 start consumer. */
  prsConsumerLETIMER0_STOP       = offsetof(PRS_TypeDef, CONSUMER_LETIMER0_STOP),       /**< LETIMER0 stop consumer. */
#endif
  prsConsumerTIMER0_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER0_CC0),          /**< TIMER0 capture/compare channel 0 consumer. */
  prsConsumerTIMER0_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER0_CC1),          /**< TIMER0 capture/compare channel 1 consumer. */
  prsConsumerTIMER0_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER0_CC2),          /**< TIMER0 capture/compare channel 2 consumer. */
  prsConsumerTIMER1_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER1_CC0),          /**< TIMER1 capture/compare channel 0 consumer. */
  prsConsumerTIMER1_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER1_CC1),          /**< TIMER1 capture/compare channel 1 consumer. */
  prsConsumerTIMER1_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER1_CC2),          /**< TIMER1 capture/compare channel 2 consumer. */
  prsConsumerTIMER2_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER2_CC0),          /**< TIMER2 capture/compare channel 0 consumer. */
  prsConsumerTIMER2_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER2_CC1),          /**< TIMER2 capture/compare channel 1 consumer. */
  prsConsumerTIMER2_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER2_CC2),          /**< TIMER2 capture/compare channel 2 consumer. */
  prsConsumerTIMER3_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER3_CC0),          /**< TIMER3 capture/compare channel 0 consumer. */
  prsConsumerTIMER3_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER3_CC1),          /**< TIMER3 capture/compare channel 1 consumer. */
  prsConsumerTIMER3_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER3_CC2),          /**< TIMER3 capture/compare channel 2 consumer. */
#if defined(TIMER4)
  prsConsumerTIMER4_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER4_CC0),          /**< TIMER4 capture/compare channel 0 consumer. */
  prsConsumerTIMER4_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER4_CC1),          /**< TIMER4 capture/compare channel 1 consumer. */
  prsConsumerTIMER4_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER4_CC2),          /**< TIMER4 capture/compare channel 2 consumer. */
#endif
#if defined(TIMER5)
  prsConsumerTIMER5_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER5_CC0),          /**< TIMER5 capture/compare channel 0 consumer. */
  prsConsumerTIMER5_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER5_CC1),          /**< TIMER5 capture/compare channel 1 consumer. */
  prsConsumerTIMER5_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER5_CC2),          /**< TIMER5 capture/compare channel 2 consumer. */
#endif
#if defined(TIMER6)
  prsConsumerTIMER6_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER6_CC0),          /**< TIMER6 capture/compare channel 0 consumer. */
  prsConsumerTIMER6_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER6_CC1),          /**< TIMER6 capture/compare channel 1 consumer. */
  prsConsumerTIMER6_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER6_CC2),          /**< TIMER6 capture/compare channel 2 consumer. */
#endif
#if defined(TIMER7)
  prsConsumerTIMER7_CC0          = offsetof(PRS_TypeDef, CONSUMER_TIMER7_CC0),          /**< TIMER7 capture/compare channel 0 consumer. */
  prsConsumerTIMER7_CC1          = offsetof(PRS_TypeDef, CONSUMER_TIMER7_CC1),          /**< TIMER7 capture/compare channel 1 consumer. */
  prsConsumerTIMER7_CC2          = offsetof(PRS_TypeDef, CONSUMER_TIMER7_CC2),          /**< TIMER7 capture/compare channel 2 consumer. */
#endif
#if defined(USART0)
  prsConsumerUSART0_CLK          = offsetof(PRS_TypeDef, CONSUMER_USART0_CLK),          /**< USART0 clock consumer. */
  prsConsumerUSART0_IR           = offsetof(PRS_TypeDef, CONSUMER_USART0_IR),           /**< USART0 IR consumer. */
  prsConsumerUSART0_RX           = offsetof(PRS_TypeDef, CONSUMER_USART0_RX),           /**< USART0 RX consumer. */
  prsConsumerUSART0_TRIGGER      = offsetof(PRS_TypeDef, CONSUMER_USART0_TRIGGER),      /**< USART0 trigger consumer. */
#endif
#if defined(USART1)
  prsConsumerUSART1_CLK          = offsetof(PRS_TypeDef, CONSUMER_USART1_CLK),          /**< USART1 clock consumer. */
  prsConsumerUSART1_IR           = offsetof(PRS_TypeDef, CONSUMER_USART1_IR),           /**< USART1 IR consumer. */
  prsConsumerUSART1_RX           = offsetof(PRS_TypeDef, CONSUMER_USART1_RX),           /**< USART1 TX consumer. */
  prsConsumerUSART1_TRIGGER      = offsetof(PRS_TypeDef, CONSUMER_USART1_TRIGGER),      /**< USART1 trigger consumer. */
#endif
#if defined(USART2)
  prsConsumerUSART2_CLK          = offsetof(PRS_TypeDef, CONSUMER_USART2_CLK),          /**< USART2 clock consumer. */
  prsConsumerUSART2_IR           = offsetof(PRS_TypeDef, CONSUMER_USART2_IR),           /**< USART2 IR consumer. */
  prsConsumerUSART2_RX           = offsetof(PRS_TypeDef, CONSUMER_USART2_RX),           /**< USART2 RX consumer. */
  prsConsumerUSART2_TRIGGER      = offsetof(PRS_TypeDef, CONSUMER_USART2_TRIGGER),      /**< USART2 trigger consumer. */
#endif
#if defined(EUSART0)
  prsConsumerEUSART0_CLK         = offsetof(PRS_TypeDef, CONSUMER_EUSART0_CLK),         /**< EUSART0 clk consumer. */
  prsConsumerEUSART0_RX          = offsetof(PRS_TypeDef, CONSUMER_EUSART0_RX),          /**< EUSART0 RX consumer. */
  prsConsumerEUSART0_TRIGGER     = offsetof(PRS_TypeDef, CONSUMER_EUSART0_TRIGGER),     /**< EUSART0 trigger consumer. */
#endif
#if defined(EUSART1)
  prsConsumerEUSART1_CLK         = offsetof(PRS_TypeDef, CONSUMER_EUSART1_CLK),         /**< EUSART1 clk consumer. */
  prsConsumerEUSART1_RX          = offsetof(PRS_TypeDef, CONSUMER_EUSART1_RX),          /**< EUSART1 RX consumer. */
  prsConsumerEUSART1_TRIGGER     = offsetof(PRS_TypeDef, CONSUMER_EUSART1_TRIGGER),     /**< EUSART1 trigger consumer. */
#endif
#if defined(EUSART2)
  prsConsumerEUSART2_CLK         = offsetof(PRS_TypeDef, CONSUMER_EUSART2_CLK),         /**< EUSART1 clk consumer. */
  prsConsumerEUSART2_RX          = offsetof(PRS_TypeDef, CONSUMER_EUSART2_RX),          /**< EUSART2 RX consumer. */
  prsConsumerEUSART2_TRIGGER     = offsetof(PRS_TypeDef, CONSUMER_EUSART2_TRIGGER),     /**< EUSART2 trigger consumer. */
#endif
#if defined(EUSART3)
  prsConsumerEUSART3_RX          = offsetof(PRS_TypeDef, CONSUMER_EUSART3_RX),             /**< EUSART3 RX consumer. */
  prsConsumerEUSART3_TRIGGER     = offsetof(PRS_TypeDef, CONSUMER_EUSART3_TRIGGER),        /**< EUSART3 trigger consumer. */
#endif
#if defined(EUSART4)
  prsConsumerEUSART4_RX          = offsetof(PRS_TypeDef, CONSUMER_EUSART4_RX),             /**< EUSART4 RX consumer. */
  prsConsumerEUSART4_TRIGGER     = offsetof(PRS_TypeDef, CONSUMER_EUSART4_TRIGGER),        /**< EUSART4 trigger consumer. */
#endif
#if defined(EUART0)
  prsConsumerEUART0_RX           = offsetof(PRS_TypeDef, CONSUMER_EUART0_RX),           /**< EUART0 RX consumer. */
  prsConsumerEUART0_TRIGGER      = offsetof(PRS_TypeDef, CONSUMER_EUART0_TRIGGER),      /**< EUART0 TRIGGER Consumer. */
#endif
  prsConsumerWDOG0_SRC0          = offsetof(PRS_TypeDef, CONSUMER_WDOG0_SRC0),          /**< WDOG0 source 0 consumer. */
  prsConsumerWDOG0_SRC1          = offsetof(PRS_TypeDef, CONSUMER_WDOG0_SRC1),          /**< WDOG0 source 1 consumer. */
#if defined(WDOG1)
  prsConsumerWDOG1_SRC0          = offsetof(PRS_TypeDef, CONSUMER_WDOG1_SRC0),          /**< WDOG1 source 0 consumer. */
  prsConsumerWDOG1_SRC1          = offsetof(PRS_TypeDef, CONSUMER_WDOG1_SRC1),          /**< WDOG1 source 1 consumer. */
#endif
#if defined(PCNT0)
  prsConsumerPCNT0_IN0           = offsetof(PRS_TypeDef, CONSUMER_PCNT0_S0IN),          /**< PCNT0 input 0 consumer. */
  prsConsumerPCNT0_IN1           = offsetof(PRS_TypeDef, CONSUMER_PCNT0_S1IN),          /**< PCNT0 input 1 consumer. */
#endif
#if defined(_PRS_CONSUMER_RTCC_CC2_MASK)
  prsConsumerRTCC_CC0            = offsetof(PRS_TypeDef, CONSUMER_RTCC_CC0),            /**< RTCC capture/compare channel 0 consumer. */
  prsConsumerRTCC_CC1            = offsetof(PRS_TypeDef, CONSUMER_RTCC_CC1),            /**< RTCC capture/compare channel 1 consumer. */
  prsConsumerRTCC_CC2            = offsetof(PRS_TypeDef, CONSUMER_RTCC_CC2),            /**< RTCC capture/compare channel 2 consumer. */
#endif
#if defined(SYSRTC0)
  prsConsumerSYSRTC0_SRC0        = offsetof(PRS_TypeDef, CONSUMER_SYSRTC0_IN0),         /**< SYSRTC0 input 0 consumer. */
  prsConsumerSYSRTC0_SRC1        = offsetof(PRS_TypeDef, CONSUMER_SYSRTC0_IN1),         /**< SYSRTC0 input 1 consumer. */
#endif
#if defined(_PRS_CONSUMER_HFXO0_OSCREQ_MASK)
  prsConsumerHFXO0_OSCREQ        = offsetof(PRS_TypeDef, CONSUMER_HFXO0_OSCREQ),        /**< OSCREQ consumer. */
  prsConsumerHFXO0_TIMEOUT       = offsetof(PRS_TypeDef, CONSUMER_HFXO0_TIMEOUT),       /**< HFXO0_TIMEOUT consumer. */
#endif
#if defined(LESENSE)
  prsConsumerLESENSE_START       = offsetof(PRS_TypeDef, CONSUMER_LESENSE_START),       /**< LESENSE_START consumer. */
#endif
#if defined(VDAC0)
  prsConsumerVDAC0_ASYNCTRIGCH0  = offsetof(PRS_TypeDef, CONSUMER_VDAC0_ASYNCTRIGCH0),  /**< VDAC0 ASYNC TRIGER CH0 consumer. */
  prsConsumerVDAC0_ASYNCTRIGCH1  = offsetof(PRS_TypeDef, CONSUMER_VDAC0_ASYNCTRIGCH1),  /**< VDAC0 ASYNC TRIGER CH1 consumer. */
  prsConsumerVDAC0_SYNCTRIGCH0   = offsetof(PRS_TypeDef, CONSUMER_VDAC0_SYNCTRIGCH0),   /**< VDAC0 SYNC TRIGER CH0 consumer. */
  prsConsumerVDAC0_SYNCTRIGCH1   = offsetof(PRS_TypeDef, CONSUMER_VDAC0_SYNCTRIGCH1),   /**< VDAC0 SYNC TRIGER CH1 consumer. */
#endif
#if defined(VDAC1)
  prsConsumerVDAC1_ASYNCTRIGCH0  = offsetof(PRS_TypeDef, CONSUMER_VDAC1_ASYNCTRIGCH0),  /**< VDAC1 ASYNC TRIGER CH0 consumer. */
  prsConsumerVDAC1_ASYNCTRIGCH1  = offsetof(PRS_TypeDef, CONSUMER_VDAC1_ASYNCTRIGCH1),  /**< VDAC1 ASYNC TRIGER CH1 consumer. */
  prsConsumerVDAC1_SYNCTRIGCH0   = offsetof(PRS_TypeDef, CONSUMER_VDAC1_SYNCTRIGCH0),   /**< VDAC1 SYNC TRIGER CH0 consumer. */
  prsConsumerVDAC1_SYNCTRIGCH1   = offsetof(PRS_TypeDef, CONSUMER_VDAC1_SYNCTRIGCH1),   /**< VDAC1 SYNC TRIGER CH1 consumer. */
#endif
} PRS_Consumer_t;
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Set level control bit for one or more channels.
 *
 * @details
 *   The level value for a channel is XORed with both the pulse possibly issued
 *   by PRS_PulseTrigger() and the PRS input signal selected for the channel(s).
 *
 * @cond DOXYDOC_S2_DEVICE
 * @note
 *   Note that software level control is only available for asynchronous
 *   channels on Series 2 devices.
 * @endcond
 *
 * @param[in] level
 *   Level to use for channels indicated by @p mask. Use logical OR combination
 *   of PRS_SWLEVEL_CHnLEVEL defines for channels to set high level, otherwise 0.
 *
 * @param[in] mask
 *   Mask indicating which channels to set level for. Use logical OR combination
 *   of PRS_SWLEVEL_CHnLEVEL defines.
 ******************************************************************************/
__STATIC_INLINE void PRS_LevelSet(uint32_t level, uint32_t mask)
{
#if defined(_PRS_SWLEVEL_MASK)
  PRS->SWLEVEL = (PRS->SWLEVEL & ~mask) | (level & mask);
#else
  PRS->ASYNC_SWLEVEL = (PRS->ASYNC_SWLEVEL & ~mask) | (level & mask);
#endif
}

/***************************************************************************//**
 * @brief
 *   Get level control bit for all channels.
 *
 * @return
 *   The current software level configuration.
 ******************************************************************************/
__STATIC_INLINE uint32_t PRS_LevelGet(void)
{
#if defined(_PRS_SWLEVEL_MASK)
  return PRS->SWLEVEL;
#else
  return PRS->ASYNC_SWLEVEL;
#endif
}

#if defined(_PRS_ASYNC_PEEK_MASK) || defined(_PRS_PEEK_MASK)
/***************************************************************************//**
 * @brief
 *   Get the PRS channel values for all channels.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @return
 *   The current PRS channel output values for all channels as a bitset.
 ******************************************************************************/
__STATIC_INLINE uint32_t PRS_Values(PRS_ChType_t type)
{
#if defined(_PRS_ASYNC_PEEK_MASK)
  if (type == prsTypeAsync) {
    return PRS->ASYNC_PEEK;
  } else {
    return PRS->SYNC_PEEK;
  }
#else
  (void) type;
  return PRS->PEEK;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get the PRS channel value for a single channel.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @return
 *   The current PRS channel output value. This is either 0 or 1.
 ******************************************************************************/
__STATIC_INLINE bool PRS_ChannelValue(unsigned int ch, PRS_ChType_t type)
{
  return (PRS_Values(type) >> ch) & 0x1U;
}
#endif

/***************************************************************************//**
 * @brief
 *   Trigger a high pulse (one HFPERCLK) for one or more channels.
 *
 * @details
 *   Setting a bit for a channel causes the bit in the register to remain high
 *   for one HFPERCLK cycle. Pulse is XORed with both the corresponding bit
 *   in PRS SWLEVEL register and the PRS input signal selected for the
 *   channel(s).
 *
 * @param[in] channels
 *   Logical ORed combination of channels to trigger a pulse for. Use
 *   PRS_SWPULSE_CHnPULSE defines.
 ******************************************************************************/
__STATIC_INLINE void PRS_PulseTrigger(uint32_t channels)
{
#if defined(_PRS_SWPULSE_MASK)
  PRS->SWPULSE = channels & _PRS_SWPULSE_MASK;
#else
  PRS->ASYNC_SWPULSE = channels & _PRS_ASYNC_SWPULSE_MASK;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set the PRS channel level for one asynchronous PRS channel.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] level
 *   true to set the level high (1) and false to set the level low (0).
 ******************************************************************************/
__STATIC_INLINE void PRS_ChannelLevelSet(unsigned int ch, bool level)
{
  PRS_LevelSet((uint32_t) level << ch, 0x1UL << ch);
}

/***************************************************************************//**
 * @brief
 *   Trigger a pulse on one PRS channel.
 *
 * @param[in] ch
 *   PRS channel number.
 ******************************************************************************/
__STATIC_INLINE void PRS_ChannelPulse(unsigned int ch)
{
  PRS_PulseTrigger(0x1UL << ch);
}

void PRS_SourceSignalSet(unsigned int ch,
                         uint32_t source,
                         uint32_t signal,
                         PRS_Edge_TypeDef edge);

#if defined(PRS_ASYNC_SUPPORTED)
void PRS_SourceAsyncSignalSet(unsigned int ch,
                              uint32_t source,
                              uint32_t signal);
#endif
#if defined(_PRS_ROUTELOC0_MASK) || (_PRS_ROUTE_MASK)
void PRS_GpioOutputLocation(unsigned int ch,
                            unsigned int location);
#endif

int PRS_GetFreeChannel(PRS_ChType_t type);
void PRS_Reset(void);
void PRS_ConnectSignal(unsigned int ch, PRS_ChType_t type, PRS_Signal_t signal);
#if defined(_SILICON_LABS_32B_SERIES_2)
uint32_t PRS_ConvertToSyncSource(uint32_t asyncSource);
uint32_t PRS_ConvertToSyncSignal(uint32_t asyncSource, uint32_t asyncSignal);
void PRS_ConnectConsumer(unsigned int ch, PRS_ChType_t type, PRS_Consumer_t consumer);
void PRS_PinOutput(unsigned int ch, PRS_ChType_t type, GPIO_Port_TypeDef port, uint8_t pin);
void PRS_Combine(unsigned int chA, unsigned int chB, PRS_Logic_t logic);
#endif

/** @} (end addtogroup prs) */

#ifdef __cplusplus
}
#endif

#endif /* defined(PRS_COUNT) && (PRS_COUNT > 0) */
#endif /* EM_PRS_H */
