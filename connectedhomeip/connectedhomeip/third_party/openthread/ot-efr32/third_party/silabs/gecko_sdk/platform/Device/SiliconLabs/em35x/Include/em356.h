/**************************************************************************//**
* @file     em356.h
* @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Header File
*           for em356
* @version 5.8.3
* @date     23. November 2012
*
* @note
*
******************************************************************************
* @section License
* <b>(C) Copyright 2016 Silicon Labs, www.silabs.com</b>
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

#ifndef EM356_H
#define EM356_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @addtogroup Parts
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup EM356 EM356
 * @{
 *****************************************************************************/

/** Interrupt Number Definition */
typedef enum IRQn{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn   = -14,      /*!<  2 Non Maskable Interrupt                                */
  MemoryManagement_IRQn = -12,      /*!<  4 Cortex-M3 Memory Management Interrupt                 */
  BusFault_IRQn         = -11,      /*!<  5 Cortex-M3 Bus Fault Interrupt                         */
  UsageFault_IRQn       = -10,      /*!<  6 Cortex-M3 Usage Fault Interrupt                       */
  SVCall_IRQn           = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                           */
  DebugMonitor_IRQn     = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt                     */
  PendSV_IRQn           = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                           */
  SysTick_IRQn          = -1,       /*!< 15 Cortex-M3 System Tick Interrupt                       */

/******  EM35x Peripheral Interrupt Numbers ******************************************************/
  TIM1_IRQn             = 0,  /*!< 16+0  EM35x TIM1 Interrupt */
  TIM2_IRQn             = 1,  /*!< 16+1  EM35x TIM2 Interrupt */
  MGMT_IRQn             = 2,  /*!< 16+2  EM35x MGMT Interrupt */
  BB_IRQn               = 3,  /*!< 16+3  EM35x BB Interrupt */
  SLEEPTMR_IRQn         = 4,  /*!< 16+4  EM35x SLEEPTMR Interrupt */
  SC1_IRQn              = 5,  /*!< 16+5  EM35x SC1 Interrupt */
  SC2_IRQn              = 6,  /*!< 16+6  EM35x SC2 Interrupt */
  AESCCM_IRQn           = 7,  /*!< 16+7  EM35x AESCCM Interrupt */
  MACTMR_IRQn           = 8,  /*!< 16+8  EM35x MACTMR Interrupt */
  MACTX_IRQn            = 9,  /*!< 16+9  EM35x MACTX Interrupt */
  MACRX_IRQn            = 10, /*!< 16+10 EM35x MACRX Interrupt */
  ADC_IRQn              = 11, /*!< 16+11 EM35x ADC Interrupt */
  IRQA_IRQn             = 12, /*!< 16+12 EM35x IRQA Interrupt */
  IRQB_IRQn             = 13, /*!< 16+13 EM35x IRQB Interrupt */
  IRQC_IRQn             = 14, /*!< 16+14 EM35x IRQC Interrupt */
  IRQD_IRQn             = 15, /*!< 16+15 EM35x IRQD Interrupt */
  DEBUG_IRQn            = 16, /*!< 16+16 EM35x DEBUG Interrupt */
} IRQn_Type;

/**************************************************************************//**
 * @defgroup EM356_Core EM356 Core
 * @{
 * @brief Processor and Core Peripheral Section
 *****************************************************************************/
#define __CM3_REV                0x0101 /*!< Core Revision r1p1 */
#define __MPU_PRESENT            1 /*!< MPU present */
#define __VTOR_PRESENT           1 /*!< Presence of VTOR register in SCB */
#define __NVIC_PRIO_BITS         5 /*!< Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig   0 /*!< Set to 1 if different SysTick Config is used */

#if defined(MPU_BASE)
//The original, non CMSIS Device register header defined MPU_BASE.
//That definition conflicts with the CMSIS definition in Device.
//By undefining the other version, the inclusion of the CMSIS definition
//will work cleanly.
//Long term the other usage will go away so this undef can be removed.
  #undef MPU_BASE
