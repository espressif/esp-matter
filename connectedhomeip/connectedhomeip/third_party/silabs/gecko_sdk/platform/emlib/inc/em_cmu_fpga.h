/***************************************************************************//**
 * @file em_cmu_fpga.h
 * @brief Clock management unit (CMU) API for FPGAs
 * @version 5.3.5
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef EM_CMU_FPGA_H
#define EM_CMU_FPGA_H

#include "em_device.h"

#if defined(CMU_PRESENT) && defined(FPGA)

#include <stdbool.h>
#include "sl_assert.h"
#include "em_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CMU
 * @{
 ******************************************************************************/

#define CMU_VSCALEEM01_LOWPOWER_VOLTAGE_CLOCK_MAX     40000000UL // Lynx value
#define VSCALE_EM01_LOW_POWER           1 // Lynx value
#define VSCALE_EM01_HIGH_PERFORMANCE    0 // Lynx value

#if defined(LFRCO_PRECISION_MODE) && LFRCO_PRECISION_MODE
#define PLFRCO_PRESENT
#endif

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
/** Clock divisors. These values are valid for prescalers. */
#define cmuClkDiv_1     1     /**< Divide clock by 1. */
#define cmuClkDiv_2     2     /**< Divide clock by 2. */
#define cmuClkDiv_4     4     /**< Divide clock by 4. */
#define cmuClkDiv_8     8     /**< Divide clock by 8. */
#define cmuClkDiv_16    16    /**< Divide clock by 16. */
#define cmuClkDiv_32    32    /**< Divide clock by 32. */
#define cmuClkDiv_64    64    /**< Divide clock by 64. */
#define cmuClkDiv_128   128   /**< Divide clock by 128. */
#define cmuClkDiv_256   256   /**< Divide clock by 256. */
#define cmuClkDiv_512   512   /**< Divide clock by 512. */
#define cmuClkDiv_1024  1024  /**< Divide clock by 1024. */
#define cmuClkDiv_2048  2048  /**< Divide clock by 2048. */
#define cmuClkDiv_4096  4096  /**< Divide clock by 4096. */
#define cmuClkDiv_8192  8192  /**< Divide clock by 8192. */
#define cmuClkDiv_16384 16384 /**< Divide clock by 16384. */
#define cmuClkDiv_32768 32768 /**< Divide clock by 32768. */

/** Clock divider configuration */
typedef uint32_t CMU_ClkDiv_TypeDef;

/** High frequency system RCO bands */
typedef enum {
  cmuHFRCOFreq_1M0Hz            = 1000000U,             /**< 1MHz RC band   */
  cmuHFRCOFreq_2M0Hz            = 2000000U,             /**< 2MHz RC band   */
  cmuHFRCOFreq_4M0Hz            = 4000000U,             /**< 4MHz RC band   */
  cmuHFRCOFreq_7M0Hz            = 7000000U,             /**< 7MHz RC band   */
  cmuHFRCOFreq_13M0Hz           = 13000000U,            /**< 13MHz RC band  */
  cmuHFRCOFreq_16M0Hz           = 16000000U,            /**< 16MHz RC band  */
  cmuHFRCOFreq_19M0Hz           = 19000000U,            /**< 19MHz RC band  */
  cmuHFRCOFreq_26M0Hz           = 26000000U,            /**< 26MHz RC band  */
  cmuHFRCOFreq_32M0Hz           = 32000000U,            /**< 32MHz RC band  */
  cmuHFRCOFreq_38M0Hz           = 38000000U,            /**< 38MHz RC band  */
  cmuHFRCOFreq_48M0Hz           = 48000000U,            /**< 48MHz RC band  */
  cmuHFRCOFreq_56M0Hz           = 56000000U,            /**< 56MHz RC band  */
  cmuHFRCOFreq_64M0Hz           = 64000000U,            /**< 64MHz RC band  */
  cmuHFRCOFreq_72M0Hz           = 72000000U,            /**< 72MHz RC band  */
  cmuHFRCOFreq_UserDefined      = 0,
} CMU_HFRCOFreq_TypeDef;
#define CMU_HFRCO_MIN           cmuHFRCOFreq_1M0Hz
#define CMU_HFRCO_MAX           cmuHFRCOFreq_72M0Hz

/** HFRCODPLL frequency bands */
typedef enum {
  cmuHFRCODPLLFreq_1M0Hz            = 1000000U,         /**< 1MHz RC band.  */
  cmuHFRCODPLLFreq_2M0Hz            = 2000000U,         /**< 2MHz RC band.  */
  cmuHFRCODPLLFreq_4M0Hz            = 4000000U,         /**< 4MHz RC band.  */
  cmuHFRCODPLLFreq_7M0Hz            = 7000000U,         /**< 7MHz RC band.  */
  cmuHFRCODPLLFreq_13M0Hz           = 13000000U,        /**< 13MHz RC band. */
  cmuHFRCODPLLFreq_16M0Hz           = 16000000U,        /**< 16MHz RC band. */
  cmuHFRCODPLLFreq_19M0Hz           = 19000000U,        /**< 19MHz RC band. */
  cmuHFRCODPLLFreq_26M0Hz           = 26000000U,        /**< 26MHz RC band. */
  cmuHFRCODPLLFreq_32M0Hz           = 32000000U,        /**< 32MHz RC band. */
  cmuHFRCODPLLFreq_38M0Hz           = 38000000U,        /**< 38MHz RC band. */
  cmuHFRCODPLLFreq_48M0Hz           = 48000000U,        /**< 48MHz RC band. */
  cmuHFRCODPLLFreq_56M0Hz           = 56000000U,        /**< 56MHz RC band. */
  cmuHFRCODPLLFreq_64M0Hz           = 64000000U,        /**< 64MHz RC band. */
  cmuHFRCODPLLFreq_80M0Hz           = 80000000U,        /**< 80MHz RC band. */
  cmuHFRCODPLLFreq_UserDefined      = 0,
} CMU_HFRCODPLLFreq_TypeDef;

