/***************************************************************************//**
 * @file
 * @brief Digital to Analog Converter (VDAC) peripheral API
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

#ifndef EM_VDAC_H
#define EM_VDAC_H

#include "em_device.h"

#if defined(VDAC_COUNT) && (VDAC_COUNT > 0)

#include "sl_assert.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup vdac VDAC - Voltage DAC
 * @brief Digital to Analog Voltage Converter (VDAC) Peripheral API
 *
 * @details
 *  This module contains functions to control the VDAC peripheral of Silicon
 *  Labs' 32-bit MCUs and SoCs. VDAC converts digital values to analog
 *  signals at up to 500 ksps with 12-bit accuracy. VDAC is designed for
 *  low energy consumption, but can also provide very good performance.
 *
 *  The following steps are necessary for basic operation:
 *
 *  Clock enable:
 *  @code
    CMU_ClockEnable(cmuClock_VDAC0, true);@endcode
 *
 *  Initialize the VDAC with default settings and modify selected fields:
 *  @code
    VDAC_Init_TypeDef vdacInit          = VDAC_INIT_DEFAULT;
    VDAC_InitChannel_TypeDef vdacChInit = VDAC_INITCHANNEL_DEFAULT;

    // Set prescaler to get 1 MHz VDAC clock frequency.
    vdacInit.prescaler = VDAC_PrescaleCalc(1000000, true, 0); // function call for series 0/1
    VDAC_Init(VDAC0, &vdacInit);

    vdacChInit.enable = true;
    VDAC_InitChannel(VDAC0, &vdacChInit, 0);@endcode
 *
 *  Perform a conversion:
 *  @code
    VDAC_ChannelOutputSet(VDAC0, 0, 250);@endcode
 *
 * @note The output stage of a VDAC channel consists of an on-chip operational
 *   amplifier (OPAMP) in the OPAMP module. This OPAMP is highly configurable;
 *   and to exploit the VDAC functionality fully, configure the OPAMP using
 *   the OPAMP API. Using the OPAMP API also loads OPAMP calibration values.
 *   The default (reset) settings of OPAMP is sufficient for many applications.
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Validation of VDAC register block pointer reference for assert statements.*/

#if VDAC_COUNT == 1
#define VDAC_REF_VALID(ref)   ((ref) == VDAC0)
#elif VDAC_COUNT == 2
#define VDAC_REF_VALID(ref)   (((ref) == VDAC0) || ((ref) == VDAC1))
#else
#error "Undefined number of VDACs."
#endif

/** @endcond */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

#if !defined(_SILICON_LABS_32B_SERIES_2)
/** Channel refresh period. */
typedef enum {
  vdacRefresh8  = _VDAC_CTRL_REFRESHPERIOD_8CYCLES,  /**< Refresh every 8 clock cycles. */
  vdacRefresh16 = _VDAC_CTRL_REFRESHPERIOD_16CYCLES, /**< Refresh every 16 clock cycles. */
  vdacRefresh32 = _VDAC_CTRL_REFRESHPERIOD_32CYCLES, /**< Refresh every 32 clock cycles. */
  vdacRefresh64 = _VDAC_CTRL_REFRESHPERIOD_64CYCLES, /**< Refresh every 64 clock cycles. */
} VDAC_Refresh_TypeDef;

/** Reference voltage for VDAC. */
typedef enum {
  vdacRef1V25Ln = _VDAC_CTRL_REFSEL_1V25LN, /**< Internal low noise 1.25 V band gap reference. */
  vdacRef2V5Ln  = _VDAC_CTRL_REFSEL_2V5LN,  /**< Internal low noise 2.5 V band gap reference. */
  vdacRef1V25   = _VDAC_CTRL_REFSEL_1V25,   /**< Internal 1.25 V band gap reference. */
  vdacRef2V5    = _VDAC_CTRL_REFSEL_2V5,    /**< Internal 2.5 V band gap reference. */
  vdacRefAvdd   = _VDAC_CTRL_REFSEL_VDD,    /**< AVDD reference. */
  vdacRefExtPin = _VDAC_CTRL_REFSEL_EXT,    /**< External pin reference. */
} VDAC_Ref_TypeDef;

