/***************************************************************************//**
 * @file
 * @brief Pulse Counter (PCNT) peripheral API
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

#ifndef EM_PCNT_H
#define EM_PCNT_H

#include "em_device.h"
#if defined(PCNT_COUNT) && (PCNT_COUNT > 0)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup pcnt
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
/** PCNT0 Counter register size. */
#if defined(_EFM32_GECKO_FAMILY)
#define PCNT0_CNT_SIZE    (8)   /**< PCNT0 counter is  8 bits. */
#else
#define PCNT0_CNT_SIZE   (16)   /**< PCNT0 counter is 16 bits. */
#endif

#ifdef PCNT1
/** PCNT1 Counter register size. */
#if defined(_SILICON_LABS_32B_SERIES_0)
#define PCNT1_CNT_SIZE    (8)   /**< PCNT1 counter is  8 bits. */
#else
#define PCNT1_CNT_SIZE   (16)   /**< PCNT1 counter is  16 bits. */
#endif
#endif

#ifdef PCNT2
/** PCNT2 Counter register size. */
#if defined(_SILICON_LABS_32B_SERIES_0)
#define PCNT2_CNT_SIZE    (8)   /**< PCNT2 counter is  8 bits. */
#else
#define PCNT2_CNT_SIZE   (16)   /**< PCNT2 counter is  16 bits. */
#endif
#endif

/* Define values that can be used in case some state/mode are not defined for some devices.*/
/** PCNT mode disable. */
#define PCNT_MODE_DISABLE   0xFF
/** PCNT count event is none. */
#define PCNT_CNT_EVENT_NONE  0xFF

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** Mode selection. */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
typedef enum {
  /** Disable pulse counter. */
  pcntModeDisable   = _PCNT_CTRL_MODE_DISABLE,

  /** Single input LFACLK oversampling mode (available in EM0-EM2). */
  pcntModeOvsSingle = _PCNT_CTRL_MODE_OVSSINGLE,

  /** Externally clocked single input counter mode (available in EM0-EM3). */
  pcntModeExtSingle = _PCNT_CTRL_MODE_EXTCLKSINGLE,

  /** Externally clocked quadrature decoder mode (available in EM0-EM3). */
  pcntModeExtQuad   = _PCNT_CTRL_MODE_EXTCLKQUAD,

#if defined(_PCNT_CTRL_MODE_OVSQUAD1X)
  /** LFACLK oversampling quadrature decoder 1X mode (available in EM0-EM2). */
  pcntModeOvsQuad1  = _PCNT_CTRL_MODE_OVSQUAD1X,

  /** LFACLK oversampling quadrature decoder 2X mode (available in EM0-EM2). */
  pcntModeOvsQuad2  = _PCNT_CTRL_MODE_OVSQUAD2X,

  /** LFACLK oversampling quadrature decoder 4X mode (available in EM0-EM2). */
  pcntModeOvsQuad4  = _PCNT_CTRL_MODE_OVSQUAD4X,
#endif
} PCNT_Mode_TypeDef;

#else
typedef enum {
  /** Disable pulse counter. */
  pcntModeDisable   = PCNT_MODE_DISABLE,

  /** Single input LFACLK oversampling mode (available in EM0-EM2). */
  pcntModeOvsSingle = _PCNT_CFG_MODE_OVSSINGLE,

  /** Externally clocked single input counter mode (available in EM0-EM3). */
  pcntModeExtSingle = _PCNT_CFG_MODE_EXTCLKSINGLE,

  /** Externally clocked quadrature decoder mode (available in EM0-EM3). */
  pcntModeExtQuad   = _PCNT_CFG_MODE_EXTCLKQUAD,

  /** LFACLK oversampling quadrature decoder 1X mode (available in EM0-EM2). */
  pcntModeOvsQuad1  = _PCNT_CFG_MODE_OVSQUAD1X,

  /** LFACLK oversampling quadrature decoder 2X mode (available in EM0-EM2). */
  pcntModeOvsQuad2  = _PCNT_CFG_MODE_OVSQUAD2X,

  /** LFACLK oversampling quadrature decoder 4X mode (available in EM0-EM2). */
  pcntModeOvsQuad4  = _PCNT_CFG_MODE_OVSQUAD4X,
} PCNT_Mode_TypeDef;
#endif