/** HFRCODPLL maximum frequency */
#define CMU_HFRCODPLL_MIN       cmuHFRCODPLLFreq_1M0Hz
/** HFRCODPLL minimum frequency */
#define CMU_HFRCODPLL_MAX       cmuHFRCODPLLFreq_80M0Hz

/** HFRCOEM23 frequency bands */
typedef enum {
  cmuHFRCOEM23Freq_1M0Hz            = 1000000U,         /**< 1MHz RC band.  */
  cmuHFRCOEM23Freq_2M0Hz            = 2000000U,         /**< 2MHz RC band.  */
  cmuHFRCOEM23Freq_4M0Hz            = 4000000U,         /**< 4MHz RC band.  */
  cmuHFRCOEM23Freq_13M0Hz           = 13000000U,        /**< 13MHz RC band. */
  cmuHFRCOEM23Freq_16M0Hz           = 16000000U,        /**< 16MHz RC band. */
  cmuHFRCOEM23Freq_19M0Hz           = 19000000U,        /**< 19MHz RC band. */
  cmuHFRCOEM23Freq_26M0Hz           = 26000000U,        /**< 26MHz RC band. */
  cmuHFRCOEM23Freq_32M0Hz           = 32000000U,        /**< 32MHz RC band. */
  cmuHFRCOEM23Freq_40M0Hz           = 40000000U,        /**< 40MHz RC band. */
  cmuHFRCOEM23Freq_UserDefined      = 0,
} CMU_HFRCOEM23Freq_TypeDef;

/** HFRCOEM23 maximum frequency */
#define CMU_HFRCOEM23_MIN       cmuHFRCOEM23Freq_1M0Hz
/** HFRCOEM23 minimum frequency */
#define CMU_HFRCOEM23_MAX       cmuHFRCOEM23Freq_40M0Hz

/** Clock points in CMU. Please refer to CMU overview in reference manual. */
typedef enum {
  /*******************/
  /* Clock branches  */
  /*******************/

  cmuClock_SYSCLK,                  /**< System clock.  */
  cmuClock_HCLK,                    /**< Core and AHB bus interface clock. */
  cmuClock_EXPCLK,                  /**< Export clock. */
  cmuClock_PCLK,                    /**< Peripheral APB bus interface clock. */
  cmuClock_LSPCLK,                  /**< Low speed peripheral APB bus interface clock. */
  cmuClock_IADCCLK,                 /**< IADC clock. */
  cmuClock_EM01GRPACLK,             /**< EM01GRPA clock. */
#if defined(_CMU_EM01GRPBCLKCTRL_MASK)
  cmuClock_EM01GRPBCLK,             /**< EM01GRPB clock. */
#endif
  cmuClock_EM01GRPCCLK,             /**< EM01GRPC clock. */
  cmuClock_EM23GRPACLK,             /**< EM23GRPA clock. */
  cmuClock_EM4GRPACLK,              /**< EM4GRPA clock. */
  cmuClock_LFRCO,                   /**< LFRCO clock. */
  cmuClock_ULFRCO,                  /**< ULFRCO clock. */
  cmuClock_LFXO,                    /**< LFXO clock. */
  cmuClock_HFRCO0,                  /**< HFRCO0 clock. */
  cmuClock_WDOG0CLK,                /**< WDOG0 clock. */
#if WDOG_COUNT > 1
  cmuClock_WDOG1CLK,                /**< WDOG1 clock. */
#endif
  cmuClock_DPLLREFCLK,              /**< DPLL reference clock. */
#if defined(_CMU_TRACECLKCTRL_MASK)
  cmuClock_TRACECLK,                /**< Debug trace clock. */
#endif
  cmuClock_RTCCCLK,                 /**< RTCC clock. */
#if defined(LESENSE_PRESENT)
  cmuClock_LESENSEHFCLK,
  cmuClock_LESENSELFCLK,
#endif

  /*********************/
  /* Peripheral clocks */
  /*********************/

  cmuClock_CORE,                    /**< Cortex-M33 core clock. */
  cmuClock_SYSTICK,                 /**< Optional Cortex-M33 SYSTICK clock. */
  cmuClock_ACMP0,                   /**< ACMP0 clock. */
  cmuClock_ACMP1,                   /**< ACMP1 clock. */
  cmuClock_BURTC,                   /**< BURTC clock. */
  cmuClock_GPCRC,                   /**< GPCRC clock. */
  cmuClock_GPIO,                    /**< GPIO clock. */
  cmuClock_I2C0,                    /**< I2C0 clock. */
  cmuClock_I2C1,                    /**< I2C1 clock. */
  cmuClock_SYSCFG,                  /**< SYSCFG clock. */
  cmuClock_IADC0,                   /**< IADC clock. */
  cmuClock_LDMA,                    /**< LDMA clock. */
  cmuClock_LDMAXBAR,                /**< LDMAXBAR clock. */
  cmuClock_LETIMER0,                /**< LETIMER clock. */
  cmuClock_PRS,                     /**< PRS clock. */
  cmuClock_RTCC,                    /**< RTCC clock. */
  cmuClock_SYSRTC,                  /**< SYSRTC clock. */
  cmuClock_TIMER0,                  /**< TIMER0 clock. */
  cmuClock_TIMER1,                  /**< TIMER1 clock. */
  cmuClock_TIMER2,                  /**< TIMER2 clock. */
  cmuClock_TIMER3,                  /**< TIMER3 clock. */
  cmuClock_TIMER4,                  /**< TIMER4 clock. */
  cmuClock_TIMER5,                  /**< TIMER5 clock. */
  cmuClock_TIMER6,                  /**< TIMER6 clock. */
  cmuClock_TIMER7,                  /**< TIMER7 clock. */
  cmuClock_BURAM,
  cmuClock_LESENSE,
  cmuClock_LESENSEHF,               /**< LESENSEHF clock. */
#if defined(USART0)
  cmuClock_USART0,                  /**< USART0 clock. */
#endif
#if defined(USART1)
  cmuClock_USART1,                  /**< USART1 clock. */
#endif
#if defined(USART2)
  cmuClock_USART2,                  /**< USART2 clock. */
#endif
  cmuClock_WDOG0,                   /**< WDOG0 clock. */
#if defined(WDOG1)
  cmuClock_WDOG1,                   /**< WDOG1 clock. */
#endif
  cmuClock_PDM,                     /**< PDM clock. */
  cmuClock_PDMREF,                  /**< PDM reference clock. */
  cmuClock_LFA,
  cmuClock_LCDpre,
  cmuClock_LCD,
#if defined(EUART0)
  cmuClock_EUART0,                  /**< EUART0 clock. */
#endif
#if defined(EUSART0)
  cmuClock_EUSART0,                 /**< EUSART0 clock. */
  cmuClock_EUSART0CLK,
#endif
#if defined(EUSART1)
  cmuClock_EUSART1,                 /**< EUSART1 clock. */
#endif
#if defined(EUSART2)
  cmuClock_EUSART2,                 /**< EUSART2 clock. */
#endif
#if defined(EUSART3)
  cmuClock_EUSART3,                 /**< EUSART3 clock. */
#endif
#if defined(EUSART4)
  cmuClock_EUSART4,                 /**< EUSART4 clock. */
#endif
  cmuClock_PCNT0,
  cmuClock_KEYSCAN,
  cmuClock_HFPER,
  cmuClock_MSC,
  cmuClock_DMEM,
  cmuClock_SEMAILBOX,
  cmuClock_SMU,
  cmuClock_VDAC0
} CMU_Clock_TypeDef;