/** Peripheral Reflex System signal used to trigger VDAC channel conversion. */
typedef enum {
  vdacPrsSelCh0 =  _VDAC_CH0CTRL_PRSSEL_PRSCH0,  /**< PRS ch 0 triggers conversion. */
  vdacPrsSelCh1 =  _VDAC_CH0CTRL_PRSSEL_PRSCH1,  /**< PRS ch 1 triggers conversion. */
  vdacPrsSelCh2 =  _VDAC_CH0CTRL_PRSSEL_PRSCH2,  /**< PRS ch 2 triggers conversion. */
  vdacPrsSelCh3 =  _VDAC_CH0CTRL_PRSSEL_PRSCH3,  /**< PRS ch 3 triggers conversion. */
  vdacPrsSelCh4 =  _VDAC_CH0CTRL_PRSSEL_PRSCH4,  /**< PRS ch 4 triggers conversion. */
  vdacPrsSelCh5 =  _VDAC_CH0CTRL_PRSSEL_PRSCH5,  /**< PRS ch 5 triggers conversion. */
  vdacPrsSelCh6 =  _VDAC_CH0CTRL_PRSSEL_PRSCH6,  /**< PRS ch 6 triggers conversion. */
  vdacPrsSelCh7 =  _VDAC_CH0CTRL_PRSSEL_PRSCH7,  /**< PRS ch 7 triggers conversion. */
#if defined(_VDAC_CH0CTRL_PRSSEL_PRSCH8)
  vdacPrsSelCh8 =  _VDAC_CH0CTRL_PRSSEL_PRSCH8,  /**< PRS ch 8 triggers conversion. */
#endif
#if defined(_VDAC_CH0CTRL_PRSSEL_PRSCH9)
  vdacPrsSelCh9 =  _VDAC_CH0CTRL_PRSSEL_PRSCH9,  /**< PRS ch 9 triggers conversion. */
#endif
#if defined(_VDAC_CH0CTRL_PRSSEL_PRSCH10)
  vdacPrsSelCh10 = _VDAC_CH0CTRL_PRSSEL_PRSCH10, /**< PRS ch 10 triggers conversion. */
#endif
#if defined(_VDAC_CH0CTRL_PRSSEL_PRSCH11)
  vdacPrsSelCh11 = _VDAC_CH0CTRL_PRSSEL_PRSCH11, /**< PRS ch 11 triggers conversion. */
#endif
} VDAC_PrsSel_TypeDef;

/** Channel conversion trigger mode. */
typedef enum {
  vdacTrigModeSw        = _VDAC_CH0CTRL_TRIGMODE_SW,        /**< Channel is triggered by CHnDATA or COMBDATA write. */
  vdacTrigModePrs       = _VDAC_CH0CTRL_TRIGMODE_PRS,       /**< Channel is triggered by PRS input. */
  vdacTrigModeRefresh   = _VDAC_CH0CTRL_TRIGMODE_REFRESH,   /**< Channel is triggered by Refresh timer. */
  vdacTrigModeSwPrs     = _VDAC_CH0CTRL_TRIGMODE_SWPRS,     /**< Channel is triggered by CHnDATA/COMBDATA write or PRS input. */
  vdacTrigModeSwRefresh = _VDAC_CH0CTRL_TRIGMODE_SWREFRESH, /**< Channel is triggered by CHnDATA/COMBDATA write or Refresh timer. */
  vdacTrigModeLesense   = _VDAC_CH0CTRL_TRIGMODE_LESENSE,   /**< Channel is triggered by LESENSE. */
} VDAC_TrigMode_TypeDef;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** VDAC initialization structure, common for both channels. */
typedef struct {
  /** Selects between main and alternate output path calibration values. */
  bool                  mainCalibration;

  /** Selects clock from asynchronous or synchronous (with respect to
      peripheral clock) source. */
  bool                  asyncClockMode;

  /** Warm-up mode, keep VDAC on (in idle) - or shutdown between conversions.*/
  bool                  warmupKeepOn;

  /** Channel refresh period. */
  VDAC_Refresh_TypeDef  refresh;

  /** Prescaler for VDAC clock. Clock is source clock divided by prescaler+1. */
  uint32_t              prescaler;

  /** Reference voltage to use. */
  VDAC_Ref_TypeDef      reference;

  /** Enable/disable reset of prescaler on CH 0 start. */
  bool                 ch0ResetPre;

  /** Enable/disable output enable control by CH1 PRS signal. */
  bool                 outEnablePRS;

  /** Enable/disable sine mode. */
  bool                 sineEnable;

  /** Select if single ended or differential output mode. */
  bool                 diff;
} VDAC_Init_TypeDef;