#if defined(_PCNT_CTRL_CNTEV_MASK)
/** Counter event selection.
 *  Note: unshifted values are being used for enumeration because multiple
 *  configuration structure members use this type definition. */
typedef enum {
  /** Counts up on up-count and down on down-count events. */
  pcntCntEventBoth = _PCNT_CTRL_CNTEV_BOTH,

  /** Only counts up on up-count events. */
  pcntCntEventUp   = _PCNT_CTRL_CNTEV_UP,

  /** Only counts down on down-count events. */
  pcntCntEventDown = _PCNT_CTRL_CNTEV_DOWN,

  /** Never counts. */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  pcntCntEventNone = _PCNT_CTRL_CNTEV_NONE
#else
  pcntCntEventNone = PCNT_CNT_EVENT_NONE
#endif
} PCNT_CntEvent_TypeDef;
#endif

/** PRS sources for @p s0PRS and @p s1PRS. */
#if defined(_PCNT_INPUT_MASK)
typedef enum {
  pcntPRSCh0 = 0,     /**< PRS channel 0. */
  pcntPRSCh1 = 1,     /**< PRS channel 1. */
  pcntPRSCh2 = 2,     /**< PRS channel 2. */
  pcntPRSCh3 = 3,     /**< PRS channel 3. */
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH4)
  pcntPRSCh4 = 4,     /**< PRS channel 4. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH5)
  pcntPRSCh5 = 5,     /**< PRS channel 5. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH6)
  pcntPRSCh6 = 6,     /**< PRS channel 6. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH7)
  pcntPRSCh7 = 7,     /**< PRS channel 7. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH8)
  pcntPRSCh8 = 8,     /**< PRS channel 8. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH9)
  pcntPRSCh9 = 9,     /**< PRS channel 9. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH10)
  pcntPRSCh10 = 10,   /**< PRS channel 10. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH11)
  pcntPRSCh11 = 11,   /**< PRS channel 11. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH12)
  pcntPRSCh12 = 12,   /**< PRS channel 12. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH13)
  pcntPRSCh13 = 13,   /**< PRS channel 13. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH14)
  pcntPRSCh14 = 14,   /**< PRS channel 14. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH15)
  pcntPRSCh15 = 15,   /**< PRS channel 15. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH16)
  pcntPRSCh16 = 16,   /**< PRS channel 16. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH17)
  pcntPRSCh17 = 17,   /**< PRS channel 17. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH18)
  pcntPRSCh18 = 18,   /**< PRS channel 18. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH19)
  pcntPRSCh19 = 19,   /**< PRS channel 19. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH20)
  pcntPRSCh20 = 20,   /**< PRS channel 20. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH21)
  pcntPRSCh21 = 21,   /**< PRS channel 21. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH22)
  pcntPRSCh22 = 22,   /**< PRS channel 22. */
#endif
#if defined(PCNT_INPUT_S0PRSSEL_PRSCH23)
  pcntPRSCh23 = 23,   /**< PRS channel 23. */
#endif
} PCNT_PRSSel_TypeDef;
#elif defined(_SILICON_LABS_32B_SERIES_2)
typedef unsigned int PCNT_PRSSel_TypeDef;
#endif