/** OCELOT TEMPORARY DEFINE. */
#define cmuClock_CORELE             cmuClock_CORE

/** Oscillator types. */
typedef enum {
  cmuOsc_LFXO,     /**< Low frequency crystal oscillator. */
#if defined(LFRCO_PRESENT)
  cmuOsc_LFRCO,    /**< Low frequency RC oscillator. */
#endif
#if defined(PLFRCO_PRESENT)
  cmuOsc_PLFRCO,    /**< Precision Low frequency RC oscillator. */
#endif
  cmuOsc_FSRCO,       /**< Fast startup fixed frequency RC oscillator. */
  cmuOsc_HFXO,     /**< High frequency crystal oscillator. */
  cmuOsc_HFRCO,    /**< High frequency RC oscillator. */
  cmuOsc_HFRCODPLL,   /**< High frequency RC and DPLL oscillator. */
#if defined(HFRCOEM23_PRESENT)
  cmuOsc_HFRCOEM23,   /**< High frequency deep sleep RC oscillator. */
#endif
  cmuOsc_ULFRCO,   /**< Ultra low frequency RC oscillator. */
} CMU_Osc_TypeDef;

/** Selectable clock sources. */
typedef enum {
  cmuSelect_Error,       /**< Usage error. */
  cmuSelect_Disabled,    /**< Clock selector disabled. */
  cmuSelect_FSRCO,       /**< Fast startup fixed frequency RC oscillator. */
  cmuSelect_HFXO,        /**< High frequency crystal oscillator. */
  cmuSelect_HFRCO,       /**< High frequency RC. */
  cmuSelect_HFRCODPLL,   /**< High frequency RC and DPLL oscillator. */
#if defined(HFRCOEM23_PRESENT)
  cmuSelect_HFRCOEM23,   /**< High frequency deep sleep RC oscillator. */
#endif
  cmuSelect_CLKIN0,      /**< External clock input. */
  cmuSelect_LFXO,        /**< Low frequency crystal oscillator. */
#if defined(LFRCO_PRESENT)
  cmuSelect_LFRCO,       /**< Low frequency RC oscillator. */
#endif
#if defined(PLFRCO_PRESENT)
  cmuSelect_PLFRCO,                     /**< Precision Low frequency RC oscillator. */
#endif
  cmuSelect_ULFRCO,      /**< Ultra low frequency RC oscillator. */
  cmuSelect_PCLK,        /**< Peripheral APB bus interface clock. */
  cmuSelect_HCLK,        /**< Core and AHB bus interface clock. */
  cmuSelect_HCLKDIV1024, /**< Prescaled HCLK frequency clock. */
  cmuSelect_EM01GRPACLK, /**< EM01GRPA clock. */
  cmuSelect_EM01GRPCCLK, /**< EM01GRPC clock. */
  cmuSelect_EM23GRPACLK, /**< EM23GRPACLK clock.*/
  cmuSelect_EXPCLK,      /**< Pin export clock. */
  cmuSelect_PRS          /**< PRS input as clock. */
} CMU_Select_TypeDef;