/** Default configuration for VDAC initialization structure. */
#define VDAC_INIT_DEFAULT                                                \
  {                                                                      \
    true,                 /* Use main output path calibration values. */ \
    false,                /* Use synchronous clock mode. */              \
    false,                /* Turn off between sample off conversions.*/  \
    vdacRefresh8,         /* Refresh every 8th cycle. */                 \
    0,                    /* No prescaling. */                           \
    vdacRef1V25Ln,        /* 1.25 V internal low noise reference. */     \
    false,                /* Do not reset prescaler on CH 0 start. */    \
    false,                /* VDAC output enable always on. */            \
    false,                /* Disable sine mode. */                       \
    false                 /* Single ended mode. */                       \
  }

/** VDAC channel initialization structure. */
typedef struct {
  /** Enable channel. */
  bool                  enable;

  /**
   * Peripheral reflex system trigger selection. Only applicable if @p trigMode
   * is set to @p vdacTrigModePrs or @p vdacTrigModeSwPrs. */
  VDAC_PrsSel_TypeDef   prsSel;

  /** Treat the PRS signal asynchronously. */
  bool                  prsAsync;

  /** Channel conversion trigger mode. */
  VDAC_TrigMode_TypeDef trigMode;

  /** Set channel conversion mode to sample/shut-off mode. Default is
   *  continuous.*/
  bool                  sampleOffMode;
} VDAC_InitChannel_TypeDef;

/** Default configuration for VDAC channel initialization structure. */
#define VDAC_INITCHANNEL_DEFAULT                                                \
  {                                                                             \
    false,            /* Leave channel disabled when initialization is done. */ \
    vdacPrsSelCh0,    /* PRS CH 0 triggers conversion. */                       \
    false,            /* Treat PRS channel as a synchronous signal. */          \
    vdacTrigModeSw,   /* Conversion trigged by CH0DATA or COMBDATA write. */    \
    false,            /* Channel conversion set to continuous. */               \
  }
#else // defined(_SILICON_LABS_32B_SERIES_2)

/** Channel refresh period. */
typedef enum {
  vdacRefresh2    = _VDAC_CFG_REFRESHPERIOD_CYCLES2,    /**< Refresh every 2 clock cycles. */
  vdacRefresh4    = _VDAC_CFG_REFRESHPERIOD_CYCLES4,    /**< Refresh every 4 clock cycles. */
  vdacRefresh8    = _VDAC_CFG_REFRESHPERIOD_CYCLES8,    /**< Refresh every 8 clock cycles. */
  vdacRefresh16   = _VDAC_CFG_REFRESHPERIOD_CYCLES16,   /**< Refresh every 16 clock cycles. */
  vdacRefresh32   = _VDAC_CFG_REFRESHPERIOD_CYCLES32,   /**< Refresh every 32 clock cycles. */
  vdacRefresh64   = _VDAC_CFG_REFRESHPERIOD_CYCLES64,   /**< Refresh every 64 clock cycles. */
  vdacRefresh128  = _VDAC_CFG_REFRESHPERIOD_CYCLES128,  /**< Refresh every 128 clock cycles. */
  vdacRefresh256  = _VDAC_CFG_REFRESHPERIOD_CYCLES256,  /**< Refresh every 256 clock cycles. */
} VDAC_Refresh_TypeDef;