#endif

/** @} End of group EM356_Core */

/**************************************************************************//**
* @defgroup EM356_Part EM356 Part
* @{
******************************************************************************/

/** Part family */
#define _EM35x_FAMILY  1

/* If part number is not defined as compiler option, define it */
#if !defined(EM356)
#define EM356    1
#endif

/** Configure part number */
#define PART_NUMBER          "EM356" /**< Part Number */

/** Memory map for EM356 */
#define RAM_BOTTOM          (0x20000000UL)
#define RAM_SIZE_B          (0x00003000UL)
#define RAM_SIZE_W          (RAM_SIZE_B / 4)
#define RAM_TOP             (RAM_BOTTOM + RAM_SIZE_B - 1)

#define MFB_BOTTOM          (0x08000000UL)
#define MFB_SIZE_B          (0x00030000UL)
#define MFB_SIZE_W          (MFB_SIZE_B / 4)
#define MFB_TOP             (MFB_BOTTOM + MFB_SIZE_B - 1)
#define MFB_PAGE_SIZE_B     (2048)
#define MFB_PAGE_SIZE_W     (MFB_PAGE_SIZE_B / 4)
#define MFB_PAGE_MASK_B     (0xFFFFF800UL)
#define MFB_REGION_SIZE     (4)  /*!< Size of a write protect region in pages */
#define MFB_ADDR_MASK       (0x0003FFFFUL)

#define CIB_BOTTOM          (0x08040800UL)
#define CIB_SIZE_B          (2048)
#define CIB_SIZE_W          (CIB_SIZE_B / 4)
#define CIB_TOP             (CIB_BOTTOM + CIB_SIZE_B - 1)
#define CIB_PAGE_SIZE_B     (2048)
#define CIB_PAGE_SIZE_W     (CIB_PAGE_SIZE_B / 4)
#define CIB_OB_BOTTOM       (CIB_BOTTOM + 0x00)   /*!< bottom address of CIB option bytes */
#define CIB_OB_TOP          (CIB_BOTTOM + 0x0F)   /*!< top address of CIB option bytes */

#define FIB_BOTTOM          (0x08040000UL)
#define FIB_SIZE_B          (2048)
#define FIB_SIZE_W          (FIB_SIZE_B / 4)
#define FIB_TOP             (FIB_BOTTOM + FIB_SIZE_B - 1)
#define FIB_PAGE_SIZE_B     (2048)
#define FIB_PAGE_SIZE_W     (FIB_PAGE_SIZE_B / 4)

#define FPEC_KEY1           0x45670123UL          /*!< magic key defined in hardware */
#define FPEC_KEY2           0xCDEF89ABUL          /*!< magic key defined in hardware */

/* Default bootloader size in bytes */
#define DEFAULT_BTL_SIZE_B (MFB_REGION_SIZE * MFB_PAGE_SIZE_B)

/* Size in bytes that one RAM retention bit covers */
#define RAM_RETAIN_BLOCK_SIZE (0)

/* Translation between page number and simee (word based) address */
#define SIMEE_ADDR_TO_PAGE(x)   ((uint8_t)(((uint16_t)(x)) >> 10))
#define PAGE_TO_SIMEE_ADDR(x)   (((uint16_t)(x)) << 10)

/* Translation between page number and code addresses, used by bootloaders */
#define PROG_ADDR_TO_PAGE(x)    ((uint8_t)((((uint32_t)(x)) & MFB_ADDR_MASK) >> 11))
#define PAGE_TO_PROG_ADDR(x)    ((((uint32_t)(x)) << 11) | MFB_BOTTOM)

/* Part number capabilities */
#define PORT_A_PIN_COUNT   7
#define PORT_A_PIN_MASK    0xBF // 7   5 4 3 2 1 0
#define PORT_B_PIN_COUNT   5
#define PORT_B_PIN_MASK    0x1F //       4 3 2 1 0
#define PORT_C_PIN_COUNT   8
#define PORT_C_PIN_MASK    0xFF // 7 6 5 4 3 2 1 0
#define SC_PRESENT
#define SC_COUNT           2
#define SC_MASK            0x03