/** DPLL reference clock edge detect selector. */
typedef enum {
  cmuDPLLEdgeSel_Fall = 0,    /**< Detect falling edge of reference clock. */
  cmuDPLLEdgeSel_Rise = 1     /**< Detect rising edge of reference clock. */
} CMU_DPLLEdgeSel_TypeDef;

/** DPLL lock mode selector. */
typedef enum {
  cmuDPLLLockMode_Freq  = _DPLL_CFG_MODE_FLL,   /**< Frequency lock mode. */
  cmuDPLLLockMode_Phase = _DPLL_CFG_MODE_PLL    /**< Phase lock mode. */
} CMU_DPLLLockMode_TypeDef;

/** LFXO oscillator modes. */
typedef enum {
  cmuLfxoOscMode_Crystal       = _LFXO_CFG_MODE_XTAL,      /**< Crystal oscillator. */
  cmuLfxoOscMode_AcCoupledSine = _LFXO_CFG_MODE_BUFEXTCLK, /**< External AC coupled sine. */
  cmuLfxoOscMode_External      = _LFXO_CFG_MODE_DIGEXTCLK, /**< External digital clock. */
} CMU_LfxoOscMode_TypeDef;

/** LFXO start-up timeout delay. */
typedef enum {
  cmuLfxoStartupDelay_2Cycles   = _LFXO_CFG_TIMEOUT_CYCLES2,   /**< 2 cycles start-up delay. */
  cmuLfxoStartupDelay_256Cycles = _LFXO_CFG_TIMEOUT_CYCLES256, /**< 256 cycles start-up delay. */
  cmuLfxoStartupDelay_1KCycles  = _LFXO_CFG_TIMEOUT_CYCLES1K,  /**< 1K cycles start-up delay. */
  cmuLfxoStartupDelay_2KCycles  = _LFXO_CFG_TIMEOUT_CYCLES2K,  /**< 2K cycles start-up delay. */
  cmuLfxoStartupDelay_4KCycles  = _LFXO_CFG_TIMEOUT_CYCLES4K,  /**< 4K cycles start-up delay. */
  cmuLfxoStartupDelay_8KCycles  = _LFXO_CFG_TIMEOUT_CYCLES8K,  /**< 8K cycles start-up delay. */
  cmuLfxoStartupDelay_16KCycles = _LFXO_CFG_TIMEOUT_CYCLES16K, /**< 16K cycles start-up delay. */
  cmuLfxoStartupDelay_32KCycles = _LFXO_CFG_TIMEOUT_CYCLES32K, /**< 32K cycles start-up delay. */
} CMU_LfxoStartupDelay_TypeDef;

//TODO UPDATE with SYXO new IP.
#define SYSXO
/** HFXO oscillator modes. */
typedef enum {
#if defined(SYSXO)
  cmuHfxoOscMode_Crystal,
  cmuHfxoOscMode_ExternalSine,
#if defined(_HFXO_CFG_MODE_EXTCLKPKDET)
  cmuHfxoOscMode_ExternalSinePkDet,
#endif
#else
  cmuHfxoOscMode_Crystal           = _HFXO_CFG_MODE_XTAL,        /**< Crystal oscillator. */
  cmuHfxoOscMode_ExternalSine      = _HFXO_CFG_MODE_EXTCLK,      /**< External digital clock. */
#if defined(_HFXO_CFG_MODE_EXTCLKPKDET)
  cmuHfxoOscMode_ExternalSinePkDet = _HFXO_CFG_MODE_EXTCLKPKDET, /**< External digital clock with peak detector used. */
#endif
#endif
} CMU_HfxoOscMode_TypeDef;