/** Timer overflow period. */
typedef enum {
  vdacCycles2  = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES2,    /**< Overflows every 2 clock cycles. */
  vdacCycles4  = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES4,    /**< Overflows every 4 clock cycles. */
  vdacCycles8  = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES8,    /**< Overflows every 8 clock cycles. */
  vdacCycles16 = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES16,   /**< Overflows every 16 clock cycles. */
  vdacCycles32 = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES32,   /**< Overflows every 32 clock cycles. */
  vdacCycles64 = _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES64    /**< Overflows every 64 clock cycles. */
} VDAC_TimerOverflow_TypeDef;

/** Reference voltage for VDAC. */
typedef enum {
  vdacRef1V25   = _VDAC_CFG_REFRSEL_V125,   /**< Internal 1.25 V band gap reference. */
  vdacRef2V5    = _VDAC_CFG_REFRSEL_V25,    /**< Internal 2.5 V band gap reference. */
  vdacRefAvdd   = _VDAC_CFG_REFRSEL_VDD,    /**< AVDD reference. */
  vdacRefExtPin = _VDAC_CFG_REFRSEL_EXT,    /**< External pin reference. */
} VDAC_Ref_TypeDef;

/** Refresh source for VDAC. */
typedef enum {
  vdacRefreshSrcNone          = _VDAC_CH0CFG_REFRESHSOURCE_NONE,        /**< No refresh source. */
  vdacRefreshSrcRefreshTimer  = _VDAC_CH0CFG_REFRESHSOURCE_REFRESHTIMER,/**< Refresh triggered by refresh timer overflow. */
  vdacRefreshSrcSyncPrs       = _VDAC_CH0CFG_REFRESHSOURCE_SYNCPRS,     /**< Refresh triggered by sync PRS. */
  vdacRefreshSrcAsyncPrs      = _VDAC_CH0CFG_REFRESHSOURCE_ASYNCPRS,    /**< Refresh triggered by async PRS. */
} VDAC_RefreshSource_TypeDef;

/** Channel conversion trigger mode. */
typedef enum {
  vdacTrigModeNone          = _VDAC_CH0CFG_TRIGMODE_NONE,           /**< No conversion trigger source selected. */
  vdacTrigModeSw            = _VDAC_CH0CFG_TRIGMODE_SW,             /**< Channel is triggered by CHnDATA or COMBDATA write. */
  vdacTrigModeSyncPrs       = _VDAC_CH0CFG_TRIGMODE_SYNCPRS,        /**< Channel is triggered by Sync PRS input. */
#if defined(LESENSE_PRESENT) && defined(_VDAC_CH0CFG_TRIGMODE_LESENSE)
  vdacTrigModeLesense       = _VDAC_CH0CFG_TRIGMODE_LESENSE,        /**< Channel is triggered by LESENSE. */
#endif
  vdacTrigModeInternalTimer = _VDAC_CH0CFG_TRIGMODE_INTERNALTIMER,  /**< Channel is triggered by Internal Timer. */
  vdacTrigModeAsyncPrs      = _VDAC_CH0CFG_TRIGMODE_ASYNCPRS        /**< Channel is triggered by Async PRS input. */
} VDAC_TrigMode_TypeDef;

/** Channel power mode. */
typedef enum {
  vdacPowerModeHighPower    = _VDAC_CH0CFG_POWERMODE_HIGHPOWER,     /**< High power buffer mode. */
  vdacPowerModeLowPower     = _VDAC_CH0CFG_POWERMODE_LOWPOWER       /**< Low power buffer mode. */
} VDAC_PowerMode_TypeDef;