#if defined(_PCNT_INPUT_MASK) || defined(_SILICON_LABS_32B_SERIES_2)
/** PRS inputs of PCNT. */
typedef enum {
  pcntPRSInputS0 = 0, /** PRS input 0. */
  pcntPRSInputS1 = 1  /** PRS input 1. */
} PCNT_PRSInput_TypeDef;
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** Initialization structure. */
typedef struct {
  /** Mode to operate in. */
  PCNT_Mode_TypeDef     mode;

  /** Initial counter value (refer to reference manual for max value allowed).
   * Only used for #pcntModeOvsSingle (and possibly #pcntModeDisable) modes.
   * If using #pcntModeExtSingle or #pcntModeExtQuad modes, counter
   * value is reset to HW reset value. */
  uint32_t              counter;

  /** Initial top value (refer to reference manual for max value allowed).
   * Only used for #pcntModeOvsSingle (and possibly #pcntModeDisable) modes.
   * If using #pcntModeExtSingle or #pcntModeExtQuad modes, top
   * value is reset to HW reset value. */
  uint32_t              top;

  /** Polarity of incoming edge.
   * @li #pcntModeExtSingle mode - if false, positive edges are counted,
   *   otherwise negative edges.
   * @li #pcntModeExtQuad mode - if true, counting direction is inverted. */
  bool                  negEdge;

  /** Counting direction, only applicable for #pcntModeOvsSingle and
   * #pcntModeExtSingle modes. */
  bool                  countDown;

  /** Enable filter, only available in #pcntModeOvsSingle* mode. */
  bool                  filter;

#if defined(_SILICON_LABS_32B_SERIES_2)
  /** Enable/disable PCNT counting during debug halt. Only in OVSSINGLE and OVSQUAD modes. */
  bool                  debugHalt;
#endif

#if defined(PCNT_CTRL_HYST) || defined(_SILICON_LABS_32B_SERIES_2)
  /** Set to true to enable hysteresis. When enabled, PCNT will always
   *  overflow and underflow to TOP/2. */
  bool                  hyst;
#endif

#if defined(PCNT_CTRL_S1CDIR)
  /** Set to true to enable S1 to determine the direction of counting in
   *  OVSSINGLE or EXTCLKSINGLE modes. @n
   *  When S1 is high, the count direction is given by CNTDIR, and when S1 is
   *  low, the count direction is the opposite. */
  bool                  s1CntDir;
#endif

#if defined(_PCNT_CTRL_CNTEV_SHIFT)
  /** Selects whether the regular counter responds to up-count events,
   *  down-count events, both, or none. */
  PCNT_CntEvent_TypeDef cntEvent;
#endif

#if defined(_PCNT_CTRL_AUXCNTEV_SHIFT)
  /** Selects whether the auxiliary counter responds to up-count events,
   *  down-count events, both, or none. */
  PCNT_CntEvent_TypeDef auxCntEvent;
#endif

#if defined(_PCNT_INPUT_MASK) || defined(_SILICON_LABS_32B_SERIES_2)
  /** Select PRS channel as input to S0IN in PCNTx_INPUT register. */
  PCNT_PRSSel_TypeDef   s0PRS;

  /** Select PRS channel as input to S1IN in PCNTx_INPUT register. */
  PCNT_PRSSel_TypeDef   s1PRS;
#endif
} PCNT_Init_TypeDef;

/** Default Debug. */
#if defined(_SILICON_LABS_32B_SERIES_2)
#define DEFAULT_DEBUG_HALT  true,
#else
#define DEFAULT_DEBUG_HALT
#endif

/** Default Mode. */
#define DEFAULT_MODE    pcntModeDisable,         /**< Disabled by default. */

/** Default Hysteresis. */
#if defined(PCNT_CTRL_HYST) || defined(_SILICON_LABS_32B_SERIES_2)
#define DEFAULT_HYST    false,                   /**< Hysteresis disabled. */
#else
#define DEFAULT_HYST
#endif

/** Default counter direction*/
#if defined(PCNT_CTRL_S1CDIR)
#define DEFAULT_CDIR    true,                    /**< Counter direction is given by CNTDIR. */
#else
#define DEFAULT_CDIR
#endif

/** Default count event*/
#if defined(_PCNT_CTRL_CNTEV_SHIFT)
#define DEFAULT_CNTEV    pcntCntEventUp,         /**< Regular counter counts up on upcount events. */
#else
#define DEFAULT_CNTEV
#endif

/** Default auxiliary count event. */
#if defined(_PCNT_CTRL_AUXCNTEV_SHIFT)
#define DEFAULT_AUXCNTEV    pcntCntEventNone,    /**< Auxiliary counter doesn't respond to events. */
#else
#define DEFAULT_AUXCNTEV
#endif

/** Default selected PRS channel as S0IN and S1IN. */
#if defined(_PCNT_INPUT_MASK)
#define DEFAULT_PRS_CH      pcntPRSCh0,          /**< PRS channel 0 selected as S0IN and as S1IN. */
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define DEFAULT_PRS_CH      0u,
#else
#define DEFAULT_PRS_CH
#endif