/** HFXO core bias LSB change timeout. */
typedef enum {
#if defined(SYSXO)
  cmuHfxoCbLsbTimeout_8us,    /**< 8 us timeout. */
  cmuHfxoCbLsbTimeout_20us,   /**< 20 us timeout. */
  cmuHfxoCbLsbTimeout_41us,   /**< 41 us timeout. */
  cmuHfxoCbLsbTimeout_62us,   /**< 62 us timeout. */
  cmuHfxoCbLsbTimeout_83us,   /**< 83 us timeout. */
  cmuHfxoCbLsbTimeout_104us,  /**< 104 us timeout. */
  cmuHfxoCbLsbTimeout_125us,  /**< 125 us timeout. */
  cmuHfxoCbLsbTimeout_166us,  /**< 166 us timeout. */
  cmuHfxoCbLsbTimeout_208us,  /**< 208 us timeout. */
  cmuHfxoCbLsbTimeout_250us,  /**< 250 us timeout. */
  cmuHfxoCbLsbTimeout_333us,  /**< 333 us timeout. */
  cmuHfxoCbLsbTimeout_416us,  /**< 416 us timeout. */
  cmuHfxoCbLsbTimeout_833us,  /**< 833 us timeout. */
  cmuHfxoCbLsbTimeout_1250us, /**< 1250 us timeout. */
  cmuHfxoCbLsbTimeout_2083us, /**< 2083 us timeout. */
  cmuHfxoCbLsbTimeout_3750us, /**< 3750 us timeout. */
#else
  cmuHfxoCbLsbTimeout_8us    = _HFXO_XTALCFG_TIMEOUTCBLSB_T8US,    /**< 8 us timeout. */
  cmuHfxoCbLsbTimeout_20us   = _HFXO_XTALCFG_TIMEOUTCBLSB_T20US,   /**< 20 us timeout. */
  cmuHfxoCbLsbTimeout_41us   = _HFXO_XTALCFG_TIMEOUTCBLSB_T41US,   /**< 41 us timeout. */
  cmuHfxoCbLsbTimeout_62us   = _HFXO_XTALCFG_TIMEOUTCBLSB_T62US,   /**< 62 us timeout. */
  cmuHfxoCbLsbTimeout_83us   = _HFXO_XTALCFG_TIMEOUTCBLSB_T83US,   /**< 83 us timeout. */
  cmuHfxoCbLsbTimeout_104us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T104US,  /**< 104 us timeout. */
  cmuHfxoCbLsbTimeout_125us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T125US,  /**< 125 us timeout. */
  cmuHfxoCbLsbTimeout_166us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T166US,  /**< 166 us timeout. */
  cmuHfxoCbLsbTimeout_208us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T208US,  /**< 208 us timeout. */
  cmuHfxoCbLsbTimeout_250us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T250US,  /**< 250 us timeout. */
  cmuHfxoCbLsbTimeout_333us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T333US,  /**< 333 us timeout. */
  cmuHfxoCbLsbTimeout_416us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T416US,  /**< 416 us timeout. */
  cmuHfxoCbLsbTimeout_833us  = _HFXO_XTALCFG_TIMEOUTCBLSB_T833US,  /**< 833 us timeout. */
  cmuHfxoCbLsbTimeout_1250us = _HFXO_XTALCFG_TIMEOUTCBLSB_T1250US, /**< 1250 us timeout. */
  cmuHfxoCbLsbTimeout_2083us = _HFXO_XTALCFG_TIMEOUTCBLSB_T2083US, /**< 2083 us timeout. */
  cmuHfxoCbLsbTimeout_3750us = _HFXO_XTALCFG_TIMEOUTCBLSB_T3750US, /**< 3750 us timeout. */
#endif
} CMU_HfxoCbLsbTimeout_TypeDef;

/** HFXO steady state timeout. */
typedef enum {
#if defined(SYSXO)
  cmuHfxoSteadyStateTimeout_16us,   /**< 16 us timeout. */
  cmuHfxoSteadyStateTimeout_41us,   /**< 41 us timeout. */
  cmuHfxoSteadyStateTimeout_83us,   /**< 83 us timeout. */
  cmuHfxoSteadyStateTimeout_125us,  /**< 125 us timeout. */
  cmuHfxoSteadyStateTimeout_166us,  /**< 166 us timeout. */
  cmuHfxoSteadyStateTimeout_208us,  /**< 208 us timeout. */
  cmuHfxoSteadyStateTimeout_250us,  /**< 250 us timeout. */
  cmuHfxoSteadyStateTimeout_333us,  /**< 333 us timeout. */
  cmuHfxoSteadyStateTimeout_416us,  /**< 416 us timeout. */
  cmuHfxoSteadyStateTimeout_500us,  /**< 500 us timeout. */
  cmuHfxoSteadyStateTimeout_666us,  /**< 666 us timeout. */
  cmuHfxoSteadyStateTimeout_833us,  /**< 833 us timeout. */
  cmuHfxoSteadyStateTimeout_1666us, /**< 1666 us timeout. */
  cmuHfxoSteadyStateTimeout_2500us, /**< 2500 us timeout. */
  cmuHfxoSteadyStateTimeout_4166us, /**< 4166 us timeout. */
  cmuHfxoSteadyStateTimeout_7500us, /**< 7500 us timeout. */
#else
  cmuHfxoSteadyStateTimeout_16us   = _HFXO_XTALCFG_TIMEOUTSTEADY_T16US,   /**< 16 us timeout. */
  cmuHfxoSteadyStateTimeout_41us   = _HFXO_XTALCFG_TIMEOUTSTEADY_T41US,   /**< 41 us timeout. */
  cmuHfxoSteadyStateTimeout_83us   = _HFXO_XTALCFG_TIMEOUTSTEADY_T83US,   /**< 83 us timeout. */
  cmuHfxoSteadyStateTimeout_125us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T125US,  /**< 125 us timeout. */
  cmuHfxoSteadyStateTimeout_166us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T166US,  /**< 166 us timeout. */
  cmuHfxoSteadyStateTimeout_208us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T208US,  /**< 208 us timeout. */
  cmuHfxoSteadyStateTimeout_250us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T250US,  /**< 250 us timeout. */
  cmuHfxoSteadyStateTimeout_333us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T333US,  /**< 333 us timeout. */
  cmuHfxoSteadyStateTimeout_416us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T416US,  /**< 416 us timeout. */
  cmuHfxoSteadyStateTimeout_500us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T500US,  /**< 500 us timeout. */
  cmuHfxoSteadyStateTimeout_666us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T666US,  /**< 666 us timeout. */
  cmuHfxoSteadyStateTimeout_833us  = _HFXO_XTALCFG_TIMEOUTSTEADY_T833US,  /**< 833 us timeout. */
  cmuHfxoSteadyStateTimeout_1666us = _HFXO_XTALCFG_TIMEOUTSTEADY_T1666US, /**< 1666 us timeout. */
  cmuHfxoSteadyStateTimeout_2500us = _HFXO_XTALCFG_TIMEOUTSTEADY_T2500US, /**< 2500 us timeout. */
  cmuHfxoSteadyStateTimeout_4166us = _HFXO_XTALCFG_TIMEOUTSTEADY_T4166US, /**< 4166 us timeout. */
  cmuHfxoSteadyStateTimeout_7500us = _HFXO_XTALCFG_TIMEOUTSTEADY_T7500US, /**< 7500 us timeout. */
#endif
} CMU_HfxoSteadyStateTimeout_TypeDef;