/** VDAC channel Abus port selection. */
typedef enum {
  /** No GPIO selected.  */
  vdacChPortNone    = _VDAC_OUTCTRL_ABUSPORTSELCH0_NONE,
  /** Port A selected.  */
  vdacChPortA       = _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTA,
  /** Port B selected.  */
  vdacChPortB       = _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTB,
  /** Port C selected.  */
  vdacChPortC       = _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTC,
  /** Port D selected.  */
  vdacChPortD       = _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTD,
} VDAC_ChPortSel_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** VDAC initialization structure, common for both channels. */
typedef struct {
  /** Number of prescaled CLK_DAC + 1 for the vdac to warmup. */
  uint32_t                    warmupTime;

  /** Halt during debug. */
  bool                        dbgHalt;

  /** Always allow clk_dac. */
  bool                        onDemandClk;

  /** DMA Wakeup. */
  bool                        dmaWakeUp;

  /** Bias keep warm enable. */
  bool                        biasKeepWarm;

  /** Channel refresh period. */
  VDAC_Refresh_TypeDef        refresh;

  /** Internal timer overflow period. */
  VDAC_TimerOverflow_TypeDef  timerOverflow;

  /** Prescaler for VDAC clock. Clock is source clock divided by prescaler+1. */
  uint32_t                    prescaler;

  /** Reference voltage to use. */
  VDAC_Ref_TypeDef            reference;

  /** Enable/disable reset of prescaler on CH 0 start. */
  bool                        ch0ResetPre;

  /** Sine reset mode. */
  bool                        sineReset;

  /** Enable/disable sine mode. */
  bool                        sineEnable;

  /** Select if single ended or differential output mode. */
  bool                        diff;
} VDAC_Init_TypeDef;

/** Default configuration for VDAC initialization structure. */
#define VDAC_INIT_DEFAULT                                                                            \
  {                                                                                                  \
    _VDAC_CFG_WARMUPTIME_DEFAULT, /* Number of prescaled DAC_CLK for Vdac to warmup. */              \
    false,                        /* Continue while debugging. */                                    \
    true,                         /* On demand clock. */                                             \
    false,                        /* DMA wake up. */                                                 \
    false,                        /* Bias keep warm. */                                              \
    vdacRefresh8,                 /* Refresh every 8th cycle. */                                     \
    vdacCycles2,                  /* Internal overflow every 8th cycle. */                           \
    0,                            /* No prescaling. */                                               \
    vdacRef1V25,                  /* 1.25 V internal low noise reference. */                         \
    false,                        /* Do not reset prescaler on CH 0 start. */                        \
    false,                        /* Sine wave is stopped at the sample its currently outputting. */ \
    false,                        /* Disable sine mode. */                                           \
    false                         /* Differential mode. */                                           \
  }

/** VDAC channel initialization structure. */
typedef struct {
  /** Enable channel. */
  bool                        enable;

  /** Warm-up mode, keep VDAC on (in idle) - or shutdown between conversions.*/
  bool                        warmupKeepOn;

  /** Select high capacitance load mode in conjunction with high power. */
  bool                        highCapLoadEnable;

  /** Channel x FIFO Low threshold data valid level. */
  uint32_t                    fifoLowDataThreshold;

  /** Channel refresh source. */
  VDAC_RefreshSource_TypeDef  chRefreshSource;

  /** Channel conversion trigger mode. */
  VDAC_TrigMode_TypeDef       trigMode;

  /** Channel power mode. */
  VDAC_PowerMode_TypeDef      powerMode;

  /** Set channel conversion mode to sample/shut-off mode. Default is
   *  continuous.*/
  bool                        sampleOffMode;

  /** Vdac channel output pin. */
  uint32_t                    pin;

  /** Vdac channel output port. */
  VDAC_ChPortSel_t            port;

  /** Short High power and low power output. */
  bool                        shortOutput;

  /**  Alternative output enable. */
  bool                        auxOutEnable;

  /**  Main output enable. */
  bool                        mainOutEnable;

  /**  Channel output hold time. */
  uint32_t                    holdOutTime;
} VDAC_InitChannel_TypeDef;