/** Default configuration for PCNT initialization structure. */
#define PCNT_INIT_DEFAULT                                                       \
  {                                                                             \
    DEFAULT_MODE                          /* Default mode. */                   \
      _PCNT_CNT_RESETVALUE,               /* Default counter HW reset value. */ \
      _PCNT_TOP_RESETVALUE,               /* Default counter HW reset value. */ \
      false,                              /* Use positive edge. */              \
      false,                              /* Up-counting. */                    \
      false,                              /* Filter disabled. */                \
    DEFAULT_DEBUG_HALT                    /* Debug Halt enabled. */             \
    DEFAULT_HYST                          /* Default Hysteresis. */             \
    DEFAULT_CDIR                          /* Default CNTDIR. */                 \
    DEFAULT_CNTEV                         /* Faults CNTEV. */                   \
    DEFAULT_AUXCNTEV                      /* Default AUXCNTEV. */               \
    DEFAULT_PRS_CH                        /* PRS channel 0 selected as S0IN. */ \
      DEFAULT_PRS_CH                      /* PRS channel 0 selected as S1IN. */ \
  }

#if defined(PCNT_OVSCFG_FILTLEN_DEFAULT) || defined(_SILICON_LABS_32B_SERIES_2)
/** Filter initialization structure */
typedef struct {
  /** Used only in OVSINGLE and OVSQUAD1X-4X modes. To use this, enable filter by
   *  setting filter to true during PCNT_Init(). Filter length = (filtLen + 5) LFACLK cycles. */
  uint8_t               filtLen;

  /** When set, removes flutter from Quaddecoder inputs S0IN and S1IN.
   *  Available only in OVSQUAD1X-4X modes. */
  bool                  flutterrm;
} PCNT_Filter_TypeDef;
#endif

/** Default configuration for PCNT initialization structure. */
#if defined(PCNT_OVSCFG_FILTLEN_DEFAULT) || defined(_SILICON_LABS_32B_SERIES_2)
#define PCNT_FILTER_DEFAULT                                                          \
  {                                                                                  \
    0,                                      /* Default length is 5 LFACLK cycles. */ \
    false                                   /* No flutter removal. */                \
  }
#endif

#if defined(PCNT_CTRL_TCCMODE_DEFAULT)

/** Modes for Triggered Compare and Clear module. */
typedef enum {
  /** Triggered compare and clear not enabled. */
  tccModeDisabled       = _PCNT_CTRL_TCCMODE_DISABLED,

  /** Compare and clear performed on each (optionally prescaled) LFA clock cycle. */
  tccModeLFA            = _PCNT_CTRL_TCCMODE_LFA,

  /** Compare and clear performed on PRS edges. Polarity defined by prsPolarity. */
  tccModePRS            = _PCNT_CTRL_TCCMODE_PRS
} PCNT_TCCMode_TypeDef;

/** Prescaler values for LFA compare and clear events. Only has effect when TCC mode is LFA. */
typedef enum {
  /** Compare and clear event each LFA cycle. */
  tccPrescDiv1          = _PCNT_CTRL_TCCPRESC_DIV1,

  /** Compare and clear event every other LFA cycle. */
  tccPrescDiv2          = _PCNT_CTRL_TCCPRESC_DIV2,

  /** Compare and clear event every 4th LFA cycle. */
  tccPrescDiv4          = _PCNT_CTRL_TCCPRESC_DIV4,

  /** Compare and clear event every 8th LFA cycle. */
  tccPrescDiv8          = _PCNT_CTRL_TCCPRESC_DIV8
} PCNT_TCCPresc_Typedef;

/** Compare modes for TCC module. */
typedef enum {
  /** Compare match if PCNT_CNT is less than, or equal to PCNT_TOP. */
  tccCompLTOE           = _PCNT_CTRL_TCCCOMP_LTOE,

  /** Compare match if PCNT_CNT is greater than or equal to PCNT_TOP. */
  tccCompGTOE           = _PCNT_CTRL_TCCCOMP_GTOE,

  /** Compare match if PCNT_CNT is less than, or equal to PCNT_TOP[15:8]], and greater
   *  than, or equal to PCNT_TOP[7:0]. */
  tccCompRange          = _PCNT_CTRL_TCCCOMP_RANGE
} PCNT_TCCComp_Typedef;