/** HFXO core degeneration control. */
typedef enum {
#if defined(SYSXO)
  cmuHfxoCoreDegen_None, /**< No core degeneration. */
  cmuHfxoCoreDegen_33,   /**< Core degeneration control 33. */
  cmuHfxoCoreDegen_50,   /**< Core degeneration control 50. */
  cmuHfxoCoreDegen_100,  /**< Core degeneration control 100. */
#else
  cmuHfxoCoreDegen_None = _HFXO_XTALCTRL_COREDGENANA_NONE,    /**< No core degeneration. */
  cmuHfxoCoreDegen_33   = _HFXO_XTALCTRL_COREDGENANA_DGEN33,  /**< Core degeneration control 33. */
  cmuHfxoCoreDegen_50   = _HFXO_XTALCTRL_COREDGENANA_DGEN50,  /**< Core degeneration control 50. */
  cmuHfxoCoreDegen_100  = _HFXO_XTALCTRL_COREDGENANA_DGEN100, /**< Core degeneration control 100. */
#endif
} CMU_HfxoCoreDegen_TypeDef;

/** HFXO XI and XO pin fixed capacitor control. */
typedef enum {
#if defined(SYSXO)
  cmuHfxoCtuneFixCap_None, /**< No fixed capacitors. */
  cmuHfxoCtuneFixCap_Xi,   /**< Fixed capacitor on XI pin. */
  cmuHfxoCtuneFixCap_Xo,   /**< Fixed capacitor on XO pin. */
  cmuHfxoCtuneFixCap_Both, /**< Fixed capacitor on both pins. */
#else
  cmuHfxoCtuneFixCap_None = _HFXO_XTALCTRL_CTUNEFIXANA_NONE,  /**< No fixed capacitors. */
  cmuHfxoCtuneFixCap_Xi   = _HFXO_XTALCTRL_CTUNEFIXANA_XI,    /**< Fixed capacitor on XI pin. */
  cmuHfxoCtuneFixCap_Xo   = _HFXO_XTALCTRL_CTUNEFIXANA_XO,    /**< Fixed capacitor on XO pin. */
  cmuHfxoCtuneFixCap_Both = _HFXO_XTALCTRL_CTUNEFIXANA_BOTH,  /**< Fixed capacitor on both pins. */
#endif
} CMU_HfxoCtuneFixCap_TypeDef;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** LFXO initialization structure. Init values should be obtained from a
    configuration tool, app. note or xtal data sheet.  */
typedef struct {
  uint8_t   gain;                       /**< Startup gain. */
  uint8_t   capTune;                    /**< Internal capacitance tuning. */
  CMU_LfxoStartupDelay_TypeDef timeout; /**< Startup delay. */
  CMU_LfxoOscMode_TypeDef mode;         /**< Oscillator mode. */
  bool      highAmplitudeEn;            /**< High amplitude enable. */
  bool      agcEn;                      /**< AGC enable. */
  bool      failDetEM4WUEn;             /**< EM4 wakeup on failure enable. */
  bool      failDetEn;              /**< Oscillator failure detection enable. */
  bool      disOnDemand;                /**< Disable on-demand requests. */
  bool      forceEn;                    /**< Force oscillator enable. */
  bool      regLock;                    /**< Lock register access. */
} CMU_LFXOInit_TypeDef;

/** Default LFXO initialization values for XTAL mode. */
#define CMU_LFXOINIT_DEFAULT                      \
  {                                               \
    1,                                            \
    38,                                           \
    cmuLfxoStartupDelay_4KCycles,                 \
    cmuLfxoOscMode_Crystal,                       \
    false,                  /* highAmplitudeEn */ \
    true,                   /* agcEn           */ \
    false,                  /* failDetEM4WUEn  */ \
    false,                  /* failDetEn       */ \
    false,                  /* DisOndemand     */ \
    false,                  /* ForceEn         */ \
    false                   /* Lock registers  */ \
  }

/** HFXO initialization structure. Init values should be obtained from a configuration tool,
    app note or xtal data sheet  */