#include <core_cm3.h>  /* Cortex-M3 processor and core peripherals */
#include "system_em35x.h"  /* System Header */

/*@}*/ /* end of group EM356_Part */

/**************************************************************************//**
 * @defgroup EM356_Peripheral_TypeDefs EM356 Peripheral TypeDefs
 * @{
 * @brief Device Specific Peripheral Register Structures
 *****************************************************************************/

#include "em35x_cmhv.h"
#include "em35x_baseband.h"
#include "em35x_mac.h"
#include "em35x_aesccm.h"
#include "em35x_cmlv.h"
#include "em35x_ramctrl.h"
#include "em35x_wdog.h"
#include "em35x_sleeptmr.h"
#include "em35x_calibrationadc.h"
#include "em35x_flashctrl.h"
#include "em35x_event_adc.h"
#include "em35x_event_bb.h"
#include "em35x_event_gpio.h"
#include "em35x_event_mac.h"
#include "em35x_event_mgmt.h"
#include "em35x_event_miss.h"
#include "em35x_event_nmi.h"
#include "em35x_event_sc12.h"
#include "em35x_event_aesccm.h"
#include "em35x_event_sleeptmr.h"
#include "em35x_event_tim.h"
#include "em35x_adc.h"
#include "em35x_gpio_p.h"
#include "em35x_gpio.h"
#include "em35x_sc.h"
#include "em35x_tim.h"

/*@}*/ /* end of group EM356_Peripheral_TypeDefs */

/**************************************************************************//**
 * @defgroup EM356_Peripheral_Base EM356 Peripheral Memory Map
 * @{
 *****************************************************************************/

#define CMHV_BASE            (0x40000000UL) /**< CMHV base address */
#define BASEBAND_BASE        (0x40001000UL) /**< BASEBAND base address */
#define MAC_BASE             (0x40002000UL) /**< MAC base address */
#define AESCCM_BASE          (0x40003000UL) /**< AESCCM base address */
#define CMLV_BASE            (0x40004000UL) /**< CMLV base address */
#define RAMCTRL_BASE         (0x40005000UL) /**< RAMCTRL base address */
#define WDOG_BASE            (0x40006000UL) /**< WDOG base address */
#define SLEEPTMR_BASE        (0x4000600CUL) /**< SLEEPTMR base address */
#define CALIBRATIONADC_BASE  (0x40007000UL) /**< Calibration ADC base address */
#define FLASHCTRL_BASE       (0x40008000UL) /**< FLASHCTRL base address */
#define EVENT_MAC_BASE       (0x4000A000UL) /**< Event Manager MAC base address */
#define EVENT_BB_BASE        (0x4000A00CUL) /**< Event Manager BB base address */
#define EVENT_AESCCM_BASE    (0x4000A010UL) /**< Event Manager AESCCM base address */
#define EVENT_SLEEPTMR_BASE  (0x4000A014UL) /**< Event Manager SLEEPTMR base address */
#define EVENT_MGMT_BASE      (0x4000A018UL) /**< Event Manager MGMT base address */
#define EVENT_NMI_BASE       (0x4000A01CUL) /**< Event Manager NMI base address */
#define EVENT_TIM1_BASE      (0x4000A800UL) /**< Event Manager TIM1 base address */
#define EVENT_TIM2_BASE      (0x4000A804UL) /**< Event Manager TIM2 base address */
#define EVENT_SC1_BASE       (0x4000A808UL) /**< Event Manager SC1 base address */
#define EVENT_SC2_BASE       (0x4000A80CUL) /**< Event Manager SC2 base address */
#define EVENT_ADC_BASE       (0x4000A810UL) /**< Event Manager ADC base address */
#define EVENT_GPIO_BASE      (0x4000A814UL) /**< Event Manager GPIO base address */
#define EVENT_MISS_BASE      (0x4000A820UL) /**< Event Manager MISS base address */
#define GPIO_BASE            (0x4000B000UL) /**< GPIO base address */
#define SC2_BASE             (0x4000C000UL) /**< SC2 base address */
#define SC1_BASE             (0x4000C800UL) /**< SC1 base address */
#define ADC_BASE             (0x4000D000UL) /**< ADC base address */
#define TIM1_BASE            (0x4000E000UL) /**< TIM1 base address */
#define TIM2_BASE            (0x4000F000UL) /**< TIM2 base address */