/** TCC initialization structure. */
typedef struct {
  /** Mode to operate in. */
  PCNT_TCCMode_TypeDef      mode;

  /** Prescaler value for LFACLK in LFA mode. */
  PCNT_TCCPresc_Typedef     prescaler;

  /** Choose the event that will trigger a clear. */
  PCNT_TCCComp_Typedef      compare;

  /** PRS input to TCC module, either for gating the PCNT clock, triggering the TCC comparison, or both. */
  PCNT_PRSSel_TypeDef       tccPRS;

  /** TCC PRS input polarity. @n
   *  False = Rising edge for comparison trigger, and PCNT clock gated when PRS signal is high. @n
   *  True = Falling edge for comparison trigger, and PCNT clock gated when PRS signal is low. */
  bool                      prsPolarity;

  /** Enable gating PCNT input clock through TCC PRS signal.
   *  Polarity selection is done through prsPolarity. */
  bool                      prsGateEnable;
} PCNT_TCC_TypeDef;

/** TCC Default. */
#define PCNT_TCC_DEFAULT                                                                            \
  {                                                                                                 \
    tccModeDisabled,                            /* Disabled by default. */                          \
    tccPrescDiv1,                               /* Do not prescale LFA clock in LFA mode. */        \
    tccCompLTOE,                                /* Clear when CNT <= TOP. */                        \
    pcntPRSCh0,                                 /* Select PRS channel 0 as input to TCC. */         \
    false,                                      /* PRS polarity is rising edge, and gate when 1. */ \
    false                                       /* Do not gate PCNT counter input. */               \
  }

#endif
/* defined(PCNT_CTRL_TCCMODE_DEFAULT) */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get the pulse counter value.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @return
 *   Current pulse counter value.
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_CounterGet(PCNT_TypeDef *pcnt)
{
  return pcnt->CNT;
}

#if defined(_PCNT_AUXCNT_MASK)
/***************************************************************************//**
 * @brief
 *   Get the auxiliary counter value.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @return
 *   Current auxiliary counter value.
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_AuxCounterGet(PCNT_TypeDef *pcnt)
{
  return pcnt->AUXCNT;
}
#endif

void PCNT_CounterReset(PCNT_TypeDef *pcnt);
void PCNT_CounterTopSet(PCNT_TypeDef *pcnt, uint32_t count, uint32_t top);

/***************************************************************************//**
 * @brief
 *   Set a counter value.
 *
 * @details
 *   Pulse counter is disabled while changing counter value and re-enabled
 *   (if originally enabled) when counter value has been set.
 *
 * @note
 *   This function will stall until synchronization to low-frequency domain is
 *   completed. For that reason, it should normally not be used when using
 *   an external clock to clock the PCNT module since stall time may be
 *   undefined in that case. The counter should normally only be set when
 *   operating in (or about to enable) #pcntModeOvsSingle mode.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @param[in] count
 *   Value to set in counter register.
 ******************************************************************************/
__STATIC_INLINE void PCNT_CounterSet(PCNT_TypeDef *pcnt, uint32_t count)
{
  PCNT_CounterTopSet(pcnt, count, pcnt->TOP);
}

void PCNT_Enable(PCNT_TypeDef *pcnt, PCNT_Mode_TypeDef mode);
bool PCNT_IsEnabled(PCNT_TypeDef *pcnt);
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
void PCNT_FreezeEnable(PCNT_TypeDef *pcnt, bool enable);
#endif
void PCNT_Init(PCNT_TypeDef *pcnt, const PCNT_Init_TypeDef *init);

#if defined(PCNT_OVSCFG_FILTLEN_DEFAULT) || defined(_SILICON_LABS_32B_SERIES_2)
void PCNT_FilterConfiguration(PCNT_TypeDef *pcnt, const PCNT_Filter_TypeDef *config, bool enable);
#endif

#if defined(_PCNT_INPUT_MASK) || defined(_SILICON_LABS_32B_SERIES_2)
void PCNT_PRSInputEnable(PCNT_TypeDef *pcnt,
                         PCNT_PRSInput_TypeDef prsInput,
                         bool enable);
#endif

#if defined(PCNT_CTRL_TCCMODE_DEFAULT)
void PCNT_TCCConfiguration(PCNT_TypeDef *pcnt, const PCNT_TCC_TypeDef *config);
#endif