/** Default configuration for VDAC channel initialization structure. */
#define VDAC_INITCHANNEL_DEFAULT                                                      \
  {                                                                                   \
    false,                  /* Leave channel disabled when initialization is done. */ \
    false,                  /* Turn off between sample off conversions.*/             \
    true,                   /* Enable High cap mode. */                               \
    0,                      /* FIFO data low watermark at 0. */                       \
    vdacRefreshSrcNone,     /* Channel refresh source. */                             \
    vdacTrigModeSw,         /* Conversion trigged by CH0DATA or COMBDATA write. */    \
    vdacPowerModeHighPower, /* High power mode enabled. */                            \
    false,                  /* Continuous conversion mode. */                         \
    0,                      /* ABUS pin selected. */                                  \
    vdacChPortNone,         /* No Analog bus port selected. */                        \
    false,                  /* Output not shorted */                                  \
    false,                  /* Alternative output disabled. */                        \
    true,                   /* Main output enabled. */                                \
    0                       /* Hold out time. Previously called settle time */        \
  }

#endif
/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void VDAC_ChannelOutputSet(VDAC_TypeDef *vdac,
                           unsigned int channel,
                           uint32_t     value);
void VDAC_Enable(VDAC_TypeDef *vdac, unsigned int ch, bool enable);
void VDAC_Init(VDAC_TypeDef *vdac, const VDAC_Init_TypeDef *init);
void VDAC_InitChannel(VDAC_TypeDef *vdac,
                      const VDAC_InitChannel_TypeDef *init,
                      unsigned int ch);

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *  Start/stop Sinemode.
 *
 * @details
 *   This function sends the sine mode start/stop signal to the DAC.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] start
 *   True to start the Sine mode, false to stop it.
 ******************************************************************************/