typedef struct {
  CMU_HfxoCbLsbTimeout_TypeDef        timeoutCbLsb;            /**< Core bias change timeout. */
  CMU_HfxoSteadyStateTimeout_TypeDef  timeoutSteadyFirstLock;  /**< Steady state timeout duration for first lock. */
  CMU_HfxoSteadyStateTimeout_TypeDef  timeoutSteady;           /**< Steady state timeout duration. */
  uint8_t                             ctuneXoStartup;          /**< XO pin startup tuning capacitance. */
  uint8_t                             ctuneXiStartup;          /**< XI pin startup tuning capacitance. */
  uint8_t                             coreBiasStartup;         /**< Core bias startup current. */
  uint8_t                             imCoreBiasStartup;       /**< Core bias intermediate startup current. */
  CMU_HfxoCoreDegen_TypeDef           coreDegenAna;            /**< Core degeneration control. */
  CMU_HfxoCtuneFixCap_TypeDef         ctuneFixAna;             /**< Fixed tuning capacitance on XI/XO. */
  uint8_t                             ctuneXoAna;              /**< Tuning capacitance on XO. */
  uint8_t                             ctuneXiAna;              /**< Tuning capacitance on XI. */
  uint8_t                             coreBiasAna;             /**< Core bias current. */
  bool                                enXiDcBiasAna;           /**< Enable XI internal DC bias. */
  CMU_HfxoOscMode_TypeDef             mode;                    /**< Oscillator mode. */
  bool                                forceXo2GndAna;          /**< Force XO pin to ground. */
  bool                                forceXi2GndAna;          /**< Force XI pin to ground. */
  bool                                disOnDemand;             /**< Disable on-demand requests. */
  bool                                forceEn;                 /**< Force oscillator enable. */
  bool                                regLock;                 /**< Lock register access. */
} CMU_HFXOInit_TypeDef;

/** Default HFXO initialization values for XTAL mode. */
#define CMU_HFXOINIT_DEFAULT                                        \
  {                                                                 \
    cmuHfxoCbLsbTimeout_416us,                                      \
    cmuHfxoSteadyStateTimeout_833us,  /* First lock              */ \
    cmuHfxoSteadyStateTimeout_83us,   /* Subsequent locks        */ \
    0U,                         /* ctuneXoStartup                */ \
    0U,                         /* ctuneXiStartup                */ \
    32U,                        /* coreBiasStartup               */ \
    32U,                        /* imCoreBiasStartup             */ \
    cmuHfxoCoreDegen_None,                                          \
    cmuHfxoCtuneFixCap_Both,                                        \
    0U, /*    _HFXO_XTALCTRL_CTUNEXOANA_DEFAULT,  ctuneXoAna*/      \
    0U,/*    _HFXO_XTALCTRL_CTUNEXIANA_DEFAULT,  ctuneXiAna*/       \
    60U,                        /* coreBiasAna                   */ \
    false,                      /* enXiDcBiasAna                 */ \
    cmuHfxoOscMode_Crystal,                                         \
    false,                      /* forceXo2GndAna                */ \
    false,                      /* forceXi2GndAna                */ \
    false,                      /* DisOndemand                   */ \
    false,                      /* ForceEn                       */ \
    false                       /* Lock registers                */ \
  }

/** Default HFXO initialization values for external sine mode. */
#define CMU_HFXOINIT_EXTERNAL_SINE                                            \
  {                                                                           \
    (CMU_HfxoCbLsbTimeout_TypeDef)0,       /* timeoutCbLsb                 */ \
    (CMU_HfxoSteadyStateTimeout_TypeDef)0, /* timeoutSteady, first lock    */ \
    (CMU_HfxoSteadyStateTimeout_TypeDef)0, /* timeoutSteady, subseq. locks */ \
    0U,                         /* ctuneXoStartup                */           \
    0U,                         /* ctuneXiStartup                */           \
    0U,                         /* coreBiasStartup               */           \
    0U,                         /* imCoreBiasStartup             */           \
    cmuHfxoCoreDegen_None,                                                    \
    cmuHfxoCtuneFixCap_None,                                                  \
    0U,                         /* ctuneXoAna                    */           \
    0U,                         /* ctuneXiAna                    */           \
    0U,                         /* coreBiasAna                   */           \
    false, /* enXiDcBiasAna, false=DC true=AC coupling of signal */           \
    cmuHfxoOscMode_ExternalSine,                                              \
    false,                      /* forceXo2GndAna                */           \
    false,                      /* forceXi2GndAna                */           \
    false,                      /* DisOndemand                   */           \
    false,                      /* ForceEn                       */           \
    false                       /* Lock registers                */           \
  }

/** Default HFXO initialization values for external sine mode with peak detector. */
#define CMU_HFXOINIT_EXTERNAL_SINEPKDET                                       \
  {                                                                           \
    (CMU_HfxoCbLsbTimeout_TypeDef)0,       /* timeoutCbLsb                 */ \
    (CMU_HfxoSteadyStateTimeout_TypeDef)0, /* timeoutSteady, first lock    */ \
    (CMU_HfxoSteadyStateTimeout_TypeDef)0, /* timeoutSteady, subseq. locks */ \
    0U,                         /* ctuneXoStartup                */           \
    0U,                         /* ctuneXiStartup                */           \
    0U,                         /* coreBiasStartup               */           \
    0U,                         /* imCoreBiasStartup             */           \
    cmuHfxoCoreDegen_None,                                                    \
    cmuHfxoCtuneFixCap_None,                                                  \
    0U,                         /* ctuneXoAna                    */           \
    0U,                         /* ctuneXiAna                    */           \
    0U,                         /* coreBiasAna                   */           \
    false, /* enXiDcBiasAna, false=DC true=AC coupling of signal */           \
    cmuHfxoOscMode_ExternalSinePkDet,                                         \
    false,                      /* forceXo2GndAna                */           \
    false,                      /* forceXi2GndAna                */           \
    false,                      /* DisOndemand                   */           \
    false,                      /* ForceEn                       */           \
    false                       /* Lock registers                */           \
  }

