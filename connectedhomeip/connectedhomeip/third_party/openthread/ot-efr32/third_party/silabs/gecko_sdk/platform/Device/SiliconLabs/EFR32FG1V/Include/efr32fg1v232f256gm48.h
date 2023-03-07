/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer Header File
 *        for EFR32FG1V232F256GM48
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

#ifndef EFR32FG1V232F256GM48_H
#define EFR32FG1V232F256GM48_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48 EFR32FG1V232F256GM48
 * @{
 ******************************************************************************/

/** Interrupt Number Definition */
typedef enum IRQn{
/******  Cortex-M4 Processor Exceptions Numbers ********************************************/
  NonMaskableInt_IRQn   = -14,              /*!< 2  Cortex-M4 Non Maskable Interrupt      */
  HardFault_IRQn        = -13,              /*!< 3  Cortex-M4 Hard Fault Interrupt        */
  MemoryManagement_IRQn = -12,              /*!< 4  Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn         = -11,              /*!< 5  Cortex-M4 Bus Fault Interrupt         */
  UsageFault_IRQn       = -10,              /*!< 6  Cortex-M4 Usage Fault Interrupt       */
  SVCall_IRQn           = -5,               /*!< 11 Cortex-M4 SV Call Interrupt           */
  DebugMonitor_IRQn     = -4,               /*!< 12 Cortex-M4 Debug Monitor Interrupt     */
  PendSV_IRQn           = -2,               /*!< 14 Cortex-M4 Pend SV Interrupt           */
  SysTick_IRQn          = -1,               /*!< 15 Cortex-M4 System Tick Interrupt       */

/******  EFR32FG1V Peripheral Interrupt Numbers ********************************************/

  EMU_IRQn              = 0,  /*!< 16+0 EFR32 EMU Interrupt */
  FRC_PRI_IRQn          = 1,  /*!< 16+1 EFR32 FRC_PRI Interrupt */
  WDOG0_IRQn            = 2,  /*!< 16+2 EFR32 WDOG0 Interrupt */
  FRC_IRQn              = 3,  /*!< 16+3 EFR32 FRC Interrupt */
  MODEM_IRQn            = 4,  /*!< 16+4 EFR32 MODEM Interrupt */
  RAC_SEQ_IRQn          = 5,  /*!< 16+5 EFR32 RAC_SEQ Interrupt */
  RAC_RSM_IRQn          = 6,  /*!< 16+6 EFR32 RAC_RSM Interrupt */
  BUFC_IRQn             = 7,  /*!< 16+7 EFR32 BUFC Interrupt */
  LDMA_IRQn             = 8,  /*!< 16+8 EFR32 LDMA Interrupt */
  GPIO_EVEN_IRQn        = 9,  /*!< 16+9 EFR32 GPIO_EVEN Interrupt */
  TIMER0_IRQn           = 10, /*!< 16+10 EFR32 TIMER0 Interrupt */
  USART0_RX_IRQn        = 11, /*!< 16+11 EFR32 USART0_RX Interrupt */
  USART0_TX_IRQn        = 12, /*!< 16+12 EFR32 USART0_TX Interrupt */
  ADC0_IRQn             = 14, /*!< 16+14 EFR32 ADC0 Interrupt */
  I2C0_IRQn             = 16, /*!< 16+16 EFR32 I2C0 Interrupt */
  GPIO_ODD_IRQn         = 17, /*!< 16+17 EFR32 GPIO_ODD Interrupt */
  TIMER1_IRQn           = 18, /*!< 16+18 EFR32 TIMER1 Interrupt */
  USART1_RX_IRQn        = 19, /*!< 16+19 EFR32 USART1_RX Interrupt */
  USART1_TX_IRQn        = 20, /*!< 16+20 EFR32 USART1_TX Interrupt */
  LEUART0_IRQn          = 21, /*!< 16+21 EFR32 LEUART0 Interrupt */
  CMU_IRQn              = 23, /*!< 16+23 EFR32 CMU Interrupt */
  MSC_IRQn              = 24, /*!< 16+24 EFR32 MSC Interrupt */
  CRYPTO_IRQn           = 25, /*!< 16+25 EFR32 CRYPTO Interrupt */
  LETIMER0_IRQn         = 26, /*!< 16+26 EFR32 LETIMER0 Interrupt */
  AGC_IRQn              = 27, /*!< 16+27 EFR32 AGC Interrupt */
  PROTIMER_IRQn         = 28, /*!< 16+28 EFR32 PROTIMER Interrupt */
  RTCC_IRQn             = 29, /*!< 16+29 EFR32 RTCC Interrupt */
  SYNTH_IRQn            = 30, /*!< 16+30 EFR32 SYNTH Interrupt */
  CRYOTIMER_IRQn        = 31, /*!< 16+31 EFR32 CRYOTIMER Interrupt */
  RFSENSE_IRQn          = 32, /*!< 16+32 EFR32 RFSENSE Interrupt */
  FPUEH_IRQn            = 33, /*!< 16+33 EFR32 FPUEH Interrupt */
} IRQn_Type;

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Core Core
 * @{
 * @brief Processor and Core Peripheral Section
 ******************************************************************************/
#define __MPU_PRESENT             1U /**< Presence of MPU  */
#define __FPU_PRESENT             1U /**< Presence of FPU  */
#define __VTOR_PRESENT            1U /**< Presence of VTOR register in SCB */
#define __NVIC_PRIO_BITS          3U /**< NVIC interrupt priority bits */
#define __Vendor_SysTickConfig    0U /**< Is 1 if different SysTick counter is used */

/** @} End of group EFR32FG1V232F256GM48_Core */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Part Part
 * @{
 ******************************************************************************/

/** Part family */
#define _EFR32_FLEX_FAMILY                      1                               /**< FLEX Gecko RF SoC Family  */
#define _EFR_DEVICE                                                             /**< Silicon Labs EFR-type RF SoC */
#define _SILICON_LABS_32B_SERIES_1                                              /**< Silicon Labs series number */
#define _SILICON_LABS_32B_SERIES                1                               /**< Silicon Labs series number */
#define _SILICON_LABS_32B_SERIES_1_CONFIG_1                                     /**< Series 1, Configuration 1 */
#define _SILICON_LABS_32B_SERIES_1_CONFIG       1                               /**< Series 1, Configuration 1 */
#define _SILICON_LABS_GECKO_INTERNAL_SDID       80                              /**< Silicon Labs internal use only, may change any time */
#define _SILICON_LABS_GECKO_INTERNAL_SDID_80                                    /**< Silicon Labs internal use only, may change any time */
#define _SILICON_LABS_EFR32_RADIO_SUBGHZ        1                               /**< Radio supports Sub-GHz */
#define _SILICON_LABS_EFR32_RADIO_2G4HZ         2                               /**< Radio supports 2.4 GHz */
#define _SILICON_LABS_EFR32_RADIO_DUALBAND      3                               /**< Radio supports dual band */
#define _SILICON_LABS_EFR32_RADIO_TYPE          _SILICON_LABS_EFR32_RADIO_2G4HZ /**< Radio type */
#define _SILICON_LABS_32B_PLATFORM_2                                            /**< @deprecated Silicon Labs platform name */
#define _SILICON_LABS_32B_PLATFORM              2                               /**< @deprecated Silicon Labs platform name */
#define _SILICON_LABS_32B_PLATFORM_2_GEN_1                                      /**< @deprecated Platform 2, generation 1 */
#define _SILICON_LABS_32B_PLATFORM_2_GEN        1                               /**< @deprecated Platform 2, generation 1 */

/* If part number is not defined as compiler option, define it */
#if !defined(EFR32FG1V232F256GM48)
#define EFR32FG1V232F256GM48    1 /**< FLEX Gecko Part */
#endif

/** Configure part number */
#define PART_NUMBER               "EFR32FG1V232F256GM48" /**< Part Number */

/** Memory Base addresses and limits */
#define CRYPTO_MEM_BASE           (0x400F0000UL) /**< CRYPTO base address  */
#define CRYPTO_MEM_SIZE           (0x400UL)      /**< CRYPTO available address space  */
#define CRYPTO_MEM_END            (0x400F03FFUL) /**< CRYPTO end address  */
#define CRYPTO_MEM_BITS           (0x0000000AUL) /**< CRYPTO used bits  */
#define RAM_MEM_BASE              (0x20000000UL) /**< RAM base address  */
#define RAM_MEM_SIZE              (0x7C00UL)     /**< RAM available address space  */
#define RAM_MEM_END               (0x20007BFFUL) /**< RAM end address  */
#define RAM_MEM_BITS              (0x0000000FUL) /**< RAM used bits  */
#define PER_BITSET_MEM_BASE       (0x46000000UL) /**< PER_BITSET base address  */
#define PER_BITSET_MEM_SIZE       (0xE8000UL)    /**< PER_BITSET available address space  */
#define PER_BITSET_MEM_END        (0x460E7FFFUL) /**< PER_BITSET end address  */
#define PER_BITSET_MEM_BITS       (0x00000014UL) /**< PER_BITSET used bits  */
#define CRYPTO_BITSET_MEM_BASE    (0x460F0000UL) /**< CRYPTO_BITSET base address  */
#define CRYPTO_BITSET_MEM_SIZE    (0x400UL)      /**< CRYPTO_BITSET available address space  */
#define CRYPTO_BITSET_MEM_END     (0x460F03FFUL) /**< CRYPTO_BITSET end address  */
#define CRYPTO_BITSET_MEM_BITS    (0x0000000AUL) /**< CRYPTO_BITSET used bits  */
#define PER_MEM_BASE              (0x40000000UL) /**< PER base address  */
#define PER_MEM_SIZE              (0xE8000UL)    /**< PER available address space  */
#define PER_MEM_END               (0x400E7FFFUL) /**< PER end address  */
#define PER_MEM_BITS              (0x00000014UL) /**< PER used bits  */
#define CRYPTO_BITCLR_MEM_BASE    (0x440F0000UL) /**< CRYPTO_BITCLR base address  */
#define CRYPTO_BITCLR_MEM_SIZE    (0x400UL)      /**< CRYPTO_BITCLR available address space  */
#define CRYPTO_BITCLR_MEM_END     (0x440F03FFUL) /**< CRYPTO_BITCLR end address  */
#define CRYPTO_BITCLR_MEM_BITS    (0x0000000AUL) /**< CRYPTO_BITCLR used bits  */
#define RAM_CODE_MEM_BASE         (0x10000000UL) /**< RAM_CODE base address  */
#define RAM_CODE_MEM_SIZE         (0x7C00UL)     /**< RAM_CODE available address space  */
#define RAM_CODE_MEM_END          (0x10007BFFUL) /**< RAM_CODE end address  */
#define RAM_CODE_MEM_BITS         (0x0000000FUL) /**< RAM_CODE used bits  */
#define FLASH_MEM_BASE            (0x00000000UL) /**< FLASH base address  */
#define FLASH_MEM_SIZE            (0x10000000UL) /**< FLASH available address space  */
#define FLASH_MEM_END             (0x0FFFFFFFUL) /**< FLASH end address  */
#define FLASH_MEM_BITS            (0x0000001CUL) /**< FLASH used bits  */
#define PER_BITCLR_MEM_BASE       (0x44000000UL) /**< PER_BITCLR base address  */
#define PER_BITCLR_MEM_SIZE       (0xE8000UL)    /**< PER_BITCLR available address space  */
#define PER_BITCLR_MEM_END        (0x440E7FFFUL) /**< PER_BITCLR end address  */
#define PER_BITCLR_MEM_BITS       (0x00000014UL) /**< PER_BITCLR used bits  */

/** Bit banding area */
#define BITBAND_PER_BASE          (0x42000000UL) /**< Peripheral Address Space bit-band area */
#define BITBAND_RAM_BASE          (0x22000000UL) /**< SRAM Address Space bit-band area */

/** Flash and SRAM limits for EFR32FG1V232F256GM48 */
#define FLASH_BASE                (0x00000000UL) /**< Flash Base Address */
#define FLASH_SIZE                (0x00040000UL) /**< Available Flash Memory */
#define FLASH_PAGE_SIZE           2048U          /**< Flash Memory page size */
#define SRAM_BASE                 (0x20000000UL) /**< SRAM Base Address */
#define SRAM_SIZE                 (0x00007C00UL) /**< Available SRAM Memory */
#define __CM4_REV                 0x0001U        /**< Cortex-M4 Core revision r0p1 */
#define PRS_CHAN_COUNT            12             /**< Number of PRS channels */
#define DMA_CHAN_COUNT            8              /**< Number of DMA channels */
#define EXT_IRQ_COUNT             34             /**< Number of External (NVIC) interrupts */

/** AF channels connect the different on-chip peripherals with the af-mux */
#define AFCHAN_MAX                72U
/** AF channel maximum location number */
#define AFCHANLOC_MAX             32U
/** Analog AF channels */
#define AFACHAN_MAX               61U

/* Part number capabilities */

#define TIMER_PRESENT           /**< TIMER is available in this part */
#define TIMER_COUNT           2 /**< 2 TIMERs available  */
#define USART_PRESENT           /**< USART is available in this part */
#define USART_COUNT           2 /**< 2 USARTs available  */
#define LEUART_PRESENT          /**< LEUART is available in this part */
#define LEUART_COUNT          1 /**< 1 LEUARTs available  */
#define LETIMER_PRESENT         /**< LETIMER is available in this part */
#define LETIMER_COUNT         1 /**< 1 LETIMERs available  */
#define I2C_PRESENT             /**< I2C is available in this part */
#define I2C_COUNT             1 /**< 1 I2Cs available  */
#define ADC_PRESENT             /**< ADC is available in this part */
#define ADC_COUNT             1 /**< 1 ADCs available  */
#define WDOG_PRESENT            /**< WDOG is available in this part */
#define WDOG_COUNT            1 /**< 1 WDOGs available  */
#define MSC_PRESENT             /**< MSC is available in this part */
#define MSC_COUNT             1 /**< 1 MSC available */
#define EMU_PRESENT             /**< EMU is available in this part */
#define EMU_COUNT             1 /**< 1 EMU available */
#define RMU_PRESENT             /**< RMU is available in this part */
#define RMU_COUNT             1 /**< 1 RMU available */
#define CMU_PRESENT             /**< CMU is available in this part */
#define CMU_COUNT             1 /**< 1 CMU available */
#define CRYPTO_PRESENT          /**< CRYPTO is available in this part */
#define CRYPTO_COUNT          1 /**< 1 CRYPTO available */
#define GPIO_PRESENT            /**< GPIO is available in this part */
#define GPIO_COUNT            1 /**< 1 GPIO available */
#define PRS_PRESENT             /**< PRS is available in this part */
#define PRS_COUNT             1 /**< 1 PRS available */
#define LDMA_PRESENT            /**< LDMA is available in this part */
#define LDMA_COUNT            1 /**< 1 LDMA available */
#define FPUEH_PRESENT           /**< FPUEH is available in this part */
#define FPUEH_COUNT           1 /**< 1 FPUEH available */
#define GPCRC_PRESENT           /**< GPCRC is available in this part */
#define GPCRC_COUNT           1 /**< 1 GPCRC available */
#define CRYOTIMER_PRESENT       /**< CRYOTIMER is available in this part */
#define CRYOTIMER_COUNT       1 /**< 1 CRYOTIMER available */
#define RTCC_PRESENT            /**< RTCC is available in this part */
#define RTCC_COUNT            1 /**< 1 RTCC available */
#define BOOTLOADER_PRESENT      /**< BOOTLOADER is available in this part */
#define BOOTLOADER_COUNT      1 /**< 1 BOOTLOADER available */
#define DCDC_PRESENT            /**< DCDC is available in this part */
#define DCDC_COUNT            1 /**< 1 DCDC available */

#include "core_cm4.h"           /* Cortex-M4 processor and core peripherals */
#include "system_efr32fg1v.h"   /* System Header File */

/** @} End of group EFR32FG1V232F256GM48_Part */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Peripheral_TypeDefs Peripheral TypeDefs
 * @{
 * @brief Device Specific Peripheral Register Structures
 ******************************************************************************/

#include "efr32fg1v_msc.h"
#include "efr32fg1v_emu.h"
#include "efr32fg1v_rmu.h"

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_CMU CMU
 * @{
 * @brief EFR32FG1V232F256GM48_CMU Register Declaration
 ******************************************************************************/
/** CMU Register Declaration */
typedef struct {
  __IOM uint32_t CTRL;                /**< CMU Control Register  */

  uint32_t       RESERVED0[3U];       /**< Reserved for future use **/
  __IOM uint32_t HFRCOCTRL;           /**< HFRCO Control Register  */

  uint32_t       RESERVED1[1U];       /**< Reserved for future use **/
  __IOM uint32_t AUXHFRCOCTRL;        /**< AUXHFRCO Control Register  */

  uint32_t       RESERVED2[1U];       /**< Reserved for future use **/
  __IOM uint32_t LFRCOCTRL;           /**< LFRCO Control Register  */
  __IOM uint32_t HFXOCTRL;            /**< HFXO Control Register  */
  __IOM uint32_t HFXOCTRL1;           /**< HFXO Control 1  */
  __IOM uint32_t HFXOSTARTUPCTRL;     /**< HFXO Startup Control  */
  __IOM uint32_t HFXOSTEADYSTATECTRL; /**< HFXO Steady State Control  */
  __IOM uint32_t HFXOTIMEOUTCTRL;     /**< HFXO Timeout Control  */
  __IOM uint32_t LFXOCTRL;            /**< LFXO Control Register  */
  __IOM uint32_t ULFRCOCTRL;          /**< ULFRCO Control Register  */

  uint32_t       RESERVED3[4U];       /**< Reserved for future use **/
  __IOM uint32_t CALCTRL;             /**< Calibration Control Register  */
  __IOM uint32_t CALCNT;              /**< Calibration Counter Register  */
  uint32_t       RESERVED4[2U];       /**< Reserved for future use **/
  __IOM uint32_t OSCENCMD;            /**< Oscillator Enable/Disable Command Register  */
  __IOM uint32_t CMD;                 /**< Command Register  */
  uint32_t       RESERVED5[2U];       /**< Reserved for future use **/
  __IOM uint32_t DBGCLKSEL;           /**< Debug Trace Clock Select  */
  __IOM uint32_t HFCLKSEL;            /**< High Frequency Clock Select Command Register  */
  uint32_t       RESERVED6[2U];       /**< Reserved for future use **/
  __IOM uint32_t LFACLKSEL;           /**< Low Frequency A Clock Select Register  */
  __IOM uint32_t LFBCLKSEL;           /**< Low Frequency B Clock Select Register  */
  __IOM uint32_t LFECLKSEL;           /**< Low Frequency E Clock Select Register  */

  uint32_t       RESERVED7[1U];       /**< Reserved for future use **/
  __IM uint32_t  STATUS;              /**< Status Register  */
  __IM uint32_t  HFCLKSTATUS;         /**< HFCLK Status Register  */
  uint32_t       RESERVED8[1U];       /**< Reserved for future use **/
  __IM uint32_t  HFXOTRIMSTATUS;      /**< HFXO Trim Status  */
  __IM uint32_t  IF;                  /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;                 /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;                 /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN;                 /**< Interrupt Enable Register  */
  __IOM uint32_t HFBUSCLKEN0;         /**< High Frequency Bus Clock Enable Register 0  */

  uint32_t       RESERVED9[3U];       /**< Reserved for future use **/
  __IOM uint32_t HFPERCLKEN0;         /**< High Frequency Peripheral Clock Enable Register 0  */

  uint32_t       RESERVED10[7U];      /**< Reserved for future use **/
  __IOM uint32_t LFACLKEN0;           /**< Low Frequency a Clock Enable Register 0  (Async Reg)  */
  uint32_t       RESERVED11[1U];      /**< Reserved for future use **/
  __IOM uint32_t LFBCLKEN0;           /**< Low Frequency B Clock Enable Register 0 (Async Reg)  */

  uint32_t       RESERVED12[1U];      /**< Reserved for future use **/
  __IOM uint32_t LFECLKEN0;           /**< Low Frequency E Clock Enable Register 0 (Async Reg)  */
  uint32_t       RESERVED13[3U];      /**< Reserved for future use **/
  __IOM uint32_t HFPRESC;             /**< High Frequency Clock Prescaler Register  */

  uint32_t       RESERVED14[1U];      /**< Reserved for future use **/
  __IOM uint32_t HFCOREPRESC;         /**< High Frequency Core Clock Prescaler Register  */
  __IOM uint32_t HFPERPRESC;          /**< High Frequency Peripheral Clock Prescaler Register  */

  uint32_t       RESERVED15[1U];      /**< Reserved for future use **/
  __IOM uint32_t HFEXPPRESC;          /**< High Frequency Export Clock Prescaler Register  */

  uint32_t       RESERVED16[2U];      /**< Reserved for future use **/
  __IOM uint32_t LFAPRESC0;           /**< Low Frequency a Prescaler Register 0 (Async Reg)  */
  uint32_t       RESERVED17[1U];      /**< Reserved for future use **/
  __IOM uint32_t LFBPRESC0;           /**< Low Frequency B Prescaler Register 0  (Async Reg)  */
  uint32_t       RESERVED18[1U];      /**< Reserved for future use **/
  __IOM uint32_t LFEPRESC0;           /**< Low Frequency E Prescaler Register 0  (Async Reg)  */

  uint32_t       RESERVED19[3U];      /**< Reserved for future use **/
  __IM uint32_t  SYNCBUSY;            /**< Synchronization Busy Register  */
  __IOM uint32_t FREEZE;              /**< Freeze Register  */
  uint32_t       RESERVED20[2U];      /**< Reserved for future use **/
  __IOM uint32_t PCNTCTRL;            /**< PCNT Control Register  */

  uint32_t       RESERVED21[2U];      /**< Reserved for future use **/
  __IOM uint32_t ADCCTRL;             /**< ADC Control Register  */

  uint32_t       RESERVED22[4U];      /**< Reserved for future use **/
  __IOM uint32_t ROUTEPEN;            /**< I/O Routing Pin Enable Register  */
  __IOM uint32_t ROUTELOC0;           /**< I/O Routing Location Register  */

  uint32_t       RESERVED23[2U];      /**< Reserved for future use **/
  __IOM uint32_t LOCK;                /**< Configuration Lock Register  */
} CMU_TypeDef;                        /** @} */

#include "efr32fg1v_crypto.h"
#include "efr32fg1v_gpio_p.h"
#include "efr32fg1v_gpio.h"
#include "efr32fg1v_prs_ch.h"

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_PRS PRS
 * @{
 * @brief EFR32FG1V232F256GM48_PRS Register Declaration
 ******************************************************************************/
/** PRS Register Declaration */
typedef struct {
  __IOM uint32_t SWPULSE;       /**< Software Pulse Register  */
  __IOM uint32_t SWLEVEL;       /**< Software Level Register  */
  __IOM uint32_t ROUTEPEN;      /**< I/O Routing Pin Enable Register  */
  uint32_t       RESERVED0[1U]; /**< Reserved for future use **/
  __IOM uint32_t ROUTELOC0;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC1;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC2;     /**< I/O Routing Location Register  */

  uint32_t       RESERVED1[1U]; /**< Reserved for future use **/
  __IOM uint32_t CTRL;          /**< Control Register  */
  __IOM uint32_t DMAREQ0;       /**< DMA Request 0 Register  */
  __IOM uint32_t DMAREQ1;       /**< DMA Request 1 Register  */
  uint32_t       RESERVED2[1U]; /**< Reserved for future use **/
  __IM uint32_t  PEEK;          /**< PRS Channel Values  */

  uint32_t       RESERVED3[3U]; /**< Reserved registers */
  PRS_CH_TypeDef CH[12U];       /**< Channel registers */
} PRS_TypeDef;                  /** @} */

#include "efr32fg1v_ldma_ch.h"
#include "efr32fg1v_ldma.h"
#include "efr32fg1v_fpueh.h"
#include "efr32fg1v_gpcrc.h"
#include "efr32fg1v_timer_cc.h"
#include "efr32fg1v_timer.h"
#include "efr32fg1v_usart.h"
#include "efr32fg1v_leuart.h"
#include "efr32fg1v_letimer.h"
#include "efr32fg1v_cryotimer.h"
#include "efr32fg1v_i2c.h"
#include "efr32fg1v_adc.h"
#include "efr32fg1v_rtcc_cc.h"
#include "efr32fg1v_rtcc_ret.h"
#include "efr32fg1v_rtcc.h"
#include "efr32fg1v_wdog_pch.h"
#include "efr32fg1v_wdog.h"
#include "efr32fg1v_dma_descriptor.h"
#include "efr32fg1v_devinfo.h"
#include "efr32fg1v_romtable.h"

/** @} End of group EFR32FG1V232F256GM48_Peripheral_TypeDefs  */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Peripheral_Base Peripheral Memory Map
 * @{
 ******************************************************************************/

#define MSC_BASE          (0x400E0000UL) /**< MSC base address  */
#define EMU_BASE          (0x400E3000UL) /**< EMU base address  */
#define RMU_BASE          (0x400E5000UL) /**< RMU base address  */
#define CMU_BASE          (0x400E4000UL) /**< CMU base address  */
#define CRYPTO_BASE       (0x400F0000UL) /**< CRYPTO base address  */
#define GPIO_BASE         (0x4000A000UL) /**< GPIO base address  */
#define PRS_BASE          (0x400E6000UL) /**< PRS base address  */
#define LDMA_BASE         (0x400E2000UL) /**< LDMA base address  */
#define FPUEH_BASE        (0x400E1000UL) /**< FPUEH base address  */
#define GPCRC_BASE        (0x4001C000UL) /**< GPCRC base address  */
#define TIMER0_BASE       (0x40018000UL) /**< TIMER0 base address  */
#define TIMER1_BASE       (0x40018400UL) /**< TIMER1 base address  */
#define USART0_BASE       (0x40010000UL) /**< USART0 base address  */
#define USART1_BASE       (0x40010400UL) /**< USART1 base address  */
#define LEUART0_BASE      (0x4004A000UL) /**< LEUART0 base address  */
#define LETIMER0_BASE     (0x40046000UL) /**< LETIMER0 base address  */
#define CRYOTIMER_BASE    (0x4001E000UL) /**< CRYOTIMER base address  */
#define I2C0_BASE         (0x4000C000UL) /**< I2C0 base address  */
#define ADC0_BASE         (0x40002000UL) /**< ADC0 base address  */
#define RTCC_BASE         (0x40042000UL) /**< RTCC base address  */
#define WDOG0_BASE        (0x40052000UL) /**< WDOG0 base address  */
#define DEVINFO_BASE      (0x0FE081B0UL) /**< DEVINFO base address */
#define ROMTABLE_BASE     (0xE00FFFD0UL) /**< ROMTABLE base address */
#define LOCKBITS_BASE     (0x0FE04000UL) /**< Lock-bits page base address */
#define USERDATA_BASE     (0x0FE00000UL) /**< User data page base address */

/** @} End of group EFR32FG1V232F256GM48_Peripheral_Base */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Peripheral_Declaration Peripheral Declarations
 * @{
 ******************************************************************************/

#define MSC          ((MSC_TypeDef *) MSC_BASE)             /**< MSC base pointer */
#define EMU          ((EMU_TypeDef *) EMU_BASE)             /**< EMU base pointer */
#define RMU          ((RMU_TypeDef *) RMU_BASE)             /**< RMU base pointer */
#define CMU          ((CMU_TypeDef *) CMU_BASE)             /**< CMU base pointer */
#define CRYPTO       ((CRYPTO_TypeDef *) CRYPTO_BASE)       /**< CRYPTO base pointer */
#define GPIO         ((GPIO_TypeDef *) GPIO_BASE)           /**< GPIO base pointer */
#define PRS          ((PRS_TypeDef *) PRS_BASE)             /**< PRS base pointer */
#define LDMA         ((LDMA_TypeDef *) LDMA_BASE)           /**< LDMA base pointer */
#define FPUEH        ((FPUEH_TypeDef *) FPUEH_BASE)         /**< FPUEH base pointer */
#define GPCRC        ((GPCRC_TypeDef *) GPCRC_BASE)         /**< GPCRC base pointer */
#define TIMER0       ((TIMER_TypeDef *) TIMER0_BASE)        /**< TIMER0 base pointer */
#define TIMER1       ((TIMER_TypeDef *) TIMER1_BASE)        /**< TIMER1 base pointer */
#define USART0       ((USART_TypeDef *) USART0_BASE)        /**< USART0 base pointer */
#define USART1       ((USART_TypeDef *) USART1_BASE)        /**< USART1 base pointer */
#define LEUART0      ((LEUART_TypeDef *) LEUART0_BASE)      /**< LEUART0 base pointer */
#define LETIMER0     ((LETIMER_TypeDef *) LETIMER0_BASE)    /**< LETIMER0 base pointer */
#define CRYOTIMER    ((CRYOTIMER_TypeDef *) CRYOTIMER_BASE) /**< CRYOTIMER base pointer */
#define I2C0         ((I2C_TypeDef *) I2C0_BASE)            /**< I2C0 base pointer */
#define ADC0         ((ADC_TypeDef *) ADC0_BASE)            /**< ADC0 base pointer */
#define RTCC         ((RTCC_TypeDef *) RTCC_BASE)           /**< RTCC base pointer */
#define WDOG0        ((WDOG_TypeDef *) WDOG0_BASE)          /**< WDOG0 base pointer */
#define DEVINFO      ((DEVINFO_TypeDef *) DEVINFO_BASE)     /**< DEVINFO base pointer */
#define ROMTABLE     ((ROMTABLE_TypeDef *) ROMTABLE_BASE)   /**< ROMTABLE base pointer */

/** @} End of group EFR32FG1V232F256GM48_Peripheral_Declaration */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_Peripheral_Offsets Peripheral Offsets
 * @{
 ******************************************************************************/

#define TIMER_OFFSET      0x400 /**< Offset in bytes between TIMER instances */
#define USART_OFFSET      0x400 /**< Offset in bytes between USART instances */
#define LEUART_OFFSET     0x400 /**< Offset in bytes between LEUART instances */
#define LETIMER_OFFSET    0x400 /**< Offset in bytes between LETIMER instances */
#define I2C_OFFSET        0x400 /**< Offset in bytes between I2C instances */
#define ADC_OFFSET        0x400 /**< Offset in bytes between ADC instances */
#define WDOG_OFFSET       0x400 /**< Offset in bytes between WDOG instances */

/** @} End of group EFR32FG1V232F256GM48_Peripheral_Offsets */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_BitFields Bit Fields
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup EFR32FG1V232F256GM48_PRS
 * @{
 * @addtogroup EFR32FG1V232F256GM48_PRS_Signals PRS Signals
 * @{
 * @brief PRS Signal names
 ******************************************************************************/
#define PRS_PRS_CH0             ((1 << 8) + 0)  /**< PRS PRS channel 0 */
#define PRS_PRS_CH1             ((1 << 8) + 1)  /**< PRS PRS channel 1 */
#define PRS_PRS_CH2             ((1 << 8) + 2)  /**< PRS PRS channel 2 */
#define PRS_PRS_CH3             ((1 << 8) + 3)  /**< PRS PRS channel 3 */
#define PRS_PRS_CH4             ((1 << 8) + 4)  /**< PRS PRS channel 4 */
#define PRS_PRS_CH5             ((1 << 8) + 5)  /**< PRS PRS channel 5 */
#define PRS_PRS_CH6             ((1 << 8) + 6)  /**< PRS PRS channel 6 */
#define PRS_PRS_CH7             ((1 << 8) + 7)  /**< PRS PRS channel 7 */
#define PRS_PRS_CH8             ((2 << 8) + 0)  /**< PRS PRS channel 8 */
#define PRS_PRS_CH9             ((2 << 8) + 1)  /**< PRS PRS channel 9 */
#define PRS_PRS_CH10            ((2 << 8) + 2)  /**< PRS PRS channel 10 */
#define PRS_PRS_CH11            ((2 << 8) + 3)  /**< PRS PRS channel 11 */
#define PRS_ADC0_SINGLE         ((8 << 8) + 0)  /**< PRS ADC single conversion done */
#define PRS_ADC0_SCAN           ((8 << 8) + 1)  /**< PRS ADC scan conversion done */
#define PRS_USART0_IRTX         ((16 << 8) + 0) /**< PRS USART 0 IRDA out */
#define PRS_USART0_TXC          ((16 << 8) + 1) /**< PRS USART 0 TX complete */
#define PRS_USART0_RXDATAV      ((16 << 8) + 2) /**< PRS USART 0 RX Data Valid */
#define PRS_USART0_RTS          ((16 << 8) + 3) /**< PRS USART 0 RTS */
#define PRS_USART0_TX           ((16 << 8) + 5) /**< PRS USART 0 TX */
#define PRS_USART0_CS           ((16 << 8) + 6) /**< PRS USART 0 CS */
#define PRS_USART1_TXC          ((17 << 8) + 1) /**< PRS USART 1 TX complete */
#define PRS_USART1_RXDATAV      ((17 << 8) + 2) /**< PRS USART 1 RX Data Valid */
#define PRS_USART1_RTS          ((17 << 8) + 3) /**< PRS USART 0 RTS */
#define PRS_USART1_TX           ((17 << 8) + 5) /**< PRS USART 1 TX */
#define PRS_USART1_CS           ((17 << 8) + 6) /**< PRS USART 1 CS */
#define PRS_TIMER0_UF           ((28 << 8) + 0) /**< PRS Timer 0 Underflow */
#define PRS_TIMER0_OF           ((28 << 8) + 1) /**< PRS Timer 0 Overflow */
#define PRS_TIMER0_CC0          ((28 << 8) + 2) /**< PRS Timer 0 Compare/Capture 0 */
#define PRS_TIMER0_CC1          ((28 << 8) + 3) /**< PRS Timer 0 Compare/Capture 1 */
#define PRS_TIMER0_CC2          ((28 << 8) + 4) /**< PRS Timer 0 Compare/Capture 2 */
#define PRS_TIMER1_UF           ((29 << 8) + 0) /**< PRS Timer 1 Underflow */
#define PRS_TIMER1_OF           ((29 << 8) + 1) /**< PRS Timer 1 Overflow */
#define PRS_TIMER1_CC0          ((29 << 8) + 2) /**< PRS Timer 1 Compare/Capture 0 */
#define PRS_TIMER1_CC1          ((29 << 8) + 3) /**< PRS Timer 1 Compare/Capture 1 */
#define PRS_TIMER1_CC2          ((29 << 8) + 4) /**< PRS Timer 1 Compare/Capture 2 */
#define PRS_TIMER1_CC3          ((29 << 8) + 5) /**< PRS Timer 1 Compare/Capture 3 */
#define PRS_RAC_ACTIVE          ((32 << 8) + 0) /**< PRS RAC is active */
#define PRS_RAC_TX              ((32 << 8) + 1) /**< PRS RAC is in TX */
#define PRS_RAC_RX              ((32 << 8) + 2) /**< PRS RAC is in RX */
#define PRS_RAC_LNAEN           ((32 << 8) + 3) /**< PRS LNA enable */
#define PRS_RAC_PAEN            ((32 << 8) + 4) /**< PRS PA enable */
#define PRS_PROTIMER_LBTS       ((35 << 8) + 5) /**< PRS Listen Before Talk Success */
#define PRS_PROTIMER_LBTR       ((35 << 8) + 6) /**< PRS Listen Before Talk Retry */
#define PRS_PROTIMER_LBTF       ((35 << 8) + 7) /**< PRS Listen Before Talk Failure */
#define PRS_MODEM_FRAMEDET      ((38 << 8) + 0) /**< PRS Frame detected */
#define PRS_MODEM_PREDET        ((38 << 8) + 1) /**< PRS Receive preamble detected */
#define PRS_MODEM_TIMDET        ((38 << 8) + 2) /**< PRS Receive timing detected */
#define PRS_MODEM_FRAMESENT     ((38 << 8) + 3) /**< PRS Entire frame transmitted */
#define PRS_MODEM_SYNCSENT      ((38 << 8) + 4) /**< PRS Syncword transmitted */
#define PRS_MODEM_PRESENT       ((38 << 8) + 5) /**< PRS Preamble transmitted */
#define PRS_MODEM_ANT0          ((39 << 8) + 5) /**< PRS Antenna 0 select */
#define PRS_MODEM_ANT1          ((39 << 8) + 6) /**< PRS Antenna 1 select */
#define PRS_RTCC_CCV0           ((41 << 8) + 1) /**< PRS RTCC Compare 0 */
#define PRS_RTCC_CCV1           ((41 << 8) + 2) /**< PRS RTCC Compare 1 */
#define PRS_RTCC_CCV2           ((41 << 8) + 3) /**< PRS RTCC Compare 2 */
#define PRS_GPIO_PIN0           ((48 << 8) + 0) /**< PRS GPIO pin 0 */
#define PRS_GPIO_PIN1           ((48 << 8) + 1) /**< PRS GPIO pin 1 */
#define PRS_GPIO_PIN2           ((48 << 8) + 2) /**< PRS GPIO pin 2 */
#define PRS_GPIO_PIN3           ((48 << 8) + 3) /**< PRS GPIO pin 3 */
#define PRS_GPIO_PIN4           ((48 << 8) + 4) /**< PRS GPIO pin 4 */
#define PRS_GPIO_PIN5           ((48 << 8) + 5) /**< PRS GPIO pin 5 */
#define PRS_GPIO_PIN6           ((48 << 8) + 6) /**< PRS GPIO pin 6 */
#define PRS_GPIO_PIN7           ((48 << 8) + 7) /**< PRS GPIO pin 7 */
#define PRS_GPIO_PIN8           ((49 << 8) + 0) /**< PRS GPIO pin 8 */
#define PRS_GPIO_PIN9           ((49 << 8) + 1) /**< PRS GPIO pin 9 */
#define PRS_GPIO_PIN10          ((49 << 8) + 2) /**< PRS GPIO pin 10 */
#define PRS_GPIO_PIN11          ((49 << 8) + 3) /**< PRS GPIO pin 11 */
#define PRS_GPIO_PIN12          ((49 << 8) + 4) /**< PRS GPIO pin 12 */
#define PRS_GPIO_PIN13          ((49 << 8) + 5) /**< PRS GPIO pin 13 */
#define PRS_GPIO_PIN14          ((49 << 8) + 6) /**< PRS GPIO pin 14 */
#define PRS_GPIO_PIN15          ((49 << 8) + 7) /**< PRS GPIO pin 15 */
#define PRS_LETIMER0_CH0        ((52 << 8) + 0) /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER0_CH1        ((52 << 8) + 1) /**< PRS LETIMER CH1 Out */
#define PRS_RFSENSE_WU          ((59 << 8) + 0) /**< PRS RFSENSE Output */
#define PRS_CRYOTIMER_PERIOD    ((60 << 8) + 0) /**< PRS CRYOTIMER Output */
#define PRS_CMU_CLKOUT0         ((61 << 8) + 0) /**< PRS Clock Output 0 */
#define PRS_CMU_CLKOUT1         ((61 << 8) + 1) /**< PRS Clock Output 1 */
#define PRS_CM4_TXEV            ((67 << 8) + 0) /**< PRS  */

/** @} */
/** @} End of group EFR32FG1V232F256GM48_PRS */

#include "efr32fg1v_dmareq.h"

/***************************************************************************//**
 * @addtogroup EFR32FG1V232F256GM48_CMU
 * @{
 * @defgroup EFR32FG1V232F256GM48_CMU_BitFields  CMU Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for CMU CTRL */
#define _CMU_CTRL_RESETVALUE                              0x00300000UL                          /**< Default value for CMU_CTRL */
#define _CMU_CTRL_MASK                                    0x001101EFUL                          /**< Mask for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_SHIFT                        0                                     /**< Shift value for CMU_CLKOUTSEL0 */
#define _CMU_CTRL_CLKOUTSEL0_MASK                         0xFUL                                 /**< Bit mask for CMU_CLKOUTSEL0 */
#define _CMU_CTRL_CLKOUTSEL0_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_DISABLED                     0x00000000UL                          /**< Mode DISABLED for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_ULFRCO                       0x00000001UL                          /**< Mode ULFRCO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_LFRCO                        0x00000002UL                          /**< Mode LFRCO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_LFXO                         0x00000003UL                          /**< Mode LFXO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_HFXO                         0x00000006UL                          /**< Mode HFXO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_HFEXPCLK                     0x00000007UL                          /**< Mode HFEXPCLK for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_ULFRCOQ                      0x00000009UL                          /**< Mode ULFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_LFRCOQ                       0x0000000AUL                          /**< Mode LFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_LFXOQ                        0x0000000BUL                          /**< Mode LFXOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_HFRCOQ                       0x0000000CUL                          /**< Mode HFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_AUXHFRCOQ                    0x0000000DUL                          /**< Mode AUXHFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_HFXOQ                        0x0000000EUL                          /**< Mode HFXOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL0_HFSRCCLK                     0x0000000FUL                          /**< Mode HFSRCCLK for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_DEFAULT                       (_CMU_CTRL_CLKOUTSEL0_DEFAULT << 0)   /**< Shifted mode DEFAULT for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_DISABLED                      (_CMU_CTRL_CLKOUTSEL0_DISABLED << 0)  /**< Shifted mode DISABLED for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_ULFRCO                        (_CMU_CTRL_CLKOUTSEL0_ULFRCO << 0)    /**< Shifted mode ULFRCO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_LFRCO                         (_CMU_CTRL_CLKOUTSEL0_LFRCO << 0)     /**< Shifted mode LFRCO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_LFXO                          (_CMU_CTRL_CLKOUTSEL0_LFXO << 0)      /**< Shifted mode LFXO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_HFXO                          (_CMU_CTRL_CLKOUTSEL0_HFXO << 0)      /**< Shifted mode HFXO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_HFEXPCLK                      (_CMU_CTRL_CLKOUTSEL0_HFEXPCLK << 0)  /**< Shifted mode HFEXPCLK for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_ULFRCOQ                       (_CMU_CTRL_CLKOUTSEL0_ULFRCOQ << 0)   /**< Shifted mode ULFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_LFRCOQ                        (_CMU_CTRL_CLKOUTSEL0_LFRCOQ << 0)    /**< Shifted mode LFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_LFXOQ                         (_CMU_CTRL_CLKOUTSEL0_LFXOQ << 0)     /**< Shifted mode LFXOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_HFRCOQ                        (_CMU_CTRL_CLKOUTSEL0_HFRCOQ << 0)    /**< Shifted mode HFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_AUXHFRCOQ                     (_CMU_CTRL_CLKOUTSEL0_AUXHFRCOQ << 0) /**< Shifted mode AUXHFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_HFXOQ                         (_CMU_CTRL_CLKOUTSEL0_HFXOQ << 0)     /**< Shifted mode HFXOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL0_HFSRCCLK                      (_CMU_CTRL_CLKOUTSEL0_HFSRCCLK << 0)  /**< Shifted mode HFSRCCLK for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_SHIFT                        5                                     /**< Shift value for CMU_CLKOUTSEL1 */
#define _CMU_CTRL_CLKOUTSEL1_MASK                         0x1E0UL                               /**< Bit mask for CMU_CLKOUTSEL1 */
#define _CMU_CTRL_CLKOUTSEL1_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_DISABLED                     0x00000000UL                          /**< Mode DISABLED for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_ULFRCO                       0x00000001UL                          /**< Mode ULFRCO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_LFRCO                        0x00000002UL                          /**< Mode LFRCO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_LFXO                         0x00000003UL                          /**< Mode LFXO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_HFXO                         0x00000006UL                          /**< Mode HFXO for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_HFEXPCLK                     0x00000007UL                          /**< Mode HFEXPCLK for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_ULFRCOQ                      0x00000009UL                          /**< Mode ULFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_LFRCOQ                       0x0000000AUL                          /**< Mode LFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_LFXOQ                        0x0000000BUL                          /**< Mode LFXOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_HFRCOQ                       0x0000000CUL                          /**< Mode HFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_AUXHFRCOQ                    0x0000000DUL                          /**< Mode AUXHFRCOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_HFXOQ                        0x0000000EUL                          /**< Mode HFXOQ for CMU_CTRL */
#define _CMU_CTRL_CLKOUTSEL1_HFSRCCLK                     0x0000000FUL                          /**< Mode HFSRCCLK for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_DEFAULT                       (_CMU_CTRL_CLKOUTSEL1_DEFAULT << 5)   /**< Shifted mode DEFAULT for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_DISABLED                      (_CMU_CTRL_CLKOUTSEL1_DISABLED << 5)  /**< Shifted mode DISABLED for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_ULFRCO                        (_CMU_CTRL_CLKOUTSEL1_ULFRCO << 5)    /**< Shifted mode ULFRCO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_LFRCO                         (_CMU_CTRL_CLKOUTSEL1_LFRCO << 5)     /**< Shifted mode LFRCO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_LFXO                          (_CMU_CTRL_CLKOUTSEL1_LFXO << 5)      /**< Shifted mode LFXO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_HFXO                          (_CMU_CTRL_CLKOUTSEL1_HFXO << 5)      /**< Shifted mode HFXO for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_HFEXPCLK                      (_CMU_CTRL_CLKOUTSEL1_HFEXPCLK << 5)  /**< Shifted mode HFEXPCLK for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_ULFRCOQ                       (_CMU_CTRL_CLKOUTSEL1_ULFRCOQ << 5)   /**< Shifted mode ULFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_LFRCOQ                        (_CMU_CTRL_CLKOUTSEL1_LFRCOQ << 5)    /**< Shifted mode LFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_LFXOQ                         (_CMU_CTRL_CLKOUTSEL1_LFXOQ << 5)     /**< Shifted mode LFXOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_HFRCOQ                        (_CMU_CTRL_CLKOUTSEL1_HFRCOQ << 5)    /**< Shifted mode HFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_AUXHFRCOQ                     (_CMU_CTRL_CLKOUTSEL1_AUXHFRCOQ << 5) /**< Shifted mode AUXHFRCOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_HFXOQ                         (_CMU_CTRL_CLKOUTSEL1_HFXOQ << 5)     /**< Shifted mode HFXOQ for CMU_CTRL */
#define CMU_CTRL_CLKOUTSEL1_HFSRCCLK                      (_CMU_CTRL_CLKOUTSEL1_HFSRCCLK << 5)  /**< Shifted mode HFSRCCLK for CMU_CTRL */
#define CMU_CTRL_WSHFLE                                   (0x1UL << 16)                         /**< Wait State for High-Frequency LE Interface */
#define _CMU_CTRL_WSHFLE_SHIFT                            16                                    /**< Shift value for CMU_WSHFLE */
#define _CMU_CTRL_WSHFLE_MASK                             0x10000UL                             /**< Bit mask for CMU_WSHFLE */
#define _CMU_CTRL_WSHFLE_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for CMU_CTRL */
#define CMU_CTRL_WSHFLE_DEFAULT                           (_CMU_CTRL_WSHFLE_DEFAULT << 16)      /**< Shifted mode DEFAULT for CMU_CTRL */
#define CMU_CTRL_HFPERCLKEN                               (0x1UL << 20)                         /**< HFPERCLK Enable */
#define _CMU_CTRL_HFPERCLKEN_SHIFT                        20                                    /**< Shift value for CMU_HFPERCLKEN */
#define _CMU_CTRL_HFPERCLKEN_MASK                         0x100000UL                            /**< Bit mask for CMU_HFPERCLKEN */
#define _CMU_CTRL_HFPERCLKEN_DEFAULT                      0x00000001UL                          /**< Mode DEFAULT for CMU_CTRL */
#define CMU_CTRL_HFPERCLKEN_DEFAULT                       (_CMU_CTRL_HFPERCLKEN_DEFAULT << 20)  /**< Shifted mode DEFAULT for CMU_CTRL */

/* Bit fields for CMU HFRCOCTRL */
#define _CMU_HFRCOCTRL_RESETVALUE                         0xB1481F3CUL                                /**< Default value for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_MASK                               0xFFFF3F7FUL                                /**< Mask for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_TUNING_SHIFT                       0                                           /**< Shift value for CMU_TUNING */
#define _CMU_HFRCOCTRL_TUNING_MASK                        0x7FUL                                      /**< Bit mask for CMU_TUNING */
#define _CMU_HFRCOCTRL_TUNING_DEFAULT                     0x0000003CUL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_TUNING_DEFAULT                      (_CMU_HFRCOCTRL_TUNING_DEFAULT << 0)        /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_FINETUNING_SHIFT                   8                                           /**< Shift value for CMU_FINETUNING */
#define _CMU_HFRCOCTRL_FINETUNING_MASK                    0x3F00UL                                    /**< Bit mask for CMU_FINETUNING */
#define _CMU_HFRCOCTRL_FINETUNING_DEFAULT                 0x0000001FUL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_FINETUNING_DEFAULT                  (_CMU_HFRCOCTRL_FINETUNING_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_FREQRANGE_SHIFT                    16                                          /**< Shift value for CMU_FREQRANGE */
#define _CMU_HFRCOCTRL_FREQRANGE_MASK                     0x1F0000UL                                  /**< Bit mask for CMU_FREQRANGE */
#define _CMU_HFRCOCTRL_FREQRANGE_DEFAULT                  0x00000008UL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_FREQRANGE_DEFAULT                   (_CMU_HFRCOCTRL_FREQRANGE_DEFAULT << 16)    /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_CMPBIAS_SHIFT                      21                                          /**< Shift value for CMU_CMPBIAS */
#define _CMU_HFRCOCTRL_CMPBIAS_MASK                       0xE00000UL                                  /**< Bit mask for CMU_CMPBIAS */
#define _CMU_HFRCOCTRL_CMPBIAS_DEFAULT                    0x00000002UL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_CMPBIAS_DEFAULT                     (_CMU_HFRCOCTRL_CMPBIAS_DEFAULT << 21)      /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_LDOHP                               (0x1UL << 24)                               /**< HFRCO LDO High Power Mode */
#define _CMU_HFRCOCTRL_LDOHP_SHIFT                        24                                          /**< Shift value for CMU_LDOHP */
#define _CMU_HFRCOCTRL_LDOHP_MASK                         0x1000000UL                                 /**< Bit mask for CMU_LDOHP */
#define _CMU_HFRCOCTRL_LDOHP_DEFAULT                      0x00000001UL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_LDOHP_DEFAULT                       (_CMU_HFRCOCTRL_LDOHP_DEFAULT << 24)        /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_CLKDIV_SHIFT                       25                                          /**< Shift value for CMU_CLKDIV */
#define _CMU_HFRCOCTRL_CLKDIV_MASK                        0x6000000UL                                 /**< Bit mask for CMU_CLKDIV */
#define _CMU_HFRCOCTRL_CLKDIV_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_CLKDIV_DIV1                        0x00000000UL                                /**< Mode DIV1 for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_CLKDIV_DIV2                        0x00000001UL                                /**< Mode DIV2 for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_CLKDIV_DIV4                        0x00000002UL                                /**< Mode DIV4 for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_CLKDIV_DEFAULT                      (_CMU_HFRCOCTRL_CLKDIV_DEFAULT << 25)       /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_CLKDIV_DIV1                         (_CMU_HFRCOCTRL_CLKDIV_DIV1 << 25)          /**< Shifted mode DIV1 for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_CLKDIV_DIV2                         (_CMU_HFRCOCTRL_CLKDIV_DIV2 << 25)          /**< Shifted mode DIV2 for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_CLKDIV_DIV4                         (_CMU_HFRCOCTRL_CLKDIV_DIV4 << 25)          /**< Shifted mode DIV4 for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_FINETUNINGEN                        (0x1UL << 27)                               /**< Enable Reference for Fine Tuning */
#define _CMU_HFRCOCTRL_FINETUNINGEN_SHIFT                 27                                          /**< Shift value for CMU_FINETUNINGEN */
#define _CMU_HFRCOCTRL_FINETUNINGEN_MASK                  0x8000000UL                                 /**< Bit mask for CMU_FINETUNINGEN */
#define _CMU_HFRCOCTRL_FINETUNINGEN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_FINETUNINGEN_DEFAULT                (_CMU_HFRCOCTRL_FINETUNINGEN_DEFAULT << 27) /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */
#define _CMU_HFRCOCTRL_VREFTC_SHIFT                       28                                          /**< Shift value for CMU_VREFTC */
#define _CMU_HFRCOCTRL_VREFTC_MASK                        0xF0000000UL                                /**< Bit mask for CMU_VREFTC */
#define _CMU_HFRCOCTRL_VREFTC_DEFAULT                     0x0000000BUL                                /**< Mode DEFAULT for CMU_HFRCOCTRL */
#define CMU_HFRCOCTRL_VREFTC_DEFAULT                      (_CMU_HFRCOCTRL_VREFTC_DEFAULT << 28)       /**< Shifted mode DEFAULT for CMU_HFRCOCTRL */

/* Bit fields for CMU AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_RESETVALUE                      0xB1481F3CUL                                   /**< Default value for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_MASK                            0xFFFF3F7FUL                                   /**< Mask for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_TUNING_SHIFT                    0                                              /**< Shift value for CMU_TUNING */
#define _CMU_AUXHFRCOCTRL_TUNING_MASK                     0x7FUL                                         /**< Bit mask for CMU_TUNING */
#define _CMU_AUXHFRCOCTRL_TUNING_DEFAULT                  0x0000003CUL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_TUNING_DEFAULT                   (_CMU_AUXHFRCOCTRL_TUNING_DEFAULT << 0)        /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_FINETUNING_SHIFT                8                                              /**< Shift value for CMU_FINETUNING */
#define _CMU_AUXHFRCOCTRL_FINETUNING_MASK                 0x3F00UL                                       /**< Bit mask for CMU_FINETUNING */
#define _CMU_AUXHFRCOCTRL_FINETUNING_DEFAULT              0x0000001FUL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_FINETUNING_DEFAULT               (_CMU_AUXHFRCOCTRL_FINETUNING_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_FREQRANGE_SHIFT                 16                                             /**< Shift value for CMU_FREQRANGE */
#define _CMU_AUXHFRCOCTRL_FREQRANGE_MASK                  0x1F0000UL                                     /**< Bit mask for CMU_FREQRANGE */
#define _CMU_AUXHFRCOCTRL_FREQRANGE_DEFAULT               0x00000008UL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_FREQRANGE_DEFAULT                (_CMU_AUXHFRCOCTRL_FREQRANGE_DEFAULT << 16)    /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_CMPBIAS_SHIFT                   21                                             /**< Shift value for CMU_CMPBIAS */
#define _CMU_AUXHFRCOCTRL_CMPBIAS_MASK                    0xE00000UL                                     /**< Bit mask for CMU_CMPBIAS */
#define _CMU_AUXHFRCOCTRL_CMPBIAS_DEFAULT                 0x00000002UL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_CMPBIAS_DEFAULT                  (_CMU_AUXHFRCOCTRL_CMPBIAS_DEFAULT << 21)      /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_LDOHP                            (0x1UL << 24)                                  /**< AUXHFRCO LDO High Power Mode */
#define _CMU_AUXHFRCOCTRL_LDOHP_SHIFT                     24                                             /**< Shift value for CMU_LDOHP */
#define _CMU_AUXHFRCOCTRL_LDOHP_MASK                      0x1000000UL                                    /**< Bit mask for CMU_LDOHP */
#define _CMU_AUXHFRCOCTRL_LDOHP_DEFAULT                   0x00000001UL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_LDOHP_DEFAULT                    (_CMU_AUXHFRCOCTRL_LDOHP_DEFAULT << 24)        /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_CLKDIV_SHIFT                    25                                             /**< Shift value for CMU_CLKDIV */
#define _CMU_AUXHFRCOCTRL_CLKDIV_MASK                     0x6000000UL                                    /**< Bit mask for CMU_CLKDIV */
#define _CMU_AUXHFRCOCTRL_CLKDIV_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_CLKDIV_DIV1                     0x00000000UL                                   /**< Mode DIV1 for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_CLKDIV_DIV2                     0x00000001UL                                   /**< Mode DIV2 for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_CLKDIV_DIV4                     0x00000002UL                                   /**< Mode DIV4 for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_CLKDIV_DEFAULT                   (_CMU_AUXHFRCOCTRL_CLKDIV_DEFAULT << 25)       /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_CLKDIV_DIV1                      (_CMU_AUXHFRCOCTRL_CLKDIV_DIV1 << 25)          /**< Shifted mode DIV1 for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_CLKDIV_DIV2                      (_CMU_AUXHFRCOCTRL_CLKDIV_DIV2 << 25)          /**< Shifted mode DIV2 for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_CLKDIV_DIV4                      (_CMU_AUXHFRCOCTRL_CLKDIV_DIV4 << 25)          /**< Shifted mode DIV4 for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_FINETUNINGEN                     (0x1UL << 27)                                  /**< Enable Reference for Fine Tuning */
#define _CMU_AUXHFRCOCTRL_FINETUNINGEN_SHIFT              27                                             /**< Shift value for CMU_FINETUNINGEN */
#define _CMU_AUXHFRCOCTRL_FINETUNINGEN_MASK               0x8000000UL                                    /**< Bit mask for CMU_FINETUNINGEN */
#define _CMU_AUXHFRCOCTRL_FINETUNINGEN_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_FINETUNINGEN_DEFAULT             (_CMU_AUXHFRCOCTRL_FINETUNINGEN_DEFAULT << 27) /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */
#define _CMU_AUXHFRCOCTRL_VREFTC_SHIFT                    28                                             /**< Shift value for CMU_VREFTC */
#define _CMU_AUXHFRCOCTRL_VREFTC_MASK                     0xF0000000UL                                   /**< Bit mask for CMU_VREFTC */
#define _CMU_AUXHFRCOCTRL_VREFTC_DEFAULT                  0x0000000BUL                                   /**< Mode DEFAULT for CMU_AUXHFRCOCTRL */
#define CMU_AUXHFRCOCTRL_VREFTC_DEFAULT                   (_CMU_AUXHFRCOCTRL_VREFTC_DEFAULT << 28)       /**< Shifted mode DEFAULT for CMU_AUXHFRCOCTRL */

/* Bit fields for CMU LFRCOCTRL */
#define _CMU_LFRCOCTRL_RESETVALUE                         0x81060100UL                              /**< Default value for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_MASK                               0xF30701FFUL                              /**< Mask for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_TUNING_SHIFT                       0                                         /**< Shift value for CMU_TUNING */
#define _CMU_LFRCOCTRL_TUNING_MASK                        0x1FFUL                                   /**< Bit mask for CMU_TUNING */
#define _CMU_LFRCOCTRL_TUNING_DEFAULT                     0x00000100UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_TUNING_DEFAULT                      (_CMU_LFRCOCTRL_TUNING_DEFAULT << 0)      /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENVREF                              (0x1UL << 16)                             /**< Enable Duty Cycling of Vref */
#define _CMU_LFRCOCTRL_ENVREF_SHIFT                       16                                        /**< Shift value for CMU_ENVREF */
#define _CMU_LFRCOCTRL_ENVREF_MASK                        0x10000UL                                 /**< Bit mask for CMU_ENVREF */
#define _CMU_LFRCOCTRL_ENVREF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENVREF_DEFAULT                      (_CMU_LFRCOCTRL_ENVREF_DEFAULT << 16)     /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENCHOP                              (0x1UL << 17)                             /**< Enable Comparator Chopping */
#define _CMU_LFRCOCTRL_ENCHOP_SHIFT                       17                                        /**< Shift value for CMU_ENCHOP */
#define _CMU_LFRCOCTRL_ENCHOP_MASK                        0x20000UL                                 /**< Bit mask for CMU_ENCHOP */
#define _CMU_LFRCOCTRL_ENCHOP_DEFAULT                     0x00000001UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENCHOP_DEFAULT                      (_CMU_LFRCOCTRL_ENCHOP_DEFAULT << 17)     /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENDEM                               (0x1UL << 18)                             /**< Enable Dynamic Element Matching */
#define _CMU_LFRCOCTRL_ENDEM_SHIFT                        18                                        /**< Shift value for CMU_ENDEM */
#define _CMU_LFRCOCTRL_ENDEM_MASK                         0x40000UL                                 /**< Bit mask for CMU_ENDEM */
#define _CMU_LFRCOCTRL_ENDEM_DEFAULT                      0x00000001UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_ENDEM_DEFAULT                       (_CMU_LFRCOCTRL_ENDEM_DEFAULT << 18)      /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_TIMEOUT_SHIFT                      24                                        /**< Shift value for CMU_TIMEOUT */
#define _CMU_LFRCOCTRL_TIMEOUT_MASK                       0x3000000UL                               /**< Bit mask for CMU_TIMEOUT */
#define _CMU_LFRCOCTRL_TIMEOUT_2CYCLES                    0x00000000UL                              /**< Mode 2CYCLES for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_TIMEOUT_DEFAULT                    0x00000001UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_TIMEOUT_16CYCLES                   0x00000001UL                              /**< Mode 16CYCLES for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_TIMEOUT_32CYCLES                   0x00000002UL                              /**< Mode 32CYCLES for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_TIMEOUT_2CYCLES                     (_CMU_LFRCOCTRL_TIMEOUT_2CYCLES << 24)    /**< Shifted mode 2CYCLES for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_TIMEOUT_DEFAULT                     (_CMU_LFRCOCTRL_TIMEOUT_DEFAULT << 24)    /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_TIMEOUT_16CYCLES                    (_CMU_LFRCOCTRL_TIMEOUT_16CYCLES << 24)   /**< Shifted mode 16CYCLES for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_TIMEOUT_32CYCLES                    (_CMU_LFRCOCTRL_TIMEOUT_32CYCLES << 24)   /**< Shifted mode 32CYCLES for CMU_LFRCOCTRL */
#define _CMU_LFRCOCTRL_GMCCURTUNE_SHIFT                   28                                        /**< Shift value for CMU_GMCCURTUNE */
#define _CMU_LFRCOCTRL_GMCCURTUNE_MASK                    0xF0000000UL                              /**< Bit mask for CMU_GMCCURTUNE */
#define _CMU_LFRCOCTRL_GMCCURTUNE_DEFAULT                 0x00000008UL                              /**< Mode DEFAULT for CMU_LFRCOCTRL */
#define CMU_LFRCOCTRL_GMCCURTUNE_DEFAULT                  (_CMU_LFRCOCTRL_GMCCURTUNE_DEFAULT << 28) /**< Shifted mode DEFAULT for CMU_LFRCOCTRL */

/* Bit fields for CMU HFXOCTRL */
#define _CMU_HFXOCTRL_RESETVALUE                          0x00000000UL                                     /**< Default value for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_MASK                                0x37000731UL                                     /**< Mask for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_MODE                                 (0x1UL << 0)                                     /**< HFXO Mode */
#define _CMU_HFXOCTRL_MODE_SHIFT                          0                                                /**< Shift value for CMU_MODE */
#define _CMU_HFXOCTRL_MODE_MASK                           0x1UL                                            /**< Bit mask for CMU_MODE */
#define _CMU_HFXOCTRL_MODE_DEFAULT                        0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_MODE_XTAL                           0x00000000UL                                     /**< Mode XTAL for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_MODE_EXTCLK                         0x00000001UL                                     /**< Mode EXTCLK for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_MODE_DEFAULT                         (_CMU_HFXOCTRL_MODE_DEFAULT << 0)                /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_MODE_XTAL                            (_CMU_HFXOCTRL_MODE_XTAL << 0)                   /**< Shifted mode XTAL for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_MODE_EXTCLK                          (_CMU_HFXOCTRL_MODE_EXTCLK << 0)                 /**< Shifted mode EXTCLK for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_SHIFT           4                                                /**< Shift value for CMU_PEAKDETSHUNTOPTMODE */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MASK            0x30UL                                           /**< Bit mask for CMU_PEAKDETSHUNTOPTMODE */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_AUTOCMD         0x00000000UL                                     /**< Mode AUTOCMD for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_CMD             0x00000001UL                                     /**< Mode CMD for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MANUAL          0x00000002UL                                     /**< Mode MANUAL for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_DEFAULT          (_CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_DEFAULT << 4) /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_AUTOCMD          (_CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_AUTOCMD << 4) /**< Shifted mode AUTOCMD for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_CMD              (_CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_CMD << 4)     /**< Shifted mode CMD for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MANUAL           (_CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MANUAL << 4)  /**< Shifted mode MANUAL for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LOWPOWER                             (0x1UL << 8)                                     /**< Low Power Mode Control */
#define _CMU_HFXOCTRL_LOWPOWER_SHIFT                      8                                                /**< Shift value for CMU_LOWPOWER */
#define _CMU_HFXOCTRL_LOWPOWER_MASK                       0x100UL                                          /**< Bit mask for CMU_LOWPOWER */
#define _CMU_HFXOCTRL_LOWPOWER_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LOWPOWER_DEFAULT                     (_CMU_HFXOCTRL_LOWPOWER_DEFAULT << 8)            /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_XTI2GND                              (0x1UL << 9)                                     /**< Clamp HFXTAL_N Pin to Ground When HFXO Oscillator is Off */
#define _CMU_HFXOCTRL_XTI2GND_SHIFT                       9                                                /**< Shift value for CMU_XTI2GND */
#define _CMU_HFXOCTRL_XTI2GND_MASK                        0x200UL                                          /**< Bit mask for CMU_XTI2GND */
#define _CMU_HFXOCTRL_XTI2GND_DEFAULT                     0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_XTI2GND_DEFAULT                      (_CMU_HFXOCTRL_XTI2GND_DEFAULT << 9)             /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_XTO2GND                              (0x1UL << 10)                                    /**< Clamp HFXTAL_P Pin to Ground When HFXO Oscillator is Off */
#define _CMU_HFXOCTRL_XTO2GND_SHIFT                       10                                               /**< Shift value for CMU_XTO2GND */
#define _CMU_HFXOCTRL_XTO2GND_MASK                        0x400UL                                          /**< Bit mask for CMU_XTO2GND */
#define _CMU_HFXOCTRL_XTO2GND_DEFAULT                     0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_XTO2GND_DEFAULT                      (_CMU_HFXOCTRL_XTO2GND_DEFAULT << 10)            /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_SHIFT                     24                                               /**< Shift value for CMU_LFTIMEOUT */
#define _CMU_HFXOCTRL_LFTIMEOUT_MASK                      0x7000000UL                                      /**< Bit mask for CMU_LFTIMEOUT */
#define _CMU_HFXOCTRL_LFTIMEOUT_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_0CYCLES                   0x00000000UL                                     /**< Mode 0CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_2CYCLES                   0x00000001UL                                     /**< Mode 2CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_4CYCLES                   0x00000002UL                                     /**< Mode 4CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_16CYCLES                  0x00000003UL                                     /**< Mode 16CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_32CYCLES                  0x00000004UL                                     /**< Mode 32CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_64CYCLES                  0x00000005UL                                     /**< Mode 64CYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_1KCYCLES                  0x00000006UL                                     /**< Mode 1KCYCLES for CMU_HFXOCTRL */
#define _CMU_HFXOCTRL_LFTIMEOUT_4KCYCLES                  0x00000007UL                                     /**< Mode 4KCYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_DEFAULT                    (_CMU_HFXOCTRL_LFTIMEOUT_DEFAULT << 24)          /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_0CYCLES                    (_CMU_HFXOCTRL_LFTIMEOUT_0CYCLES << 24)          /**< Shifted mode 0CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_2CYCLES                    (_CMU_HFXOCTRL_LFTIMEOUT_2CYCLES << 24)          /**< Shifted mode 2CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_4CYCLES                    (_CMU_HFXOCTRL_LFTIMEOUT_4CYCLES << 24)          /**< Shifted mode 4CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_16CYCLES                   (_CMU_HFXOCTRL_LFTIMEOUT_16CYCLES << 24)         /**< Shifted mode 16CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_32CYCLES                   (_CMU_HFXOCTRL_LFTIMEOUT_32CYCLES << 24)         /**< Shifted mode 32CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_64CYCLES                   (_CMU_HFXOCTRL_LFTIMEOUT_64CYCLES << 24)         /**< Shifted mode 64CYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_1KCYCLES                   (_CMU_HFXOCTRL_LFTIMEOUT_1KCYCLES << 24)         /**< Shifted mode 1KCYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_LFTIMEOUT_4KCYCLES                   (_CMU_HFXOCTRL_LFTIMEOUT_4KCYCLES << 24)         /**< Shifted mode 4KCYCLES for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_AUTOSTARTEM0EM1                      (0x1UL << 28)                                    /**< Automatically Start of HFXO Upon EM0/EM1 Entry From EM2/EM3 */
#define _CMU_HFXOCTRL_AUTOSTARTEM0EM1_SHIFT               28                                               /**< Shift value for CMU_AUTOSTARTEM0EM1 */
#define _CMU_HFXOCTRL_AUTOSTARTEM0EM1_MASK                0x10000000UL                                     /**< Bit mask for CMU_AUTOSTARTEM0EM1 */
#define _CMU_HFXOCTRL_AUTOSTARTEM0EM1_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_AUTOSTARTEM0EM1_DEFAULT              (_CMU_HFXOCTRL_AUTOSTARTEM0EM1_DEFAULT << 28)    /**< Shifted mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_AUTOSTARTSELEM0EM1                   (0x1UL << 29)                                    /**< Automatically Start and Select of HFXO Upon EM0/EM1 Entry From EM2/EM3 */
#define _CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_SHIFT            29                                               /**< Shift value for CMU_AUTOSTARTSELEM0EM1 */
#define _CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_MASK             0x20000000UL                                     /**< Bit mask for CMU_AUTOSTARTSELEM0EM1 */
#define _CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for CMU_HFXOCTRL */
#define CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_DEFAULT           (_CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_DEFAULT << 29) /**< Shifted mode DEFAULT for CMU_HFXOCTRL */

/* Bit fields for CMU HFXOCTRL1 */
#define _CMU_HFXOCTRL1_RESETVALUE                         0x00000240UL                             /**< Default value for CMU_HFXOCTRL1 */
#define _CMU_HFXOCTRL1_MASK                               0x00000277UL                             /**< Mask for CMU_HFXOCTRL1 */
#define _CMU_HFXOCTRL1_PEAKDETTHR_SHIFT                   0                                        /**< Shift value for CMU_PEAKDETTHR */
#define _CMU_HFXOCTRL1_PEAKDETTHR_MASK                    0x7UL                                    /**< Bit mask for CMU_PEAKDETTHR */
#define _CMU_HFXOCTRL1_PEAKDETTHR_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for CMU_HFXOCTRL1 */
#define CMU_HFXOCTRL1_PEAKDETTHR_DEFAULT                  (_CMU_HFXOCTRL1_PEAKDETTHR_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFXOCTRL1 */
#define _CMU_HFXOCTRL1_REGLVL_SHIFT                       4                                        /**< Shift value for CMU_REGLVL */
#define _CMU_HFXOCTRL1_REGLVL_MASK                        0x70UL                                   /**< Bit mask for CMU_REGLVL */
#define _CMU_HFXOCTRL1_REGLVL_DEFAULT                     0x00000004UL                             /**< Mode DEFAULT for CMU_HFXOCTRL1 */
#define CMU_HFXOCTRL1_REGLVL_DEFAULT                      (_CMU_HFXOCTRL1_REGLVL_DEFAULT << 4)     /**< Shifted mode DEFAULT for CMU_HFXOCTRL1 */
#define CMU_HFXOCTRL1_XTIBIASEN                           (0x1UL << 9)                             /**< Reserved for internal use. Do not change. */
#define _CMU_HFXOCTRL1_XTIBIASEN_SHIFT                    9                                        /**< Shift value for CMU_XTIBIASEN */
#define _CMU_HFXOCTRL1_XTIBIASEN_MASK                     0x200UL                                  /**< Bit mask for CMU_XTIBIASEN */
#define _CMU_HFXOCTRL1_XTIBIASEN_DEFAULT                  0x00000001UL                             /**< Mode DEFAULT for CMU_HFXOCTRL1 */
#define CMU_HFXOCTRL1_XTIBIASEN_DEFAULT                   (_CMU_HFXOCTRL1_XTIBIASEN_DEFAULT << 9)  /**< Shifted mode DEFAULT for CMU_HFXOCTRL1 */

/* Bit fields for CMU HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_RESETVALUE                   0xA1250060UL                                     /**< Default value for CMU_HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_MASK                         0xFFEFF87FUL                                     /**< Mask for CMU_HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_SHIFT           0                                                /**< Shift value for CMU_IBTRIMXOCORE */
#define _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_MASK            0x7FUL                                           /**< Bit mask for CMU_IBTRIMXOCORE */
#define _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT         0x00000060UL                                     /**< Mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT          (_CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_CTUNE_SHIFT                  11                                               /**< Shift value for CMU_CTUNE */
#define _CMU_HFXOSTARTUPCTRL_CTUNE_MASK                   0xFF800UL                                        /**< Bit mask for CMU_CTUNE */
#define _CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT                0x000000A0UL                                     /**< Mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT                 (_CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT << 11)       /**< Shifted mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_RESERVED0_SHIFT              21                                               /**< Shift value for CMU_RESERVED0 */
#define _CMU_HFXOSTARTUPCTRL_RESERVED0_MASK               0xFE00000UL                                      /**< Bit mask for CMU_RESERVED0 */
#define _CMU_HFXOSTARTUPCTRL_RESERVED0_DEFAULT            0x00000009UL                                     /**< Mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define CMU_HFXOSTARTUPCTRL_RESERVED0_DEFAULT             (_CMU_HFXOSTARTUPCTRL_RESERVED0_DEFAULT << 21)   /**< Shifted mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define _CMU_HFXOSTARTUPCTRL_RESERVED1_SHIFT              28                                               /**< Shift value for CMU_RESERVED1 */
#define _CMU_HFXOSTARTUPCTRL_RESERVED1_MASK               0xF0000000UL                                     /**< Bit mask for CMU_RESERVED1 */
#define _CMU_HFXOSTARTUPCTRL_RESERVED1_DEFAULT            0x0000000AUL                                     /**< Mode DEFAULT for CMU_HFXOSTARTUPCTRL */
#define CMU_HFXOSTARTUPCTRL_RESERVED1_DEFAULT             (_CMU_HFXOSTARTUPCTRL_RESERVED1_DEFAULT << 28)   /**< Shifted mode DEFAULT for CMU_HFXOSTARTUPCTRL */

/* Bit fields for CMU HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_RESETVALUE               0xA30AAD09UL                                         /**< Default value for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_MASK                     0xF70FFFFFUL                                         /**< Mask for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_SHIFT       0                                                    /**< Shift value for CMU_IBTRIMXOCORE */
#define _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_MASK        0x7FUL                                               /**< Bit mask for CMU_IBTRIMXOCORE */
#define _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT     0x00000009UL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT      (_CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_REGISH_SHIFT             7                                                    /**< Shift value for CMU_REGISH */
#define _CMU_HFXOSTEADYSTATECTRL_REGISH_MASK              0x780UL                                              /**< Bit mask for CMU_REGISH */
#define _CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT           0x0000000AUL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT            (_CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT << 7)       /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT              11                                                   /**< Shift value for CMU_CTUNE */
#define _CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK               0xFF800UL                                            /**< Bit mask for CMU_CTUNE */
#define _CMU_HFXOSTEADYSTATECTRL_CTUNE_DEFAULT            0x00000155UL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_CTUNE_DEFAULT             (_CMU_HFXOSTEADYSTATECTRL_CTUNE_DEFAULT << 11)       /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_REGSELILOW_SHIFT         24                                                   /**< Shift value for CMU_REGSELILOW */
#define _CMU_HFXOSTEADYSTATECTRL_REGSELILOW_MASK          0x3000000UL                                          /**< Bit mask for CMU_REGSELILOW */
#define _CMU_HFXOSTEADYSTATECTRL_REGSELILOW_DEFAULT       0x00000003UL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_REGSELILOW_DEFAULT        (_CMU_HFXOSTEADYSTATECTRL_REGSELILOW_DEFAULT << 24)  /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_PEAKDETEN                 (0x1UL << 26)                                        /**< Enables Oscillator Peak Detectors */
#define _CMU_HFXOSTEADYSTATECTRL_PEAKDETEN_SHIFT          26                                                   /**< Shift value for CMU_PEAKDETEN */
#define _CMU_HFXOSTEADYSTATECTRL_PEAKDETEN_MASK           0x4000000UL                                          /**< Bit mask for CMU_PEAKDETEN */
#define _CMU_HFXOSTEADYSTATECTRL_PEAKDETEN_DEFAULT        0x00000000UL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_PEAKDETEN_DEFAULT         (_CMU_HFXOSTEADYSTATECTRL_PEAKDETEN_DEFAULT << 26)   /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define _CMU_HFXOSTEADYSTATECTRL_REGISHUPPER_SHIFT        28                                                   /**< Shift value for CMU_REGISHUPPER */
#define _CMU_HFXOSTEADYSTATECTRL_REGISHUPPER_MASK         0xF0000000UL                                         /**< Bit mask for CMU_REGISHUPPER */
#define _CMU_HFXOSTEADYSTATECTRL_REGISHUPPER_DEFAULT      0x0000000AUL                                         /**< Mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */
#define CMU_HFXOSTEADYSTATECTRL_REGISHUPPER_DEFAULT       (_CMU_HFXOSTEADYSTATECTRL_REGISHUPPER_DEFAULT << 28) /**< Shifted mode DEFAULT for CMU_HFXOSTEADYSTATECTRL */

/* Bit fields for CMU HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_RESETVALUE                   0x00026667UL                                           /**< Default value for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_MASK                         0x000FFFFFUL                                           /**< Mask for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_SHIFT         0                                                      /**< Shift value for CMU_STARTUPTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_MASK          0xFUL                                                  /**< Bit mask for CMU_STARTUPTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2CYCLES       0x00000000UL                                           /**< Mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4CYCLES       0x00000001UL                                           /**< Mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16CYCLES      0x00000002UL                                           /**< Mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32CYCLES      0x00000003UL                                           /**< Mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_256CYCLES     0x00000004UL                                           /**< Mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_1KCYCLES      0x00000005UL                                           /**< Mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2KCYCLES      0x00000006UL                                           /**< Mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_DEFAULT       0x00000007UL                                           /**< Mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4KCYCLES      0x00000007UL                                           /**< Mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_8KCYCLES      0x00000008UL                                           /**< Mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16KCYCLES     0x00000009UL                                           /**< Mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32KCYCLES     0x0000000AUL                                           /**< Mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2CYCLES        (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2CYCLES << 0)     /**< Shifted mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4CYCLES        (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4CYCLES << 0)     /**< Shifted mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16CYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16CYCLES << 0)    /**< Shifted mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32CYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32CYCLES << 0)    /**< Shifted mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_256CYCLES      (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_256CYCLES << 0)   /**< Shifted mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_1KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_1KCYCLES << 0)    /**< Shifted mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_2KCYCLES << 0)    /**< Shifted mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_DEFAULT        (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4KCYCLES << 0)    /**< Shifted mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_8KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_8KCYCLES << 0)    /**< Shifted mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16KCYCLES      (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_16KCYCLES << 0)   /**< Shifted mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32KCYCLES      (_CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_32KCYCLES << 0)   /**< Shifted mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_SHIFT          4                                                      /**< Shift value for CMU_STEADYTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_MASK           0xF0UL                                                 /**< Bit mask for CMU_STEADYTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2CYCLES        0x00000000UL                                           /**< Mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4CYCLES        0x00000001UL                                           /**< Mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16CYCLES       0x00000002UL                                           /**< Mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32CYCLES       0x00000003UL                                           /**< Mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_256CYCLES      0x00000004UL                                           /**< Mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_1KCYCLES       0x00000005UL                                           /**< Mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_DEFAULT        0x00000006UL                                           /**< Mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2KCYCLES       0x00000006UL                                           /**< Mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4KCYCLES       0x00000007UL                                           /**< Mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_8KCYCLES       0x00000008UL                                           /**< Mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16KCYCLES      0x00000009UL                                           /**< Mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32KCYCLES      0x0000000AUL                                           /**< Mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2CYCLES         (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2CYCLES << 4)      /**< Shifted mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4CYCLES         (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4CYCLES << 4)      /**< Shifted mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16CYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16CYCLES << 4)     /**< Shifted mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32CYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32CYCLES << 4)     /**< Shifted mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_256CYCLES       (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_256CYCLES << 4)    /**< Shifted mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_1KCYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_1KCYCLES << 4)     /**< Shifted mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_DEFAULT         (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2KCYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_2KCYCLES << 4)     /**< Shifted mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4KCYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_4KCYCLES << 4)     /**< Shifted mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_8KCYCLES        (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_8KCYCLES << 4)     /**< Shifted mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_16KCYCLES << 4)    /**< Shifted mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32KCYCLES       (_CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_32KCYCLES << 4)    /**< Shifted mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_RESERVED2_SHIFT              8                                                      /**< Shift value for CMU_RESERVED2 */
#define _CMU_HFXOTIMEOUTCTRL_RESERVED2_MASK               0xF00UL                                                /**< Bit mask for CMU_RESERVED2 */
#define _CMU_HFXOTIMEOUTCTRL_RESERVED2_DEFAULT            0x00000006UL                                           /**< Mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_RESERVED2_DEFAULT             (_CMU_HFXOTIMEOUTCTRL_RESERVED2_DEFAULT << 8)          /**< Shifted mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_SHIFT         12                                                     /**< Shift value for CMU_PEAKDETTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_MASK          0xF000UL                                               /**< Bit mask for CMU_PEAKDETTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2CYCLES       0x00000000UL                                           /**< Mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4CYCLES       0x00000001UL                                           /**< Mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16CYCLES      0x00000002UL                                           /**< Mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32CYCLES      0x00000003UL                                           /**< Mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_256CYCLES     0x00000004UL                                           /**< Mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_1KCYCLES      0x00000005UL                                           /**< Mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_DEFAULT       0x00000006UL                                           /**< Mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2KCYCLES      0x00000006UL                                           /**< Mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4KCYCLES      0x00000007UL                                           /**< Mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_8KCYCLES      0x00000008UL                                           /**< Mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16KCYCLES     0x00000009UL                                           /**< Mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32KCYCLES     0x0000000AUL                                           /**< Mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2CYCLES        (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2CYCLES << 12)    /**< Shifted mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4CYCLES        (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4CYCLES << 12)    /**< Shifted mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16CYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16CYCLES << 12)   /**< Shifted mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32CYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32CYCLES << 12)   /**< Shifted mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_256CYCLES      (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_256CYCLES << 12)  /**< Shifted mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_1KCYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_1KCYCLES << 12)   /**< Shifted mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_DEFAULT        (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_DEFAULT << 12)    /**< Shifted mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2KCYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_2KCYCLES << 12)   /**< Shifted mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4KCYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_4KCYCLES << 12)   /**< Shifted mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_8KCYCLES       (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_8KCYCLES << 12)   /**< Shifted mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16KCYCLES      (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_16KCYCLES << 12)  /**< Shifted mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32KCYCLES      (_CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32KCYCLES << 12)  /**< Shifted mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_SHIFT        16                                                     /**< Shift value for CMU_SHUNTOPTTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_MASK         0xF0000UL                                              /**< Bit mask for CMU_SHUNTOPTTIMEOUT */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2CYCLES      0x00000000UL                                           /**< Mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4CYCLES      0x00000001UL                                           /**< Mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_DEFAULT      0x00000002UL                                           /**< Mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16CYCLES     0x00000002UL                                           /**< Mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32CYCLES     0x00000003UL                                           /**< Mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_256CYCLES    0x00000004UL                                           /**< Mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_1KCYCLES     0x00000005UL                                           /**< Mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2KCYCLES     0x00000006UL                                           /**< Mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4KCYCLES     0x00000007UL                                           /**< Mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_8KCYCLES     0x00000008UL                                           /**< Mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16KCYCLES    0x00000009UL                                           /**< Mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32KCYCLES    0x0000000AUL                                           /**< Mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2CYCLES       (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2CYCLES << 16)   /**< Shifted mode 2CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4CYCLES       (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4CYCLES << 16)   /**< Shifted mode 4CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_DEFAULT       (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_DEFAULT << 16)   /**< Shifted mode DEFAULT for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16CYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16CYCLES << 16)  /**< Shifted mode 16CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32CYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32CYCLES << 16)  /**< Shifted mode 32CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_256CYCLES     (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_256CYCLES << 16) /**< Shifted mode 256CYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_1KCYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_1KCYCLES << 16)  /**< Shifted mode 1KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2KCYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_2KCYCLES << 16)  /**< Shifted mode 2KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4KCYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_4KCYCLES << 16)  /**< Shifted mode 4KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_8KCYCLES      (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_8KCYCLES << 16)  /**< Shifted mode 8KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16KCYCLES     (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16KCYCLES << 16) /**< Shifted mode 16KCYCLES for CMU_HFXOTIMEOUTCTRL */
#define CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32KCYCLES     (_CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_32KCYCLES << 16) /**< Shifted mode 32KCYCLES for CMU_HFXOTIMEOUTCTRL */

/* Bit fields for CMU LFXOCTRL */
#define _CMU_LFXOCTRL_RESETVALUE                          0x07009000UL                            /**< Default value for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_MASK                                0x0713DB7FUL                            /**< Mask for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TUNING_SHIFT                        0                                       /**< Shift value for CMU_TUNING */
#define _CMU_LFXOCTRL_TUNING_MASK                         0x7FUL                                  /**< Bit mask for CMU_TUNING */
#define _CMU_LFXOCTRL_TUNING_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TUNING_DEFAULT                       (_CMU_LFXOCTRL_TUNING_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_MODE_SHIFT                          8                                       /**< Shift value for CMU_MODE */
#define _CMU_LFXOCTRL_MODE_MASK                           0x300UL                                 /**< Bit mask for CMU_MODE */
#define _CMU_LFXOCTRL_MODE_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_MODE_XTAL                           0x00000000UL                            /**< Mode XTAL for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_MODE_BUFEXTCLK                      0x00000001UL                            /**< Mode BUFEXTCLK for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_MODE_DIGEXTCLK                      0x00000002UL                            /**< Mode DIGEXTCLK for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_MODE_DEFAULT                         (_CMU_LFXOCTRL_MODE_DEFAULT << 8)       /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_MODE_XTAL                            (_CMU_LFXOCTRL_MODE_XTAL << 8)          /**< Shifted mode XTAL for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_MODE_BUFEXTCLK                       (_CMU_LFXOCTRL_MODE_BUFEXTCLK << 8)     /**< Shifted mode BUFEXTCLK for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_MODE_DIGEXTCLK                       (_CMU_LFXOCTRL_MODE_DIGEXTCLK << 8)     /**< Shifted mode DIGEXTCLK for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_GAIN_SHIFT                          11                                      /**< Shift value for CMU_GAIN */
#define _CMU_LFXOCTRL_GAIN_MASK                           0x1800UL                                /**< Bit mask for CMU_GAIN */
#define _CMU_LFXOCTRL_GAIN_DEFAULT                        0x00000002UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_GAIN_DEFAULT                         (_CMU_LFXOCTRL_GAIN_DEFAULT << 11)      /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_HIGHAMPL                             (0x1UL << 14)                           /**< LFXO High XTAL Oscillation Amplitude Enable */
#define _CMU_LFXOCTRL_HIGHAMPL_SHIFT                      14                                      /**< Shift value for CMU_HIGHAMPL */
#define _CMU_LFXOCTRL_HIGHAMPL_MASK                       0x4000UL                                /**< Bit mask for CMU_HIGHAMPL */
#define _CMU_LFXOCTRL_HIGHAMPL_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_HIGHAMPL_DEFAULT                     (_CMU_LFXOCTRL_HIGHAMPL_DEFAULT << 14)  /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_AGC                                  (0x1UL << 15)                           /**< LFXO AGC Enable */
#define _CMU_LFXOCTRL_AGC_SHIFT                           15                                      /**< Shift value for CMU_AGC */
#define _CMU_LFXOCTRL_AGC_MASK                            0x8000UL                                /**< Bit mask for CMU_AGC */
#define _CMU_LFXOCTRL_AGC_DEFAULT                         0x00000001UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_AGC_DEFAULT                          (_CMU_LFXOCTRL_AGC_DEFAULT << 15)       /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_CUR_SHIFT                           16                                      /**< Shift value for CMU_CUR */
#define _CMU_LFXOCTRL_CUR_MASK                            0x30000UL                               /**< Bit mask for CMU_CUR */
#define _CMU_LFXOCTRL_CUR_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_CUR_DEFAULT                          (_CMU_LFXOCTRL_CUR_DEFAULT << 16)       /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_BUFCUR                               (0x1UL << 20)                           /**< LFXO Buffer Bias Current */
#define _CMU_LFXOCTRL_BUFCUR_SHIFT                        20                                      /**< Shift value for CMU_BUFCUR */
#define _CMU_LFXOCTRL_BUFCUR_MASK                         0x100000UL                              /**< Bit mask for CMU_BUFCUR */
#define _CMU_LFXOCTRL_BUFCUR_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_BUFCUR_DEFAULT                       (_CMU_LFXOCTRL_BUFCUR_DEFAULT << 20)    /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_SHIFT                       24                                      /**< Shift value for CMU_TIMEOUT */
#define _CMU_LFXOCTRL_TIMEOUT_MASK                        0x7000000UL                             /**< Bit mask for CMU_TIMEOUT */
#define _CMU_LFXOCTRL_TIMEOUT_2CYCLES                     0x00000000UL                            /**< Mode 2CYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_256CYCLES                   0x00000001UL                            /**< Mode 256CYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_1KCYCLES                    0x00000002UL                            /**< Mode 1KCYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_2KCYCLES                    0x00000003UL                            /**< Mode 2KCYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_4KCYCLES                    0x00000004UL                            /**< Mode 4KCYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_8KCYCLES                    0x00000005UL                            /**< Mode 8KCYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_16KCYCLES                   0x00000006UL                            /**< Mode 16KCYCLES for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_DEFAULT                     0x00000007UL                            /**< Mode DEFAULT for CMU_LFXOCTRL */
#define _CMU_LFXOCTRL_TIMEOUT_32KCYCLES                   0x00000007UL                            /**< Mode 32KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_2CYCLES                      (_CMU_LFXOCTRL_TIMEOUT_2CYCLES << 24)   /**< Shifted mode 2CYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_256CYCLES                    (_CMU_LFXOCTRL_TIMEOUT_256CYCLES << 24) /**< Shifted mode 256CYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_1KCYCLES                     (_CMU_LFXOCTRL_TIMEOUT_1KCYCLES << 24)  /**< Shifted mode 1KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_2KCYCLES                     (_CMU_LFXOCTRL_TIMEOUT_2KCYCLES << 24)  /**< Shifted mode 2KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_4KCYCLES                     (_CMU_LFXOCTRL_TIMEOUT_4KCYCLES << 24)  /**< Shifted mode 4KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_8KCYCLES                     (_CMU_LFXOCTRL_TIMEOUT_8KCYCLES << 24)  /**< Shifted mode 8KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_16KCYCLES                    (_CMU_LFXOCTRL_TIMEOUT_16KCYCLES << 24) /**< Shifted mode 16KCYCLES for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_DEFAULT                      (_CMU_LFXOCTRL_TIMEOUT_DEFAULT << 24)   /**< Shifted mode DEFAULT for CMU_LFXOCTRL */
#define CMU_LFXOCTRL_TIMEOUT_32KCYCLES                    (_CMU_LFXOCTRL_TIMEOUT_32KCYCLES << 24) /**< Shifted mode 32KCYCLES for CMU_LFXOCTRL */

/* Bit fields for CMU ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_RESETVALUE                        0x00020020UL                            /**< Default value for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MASK                              0x00030C3FUL                            /**< Mask for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_TUNING_SHIFT                      0                                       /**< Shift value for CMU_TUNING */
#define _CMU_ULFRCOCTRL_TUNING_MASK                       0x3FUL                                  /**< Bit mask for CMU_TUNING */
#define _CMU_ULFRCOCTRL_TUNING_DEFAULT                    0x00000020UL                            /**< Mode DEFAULT for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_TUNING_DEFAULT                     (_CMU_ULFRCOCTRL_TUNING_DEFAULT << 0)   /**< Shifted mode DEFAULT for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MODE_SHIFT                        10                                      /**< Shift value for CMU_MODE */
#define _CMU_ULFRCOCTRL_MODE_MASK                         0xC00UL                                 /**< Bit mask for CMU_MODE */
#define _CMU_ULFRCOCTRL_MODE_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MODE_1KHZ                         0x00000000UL                            /**< Mode 1KHZ for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MODE_2KHZ                         0x00000001UL                            /**< Mode 2KHZ for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MODE_4KHZ                         0x00000002UL                            /**< Mode 4KHZ for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_MODE_32KHZ                        0x00000003UL                            /**< Mode 32KHZ for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_MODE_DEFAULT                       (_CMU_ULFRCOCTRL_MODE_DEFAULT << 10)    /**< Shifted mode DEFAULT for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_MODE_1KHZ                          (_CMU_ULFRCOCTRL_MODE_1KHZ << 10)       /**< Shifted mode 1KHZ for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_MODE_2KHZ                          (_CMU_ULFRCOCTRL_MODE_2KHZ << 10)       /**< Shifted mode 2KHZ for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_MODE_4KHZ                          (_CMU_ULFRCOCTRL_MODE_4KHZ << 10)       /**< Shifted mode 4KHZ for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_MODE_32KHZ                         (_CMU_ULFRCOCTRL_MODE_32KHZ << 10)      /**< Shifted mode 32KHZ for CMU_ULFRCOCTRL */
#define _CMU_ULFRCOCTRL_RESTRIM_SHIFT                     16                                      /**< Shift value for CMU_RESTRIM */
#define _CMU_ULFRCOCTRL_RESTRIM_MASK                      0x30000UL                               /**< Bit mask for CMU_RESTRIM */
#define _CMU_ULFRCOCTRL_RESTRIM_DEFAULT                   0x00000002UL                            /**< Mode DEFAULT for CMU_ULFRCOCTRL */
#define CMU_ULFRCOCTRL_RESTRIM_DEFAULT                    (_CMU_ULFRCOCTRL_RESTRIM_DEFAULT << 16) /**< Shifted mode DEFAULT for CMU_ULFRCOCTRL */

/* Bit fields for CMU CALCTRL */
#define _CMU_CALCTRL_RESETVALUE                           0x00000000UL                            /**< Default value for CMU_CALCTRL */
#define _CMU_CALCTRL_MASK                                 0x0F0F0177UL                            /**< Mask for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_SHIFT                          0                                       /**< Shift value for CMU_UPSEL */
#define _CMU_CALCTRL_UPSEL_MASK                           0x7UL                                   /**< Bit mask for CMU_UPSEL */
#define _CMU_CALCTRL_UPSEL_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_HFXO                           0x00000000UL                            /**< Mode HFXO for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_LFXO                           0x00000001UL                            /**< Mode LFXO for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_HFRCO                          0x00000002UL                            /**< Mode HFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_LFRCO                          0x00000003UL                            /**< Mode LFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_AUXHFRCO                       0x00000004UL                            /**< Mode AUXHFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_UPSEL_PRS                            0x00000005UL                            /**< Mode PRS for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_DEFAULT                         (_CMU_CALCTRL_UPSEL_DEFAULT << 0)       /**< Shifted mode DEFAULT for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_HFXO                            (_CMU_CALCTRL_UPSEL_HFXO << 0)          /**< Shifted mode HFXO for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_LFXO                            (_CMU_CALCTRL_UPSEL_LFXO << 0)          /**< Shifted mode LFXO for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_HFRCO                           (_CMU_CALCTRL_UPSEL_HFRCO << 0)         /**< Shifted mode HFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_LFRCO                           (_CMU_CALCTRL_UPSEL_LFRCO << 0)         /**< Shifted mode LFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_AUXHFRCO                        (_CMU_CALCTRL_UPSEL_AUXHFRCO << 0)      /**< Shifted mode AUXHFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_UPSEL_PRS                             (_CMU_CALCTRL_UPSEL_PRS << 0)           /**< Shifted mode PRS for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_SHIFT                        4                                       /**< Shift value for CMU_DOWNSEL */
#define _CMU_CALCTRL_DOWNSEL_MASK                         0x70UL                                  /**< Bit mask for CMU_DOWNSEL */
#define _CMU_CALCTRL_DOWNSEL_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_HFCLK                        0x00000000UL                            /**< Mode HFCLK for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_HFXO                         0x00000001UL                            /**< Mode HFXO for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_LFXO                         0x00000002UL                            /**< Mode LFXO for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_HFRCO                        0x00000003UL                            /**< Mode HFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_LFRCO                        0x00000004UL                            /**< Mode LFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_AUXHFRCO                     0x00000005UL                            /**< Mode AUXHFRCO for CMU_CALCTRL */
#define _CMU_CALCTRL_DOWNSEL_PRS                          0x00000006UL                            /**< Mode PRS for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_DEFAULT                       (_CMU_CALCTRL_DOWNSEL_DEFAULT << 4)     /**< Shifted mode DEFAULT for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_HFCLK                         (_CMU_CALCTRL_DOWNSEL_HFCLK << 4)       /**< Shifted mode HFCLK for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_HFXO                          (_CMU_CALCTRL_DOWNSEL_HFXO << 4)        /**< Shifted mode HFXO for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_LFXO                          (_CMU_CALCTRL_DOWNSEL_LFXO << 4)        /**< Shifted mode LFXO for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_HFRCO                         (_CMU_CALCTRL_DOWNSEL_HFRCO << 4)       /**< Shifted mode HFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_LFRCO                         (_CMU_CALCTRL_DOWNSEL_LFRCO << 4)       /**< Shifted mode LFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_AUXHFRCO                      (_CMU_CALCTRL_DOWNSEL_AUXHFRCO << 4)    /**< Shifted mode AUXHFRCO for CMU_CALCTRL */
#define CMU_CALCTRL_DOWNSEL_PRS                           (_CMU_CALCTRL_DOWNSEL_PRS << 4)         /**< Shifted mode PRS for CMU_CALCTRL */
#define CMU_CALCTRL_CONT                                  (0x1UL << 8)                            /**< Continuous Calibration */
#define _CMU_CALCTRL_CONT_SHIFT                           8                                       /**< Shift value for CMU_CONT */
#define _CMU_CALCTRL_CONT_MASK                            0x100UL                                 /**< Bit mask for CMU_CONT */
#define _CMU_CALCTRL_CONT_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for CMU_CALCTRL */
#define CMU_CALCTRL_CONT_DEFAULT                          (_CMU_CALCTRL_CONT_DEFAULT << 8)        /**< Shifted mode DEFAULT for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_SHIFT                       16                                      /**< Shift value for CMU_PRSUPSEL */
#define _CMU_CALCTRL_PRSUPSEL_MASK                        0xF0000UL                               /**< Bit mask for CMU_PRSUPSEL */
#define _CMU_CALCTRL_PRSUPSEL_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH0                      0x00000000UL                            /**< Mode PRSCH0 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH1                      0x00000001UL                            /**< Mode PRSCH1 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH2                      0x00000002UL                            /**< Mode PRSCH2 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH3                      0x00000003UL                            /**< Mode PRSCH3 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH4                      0x00000004UL                            /**< Mode PRSCH4 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH5                      0x00000005UL                            /**< Mode PRSCH5 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH6                      0x00000006UL                            /**< Mode PRSCH6 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH7                      0x00000007UL                            /**< Mode PRSCH7 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH8                      0x00000008UL                            /**< Mode PRSCH8 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH9                      0x00000009UL                            /**< Mode PRSCH9 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH10                     0x0000000AUL                            /**< Mode PRSCH10 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSUPSEL_PRSCH11                     0x0000000BUL                            /**< Mode PRSCH11 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_DEFAULT                      (_CMU_CALCTRL_PRSUPSEL_DEFAULT << 16)   /**< Shifted mode DEFAULT for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH0                       (_CMU_CALCTRL_PRSUPSEL_PRSCH0 << 16)    /**< Shifted mode PRSCH0 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH1                       (_CMU_CALCTRL_PRSUPSEL_PRSCH1 << 16)    /**< Shifted mode PRSCH1 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH2                       (_CMU_CALCTRL_PRSUPSEL_PRSCH2 << 16)    /**< Shifted mode PRSCH2 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH3                       (_CMU_CALCTRL_PRSUPSEL_PRSCH3 << 16)    /**< Shifted mode PRSCH3 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH4                       (_CMU_CALCTRL_PRSUPSEL_PRSCH4 << 16)    /**< Shifted mode PRSCH4 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH5                       (_CMU_CALCTRL_PRSUPSEL_PRSCH5 << 16)    /**< Shifted mode PRSCH5 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH6                       (_CMU_CALCTRL_PRSUPSEL_PRSCH6 << 16)    /**< Shifted mode PRSCH6 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH7                       (_CMU_CALCTRL_PRSUPSEL_PRSCH7 << 16)    /**< Shifted mode PRSCH7 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH8                       (_CMU_CALCTRL_PRSUPSEL_PRSCH8 << 16)    /**< Shifted mode PRSCH8 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH9                       (_CMU_CALCTRL_PRSUPSEL_PRSCH9 << 16)    /**< Shifted mode PRSCH9 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH10                      (_CMU_CALCTRL_PRSUPSEL_PRSCH10 << 16)   /**< Shifted mode PRSCH10 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSUPSEL_PRSCH11                      (_CMU_CALCTRL_PRSUPSEL_PRSCH11 << 16)   /**< Shifted mode PRSCH11 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_SHIFT                     24                                      /**< Shift value for CMU_PRSDOWNSEL */
#define _CMU_CALCTRL_PRSDOWNSEL_MASK                      0xF000000UL                             /**< Bit mask for CMU_PRSDOWNSEL */
#define _CMU_CALCTRL_PRSDOWNSEL_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH0                    0x00000000UL                            /**< Mode PRSCH0 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH1                    0x00000001UL                            /**< Mode PRSCH1 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH2                    0x00000002UL                            /**< Mode PRSCH2 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH3                    0x00000003UL                            /**< Mode PRSCH3 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH4                    0x00000004UL                            /**< Mode PRSCH4 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH5                    0x00000005UL                            /**< Mode PRSCH5 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH6                    0x00000006UL                            /**< Mode PRSCH6 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH7                    0x00000007UL                            /**< Mode PRSCH7 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH8                    0x00000008UL                            /**< Mode PRSCH8 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH9                    0x00000009UL                            /**< Mode PRSCH9 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH10                   0x0000000AUL                            /**< Mode PRSCH10 for CMU_CALCTRL */
#define _CMU_CALCTRL_PRSDOWNSEL_PRSCH11                   0x0000000BUL                            /**< Mode PRSCH11 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_DEFAULT                    (_CMU_CALCTRL_PRSDOWNSEL_DEFAULT << 24) /**< Shifted mode DEFAULT for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH0                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH0 << 24)  /**< Shifted mode PRSCH0 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH1                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH1 << 24)  /**< Shifted mode PRSCH1 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH2                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH2 << 24)  /**< Shifted mode PRSCH2 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH3                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH3 << 24)  /**< Shifted mode PRSCH3 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH4                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH4 << 24)  /**< Shifted mode PRSCH4 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH5                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH5 << 24)  /**< Shifted mode PRSCH5 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH6                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH6 << 24)  /**< Shifted mode PRSCH6 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH7                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH7 << 24)  /**< Shifted mode PRSCH7 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH8                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH8 << 24)  /**< Shifted mode PRSCH8 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH9                     (_CMU_CALCTRL_PRSDOWNSEL_PRSCH9 << 24)  /**< Shifted mode PRSCH9 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH10                    (_CMU_CALCTRL_PRSDOWNSEL_PRSCH10 << 24) /**< Shifted mode PRSCH10 for CMU_CALCTRL */
#define CMU_CALCTRL_PRSDOWNSEL_PRSCH11                    (_CMU_CALCTRL_PRSDOWNSEL_PRSCH11 << 24) /**< Shifted mode PRSCH11 for CMU_CALCTRL */

/* Bit fields for CMU CALCNT */
#define _CMU_CALCNT_RESETVALUE                            0x00000000UL                      /**< Default value for CMU_CALCNT */
#define _CMU_CALCNT_MASK                                  0x000FFFFFUL                      /**< Mask for CMU_CALCNT */
#define _CMU_CALCNT_CALCNT_SHIFT                          0                                 /**< Shift value for CMU_CALCNT */
#define _CMU_CALCNT_CALCNT_MASK                           0xFFFFFUL                         /**< Bit mask for CMU_CALCNT */
#define _CMU_CALCNT_CALCNT_DEFAULT                        0x00000000UL                      /**< Mode DEFAULT for CMU_CALCNT */
#define CMU_CALCNT_CALCNT_DEFAULT                         (_CMU_CALCNT_CALCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_CALCNT */

/* Bit fields for CMU OSCENCMD */
#define _CMU_OSCENCMD_RESETVALUE                          0x00000000UL                             /**< Default value for CMU_OSCENCMD */
#define _CMU_OSCENCMD_MASK                                0x000003FFUL                             /**< Mask for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFRCOEN                              (0x1UL << 0)                             /**< HFRCO Enable */
#define _CMU_OSCENCMD_HFRCOEN_SHIFT                       0                                        /**< Shift value for CMU_HFRCOEN */
#define _CMU_OSCENCMD_HFRCOEN_MASK                        0x1UL                                    /**< Bit mask for CMU_HFRCOEN */
#define _CMU_OSCENCMD_HFRCOEN_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFRCOEN_DEFAULT                      (_CMU_OSCENCMD_HFRCOEN_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFRCODIS                             (0x1UL << 1)                             /**< HFRCO Disable */
#define _CMU_OSCENCMD_HFRCODIS_SHIFT                      1                                        /**< Shift value for CMU_HFRCODIS */
#define _CMU_OSCENCMD_HFRCODIS_MASK                       0x2UL                                    /**< Bit mask for CMU_HFRCODIS */
#define _CMU_OSCENCMD_HFRCODIS_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFRCODIS_DEFAULT                     (_CMU_OSCENCMD_HFRCODIS_DEFAULT << 1)    /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFXOEN                               (0x1UL << 2)                             /**< HFXO Enable */
#define _CMU_OSCENCMD_HFXOEN_SHIFT                        2                                        /**< Shift value for CMU_HFXOEN */
#define _CMU_OSCENCMD_HFXOEN_MASK                         0x4UL                                    /**< Bit mask for CMU_HFXOEN */
#define _CMU_OSCENCMD_HFXOEN_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFXOEN_DEFAULT                       (_CMU_OSCENCMD_HFXOEN_DEFAULT << 2)      /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFXODIS                              (0x1UL << 3)                             /**< HFXO Disable */
#define _CMU_OSCENCMD_HFXODIS_SHIFT                       3                                        /**< Shift value for CMU_HFXODIS */
#define _CMU_OSCENCMD_HFXODIS_MASK                        0x8UL                                    /**< Bit mask for CMU_HFXODIS */
#define _CMU_OSCENCMD_HFXODIS_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_HFXODIS_DEFAULT                      (_CMU_OSCENCMD_HFXODIS_DEFAULT << 3)     /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_AUXHFRCOEN                           (0x1UL << 4)                             /**< AUXHFRCO Enable */
#define _CMU_OSCENCMD_AUXHFRCOEN_SHIFT                    4                                        /**< Shift value for CMU_AUXHFRCOEN */
#define _CMU_OSCENCMD_AUXHFRCOEN_MASK                     0x10UL                                   /**< Bit mask for CMU_AUXHFRCOEN */
#define _CMU_OSCENCMD_AUXHFRCOEN_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_AUXHFRCOEN_DEFAULT                   (_CMU_OSCENCMD_AUXHFRCOEN_DEFAULT << 4)  /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_AUXHFRCODIS                          (0x1UL << 5)                             /**< AUXHFRCO Disable */
#define _CMU_OSCENCMD_AUXHFRCODIS_SHIFT                   5                                        /**< Shift value for CMU_AUXHFRCODIS */
#define _CMU_OSCENCMD_AUXHFRCODIS_MASK                    0x20UL                                   /**< Bit mask for CMU_AUXHFRCODIS */
#define _CMU_OSCENCMD_AUXHFRCODIS_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_AUXHFRCODIS_DEFAULT                  (_CMU_OSCENCMD_AUXHFRCODIS_DEFAULT << 5) /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFRCOEN                              (0x1UL << 6)                             /**< LFRCO Enable */
#define _CMU_OSCENCMD_LFRCOEN_SHIFT                       6                                        /**< Shift value for CMU_LFRCOEN */
#define _CMU_OSCENCMD_LFRCOEN_MASK                        0x40UL                                   /**< Bit mask for CMU_LFRCOEN */
#define _CMU_OSCENCMD_LFRCOEN_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFRCOEN_DEFAULT                      (_CMU_OSCENCMD_LFRCOEN_DEFAULT << 6)     /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFRCODIS                             (0x1UL << 7)                             /**< LFRCO Disable */
#define _CMU_OSCENCMD_LFRCODIS_SHIFT                      7                                        /**< Shift value for CMU_LFRCODIS */
#define _CMU_OSCENCMD_LFRCODIS_MASK                       0x80UL                                   /**< Bit mask for CMU_LFRCODIS */
#define _CMU_OSCENCMD_LFRCODIS_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFRCODIS_DEFAULT                     (_CMU_OSCENCMD_LFRCODIS_DEFAULT << 7)    /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFXOEN                               (0x1UL << 8)                             /**< LFXO Enable */
#define _CMU_OSCENCMD_LFXOEN_SHIFT                        8                                        /**< Shift value for CMU_LFXOEN */
#define _CMU_OSCENCMD_LFXOEN_MASK                         0x100UL                                  /**< Bit mask for CMU_LFXOEN */
#define _CMU_OSCENCMD_LFXOEN_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFXOEN_DEFAULT                       (_CMU_OSCENCMD_LFXOEN_DEFAULT << 8)      /**< Shifted mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFXODIS                              (0x1UL << 9)                             /**< LFXO Disable */
#define _CMU_OSCENCMD_LFXODIS_SHIFT                       9                                        /**< Shift value for CMU_LFXODIS */
#define _CMU_OSCENCMD_LFXODIS_MASK                        0x200UL                                  /**< Bit mask for CMU_LFXODIS */
#define _CMU_OSCENCMD_LFXODIS_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_OSCENCMD */
#define CMU_OSCENCMD_LFXODIS_DEFAULT                      (_CMU_OSCENCMD_LFXODIS_DEFAULT << 9)     /**< Shifted mode DEFAULT for CMU_OSCENCMD */

/* Bit fields for CMU CMD */
#define _CMU_CMD_RESETVALUE                               0x00000000UL                              /**< Default value for CMU_CMD */
#define _CMU_CMD_MASK                                     0x00000033UL                              /**< Mask for CMU_CMD */
#define CMU_CMD_CALSTART                                  (0x1UL << 0)                              /**< Calibration Start */
#define _CMU_CMD_CALSTART_SHIFT                           0                                         /**< Shift value for CMU_CALSTART */
#define _CMU_CMD_CALSTART_MASK                            0x1UL                                     /**< Bit mask for CMU_CALSTART */
#define _CMU_CMD_CALSTART_DEFAULT                         0x00000000UL                              /**< Mode DEFAULT for CMU_CMD */
#define CMU_CMD_CALSTART_DEFAULT                          (_CMU_CMD_CALSTART_DEFAULT << 0)          /**< Shifted mode DEFAULT for CMU_CMD */
#define CMU_CMD_CALSTOP                                   (0x1UL << 1)                              /**< Calibration Stop */
#define _CMU_CMD_CALSTOP_SHIFT                            1                                         /**< Shift value for CMU_CALSTOP */
#define _CMU_CMD_CALSTOP_MASK                             0x2UL                                     /**< Bit mask for CMU_CALSTOP */
#define _CMU_CMD_CALSTOP_DEFAULT                          0x00000000UL                              /**< Mode DEFAULT for CMU_CMD */
#define CMU_CMD_CALSTOP_DEFAULT                           (_CMU_CMD_CALSTOP_DEFAULT << 1)           /**< Shifted mode DEFAULT for CMU_CMD */
#define CMU_CMD_HFXOPEAKDETSTART                          (0x1UL << 4)                              /**< HFXO Peak Detection Start */
#define _CMU_CMD_HFXOPEAKDETSTART_SHIFT                   4                                         /**< Shift value for CMU_HFXOPEAKDETSTART */
#define _CMU_CMD_HFXOPEAKDETSTART_MASK                    0x10UL                                    /**< Bit mask for CMU_HFXOPEAKDETSTART */
#define _CMU_CMD_HFXOPEAKDETSTART_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for CMU_CMD */
#define CMU_CMD_HFXOPEAKDETSTART_DEFAULT                  (_CMU_CMD_HFXOPEAKDETSTART_DEFAULT << 4)  /**< Shifted mode DEFAULT for CMU_CMD */
#define CMU_CMD_HFXOSHUNTOPTSTART                         (0x1UL << 5)                              /**< HFXO Shunt Current Optimization Start */
#define _CMU_CMD_HFXOSHUNTOPTSTART_SHIFT                  5                                         /**< Shift value for CMU_HFXOSHUNTOPTSTART */
#define _CMU_CMD_HFXOSHUNTOPTSTART_MASK                   0x20UL                                    /**< Bit mask for CMU_HFXOSHUNTOPTSTART */
#define _CMU_CMD_HFXOSHUNTOPTSTART_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CMD */
#define CMU_CMD_HFXOSHUNTOPTSTART_DEFAULT                 (_CMU_CMD_HFXOSHUNTOPTSTART_DEFAULT << 5) /**< Shifted mode DEFAULT for CMU_CMD */

/* Bit fields for CMU DBGCLKSEL */
#define _CMU_DBGCLKSEL_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_DBGCLKSEL */
#define _CMU_DBGCLKSEL_MASK                               0x00000001UL                       /**< Mask for CMU_DBGCLKSEL */
#define _CMU_DBGCLKSEL_DBG_SHIFT                          0                                  /**< Shift value for CMU_DBG */
#define _CMU_DBGCLKSEL_DBG_MASK                           0x1UL                              /**< Bit mask for CMU_DBG */
#define _CMU_DBGCLKSEL_DBG_DEFAULT                        0x00000000UL                       /**< Mode DEFAULT for CMU_DBGCLKSEL */
#define _CMU_DBGCLKSEL_DBG_AUXHFRCO                       0x00000000UL                       /**< Mode AUXHFRCO for CMU_DBGCLKSEL */
#define _CMU_DBGCLKSEL_DBG_HFCLK                          0x00000001UL                       /**< Mode HFCLK for CMU_DBGCLKSEL */
#define CMU_DBGCLKSEL_DBG_DEFAULT                         (_CMU_DBGCLKSEL_DBG_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_DBGCLKSEL */
#define CMU_DBGCLKSEL_DBG_AUXHFRCO                        (_CMU_DBGCLKSEL_DBG_AUXHFRCO << 0) /**< Shifted mode AUXHFRCO for CMU_DBGCLKSEL */
#define CMU_DBGCLKSEL_DBG_HFCLK                           (_CMU_DBGCLKSEL_DBG_HFCLK << 0)    /**< Shifted mode HFCLK for CMU_DBGCLKSEL */

/* Bit fields for CMU HFCLKSEL */
#define _CMU_HFCLKSEL_RESETVALUE                          0x00000000UL                    /**< Default value for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_MASK                                0x00000007UL                    /**< Mask for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_HF_SHIFT                            0                               /**< Shift value for CMU_HF */
#define _CMU_HFCLKSEL_HF_MASK                             0x7UL                           /**< Bit mask for CMU_HF */
#define _CMU_HFCLKSEL_HF_DEFAULT                          0x00000000UL                    /**< Mode DEFAULT for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_HF_HFRCO                            0x00000001UL                    /**< Mode HFRCO for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_HF_HFXO                             0x00000002UL                    /**< Mode HFXO for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_HF_LFRCO                            0x00000003UL                    /**< Mode LFRCO for CMU_HFCLKSEL */
#define _CMU_HFCLKSEL_HF_LFXO                             0x00000004UL                    /**< Mode LFXO for CMU_HFCLKSEL */
#define CMU_HFCLKSEL_HF_DEFAULT                           (_CMU_HFCLKSEL_HF_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFCLKSEL */
#define CMU_HFCLKSEL_HF_HFRCO                             (_CMU_HFCLKSEL_HF_HFRCO << 0)   /**< Shifted mode HFRCO for CMU_HFCLKSEL */
#define CMU_HFCLKSEL_HF_HFXO                              (_CMU_HFCLKSEL_HF_HFXO << 0)    /**< Shifted mode HFXO for CMU_HFCLKSEL */
#define CMU_HFCLKSEL_HF_LFRCO                             (_CMU_HFCLKSEL_HF_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_HFCLKSEL */
#define CMU_HFCLKSEL_HF_LFXO                              (_CMU_HFCLKSEL_HF_LFXO << 0)    /**< Shifted mode LFXO for CMU_HFCLKSEL */

/* Bit fields for CMU LFACLKSEL */
#define _CMU_LFACLKSEL_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_MASK                               0x00000007UL                       /**< Mask for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_LFA_SHIFT                          0                                  /**< Shift value for CMU_LFA */
#define _CMU_LFACLKSEL_LFA_MASK                           0x7UL                              /**< Bit mask for CMU_LFA */
#define _CMU_LFACLKSEL_LFA_DEFAULT                        0x00000000UL                       /**< Mode DEFAULT for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_LFA_DISABLED                       0x00000000UL                       /**< Mode DISABLED for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_LFA_LFRCO                          0x00000001UL                       /**< Mode LFRCO for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_LFA_LFXO                           0x00000002UL                       /**< Mode LFXO for CMU_LFACLKSEL */
#define _CMU_LFACLKSEL_LFA_ULFRCO                         0x00000004UL                       /**< Mode ULFRCO for CMU_LFACLKSEL */
#define CMU_LFACLKSEL_LFA_DEFAULT                         (_CMU_LFACLKSEL_LFA_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_LFACLKSEL */
#define CMU_LFACLKSEL_LFA_DISABLED                        (_CMU_LFACLKSEL_LFA_DISABLED << 0) /**< Shifted mode DISABLED for CMU_LFACLKSEL */
#define CMU_LFACLKSEL_LFA_LFRCO                           (_CMU_LFACLKSEL_LFA_LFRCO << 0)    /**< Shifted mode LFRCO for CMU_LFACLKSEL */
#define CMU_LFACLKSEL_LFA_LFXO                            (_CMU_LFACLKSEL_LFA_LFXO << 0)     /**< Shifted mode LFXO for CMU_LFACLKSEL */
#define CMU_LFACLKSEL_LFA_ULFRCO                          (_CMU_LFACLKSEL_LFA_ULFRCO << 0)   /**< Shifted mode ULFRCO for CMU_LFACLKSEL */

/* Bit fields for CMU LFBCLKSEL */
#define _CMU_LFBCLKSEL_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_MASK                               0x00000007UL                       /**< Mask for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_SHIFT                          0                                  /**< Shift value for CMU_LFB */
#define _CMU_LFBCLKSEL_LFB_MASK                           0x7UL                              /**< Bit mask for CMU_LFB */
#define _CMU_LFBCLKSEL_LFB_DEFAULT                        0x00000000UL                       /**< Mode DEFAULT for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_DISABLED                       0x00000000UL                       /**< Mode DISABLED for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_LFRCO                          0x00000001UL                       /**< Mode LFRCO for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_LFXO                           0x00000002UL                       /**< Mode LFXO for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_HFCLKLE                        0x00000003UL                       /**< Mode HFCLKLE for CMU_LFBCLKSEL */
#define _CMU_LFBCLKSEL_LFB_ULFRCO                         0x00000004UL                       /**< Mode ULFRCO for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_DEFAULT                         (_CMU_LFBCLKSEL_LFB_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_DISABLED                        (_CMU_LFBCLKSEL_LFB_DISABLED << 0) /**< Shifted mode DISABLED for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_LFRCO                           (_CMU_LFBCLKSEL_LFB_LFRCO << 0)    /**< Shifted mode LFRCO for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_LFXO                            (_CMU_LFBCLKSEL_LFB_LFXO << 0)     /**< Shifted mode LFXO for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_HFCLKLE                         (_CMU_LFBCLKSEL_LFB_HFCLKLE << 0)  /**< Shifted mode HFCLKLE for CMU_LFBCLKSEL */
#define CMU_LFBCLKSEL_LFB_ULFRCO                          (_CMU_LFBCLKSEL_LFB_ULFRCO << 0)   /**< Shifted mode ULFRCO for CMU_LFBCLKSEL */

/* Bit fields for CMU LFECLKSEL */
#define _CMU_LFECLKSEL_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_MASK                               0x00000007UL                       /**< Mask for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_LFE_SHIFT                          0                                  /**< Shift value for CMU_LFE */
#define _CMU_LFECLKSEL_LFE_MASK                           0x7UL                              /**< Bit mask for CMU_LFE */
#define _CMU_LFECLKSEL_LFE_DEFAULT                        0x00000000UL                       /**< Mode DEFAULT for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_LFE_DISABLED                       0x00000000UL                       /**< Mode DISABLED for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_LFE_LFRCO                          0x00000001UL                       /**< Mode LFRCO for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_LFE_LFXO                           0x00000002UL                       /**< Mode LFXO for CMU_LFECLKSEL */
#define _CMU_LFECLKSEL_LFE_ULFRCO                         0x00000004UL                       /**< Mode ULFRCO for CMU_LFECLKSEL */
#define CMU_LFECLKSEL_LFE_DEFAULT                         (_CMU_LFECLKSEL_LFE_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_LFECLKSEL */
#define CMU_LFECLKSEL_LFE_DISABLED                        (_CMU_LFECLKSEL_LFE_DISABLED << 0) /**< Shifted mode DISABLED for CMU_LFECLKSEL */
#define CMU_LFECLKSEL_LFE_LFRCO                           (_CMU_LFECLKSEL_LFE_LFRCO << 0)    /**< Shifted mode LFRCO for CMU_LFECLKSEL */
#define CMU_LFECLKSEL_LFE_LFXO                            (_CMU_LFECLKSEL_LFE_LFXO << 0)     /**< Shifted mode LFXO for CMU_LFECLKSEL */
#define CMU_LFECLKSEL_LFE_ULFRCO                          (_CMU_LFECLKSEL_LFE_ULFRCO << 0)   /**< Shifted mode ULFRCO for CMU_LFECLKSEL */

/* Bit fields for CMU STATUS */
#define _CMU_STATUS_RESETVALUE                            0x00010003UL                                /**< Default value for CMU_STATUS */
#define _CMU_STATUS_MASK                                  0x07E103FFUL                                /**< Mask for CMU_STATUS */
#define CMU_STATUS_HFRCOENS                               (0x1UL << 0)                                /**< HFRCO Enable Status */
#define _CMU_STATUS_HFRCOENS_SHIFT                        0                                           /**< Shift value for CMU_HFRCOENS */
#define _CMU_STATUS_HFRCOENS_MASK                         0x1UL                                       /**< Bit mask for CMU_HFRCOENS */
#define _CMU_STATUS_HFRCOENS_DEFAULT                      0x00000001UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFRCOENS_DEFAULT                       (_CMU_STATUS_HFRCOENS_DEFAULT << 0)         /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFRCORDY                               (0x1UL << 1)                                /**< HFRCO Ready */
#define _CMU_STATUS_HFRCORDY_SHIFT                        1                                           /**< Shift value for CMU_HFRCORDY */
#define _CMU_STATUS_HFRCORDY_MASK                         0x2UL                                       /**< Bit mask for CMU_HFRCORDY */
#define _CMU_STATUS_HFRCORDY_DEFAULT                      0x00000001UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFRCORDY_DEFAULT                       (_CMU_STATUS_HFRCORDY_DEFAULT << 1)         /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOENS                                (0x1UL << 2)                                /**< HFXO Enable Status */
#define _CMU_STATUS_HFXOENS_SHIFT                         2                                           /**< Shift value for CMU_HFXOENS */
#define _CMU_STATUS_HFXOENS_MASK                          0x4UL                                       /**< Bit mask for CMU_HFXOENS */
#define _CMU_STATUS_HFXOENS_DEFAULT                       0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOENS_DEFAULT                        (_CMU_STATUS_HFXOENS_DEFAULT << 2)          /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXORDY                                (0x1UL << 3)                                /**< HFXO Ready */
#define _CMU_STATUS_HFXORDY_SHIFT                         3                                           /**< Shift value for CMU_HFXORDY */
#define _CMU_STATUS_HFXORDY_MASK                          0x8UL                                       /**< Bit mask for CMU_HFXORDY */
#define _CMU_STATUS_HFXORDY_DEFAULT                       0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXORDY_DEFAULT                        (_CMU_STATUS_HFXORDY_DEFAULT << 3)          /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_AUXHFRCOENS                            (0x1UL << 4)                                /**< AUXHFRCO Enable Status */
#define _CMU_STATUS_AUXHFRCOENS_SHIFT                     4                                           /**< Shift value for CMU_AUXHFRCOENS */
#define _CMU_STATUS_AUXHFRCOENS_MASK                      0x10UL                                      /**< Bit mask for CMU_AUXHFRCOENS */
#define _CMU_STATUS_AUXHFRCOENS_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_AUXHFRCOENS_DEFAULT                    (_CMU_STATUS_AUXHFRCOENS_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_AUXHFRCORDY                            (0x1UL << 5)                                /**< AUXHFRCO Ready */
#define _CMU_STATUS_AUXHFRCORDY_SHIFT                     5                                           /**< Shift value for CMU_AUXHFRCORDY */
#define _CMU_STATUS_AUXHFRCORDY_MASK                      0x20UL                                      /**< Bit mask for CMU_AUXHFRCORDY */
#define _CMU_STATUS_AUXHFRCORDY_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_AUXHFRCORDY_DEFAULT                    (_CMU_STATUS_AUXHFRCORDY_DEFAULT << 5)      /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFRCOENS                               (0x1UL << 6)                                /**< LFRCO Enable Status */
#define _CMU_STATUS_LFRCOENS_SHIFT                        6                                           /**< Shift value for CMU_LFRCOENS */
#define _CMU_STATUS_LFRCOENS_MASK                         0x40UL                                      /**< Bit mask for CMU_LFRCOENS */
#define _CMU_STATUS_LFRCOENS_DEFAULT                      0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFRCOENS_DEFAULT                       (_CMU_STATUS_LFRCOENS_DEFAULT << 6)         /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFRCORDY                               (0x1UL << 7)                                /**< LFRCO Ready */
#define _CMU_STATUS_LFRCORDY_SHIFT                        7                                           /**< Shift value for CMU_LFRCORDY */
#define _CMU_STATUS_LFRCORDY_MASK                         0x80UL                                      /**< Bit mask for CMU_LFRCORDY */
#define _CMU_STATUS_LFRCORDY_DEFAULT                      0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFRCORDY_DEFAULT                       (_CMU_STATUS_LFRCORDY_DEFAULT << 7)         /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFXOENS                                (0x1UL << 8)                                /**< LFXO Enable Status */
#define _CMU_STATUS_LFXOENS_SHIFT                         8                                           /**< Shift value for CMU_LFXOENS */
#define _CMU_STATUS_LFXOENS_MASK                          0x100UL                                     /**< Bit mask for CMU_LFXOENS */
#define _CMU_STATUS_LFXOENS_DEFAULT                       0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFXOENS_DEFAULT                        (_CMU_STATUS_LFXOENS_DEFAULT << 8)          /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFXORDY                                (0x1UL << 9)                                /**< LFXO Ready */
#define _CMU_STATUS_LFXORDY_SHIFT                         9                                           /**< Shift value for CMU_LFXORDY */
#define _CMU_STATUS_LFXORDY_MASK                          0x200UL                                     /**< Bit mask for CMU_LFXORDY */
#define _CMU_STATUS_LFXORDY_DEFAULT                       0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_LFXORDY_DEFAULT                        (_CMU_STATUS_LFXORDY_DEFAULT << 9)          /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_CALRDY                                 (0x1UL << 16)                               /**< Calibration Ready */
#define _CMU_STATUS_CALRDY_SHIFT                          16                                          /**< Shift value for CMU_CALRDY */
#define _CMU_STATUS_CALRDY_MASK                           0x10000UL                                   /**< Bit mask for CMU_CALRDY */
#define _CMU_STATUS_CALRDY_DEFAULT                        0x00000001UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_CALRDY_DEFAULT                         (_CMU_STATUS_CALRDY_DEFAULT << 16)          /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOREQ                                (0x1UL << 21)                               /**< HFXO is Required By Hardware */
#define _CMU_STATUS_HFXOREQ_SHIFT                         21                                          /**< Shift value for CMU_HFXOREQ */
#define _CMU_STATUS_HFXOREQ_MASK                          0x200000UL                                  /**< Bit mask for CMU_HFXOREQ */
#define _CMU_STATUS_HFXOREQ_DEFAULT                       0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOREQ_DEFAULT                        (_CMU_STATUS_HFXOREQ_DEFAULT << 21)         /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOPEAKDETRDY                         (0x1UL << 22)                               /**< HFXO Peak Detection Ready */
#define _CMU_STATUS_HFXOPEAKDETRDY_SHIFT                  22                                          /**< Shift value for CMU_HFXOPEAKDETRDY */
#define _CMU_STATUS_HFXOPEAKDETRDY_MASK                   0x400000UL                                  /**< Bit mask for CMU_HFXOPEAKDETRDY */
#define _CMU_STATUS_HFXOPEAKDETRDY_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOPEAKDETRDY_DEFAULT                 (_CMU_STATUS_HFXOPEAKDETRDY_DEFAULT << 22)  /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOSHUNTOPTRDY                        (0x1UL << 23)                               /**< HFXO Shunt Current Optimization Ready */
#define _CMU_STATUS_HFXOSHUNTOPTRDY_SHIFT                 23                                          /**< Shift value for CMU_HFXOSHUNTOPTRDY */
#define _CMU_STATUS_HFXOSHUNTOPTRDY_MASK                  0x800000UL                                  /**< Bit mask for CMU_HFXOSHUNTOPTRDY */
#define _CMU_STATUS_HFXOSHUNTOPTRDY_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOSHUNTOPTRDY_DEFAULT                (_CMU_STATUS_HFXOSHUNTOPTRDY_DEFAULT << 23) /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOAMPHIGH                            (0x1UL << 24)                               /**< HFXO Oscillation Amplitude is Too High */
#define _CMU_STATUS_HFXOAMPHIGH_SHIFT                     24                                          /**< Shift value for CMU_HFXOAMPHIGH */
#define _CMU_STATUS_HFXOAMPHIGH_MASK                      0x1000000UL                                 /**< Bit mask for CMU_HFXOAMPHIGH */
#define _CMU_STATUS_HFXOAMPHIGH_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOAMPHIGH_DEFAULT                    (_CMU_STATUS_HFXOAMPHIGH_DEFAULT << 24)     /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOAMPLOW                             (0x1UL << 25)                               /**< HFXO Amplitude Tuning Value Too Low */
#define _CMU_STATUS_HFXOAMPLOW_SHIFT                      25                                          /**< Shift value for CMU_HFXOAMPLOW */
#define _CMU_STATUS_HFXOAMPLOW_MASK                       0x2000000UL                                 /**< Bit mask for CMU_HFXOAMPLOW */
#define _CMU_STATUS_HFXOAMPLOW_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOAMPLOW_DEFAULT                     (_CMU_STATUS_HFXOAMPLOW_DEFAULT << 25)      /**< Shifted mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOREGILOW                            (0x1UL << 26)                               /**< HFXO Regulator Shunt Current Too Low */
#define _CMU_STATUS_HFXOREGILOW_SHIFT                     26                                          /**< Shift value for CMU_HFXOREGILOW */
#define _CMU_STATUS_HFXOREGILOW_MASK                      0x4000000UL                                 /**< Bit mask for CMU_HFXOREGILOW */
#define _CMU_STATUS_HFXOREGILOW_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for CMU_STATUS */
#define CMU_STATUS_HFXOREGILOW_DEFAULT                    (_CMU_STATUS_HFXOREGILOW_DEFAULT << 26)     /**< Shifted mode DEFAULT for CMU_STATUS */

/* Bit fields for CMU HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_RESETVALUE                       0x00000001UL                             /**< Default value for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_MASK                             0x00000007UL                             /**< Mask for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_SELECTED_SHIFT                   0                                        /**< Shift value for CMU_SELECTED */
#define _CMU_HFCLKSTATUS_SELECTED_MASK                    0x7UL                                    /**< Bit mask for CMU_SELECTED */
#define _CMU_HFCLKSTATUS_SELECTED_DEFAULT                 0x00000001UL                             /**< Mode DEFAULT for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_SELECTED_HFRCO                   0x00000001UL                             /**< Mode HFRCO for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_SELECTED_HFXO                    0x00000002UL                             /**< Mode HFXO for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_SELECTED_LFRCO                   0x00000003UL                             /**< Mode LFRCO for CMU_HFCLKSTATUS */
#define _CMU_HFCLKSTATUS_SELECTED_LFXO                    0x00000004UL                             /**< Mode LFXO for CMU_HFCLKSTATUS */
#define CMU_HFCLKSTATUS_SELECTED_DEFAULT                  (_CMU_HFCLKSTATUS_SELECTED_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFCLKSTATUS */
#define CMU_HFCLKSTATUS_SELECTED_HFRCO                    (_CMU_HFCLKSTATUS_SELECTED_HFRCO << 0)   /**< Shifted mode HFRCO for CMU_HFCLKSTATUS */
#define CMU_HFCLKSTATUS_SELECTED_HFXO                     (_CMU_HFCLKSTATUS_SELECTED_HFXO << 0)    /**< Shifted mode HFXO for CMU_HFCLKSTATUS */
#define CMU_HFCLKSTATUS_SELECTED_LFRCO                    (_CMU_HFCLKSTATUS_SELECTED_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_HFCLKSTATUS */
#define CMU_HFCLKSTATUS_SELECTED_LFXO                     (_CMU_HFCLKSTATUS_SELECTED_LFXO << 0)    /**< Shifted mode LFXO for CMU_HFCLKSTATUS */

/* Bit fields for CMU HFXOTRIMSTATUS */
#define _CMU_HFXOTRIMSTATUS_RESETVALUE                    0x00000500UL                                    /**< Default value for CMU_HFXOTRIMSTATUS */
#define _CMU_HFXOTRIMSTATUS_MASK                          0x000007FFUL                                    /**< Mask for CMU_HFXOTRIMSTATUS */
#define _CMU_HFXOTRIMSTATUS_IBTRIMXOCORE_SHIFT            0                                               /**< Shift value for CMU_IBTRIMXOCORE */
#define _CMU_HFXOTRIMSTATUS_IBTRIMXOCORE_MASK             0x7FUL                                          /**< Bit mask for CMU_IBTRIMXOCORE */
#define _CMU_HFXOTRIMSTATUS_IBTRIMXOCORE_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for CMU_HFXOTRIMSTATUS */
#define CMU_HFXOTRIMSTATUS_IBTRIMXOCORE_DEFAULT           (_CMU_HFXOTRIMSTATUS_IBTRIMXOCORE_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_HFXOTRIMSTATUS */
#define _CMU_HFXOTRIMSTATUS_REGISH_SHIFT                  7                                               /**< Shift value for CMU_REGISH */
#define _CMU_HFXOTRIMSTATUS_REGISH_MASK                   0x780UL                                         /**< Bit mask for CMU_REGISH */
#define _CMU_HFXOTRIMSTATUS_REGISH_DEFAULT                0x0000000AUL                                    /**< Mode DEFAULT for CMU_HFXOTRIMSTATUS */
#define CMU_HFXOTRIMSTATUS_REGISH_DEFAULT                 (_CMU_HFXOTRIMSTATUS_REGISH_DEFAULT << 7)       /**< Shifted mode DEFAULT for CMU_HFXOTRIMSTATUS */

/* Bit fields for CMU IF */
#define _CMU_IF_RESETVALUE                                0x00000001UL                            /**< Default value for CMU_IF */
#define _CMU_IF_MASK                                      0x80007F7FUL                            /**< Mask for CMU_IF */
#define CMU_IF_HFRCORDY                                   (0x1UL << 0)                            /**< HFRCO Ready Interrupt Flag */
#define _CMU_IF_HFRCORDY_SHIFT                            0                                       /**< Shift value for CMU_HFRCORDY */
#define _CMU_IF_HFRCORDY_MASK                             0x1UL                                   /**< Bit mask for CMU_HFRCORDY */
#define _CMU_IF_HFRCORDY_DEFAULT                          0x00000001UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFRCORDY_DEFAULT                           (_CMU_IF_HFRCORDY_DEFAULT << 0)         /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXORDY                                    (0x1UL << 1)                            /**< HFXO Ready Interrupt Flag */
#define _CMU_IF_HFXORDY_SHIFT                             1                                       /**< Shift value for CMU_HFXORDY */
#define _CMU_IF_HFXORDY_MASK                              0x2UL                                   /**< Bit mask for CMU_HFXORDY */
#define _CMU_IF_HFXORDY_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXORDY_DEFAULT                            (_CMU_IF_HFXORDY_DEFAULT << 1)          /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_LFRCORDY                                   (0x1UL << 2)                            /**< LFRCO Ready Interrupt Flag */
#define _CMU_IF_LFRCORDY_SHIFT                            2                                       /**< Shift value for CMU_LFRCORDY */
#define _CMU_IF_LFRCORDY_MASK                             0x4UL                                   /**< Bit mask for CMU_LFRCORDY */
#define _CMU_IF_LFRCORDY_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_LFRCORDY_DEFAULT                           (_CMU_IF_LFRCORDY_DEFAULT << 2)         /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_LFXORDY                                    (0x1UL << 3)                            /**< LFXO Ready Interrupt Flag */
#define _CMU_IF_LFXORDY_SHIFT                             3                                       /**< Shift value for CMU_LFXORDY */
#define _CMU_IF_LFXORDY_MASK                              0x8UL                                   /**< Bit mask for CMU_LFXORDY */
#define _CMU_IF_LFXORDY_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_LFXORDY_DEFAULT                            (_CMU_IF_LFXORDY_DEFAULT << 3)          /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_AUXHFRCORDY                                (0x1UL << 4)                            /**< AUXHFRCO Ready Interrupt Flag */
#define _CMU_IF_AUXHFRCORDY_SHIFT                         4                                       /**< Shift value for CMU_AUXHFRCORDY */
#define _CMU_IF_AUXHFRCORDY_MASK                          0x10UL                                  /**< Bit mask for CMU_AUXHFRCORDY */
#define _CMU_IF_AUXHFRCORDY_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_AUXHFRCORDY_DEFAULT                        (_CMU_IF_AUXHFRCORDY_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_CALRDY                                     (0x1UL << 5)                            /**< Calibration Ready Interrupt Flag */
#define _CMU_IF_CALRDY_SHIFT                              5                                       /**< Shift value for CMU_CALRDY */
#define _CMU_IF_CALRDY_MASK                               0x20UL                                  /**< Bit mask for CMU_CALRDY */
#define _CMU_IF_CALRDY_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_CALRDY_DEFAULT                             (_CMU_IF_CALRDY_DEFAULT << 5)           /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_CALOF                                      (0x1UL << 6)                            /**< Calibration Overflow Interrupt Flag */
#define _CMU_IF_CALOF_SHIFT                               6                                       /**< Shift value for CMU_CALOF */
#define _CMU_IF_CALOF_MASK                                0x40UL                                  /**< Bit mask for CMU_CALOF */
#define _CMU_IF_CALOF_DEFAULT                             0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_CALOF_DEFAULT                              (_CMU_IF_CALOF_DEFAULT << 6)            /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXODISERR                                 (0x1UL << 8)                            /**< HFXO Disable Error Interrupt Flag */
#define _CMU_IF_HFXODISERR_SHIFT                          8                                       /**< Shift value for CMU_HFXODISERR */
#define _CMU_IF_HFXODISERR_MASK                           0x100UL                                 /**< Bit mask for CMU_HFXODISERR */
#define _CMU_IF_HFXODISERR_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXODISERR_DEFAULT                         (_CMU_IF_HFXODISERR_DEFAULT << 8)       /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOAUTOSW                                 (0x1UL << 9)                            /**< HFXO Automatic Switch Interrupt Flag */
#define _CMU_IF_HFXOAUTOSW_SHIFT                          9                                       /**< Shift value for CMU_HFXOAUTOSW */
#define _CMU_IF_HFXOAUTOSW_MASK                           0x200UL                                 /**< Bit mask for CMU_HFXOAUTOSW */
#define _CMU_IF_HFXOAUTOSW_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOAUTOSW_DEFAULT                         (_CMU_IF_HFXOAUTOSW_DEFAULT << 9)       /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOPEAKDETERR                             (0x1UL << 10)                           /**< HFXO Automatic Peak Detection Error Interrupt Flag */
#define _CMU_IF_HFXOPEAKDETERR_SHIFT                      10                                      /**< Shift value for CMU_HFXOPEAKDETERR */
#define _CMU_IF_HFXOPEAKDETERR_MASK                       0x400UL                                 /**< Bit mask for CMU_HFXOPEAKDETERR */
#define _CMU_IF_HFXOPEAKDETERR_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOPEAKDETERR_DEFAULT                     (_CMU_IF_HFXOPEAKDETERR_DEFAULT << 10)  /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOPEAKDETRDY                             (0x1UL << 11)                           /**< HFXO Automatic Peak Detection Ready Interrupt Flag */
#define _CMU_IF_HFXOPEAKDETRDY_SHIFT                      11                                      /**< Shift value for CMU_HFXOPEAKDETRDY */
#define _CMU_IF_HFXOPEAKDETRDY_MASK                       0x800UL                                 /**< Bit mask for CMU_HFXOPEAKDETRDY */
#define _CMU_IF_HFXOPEAKDETRDY_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOPEAKDETRDY_DEFAULT                     (_CMU_IF_HFXOPEAKDETRDY_DEFAULT << 11)  /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOSHUNTOPTRDY                            (0x1UL << 12)                           /**< HFXO Automatic Shunt Current Optimization Ready Interrupt Flag */
#define _CMU_IF_HFXOSHUNTOPTRDY_SHIFT                     12                                      /**< Shift value for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IF_HFXOSHUNTOPTRDY_MASK                      0x1000UL                                /**< Bit mask for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IF_HFXOSHUNTOPTRDY_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFXOSHUNTOPTRDY_DEFAULT                    (_CMU_IF_HFXOSHUNTOPTRDY_DEFAULT << 12) /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_HFRCODIS                                   (0x1UL << 13)                           /**< HFRCO Disable Interrupt Flag */
#define _CMU_IF_HFRCODIS_SHIFT                            13                                      /**< Shift value for CMU_HFRCODIS */
#define _CMU_IF_HFRCODIS_MASK                             0x2000UL                                /**< Bit mask for CMU_HFRCODIS */
#define _CMU_IF_HFRCODIS_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_HFRCODIS_DEFAULT                           (_CMU_IF_HFRCODIS_DEFAULT << 13)        /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_LFTIMEOUTERR                               (0x1UL << 14)                           /**< Low Frequency Timeout Error Interrupt Flag */
#define _CMU_IF_LFTIMEOUTERR_SHIFT                        14                                      /**< Shift value for CMU_LFTIMEOUTERR */
#define _CMU_IF_LFTIMEOUTERR_MASK                         0x4000UL                                /**< Bit mask for CMU_LFTIMEOUTERR */
#define _CMU_IF_LFTIMEOUTERR_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_LFTIMEOUTERR_DEFAULT                       (_CMU_IF_LFTIMEOUTERR_DEFAULT << 14)    /**< Shifted mode DEFAULT for CMU_IF */
#define CMU_IF_CMUERR                                     (0x1UL << 31)                           /**< CMU Error Interrupt Flag */
#define _CMU_IF_CMUERR_SHIFT                              31                                      /**< Shift value for CMU_CMUERR */
#define _CMU_IF_CMUERR_MASK                               0x80000000UL                            /**< Bit mask for CMU_CMUERR */
#define _CMU_IF_CMUERR_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for CMU_IF */
#define CMU_IF_CMUERR_DEFAULT                             (_CMU_IF_CMUERR_DEFAULT << 31)          /**< Shifted mode DEFAULT for CMU_IF */

/* Bit fields for CMU IFS */
#define _CMU_IFS_RESETVALUE                               0x00000000UL                             /**< Default value for CMU_IFS */
#define _CMU_IFS_MASK                                     0x80007F7FUL                             /**< Mask for CMU_IFS */
#define CMU_IFS_HFRCORDY                                  (0x1UL << 0)                             /**< Set HFRCORDY Interrupt Flag */
#define _CMU_IFS_HFRCORDY_SHIFT                           0                                        /**< Shift value for CMU_HFRCORDY */
#define _CMU_IFS_HFRCORDY_MASK                            0x1UL                                    /**< Bit mask for CMU_HFRCORDY */
#define _CMU_IFS_HFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFRCORDY_DEFAULT                          (_CMU_IFS_HFRCORDY_DEFAULT << 0)         /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXORDY                                   (0x1UL << 1)                             /**< Set HFXORDY Interrupt Flag */
#define _CMU_IFS_HFXORDY_SHIFT                            1                                        /**< Shift value for CMU_HFXORDY */
#define _CMU_IFS_HFXORDY_MASK                             0x2UL                                    /**< Bit mask for CMU_HFXORDY */
#define _CMU_IFS_HFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXORDY_DEFAULT                           (_CMU_IFS_HFXORDY_DEFAULT << 1)          /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFRCORDY                                  (0x1UL << 2)                             /**< Set LFRCORDY Interrupt Flag */
#define _CMU_IFS_LFRCORDY_SHIFT                           2                                        /**< Shift value for CMU_LFRCORDY */
#define _CMU_IFS_LFRCORDY_MASK                            0x4UL                                    /**< Bit mask for CMU_LFRCORDY */
#define _CMU_IFS_LFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFRCORDY_DEFAULT                          (_CMU_IFS_LFRCORDY_DEFAULT << 2)         /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFXORDY                                   (0x1UL << 3)                             /**< Set LFXORDY Interrupt Flag */
#define _CMU_IFS_LFXORDY_SHIFT                            3                                        /**< Shift value for CMU_LFXORDY */
#define _CMU_IFS_LFXORDY_MASK                             0x8UL                                    /**< Bit mask for CMU_LFXORDY */
#define _CMU_IFS_LFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFXORDY_DEFAULT                           (_CMU_IFS_LFXORDY_DEFAULT << 3)          /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_AUXHFRCORDY                               (0x1UL << 4)                             /**< Set AUXHFRCORDY Interrupt Flag */
#define _CMU_IFS_AUXHFRCORDY_SHIFT                        4                                        /**< Shift value for CMU_AUXHFRCORDY */
#define _CMU_IFS_AUXHFRCORDY_MASK                         0x10UL                                   /**< Bit mask for CMU_AUXHFRCORDY */
#define _CMU_IFS_AUXHFRCORDY_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_AUXHFRCORDY_DEFAULT                       (_CMU_IFS_AUXHFRCORDY_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_CALRDY                                    (0x1UL << 5)                             /**< Set CALRDY Interrupt Flag */
#define _CMU_IFS_CALRDY_SHIFT                             5                                        /**< Shift value for CMU_CALRDY */
#define _CMU_IFS_CALRDY_MASK                              0x20UL                                   /**< Bit mask for CMU_CALRDY */
#define _CMU_IFS_CALRDY_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_CALRDY_DEFAULT                            (_CMU_IFS_CALRDY_DEFAULT << 5)           /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_CALOF                                     (0x1UL << 6)                             /**< Set CALOF Interrupt Flag */
#define _CMU_IFS_CALOF_SHIFT                              6                                        /**< Shift value for CMU_CALOF */
#define _CMU_IFS_CALOF_MASK                               0x40UL                                   /**< Bit mask for CMU_CALOF */
#define _CMU_IFS_CALOF_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_CALOF_DEFAULT                             (_CMU_IFS_CALOF_DEFAULT << 6)            /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXODISERR                                (0x1UL << 8)                             /**< Set HFXODISERR Interrupt Flag */
#define _CMU_IFS_HFXODISERR_SHIFT                         8                                        /**< Shift value for CMU_HFXODISERR */
#define _CMU_IFS_HFXODISERR_MASK                          0x100UL                                  /**< Bit mask for CMU_HFXODISERR */
#define _CMU_IFS_HFXODISERR_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXODISERR_DEFAULT                        (_CMU_IFS_HFXODISERR_DEFAULT << 8)       /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOAUTOSW                                (0x1UL << 9)                             /**< Set HFXOAUTOSW Interrupt Flag */
#define _CMU_IFS_HFXOAUTOSW_SHIFT                         9                                        /**< Shift value for CMU_HFXOAUTOSW */
#define _CMU_IFS_HFXOAUTOSW_MASK                          0x200UL                                  /**< Bit mask for CMU_HFXOAUTOSW */
#define _CMU_IFS_HFXOAUTOSW_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOAUTOSW_DEFAULT                        (_CMU_IFS_HFXOAUTOSW_DEFAULT << 9)       /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOPEAKDETERR                            (0x1UL << 10)                            /**< Set HFXOPEAKDETERR Interrupt Flag */
#define _CMU_IFS_HFXOPEAKDETERR_SHIFT                     10                                       /**< Shift value for CMU_HFXOPEAKDETERR */
#define _CMU_IFS_HFXOPEAKDETERR_MASK                      0x400UL                                  /**< Bit mask for CMU_HFXOPEAKDETERR */
#define _CMU_IFS_HFXOPEAKDETERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOPEAKDETERR_DEFAULT                    (_CMU_IFS_HFXOPEAKDETERR_DEFAULT << 10)  /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOPEAKDETRDY                            (0x1UL << 11)                            /**< Set HFXOPEAKDETRDY Interrupt Flag */
#define _CMU_IFS_HFXOPEAKDETRDY_SHIFT                     11                                       /**< Shift value for CMU_HFXOPEAKDETRDY */
#define _CMU_IFS_HFXOPEAKDETRDY_MASK                      0x800UL                                  /**< Bit mask for CMU_HFXOPEAKDETRDY */
#define _CMU_IFS_HFXOPEAKDETRDY_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOPEAKDETRDY_DEFAULT                    (_CMU_IFS_HFXOPEAKDETRDY_DEFAULT << 11)  /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOSHUNTOPTRDY                           (0x1UL << 12)                            /**< Set HFXOSHUNTOPTRDY Interrupt Flag */
#define _CMU_IFS_HFXOSHUNTOPTRDY_SHIFT                    12                                       /**< Shift value for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IFS_HFXOSHUNTOPTRDY_MASK                     0x1000UL                                 /**< Bit mask for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IFS_HFXOSHUNTOPTRDY_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFXOSHUNTOPTRDY_DEFAULT                   (_CMU_IFS_HFXOSHUNTOPTRDY_DEFAULT << 12) /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFRCODIS                                  (0x1UL << 13)                            /**< Set HFRCODIS Interrupt Flag */
#define _CMU_IFS_HFRCODIS_SHIFT                           13                                       /**< Shift value for CMU_HFRCODIS */
#define _CMU_IFS_HFRCODIS_MASK                            0x2000UL                                 /**< Bit mask for CMU_HFRCODIS */
#define _CMU_IFS_HFRCODIS_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_HFRCODIS_DEFAULT                          (_CMU_IFS_HFRCODIS_DEFAULT << 13)        /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFTIMEOUTERR                              (0x1UL << 14)                            /**< Set LFTIMEOUTERR Interrupt Flag */
#define _CMU_IFS_LFTIMEOUTERR_SHIFT                       14                                       /**< Shift value for CMU_LFTIMEOUTERR */
#define _CMU_IFS_LFTIMEOUTERR_MASK                        0x4000UL                                 /**< Bit mask for CMU_LFTIMEOUTERR */
#define _CMU_IFS_LFTIMEOUTERR_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_LFTIMEOUTERR_DEFAULT                      (_CMU_IFS_LFTIMEOUTERR_DEFAULT << 14)    /**< Shifted mode DEFAULT for CMU_IFS */
#define CMU_IFS_CMUERR                                    (0x1UL << 31)                            /**< Set CMUERR Interrupt Flag */
#define _CMU_IFS_CMUERR_SHIFT                             31                                       /**< Shift value for CMU_CMUERR */
#define _CMU_IFS_CMUERR_MASK                              0x80000000UL                             /**< Bit mask for CMU_CMUERR */
#define _CMU_IFS_CMUERR_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IFS */
#define CMU_IFS_CMUERR_DEFAULT                            (_CMU_IFS_CMUERR_DEFAULT << 31)          /**< Shifted mode DEFAULT for CMU_IFS */

/* Bit fields for CMU IFC */
#define _CMU_IFC_RESETVALUE                               0x00000000UL                             /**< Default value for CMU_IFC */
#define _CMU_IFC_MASK                                     0x80007F7FUL                             /**< Mask for CMU_IFC */
#define CMU_IFC_HFRCORDY                                  (0x1UL << 0)                             /**< Clear HFRCORDY Interrupt Flag */
#define _CMU_IFC_HFRCORDY_SHIFT                           0                                        /**< Shift value for CMU_HFRCORDY */
#define _CMU_IFC_HFRCORDY_MASK                            0x1UL                                    /**< Bit mask for CMU_HFRCORDY */
#define _CMU_IFC_HFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFRCORDY_DEFAULT                          (_CMU_IFC_HFRCORDY_DEFAULT << 0)         /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXORDY                                   (0x1UL << 1)                             /**< Clear HFXORDY Interrupt Flag */
#define _CMU_IFC_HFXORDY_SHIFT                            1                                        /**< Shift value for CMU_HFXORDY */
#define _CMU_IFC_HFXORDY_MASK                             0x2UL                                    /**< Bit mask for CMU_HFXORDY */
#define _CMU_IFC_HFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXORDY_DEFAULT                           (_CMU_IFC_HFXORDY_DEFAULT << 1)          /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFRCORDY                                  (0x1UL << 2)                             /**< Clear LFRCORDY Interrupt Flag */
#define _CMU_IFC_LFRCORDY_SHIFT                           2                                        /**< Shift value for CMU_LFRCORDY */
#define _CMU_IFC_LFRCORDY_MASK                            0x4UL                                    /**< Bit mask for CMU_LFRCORDY */
#define _CMU_IFC_LFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFRCORDY_DEFAULT                          (_CMU_IFC_LFRCORDY_DEFAULT << 2)         /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFXORDY                                   (0x1UL << 3)                             /**< Clear LFXORDY Interrupt Flag */
#define _CMU_IFC_LFXORDY_SHIFT                            3                                        /**< Shift value for CMU_LFXORDY */
#define _CMU_IFC_LFXORDY_MASK                             0x8UL                                    /**< Bit mask for CMU_LFXORDY */
#define _CMU_IFC_LFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFXORDY_DEFAULT                           (_CMU_IFC_LFXORDY_DEFAULT << 3)          /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_AUXHFRCORDY                               (0x1UL << 4)                             /**< Clear AUXHFRCORDY Interrupt Flag */
#define _CMU_IFC_AUXHFRCORDY_SHIFT                        4                                        /**< Shift value for CMU_AUXHFRCORDY */
#define _CMU_IFC_AUXHFRCORDY_MASK                         0x10UL                                   /**< Bit mask for CMU_AUXHFRCORDY */
#define _CMU_IFC_AUXHFRCORDY_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_AUXHFRCORDY_DEFAULT                       (_CMU_IFC_AUXHFRCORDY_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_CALRDY                                    (0x1UL << 5)                             /**< Clear CALRDY Interrupt Flag */
#define _CMU_IFC_CALRDY_SHIFT                             5                                        /**< Shift value for CMU_CALRDY */
#define _CMU_IFC_CALRDY_MASK                              0x20UL                                   /**< Bit mask for CMU_CALRDY */
#define _CMU_IFC_CALRDY_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_CALRDY_DEFAULT                            (_CMU_IFC_CALRDY_DEFAULT << 5)           /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_CALOF                                     (0x1UL << 6)                             /**< Clear CALOF Interrupt Flag */
#define _CMU_IFC_CALOF_SHIFT                              6                                        /**< Shift value for CMU_CALOF */
#define _CMU_IFC_CALOF_MASK                               0x40UL                                   /**< Bit mask for CMU_CALOF */
#define _CMU_IFC_CALOF_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_CALOF_DEFAULT                             (_CMU_IFC_CALOF_DEFAULT << 6)            /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXODISERR                                (0x1UL << 8)                             /**< Clear HFXODISERR Interrupt Flag */
#define _CMU_IFC_HFXODISERR_SHIFT                         8                                        /**< Shift value for CMU_HFXODISERR */
#define _CMU_IFC_HFXODISERR_MASK                          0x100UL                                  /**< Bit mask for CMU_HFXODISERR */
#define _CMU_IFC_HFXODISERR_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXODISERR_DEFAULT                        (_CMU_IFC_HFXODISERR_DEFAULT << 8)       /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOAUTOSW                                (0x1UL << 9)                             /**< Clear HFXOAUTOSW Interrupt Flag */
#define _CMU_IFC_HFXOAUTOSW_SHIFT                         9                                        /**< Shift value for CMU_HFXOAUTOSW */
#define _CMU_IFC_HFXOAUTOSW_MASK                          0x200UL                                  /**< Bit mask for CMU_HFXOAUTOSW */
#define _CMU_IFC_HFXOAUTOSW_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOAUTOSW_DEFAULT                        (_CMU_IFC_HFXOAUTOSW_DEFAULT << 9)       /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOPEAKDETERR                            (0x1UL << 10)                            /**< Clear HFXOPEAKDETERR Interrupt Flag */
#define _CMU_IFC_HFXOPEAKDETERR_SHIFT                     10                                       /**< Shift value for CMU_HFXOPEAKDETERR */
#define _CMU_IFC_HFXOPEAKDETERR_MASK                      0x400UL                                  /**< Bit mask for CMU_HFXOPEAKDETERR */
#define _CMU_IFC_HFXOPEAKDETERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOPEAKDETERR_DEFAULT                    (_CMU_IFC_HFXOPEAKDETERR_DEFAULT << 10)  /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOPEAKDETRDY                            (0x1UL << 11)                            /**< Clear HFXOPEAKDETRDY Interrupt Flag */
#define _CMU_IFC_HFXOPEAKDETRDY_SHIFT                     11                                       /**< Shift value for CMU_HFXOPEAKDETRDY */
#define _CMU_IFC_HFXOPEAKDETRDY_MASK                      0x800UL                                  /**< Bit mask for CMU_HFXOPEAKDETRDY */
#define _CMU_IFC_HFXOPEAKDETRDY_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOPEAKDETRDY_DEFAULT                    (_CMU_IFC_HFXOPEAKDETRDY_DEFAULT << 11)  /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOSHUNTOPTRDY                           (0x1UL << 12)                            /**< Clear HFXOSHUNTOPTRDY Interrupt Flag */
#define _CMU_IFC_HFXOSHUNTOPTRDY_SHIFT                    12                                       /**< Shift value for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IFC_HFXOSHUNTOPTRDY_MASK                     0x1000UL                                 /**< Bit mask for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IFC_HFXOSHUNTOPTRDY_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFXOSHUNTOPTRDY_DEFAULT                   (_CMU_IFC_HFXOSHUNTOPTRDY_DEFAULT << 12) /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFRCODIS                                  (0x1UL << 13)                            /**< Clear HFRCODIS Interrupt Flag */
#define _CMU_IFC_HFRCODIS_SHIFT                           13                                       /**< Shift value for CMU_HFRCODIS */
#define _CMU_IFC_HFRCODIS_MASK                            0x2000UL                                 /**< Bit mask for CMU_HFRCODIS */
#define _CMU_IFC_HFRCODIS_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_HFRCODIS_DEFAULT                          (_CMU_IFC_HFRCODIS_DEFAULT << 13)        /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFTIMEOUTERR                              (0x1UL << 14)                            /**< Clear LFTIMEOUTERR Interrupt Flag */
#define _CMU_IFC_LFTIMEOUTERR_SHIFT                       14                                       /**< Shift value for CMU_LFTIMEOUTERR */
#define _CMU_IFC_LFTIMEOUTERR_MASK                        0x4000UL                                 /**< Bit mask for CMU_LFTIMEOUTERR */
#define _CMU_IFC_LFTIMEOUTERR_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_LFTIMEOUTERR_DEFAULT                      (_CMU_IFC_LFTIMEOUTERR_DEFAULT << 14)    /**< Shifted mode DEFAULT for CMU_IFC */
#define CMU_IFC_CMUERR                                    (0x1UL << 31)                            /**< Clear CMUERR Interrupt Flag */
#define _CMU_IFC_CMUERR_SHIFT                             31                                       /**< Shift value for CMU_CMUERR */
#define _CMU_IFC_CMUERR_MASK                              0x80000000UL                             /**< Bit mask for CMU_CMUERR */
#define _CMU_IFC_CMUERR_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IFC */
#define CMU_IFC_CMUERR_DEFAULT                            (_CMU_IFC_CMUERR_DEFAULT << 31)          /**< Shifted mode DEFAULT for CMU_IFC */

/* Bit fields for CMU IEN */
#define _CMU_IEN_RESETVALUE                               0x00000000UL                             /**< Default value for CMU_IEN */
#define _CMU_IEN_MASK                                     0x80007F7FUL                             /**< Mask for CMU_IEN */
#define CMU_IEN_HFRCORDY                                  (0x1UL << 0)                             /**< HFRCORDY Interrupt Enable */
#define _CMU_IEN_HFRCORDY_SHIFT                           0                                        /**< Shift value for CMU_HFRCORDY */
#define _CMU_IEN_HFRCORDY_MASK                            0x1UL                                    /**< Bit mask for CMU_HFRCORDY */
#define _CMU_IEN_HFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFRCORDY_DEFAULT                          (_CMU_IEN_HFRCORDY_DEFAULT << 0)         /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXORDY                                   (0x1UL << 1)                             /**< HFXORDY Interrupt Enable */
#define _CMU_IEN_HFXORDY_SHIFT                            1                                        /**< Shift value for CMU_HFXORDY */
#define _CMU_IEN_HFXORDY_MASK                             0x2UL                                    /**< Bit mask for CMU_HFXORDY */
#define _CMU_IEN_HFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXORDY_DEFAULT                           (_CMU_IEN_HFXORDY_DEFAULT << 1)          /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFRCORDY                                  (0x1UL << 2)                             /**< LFRCORDY Interrupt Enable */
#define _CMU_IEN_LFRCORDY_SHIFT                           2                                        /**< Shift value for CMU_LFRCORDY */
#define _CMU_IEN_LFRCORDY_MASK                            0x4UL                                    /**< Bit mask for CMU_LFRCORDY */
#define _CMU_IEN_LFRCORDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFRCORDY_DEFAULT                          (_CMU_IEN_LFRCORDY_DEFAULT << 2)         /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFXORDY                                   (0x1UL << 3)                             /**< LFXORDY Interrupt Enable */
#define _CMU_IEN_LFXORDY_SHIFT                            3                                        /**< Shift value for CMU_LFXORDY */
#define _CMU_IEN_LFXORDY_MASK                             0x8UL                                    /**< Bit mask for CMU_LFXORDY */
#define _CMU_IEN_LFXORDY_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFXORDY_DEFAULT                           (_CMU_IEN_LFXORDY_DEFAULT << 3)          /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_AUXHFRCORDY                               (0x1UL << 4)                             /**< AUXHFRCORDY Interrupt Enable */
#define _CMU_IEN_AUXHFRCORDY_SHIFT                        4                                        /**< Shift value for CMU_AUXHFRCORDY */
#define _CMU_IEN_AUXHFRCORDY_MASK                         0x10UL                                   /**< Bit mask for CMU_AUXHFRCORDY */
#define _CMU_IEN_AUXHFRCORDY_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_AUXHFRCORDY_DEFAULT                       (_CMU_IEN_AUXHFRCORDY_DEFAULT << 4)      /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_CALRDY                                    (0x1UL << 5)                             /**< CALRDY Interrupt Enable */
#define _CMU_IEN_CALRDY_SHIFT                             5                                        /**< Shift value for CMU_CALRDY */
#define _CMU_IEN_CALRDY_MASK                              0x20UL                                   /**< Bit mask for CMU_CALRDY */
#define _CMU_IEN_CALRDY_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_CALRDY_DEFAULT                            (_CMU_IEN_CALRDY_DEFAULT << 5)           /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_CALOF                                     (0x1UL << 6)                             /**< CALOF Interrupt Enable */
#define _CMU_IEN_CALOF_SHIFT                              6                                        /**< Shift value for CMU_CALOF */
#define _CMU_IEN_CALOF_MASK                               0x40UL                                   /**< Bit mask for CMU_CALOF */
#define _CMU_IEN_CALOF_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_CALOF_DEFAULT                             (_CMU_IEN_CALOF_DEFAULT << 6)            /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXODISERR                                (0x1UL << 8)                             /**< HFXODISERR Interrupt Enable */
#define _CMU_IEN_HFXODISERR_SHIFT                         8                                        /**< Shift value for CMU_HFXODISERR */
#define _CMU_IEN_HFXODISERR_MASK                          0x100UL                                  /**< Bit mask for CMU_HFXODISERR */
#define _CMU_IEN_HFXODISERR_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXODISERR_DEFAULT                        (_CMU_IEN_HFXODISERR_DEFAULT << 8)       /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOAUTOSW                                (0x1UL << 9)                             /**< HFXOAUTOSW Interrupt Enable */
#define _CMU_IEN_HFXOAUTOSW_SHIFT                         9                                        /**< Shift value for CMU_HFXOAUTOSW */
#define _CMU_IEN_HFXOAUTOSW_MASK                          0x200UL                                  /**< Bit mask for CMU_HFXOAUTOSW */
#define _CMU_IEN_HFXOAUTOSW_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOAUTOSW_DEFAULT                        (_CMU_IEN_HFXOAUTOSW_DEFAULT << 9)       /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOPEAKDETERR                            (0x1UL << 10)                            /**< HFXOPEAKDETERR Interrupt Enable */
#define _CMU_IEN_HFXOPEAKDETERR_SHIFT                     10                                       /**< Shift value for CMU_HFXOPEAKDETERR */
#define _CMU_IEN_HFXOPEAKDETERR_MASK                      0x400UL                                  /**< Bit mask for CMU_HFXOPEAKDETERR */
#define _CMU_IEN_HFXOPEAKDETERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOPEAKDETERR_DEFAULT                    (_CMU_IEN_HFXOPEAKDETERR_DEFAULT << 10)  /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOPEAKDETRDY                            (0x1UL << 11)                            /**< HFXOPEAKDETRDY Interrupt Enable */
#define _CMU_IEN_HFXOPEAKDETRDY_SHIFT                     11                                       /**< Shift value for CMU_HFXOPEAKDETRDY */
#define _CMU_IEN_HFXOPEAKDETRDY_MASK                      0x800UL                                  /**< Bit mask for CMU_HFXOPEAKDETRDY */
#define _CMU_IEN_HFXOPEAKDETRDY_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOPEAKDETRDY_DEFAULT                    (_CMU_IEN_HFXOPEAKDETRDY_DEFAULT << 11)  /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOSHUNTOPTRDY                           (0x1UL << 12)                            /**< HFXOSHUNTOPTRDY Interrupt Enable */
#define _CMU_IEN_HFXOSHUNTOPTRDY_SHIFT                    12                                       /**< Shift value for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IEN_HFXOSHUNTOPTRDY_MASK                     0x1000UL                                 /**< Bit mask for CMU_HFXOSHUNTOPTRDY */
#define _CMU_IEN_HFXOSHUNTOPTRDY_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFXOSHUNTOPTRDY_DEFAULT                   (_CMU_IEN_HFXOSHUNTOPTRDY_DEFAULT << 12) /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFRCODIS                                  (0x1UL << 13)                            /**< HFRCODIS Interrupt Enable */
#define _CMU_IEN_HFRCODIS_SHIFT                           13                                       /**< Shift value for CMU_HFRCODIS */
#define _CMU_IEN_HFRCODIS_MASK                            0x2000UL                                 /**< Bit mask for CMU_HFRCODIS */
#define _CMU_IEN_HFRCODIS_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_HFRCODIS_DEFAULT                          (_CMU_IEN_HFRCODIS_DEFAULT << 13)        /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFTIMEOUTERR                              (0x1UL << 14)                            /**< LFTIMEOUTERR Interrupt Enable */
#define _CMU_IEN_LFTIMEOUTERR_SHIFT                       14                                       /**< Shift value for CMU_LFTIMEOUTERR */
#define _CMU_IEN_LFTIMEOUTERR_MASK                        0x4000UL                                 /**< Bit mask for CMU_LFTIMEOUTERR */
#define _CMU_IEN_LFTIMEOUTERR_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_LFTIMEOUTERR_DEFAULT                      (_CMU_IEN_LFTIMEOUTERR_DEFAULT << 14)    /**< Shifted mode DEFAULT for CMU_IEN */
#define CMU_IEN_CMUERR                                    (0x1UL << 31)                            /**< CMUERR Interrupt Enable */
#define _CMU_IEN_CMUERR_SHIFT                             31                                       /**< Shift value for CMU_CMUERR */
#define _CMU_IEN_CMUERR_MASK                              0x80000000UL                             /**< Bit mask for CMU_CMUERR */
#define _CMU_IEN_CMUERR_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CMU_IEN */
#define CMU_IEN_CMUERR_DEFAULT                            (_CMU_IEN_CMUERR_DEFAULT << 31)          /**< Shifted mode DEFAULT for CMU_IEN */

/* Bit fields for CMU HFBUSCLKEN0 */
#define _CMU_HFBUSCLKEN0_RESETVALUE                       0x00000000UL                           /**< Default value for CMU_HFBUSCLKEN0 */
#define _CMU_HFBUSCLKEN0_MASK                             0x0000003FUL                           /**< Mask for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_LE                                (0x1UL << 0)                           /**< Low Energy Peripheral Interface Clock Enable */
#define _CMU_HFBUSCLKEN0_LE_SHIFT                         0                                      /**< Shift value for CMU_LE */
#define _CMU_HFBUSCLKEN0_LE_MASK                          0x1UL                                  /**< Bit mask for CMU_LE */
#define _CMU_HFBUSCLKEN0_LE_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_LE_DEFAULT                        (_CMU_HFBUSCLKEN0_LE_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_CRYPTO                            (0x1UL << 1)                           /**< Advanced Encryption Standard Accelerator Clock Enable */
#define _CMU_HFBUSCLKEN0_CRYPTO_SHIFT                     1                                      /**< Shift value for CMU_CRYPTO */
#define _CMU_HFBUSCLKEN0_CRYPTO_MASK                      0x2UL                                  /**< Bit mask for CMU_CRYPTO */
#define _CMU_HFBUSCLKEN0_CRYPTO_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_CRYPTO_DEFAULT                    (_CMU_HFBUSCLKEN0_CRYPTO_DEFAULT << 1) /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_GPIO                              (0x1UL << 2)                           /**< General purpose Input/Output Clock Enable */
#define _CMU_HFBUSCLKEN0_GPIO_SHIFT                       2                                      /**< Shift value for CMU_GPIO */
#define _CMU_HFBUSCLKEN0_GPIO_MASK                        0x4UL                                  /**< Bit mask for CMU_GPIO */
#define _CMU_HFBUSCLKEN0_GPIO_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_GPIO_DEFAULT                      (_CMU_HFBUSCLKEN0_GPIO_DEFAULT << 2)   /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_PRS                               (0x1UL << 3)                           /**< Peripheral Reflex System Clock Enable */
#define _CMU_HFBUSCLKEN0_PRS_SHIFT                        3                                      /**< Shift value for CMU_PRS */
#define _CMU_HFBUSCLKEN0_PRS_MASK                         0x8UL                                  /**< Bit mask for CMU_PRS */
#define _CMU_HFBUSCLKEN0_PRS_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_PRS_DEFAULT                       (_CMU_HFBUSCLKEN0_PRS_DEFAULT << 3)    /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_LDMA                              (0x1UL << 4)                           /**< Linked Direct Memory Access Controller Clock Enable */
#define _CMU_HFBUSCLKEN0_LDMA_SHIFT                       4                                      /**< Shift value for CMU_LDMA */
#define _CMU_HFBUSCLKEN0_LDMA_MASK                        0x10UL                                 /**< Bit mask for CMU_LDMA */
#define _CMU_HFBUSCLKEN0_LDMA_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_LDMA_DEFAULT                      (_CMU_HFBUSCLKEN0_LDMA_DEFAULT << 4)   /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_GPCRC                             (0x1UL << 5)                           /**< General Purpose CRC Clock Enable */
#define _CMU_HFBUSCLKEN0_GPCRC_SHIFT                      5                                      /**< Shift value for CMU_GPCRC */
#define _CMU_HFBUSCLKEN0_GPCRC_MASK                       0x20UL                                 /**< Bit mask for CMU_GPCRC */
#define _CMU_HFBUSCLKEN0_GPCRC_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for CMU_HFBUSCLKEN0 */
#define CMU_HFBUSCLKEN0_GPCRC_DEFAULT                     (_CMU_HFBUSCLKEN0_GPCRC_DEFAULT << 5)  /**< Shifted mode DEFAULT for CMU_HFBUSCLKEN0 */

/* Bit fields for CMU HFPERCLKEN0 */
#define _CMU_HFPERCLKEN0_RESETVALUE                       0x00000000UL                              /**< Default value for CMU_HFPERCLKEN0 */
#define _CMU_HFPERCLKEN0_MASK                             0x000001CFUL                              /**< Mask for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_TIMER0                            (0x1UL << 0)                              /**< Timer 0 Clock Enable */
#define _CMU_HFPERCLKEN0_TIMER0_SHIFT                     0                                         /**< Shift value for CMU_TIMER0 */
#define _CMU_HFPERCLKEN0_TIMER0_MASK                      0x1UL                                     /**< Bit mask for CMU_TIMER0 */
#define _CMU_HFPERCLKEN0_TIMER0_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_TIMER0_DEFAULT                    (_CMU_HFPERCLKEN0_TIMER0_DEFAULT << 0)    /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_TIMER1                            (0x1UL << 1)                              /**< Timer 1 Clock Enable */
#define _CMU_HFPERCLKEN0_TIMER1_SHIFT                     1                                         /**< Shift value for CMU_TIMER1 */
#define _CMU_HFPERCLKEN0_TIMER1_MASK                      0x2UL                                     /**< Bit mask for CMU_TIMER1 */
#define _CMU_HFPERCLKEN0_TIMER1_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_TIMER1_DEFAULT                    (_CMU_HFPERCLKEN0_TIMER1_DEFAULT << 1)    /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_USART0                            (0x1UL << 2)                              /**< Universal Synchronous/Asynchronous Receiver/Transmitter 0 Clock Enable */
#define _CMU_HFPERCLKEN0_USART0_SHIFT                     2                                         /**< Shift value for CMU_USART0 */
#define _CMU_HFPERCLKEN0_USART0_MASK                      0x4UL                                     /**< Bit mask for CMU_USART0 */
#define _CMU_HFPERCLKEN0_USART0_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_USART0_DEFAULT                    (_CMU_HFPERCLKEN0_USART0_DEFAULT << 2)    /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_USART1                            (0x1UL << 3)                              /**< Universal Synchronous/Asynchronous Receiver/Transmitter 1 Clock Enable */
#define _CMU_HFPERCLKEN0_USART1_SHIFT                     3                                         /**< Shift value for CMU_USART1 */
#define _CMU_HFPERCLKEN0_USART1_MASK                      0x8UL                                     /**< Bit mask for CMU_USART1 */
#define _CMU_HFPERCLKEN0_USART1_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_USART1_DEFAULT                    (_CMU_HFPERCLKEN0_USART1_DEFAULT << 3)    /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_CRYOTIMER                         (0x1UL << 6)                              /**< CRYOTIMER Clock Enable */
#define _CMU_HFPERCLKEN0_CRYOTIMER_SHIFT                  6                                         /**< Shift value for CMU_CRYOTIMER */
#define _CMU_HFPERCLKEN0_CRYOTIMER_MASK                   0x40UL                                    /**< Bit mask for CMU_CRYOTIMER */
#define _CMU_HFPERCLKEN0_CRYOTIMER_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_CRYOTIMER_DEFAULT                 (_CMU_HFPERCLKEN0_CRYOTIMER_DEFAULT << 6) /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_I2C0                              (0x1UL << 7)                              /**< I2C 0 Clock Enable */
#define _CMU_HFPERCLKEN0_I2C0_SHIFT                       7                                         /**< Shift value for CMU_I2C0 */
#define _CMU_HFPERCLKEN0_I2C0_MASK                        0x80UL                                    /**< Bit mask for CMU_I2C0 */
#define _CMU_HFPERCLKEN0_I2C0_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_I2C0_DEFAULT                      (_CMU_HFPERCLKEN0_I2C0_DEFAULT << 7)      /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_ADC0                              (0x1UL << 8)                              /**< Analog to Digital Converter 0 Clock Enable */
#define _CMU_HFPERCLKEN0_ADC0_SHIFT                       8                                         /**< Shift value for CMU_ADC0 */
#define _CMU_HFPERCLKEN0_ADC0_MASK                        0x100UL                                   /**< Bit mask for CMU_ADC0 */
#define _CMU_HFPERCLKEN0_ADC0_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for CMU_HFPERCLKEN0 */
#define CMU_HFPERCLKEN0_ADC0_DEFAULT                      (_CMU_HFPERCLKEN0_ADC0_DEFAULT << 8)      /**< Shifted mode DEFAULT for CMU_HFPERCLKEN0 */

/* Bit fields for CMU LFACLKEN0 */
#define _CMU_LFACLKEN0_RESETVALUE                         0x00000000UL                           /**< Default value for CMU_LFACLKEN0 */
#define _CMU_LFACLKEN0_MASK                               0x00000001UL                           /**< Mask for CMU_LFACLKEN0 */
#define CMU_LFACLKEN0_LETIMER0                            (0x1UL << 0)                           /**< Low Energy Timer 0 Clock Enable */
#define _CMU_LFACLKEN0_LETIMER0_SHIFT                     0                                      /**< Shift value for CMU_LETIMER0 */
#define _CMU_LFACLKEN0_LETIMER0_MASK                      0x1UL                                  /**< Bit mask for CMU_LETIMER0 */
#define _CMU_LFACLKEN0_LETIMER0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for CMU_LFACLKEN0 */
#define CMU_LFACLKEN0_LETIMER0_DEFAULT                    (_CMU_LFACLKEN0_LETIMER0_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_LFACLKEN0 */

/* Bit fields for CMU LFBCLKEN0 */
#define _CMU_LFBCLKEN0_RESETVALUE                         0x00000000UL                          /**< Default value for CMU_LFBCLKEN0 */
#define _CMU_LFBCLKEN0_MASK                               0x00000001UL                          /**< Mask for CMU_LFBCLKEN0 */
#define CMU_LFBCLKEN0_LEUART0                             (0x1UL << 0)                          /**< Low Energy UART 0 Clock Enable */
#define _CMU_LFBCLKEN0_LEUART0_SHIFT                      0                                     /**< Shift value for CMU_LEUART0 */
#define _CMU_LFBCLKEN0_LEUART0_MASK                       0x1UL                                 /**< Bit mask for CMU_LEUART0 */
#define _CMU_LFBCLKEN0_LEUART0_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for CMU_LFBCLKEN0 */
#define CMU_LFBCLKEN0_LEUART0_DEFAULT                     (_CMU_LFBCLKEN0_LEUART0_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_LFBCLKEN0 */

/* Bit fields for CMU LFECLKEN0 */
#define _CMU_LFECLKEN0_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_LFECLKEN0 */
#define _CMU_LFECLKEN0_MASK                               0x00000001UL                       /**< Mask for CMU_LFECLKEN0 */
#define CMU_LFECLKEN0_RTCC                                (0x1UL << 0)                       /**< Real-Time Counter and Calendar Clock Enable */
#define _CMU_LFECLKEN0_RTCC_SHIFT                         0                                  /**< Shift value for CMU_RTCC */
#define _CMU_LFECLKEN0_RTCC_MASK                          0x1UL                              /**< Bit mask for CMU_RTCC */
#define _CMU_LFECLKEN0_RTCC_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for CMU_LFECLKEN0 */
#define CMU_LFECLKEN0_RTCC_DEFAULT                        (_CMU_LFECLKEN0_RTCC_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_LFECLKEN0 */

/* Bit fields for CMU HFPRESC */
#define _CMU_HFPRESC_RESETVALUE                           0x00000000UL                              /**< Default value for CMU_HFPRESC */
#define _CMU_HFPRESC_MASK                                 0x01001F00UL                              /**< Mask for CMU_HFPRESC */
#define _CMU_HFPRESC_PRESC_SHIFT                          8                                         /**< Shift value for CMU_PRESC */
#define _CMU_HFPRESC_PRESC_MASK                           0x1F00UL                                  /**< Bit mask for CMU_PRESC */
#define _CMU_HFPRESC_PRESC_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for CMU_HFPRESC */
#define _CMU_HFPRESC_PRESC_NODIVISION                     0x00000000UL                              /**< Mode NODIVISION for CMU_HFPRESC */
#define CMU_HFPRESC_PRESC_DEFAULT                         (_CMU_HFPRESC_PRESC_DEFAULT << 8)         /**< Shifted mode DEFAULT for CMU_HFPRESC */
#define CMU_HFPRESC_PRESC_NODIVISION                      (_CMU_HFPRESC_PRESC_NODIVISION << 8)      /**< Shifted mode NODIVISION for CMU_HFPRESC */
#define _CMU_HFPRESC_HFCLKLEPRESC_SHIFT                   24                                        /**< Shift value for CMU_HFCLKLEPRESC */
#define _CMU_HFPRESC_HFCLKLEPRESC_MASK                    0x1000000UL                               /**< Bit mask for CMU_HFCLKLEPRESC */
#define _CMU_HFPRESC_HFCLKLEPRESC_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for CMU_HFPRESC */
#define _CMU_HFPRESC_HFCLKLEPRESC_DIV2                    0x00000000UL                              /**< Mode DIV2 for CMU_HFPRESC */
#define _CMU_HFPRESC_HFCLKLEPRESC_DIV4                    0x00000001UL                              /**< Mode DIV4 for CMU_HFPRESC */
#define CMU_HFPRESC_HFCLKLEPRESC_DEFAULT                  (_CMU_HFPRESC_HFCLKLEPRESC_DEFAULT << 24) /**< Shifted mode DEFAULT for CMU_HFPRESC */
#define CMU_HFPRESC_HFCLKLEPRESC_DIV2                     (_CMU_HFPRESC_HFCLKLEPRESC_DIV2 << 24)    /**< Shifted mode DIV2 for CMU_HFPRESC */
#define CMU_HFPRESC_HFCLKLEPRESC_DIV4                     (_CMU_HFPRESC_HFCLKLEPRESC_DIV4 << 24)    /**< Shifted mode DIV4 for CMU_HFPRESC */

/* Bit fields for CMU HFCOREPRESC */
#define _CMU_HFCOREPRESC_RESETVALUE                       0x00000000UL                             /**< Default value for CMU_HFCOREPRESC */
#define _CMU_HFCOREPRESC_MASK                             0x0001FF00UL                             /**< Mask for CMU_HFCOREPRESC */
#define _CMU_HFCOREPRESC_PRESC_SHIFT                      8                                        /**< Shift value for CMU_PRESC */
#define _CMU_HFCOREPRESC_PRESC_MASK                       0x1FF00UL                                /**< Bit mask for CMU_PRESC */
#define _CMU_HFCOREPRESC_PRESC_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for CMU_HFCOREPRESC */
#define _CMU_HFCOREPRESC_PRESC_NODIVISION                 0x00000000UL                             /**< Mode NODIVISION for CMU_HFCOREPRESC */
#define CMU_HFCOREPRESC_PRESC_DEFAULT                     (_CMU_HFCOREPRESC_PRESC_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_HFCOREPRESC */
#define CMU_HFCOREPRESC_PRESC_NODIVISION                  (_CMU_HFCOREPRESC_PRESC_NODIVISION << 8) /**< Shifted mode NODIVISION for CMU_HFCOREPRESC */

/* Bit fields for CMU HFPERPRESC */
#define _CMU_HFPERPRESC_RESETVALUE                        0x00000000UL                            /**< Default value for CMU_HFPERPRESC */
#define _CMU_HFPERPRESC_MASK                              0x0001FF00UL                            /**< Mask for CMU_HFPERPRESC */
#define _CMU_HFPERPRESC_PRESC_SHIFT                       8                                       /**< Shift value for CMU_PRESC */
#define _CMU_HFPERPRESC_PRESC_MASK                        0x1FF00UL                               /**< Bit mask for CMU_PRESC */
#define _CMU_HFPERPRESC_PRESC_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for CMU_HFPERPRESC */
#define _CMU_HFPERPRESC_PRESC_NODIVISION                  0x00000000UL                            /**< Mode NODIVISION for CMU_HFPERPRESC */
#define CMU_HFPERPRESC_PRESC_DEFAULT                      (_CMU_HFPERPRESC_PRESC_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_HFPERPRESC */
#define CMU_HFPERPRESC_PRESC_NODIVISION                   (_CMU_HFPERPRESC_PRESC_NODIVISION << 8) /**< Shifted mode NODIVISION for CMU_HFPERPRESC */

/* Bit fields for CMU HFEXPPRESC */
#define _CMU_HFEXPPRESC_RESETVALUE                        0x00000000UL                            /**< Default value for CMU_HFEXPPRESC */
#define _CMU_HFEXPPRESC_MASK                              0x00001F00UL                            /**< Mask for CMU_HFEXPPRESC */
#define _CMU_HFEXPPRESC_PRESC_SHIFT                       8                                       /**< Shift value for CMU_PRESC */
#define _CMU_HFEXPPRESC_PRESC_MASK                        0x1F00UL                                /**< Bit mask for CMU_PRESC */
#define _CMU_HFEXPPRESC_PRESC_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for CMU_HFEXPPRESC */
#define _CMU_HFEXPPRESC_PRESC_NODIVISION                  0x00000000UL                            /**< Mode NODIVISION for CMU_HFEXPPRESC */
#define CMU_HFEXPPRESC_PRESC_DEFAULT                      (_CMU_HFEXPPRESC_PRESC_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_HFEXPPRESC */
#define CMU_HFEXPPRESC_PRESC_NODIVISION                   (_CMU_HFEXPPRESC_PRESC_NODIVISION << 8) /**< Shifted mode NODIVISION for CMU_HFEXPPRESC */

/* Bit fields for CMU LFAPRESC0 */
#define _CMU_LFAPRESC0_RESETVALUE                         0x00000000UL                            /**< Default value for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_MASK                               0x0000000FUL                            /**< Mask for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_SHIFT                     0                                       /**< Shift value for CMU_LETIMER0 */
#define _CMU_LFAPRESC0_LETIMER0_MASK                      0xFUL                                   /**< Bit mask for CMU_LETIMER0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV1                      0x00000000UL                            /**< Mode DIV1 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV2                      0x00000001UL                            /**< Mode DIV2 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV4                      0x00000002UL                            /**< Mode DIV4 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV8                      0x00000003UL                            /**< Mode DIV8 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV16                     0x00000004UL                            /**< Mode DIV16 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV32                     0x00000005UL                            /**< Mode DIV32 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV64                     0x00000006UL                            /**< Mode DIV64 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV128                    0x00000007UL                            /**< Mode DIV128 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV256                    0x00000008UL                            /**< Mode DIV256 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV512                    0x00000009UL                            /**< Mode DIV512 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV1024                   0x0000000AUL                            /**< Mode DIV1024 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV2048                   0x0000000BUL                            /**< Mode DIV2048 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV4096                   0x0000000CUL                            /**< Mode DIV4096 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV8192                   0x0000000DUL                            /**< Mode DIV8192 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV16384                  0x0000000EUL                            /**< Mode DIV16384 for CMU_LFAPRESC0 */
#define _CMU_LFAPRESC0_LETIMER0_DIV32768                  0x0000000FUL                            /**< Mode DIV32768 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV1                       (_CMU_LFAPRESC0_LETIMER0_DIV1 << 0)     /**< Shifted mode DIV1 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV2                       (_CMU_LFAPRESC0_LETIMER0_DIV2 << 0)     /**< Shifted mode DIV2 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV4                       (_CMU_LFAPRESC0_LETIMER0_DIV4 << 0)     /**< Shifted mode DIV4 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV8                       (_CMU_LFAPRESC0_LETIMER0_DIV8 << 0)     /**< Shifted mode DIV8 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV16                      (_CMU_LFAPRESC0_LETIMER0_DIV16 << 0)    /**< Shifted mode DIV16 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV32                      (_CMU_LFAPRESC0_LETIMER0_DIV32 << 0)    /**< Shifted mode DIV32 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV64                      (_CMU_LFAPRESC0_LETIMER0_DIV64 << 0)    /**< Shifted mode DIV64 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV128                     (_CMU_LFAPRESC0_LETIMER0_DIV128 << 0)   /**< Shifted mode DIV128 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV256                     (_CMU_LFAPRESC0_LETIMER0_DIV256 << 0)   /**< Shifted mode DIV256 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV512                     (_CMU_LFAPRESC0_LETIMER0_DIV512 << 0)   /**< Shifted mode DIV512 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV1024                    (_CMU_LFAPRESC0_LETIMER0_DIV1024 << 0)  /**< Shifted mode DIV1024 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV2048                    (_CMU_LFAPRESC0_LETIMER0_DIV2048 << 0)  /**< Shifted mode DIV2048 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV4096                    (_CMU_LFAPRESC0_LETIMER0_DIV4096 << 0)  /**< Shifted mode DIV4096 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV8192                    (_CMU_LFAPRESC0_LETIMER0_DIV8192 << 0)  /**< Shifted mode DIV8192 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV16384                   (_CMU_LFAPRESC0_LETIMER0_DIV16384 << 0) /**< Shifted mode DIV16384 for CMU_LFAPRESC0 */
#define CMU_LFAPRESC0_LETIMER0_DIV32768                   (_CMU_LFAPRESC0_LETIMER0_DIV32768 << 0) /**< Shifted mode DIV32768 for CMU_LFAPRESC0 */

/* Bit fields for CMU LFBPRESC0 */
#define _CMU_LFBPRESC0_RESETVALUE                         0x00000000UL                       /**< Default value for CMU_LFBPRESC0 */
#define _CMU_LFBPRESC0_MASK                               0x00000003UL                       /**< Mask for CMU_LFBPRESC0 */
#define _CMU_LFBPRESC0_LEUART0_SHIFT                      0                                  /**< Shift value for CMU_LEUART0 */
#define _CMU_LFBPRESC0_LEUART0_MASK                       0x3UL                              /**< Bit mask for CMU_LEUART0 */
#define _CMU_LFBPRESC0_LEUART0_DIV1                       0x00000000UL                       /**< Mode DIV1 for CMU_LFBPRESC0 */
#define _CMU_LFBPRESC0_LEUART0_DIV2                       0x00000001UL                       /**< Mode DIV2 for CMU_LFBPRESC0 */
#define _CMU_LFBPRESC0_LEUART0_DIV4                       0x00000002UL                       /**< Mode DIV4 for CMU_LFBPRESC0 */
#define _CMU_LFBPRESC0_LEUART0_DIV8                       0x00000003UL                       /**< Mode DIV8 for CMU_LFBPRESC0 */
#define CMU_LFBPRESC0_LEUART0_DIV1                        (_CMU_LFBPRESC0_LEUART0_DIV1 << 0) /**< Shifted mode DIV1 for CMU_LFBPRESC0 */
#define CMU_LFBPRESC0_LEUART0_DIV2                        (_CMU_LFBPRESC0_LEUART0_DIV2 << 0) /**< Shifted mode DIV2 for CMU_LFBPRESC0 */
#define CMU_LFBPRESC0_LEUART0_DIV4                        (_CMU_LFBPRESC0_LEUART0_DIV4 << 0) /**< Shifted mode DIV4 for CMU_LFBPRESC0 */
#define CMU_LFBPRESC0_LEUART0_DIV8                        (_CMU_LFBPRESC0_LEUART0_DIV8 << 0) /**< Shifted mode DIV8 for CMU_LFBPRESC0 */

/* Bit fields for CMU LFEPRESC0 */
#define _CMU_LFEPRESC0_RESETVALUE                         0x00000000UL                    /**< Default value for CMU_LFEPRESC0 */
#define _CMU_LFEPRESC0_MASK                               0x0000000FUL                    /**< Mask for CMU_LFEPRESC0 */
#define _CMU_LFEPRESC0_RTCC_SHIFT                         0                               /**< Shift value for CMU_RTCC */
#define _CMU_LFEPRESC0_RTCC_MASK                          0xFUL                           /**< Bit mask for CMU_RTCC */
#define _CMU_LFEPRESC0_RTCC_DIV1                          0x00000000UL                    /**< Mode DIV1 for CMU_LFEPRESC0 */
#define CMU_LFEPRESC0_RTCC_DIV1                           (_CMU_LFEPRESC0_RTCC_DIV1 << 0) /**< Shifted mode DIV1 for CMU_LFEPRESC0 */

/* Bit fields for CMU SYNCBUSY */
#define _CMU_SYNCBUSY_RESETVALUE                          0x00000000UL                               /**< Default value for CMU_SYNCBUSY */
#define _CMU_SYNCBUSY_MASK                                0x3F050055UL                               /**< Mask for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFACLKEN0                            (0x1UL << 0)                               /**< Low Frequency a Clock Enable 0 Busy */
#define _CMU_SYNCBUSY_LFACLKEN0_SHIFT                     0                                          /**< Shift value for CMU_LFACLKEN0 */
#define _CMU_SYNCBUSY_LFACLKEN0_MASK                      0x1UL                                      /**< Bit mask for CMU_LFACLKEN0 */
#define _CMU_SYNCBUSY_LFACLKEN0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFACLKEN0_DEFAULT                    (_CMU_SYNCBUSY_LFACLKEN0_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFAPRESC0                            (0x1UL << 2)                               /**< Low Frequency a Prescaler 0 Busy */
#define _CMU_SYNCBUSY_LFAPRESC0_SHIFT                     2                                          /**< Shift value for CMU_LFAPRESC0 */
#define _CMU_SYNCBUSY_LFAPRESC0_MASK                      0x4UL                                      /**< Bit mask for CMU_LFAPRESC0 */
#define _CMU_SYNCBUSY_LFAPRESC0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFAPRESC0_DEFAULT                    (_CMU_SYNCBUSY_LFAPRESC0_DEFAULT << 2)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFBCLKEN0                            (0x1UL << 4)                               /**< Low Frequency B Clock Enable 0 Busy */
#define _CMU_SYNCBUSY_LFBCLKEN0_SHIFT                     4                                          /**< Shift value for CMU_LFBCLKEN0 */
#define _CMU_SYNCBUSY_LFBCLKEN0_MASK                      0x10UL                                     /**< Bit mask for CMU_LFBCLKEN0 */
#define _CMU_SYNCBUSY_LFBCLKEN0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFBCLKEN0_DEFAULT                    (_CMU_SYNCBUSY_LFBCLKEN0_DEFAULT << 4)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFBPRESC0                            (0x1UL << 6)                               /**< Low Frequency B Prescaler 0 Busy */
#define _CMU_SYNCBUSY_LFBPRESC0_SHIFT                     6                                          /**< Shift value for CMU_LFBPRESC0 */
#define _CMU_SYNCBUSY_LFBPRESC0_MASK                      0x40UL                                     /**< Bit mask for CMU_LFBPRESC0 */
#define _CMU_SYNCBUSY_LFBPRESC0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFBPRESC0_DEFAULT                    (_CMU_SYNCBUSY_LFBPRESC0_DEFAULT << 6)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFECLKEN0                            (0x1UL << 16)                              /**< Low Frequency E Clock Enable 0 Busy */
#define _CMU_SYNCBUSY_LFECLKEN0_SHIFT                     16                                         /**< Shift value for CMU_LFECLKEN0 */
#define _CMU_SYNCBUSY_LFECLKEN0_MASK                      0x10000UL                                  /**< Bit mask for CMU_LFECLKEN0 */
#define _CMU_SYNCBUSY_LFECLKEN0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFECLKEN0_DEFAULT                    (_CMU_SYNCBUSY_LFECLKEN0_DEFAULT << 16)    /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFEPRESC0                            (0x1UL << 18)                              /**< Low Frequency E Prescaler 0 Busy */
#define _CMU_SYNCBUSY_LFEPRESC0_SHIFT                     18                                         /**< Shift value for CMU_LFEPRESC0 */
#define _CMU_SYNCBUSY_LFEPRESC0_MASK                      0x40000UL                                  /**< Bit mask for CMU_LFEPRESC0 */
#define _CMU_SYNCBUSY_LFEPRESC0_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFEPRESC0_DEFAULT                    (_CMU_SYNCBUSY_LFEPRESC0_DEFAULT << 18)    /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_HFRCOBSY                             (0x1UL << 24)                              /**< HFRCO Busy */
#define _CMU_SYNCBUSY_HFRCOBSY_SHIFT                      24                                         /**< Shift value for CMU_HFRCOBSY */
#define _CMU_SYNCBUSY_HFRCOBSY_MASK                       0x1000000UL                                /**< Bit mask for CMU_HFRCOBSY */
#define _CMU_SYNCBUSY_HFRCOBSY_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_HFRCOBSY_DEFAULT                     (_CMU_SYNCBUSY_HFRCOBSY_DEFAULT << 24)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_AUXHFRCOBSY                          (0x1UL << 25)                              /**< AUXHFRCO Busy */
#define _CMU_SYNCBUSY_AUXHFRCOBSY_SHIFT                   25                                         /**< Shift value for CMU_AUXHFRCOBSY */
#define _CMU_SYNCBUSY_AUXHFRCOBSY_MASK                    0x2000000UL                                /**< Bit mask for CMU_AUXHFRCOBSY */
#define _CMU_SYNCBUSY_AUXHFRCOBSY_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_AUXHFRCOBSY_DEFAULT                  (_CMU_SYNCBUSY_AUXHFRCOBSY_DEFAULT << 25)  /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFRCOBSY                             (0x1UL << 26)                              /**< LFRCO Busy */
#define _CMU_SYNCBUSY_LFRCOBSY_SHIFT                      26                                         /**< Shift value for CMU_LFRCOBSY */
#define _CMU_SYNCBUSY_LFRCOBSY_MASK                       0x4000000UL                                /**< Bit mask for CMU_LFRCOBSY */
#define _CMU_SYNCBUSY_LFRCOBSY_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFRCOBSY_DEFAULT                     (_CMU_SYNCBUSY_LFRCOBSY_DEFAULT << 26)     /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFRCOVREFBSY                         (0x1UL << 27)                              /**< LFRCO VREF Busy */
#define _CMU_SYNCBUSY_LFRCOVREFBSY_SHIFT                  27                                         /**< Shift value for CMU_LFRCOVREFBSY */
#define _CMU_SYNCBUSY_LFRCOVREFBSY_MASK                   0x8000000UL                                /**< Bit mask for CMU_LFRCOVREFBSY */
#define _CMU_SYNCBUSY_LFRCOVREFBSY_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFRCOVREFBSY_DEFAULT                 (_CMU_SYNCBUSY_LFRCOVREFBSY_DEFAULT << 27) /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_HFXOBSY                              (0x1UL << 28)                              /**< HFXO Busy */
#define _CMU_SYNCBUSY_HFXOBSY_SHIFT                       28                                         /**< Shift value for CMU_HFXOBSY */
#define _CMU_SYNCBUSY_HFXOBSY_MASK                        0x10000000UL                               /**< Bit mask for CMU_HFXOBSY */
#define _CMU_SYNCBUSY_HFXOBSY_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_HFXOBSY_DEFAULT                      (_CMU_SYNCBUSY_HFXOBSY_DEFAULT << 28)      /**< Shifted mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFXOBSY                              (0x1UL << 29)                              /**< LFXO Busy */
#define _CMU_SYNCBUSY_LFXOBSY_SHIFT                       29                                         /**< Shift value for CMU_LFXOBSY */
#define _CMU_SYNCBUSY_LFXOBSY_MASK                        0x20000000UL                               /**< Bit mask for CMU_LFXOBSY */
#define _CMU_SYNCBUSY_LFXOBSY_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for CMU_SYNCBUSY */
#define CMU_SYNCBUSY_LFXOBSY_DEFAULT                      (_CMU_SYNCBUSY_LFXOBSY_DEFAULT << 29)      /**< Shifted mode DEFAULT for CMU_SYNCBUSY */

/* Bit fields for CMU FREEZE */
#define _CMU_FREEZE_RESETVALUE                            0x00000000UL                         /**< Default value for CMU_FREEZE */
#define _CMU_FREEZE_MASK                                  0x00000001UL                         /**< Mask for CMU_FREEZE */
#define CMU_FREEZE_REGFREEZE                              (0x1UL << 0)                         /**< Register Update Freeze */
#define _CMU_FREEZE_REGFREEZE_SHIFT                       0                                    /**< Shift value for CMU_REGFREEZE */
#define _CMU_FREEZE_REGFREEZE_MASK                        0x1UL                                /**< Bit mask for CMU_REGFREEZE */
#define _CMU_FREEZE_REGFREEZE_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for CMU_FREEZE */
#define _CMU_FREEZE_REGFREEZE_UPDATE                      0x00000000UL                         /**< Mode UPDATE for CMU_FREEZE */
#define _CMU_FREEZE_REGFREEZE_FREEZE                      0x00000001UL                         /**< Mode FREEZE for CMU_FREEZE */
#define CMU_FREEZE_REGFREEZE_DEFAULT                      (_CMU_FREEZE_REGFREEZE_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_FREEZE */
#define CMU_FREEZE_REGFREEZE_UPDATE                       (_CMU_FREEZE_REGFREEZE_UPDATE << 0)  /**< Shifted mode UPDATE for CMU_FREEZE */
#define CMU_FREEZE_REGFREEZE_FREEZE                       (_CMU_FREEZE_REGFREEZE_FREEZE << 0)  /**< Shifted mode FREEZE for CMU_FREEZE */

/* Bit fields for CMU PCNTCTRL */
#define _CMU_PCNTCTRL_RESETVALUE                          0x00000000UL /**< Default value for CMU_PCNTCTRL */
#define _CMU_PCNTCTRL_MASK                                0x00000000UL /**< Mask for CMU_PCNTCTRL */

/* Bit fields for CMU ADCCTRL */
#define _CMU_ADCCTRL_RESETVALUE                           0x00000000UL                            /**< Default value for CMU_ADCCTRL */
#define _CMU_ADCCTRL_MASK                                 0x00000130UL                            /**< Mask for CMU_ADCCTRL */
#define _CMU_ADCCTRL_ADC0CLKSEL_SHIFT                     4                                       /**< Shift value for CMU_ADC0CLKSEL */
#define _CMU_ADCCTRL_ADC0CLKSEL_MASK                      0x30UL                                  /**< Bit mask for CMU_ADC0CLKSEL */
#define _CMU_ADCCTRL_ADC0CLKSEL_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for CMU_ADCCTRL */
#define _CMU_ADCCTRL_ADC0CLKSEL_DISABLED                  0x00000000UL                            /**< Mode DISABLED for CMU_ADCCTRL */
#define _CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO                  0x00000001UL                            /**< Mode AUXHFRCO for CMU_ADCCTRL */
#define _CMU_ADCCTRL_ADC0CLKSEL_HFXO                      0x00000002UL                            /**< Mode HFXO for CMU_ADCCTRL */
#define _CMU_ADCCTRL_ADC0CLKSEL_HFSRCCLK                  0x00000003UL                            /**< Mode HFSRCCLK for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKSEL_DEFAULT                    (_CMU_ADCCTRL_ADC0CLKSEL_DEFAULT << 4)  /**< Shifted mode DEFAULT for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKSEL_DISABLED                   (_CMU_ADCCTRL_ADC0CLKSEL_DISABLED << 4) /**< Shifted mode DISABLED for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO                   (_CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO << 4) /**< Shifted mode AUXHFRCO for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKSEL_HFXO                       (_CMU_ADCCTRL_ADC0CLKSEL_HFXO << 4)     /**< Shifted mode HFXO for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKSEL_HFSRCCLK                   (_CMU_ADCCTRL_ADC0CLKSEL_HFSRCCLK << 4) /**< Shifted mode HFSRCCLK for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKINV                            (0x1UL << 8)                            /**< Invert Clock Selected By ADC0CLKSEL */
#define _CMU_ADCCTRL_ADC0CLKINV_SHIFT                     8                                       /**< Shift value for CMU_ADC0CLKINV */
#define _CMU_ADCCTRL_ADC0CLKINV_MASK                      0x100UL                                 /**< Bit mask for CMU_ADC0CLKINV */
#define _CMU_ADCCTRL_ADC0CLKINV_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for CMU_ADCCTRL */
#define CMU_ADCCTRL_ADC0CLKINV_DEFAULT                    (_CMU_ADCCTRL_ADC0CLKINV_DEFAULT << 8)  /**< Shifted mode DEFAULT for CMU_ADCCTRL */

/* Bit fields for CMU ROUTEPEN */
#define _CMU_ROUTEPEN_RESETVALUE                          0x00000000UL                            /**< Default value for CMU_ROUTEPEN */
#define _CMU_ROUTEPEN_MASK                                0x00000003UL                            /**< Mask for CMU_ROUTEPEN */
#define CMU_ROUTEPEN_CLKOUT0PEN                           (0x1UL << 0)                            /**< CLKOUT0 Pin Enable */
#define _CMU_ROUTEPEN_CLKOUT0PEN_SHIFT                    0                                       /**< Shift value for CMU_CLKOUT0PEN */
#define _CMU_ROUTEPEN_CLKOUT0PEN_MASK                     0x1UL                                   /**< Bit mask for CMU_CLKOUT0PEN */
#define _CMU_ROUTEPEN_CLKOUT0PEN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for CMU_ROUTEPEN */
#define CMU_ROUTEPEN_CLKOUT0PEN_DEFAULT                   (_CMU_ROUTEPEN_CLKOUT0PEN_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_ROUTEPEN */
#define CMU_ROUTEPEN_CLKOUT1PEN                           (0x1UL << 1)                            /**< CLKOUT1 Pin Enable */
#define _CMU_ROUTEPEN_CLKOUT1PEN_SHIFT                    1                                       /**< Shift value for CMU_CLKOUT1PEN */
#define _CMU_ROUTEPEN_CLKOUT1PEN_MASK                     0x2UL                                   /**< Bit mask for CMU_CLKOUT1PEN */
#define _CMU_ROUTEPEN_CLKOUT1PEN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for CMU_ROUTEPEN */
#define CMU_ROUTEPEN_CLKOUT1PEN_DEFAULT                   (_CMU_ROUTEPEN_CLKOUT1PEN_DEFAULT << 1) /**< Shifted mode DEFAULT for CMU_ROUTEPEN */

/* Bit fields for CMU ROUTELOC0 */
#define _CMU_ROUTELOC0_RESETVALUE                         0x00000000UL                             /**< Default value for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_MASK                               0x00000707UL                             /**< Mask for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_SHIFT                   0                                        /**< Shift value for CMU_CLKOUT0LOC */
#define _CMU_ROUTELOC0_CLKOUT0LOC_MASK                    0x7UL                                    /**< Bit mask for CMU_CLKOUT0LOC */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC0                    0x00000000UL                             /**< Mode LOC0 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC1                    0x00000001UL                             /**< Mode LOC1 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC2                    0x00000002UL                             /**< Mode LOC2 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC3                    0x00000003UL                             /**< Mode LOC3 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC4                    0x00000004UL                             /**< Mode LOC4 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC5                    0x00000005UL                             /**< Mode LOC5 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC6                    0x00000006UL                             /**< Mode LOC6 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT0LOC_LOC7                    0x00000007UL                             /**< Mode LOC7 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC0                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC0 << 0)    /**< Shifted mode LOC0 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_DEFAULT                  (_CMU_ROUTELOC0_CLKOUT0LOC_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC1                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC1 << 0)    /**< Shifted mode LOC1 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC2                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC2 << 0)    /**< Shifted mode LOC2 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC3                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC3 << 0)    /**< Shifted mode LOC3 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC4                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC4 << 0)    /**< Shifted mode LOC4 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC5                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC5 << 0)    /**< Shifted mode LOC5 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC6                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC6 << 0)    /**< Shifted mode LOC6 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT0LOC_LOC7                     (_CMU_ROUTELOC0_CLKOUT0LOC_LOC7 << 0)    /**< Shifted mode LOC7 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_SHIFT                   8                                        /**< Shift value for CMU_CLKOUT1LOC */
#define _CMU_ROUTELOC0_CLKOUT1LOC_MASK                    0x700UL                                  /**< Bit mask for CMU_CLKOUT1LOC */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC0                    0x00000000UL                             /**< Mode LOC0 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC1                    0x00000001UL                             /**< Mode LOC1 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC2                    0x00000002UL                             /**< Mode LOC2 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC3                    0x00000003UL                             /**< Mode LOC3 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC4                    0x00000004UL                             /**< Mode LOC4 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC5                    0x00000005UL                             /**< Mode LOC5 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC6                    0x00000006UL                             /**< Mode LOC6 for CMU_ROUTELOC0 */
#define _CMU_ROUTELOC0_CLKOUT1LOC_LOC7                    0x00000007UL                             /**< Mode LOC7 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC0                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC0 << 8)    /**< Shifted mode LOC0 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_DEFAULT                  (_CMU_ROUTELOC0_CLKOUT1LOC_DEFAULT << 8) /**< Shifted mode DEFAULT for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC1                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC1 << 8)    /**< Shifted mode LOC1 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC2                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC2 << 8)    /**< Shifted mode LOC2 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC3                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC3 << 8)    /**< Shifted mode LOC3 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC4                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC4 << 8)    /**< Shifted mode LOC4 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC5                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC5 << 8)    /**< Shifted mode LOC5 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC6                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC6 << 8)    /**< Shifted mode LOC6 for CMU_ROUTELOC0 */
#define CMU_ROUTELOC0_CLKOUT1LOC_LOC7                     (_CMU_ROUTELOC0_CLKOUT1LOC_LOC7 << 8)    /**< Shifted mode LOC7 for CMU_ROUTELOC0 */

/* Bit fields for CMU LOCK */
#define _CMU_LOCK_RESETVALUE                              0x00000000UL                      /**< Default value for CMU_LOCK */
#define _CMU_LOCK_MASK                                    0x0000FFFFUL                      /**< Mask for CMU_LOCK */
#define _CMU_LOCK_LOCKKEY_SHIFT                           0                                 /**< Shift value for CMU_LOCKKEY */
#define _CMU_LOCK_LOCKKEY_MASK                            0xFFFFUL                          /**< Bit mask for CMU_LOCKKEY */
#define _CMU_LOCK_LOCKKEY_DEFAULT                         0x00000000UL                      /**< Mode DEFAULT for CMU_LOCK */
#define _CMU_LOCK_LOCKKEY_UNLOCKED                        0x00000000UL                      /**< Mode UNLOCKED for CMU_LOCK */
#define _CMU_LOCK_LOCKKEY_LOCK                            0x00000000UL                      /**< Mode LOCK for CMU_LOCK */
#define _CMU_LOCK_LOCKKEY_LOCKED                          0x00000001UL                      /**< Mode LOCKED for CMU_LOCK */
#define _CMU_LOCK_LOCKKEY_UNLOCK                          0x0000580EUL                      /**< Mode UNLOCK for CMU_LOCK */
#define CMU_LOCK_LOCKKEY_DEFAULT                          (_CMU_LOCK_LOCKKEY_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_LOCK */
#define CMU_LOCK_LOCKKEY_UNLOCKED                         (_CMU_LOCK_LOCKKEY_UNLOCKED << 0) /**< Shifted mode UNLOCKED for CMU_LOCK */
#define CMU_LOCK_LOCKKEY_LOCK                             (_CMU_LOCK_LOCKKEY_LOCK << 0)     /**< Shifted mode LOCK for CMU_LOCK */
#define CMU_LOCK_LOCKKEY_LOCKED                           (_CMU_LOCK_LOCKKEY_LOCKED << 0)   /**< Shifted mode LOCKED for CMU_LOCK */
#define CMU_LOCK_LOCKKEY_UNLOCK                           (_CMU_LOCK_LOCKKEY_UNLOCK << 0)   /**< Shifted mode UNLOCK for CMU_LOCK */

/** @} */
/** @} End of group EFR32FG1V232F256GM48_CMU */

/***************************************************************************//**
 * @addtogroup EFR32FG1V232F256GM48_PRS
 * @{
 * @defgroup EFR32FG1V232F256GM48_PRS_BitFields  PRS Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for PRS SWPULSE */
#define _PRS_SWPULSE_RESETVALUE                0x00000000UL                           /**< Default value for PRS_SWPULSE */
#define _PRS_SWPULSE_MASK                      0x00000FFFUL                           /**< Mask for PRS_SWPULSE */
#define PRS_SWPULSE_CH0PULSE                   (0x1UL << 0)                           /**< Channel 0 Pulse Generation */
#define _PRS_SWPULSE_CH0PULSE_SHIFT            0                                      /**< Shift value for PRS_CH0PULSE */
#define _PRS_SWPULSE_CH0PULSE_MASK             0x1UL                                  /**< Bit mask for PRS_CH0PULSE */
#define _PRS_SWPULSE_CH0PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH0PULSE_DEFAULT           (_PRS_SWPULSE_CH0PULSE_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH1PULSE                   (0x1UL << 1)                           /**< Channel 1 Pulse Generation */
#define _PRS_SWPULSE_CH1PULSE_SHIFT            1                                      /**< Shift value for PRS_CH1PULSE */
#define _PRS_SWPULSE_CH1PULSE_MASK             0x2UL                                  /**< Bit mask for PRS_CH1PULSE */
#define _PRS_SWPULSE_CH1PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH1PULSE_DEFAULT           (_PRS_SWPULSE_CH1PULSE_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH2PULSE                   (0x1UL << 2)                           /**< Channel 2 Pulse Generation */
#define _PRS_SWPULSE_CH2PULSE_SHIFT            2                                      /**< Shift value for PRS_CH2PULSE */
#define _PRS_SWPULSE_CH2PULSE_MASK             0x4UL                                  /**< Bit mask for PRS_CH2PULSE */
#define _PRS_SWPULSE_CH2PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH2PULSE_DEFAULT           (_PRS_SWPULSE_CH2PULSE_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH3PULSE                   (0x1UL << 3)                           /**< Channel 3 Pulse Generation */
#define _PRS_SWPULSE_CH3PULSE_SHIFT            3                                      /**< Shift value for PRS_CH3PULSE */
#define _PRS_SWPULSE_CH3PULSE_MASK             0x8UL                                  /**< Bit mask for PRS_CH3PULSE */
#define _PRS_SWPULSE_CH3PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH3PULSE_DEFAULT           (_PRS_SWPULSE_CH3PULSE_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH4PULSE                   (0x1UL << 4)                           /**< Channel 4 Pulse Generation */
#define _PRS_SWPULSE_CH4PULSE_SHIFT            4                                      /**< Shift value for PRS_CH4PULSE */
#define _PRS_SWPULSE_CH4PULSE_MASK             0x10UL                                 /**< Bit mask for PRS_CH4PULSE */
#define _PRS_SWPULSE_CH4PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH4PULSE_DEFAULT           (_PRS_SWPULSE_CH4PULSE_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH5PULSE                   (0x1UL << 5)                           /**< Channel 5 Pulse Generation */
#define _PRS_SWPULSE_CH5PULSE_SHIFT            5                                      /**< Shift value for PRS_CH5PULSE */
#define _PRS_SWPULSE_CH5PULSE_MASK             0x20UL                                 /**< Bit mask for PRS_CH5PULSE */
#define _PRS_SWPULSE_CH5PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH5PULSE_DEFAULT           (_PRS_SWPULSE_CH5PULSE_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH6PULSE                   (0x1UL << 6)                           /**< Channel 6 Pulse Generation */
#define _PRS_SWPULSE_CH6PULSE_SHIFT            6                                      /**< Shift value for PRS_CH6PULSE */
#define _PRS_SWPULSE_CH6PULSE_MASK             0x40UL                                 /**< Bit mask for PRS_CH6PULSE */
#define _PRS_SWPULSE_CH6PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH6PULSE_DEFAULT           (_PRS_SWPULSE_CH6PULSE_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH7PULSE                   (0x1UL << 7)                           /**< Channel 7 Pulse Generation */
#define _PRS_SWPULSE_CH7PULSE_SHIFT            7                                      /**< Shift value for PRS_CH7PULSE */
#define _PRS_SWPULSE_CH7PULSE_MASK             0x80UL                                 /**< Bit mask for PRS_CH7PULSE */
#define _PRS_SWPULSE_CH7PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH7PULSE_DEFAULT           (_PRS_SWPULSE_CH7PULSE_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH8PULSE                   (0x1UL << 8)                           /**< Channel 8 Pulse Generation */
#define _PRS_SWPULSE_CH8PULSE_SHIFT            8                                      /**< Shift value for PRS_CH8PULSE */
#define _PRS_SWPULSE_CH8PULSE_MASK             0x100UL                                /**< Bit mask for PRS_CH8PULSE */
#define _PRS_SWPULSE_CH8PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH8PULSE_DEFAULT           (_PRS_SWPULSE_CH8PULSE_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH9PULSE                   (0x1UL << 9)                           /**< Channel 9 Pulse Generation */
#define _PRS_SWPULSE_CH9PULSE_SHIFT            9                                      /**< Shift value for PRS_CH9PULSE */
#define _PRS_SWPULSE_CH9PULSE_MASK             0x200UL                                /**< Bit mask for PRS_CH9PULSE */
#define _PRS_SWPULSE_CH9PULSE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH9PULSE_DEFAULT           (_PRS_SWPULSE_CH9PULSE_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH10PULSE                  (0x1UL << 10)                          /**< Channel 10 Pulse Generation */
#define _PRS_SWPULSE_CH10PULSE_SHIFT           10                                     /**< Shift value for PRS_CH10PULSE */
#define _PRS_SWPULSE_CH10PULSE_MASK            0x400UL                                /**< Bit mask for PRS_CH10PULSE */
#define _PRS_SWPULSE_CH10PULSE_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH10PULSE_DEFAULT          (_PRS_SWPULSE_CH10PULSE_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH11PULSE                  (0x1UL << 11)                          /**< Channel 11 Pulse Generation */
#define _PRS_SWPULSE_CH11PULSE_SHIFT           11                                     /**< Shift value for PRS_CH11PULSE */
#define _PRS_SWPULSE_CH11PULSE_MASK            0x800UL                                /**< Bit mask for PRS_CH11PULSE */
#define _PRS_SWPULSE_CH11PULSE_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH11PULSE_DEFAULT          (_PRS_SWPULSE_CH11PULSE_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_SWPULSE */

/* Bit fields for PRS SWLEVEL */
#define _PRS_SWLEVEL_RESETVALUE                0x00000000UL                           /**< Default value for PRS_SWLEVEL */
#define _PRS_SWLEVEL_MASK                      0x00000FFFUL                           /**< Mask for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH0LEVEL                   (0x1UL << 0)                           /**< Channel 0 Software Level */
#define _PRS_SWLEVEL_CH0LEVEL_SHIFT            0                                      /**< Shift value for PRS_CH0LEVEL */
#define _PRS_SWLEVEL_CH0LEVEL_MASK             0x1UL                                  /**< Bit mask for PRS_CH0LEVEL */
#define _PRS_SWLEVEL_CH0LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH0LEVEL_DEFAULT           (_PRS_SWLEVEL_CH0LEVEL_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH1LEVEL                   (0x1UL << 1)                           /**< Channel 1 Software Level */
#define _PRS_SWLEVEL_CH1LEVEL_SHIFT            1                                      /**< Shift value for PRS_CH1LEVEL */
#define _PRS_SWLEVEL_CH1LEVEL_MASK             0x2UL                                  /**< Bit mask for PRS_CH1LEVEL */
#define _PRS_SWLEVEL_CH1LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH1LEVEL_DEFAULT           (_PRS_SWLEVEL_CH1LEVEL_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH2LEVEL                   (0x1UL << 2)                           /**< Channel 2 Software Level */
#define _PRS_SWLEVEL_CH2LEVEL_SHIFT            2                                      /**< Shift value for PRS_CH2LEVEL */
#define _PRS_SWLEVEL_CH2LEVEL_MASK             0x4UL                                  /**< Bit mask for PRS_CH2LEVEL */
#define _PRS_SWLEVEL_CH2LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH2LEVEL_DEFAULT           (_PRS_SWLEVEL_CH2LEVEL_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH3LEVEL                   (0x1UL << 3)                           /**< Channel 3 Software Level */
#define _PRS_SWLEVEL_CH3LEVEL_SHIFT            3                                      /**< Shift value for PRS_CH3LEVEL */
#define _PRS_SWLEVEL_CH3LEVEL_MASK             0x8UL                                  /**< Bit mask for PRS_CH3LEVEL */
#define _PRS_SWLEVEL_CH3LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH3LEVEL_DEFAULT           (_PRS_SWLEVEL_CH3LEVEL_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH4LEVEL                   (0x1UL << 4)                           /**< Channel 4 Software Level */
#define _PRS_SWLEVEL_CH4LEVEL_SHIFT            4                                      /**< Shift value for PRS_CH4LEVEL */
#define _PRS_SWLEVEL_CH4LEVEL_MASK             0x10UL                                 /**< Bit mask for PRS_CH4LEVEL */
#define _PRS_SWLEVEL_CH4LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH4LEVEL_DEFAULT           (_PRS_SWLEVEL_CH4LEVEL_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH5LEVEL                   (0x1UL << 5)                           /**< Channel 5 Software Level */
#define _PRS_SWLEVEL_CH5LEVEL_SHIFT            5                                      /**< Shift value for PRS_CH5LEVEL */
#define _PRS_SWLEVEL_CH5LEVEL_MASK             0x20UL                                 /**< Bit mask for PRS_CH5LEVEL */
#define _PRS_SWLEVEL_CH5LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH5LEVEL_DEFAULT           (_PRS_SWLEVEL_CH5LEVEL_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH6LEVEL                   (0x1UL << 6)                           /**< Channel 6 Software Level */
#define _PRS_SWLEVEL_CH6LEVEL_SHIFT            6                                      /**< Shift value for PRS_CH6LEVEL */
#define _PRS_SWLEVEL_CH6LEVEL_MASK             0x40UL                                 /**< Bit mask for PRS_CH6LEVEL */
#define _PRS_SWLEVEL_CH6LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH6LEVEL_DEFAULT           (_PRS_SWLEVEL_CH6LEVEL_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH7LEVEL                   (0x1UL << 7)                           /**< Channel 7 Software Level */
#define _PRS_SWLEVEL_CH7LEVEL_SHIFT            7                                      /**< Shift value for PRS_CH7LEVEL */
#define _PRS_SWLEVEL_CH7LEVEL_MASK             0x80UL                                 /**< Bit mask for PRS_CH7LEVEL */
#define _PRS_SWLEVEL_CH7LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH7LEVEL_DEFAULT           (_PRS_SWLEVEL_CH7LEVEL_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH8LEVEL                   (0x1UL << 8)                           /**< Channel 8 Software Level */
#define _PRS_SWLEVEL_CH8LEVEL_SHIFT            8                                      /**< Shift value for PRS_CH8LEVEL */
#define _PRS_SWLEVEL_CH8LEVEL_MASK             0x100UL                                /**< Bit mask for PRS_CH8LEVEL */
#define _PRS_SWLEVEL_CH8LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH8LEVEL_DEFAULT           (_PRS_SWLEVEL_CH8LEVEL_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH9LEVEL                   (0x1UL << 9)                           /**< Channel 9 Software Level */
#define _PRS_SWLEVEL_CH9LEVEL_SHIFT            9                                      /**< Shift value for PRS_CH9LEVEL */
#define _PRS_SWLEVEL_CH9LEVEL_MASK             0x200UL                                /**< Bit mask for PRS_CH9LEVEL */
#define _PRS_SWLEVEL_CH9LEVEL_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH9LEVEL_DEFAULT           (_PRS_SWLEVEL_CH9LEVEL_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH10LEVEL                  (0x1UL << 10)                          /**< Channel 10 Software Level */
#define _PRS_SWLEVEL_CH10LEVEL_SHIFT           10                                     /**< Shift value for PRS_CH10LEVEL */
#define _PRS_SWLEVEL_CH10LEVEL_MASK            0x400UL                                /**< Bit mask for PRS_CH10LEVEL */
#define _PRS_SWLEVEL_CH10LEVEL_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH10LEVEL_DEFAULT          (_PRS_SWLEVEL_CH10LEVEL_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH11LEVEL                  (0x1UL << 11)                          /**< Channel 11 Software Level */
#define _PRS_SWLEVEL_CH11LEVEL_SHIFT           11                                     /**< Shift value for PRS_CH11LEVEL */
#define _PRS_SWLEVEL_CH11LEVEL_MASK            0x800UL                                /**< Bit mask for PRS_CH11LEVEL */
#define _PRS_SWLEVEL_CH11LEVEL_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH11LEVEL_DEFAULT          (_PRS_SWLEVEL_CH11LEVEL_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_SWLEVEL */

/* Bit fields for PRS ROUTEPEN */
#define _PRS_ROUTEPEN_RESETVALUE               0x00000000UL                          /**< Default value for PRS_ROUTEPEN */
#define _PRS_ROUTEPEN_MASK                     0x00000FFFUL                          /**< Mask for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH0PEN                    (0x1UL << 0)                          /**< CH0 Pin Enable */
#define _PRS_ROUTEPEN_CH0PEN_SHIFT             0                                     /**< Shift value for PRS_CH0PEN */
#define _PRS_ROUTEPEN_CH0PEN_MASK              0x1UL                                 /**< Bit mask for PRS_CH0PEN */
#define _PRS_ROUTEPEN_CH0PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH0PEN_DEFAULT            (_PRS_ROUTEPEN_CH0PEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH1PEN                    (0x1UL << 1)                          /**< CH1 Pin Enable */
#define _PRS_ROUTEPEN_CH1PEN_SHIFT             1                                     /**< Shift value for PRS_CH1PEN */
#define _PRS_ROUTEPEN_CH1PEN_MASK              0x2UL                                 /**< Bit mask for PRS_CH1PEN */
#define _PRS_ROUTEPEN_CH1PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH1PEN_DEFAULT            (_PRS_ROUTEPEN_CH1PEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH2PEN                    (0x1UL << 2)                          /**< CH2 Pin Enable */
#define _PRS_ROUTEPEN_CH2PEN_SHIFT             2                                     /**< Shift value for PRS_CH2PEN */
#define _PRS_ROUTEPEN_CH2PEN_MASK              0x4UL                                 /**< Bit mask for PRS_CH2PEN */
#define _PRS_ROUTEPEN_CH2PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH2PEN_DEFAULT            (_PRS_ROUTEPEN_CH2PEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH3PEN                    (0x1UL << 3)                          /**< CH3 Pin Enable */
#define _PRS_ROUTEPEN_CH3PEN_SHIFT             3                                     /**< Shift value for PRS_CH3PEN */
#define _PRS_ROUTEPEN_CH3PEN_MASK              0x8UL                                 /**< Bit mask for PRS_CH3PEN */
#define _PRS_ROUTEPEN_CH3PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH3PEN_DEFAULT            (_PRS_ROUTEPEN_CH3PEN_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH4PEN                    (0x1UL << 4)                          /**< CH4 Pin Enable */
#define _PRS_ROUTEPEN_CH4PEN_SHIFT             4                                     /**< Shift value for PRS_CH4PEN */
#define _PRS_ROUTEPEN_CH4PEN_MASK              0x10UL                                /**< Bit mask for PRS_CH4PEN */
#define _PRS_ROUTEPEN_CH4PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH4PEN_DEFAULT            (_PRS_ROUTEPEN_CH4PEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH5PEN                    (0x1UL << 5)                          /**< CH5 Pin Enable */
#define _PRS_ROUTEPEN_CH5PEN_SHIFT             5                                     /**< Shift value for PRS_CH5PEN */
#define _PRS_ROUTEPEN_CH5PEN_MASK              0x20UL                                /**< Bit mask for PRS_CH5PEN */
#define _PRS_ROUTEPEN_CH5PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH5PEN_DEFAULT            (_PRS_ROUTEPEN_CH5PEN_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH6PEN                    (0x1UL << 6)                          /**< CH6 Pin Enable */
#define _PRS_ROUTEPEN_CH6PEN_SHIFT             6                                     /**< Shift value for PRS_CH6PEN */
#define _PRS_ROUTEPEN_CH6PEN_MASK              0x40UL                                /**< Bit mask for PRS_CH6PEN */
#define _PRS_ROUTEPEN_CH6PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH6PEN_DEFAULT            (_PRS_ROUTEPEN_CH6PEN_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH7PEN                    (0x1UL << 7)                          /**< CH7 Pin Enable */
#define _PRS_ROUTEPEN_CH7PEN_SHIFT             7                                     /**< Shift value for PRS_CH7PEN */
#define _PRS_ROUTEPEN_CH7PEN_MASK              0x80UL                                /**< Bit mask for PRS_CH7PEN */
#define _PRS_ROUTEPEN_CH7PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH7PEN_DEFAULT            (_PRS_ROUTEPEN_CH7PEN_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH8PEN                    (0x1UL << 8)                          /**< CH8 Pin Enable */
#define _PRS_ROUTEPEN_CH8PEN_SHIFT             8                                     /**< Shift value for PRS_CH8PEN */
#define _PRS_ROUTEPEN_CH8PEN_MASK              0x100UL                               /**< Bit mask for PRS_CH8PEN */
#define _PRS_ROUTEPEN_CH8PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH8PEN_DEFAULT            (_PRS_ROUTEPEN_CH8PEN_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH9PEN                    (0x1UL << 9)                          /**< CH9 Pin Enable */
#define _PRS_ROUTEPEN_CH9PEN_SHIFT             9                                     /**< Shift value for PRS_CH9PEN */
#define _PRS_ROUTEPEN_CH9PEN_MASK              0x200UL                               /**< Bit mask for PRS_CH9PEN */
#define _PRS_ROUTEPEN_CH9PEN_DEFAULT           0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH9PEN_DEFAULT            (_PRS_ROUTEPEN_CH9PEN_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH10PEN                   (0x1UL << 10)                         /**< CH10 Pin Enable */
#define _PRS_ROUTEPEN_CH10PEN_SHIFT            10                                    /**< Shift value for PRS_CH10PEN */
#define _PRS_ROUTEPEN_CH10PEN_MASK             0x400UL                               /**< Bit mask for PRS_CH10PEN */
#define _PRS_ROUTEPEN_CH10PEN_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH10PEN_DEFAULT           (_PRS_ROUTEPEN_CH10PEN_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH11PEN                   (0x1UL << 11)                         /**< CH11 Pin Enable */
#define _PRS_ROUTEPEN_CH11PEN_SHIFT            11                                    /**< Shift value for PRS_CH11PEN */
#define _PRS_ROUTEPEN_CH11PEN_MASK             0x800UL                               /**< Bit mask for PRS_CH11PEN */
#define _PRS_ROUTEPEN_CH11PEN_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH11PEN_DEFAULT           (_PRS_ROUTEPEN_CH11PEN_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */

/* Bit fields for PRS ROUTELOC0 */
#define _PRS_ROUTELOC0_RESETVALUE              0x00000000UL                          /**< Default value for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_MASK                    0x0F07070FUL                          /**< Mask for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_SHIFT            0                                     /**< Shift value for PRS_CH0LOC */
#define _PRS_ROUTELOC0_CH0LOC_MASK             0xFUL                                 /**< Bit mask for PRS_CH0LOC */
#define _PRS_ROUTELOC0_CH0LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC8             0x00000008UL                          /**< Mode LOC8 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC9             0x00000009UL                          /**< Mode LOC9 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC10            0x0000000AUL                          /**< Mode LOC10 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC11            0x0000000BUL                          /**< Mode LOC11 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC12            0x0000000CUL                          /**< Mode LOC12 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC13            0x0000000DUL                          /**< Mode LOC13 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC0              (_PRS_ROUTELOC0_CH0LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_DEFAULT           (_PRS_ROUTELOC0_CH0LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC1              (_PRS_ROUTELOC0_CH0LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC2              (_PRS_ROUTELOC0_CH0LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC3              (_PRS_ROUTELOC0_CH0LOC_LOC3 << 0)     /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC4              (_PRS_ROUTELOC0_CH0LOC_LOC4 << 0)     /**< Shifted mode LOC4 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC5              (_PRS_ROUTELOC0_CH0LOC_LOC5 << 0)     /**< Shifted mode LOC5 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC6              (_PRS_ROUTELOC0_CH0LOC_LOC6 << 0)     /**< Shifted mode LOC6 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC7              (_PRS_ROUTELOC0_CH0LOC_LOC7 << 0)     /**< Shifted mode LOC7 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC8              (_PRS_ROUTELOC0_CH0LOC_LOC8 << 0)     /**< Shifted mode LOC8 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC9              (_PRS_ROUTELOC0_CH0LOC_LOC9 << 0)     /**< Shifted mode LOC9 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC10             (_PRS_ROUTELOC0_CH0LOC_LOC10 << 0)    /**< Shifted mode LOC10 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC11             (_PRS_ROUTELOC0_CH0LOC_LOC11 << 0)    /**< Shifted mode LOC11 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC12             (_PRS_ROUTELOC0_CH0LOC_LOC12 << 0)    /**< Shifted mode LOC12 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC13             (_PRS_ROUTELOC0_CH0LOC_LOC13 << 0)    /**< Shifted mode LOC13 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_SHIFT            8                                     /**< Shift value for PRS_CH1LOC */
#define _PRS_ROUTELOC0_CH1LOC_MASK             0x700UL                               /**< Bit mask for PRS_CH1LOC */
#define _PRS_ROUTELOC0_CH1LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC0              (_PRS_ROUTELOC0_CH1LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_DEFAULT           (_PRS_ROUTELOC0_CH1LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC1              (_PRS_ROUTELOC0_CH1LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC2              (_PRS_ROUTELOC0_CH1LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC3              (_PRS_ROUTELOC0_CH1LOC_LOC3 << 8)     /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC4              (_PRS_ROUTELOC0_CH1LOC_LOC4 << 8)     /**< Shifted mode LOC4 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC5              (_PRS_ROUTELOC0_CH1LOC_LOC5 << 8)     /**< Shifted mode LOC5 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC6              (_PRS_ROUTELOC0_CH1LOC_LOC6 << 8)     /**< Shifted mode LOC6 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC7              (_PRS_ROUTELOC0_CH1LOC_LOC7 << 8)     /**< Shifted mode LOC7 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_SHIFT            16                                    /**< Shift value for PRS_CH2LOC */
#define _PRS_ROUTELOC0_CH2LOC_MASK             0x70000UL                             /**< Bit mask for PRS_CH2LOC */
#define _PRS_ROUTELOC0_CH2LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC0              (_PRS_ROUTELOC0_CH2LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_DEFAULT           (_PRS_ROUTELOC0_CH2LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC1              (_PRS_ROUTELOC0_CH2LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC2              (_PRS_ROUTELOC0_CH2LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC3              (_PRS_ROUTELOC0_CH2LOC_LOC3 << 16)    /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC4              (_PRS_ROUTELOC0_CH2LOC_LOC4 << 16)    /**< Shifted mode LOC4 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC5              (_PRS_ROUTELOC0_CH2LOC_LOC5 << 16)    /**< Shifted mode LOC5 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC6              (_PRS_ROUTELOC0_CH2LOC_LOC6 << 16)    /**< Shifted mode LOC6 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC7              (_PRS_ROUTELOC0_CH2LOC_LOC7 << 16)    /**< Shifted mode LOC7 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_SHIFT            24                                    /**< Shift value for PRS_CH3LOC */
#define _PRS_ROUTELOC0_CH3LOC_MASK             0xF000000UL                           /**< Bit mask for PRS_CH3LOC */
#define _PRS_ROUTELOC0_CH3LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC8             0x00000008UL                          /**< Mode LOC8 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC9             0x00000009UL                          /**< Mode LOC9 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC10            0x0000000AUL                          /**< Mode LOC10 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC11            0x0000000BUL                          /**< Mode LOC11 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC12            0x0000000CUL                          /**< Mode LOC12 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC13            0x0000000DUL                          /**< Mode LOC13 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC14            0x0000000EUL                          /**< Mode LOC14 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC0              (_PRS_ROUTELOC0_CH3LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_DEFAULT           (_PRS_ROUTELOC0_CH3LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC1              (_PRS_ROUTELOC0_CH3LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC2              (_PRS_ROUTELOC0_CH3LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC3              (_PRS_ROUTELOC0_CH3LOC_LOC3 << 24)    /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC4              (_PRS_ROUTELOC0_CH3LOC_LOC4 << 24)    /**< Shifted mode LOC4 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC5              (_PRS_ROUTELOC0_CH3LOC_LOC5 << 24)    /**< Shifted mode LOC5 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC6              (_PRS_ROUTELOC0_CH3LOC_LOC6 << 24)    /**< Shifted mode LOC6 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC7              (_PRS_ROUTELOC0_CH3LOC_LOC7 << 24)    /**< Shifted mode LOC7 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC8              (_PRS_ROUTELOC0_CH3LOC_LOC8 << 24)    /**< Shifted mode LOC8 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC9              (_PRS_ROUTELOC0_CH3LOC_LOC9 << 24)    /**< Shifted mode LOC9 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC10             (_PRS_ROUTELOC0_CH3LOC_LOC10 << 24)   /**< Shifted mode LOC10 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC11             (_PRS_ROUTELOC0_CH3LOC_LOC11 << 24)   /**< Shifted mode LOC11 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC12             (_PRS_ROUTELOC0_CH3LOC_LOC12 << 24)   /**< Shifted mode LOC12 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC13             (_PRS_ROUTELOC0_CH3LOC_LOC13 << 24)   /**< Shifted mode LOC13 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC14             (_PRS_ROUTELOC0_CH3LOC_LOC14 << 24)   /**< Shifted mode LOC14 for PRS_ROUTELOC0 */

/* Bit fields for PRS ROUTELOC1 */
#define _PRS_ROUTELOC1_RESETVALUE              0x00000000UL                          /**< Default value for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_MASK                    0x0F1F0707UL                          /**< Mask for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_SHIFT            0                                     /**< Shift value for PRS_CH4LOC */
#define _PRS_ROUTELOC1_CH4LOC_MASK             0x7UL                                 /**< Bit mask for PRS_CH4LOC */
#define _PRS_ROUTELOC1_CH4LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC0              (_PRS_ROUTELOC1_CH4LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_DEFAULT           (_PRS_ROUTELOC1_CH4LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC1              (_PRS_ROUTELOC1_CH4LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC2              (_PRS_ROUTELOC1_CH4LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC3              (_PRS_ROUTELOC1_CH4LOC_LOC3 << 0)     /**< Shifted mode LOC3 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC4              (_PRS_ROUTELOC1_CH4LOC_LOC4 << 0)     /**< Shifted mode LOC4 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC5              (_PRS_ROUTELOC1_CH4LOC_LOC5 << 0)     /**< Shifted mode LOC5 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC6              (_PRS_ROUTELOC1_CH4LOC_LOC6 << 0)     /**< Shifted mode LOC6 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_SHIFT            8                                     /**< Shift value for PRS_CH5LOC */
#define _PRS_ROUTELOC1_CH5LOC_MASK             0x700UL                               /**< Bit mask for PRS_CH5LOC */
#define _PRS_ROUTELOC1_CH5LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC0              (_PRS_ROUTELOC1_CH5LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_DEFAULT           (_PRS_ROUTELOC1_CH5LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC1              (_PRS_ROUTELOC1_CH5LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC2              (_PRS_ROUTELOC1_CH5LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC3              (_PRS_ROUTELOC1_CH5LOC_LOC3 << 8)     /**< Shifted mode LOC3 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC4              (_PRS_ROUTELOC1_CH5LOC_LOC4 << 8)     /**< Shifted mode LOC4 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC5              (_PRS_ROUTELOC1_CH5LOC_LOC5 << 8)     /**< Shifted mode LOC5 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC6              (_PRS_ROUTELOC1_CH5LOC_LOC6 << 8)     /**< Shifted mode LOC6 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_SHIFT            16                                    /**< Shift value for PRS_CH6LOC */
#define _PRS_ROUTELOC1_CH6LOC_MASK             0x1F0000UL                            /**< Bit mask for PRS_CH6LOC */
#define _PRS_ROUTELOC1_CH6LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC8             0x00000008UL                          /**< Mode LOC8 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC9             0x00000009UL                          /**< Mode LOC9 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC10            0x0000000AUL                          /**< Mode LOC10 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC11            0x0000000BUL                          /**< Mode LOC11 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC12            0x0000000CUL                          /**< Mode LOC12 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC13            0x0000000DUL                          /**< Mode LOC13 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC14            0x0000000EUL                          /**< Mode LOC14 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC15            0x0000000FUL                          /**< Mode LOC15 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC16            0x00000010UL                          /**< Mode LOC16 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC17            0x00000011UL                          /**< Mode LOC17 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC0              (_PRS_ROUTELOC1_CH6LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_DEFAULT           (_PRS_ROUTELOC1_CH6LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC1              (_PRS_ROUTELOC1_CH6LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC2              (_PRS_ROUTELOC1_CH6LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC3              (_PRS_ROUTELOC1_CH6LOC_LOC3 << 16)    /**< Shifted mode LOC3 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC4              (_PRS_ROUTELOC1_CH6LOC_LOC4 << 16)    /**< Shifted mode LOC4 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC5              (_PRS_ROUTELOC1_CH6LOC_LOC5 << 16)    /**< Shifted mode LOC5 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC6              (_PRS_ROUTELOC1_CH6LOC_LOC6 << 16)    /**< Shifted mode LOC6 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC7              (_PRS_ROUTELOC1_CH6LOC_LOC7 << 16)    /**< Shifted mode LOC7 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC8              (_PRS_ROUTELOC1_CH6LOC_LOC8 << 16)    /**< Shifted mode LOC8 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC9              (_PRS_ROUTELOC1_CH6LOC_LOC9 << 16)    /**< Shifted mode LOC9 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC10             (_PRS_ROUTELOC1_CH6LOC_LOC10 << 16)   /**< Shifted mode LOC10 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC11             (_PRS_ROUTELOC1_CH6LOC_LOC11 << 16)   /**< Shifted mode LOC11 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC12             (_PRS_ROUTELOC1_CH6LOC_LOC12 << 16)   /**< Shifted mode LOC12 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC13             (_PRS_ROUTELOC1_CH6LOC_LOC13 << 16)   /**< Shifted mode LOC13 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC14             (_PRS_ROUTELOC1_CH6LOC_LOC14 << 16)   /**< Shifted mode LOC14 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC15             (_PRS_ROUTELOC1_CH6LOC_LOC15 << 16)   /**< Shifted mode LOC15 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC16             (_PRS_ROUTELOC1_CH6LOC_LOC16 << 16)   /**< Shifted mode LOC16 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC17             (_PRS_ROUTELOC1_CH6LOC_LOC17 << 16)   /**< Shifted mode LOC17 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_SHIFT            24                                    /**< Shift value for PRS_CH7LOC */
#define _PRS_ROUTELOC1_CH7LOC_MASK             0xF000000UL                           /**< Bit mask for PRS_CH7LOC */
#define _PRS_ROUTELOC1_CH7LOC_LOC0             0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_DEFAULT          0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC1             0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC2             0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC3             0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC4             0x00000004UL                          /**< Mode LOC4 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC5             0x00000005UL                          /**< Mode LOC5 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC6             0x00000006UL                          /**< Mode LOC6 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC7             0x00000007UL                          /**< Mode LOC7 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC8             0x00000008UL                          /**< Mode LOC8 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC9             0x00000009UL                          /**< Mode LOC9 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC10            0x0000000AUL                          /**< Mode LOC10 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC0              (_PRS_ROUTELOC1_CH7LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_DEFAULT           (_PRS_ROUTELOC1_CH7LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC1              (_PRS_ROUTELOC1_CH7LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC2              (_PRS_ROUTELOC1_CH7LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC3              (_PRS_ROUTELOC1_CH7LOC_LOC3 << 24)    /**< Shifted mode LOC3 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC4              (_PRS_ROUTELOC1_CH7LOC_LOC4 << 24)    /**< Shifted mode LOC4 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC5              (_PRS_ROUTELOC1_CH7LOC_LOC5 << 24)    /**< Shifted mode LOC5 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC6              (_PRS_ROUTELOC1_CH7LOC_LOC6 << 24)    /**< Shifted mode LOC6 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC7              (_PRS_ROUTELOC1_CH7LOC_LOC7 << 24)    /**< Shifted mode LOC7 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC8              (_PRS_ROUTELOC1_CH7LOC_LOC8 << 24)    /**< Shifted mode LOC8 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC9              (_PRS_ROUTELOC1_CH7LOC_LOC9 << 24)    /**< Shifted mode LOC9 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC10             (_PRS_ROUTELOC1_CH7LOC_LOC10 << 24)   /**< Shifted mode LOC10 for PRS_ROUTELOC1 */

/* Bit fields for PRS ROUTELOC2 */
#define _PRS_ROUTELOC2_RESETVALUE              0x00000000UL                           /**< Default value for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_MASK                    0x07071F0FUL                           /**< Mask for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_SHIFT            0                                      /**< Shift value for PRS_CH8LOC */
#define _PRS_ROUTELOC2_CH8LOC_MASK             0xFUL                                  /**< Bit mask for PRS_CH8LOC */
#define _PRS_ROUTELOC2_CH8LOC_LOC0             0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC1             0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC2             0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC3             0x00000003UL                           /**< Mode LOC3 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC4             0x00000004UL                           /**< Mode LOC4 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC5             0x00000005UL                           /**< Mode LOC5 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC6             0x00000006UL                           /**< Mode LOC6 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC7             0x00000007UL                           /**< Mode LOC7 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC8             0x00000008UL                           /**< Mode LOC8 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC9             0x00000009UL                           /**< Mode LOC9 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC10            0x0000000AUL                           /**< Mode LOC10 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC0              (_PRS_ROUTELOC2_CH8LOC_LOC0 << 0)      /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_DEFAULT           (_PRS_ROUTELOC2_CH8LOC_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC1              (_PRS_ROUTELOC2_CH8LOC_LOC1 << 0)      /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC2              (_PRS_ROUTELOC2_CH8LOC_LOC2 << 0)      /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC3              (_PRS_ROUTELOC2_CH8LOC_LOC3 << 0)      /**< Shifted mode LOC3 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC4              (_PRS_ROUTELOC2_CH8LOC_LOC4 << 0)      /**< Shifted mode LOC4 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC5              (_PRS_ROUTELOC2_CH8LOC_LOC5 << 0)      /**< Shifted mode LOC5 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC6              (_PRS_ROUTELOC2_CH8LOC_LOC6 << 0)      /**< Shifted mode LOC6 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC7              (_PRS_ROUTELOC2_CH8LOC_LOC7 << 0)      /**< Shifted mode LOC7 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC8              (_PRS_ROUTELOC2_CH8LOC_LOC8 << 0)      /**< Shifted mode LOC8 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC9              (_PRS_ROUTELOC2_CH8LOC_LOC9 << 0)      /**< Shifted mode LOC9 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC10             (_PRS_ROUTELOC2_CH8LOC_LOC10 << 0)     /**< Shifted mode LOC10 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_SHIFT            8                                      /**< Shift value for PRS_CH9LOC */
#define _PRS_ROUTELOC2_CH9LOC_MASK             0x1F00UL                               /**< Bit mask for PRS_CH9LOC */
#define _PRS_ROUTELOC2_CH9LOC_LOC0             0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC1             0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC2             0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC3             0x00000003UL                           /**< Mode LOC3 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC4             0x00000004UL                           /**< Mode LOC4 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC5             0x00000005UL                           /**< Mode LOC5 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC6             0x00000006UL                           /**< Mode LOC6 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC7             0x00000007UL                           /**< Mode LOC7 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC8             0x00000008UL                           /**< Mode LOC8 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC9             0x00000009UL                           /**< Mode LOC9 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC10            0x0000000AUL                           /**< Mode LOC10 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC11            0x0000000BUL                           /**< Mode LOC11 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC12            0x0000000CUL                           /**< Mode LOC12 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC13            0x0000000DUL                           /**< Mode LOC13 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC14            0x0000000EUL                           /**< Mode LOC14 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC15            0x0000000FUL                           /**< Mode LOC15 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC16            0x00000010UL                           /**< Mode LOC16 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC0              (_PRS_ROUTELOC2_CH9LOC_LOC0 << 8)      /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_DEFAULT           (_PRS_ROUTELOC2_CH9LOC_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC1              (_PRS_ROUTELOC2_CH9LOC_LOC1 << 8)      /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC2              (_PRS_ROUTELOC2_CH9LOC_LOC2 << 8)      /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC3              (_PRS_ROUTELOC2_CH9LOC_LOC3 << 8)      /**< Shifted mode LOC3 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC4              (_PRS_ROUTELOC2_CH9LOC_LOC4 << 8)      /**< Shifted mode LOC4 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC5              (_PRS_ROUTELOC2_CH9LOC_LOC5 << 8)      /**< Shifted mode LOC5 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC6              (_PRS_ROUTELOC2_CH9LOC_LOC6 << 8)      /**< Shifted mode LOC6 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC7              (_PRS_ROUTELOC2_CH9LOC_LOC7 << 8)      /**< Shifted mode LOC7 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC8              (_PRS_ROUTELOC2_CH9LOC_LOC8 << 8)      /**< Shifted mode LOC8 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC9              (_PRS_ROUTELOC2_CH9LOC_LOC9 << 8)      /**< Shifted mode LOC9 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC10             (_PRS_ROUTELOC2_CH9LOC_LOC10 << 8)     /**< Shifted mode LOC10 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC11             (_PRS_ROUTELOC2_CH9LOC_LOC11 << 8)     /**< Shifted mode LOC11 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC12             (_PRS_ROUTELOC2_CH9LOC_LOC12 << 8)     /**< Shifted mode LOC12 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC13             (_PRS_ROUTELOC2_CH9LOC_LOC13 << 8)     /**< Shifted mode LOC13 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC14             (_PRS_ROUTELOC2_CH9LOC_LOC14 << 8)     /**< Shifted mode LOC14 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC15             (_PRS_ROUTELOC2_CH9LOC_LOC15 << 8)     /**< Shifted mode LOC15 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC16             (_PRS_ROUTELOC2_CH9LOC_LOC16 << 8)     /**< Shifted mode LOC16 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_SHIFT           16                                     /**< Shift value for PRS_CH10LOC */
#define _PRS_ROUTELOC2_CH10LOC_MASK            0x70000UL                              /**< Bit mask for PRS_CH10LOC */
#define _PRS_ROUTELOC2_CH10LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC5            0x00000005UL                           /**< Mode LOC5 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC0             (_PRS_ROUTELOC2_CH10LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_DEFAULT          (_PRS_ROUTELOC2_CH10LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC1             (_PRS_ROUTELOC2_CH10LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC2             (_PRS_ROUTELOC2_CH10LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC3             (_PRS_ROUTELOC2_CH10LOC_LOC3 << 16)    /**< Shifted mode LOC3 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC4             (_PRS_ROUTELOC2_CH10LOC_LOC4 << 16)    /**< Shifted mode LOC4 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC5             (_PRS_ROUTELOC2_CH10LOC_LOC5 << 16)    /**< Shifted mode LOC5 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_SHIFT           24                                     /**< Shift value for PRS_CH11LOC */
#define _PRS_ROUTELOC2_CH11LOC_MASK            0x7000000UL                            /**< Bit mask for PRS_CH11LOC */
#define _PRS_ROUTELOC2_CH11LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC5            0x00000005UL                           /**< Mode LOC5 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC0             (_PRS_ROUTELOC2_CH11LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_DEFAULT          (_PRS_ROUTELOC2_CH11LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC1             (_PRS_ROUTELOC2_CH11LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC2             (_PRS_ROUTELOC2_CH11LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC3             (_PRS_ROUTELOC2_CH11LOC_LOC3 << 24)    /**< Shifted mode LOC3 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC4             (_PRS_ROUTELOC2_CH11LOC_LOC4 << 24)    /**< Shifted mode LOC4 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC5             (_PRS_ROUTELOC2_CH11LOC_LOC5 << 24)    /**< Shifted mode LOC5 for PRS_ROUTELOC2 */

/* Bit fields for PRS CTRL */
#define _PRS_CTRL_RESETVALUE                   0x00000000UL                         /**< Default value for PRS_CTRL */
#define _PRS_CTRL_MASK                         0x0000001FUL                         /**< Mask for PRS_CTRL */
#define PRS_CTRL_SEVONPRS                      (0x1UL << 0)                         /**< Set Event on PRS */
#define _PRS_CTRL_SEVONPRS_SHIFT               0                                    /**< Shift value for PRS_SEVONPRS */
#define _PRS_CTRL_SEVONPRS_MASK                0x1UL                                /**< Bit mask for PRS_SEVONPRS */
#define _PRS_CTRL_SEVONPRS_DEFAULT             0x00000000UL                         /**< Mode DEFAULT for PRS_CTRL */
#define PRS_CTRL_SEVONPRS_DEFAULT              (_PRS_CTRL_SEVONPRS_DEFAULT << 0)    /**< Shifted mode DEFAULT for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_SHIFT            1                                    /**< Shift value for PRS_SEVONPRSSEL */
#define _PRS_CTRL_SEVONPRSSEL_MASK             0x1EUL                               /**< Bit mask for PRS_SEVONPRSSEL */
#define _PRS_CTRL_SEVONPRSSEL_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH0           0x00000000UL                         /**< Mode PRSCH0 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH1           0x00000001UL                         /**< Mode PRSCH1 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH2           0x00000002UL                         /**< Mode PRSCH2 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH3           0x00000003UL                         /**< Mode PRSCH3 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH4           0x00000004UL                         /**< Mode PRSCH4 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH5           0x00000005UL                         /**< Mode PRSCH5 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH6           0x00000006UL                         /**< Mode PRSCH6 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH7           0x00000007UL                         /**< Mode PRSCH7 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH8           0x00000008UL                         /**< Mode PRSCH8 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH9           0x00000009UL                         /**< Mode PRSCH9 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH10          0x0000000AUL                         /**< Mode PRSCH10 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH11          0x0000000BUL                         /**< Mode PRSCH11 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_DEFAULT           (_PRS_CTRL_SEVONPRSSEL_DEFAULT << 1) /**< Shifted mode DEFAULT for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH0            (_PRS_CTRL_SEVONPRSSEL_PRSCH0 << 1)  /**< Shifted mode PRSCH0 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH1            (_PRS_CTRL_SEVONPRSSEL_PRSCH1 << 1)  /**< Shifted mode PRSCH1 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH2            (_PRS_CTRL_SEVONPRSSEL_PRSCH2 << 1)  /**< Shifted mode PRSCH2 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH3            (_PRS_CTRL_SEVONPRSSEL_PRSCH3 << 1)  /**< Shifted mode PRSCH3 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH4            (_PRS_CTRL_SEVONPRSSEL_PRSCH4 << 1)  /**< Shifted mode PRSCH4 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH5            (_PRS_CTRL_SEVONPRSSEL_PRSCH5 << 1)  /**< Shifted mode PRSCH5 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH6            (_PRS_CTRL_SEVONPRSSEL_PRSCH6 << 1)  /**< Shifted mode PRSCH6 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH7            (_PRS_CTRL_SEVONPRSSEL_PRSCH7 << 1)  /**< Shifted mode PRSCH7 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH8            (_PRS_CTRL_SEVONPRSSEL_PRSCH8 << 1)  /**< Shifted mode PRSCH8 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH9            (_PRS_CTRL_SEVONPRSSEL_PRSCH9 << 1)  /**< Shifted mode PRSCH9 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH10           (_PRS_CTRL_SEVONPRSSEL_PRSCH10 << 1) /**< Shifted mode PRSCH10 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH11           (_PRS_CTRL_SEVONPRSSEL_PRSCH11 << 1) /**< Shifted mode PRSCH11 for PRS_CTRL */

/* Bit fields for PRS DMAREQ0 */
#define _PRS_DMAREQ0_RESETVALUE                0x00000000UL                       /**< Default value for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_MASK                      0x000003C0UL                       /**< Mask for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_SHIFT              6                                  /**< Shift value for PRS_PRSSEL */
#define _PRS_DMAREQ0_PRSSEL_MASK               0x3C0UL                            /**< Bit mask for PRS_PRSSEL */
#define _PRS_DMAREQ0_PRSSEL_DEFAULT            0x00000000UL                       /**< Mode DEFAULT for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH0             0x00000000UL                       /**< Mode PRSCH0 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH1             0x00000001UL                       /**< Mode PRSCH1 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH2             0x00000002UL                       /**< Mode PRSCH2 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH3             0x00000003UL                       /**< Mode PRSCH3 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH4             0x00000004UL                       /**< Mode PRSCH4 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH5             0x00000005UL                       /**< Mode PRSCH5 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH6             0x00000006UL                       /**< Mode PRSCH6 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH7             0x00000007UL                       /**< Mode PRSCH7 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH8             0x00000008UL                       /**< Mode PRSCH8 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH9             0x00000009UL                       /**< Mode PRSCH9 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH10            0x0000000AUL                       /**< Mode PRSCH10 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH11            0x0000000BUL                       /**< Mode PRSCH11 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_DEFAULT             (_PRS_DMAREQ0_PRSSEL_DEFAULT << 6) /**< Shifted mode DEFAULT for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH0              (_PRS_DMAREQ0_PRSSEL_PRSCH0 << 6)  /**< Shifted mode PRSCH0 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH1              (_PRS_DMAREQ0_PRSSEL_PRSCH1 << 6)  /**< Shifted mode PRSCH1 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH2              (_PRS_DMAREQ0_PRSSEL_PRSCH2 << 6)  /**< Shifted mode PRSCH2 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH3              (_PRS_DMAREQ0_PRSSEL_PRSCH3 << 6)  /**< Shifted mode PRSCH3 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH4              (_PRS_DMAREQ0_PRSSEL_PRSCH4 << 6)  /**< Shifted mode PRSCH4 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH5              (_PRS_DMAREQ0_PRSSEL_PRSCH5 << 6)  /**< Shifted mode PRSCH5 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH6              (_PRS_DMAREQ0_PRSSEL_PRSCH6 << 6)  /**< Shifted mode PRSCH6 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH7              (_PRS_DMAREQ0_PRSSEL_PRSCH7 << 6)  /**< Shifted mode PRSCH7 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH8              (_PRS_DMAREQ0_PRSSEL_PRSCH8 << 6)  /**< Shifted mode PRSCH8 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH9              (_PRS_DMAREQ0_PRSSEL_PRSCH9 << 6)  /**< Shifted mode PRSCH9 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH10             (_PRS_DMAREQ0_PRSSEL_PRSCH10 << 6) /**< Shifted mode PRSCH10 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH11             (_PRS_DMAREQ0_PRSSEL_PRSCH11 << 6) /**< Shifted mode PRSCH11 for PRS_DMAREQ0 */

/* Bit fields for PRS DMAREQ1 */
#define _PRS_DMAREQ1_RESETVALUE                0x00000000UL                       /**< Default value for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_MASK                      0x000003C0UL                       /**< Mask for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_SHIFT              6                                  /**< Shift value for PRS_PRSSEL */
#define _PRS_DMAREQ1_PRSSEL_MASK               0x3C0UL                            /**< Bit mask for PRS_PRSSEL */
#define _PRS_DMAREQ1_PRSSEL_DEFAULT            0x00000000UL                       /**< Mode DEFAULT for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH0             0x00000000UL                       /**< Mode PRSCH0 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH1             0x00000001UL                       /**< Mode PRSCH1 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH2             0x00000002UL                       /**< Mode PRSCH2 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH3             0x00000003UL                       /**< Mode PRSCH3 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH4             0x00000004UL                       /**< Mode PRSCH4 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH5             0x00000005UL                       /**< Mode PRSCH5 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH6             0x00000006UL                       /**< Mode PRSCH6 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH7             0x00000007UL                       /**< Mode PRSCH7 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH8             0x00000008UL                       /**< Mode PRSCH8 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH9             0x00000009UL                       /**< Mode PRSCH9 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH10            0x0000000AUL                       /**< Mode PRSCH10 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH11            0x0000000BUL                       /**< Mode PRSCH11 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_DEFAULT             (_PRS_DMAREQ1_PRSSEL_DEFAULT << 6) /**< Shifted mode DEFAULT for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH0              (_PRS_DMAREQ1_PRSSEL_PRSCH0 << 6)  /**< Shifted mode PRSCH0 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH1              (_PRS_DMAREQ1_PRSSEL_PRSCH1 << 6)  /**< Shifted mode PRSCH1 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH2              (_PRS_DMAREQ1_PRSSEL_PRSCH2 << 6)  /**< Shifted mode PRSCH2 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH3              (_PRS_DMAREQ1_PRSSEL_PRSCH3 << 6)  /**< Shifted mode PRSCH3 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH4              (_PRS_DMAREQ1_PRSSEL_PRSCH4 << 6)  /**< Shifted mode PRSCH4 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH5              (_PRS_DMAREQ1_PRSSEL_PRSCH5 << 6)  /**< Shifted mode PRSCH5 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH6              (_PRS_DMAREQ1_PRSSEL_PRSCH6 << 6)  /**< Shifted mode PRSCH6 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH7              (_PRS_DMAREQ1_PRSSEL_PRSCH7 << 6)  /**< Shifted mode PRSCH7 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH8              (_PRS_DMAREQ1_PRSSEL_PRSCH8 << 6)  /**< Shifted mode PRSCH8 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH9              (_PRS_DMAREQ1_PRSSEL_PRSCH9 << 6)  /**< Shifted mode PRSCH9 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH10             (_PRS_DMAREQ1_PRSSEL_PRSCH10 << 6) /**< Shifted mode PRSCH10 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH11             (_PRS_DMAREQ1_PRSSEL_PRSCH11 << 6) /**< Shifted mode PRSCH11 for PRS_DMAREQ1 */

/* Bit fields for PRS PEEK */
#define _PRS_PEEK_RESETVALUE                   0x00000000UL                      /**< Default value for PRS_PEEK */
#define _PRS_PEEK_MASK                         0x00000FFFUL                      /**< Mask for PRS_PEEK */
#define PRS_PEEK_CH0VAL                        (0x1UL << 0)                      /**< Channel 0 Current Value */
#define _PRS_PEEK_CH0VAL_SHIFT                 0                                 /**< Shift value for PRS_CH0VAL */
#define _PRS_PEEK_CH0VAL_MASK                  0x1UL                             /**< Bit mask for PRS_CH0VAL */
#define _PRS_PEEK_CH0VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH0VAL_DEFAULT                (_PRS_PEEK_CH0VAL_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH1VAL                        (0x1UL << 1)                      /**< Channel 1 Current Value */
#define _PRS_PEEK_CH1VAL_SHIFT                 1                                 /**< Shift value for PRS_CH1VAL */
#define _PRS_PEEK_CH1VAL_MASK                  0x2UL                             /**< Bit mask for PRS_CH1VAL */
#define _PRS_PEEK_CH1VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH1VAL_DEFAULT                (_PRS_PEEK_CH1VAL_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH2VAL                        (0x1UL << 2)                      /**< Channel 2 Current Value */
#define _PRS_PEEK_CH2VAL_SHIFT                 2                                 /**< Shift value for PRS_CH2VAL */
#define _PRS_PEEK_CH2VAL_MASK                  0x4UL                             /**< Bit mask for PRS_CH2VAL */
#define _PRS_PEEK_CH2VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH2VAL_DEFAULT                (_PRS_PEEK_CH2VAL_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH3VAL                        (0x1UL << 3)                      /**< Channel 3 Current Value */
#define _PRS_PEEK_CH3VAL_SHIFT                 3                                 /**< Shift value for PRS_CH3VAL */
#define _PRS_PEEK_CH3VAL_MASK                  0x8UL                             /**< Bit mask for PRS_CH3VAL */
#define _PRS_PEEK_CH3VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH3VAL_DEFAULT                (_PRS_PEEK_CH3VAL_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH4VAL                        (0x1UL << 4)                      /**< Channel 4 Current Value */
#define _PRS_PEEK_CH4VAL_SHIFT                 4                                 /**< Shift value for PRS_CH4VAL */
#define _PRS_PEEK_CH4VAL_MASK                  0x10UL                            /**< Bit mask for PRS_CH4VAL */
#define _PRS_PEEK_CH4VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH4VAL_DEFAULT                (_PRS_PEEK_CH4VAL_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH5VAL                        (0x1UL << 5)                      /**< Channel 5 Current Value */
#define _PRS_PEEK_CH5VAL_SHIFT                 5                                 /**< Shift value for PRS_CH5VAL */
#define _PRS_PEEK_CH5VAL_MASK                  0x20UL                            /**< Bit mask for PRS_CH5VAL */
#define _PRS_PEEK_CH5VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH5VAL_DEFAULT                (_PRS_PEEK_CH5VAL_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH6VAL                        (0x1UL << 6)                      /**< Channel 6 Current Value */
#define _PRS_PEEK_CH6VAL_SHIFT                 6                                 /**< Shift value for PRS_CH6VAL */
#define _PRS_PEEK_CH6VAL_MASK                  0x40UL                            /**< Bit mask for PRS_CH6VAL */
#define _PRS_PEEK_CH6VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH6VAL_DEFAULT                (_PRS_PEEK_CH6VAL_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH7VAL                        (0x1UL << 7)                      /**< Channel 7 Current Value */
#define _PRS_PEEK_CH7VAL_SHIFT                 7                                 /**< Shift value for PRS_CH7VAL */
#define _PRS_PEEK_CH7VAL_MASK                  0x80UL                            /**< Bit mask for PRS_CH7VAL */
#define _PRS_PEEK_CH7VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH7VAL_DEFAULT                (_PRS_PEEK_CH7VAL_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH8VAL                        (0x1UL << 8)                      /**< Channel 8 Current Value */
#define _PRS_PEEK_CH8VAL_SHIFT                 8                                 /**< Shift value for PRS_CH8VAL */
#define _PRS_PEEK_CH8VAL_MASK                  0x100UL                           /**< Bit mask for PRS_CH8VAL */
#define _PRS_PEEK_CH8VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH8VAL_DEFAULT                (_PRS_PEEK_CH8VAL_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH9VAL                        (0x1UL << 9)                      /**< Channel 9 Current Value */
#define _PRS_PEEK_CH9VAL_SHIFT                 9                                 /**< Shift value for PRS_CH9VAL */
#define _PRS_PEEK_CH9VAL_MASK                  0x200UL                           /**< Bit mask for PRS_CH9VAL */
#define _PRS_PEEK_CH9VAL_DEFAULT               0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH9VAL_DEFAULT                (_PRS_PEEK_CH9VAL_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH10VAL                       (0x1UL << 10)                     /**< Channel 10 Current Value */
#define _PRS_PEEK_CH10VAL_SHIFT                10                                /**< Shift value for PRS_CH10VAL */
#define _PRS_PEEK_CH10VAL_MASK                 0x400UL                           /**< Bit mask for PRS_CH10VAL */
#define _PRS_PEEK_CH10VAL_DEFAULT              0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH10VAL_DEFAULT               (_PRS_PEEK_CH10VAL_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH11VAL                       (0x1UL << 11)                     /**< Channel 11 Current Value */
#define _PRS_PEEK_CH11VAL_SHIFT                11                                /**< Shift value for PRS_CH11VAL */
#define _PRS_PEEK_CH11VAL_MASK                 0x800UL                           /**< Bit mask for PRS_CH11VAL */
#define _PRS_PEEK_CH11VAL_DEFAULT              0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH11VAL_DEFAULT               (_PRS_PEEK_CH11VAL_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_PEEK */

/* Bit fields for PRS CH_CTRL */
#define _PRS_CH_CTRL_RESETVALUE                0x00000000UL                               /**< Default value for PRS_CH_CTRL */
#define _PRS_CH_CTRL_MASK                      0x5E307F07UL                               /**< Mask for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_SHIFT              0                                          /**< Shift value for PRS_SIGSEL */
#define _PRS_CH_CTRL_SIGSEL_MASK               0x7UL                                      /**< Bit mask for PRS_SIGSEL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH0             0x00000000UL                               /**< Mode PRSCH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH8             0x00000000UL                               /**< Mode PRSCH8 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC0SINGLE         0x00000000UL                               /**< Mode ADC0SINGLE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0IRTX         0x00000000UL                               /**< Mode USART0IRTX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0UF           0x00000000UL                               /**< Mode TIMER0UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1UF           0x00000000UL                               /**< Mode TIMER1UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN0           0x00000000UL                               /**< Mode GPIOPIN0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN8           0x00000000UL                               /**< Mode GPIOPIN8 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER0CH0        0x00000000UL                               /**< Mode LETIMER0CH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD    0x00000000UL                               /**< Mode CRYOTIMERPERIOD for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CMUCLKOUT0         0x00000000UL                               /**< Mode CMUCLKOUT0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CM4TXEV            0x00000000UL                               /**< Mode CM4TXEV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH1             0x00000001UL                               /**< Mode PRSCH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH9             0x00000001UL                               /**< Mode PRSCH9 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC0SCAN           0x00000001UL                               /**< Mode ADC0SCAN for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0TXC          0x00000001UL                               /**< Mode USART0TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1TXC          0x00000001UL                               /**< Mode USART1TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0OF           0x00000001UL                               /**< Mode TIMER0OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1OF           0x00000001UL                               /**< Mode TIMER1OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV0           0x00000001UL                               /**< Mode RTCCCCV0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN1           0x00000001UL                               /**< Mode GPIOPIN1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN9           0x00000001UL                               /**< Mode GPIOPIN9 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER0CH1        0x00000001UL                               /**< Mode LETIMER0CH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CMUCLKOUT1         0x00000001UL                               /**< Mode CMUCLKOUT1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH2             0x00000002UL                               /**< Mode PRSCH2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH10            0x00000002UL                               /**< Mode PRSCH10 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0RXDATAV      0x00000002UL                               /**< Mode USART0RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1RXDATAV      0x00000002UL                               /**< Mode USART1RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC0          0x00000002UL                               /**< Mode TIMER0CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC0          0x00000002UL                               /**< Mode TIMER1CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV1           0x00000002UL                               /**< Mode RTCCCCV1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN2           0x00000002UL                               /**< Mode GPIOPIN2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN10          0x00000002UL                               /**< Mode GPIOPIN10 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH3             0x00000003UL                               /**< Mode PRSCH3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH11            0x00000003UL                               /**< Mode PRSCH11 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0RTS          0x00000003UL                               /**< Mode USART0RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1RTS          0x00000003UL                               /**< Mode USART1RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC1          0x00000003UL                               /**< Mode TIMER0CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC1          0x00000003UL                               /**< Mode TIMER1CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV2           0x00000003UL                               /**< Mode RTCCCCV2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN3           0x00000003UL                               /**< Mode GPIOPIN3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN11          0x00000003UL                               /**< Mode GPIOPIN11 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH4             0x00000004UL                               /**< Mode PRSCH4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC2          0x00000004UL                               /**< Mode TIMER0CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC2          0x00000004UL                               /**< Mode TIMER1CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN4           0x00000004UL                               /**< Mode GPIOPIN4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN12          0x00000004UL                               /**< Mode GPIOPIN12 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH5             0x00000005UL                               /**< Mode PRSCH5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0TX           0x00000005UL                               /**< Mode USART0TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1TX           0x00000005UL                               /**< Mode USART1TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC3          0x00000005UL                               /**< Mode TIMER1CC3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN5           0x00000005UL                               /**< Mode GPIOPIN5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN13          0x00000005UL                               /**< Mode GPIOPIN13 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH6             0x00000006UL                               /**< Mode PRSCH6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0CS           0x00000006UL                               /**< Mode USART0CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1CS           0x00000006UL                               /**< Mode USART1CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN6           0x00000006UL                               /**< Mode GPIOPIN6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN14          0x00000006UL                               /**< Mode GPIOPIN14 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH7             0x00000007UL                               /**< Mode PRSCH7 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN7           0x00000007UL                               /**< Mode GPIOPIN7 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN15          0x00000007UL                               /**< Mode GPIOPIN15 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH0              (_PRS_CH_CTRL_SIGSEL_PRSCH0 << 0)          /**< Shifted mode PRSCH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH8              (_PRS_CH_CTRL_SIGSEL_PRSCH8 << 0)          /**< Shifted mode PRSCH8 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC0SINGLE          (_PRS_CH_CTRL_SIGSEL_ADC0SINGLE << 0)      /**< Shifted mode ADC0SINGLE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0IRTX          (_PRS_CH_CTRL_SIGSEL_USART0IRTX << 0)      /**< Shifted mode USART0IRTX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0UF            (_PRS_CH_CTRL_SIGSEL_TIMER0UF << 0)        /**< Shifted mode TIMER0UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1UF            (_PRS_CH_CTRL_SIGSEL_TIMER1UF << 0)        /**< Shifted mode TIMER1UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN0            (_PRS_CH_CTRL_SIGSEL_GPIOPIN0 << 0)        /**< Shifted mode GPIOPIN0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN8            (_PRS_CH_CTRL_SIGSEL_GPIOPIN8 << 0)        /**< Shifted mode GPIOPIN8 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER0CH0         (_PRS_CH_CTRL_SIGSEL_LETIMER0CH0 << 0)     /**< Shifted mode LETIMER0CH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD     (_PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD << 0) /**< Shifted mode CRYOTIMERPERIOD for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CMUCLKOUT0          (_PRS_CH_CTRL_SIGSEL_CMUCLKOUT0 << 0)      /**< Shifted mode CMUCLKOUT0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CM4TXEV             (_PRS_CH_CTRL_SIGSEL_CM4TXEV << 0)         /**< Shifted mode CM4TXEV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH1              (_PRS_CH_CTRL_SIGSEL_PRSCH1 << 0)          /**< Shifted mode PRSCH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH9              (_PRS_CH_CTRL_SIGSEL_PRSCH9 << 0)          /**< Shifted mode PRSCH9 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC0SCAN            (_PRS_CH_CTRL_SIGSEL_ADC0SCAN << 0)        /**< Shifted mode ADC0SCAN for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0TXC           (_PRS_CH_CTRL_SIGSEL_USART0TXC << 0)       /**< Shifted mode USART0TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1TXC           (_PRS_CH_CTRL_SIGSEL_USART1TXC << 0)       /**< Shifted mode USART1TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0OF            (_PRS_CH_CTRL_SIGSEL_TIMER0OF << 0)        /**< Shifted mode TIMER0OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1OF            (_PRS_CH_CTRL_SIGSEL_TIMER1OF << 0)        /**< Shifted mode TIMER1OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV0            (_PRS_CH_CTRL_SIGSEL_RTCCCCV0 << 0)        /**< Shifted mode RTCCCCV0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN1            (_PRS_CH_CTRL_SIGSEL_GPIOPIN1 << 0)        /**< Shifted mode GPIOPIN1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN9            (_PRS_CH_CTRL_SIGSEL_GPIOPIN9 << 0)        /**< Shifted mode GPIOPIN9 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER0CH1         (_PRS_CH_CTRL_SIGSEL_LETIMER0CH1 << 0)     /**< Shifted mode LETIMER0CH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CMUCLKOUT1          (_PRS_CH_CTRL_SIGSEL_CMUCLKOUT1 << 0)      /**< Shifted mode CMUCLKOUT1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH2              (_PRS_CH_CTRL_SIGSEL_PRSCH2 << 0)          /**< Shifted mode PRSCH2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH10             (_PRS_CH_CTRL_SIGSEL_PRSCH10 << 0)         /**< Shifted mode PRSCH10 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0RXDATAV       (_PRS_CH_CTRL_SIGSEL_USART0RXDATAV << 0)   /**< Shifted mode USART0RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1RXDATAV       (_PRS_CH_CTRL_SIGSEL_USART1RXDATAV << 0)   /**< Shifted mode USART1RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC0           (_PRS_CH_CTRL_SIGSEL_TIMER0CC0 << 0)       /**< Shifted mode TIMER0CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC0           (_PRS_CH_CTRL_SIGSEL_TIMER1CC0 << 0)       /**< Shifted mode TIMER1CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV1            (_PRS_CH_CTRL_SIGSEL_RTCCCCV1 << 0)        /**< Shifted mode RTCCCCV1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN2            (_PRS_CH_CTRL_SIGSEL_GPIOPIN2 << 0)        /**< Shifted mode GPIOPIN2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN10           (_PRS_CH_CTRL_SIGSEL_GPIOPIN10 << 0)       /**< Shifted mode GPIOPIN10 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH3              (_PRS_CH_CTRL_SIGSEL_PRSCH3 << 0)          /**< Shifted mode PRSCH3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH11             (_PRS_CH_CTRL_SIGSEL_PRSCH11 << 0)         /**< Shifted mode PRSCH11 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0RTS           (_PRS_CH_CTRL_SIGSEL_USART0RTS << 0)       /**< Shifted mode USART0RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1RTS           (_PRS_CH_CTRL_SIGSEL_USART1RTS << 0)       /**< Shifted mode USART1RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC1           (_PRS_CH_CTRL_SIGSEL_TIMER0CC1 << 0)       /**< Shifted mode TIMER0CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC1           (_PRS_CH_CTRL_SIGSEL_TIMER1CC1 << 0)       /**< Shifted mode TIMER1CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV2            (_PRS_CH_CTRL_SIGSEL_RTCCCCV2 << 0)        /**< Shifted mode RTCCCCV2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN3            (_PRS_CH_CTRL_SIGSEL_GPIOPIN3 << 0)        /**< Shifted mode GPIOPIN3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN11           (_PRS_CH_CTRL_SIGSEL_GPIOPIN11 << 0)       /**< Shifted mode GPIOPIN11 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH4              (_PRS_CH_CTRL_SIGSEL_PRSCH4 << 0)          /**< Shifted mode PRSCH4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC2           (_PRS_CH_CTRL_SIGSEL_TIMER0CC2 << 0)       /**< Shifted mode TIMER0CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC2           (_PRS_CH_CTRL_SIGSEL_TIMER1CC2 << 0)       /**< Shifted mode TIMER1CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN4            (_PRS_CH_CTRL_SIGSEL_GPIOPIN4 << 0)        /**< Shifted mode GPIOPIN4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN12           (_PRS_CH_CTRL_SIGSEL_GPIOPIN12 << 0)       /**< Shifted mode GPIOPIN12 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH5              (_PRS_CH_CTRL_SIGSEL_PRSCH5 << 0)          /**< Shifted mode PRSCH5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0TX            (_PRS_CH_CTRL_SIGSEL_USART0TX << 0)        /**< Shifted mode USART0TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1TX            (_PRS_CH_CTRL_SIGSEL_USART1TX << 0)        /**< Shifted mode USART1TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC3           (_PRS_CH_CTRL_SIGSEL_TIMER1CC3 << 0)       /**< Shifted mode TIMER1CC3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN5            (_PRS_CH_CTRL_SIGSEL_GPIOPIN5 << 0)        /**< Shifted mode GPIOPIN5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN13           (_PRS_CH_CTRL_SIGSEL_GPIOPIN13 << 0)       /**< Shifted mode GPIOPIN13 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH6              (_PRS_CH_CTRL_SIGSEL_PRSCH6 << 0)          /**< Shifted mode PRSCH6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0CS            (_PRS_CH_CTRL_SIGSEL_USART0CS << 0)        /**< Shifted mode USART0CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1CS            (_PRS_CH_CTRL_SIGSEL_USART1CS << 0)        /**< Shifted mode USART1CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN6            (_PRS_CH_CTRL_SIGSEL_GPIOPIN6 << 0)        /**< Shifted mode GPIOPIN6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN14           (_PRS_CH_CTRL_SIGSEL_GPIOPIN14 << 0)       /**< Shifted mode GPIOPIN14 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH7              (_PRS_CH_CTRL_SIGSEL_PRSCH7 << 0)          /**< Shifted mode PRSCH7 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN7            (_PRS_CH_CTRL_SIGSEL_GPIOPIN7 << 0)        /**< Shifted mode GPIOPIN7 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN15           (_PRS_CH_CTRL_SIGSEL_GPIOPIN15 << 0)       /**< Shifted mode GPIOPIN15 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_SHIFT           8                                          /**< Shift value for PRS_SOURCESEL */
#define _PRS_CH_CTRL_SOURCESEL_MASK            0x7F00UL                                   /**< Bit mask for PRS_SOURCESEL */
#define _PRS_CH_CTRL_SOURCESEL_NONE            0x00000000UL                               /**< Mode NONE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PRSL            0x00000001UL                               /**< Mode PRSL for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PRSH            0x00000002UL                               /**< Mode PRSH for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ADC0            0x00000008UL                               /**< Mode ADC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART0          0x00000010UL                               /**< Mode USART0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART1          0x00000011UL                               /**< Mode USART1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER0          0x0000001CUL                               /**< Mode TIMER0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER1          0x0000001DUL                               /**< Mode TIMER1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_RTCC            0x00000029UL                               /**< Mode RTCC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_GPIOL           0x00000030UL                               /**< Mode GPIOL for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_GPIOH           0x00000031UL                               /**< Mode GPIOH for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LETIMER0        0x00000034UL                               /**< Mode LETIMER0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CRYOTIMER       0x0000003CUL                               /**< Mode CRYOTIMER for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CMU             0x0000003DUL                               /**< Mode CMU for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CM4             0x00000043UL                               /**< Mode CM4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_NONE             (_PRS_CH_CTRL_SOURCESEL_NONE << 8)         /**< Shifted mode NONE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PRSL             (_PRS_CH_CTRL_SOURCESEL_PRSL << 8)         /**< Shifted mode PRSL for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PRSH             (_PRS_CH_CTRL_SOURCESEL_PRSH << 8)         /**< Shifted mode PRSH for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ADC0             (_PRS_CH_CTRL_SOURCESEL_ADC0 << 8)         /**< Shifted mode ADC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART0           (_PRS_CH_CTRL_SOURCESEL_USART0 << 8)       /**< Shifted mode USART0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART1           (_PRS_CH_CTRL_SOURCESEL_USART1 << 8)       /**< Shifted mode USART1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER0           (_PRS_CH_CTRL_SOURCESEL_TIMER0 << 8)       /**< Shifted mode TIMER0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER1           (_PRS_CH_CTRL_SOURCESEL_TIMER1 << 8)       /**< Shifted mode TIMER1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_RTCC             (_PRS_CH_CTRL_SOURCESEL_RTCC << 8)         /**< Shifted mode RTCC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_GPIOL            (_PRS_CH_CTRL_SOURCESEL_GPIOL << 8)        /**< Shifted mode GPIOL for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_GPIOH            (_PRS_CH_CTRL_SOURCESEL_GPIOH << 8)        /**< Shifted mode GPIOH for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LETIMER0         (_PRS_CH_CTRL_SOURCESEL_LETIMER0 << 8)     /**< Shifted mode LETIMER0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CRYOTIMER        (_PRS_CH_CTRL_SOURCESEL_CRYOTIMER << 8)    /**< Shifted mode CRYOTIMER for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CMU              (_PRS_CH_CTRL_SOURCESEL_CMU << 8)          /**< Shifted mode CMU for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CM4              (_PRS_CH_CTRL_SOURCESEL_CM4 << 8)          /**< Shifted mode CM4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_SHIFT               20                                         /**< Shift value for PRS_EDSEL */
#define _PRS_CH_CTRL_EDSEL_MASK                0x300000UL                                 /**< Bit mask for PRS_EDSEL */
#define _PRS_CH_CTRL_EDSEL_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_OFF                 0x00000000UL                               /**< Mode OFF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_POSEDGE             0x00000001UL                               /**< Mode POSEDGE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_NEGEDGE             0x00000002UL                               /**< Mode NEGEDGE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_BOTHEDGES           0x00000003UL                               /**< Mode BOTHEDGES for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_DEFAULT              (_PRS_CH_CTRL_EDSEL_DEFAULT << 20)         /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_OFF                  (_PRS_CH_CTRL_EDSEL_OFF << 20)             /**< Shifted mode OFF for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_POSEDGE              (_PRS_CH_CTRL_EDSEL_POSEDGE << 20)         /**< Shifted mode POSEDGE for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_NEGEDGE              (_PRS_CH_CTRL_EDSEL_NEGEDGE << 20)         /**< Shifted mode NEGEDGE for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_BOTHEDGES            (_PRS_CH_CTRL_EDSEL_BOTHEDGES << 20)       /**< Shifted mode BOTHEDGES for PRS_CH_CTRL */
#define PRS_CH_CTRL_STRETCH                    (0x1UL << 25)                              /**< Stretch Channel Output */
#define _PRS_CH_CTRL_STRETCH_SHIFT             25                                         /**< Shift value for PRS_STRETCH */
#define _PRS_CH_CTRL_STRETCH_MASK              0x2000000UL                                /**< Bit mask for PRS_STRETCH */
#define _PRS_CH_CTRL_STRETCH_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_STRETCH_DEFAULT            (_PRS_CH_CTRL_STRETCH_DEFAULT << 25)       /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_INV                        (0x1UL << 26)                              /**< Invert Channel */
#define _PRS_CH_CTRL_INV_SHIFT                 26                                         /**< Shift value for PRS_INV */
#define _PRS_CH_CTRL_INV_MASK                  0x4000000UL                                /**< Bit mask for PRS_INV */
#define _PRS_CH_CTRL_INV_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_INV_DEFAULT                (_PRS_CH_CTRL_INV_DEFAULT << 26)           /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ORPREV                     (0x1UL << 27)                              /**< Or Previous */
#define _PRS_CH_CTRL_ORPREV_SHIFT              27                                         /**< Shift value for PRS_ORPREV */
#define _PRS_CH_CTRL_ORPREV_MASK               0x8000000UL                                /**< Bit mask for PRS_ORPREV */
#define _PRS_CH_CTRL_ORPREV_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ORPREV_DEFAULT             (_PRS_CH_CTRL_ORPREV_DEFAULT << 27)        /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ANDNEXT                    (0x1UL << 28)                              /**< And Next */
#define _PRS_CH_CTRL_ANDNEXT_SHIFT             28                                         /**< Shift value for PRS_ANDNEXT */
#define _PRS_CH_CTRL_ANDNEXT_MASK              0x10000000UL                               /**< Bit mask for PRS_ANDNEXT */
#define _PRS_CH_CTRL_ANDNEXT_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ANDNEXT_DEFAULT            (_PRS_CH_CTRL_ANDNEXT_DEFAULT << 28)       /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ASYNC                      (0x1UL << 30)                              /**< Asynchronous Reflex */
#define _PRS_CH_CTRL_ASYNC_SHIFT               30                                         /**< Shift value for PRS_ASYNC */
#define _PRS_CH_CTRL_ASYNC_MASK                0x40000000UL                               /**< Bit mask for PRS_ASYNC */
#define _PRS_CH_CTRL_ASYNC_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ASYNC_DEFAULT              (_PRS_CH_CTRL_ASYNC_DEFAULT << 30)         /**< Shifted mode DEFAULT for PRS_CH_CTRL */

/** @} */
/** @} End of group EFR32FG1V232F256GM48_PRS */

/***************************************************************************//**
 * @defgroup EFR32FG1V232F256GM48_UNLOCK Unlock Codes
 * @{
 ******************************************************************************/
#define MSC_UNLOCK_CODE      0x1B71 /**< MSC unlock code */
#define EMU_UNLOCK_CODE      0xADE8 /**< EMU unlock code */
#define RMU_UNLOCK_CODE      0xE084 /**< RMU unlock code */
#define CMU_UNLOCK_CODE      0x580E /**< CMU unlock code */
#define GPIO_UNLOCK_CODE     0xA534 /**< GPIO unlock code */
#define TIMER_UNLOCK_CODE    0xCE80 /**< TIMER unlock code */
#define RTCC_UNLOCK_CODE     0xAEE8 /**< RTCC unlock code */

/** @} End of group EFR32FG1V232F256GM48_UNLOCK */

/** @} End of group EFR32FG1V232F256GM48_BitFields */

/***************************************************************************//**
 * @addtogroup EFR32FG1V232F256GM48_Alternate_Function Alternate Function
 * @{
 * @defgroup EFR32FG1V232F256GM48_AF_Ports Alternate Function Ports
 * @{
 ******************************************************************************/

#define AF_CMU_CLK0_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 3 : (i) == 5 ? 3 : (i) == 6 ? 5 : (i) == 7 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                               /**< Port number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 3 : (i) == 5 ? 3 : (i) == 6 ? 5 : (i) == 7 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                               /**< Port number for AF_CMU_CLK1 location number i */
#define AF_PRS_CH0_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 5 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 5 : (i) == 6 ? 5 : (i) == 7 ? 5 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 :  -1)                                                                                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 5 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 5 : (i) == 6 ? 5 : (i) == 7 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                               /**< Port number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 5 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 5 : (i) == 6 ? 5 : (i) == 7 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                               /**< Port number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 5 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 5 : (i) == 6 ? 5 : (i) == 7 ? 5 : (i) == 8 ? 3 : (i) == 9 ? 3 : (i) == 10 ? 3 : (i) == 11 ? 3 : (i) == 12 ? 3 : (i) == 13 ? 3 : (i) == 14 ? 3 :  -1)                                                                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PORT(i)          ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 3 : (i) == 4 ? 3 : (i) == 5 ? 3 : (i) == 6 ? 3 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                              /**< Port number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PORT(i)          ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 3 : (i) == 4 ? 3 : (i) == 5 ? 3 : (i) == 6 ? 3 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                              /**< Port number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 3 : (i) == 12 ? 3 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 :  -1)                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH7 location number i */
#define AF_PRS_CH8_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 0 : (i) == 10 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH8 location number i */
#define AF_PRS_CH9_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 0 : (i) == 9 ? 0 : (i) == 10 ? 0 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 :  -1)                                                                                                                                                                                                                                                 /**< Port number for AF_PRS_CH9 location number i */
#define AF_PRS_CH10_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 2 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 2 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                             /**< Port number for AF_PRS_CH10 location number i */
#define AF_PRS_CH11_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 2 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 2 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                             /**< Port number for AF_PRS_CH11 location number i */
#define AF_TIMER0_CC0_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PORT(i)       (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 2 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 3 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 5 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 0 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PORT(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PORT(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PORT(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PORT(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PORT(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_TIMER1_CDTI3 location number i */
#define AF_USART0_TX_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_USART0_TX location number i */
#define AF_USART0_RX_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 2 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 3 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 5 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 0 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_USART1_TX location number i */
#define AF_USART1_RX_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 2 : (i) == 7 ? 2 : (i) == 8 ? 2 : (i) == 9 ? 2 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 3 : (i) == 13 ? 3 : (i) == 14 ? 3 : (i) == 15 ? 3 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 5 : (i) == 20 ? 5 : (i) == 21 ? 5 : (i) == 22 ? 5 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 0 : (i) == 28 ? 0 : (i) == 29 ? 0 : (i) == 30 ? 0 : (i) == 31 ? 0 :  -1) /**< Port number for AF_USART1_RTS location number i */
#define AF_LEUART0_TX_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_LEUART0_RX location number i */
#define AF_LETIMER0_OUT0_PORT(i)    ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PORT(i)    ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_LETIMER0_OUT1 location number i */
#define AF_I2C0_SDA_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 1 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 2 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 3 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 5 :  -1) /**< Port number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 : (i) == 8 ? 1 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 2 : (i) == 12 ? 2 : (i) == 13 ? 2 : (i) == 14 ? 2 : (i) == 15 ? 2 : (i) == 16 ? 3 : (i) == 17 ? 3 : (i) == 18 ? 3 : (i) == 19 ? 3 : (i) == 20 ? 3 : (i) == 21 ? 3 : (i) == 22 ? 3 : (i) == 23 ? 5 : (i) == 24 ? 5 : (i) == 25 ? 5 : (i) == 26 ? 5 : (i) == 27 ? 5 : (i) == 28 ? 5 : (i) == 29 ? 5 : (i) == 30 ? 5 : (i) == 31 ? 0 :  -1) /**< Port number for AF_I2C0_SCL location number i */
#define AF_DBG_TDI_PORT(i)          ((i) == 0 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PORT(i)          ((i) == 0 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 1 : (i) == 2 ? 3 : (i) == 3 ? 2 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                           /**< Port number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PORT(i)     ((i) == 0 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PORT(i)     ((i) == 0 ? 5 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Port number for AF_DBG_SWCLKTCK location number i */

/** @} */
/** @} End of group EFR32FG1V232F256GM48_AF_Ports */

/***************************************************************************//**
 * @addtogroup EFR32FG1V232F256GM48_Alternate_Function Alternate Function
 * @{
 * @defgroup EFR32FG1V232F256GM48_AF_Pins Alternate Function Pins
 * @{
 ******************************************************************************/

#define AF_CMU_CLK0_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 15 : (i) == 2 ? 6 : (i) == 3 ? 11 : (i) == 4 ? 9 : (i) == 5 ? 14 : (i) == 6 ? 2 : (i) == 7 ? 7 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 14 : (i) == 2 ? 7 : (i) == 3 ? 10 : (i) == 4 ? 10 : (i) == 5 ? 15 : (i) == 6 ? 3 : (i) == 7 ? 6 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_CMU_CLK1 location number i */
#define AF_PRS_CH0_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 :  -1)                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 6 : (i) == 6 ? 7 : (i) == 7 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 6 : (i) == 5 ? 7 : (i) == 6 ? 0 : (i) == 7 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 6 : (i) == 4 ? 7 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 2 : (i) == 8 ? 9 : (i) == 9 ? 10 : (i) == 10 ? 11 : (i) == 11 ? 12 : (i) == 12 ? 13 : (i) == 13 ? 14 : (i) == 14 ? 15 :  -1)                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PIN(i)          ((i) == 0 ? 9 : (i) == 1 ? 10 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PIN(i)          ((i) == 0 ? 10 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 9 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 12 : (i) == 15 ? 13 : (i) == 16 ? 14 : (i) == 17 ? 15 :  -1)                                                                                                                                                                                                                                   /**< Pin number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_PRS_CH7 location number i */
#define AF_PRS_CH8_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 0 : (i) == 10 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_PRS_CH8 location number i */
#define AF_PRS_CH9_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 0 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 :  -1)                                                                                                                                                                                                                                                       /**< Pin number for AF_PRS_CH9 location number i */
#define AF_PRS_CH10_PIN(i)         ((i) == 0 ? 6 : (i) == 1 ? 7 : (i) == 2 ? 8 : (i) == 3 ? 9 : (i) == 4 ? 10 : (i) == 5 ? 11 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH10 location number i */
#define AF_PRS_CH11_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 8 : (i) == 2 ? 9 : (i) == 3 ? 10 : (i) == 4 ? 11 : (i) == 5 ? 6 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH11 location number i */
#define AF_TIMER0_CC0_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PIN(i)       (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PIN(i)     ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PIN(i)     ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PIN(i)     ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PIN(i)       ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI3 location number i */
#define AF_USART0_TX_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_USART0_TX location number i */
#define AF_USART0_RX_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_USART1_TX location number i */
#define AF_USART1_RX_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_USART1_RTS location number i */
#define AF_LEUART0_TX_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_LEUART0_RX location number i */
#define AF_LETIMER0_OUT0_PIN(i)    ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PIN(i)    ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_LETIMER0_OUT1 location number i */
#define AF_I2C0_SDA_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_I2C0_SCL location number i */
#define AF_DBG_TDI_PIN(i)          ((i) == 0 ? 3 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PIN(i)          ((i) == 0 ? 2 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 13 : (i) == 2 ? 15 : (i) == 3 ? 11 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PIN(i)     ((i) == 0 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PIN(i)     ((i) == 0 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWCLKTCK location number i */

/** @} */
/** @} End of group EFR32FG1V232F256GM48_AF_Pins */

/***************************************************************************//**
 *  @brief Set the value of a bit field within a register.
 *
 *  @param REG
 *       The register to update
 *  @param MASK
 *       The mask for the bit field to update
 *  @param VALUE
 *       The value to write to the bit field
 *  @param OFFSET
 *       The number of bits that the field is offset within the register.
 *       0 (zero) means LSB.
 ******************************************************************************/
#define SET_BIT_FIELD(REG, MASK, VALUE, OFFSET) \
  REG = ((REG) &~(MASK)) | (((VALUE) << (OFFSET)) & (MASK));

/** @} End of group EFR32FG1V232F256GM48 */

/** @} End of group Parts */

#ifdef __cplusplus
}
#endif

#endif /* EFR32FG1V232F256GM48_H */