__STATIC_INLINE void VDAC_SineModeStart(VDAC_TypeDef *vdac, bool start)
{
  EFM_ASSERT(VDAC_REF_VALID(vdac));

  while (vdac->STATUS & VDAC_STATUS_SYNCBUSY) {
  }

  if (start) {
    vdac->CMD = VDAC_CMD_SINEMODESTART;
    while ((vdac->STATUS & VDAC_STATUS_SINEACTIVE) == 0) {
    }
  } else {
    vdac->CMD = VDAC_CMD_SINEMODESTOP;
    while ((vdac->STATUS & VDAC_STATUS_SINEACTIVE) != 0) {
    }
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Set the output signal of VDAC channel 0 to a given value.
 *
 * @details
 *   This function sets the output signal of VDAC channel 0 by writing @p value
 *   to the CH0DATA register.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] value
 *   Value to write to channel 0 output register CH0DATA.
 ******************************************************************************/
__STATIC_INLINE void VDAC_Channel0OutputSet(VDAC_TypeDef *vdac,
                                            uint32_t value)
{
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  EFM_ASSERT(value <= _VDAC_CH0DATA_MASK);
  vdac->CH0DATA = value;
#elif defined(_SILICON_LABS_32B_SERIES_2)
  EFM_ASSERT(value <= _VDAC_CH0F_MASK);
  vdac->CH0F = value;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set the output signal of VDAC channel 1 to a given value.
 *
 * @details
 *   This function sets the output signal of VDAC channel 1 by writing @p value
 *   to the CH1DATA register.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] value
 *   Value to write to channel 1 output register CH1DATA.
 ******************************************************************************/
__STATIC_INLINE void VDAC_Channel1OutputSet(VDAC_TypeDef *vdac,
                                            uint32_t value)
{
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
  EFM_ASSERT(value <= _VDAC_CH1DATA_MASK);
  vdac->CH1DATA = value;
#elif defined(_SILICON_LABS_32B_SERIES_2)
  EFM_ASSERT(value <= _VDAC_CH1F_MASK);
  vdac->CH1F = value;
#endif
}

/***************************************************************************//**
 * @brief
 *   Clear one or more pending VDAC interrupts.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] flags
 *   Pending VDAC interrupt source to clear. Use a bitwise logic OR combination
 *   of valid interrupt flags for the VDAC module (VDAC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void VDAC_IntClear(VDAC_TypeDef *vdac, uint32_t flags)
{
#if defined(VDAC_HAS_SET_CLEAR)
  vdac->IF_CLR = flags;
#else
  vdac->IFC = flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Disable one or more VDAC interrupts.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] flags
 *   VDAC interrupt sources to disable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the VDAC module (VDAC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void VDAC_IntDisable(VDAC_TypeDef *vdac, uint32_t flags)
{
#if defined(VDAC_HAS_SET_CLEAR)
  vdac->IEN_CLR = flags;
#else
  vdac->IEN &= ~flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Enable one or more VDAC interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   VDAC_IntClear() prior to enabling the interrupt.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] flags
 *   VDAC interrupt sources to enable. Use a bitwise logic OR combination
 *   of valid interrupt flags for the VDAC module (VDAC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void VDAC_IntEnable(VDAC_TypeDef *vdac, uint32_t flags)
{
#if defined(VDAC_HAS_SET_CLEAR)
  vdac->IEN_SET = flags;
#else
  vdac->IEN |= flags;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get pending VDAC interrupt flags.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @return
 *   VDAC interrupt sources pending. Use a bitwise logic OR combination
 *   of valid interrupt flags for the VDAC module (VDAC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t VDAC_IntGet(VDAC_TypeDef *vdac)
{
  return vdac->IF;
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending VDAC interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @note
 *   Interrupt flags are not cleared by the use of this function.
 *
 * @return
 *   Pending and enabled VDAC interrupt sources.
 *   The return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in VDACx_IEN_nnn
 *     register (VDACx_IEN_nnn) and
 *   - the OR combination of valid interrupt flags of the VDAC module
 *     (VDACx_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE uint32_t VDAC_IntGetEnabled(VDAC_TypeDef *vdac)
{
  uint32_t ien = vdac->IEN;

  /* Bitwise AND of pending and enabled interrupts */
  return vdac->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending VDAC interrupts from SW.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @param[in] flags
 *   VDAC interrupt sources to set to pending. Use a bitwise logic OR
 *   combination of valid interrupt flags for the VDAC module (VDAC_IF_nnn).
 ******************************************************************************/
__STATIC_INLINE void VDAC_IntSet(VDAC_TypeDef *vdac, uint32_t flags)
{
#if defined(VDAC_HAS_SET_CLEAR)
  vdac->IF_SET = flags;
#else
  vdac->IFS = flags;
#endif
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *    Get the VDAC Status register.
 *
 * @param[in] vdac
 *   Pointer to VDAC peripheral register block.
 *
 * @return
 *    Current STATUS register value.
 ******************************************************************************/
__STATIC_INLINE uint32_t VDAC_GetStatus(VDAC_TypeDef *vdac)
{
  return vdac->STATUS;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
uint32_t VDAC_PrescaleCalc(uint32_t vdacFreq, bool syncMode, uint32_t hfperFreq);
#else
uint32_t VDAC_PrescaleCalc(VDAC_TypeDef *vdac, uint32_t vdacFreq);
#endif

void VDAC_Reset(VDAC_TypeDef *vdac);

/** @} (end addtogroup vdac) */

#ifdef __cplusplus
}
#endif

#endif /* defined(VDAC_COUNT) && (VDAC_COUNT > 0) */
#endif /* EM_VDAC_H */