/***************************************************************************//**
 * @brief
 *   Clear one or more pending PCNT interrupts.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @param[in] flags
 *   Pending PCNT interrupt source to clear. Use a bitwise logic OR combination
 *   of valid interrupt flags for the PCNT module (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void PCNT_IntClear(PCNT_TypeDef *pcnt, uint32_t flags)
{
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  pcnt->IFC = flags;
#else
  pcnt->IF_CLR = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more PCNT interrupts.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @param[in] flags
 *   PCNT interrupt sources to disable. Use a bitwise logic OR combination of
 *   valid interrupt flags for PCNT module (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void PCNT_IntDisable(PCNT_TypeDef *pcnt, uint32_t flags)
{
#if defined(PCNT_HAS_SET_CLEAR)
  pcnt->IEN_CLR = flags;
#else
  pcnt->IEN &= ~flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable one or more PCNT interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   PCNT_IntClear() prior to enabling the interrupt.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @param[in] flags
 *   PCNT interrupt sources to enable. Use a bitwise logic OR combination of
 *   valid interrupt flags for PCNT module (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void PCNT_IntEnable(PCNT_TypeDef *pcnt, uint32_t flags)
{
#if defined(PCNT_HAS_SET_CLEAR)
  pcnt->IEN_SET = flags;
#else
  pcnt->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get pending PCNT interrupt flags.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @return
 *   PCNT interrupt sources pending. A bitwise logic OR combination of valid
 *   interrupt flags for PCNT module (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_IntGet(PCNT_TypeDef *pcnt)
{
  return pcnt->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending PCNT interrupt flags.
 *
 * @details
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @param[in] pcnt
 *   Pointer to thePCNT peripheral register block.
 *
 * @return
 *   Pending and enabled PCNT interrupt sources.
 *   The return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in PCNT_IEN_nnn
 *   register (PCNT_IEN_nnn) and
 *   - the OR combination of valid interrupt flags of the PCNT module
 *   (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_IntGetEnabled(PCNT_TypeDef *pcnt)
{
  uint32_t ien;

  /* Store pcnt->IEN in temporary variable in order to define explicit order
   * of volatile accesses. */
  ien = pcnt->IEN;

  /* Bitwise AND of pending and enabled interrupts. */
  return pcnt->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending PCNT interrupts from SW.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @param[in] flags
 *   PCNT interrupt sources to set to pending. Use a bitwise logic OR combination
 *   of valid interrupt flags for PCNT module (PCNT_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void PCNT_IntSet(PCNT_TypeDef *pcnt, uint32_t flags)
{
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  pcnt->IFS = flags;
#else
  pcnt->IF_SET = flags;
#endif
}

#if defined(_PCNT_LOCK_MASK)
/***************************************************************************//**
 * @brief
 *   Lock PCNT registers.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @note When PCNT registers are locked PCNT_CFG, PCNT_EN, PCNT_SWRST, PCNT_CMD,
 *       PCNT_CTRL, PCNT_OVSCTRL, PCNT_CNT, PCNT_TOP, and PCNT_TOPB registers
 *       cannot be written to.
 ******************************************************************************/
__STATIC_INLINE void PCNT_Lock(PCNT_TypeDef *pcnt)
{
  pcnt->LOCK = ~PCNT_LOCK_PCNTLOCKKEY_UNLOCK;
}
#endif

#if defined(_PCNT_LOCK_MASK)
/***************************************************************************//**
 * @brief
 *   Unlock PCNT registers.
 *
 * @param[in] pcnt
 *   Pointer to thePCNT peripheral register block.
 ******************************************************************************/
__STATIC_INLINE void PCNT_Unlock(PCNT_TypeDef *pcnt)
{
  pcnt->LOCK = PCNT_LOCK_PCNTLOCKKEY_UNLOCK;
}
#endif

void PCNT_Reset(PCNT_TypeDef *pcnt);

/***************************************************************************//**
 * @brief
 *   Get the pulse counter top buffer value.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @return
 *   Current pulse counter top buffer value.
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_TopBufferGet(PCNT_TypeDef *pcnt)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  while (pcnt->SYNCBUSY & PCNT_SYNCBUSY_TOPB) {
  }
#endif
  return pcnt->TOPB;
}

void PCNT_TopBufferSet(PCNT_TypeDef *pcnt, uint32_t val);

/***************************************************************************//**
 * @brief
 *   Get the pulse counter top value.
 *
 * @param[in] pcnt
 *   Pointer to the PCNT peripheral register block.
 *
 * @return
 *   Current pulse counter top value.
 ******************************************************************************/
__STATIC_INLINE uint32_t PCNT_TopGet(PCNT_TypeDef *pcnt)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  while (pcnt->SYNCBUSY & PCNT_SYNCBUSY_TOP) {
  }
#endif
  return pcnt->TOP;
}