#define CMU_HFXOINIT_EXTERNAL_CLOCK   CMU_HFXOINIT_DEFAULT

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
uint32_t              CMU_Calibrate(uint32_t HFCycles, CMU_Osc_TypeDef reference);
void                  CMU_CalibrateConfig(uint32_t downCycles, CMU_Osc_TypeDef downSel,
                                          CMU_Osc_TypeDef upSel);
uint32_t              CMU_CalibrateCountGet(void);
void                  CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable);
CMU_ClkDiv_TypeDef    CMU_ClockDivGet(CMU_Clock_TypeDef clock);
void                  CMU_ClockDivSet(CMU_Clock_TypeDef clock, CMU_ClkDiv_TypeDef div);
uint32_t              CMU_ClockFreqGet(CMU_Clock_TypeDef clock);
void                  CMU_ClockSelectSet(CMU_Clock_TypeDef clock, CMU_Select_TypeDef ref);
CMU_Select_TypeDef    CMU_ClockSelectGet(CMU_Clock_TypeDef clock);
void                  CMU_FreezeEnable(bool enable);
CMU_HFRCOFreq_TypeDef CMU_HFRCOBandGet(void);
void                  CMU_HFRCOBandSet(CMU_HFRCOFreq_TypeDef setFreq);
CMU_HFRCODPLLFreq_TypeDef  CMU_HFRCODPLLBandGet(void);
void                       CMU_HFRCODPLLBandSet(CMU_HFRCODPLLFreq_TypeDef freq);
uint32_t              CMU_HFRCOStartupDelayGet(void);
void                  CMU_HFRCOStartupDelaySet(uint32_t delay);
void                  CMU_HFXOInit(const CMU_HFXOInit_TypeDef *hfxoInit);
void                  CMU_HFXOCTuneDeltaSet(int32_t delta);
int32_t               CMU_HFXOCTuneDeltaGet(void);
uint32_t              CMU_LCDClkFDIVGet(void);
void                  CMU_LCDClkFDIVSet(uint32_t div);
void                  CMU_LFXOInit(const CMU_LFXOInit_TypeDef *lfxoInit);

void                  CMU_OscillatorEnable(CMU_Osc_TypeDef osc, bool enable, bool wait);
uint32_t              CMU_OscillatorTuningGet(CMU_Osc_TypeDef osc);
void                  CMU_OscillatorTuningSet(CMU_Osc_TypeDef osc, uint32_t val);
void                  CMU_UpdateWaitStates(uint32_t freq, int vscale);
void                  CMU_UpdateWaitStates(uint32_t freq, int vscale);
void                  CMU_LFXOPrecisionSet(uint16_t precision);
bool                  CMU_PCNTClockExternalGet(unsigned int instance);
void                  CMU_PCNTClockExternalSet(unsigned int instance, bool external);

__STATIC_INLINE void CMU_CalibrateCont(bool enable)
{
  (void) enable;
}

__STATIC_INLINE void CMU_CalibrateStart(void)
{
}

__STATIC_INLINE void CMU_CalibrateStop(void)
{
}

__STATIC_INLINE uint32_t CMU_DivToLog2(CMU_ClkDiv_TypeDef div)
{
  uint32_t log2;

  /* Fixed 2^n prescalers take argument of 32768 or less. */
  EFM_ASSERT((div > 0U) && (div <= 32768U));

  /* Count leading zeroes and "reverse" result */
  log2 = (31U - __CLZ(div));

  return log2;
}

__STATIC_INLINE void CMU_IntClear(uint32_t flags)
{
  (void) flags;
}

__STATIC_INLINE void CMU_IntDisable(uint32_t flags)
{
  (void) flags;
}

__STATIC_INLINE void CMU_IntEnable(uint32_t flags)
{
  (void) flags;
}

__STATIC_INLINE uint32_t CMU_IntGet(void)
{
  return 0;
}

__STATIC_INLINE uint32_t CMU_IntGetEnabled(void)
{
  return 0;
}

__STATIC_INLINE void CMU_IntSet(uint32_t flags)
{
  (void) flags;
}
__STATIC_INLINE void CMU_Lock(void)
{
}
__STATIC_INLINE void CMU_Unlock(void)
{
}

#if !defined(_SILICON_LABS_32B_SERIES_0)
/***************************************************************************//**
 * @brief
 *   Convert prescaler dividend to a logarithmic value. It only works for even
 *   numbers equal to 2^n.
 *
 * @param[in] presc
 *   An unscaled dividend (dividend = presc + 1).
 *
 * @return
 *   Logarithm of 2, as used by fixed 2^n prescalers.
 ******************************************************************************/
__STATIC_INLINE uint32_t CMU_PrescToLog2(uint32_t presc)
{
  uint32_t log2;

  /* Integer prescalers take argument less than 32768. */
  EFM_ASSERT(presc < 32768U);

  /* Count leading zeroes and "reverse" result. */
  log2 = 31UL - __CLZ(presc + (uint32_t) 1);

  /* Check that prescaler is a 2^n number. */
  EFM_ASSERT(presc == (SL_Log2ToDiv(log2) - 1U));

  return log2;
}
#endif // !defined(_SILICON_LABS_32B_SERIES_0)

/** @} (end addtogroup CMU) */
/** @} (end addtogroup emlib) */

#ifdef __cplusplus
}
#endif

#endif /* #if defined(CMU_PRESENT) && defined(LYNX_FPGA) */
#endif /* EM_CMU_FPGA_H */