/** @} End of group EM356_Peripheral_Base */

/**************************************************************************//**
 * @defgroup EM356_Peripheral_Declaration  EM356 Peripheral Declarations
 * @{
 *****************************************************************************/

#define CMHV            ((CMHV_TypeDef *) CMHV_BASE)
#define BASEBAND        ((BASEBAND_TypeDef *) BASEBAND_BASE)
#define MAC             ((MAC_TypeDef *) MAC_BASE)
#define AESCCM          ((AESCCM_TypeDef *) AESCCM_BASE)
#define CMLV            ((CMLV_TypeDef *) CMLV_BASE)
#define RAMCTRL         ((RAMCTRL_TypeDef *) RAMCTRL_BASE)
#define WDOG            ((WDOG_TypeDef *) WDOG_BASE)
#define SLEEPTMR        ((SLEEPTMR_TypeDef *) SLEEPTMR_BASE)
#define CALIBRATIONADC  ((CALIBRATIONADC_TypeDef *) CALIBRATIONADC_BASE)
#define FLASHCTRL       ((FLASHCTRL_TypeDef *) FLASHCTRL_BASE)
#define EVENT_MAC       ((EVENT_MAC_TypeDef *) EVENT_MAC_BASE)
#define EVENT_BB        ((EVENT_BB_TypeDef *) EVENT_BB_BASE)
#define EVENT_AESCCM    ((EVENT_AESCCM_TypeDef *) EVENT_AESCCM_BASE)
#define EVENT_SLEEPTMR  ((EVENT_SLEEPTMR_TypeDef *) EVENT_SLEEPTMR_BASE)
#define EVENT_MGMT      ((EVENT_MGMT_TypeDef *) EVENT_MGMT_BASE)
#define EVENT_NMI       ((EVENT_NMI_TypeDef *) EVENT_NMI_BASE)
#define EVENT_TIM1      ((EVENT_TIM_TypeDef *) EVENT_TIM1_BASE)
#define EVENT_TIM2      ((EVENT_TIM_TypeDef *) EVENT_TIM2_BASE)
#define EVENT_SC1       ((EVENT_SC12_TypeDef *) EVENT_SC1_BASE)
#define EVENT_SC2       ((EVENT_SC12_TypeDef *) EVENT_SC2_BASE)
#define EVENT_ADC       ((EVENT_ADC_TypeDef *) EVENT_ADC_BASE)
#define EVENT_GPIO      ((EVENT_GPIO_TypeDef *) EVENT_GPIO_BASE)
#define EVENT_MISS      ((EVENT_MISS_TypeDef *) EVENT_MISS_BASE)
#define ADC             ((ADC_TypeDef *) ADC_BASE)
#define GPIO            ((GPIO_TypeDef *) GPIO_BASE)
#define SC2             ((SC_TypeDef *) SC2_BASE)
#define SC1             ((SC_TypeDef *) SC1_BASE)
#define TIM1            ((TIM_TypeDef *) TIM1_BASE)
#define TIM2            ((TIM_TypeDef *) TIM2_BASE)

/** @} End of group EM356_Peripheral_Declaration */

/**************************************************************************//**
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
 *****************************************************************************/
#define SET_BIT_FIELD(REG, MASK, VALUE, OFFSET) \
  REG = ((REG) &~(MASK)) | (((VALUE) << (OFFSET)) & (MASK));

/** @} End of group EM356  */

/** @} End of group Parts */

#ifdef __cplusplus
}
#endif

#endif /* EM356_H */