void PCNT_TopSet(PCNT_TypeDef *pcnt, uint32_t val);

/***************************************************************************//**
 * @brief
 *   Wait for an ongoing sync of register(s) to low-frequency domain to complete.
 *
 * @param[in] pcnt
 *   A pointer to the PCNT peripheral register block.
 *
 * @param[in] mask
 *   A bitmask corresponding to SYNCBUSY register defined bits indicating
 *   registers that must complete any ongoing synchronization.
 ******************************************************************************/
__STATIC_INLINE void PCNT_Sync(PCNT_TypeDef *pcnt, uint32_t mask)
{
  /* Avoid deadlock if modifying the same register twice when freeze mode is
   * activated. */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  if (pcnt->FREEZE & PCNT_FREEZE_REGFREEZE) {
    return;
  }
#endif

  /* Wait for any pending previous write operation to have been completed in
   * low-frequency domain. */
  while (pcnt->SYNCBUSY & mask) {
  }
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Start the main PCNT counter.
 *
 * @details
 *   This function will send a start command to the PCNT peripheral. The PCNT
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The @ref PCNT_Sync() function can be used to wait for the start command
 *   to be executed.
 *
 * @param[in] pcnt
 *   A pointer to the PCNT peripheral register block.
 *
 * @note
 *   This function requires the PCNT to be enabled.
 ******************************************************************************/
__STATIC_INLINE void PCNT_StartMainCnt(PCNT_TypeDef *pcnt)
{
  PCNT_Sync(pcnt, PCNT_SYNCBUSY_CMD);
  pcnt->CMD_SET = PCNT_CMD_STARTCNT;
}

/***************************************************************************//**
 * @brief
 *   Stop the main PCNT counter.
 *
 * @details
 *   This function will send a stop command to the PCNT peripheral. The PCNT
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The @ref PCNT_Sync() function can be used to wait for the stop command
 *   to be executed.
 *
 * @param[in] pcnt
 *   A pointer to the PCNT peripheral register block.
 *
 * @note
 *   This function requires the PCNT to be enabled.
 ******************************************************************************/
__STATIC_INLINE void PCNT_StopMainCnt(PCNT_TypeDef *pcnt)
{
  PCNT_Sync(pcnt, PCNT_SYNCBUSY_CMD);
  pcnt->CMD_SET = PCNT_CMD_STOPCNT;
}

/***************************************************************************//**
 * @brief
 *   Start the auxiliary PCNT counter.
 *
 * @details
 *   This function will send a start command to the PCNT peripheral. The PCNT
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The @ref PCNT_Sync() function can be used to wait for the start command
 *   to be executed.
 *
 * @param[in] pcnt
 *   A pointer to the PCNT peripheral register block.
 *
 * @note
 *   This function requires the PCNT to be enabled.
 ******************************************************************************/
__STATIC_INLINE void PCNT_StartAuxCnt(PCNT_TypeDef *pcnt)
{
  PCNT_Sync(pcnt, PCNT_SYNCBUSY_CMD);
  pcnt->CMD_SET = PCNT_CMD_STARTAUXCNT;
}

/***************************************************************************//**
 * @brief
 *   Stop the auxiliary PCNT counter.
 *
 * @details
 *   This function will send a stop command to the PCNT peripheral. The PCNT
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The @ref PCNT_Sync() function can be used to wait for the stop command
 *   to be executed.
 *
 * @param[in] pcnt
 *   A pointer to the PCNT peripheral register block.
 *
 * @note
 *   This function requires the PCNT to be enabled.
 ******************************************************************************/
__STATIC_INLINE void PCNT_StopAuxCnt(PCNT_TypeDef *pcnt)
{
  PCNT_Sync(pcnt, PCNT_SYNCBUSY_CMD);
  pcnt->CMD_SET = PCNT_CMD_STOPAUXCNT;
}
#endif

/** @} (end addtogroup pcnt) */

#ifdef __cplusplus
}
#endif

#endif /* defined(PCNT_COUNT) && (PCNT_COUNT > 0) */
#endif /* EM_PCNT_H */
