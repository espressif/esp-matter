/***************************************************************************//**
 * @file
 * @brief Security Management Unit (SMU) peripheral API
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

#ifndef EM_SMU_H
#define EM_SMU_H

#include "em_device.h"
#if defined(SMU_COUNT) && (SMU_COUNT > 0)

#include "sl_assert.h"
#include "em_bus.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup smu SMU - Security Management Unit
 * @brief Security Management Unit (SMU) Peripheral API
 *
 * @details
 *   SMU forms the control and status/reporting component of bus-level
 *   security in EFM32/EFR32 devices.
 *
 *   Peripheral-level protection is provided via the Peripheral Protection Unit
 *   (PPU). PPU provides hardware access barrier to any peripheral that is
 *   configured to be protected. When an attempt is made to access a peripheral
 *   without the required privilege/security level, PPU detects the fault
 *   and intercepts the access. No write or read of the peripheral register
 *   space occurs, and an all-zero value is returned if the access is a read.
 *
 *   Usage example
 *   @include em_smu_init.c
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** SMU peripheral identifiers. */
typedef enum {
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_84)
  smuPeripheralACMP0      = _SMU_PPUPATD0_ACMP0_SHIFT,         /**< SMU peripheral identifier for ACMP0.     */
  smuPeripheralACMP1      = _SMU_PPUPATD0_ACMP1_SHIFT,         /**< SMU peripheral identifier for ACMP1.     */
  smuPeripheralADC0       = _SMU_PPUPATD0_ADC0_SHIFT,          /**< SMU peripheral identifier for ADC0.      */
  smuPeripheralCMU        = _SMU_PPUPATD0_CMU_SHIFT,           /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER  = _SMU_PPUPATD0_CRYOTIMER_SHIFT,     /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO0    = _SMU_PPUPATD0_CRYPTO0_SHIFT,       /**< SMU peripheral identifier for CRYPTO0.   */
  smuPeripheralCRYPTO1    = _SMU_PPUPATD0_CRYPTO1_SHIFT,       /**< SMU peripheral identifier for CRYPTO1.   */
  smuPeripheralCSEN       = _SMU_PPUPATD0_CSEN_SHIFT,          /**< SMU peripheral identifier for CSEN.      */
  smuPeripheralVDAC0      = _SMU_PPUPATD0_VDAC0_SHIFT,         /**< SMU peripheral identifier for VDAC0.     */
  smuPeripheralPRS        = _SMU_PPUPATD0_PRS_SHIFT,           /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEMU        = _SMU_PPUPATD0_EMU_SHIFT,           /**< SMU peripheral identifier for EMU.       */
  smuPeripheralFPUEH      = _SMU_PPUPATD0_FPUEH_SHIFT,         /**< SMU peripheral identifier for FPUEH.     */
  smuPeripheralGPCRC      = _SMU_PPUPATD0_GPCRC_SHIFT,         /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO       = _SMU_PPUPATD0_GPIO_SHIFT,          /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0       = _SMU_PPUPATD0_I2C0_SHIFT,          /**< SMU peripheral identifier for I2C0.      */
  smuPeripheralI2C1       = _SMU_PPUPATD0_I2C1_SHIFT,          /**< SMU peripheral identifier for I2C1.      */
  smuPeripheralIDAC0      = _SMU_PPUPATD0_IDAC0_SHIFT,         /**< SMU peripheral identifier for IDAC0.     */
  smuPeripheralMSC        = _SMU_PPUPATD0_MSC_SHIFT,           /**< SMU peripheral identifier for MSC.       */
  smuPeripheralLDMA       = _SMU_PPUPATD0_LDMA_SHIFT,          /**< SMU peripheral identifier for LDMA.      */
  smuPeripheralLESENSE    = _SMU_PPUPATD0_LESENSE_SHIFT,       /**< SMU peripheral identifier for LESENSE.   */
  smuPeripheralLETIMER0   = _SMU_PPUPATD0_LETIMER0_SHIFT,      /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLEUART0    = _SMU_PPUPATD0_LEUART0_SHIFT,       /**< SMU peripheral identifier for LEUART0.   */
  smuPeripheralPCNT0      = _SMU_PPUPATD0_PCNT0_SHIFT,         /**< SMU peripheral identifier for PCNT0.     */
  smuPeripheralPCNT1      = _SMU_PPUPATD0_PCNT1_SHIFT,         /**< SMU peripheral identifier for PCNT1.     */
  smuPeripheralPCNT2      = _SMU_PPUPATD0_PCNT2_SHIFT,         /**< SMU peripheral identifier for PCNT2.     */
  smuPeripheralRMU        = 32 + _SMU_PPUPATD1_RMU_SHIFT,      /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTCC       = 32 + _SMU_PPUPATD1_RTCC_SHIFT,     /**< SMU peripheral identifier for RTCC.      */
  smuPeripheralSMU        = 32 + _SMU_PPUPATD1_SMU_SHIFT,      /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0     = 32 + _SMU_PPUPATD1_TIMER0_SHIFT,   /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1     = 32 + _SMU_PPUPATD1_TIMER1_SHIFT,   /**< SMU peripheral identifier for TIMER1.    */
  smuPeripheralTRNG0      = 32 + _SMU_PPUPATD1_TRNG0_SHIFT,    /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUSART0     = 32 + _SMU_PPUPATD1_USART0_SHIFT,   /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1     = 32 + _SMU_PPUPATD1_USART1_SHIFT,   /**< SMU peripheral identifier for USART1.    */
  smuPeripheralUSART2     = 32 + _SMU_PPUPATD1_USART2_SHIFT,   /**< SMU peripheral identifier for USART2.    */
  smuPeripheralUSART3     = 32 + _SMU_PPUPATD1_USART3_SHIFT,   /**< SMU peripheral identifier for USART3.    */
  smuPeripheralWDOG0      = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,    /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWDOG1      = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,    /**< SMU peripheral identifier for WDOG1.     */
  smuPeripheralWTIMER0    = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,  /**< SMU peripheral identifier for WTIMER0.   */
  smuPeripheralWTIMER1    = 32 + _SMU_PPUPATD1_WTIMER1_SHIFT,  /**< SMU peripheral identifier for WTIMER1.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
  smuPeripheralACMP0      = _SMU_PPUPATD0_ACMP0_SHIFT,         /**< SMU peripheral identifier for ACMP0.     */
  smuPeripheralACMP1      = _SMU_PPUPATD0_ACMP1_SHIFT,         /**< SMU peripheral identifier for ACMP1.     */
  smuPeripheralADC0       = _SMU_PPUPATD0_ADC0_SHIFT,          /**< SMU peripheral identifier for ADC0.      */
  smuPeripheralCMU        = _SMU_PPUPATD0_CMU_SHIFT,           /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER  = _SMU_PPUPATD0_CRYOTIMER_SHIFT,     /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO0    = _SMU_PPUPATD0_CRYPTO0_SHIFT,       /**< SMU peripheral identifier for CRYPTO0.   */
  smuPeripheralCRYPTO1    = _SMU_PPUPATD0_CRYPTO1_SHIFT,       /**< SMU peripheral identifier for CRYPTO1.   */
#if defined(_SMU_PPUPATD0_CSEN_SHIFT)
  smuPeripheralCSEN       = _SMU_PPUPATD0_CSEN_SHIFT,          /**< SMU peripheral identifier for CSEN.      */
#endif
#if defined(_SMU_PPUPATD0_VDAC0_SHIFT)
  smuPeripheralVDAC0      = _SMU_PPUPATD0_VDAC0_SHIFT,         /**< SMU peripheral identifier for VDAC0.     */
#endif
  smuPeripheralPRS        = _SMU_PPUPATD0_PRS_SHIFT,           /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEMU        = _SMU_PPUPATD0_EMU_SHIFT,           /**< SMU peripheral identifier for EMU.       */
  smuPeripheralFPUEH      = _SMU_PPUPATD0_FPUEH_SHIFT,         /**< SMU peripheral identifier for FPUEH.     */
  smuPeripheralGPCRC      = _SMU_PPUPATD0_GPCRC_SHIFT,         /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO       = _SMU_PPUPATD0_GPIO_SHIFT,          /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0       = _SMU_PPUPATD0_I2C0_SHIFT,          /**< SMU peripheral identifier for I2C0.      */
  smuPeripheralI2C1       = _SMU_PPUPATD0_I2C1_SHIFT,          /**< SMU peripheral identifier for I2C1.      */
#if defined(_SMU_PPUPATD0_IDAC0_SHIFT)
  smuPeripheralIDAC0      = _SMU_PPUPATD0_IDAC0_SHIFT,         /**< SMU peripheral identifier for IDAC0.     */
#endif
  smuPeripheralMSC        = _SMU_PPUPATD0_MSC_SHIFT,           /**< SMU peripheral identifier for MSC.       */
  smuPeripheralLDMA       = _SMU_PPUPATD0_LDMA_SHIFT,          /**< SMU peripheral identifier for LDMA.      */
  smuPeripheralLESENSE    = _SMU_PPUPATD0_LESENSE_SHIFT,       /**< SMU peripheral identifier for LESENSE.   */
  smuPeripheralLETIMER0   = _SMU_PPUPATD0_LETIMER0_SHIFT,      /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLEUART0    = _SMU_PPUPATD0_LEUART0_SHIFT,       /**< SMU peripheral identifier for LEUART0.   */
  smuPeripheralPCNT0      = _SMU_PPUPATD0_PCNT0_SHIFT,         /**< SMU peripheral identifier for PCNT0.     */
  smuPeripheralRMU        = 32 + _SMU_PPUPATD1_RMU_SHIFT,      /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTCC       = 32 + _SMU_PPUPATD1_RTCC_SHIFT,     /**< SMU peripheral identifier for RTCC.      */
  smuPeripheralSMU        = 32 + _SMU_PPUPATD1_SMU_SHIFT,      /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0     = 32 + _SMU_PPUPATD1_TIMER0_SHIFT,   /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1     = 32 + _SMU_PPUPATD1_TIMER1_SHIFT,   /**< SMU peripheral identifier for TIMER1.    */
  smuPeripheralTRNG0      = 32 + _SMU_PPUPATD1_TRNG0_SHIFT,    /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUSART0     = 32 + _SMU_PPUPATD1_USART0_SHIFT,   /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1     = 32 + _SMU_PPUPATD1_USART1_SHIFT,   /**< SMU peripheral identifier for USART1.    */
  smuPeripheralUSART2     = 32 + _SMU_PPUPATD1_USART2_SHIFT,   /**< SMU peripheral identifier for USART2.    */
  smuPeripheralWDOG0      = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,    /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWDOG1      = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,    /**< SMU peripheral identifier for WDOG1.     */
  smuPeripheralWTIMER0    = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,  /**< SMU peripheral identifier for WTIMER0.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
#if defined(_SMU_PPUPATD0_ACMP0_SHIFT)
  smuPeripheralACMP0      = _SMU_PPUPATD0_ACMP0_SHIFT,         /**< SMU peripheral identifier for ACMP0.     */
#endif
#if defined(_SMU_PPUPATD0_ACMP1_SHIFT)
  smuPeripheralACMP1      = _SMU_PPUPATD0_ACMP1_SHIFT,         /**< SMU peripheral identifier for ACMP1.     */
#endif
#if defined(_SMU_PPUPATD0_ADC0_SHIFT)
  smuPeripheralADC0       = _SMU_PPUPATD0_ADC0_SHIFT,          /**< SMU peripheral identifier for ADC0.      */
#endif
  smuPeripheralCMU        = _SMU_PPUPATD0_CMU_SHIFT,           /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER  = _SMU_PPUPATD0_CRYOTIMER_SHIFT,     /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO     = _SMU_PPUPATD0_CRYPTO0_SHIFT,       /**< SMU peripheral identifier for CRYPTO0.   */
#if defined(_SMU_PPUPATD0_VDAC0_SHIFT)
  smuPeripheralVDAC0      = _SMU_PPUPATD0_VDAC0_SHIFT,         /**< SMU peripheral identifier for VDAC0.     */
#endif
  smuPeripheralPRS        = _SMU_PPUPATD0_PRS_SHIFT,           /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEMU        = _SMU_PPUPATD0_EMU_SHIFT,           /**< SMU peripheral identifier for EMU.       */
  smuPeripheralFPUEH      = _SMU_PPUPATD0_FPUEH_SHIFT,         /**< SMU peripheral identifier for FPUEH.     */
  smuPeripheralGPCRC      = _SMU_PPUPATD0_GPCRC_SHIFT,         /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO       = _SMU_PPUPATD0_GPIO_SHIFT,          /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0       = _SMU_PPUPATD0_I2C0_SHIFT,          /**< SMU peripheral identifier for I2C0.      */
#if defined(_SMU_PPUPATD0_IDAC0_SHIFT)
  smuPeripheralIDAC0      = _SMU_PPUPATD0_IDAC0_SHIFT,         /**< SMU peripheral identifier for IDAC0.     */
#endif
  smuPeripheralMSC        = _SMU_PPUPATD0_MSC_SHIFT,           /**< SMU peripheral identifier for MSC.       */
  smuPeripheralLDMA       = _SMU_PPUPATD0_LDMA_SHIFT,          /**< SMU peripheral identifier for LDMA.      */
#if defined(_SMU_PPUPATD0_LESENSE_SHIFT)
  smuPeripheralLESENSE    = _SMU_PPUPATD0_LESENSE_SHIFT,       /**< SMU peripheral identifier for LESENSE.   */
#endif
  smuPeripheralLETIMER0   = _SMU_PPUPATD0_LETIMER0_SHIFT,      /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLEUART     = _SMU_PPUPATD0_LEUART0_SHIFT,       /**< SMU peripheral identifier for LEUART0.   */
#if defined(_SMU_PPUPATD0_PCNT0_SHIFT)
  smuPeripheralPCNT0      = _SMU_PPUPATD0_PCNT0_SHIFT,         /**< SMU peripheral identifier for PCNT0.     */
#endif
  smuPeripheralRMU        = _SMU_PPUPATD0_RMU_SHIFT,           /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTCC       = _SMU_PPUPATD0_RTCC_SHIFT,          /**< SMU peripheral identifier for RTCC.      */
  smuPeripheralSMU        = _SMU_PPUPATD0_SMU_SHIFT,           /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0     = 32 + _SMU_PPUPATD1_TIMER0_SHIFT,   /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1     = 32 + _SMU_PPUPATD1_TIMER1_SHIFT,   /**< SMU peripheral identifier for TIMER1.    */
  smuPeripheralTRNG0      = 32 + _SMU_PPUPATD1_TRNG0_SHIFT,    /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUSART0     = 32 + _SMU_PPUPATD1_USART0_SHIFT,   /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1     = 32 + _SMU_PPUPATD1_USART1_SHIFT,   /**< SMU peripheral identifier for USART1.    */
  smuPeripheralWDOG0      = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,    /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWDOG1      = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,    /**< SMU peripheral identifier for WDOG1.     */
  smuPeripheralWTIMER0    = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,  /**< SMU peripheral identifier for WTIMER0.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  smuPeripheralACMP0     = _SMU_PPUPATD0_ACMP0_SHIFT,          /**< SMU peripheral identifier for ACMP0.     */
  smuPeripheralACMP1     = _SMU_PPUPATD0_ACMP1_SHIFT,          /**< SMU peripheral identifier for ACMP1.     */
  smuPeripheralACMP2     = _SMU_PPUPATD0_ACMP2_SHIFT,          /**< SMU peripheral identifier for ACMP2.     */
  smuPeripheralACMP3     = _SMU_PPUPATD0_ACMP3_SHIFT,          /**< SMU peripheral identifier for ACMP3.     */
  smuPeripheralADC0      = _SMU_PPUPATD0_ADC0_SHIFT,           /**< SMU peripheral identifier for ADC0.      */
  smuPeripheralADC1      = _SMU_PPUPATD0_ADC1_SHIFT,           /**< SMU peripheral identifier for ADC1.      */
  smuPeripheralCAN0      = _SMU_PPUPATD0_CAN0_SHIFT,           /**< SMU peripheral identifier for CAN0.      */
  smuPeripheralCAN1      = _SMU_PPUPATD0_CAN1_SHIFT,           /**< SMU peripheral identifier for CAN1.      */
  smuPeripheralCMU       = _SMU_PPUPATD0_CMU_SHIFT,            /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER = _SMU_PPUPATD0_CRYOTIMER_SHIFT,      /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO0   = _SMU_PPUPATD0_CRYPTO0_SHIFT,        /**< SMU peripheral identifier for CRYPTO0.   */
  smuPeripheralCSEN      = _SMU_PPUPATD0_CSEN_SHIFT,           /**< SMU peripheral identifier for CSEN.      */
  smuPeripheralVDAC0     = _SMU_PPUPATD0_VDAC0_SHIFT,          /**< SMU peripheral identifier for VDAC0.     */
  smuPeripheralPRS       = _SMU_PPUPATD0_PRS_SHIFT,            /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEBI       = _SMU_PPUPATD0_EBI_SHIFT,            /**< SMU peripheral identifier for EBI.       */
  smuPeripheralEMU       = _SMU_PPUPATD0_EMU_SHIFT,            /**< SMU peripheral identifier for EMU.       */
#if defined(_SMU_PPUPATD0_ETH_SHIFT)
  smuPeripheralETH       = _SMU_PPUPATD0_ETH_SHIFT,            /**< SMU peripheral identifier for ETH.       */
#endif
  smuPeripheralFPUEH     = _SMU_PPUPATD0_FPUEH_SHIFT,          /**< SMU peripheral identifier for FPUEH.     */
  smuPeripheralGPCRC     = _SMU_PPUPATD0_GPCRC_SHIFT,          /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO      = _SMU_PPUPATD0_GPIO_SHIFT,           /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0      = _SMU_PPUPATD0_I2C0_SHIFT,           /**< SMU peripheral identifier for I2C0.      */
  smuPeripheralI2C1      = _SMU_PPUPATD0_I2C1_SHIFT,           /**< SMU peripheral identifier for I2C1.      */
  smuPeripheralI2C2      = _SMU_PPUPATD0_I2C2_SHIFT,           /**< SMU peripheral identifier for I2C2.      */
  smuPeripheralIDAC0     = _SMU_PPUPATD0_IDAC0_SHIFT,          /**< SMU peripheral identifier for IDAC0.     */
  smuPeripheralMSC       = _SMU_PPUPATD0_MSC_SHIFT,            /**< SMU peripheral identifier for MAC.       */
#if defined(_SMU_PPUPATD0_LCD_SHIFT)
  smuPeripheralLCD       = _SMU_PPUPATD0_LCD_SHIFT,            /**< SMU peripheral identifier for LCD.       */
#endif
  smuPeripheralLDMA      = _SMU_PPUPATD0_LDMA_SHIFT,           /**< SMU peripheral identifier for LDMA.      */
  smuPeripheralLESENSE   = _SMU_PPUPATD0_LESENSE_SHIFT,        /**< SMU peripheral identifier for LESENSE.   */
  smuPeripheralLETIMER0  = _SMU_PPUPATD0_LETIMER0_SHIFT,       /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLETIMER1  = _SMU_PPUPATD0_LETIMER1_SHIFT,       /**< SMU peripheral identifier for LETIMER1.  */
  smuPeripheralLEUART0   = _SMU_PPUPATD0_LEUART0_SHIFT,        /**< SMU peripheral identifier for LEUART0.   */
  smuPeripheralLEUART1   = _SMU_PPUPATD0_LEUART1_SHIFT,        /**< SMU peripheral identifier for LEUART1.   */
  smuPeripheralPCNT0     = 32 + _SMU_PPUPATD1_PCNT0_SHIFT,     /**< SMU peripheral identifier for PCNT0.     */
  smuPeripheralPCNT1     = 32 + _SMU_PPUPATD1_PCNT1_SHIFT,     /**< SMU peripheral identifier for PCNT1.     */
  smuPeripheralPCNT2     = 32 + _SMU_PPUPATD1_PCNT2_SHIFT,     /**< SMU peripheral identifier for PCNT2.     */
#if defined(_SMU_PPUPATD1_QSPI0_SHIFT)
  smuPeripheralQSPI0     = 32 + _SMU_PPUPATD1_QSPI0_SHIFT,     /**< SMU peripheral identifier for QSPI0.     */
#endif
  smuPeripheralRMU       = 32 + _SMU_PPUPATD1_RMU_SHIFT,       /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTC       = 32 + _SMU_PPUPATD1_RTC_SHIFT,       /**< SMU peripheral identifier for RTC.       */
  smuPeripheralRTCC      = 32 + _SMU_PPUPATD1_RTCC_SHIFT,      /**< SMU peripheral identifier for RTCC.      */
#if defined(_SMU_PPUPATD1_SDIO_SHIFT)
  smuPeripheralSDIO      = 32 + _SMU_PPUPATD1_SDIO_SHIFT,      /**< SMU peripheral identifier for SDIO.      */
#endif
  smuPeripheralSMU       = 32 + _SMU_PPUPATD1_SMU_SHIFT,       /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0    = 32 + _SMU_PPUPATD1_TIMER0_SHIFT,    /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1    = 32 + _SMU_PPUPATD1_TIMER1_SHIFT,    /**< SMU peripheral identifier for TIMER1.    */
  smuPeripheralTIMER2    = 32 + _SMU_PPUPATD1_TIMER2_SHIFT,    /**< SMU peripheral identifier for TIMER2.    */
  smuPeripheralTIMER3    = 32 + _SMU_PPUPATD1_TIMER3_SHIFT,    /**< SMU peripheral identifier for TIMER3.    */
  smuPeripheralTIMER4    = 32 + _SMU_PPUPATD1_TIMER4_SHIFT,    /**< SMU peripheral identifier for TIMER4.    */
  smuPeripheralTIMER5    = 32 + _SMU_PPUPATD1_TIMER5_SHIFT,    /**< SMU peripheral identifier for TIMER5.    */
  smuPeripheralTIMER6    = 32 + _SMU_PPUPATD1_TIMER6_SHIFT,    /**< SMU peripheral identifier for TIMER6.    */
  smuPeripheralTRNG0     = 32 + _SMU_PPUPATD1_TRNG0_SHIFT,     /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUART0     = 32 + _SMU_PPUPATD1_UART0_SHIFT,     /**< SMU peripheral identifier for UART0.     */
  smuPeripheralUART1     = 32 + _SMU_PPUPATD1_UART1_SHIFT,     /**< SMU peripheral identifier for UART1.     */
  smuPeripheralUSART0    = 32 + _SMU_PPUPATD1_USART0_SHIFT,    /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1    = 32 + _SMU_PPUPATD1_USART1_SHIFT,    /**< SMU peripheral identifier for USART1.    */
  smuPeripheralUSART2    = 32 + _SMU_PPUPATD1_USART2_SHIFT,    /**< SMU peripheral identifier for USART2.    */
  smuPeripheralUSART3    = 32 + _SMU_PPUPATD1_USART3_SHIFT,    /**< SMU peripheral identifier for USART3.    */
  smuPeripheralUSART4    = 32 + _SMU_PPUPATD1_USART4_SHIFT,    /**< SMU peripheral identifier for USART4.    */
  smuPeripheralUSART5    = 32 + _SMU_PPUPATD1_USART5_SHIFT,    /**< SMU peripheral identifier for USART5.    */
#if defined(_SMU_PPUPATD1_USB_SHIFT)
  smuPeripheralUSB       = 32 + _SMU_PPUPATD1_USB_SHIFT,       /**< SMU peripheral identifier for USB.       */
#endif
  smuPeripheralWDOG0     = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,     /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWDOG1     = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,     /**< SMU peripheral identifier for WDOG1.     */
  smuPeripheralWTIMER0   = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,   /**< SMU peripheral identifier for WTIMER0.   */
  smuPeripheralWTIMER1   = 32 + _SMU_PPUPATD1_WTIMER1_SHIFT,   /**< SMU peripheral identifier for WTIMER1.   */
  smuPeripheralWTIMER2   = 32 + _SMU_PPUPATD1_WTIMER2_SHIFT,   /**< SMU peripheral identifier for WTIMER2.   */
  smuPeripheralWTIMER3   = 32 + _SMU_PPUPATD1_WTIMER3_SHIFT,   /**< SMU peripheral identifier for WTIMER3.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_106)
  smuPeripheralACMP0     = _SMU_PPUPATD0_ACMP0_SHIFT,          /**< SMU peripheral identifier for ACMP0.     */
  smuPeripheralACMP1     = _SMU_PPUPATD0_ACMP1_SHIFT,          /**< SMU peripheral identifier for ACMP1.     */
  smuPeripheralACMP2     = _SMU_PPUPATD0_ACMP2_SHIFT,          /**< SMU peripheral identifier for ACMP2.     */
  smuPeripheralADC0      = _SMU_PPUPATD0_ADC0_SHIFT,           /**< SMU peripheral identifier for ADC0.      */
  smuPeripheralADC1      = _SMU_PPUPATD0_ADC1_SHIFT,           /**< SMU peripheral identifier for ADC1.      */
  smuPeripheralCAN0      = _SMU_PPUPATD0_CAN0_SHIFT,           /**< SMU peripheral identifier for CAN0.      */
  smuPeripheralCAN1      = _SMU_PPUPATD0_CAN1_SHIFT,           /**< SMU peripheral identifier for CAN1.      */
  smuPeripheralCMU       = _SMU_PPUPATD0_CMU_SHIFT,            /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER = _SMU_PPUPATD0_CRYOTIMER_SHIFT,      /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO0   = _SMU_PPUPATD0_CRYPTO0_SHIFT,        /**< SMU peripheral identifier for CRYPTO0.   */
  smuPeripheralCSEN      = _SMU_PPUPATD0_CSEN_SHIFT,           /**< SMU peripheral identifier for CSEN.      */
  smuPeripheralVDAC0     = _SMU_PPUPATD0_VDAC0_SHIFT,          /**< SMU peripheral identifier for VDAC0.     */
  smuPeripheralPRS       = _SMU_PPUPATD0_PRS_SHIFT,            /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEBI       = _SMU_PPUPATD0_EBI_SHIFT,            /**< SMU peripheral identifier for EBI.       */
  smuPeripheralEMU       = _SMU_PPUPATD0_EMU_SHIFT,            /**< SMU peripheral identifier for EMU.       */
#if defined(_SMU_PPUPATD0_ETH_SHIFT)
  smuPeripheralETH       = _SMU_PPUPATD0_ETH_SHIFT,            /**< SMU peripheral identifier for ETH.       */
#endif
  smuPeripheralFPUEH     = _SMU_PPUPATD0_FPUEH_SHIFT,          /**< SMU peripheral identifier for FPUEH.     */
  smuPeripheralGPCRC     = _SMU_PPUPATD0_GPCRC_SHIFT,          /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO      = _SMU_PPUPATD0_GPIO_SHIFT,           /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0      = _SMU_PPUPATD0_I2C0_SHIFT,           /**< SMU peripheral identifier for I2C0.      */
  smuPeripheralI2C1      = _SMU_PPUPATD0_I2C1_SHIFT,           /**< SMU peripheral identifier for I2C1.      */
  smuPeripheralIDAC0     = _SMU_PPUPATD0_IDAC0_SHIFT,          /**< SMU peripheral identifier for IDAC0.     */
  smuPeripheralMSC       = _SMU_PPUPATD0_MSC_SHIFT,            /**< SMU peripheral identifier for MAC.       */
#if defined(_SMU_PPUPATD0_LCD_SHIFT)
  smuPeripheralLCD       = _SMU_PPUPATD0_LCD_SHIFT,            /**< SMU peripheral identifier for LCD.       */
#endif
  smuPeripheralLDMA      = _SMU_PPUPATD0_LDMA_SHIFT,           /**< SMU peripheral identifier for LDMA.      */
  smuPeripheralLESENSE   = _SMU_PPUPATD0_LESENSE_SHIFT,        /**< SMU peripheral identifier for LESENSE.   */
  smuPeripheralLETIMER0  = _SMU_PPUPATD0_LETIMER0_SHIFT,       /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLETIMER1  = _SMU_PPUPATD0_LETIMER1_SHIFT,       /**< SMU peripheral identifier for LETIMER1.  */
  smuPeripheralLEUART0   = _SMU_PPUPATD0_LEUART0_SHIFT,        /**< SMU peripheral identifier for LEUART0.   */
  smuPeripheralLEUART1   = _SMU_PPUPATD0_LEUART1_SHIFT,        /**< SMU peripheral identifier for LEUART1.   */
  smuPeripheralPCNT0     = 32 + _SMU_PPUPATD1_PCNT0_SHIFT,     /**< SMU peripheral identifier for PCNT0.     */
  smuPeripheralPCNT1     = 32 + _SMU_PPUPATD1_PCNT1_SHIFT,     /**< SMU peripheral identifier for PCNT1.     */
  smuPeripheralPCNT2     = 32 + _SMU_PPUPATD1_PCNT2_SHIFT,     /**< SMU peripheral identifier for PCNT2.     */
#if defined(_SMU_PPUPATD1_QSPI0_SHIFT)
  smuPeripheralQSPI0     = 32 + _SMU_PPUPATD1_QSPI0_SHIFT,     /**< SMU peripheral identifier for QSPI0.     */
#endif
  smuPeripheralRMU       = 32 + _SMU_PPUPATD1_RMU_SHIFT,       /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTC       = 32 + _SMU_PPUPATD1_RTC_SHIFT,       /**< SMU peripheral identifier for RTC.       */
  smuPeripheralRTCC      = 32 + _SMU_PPUPATD1_RTCC_SHIFT,      /**< SMU peripheral identifier for RTCC.      */
#if defined(_SMU_PPUPATD1_SDIO_SHIFT)
  smuPeripheralSDIO      = 32 + _SMU_PPUPATD1_SDIO_SHIFT,      /**< SMU peripheral identifier for SDIO.      */
#endif
  smuPeripheralSMU       = 32 + _SMU_PPUPATD1_SMU_SHIFT,       /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0    = 32 + _SMU_PPUPATD1_TIMER0_SHIFT,    /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1    = 32 + _SMU_PPUPATD1_TIMER1_SHIFT,    /**< SMU peripheral identifier for TIMER1.    */
  smuPeripheralTIMER2    = 32 + _SMU_PPUPATD1_TIMER2_SHIFT,    /**< SMU peripheral identifier for TIMER2.    */
  smuPeripheralTIMER3    = 32 + _SMU_PPUPATD1_TIMER3_SHIFT,    /**< SMU peripheral identifier for TIMER3.    */
  smuPeripheralTRNG0     = 32 + _SMU_PPUPATD1_TRNG0_SHIFT,     /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUART0     = 32 + _SMU_PPUPATD1_UART0_SHIFT,     /**< SMU peripheral identifier for UART0.     */
  smuPeripheralUART1     = 32 + _SMU_PPUPATD1_UART1_SHIFT,     /**< SMU peripheral identifier for UART1.     */
  smuPeripheralUSART0    = 32 + _SMU_PPUPATD1_USART0_SHIFT,    /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1    = 32 + _SMU_PPUPATD1_USART1_SHIFT,    /**< SMU peripheral identifier for USART1.    */
  smuPeripheralUSART2    = 32 + _SMU_PPUPATD1_USART2_SHIFT,    /**< SMU peripheral identifier for USART2.    */
  smuPeripheralUSART3    = 32 + _SMU_PPUPATD1_USART3_SHIFT,    /**< SMU peripheral identifier for USART3.    */
  smuPeripheralUSART4    = 32 + _SMU_PPUPATD1_USART4_SHIFT,    /**< SMU peripheral identifier for USART4.    */
#if defined(_SMU_PPUPATD1_USB_SHIFT)
  smuPeripheralUSB       = 32 + _SMU_PPUPATD1_USB_SHIFT,       /**< SMU peripheral identifier for USB.       */
#endif
  smuPeripheralWDOG0     = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,     /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWDOG1     = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,     /**< SMU peripheral identifier for WDOG1.     */
  smuPeripheralWTIMER0   = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,   /**< SMU peripheral identifier for WTIMER0.   */
  smuPeripheralWTIMER1   = 32 + _SMU_PPUPATD1_WTIMER1_SHIFT,   /**< SMU peripheral identifier for WTIMER1.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_103)
  smuPeripheralACMP0     = _SMU_PPUPATD0_ACMP0_SHIFT,          /**< SMU peripheral identifier for ACMP0.     */
  smuPeripheralACMP1     = _SMU_PPUPATD0_ACMP1_SHIFT,          /**< SMU peripheral identifier for ACMP1.     */
  smuPeripheralADC0      = _SMU_PPUPATD0_ADC0_SHIFT,           /**< SMU peripheral identifier for ADC0.      */
  smuPeripheralCAN0      = _SMU_PPUPATD0_CAN0_SHIFT,           /**< SMU peripheral identifier for CAN0.      */
  smuPeripheralCMU       = _SMU_PPUPATD0_CMU_SHIFT,            /**< SMU peripheral identifier for CMU.       */
  smuPeripheralCRYOTIMER = _SMU_PPUPATD0_CRYOTIMER_SHIFT,      /**< SMU peripheral identifier for CRYOTIMER. */
  smuPeripheralCRYPTO0   = _SMU_PPUPATD0_CRYPTO0_SHIFT,        /**< SMU peripheral identifier for CRYPTO0.   */
  smuPeripheralCSEN      = _SMU_PPUPATD0_CSEN_SHIFT,           /**< SMU peripheral identifier for CSEN.      */
  smuPeripheralVDAC0     = _SMU_PPUPATD0_VDAC0_SHIFT,          /**< SMU peripheral identifier for VDAC0.     */
  smuPeripheralPRS       = _SMU_PPUPATD0_PRS_SHIFT,            /**< SMU peripheral identifier for PRS.       */
  smuPeripheralEMU       = _SMU_PPUPATD0_EMU_SHIFT,            /**< SMU peripheral identifier for EMU.       */
  smuPeripheralGPCRC     = _SMU_PPUPATD0_GPCRC_SHIFT,          /**< SMU peripheral identifier for GPCRC.     */
  smuPeripheralGPIO      = _SMU_PPUPATD0_GPIO_SHIFT,           /**< SMU peripheral identifier for GPIO.      */
  smuPeripheralI2C0      = _SMU_PPUPATD0_I2C0_SHIFT,           /**< SMU peripheral identifier for I2C0.      */
  smuPeripheralI2C1      = _SMU_PPUPATD0_I2C1_SHIFT,           /**< SMU peripheral identifier for I2C1.      */
  smuPeripheralMSC       = _SMU_PPUPATD0_MSC_SHIFT,            /**< SMU peripheral identifier for MAC.       */
#if defined(_SMU_PPUPATD0_LCD_SHIFT)
  smuPeripheralLCD       = _SMU_PPUPATD0_LCD_SHIFT,            /**< SMU peripheral identifier for LCD.       */
#endif
  smuPeripheralLDMA      = _SMU_PPUPATD0_LDMA_SHIFT,           /**< SMU peripheral identifier for LDMA.      */
  smuPeripheralLESENSE   = _SMU_PPUPATD0_LESENSE_SHIFT,        /**< SMU peripheral identifier for LESENSE.   */
  smuPeripheralLETIMER0  = _SMU_PPUPATD0_LETIMER0_SHIFT,       /**< SMU peripheral identifier for LETIMER0.  */
  smuPeripheralLEUART0   = _SMU_PPUPATD0_LEUART0_SHIFT,        /**< SMU peripheral identifier for LEUART0.   */
  smuPeripheralPCNT0     = _SMU_PPUPATD0_PCNT0_SHIFT,          /**< SMU peripheral identifier for PCNT0.     */
  smuPeripheralRMU       = _SMU_PPUPATD0_RMU_SHIFT,            /**< SMU peripheral identifier for RMU.       */
  smuPeripheralRTCC      = _SMU_PPUPATD0_RTCC_SHIFT,           /**< SMU peripheral identifier for RTCC.      */
  smuPeripheralSMU       = _SMU_PPUPATD0_SMU_SHIFT,            /**< SMU peripheral identifier for SMU.       */
  smuPeripheralTIMER0    = _SMU_PPUPATD0_TIMER0_SHIFT,         /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTIMER1    = _SMU_PPUPATD0_TIMER1_SHIFT,         /**< SMU peripheral identifier for TIMER0.    */
  smuPeripheralTRNG0     = _SMU_PPUPATD0_TRNG0_SHIFT,          /**< SMU peripheral identifier for TRNG0.     */
  smuPeripheralUART0     = _SMU_PPUPATD0_UART0_SHIFT,          /**< SMU peripheral identifier for UART0.     */
  smuPeripheralUSART0    = _SMU_PPUPATD0_USART0_SHIFT,         /**< SMU peripheral identifier for USART0.    */
  smuPeripheralUSART1    = _SMU_PPUPATD0_USART1_SHIFT,         /**< SMU peripheral identifier for USART1.    */
  smuPeripheralUSART2    = _SMU_PPUPATD0_USART2_SHIFT,         /**< SMU peripheral identifier for USART2.    */
  smuPeripheralUSART3    = 32 + _SMU_PPUPATD1_USART3_SHIFT,    /**< SMU peripheral identifier for USART3.    */
  smuPeripheralWDOG0     = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,     /**< SMU peripheral identifier for WDOG0.     */
  smuPeripheralWTIMER0   = 32 + _SMU_PPUPATD1_WTIMER0_SHIFT,   /**< SMU peripheral identifier for WTIMER0.   */
  smuPeripheralWTIMER1   = 32 + _SMU_PPUPATD1_WTIMER1_SHIFT,   /**< SMU peripheral identifier for WTIMER1.   */

#elif defined(_SILICON_LABS_32B_SERIES_2)
#if defined(SMU_PPUPATD0_SCRATCHPAD)
  smuPeripheralSCRATCHPAD   = _SMU_PPUPATD0_SCRATCHPAD_SHIFT,     /**< SMU peripheral identifier for SCRATCHPAD */
#endif
  smuPeripheralEMU          = _SMU_PPUPATD0_EMU_SHIFT,            /**< SMU peripheral identifier for EMU       */
  smuPeripheralCMU          = _SMU_PPUPATD0_CMU_SHIFT,            /**< SMU peripheral identifier for CMU       */
#if defined(_SMU_PPUPATD0_HFXO0_SHIFT)
  smuPeripheralHFXO         = _SMU_PPUPATD0_HFXO0_SHIFT,          /**< SMU peripheral identifier for HFXO0     */
#endif
  smuPeripheralHFRCO0       = _SMU_PPUPATD0_HFRCO0_SHIFT,         /**< SMU peripheral identifier for HFRCO0    */
  smuPeripheralFSRCO        = _SMU_PPUPATD0_FSRCO_SHIFT,          /**< SMU peripheral identifier for FSRCO     */
  smuPeripheralDPLL0        = _SMU_PPUPATD0_DPLL0_SHIFT,          /**< SMU peripheral identifier for DPLL0     */
  smuPeripheralLFXO         = _SMU_PPUPATD0_LFXO_SHIFT,           /**< SMU peripheral identifier for LFXO      */
  smuPeripheralLFRCO        = _SMU_PPUPATD0_LFRCO_SHIFT,          /**< SMU peripheral identifier for LFRCO     */
  smuPeripheralULFRCO       = _SMU_PPUPATD0_ULFRCO_SHIFT,         /**< SMU peripheral identifier for ULFRCO    */
  smuPeripheralMSC          = _SMU_PPUPATD0_MSC_SHIFT,            /**< SMU peripheral identifier for MSC       */
  smuPeripheralICACHE0      = _SMU_PPUPATD0_ICACHE0_SHIFT,        /**< SMU peripheral identifier for ICACHE0   */
  smuPeripheralPRS          = _SMU_PPUPATD0_PRS_SHIFT,            /**< SMU peripheral identifier for PRS       */
  smuPeripheralGPIO         = _SMU_PPUPATD0_GPIO_SHIFT,           /**< SMU peripheral identifier for GPIO      */
  smuPeripheralLDMA         = _SMU_PPUPATD0_LDMA_SHIFT,           /**< SMU peripheral identifier for LDMA      */
  smuPeripheralLDMAXBAR     = _SMU_PPUPATD0_LDMAXBAR_SHIFT,       /**< SMU peripheral identifier for LDMAXBAR  */
  smuPeripheralTIMER0       = _SMU_PPUPATD0_TIMER0_SHIFT,         /**< SMU peripheral identifier for TIMER0    */
  smuPeripheralTIMER1       = _SMU_PPUPATD0_TIMER1_SHIFT,         /**< SMU peripheral identifier for TIMER1    */
  smuPeripheralTIMER2       = _SMU_PPUPATD0_TIMER2_SHIFT,         /**< SMU peripheral identifier for TIMER2    */
  smuPeripheralTIMER3       = _SMU_PPUPATD0_TIMER3_SHIFT,         /**< SMU peripheral identifier for TIMER3    */
#if defined(_SMU_PPUPATD0_TIMER4_SHIFT)
  smuPeripheralTIMER4       = _SMU_PPUPATD0_TIMER4_SHIFT,         /**< SMU peripheral identifier for TIMER4    */
#endif
#if defined(_SMU_PPUPATD0_TIMER5_SHIFT)
  smuPeripheralTIMER5       = _SMU_PPUPATD0_TIMER5_SHIFT,         /**< SMU peripheral identifier for TIMER5    */
#endif
#if defined(_SMU_PPUPATD0_TIMER6_SHIFT)
  smuPeripheralTIMER6       = _SMU_PPUPATD0_TIMER6_SHIFT,         /**< SMU peripheral identifier for TIMER6    */
#endif
#if defined(_SMU_PPUPATD0_TIMER7_SHIFT)
  smuPeripheralTIMER7       = _SMU_PPUPATD0_TIMER7_SHIFT,         /**< SMU peripheral identifier for TIMER7    */
#endif
#if defined(_SMU_PPUPATD0_USART0_SHIFT)
  smuPeripheralUSART0       = _SMU_PPUPATD0_USART0_SHIFT,         /**< SMU peripheral identifier for USART0    */
#endif
#if defined(_SMU_PPUPATD0_USART1_SHIFT)
  smuPeripheralUSART1       = _SMU_PPUPATD0_USART1_SHIFT,         /**< SMU peripheral identifier for USART1    */
#endif
#if defined(_SMU_PPUPATD0_USART2_SHIFT)
  smuPeripheralUSART2       = _SMU_PPUPATD0_USART2_SHIFT,         /**< SMU peripheral identifier for USART2    */
#endif
  smuPeripheralBURTC        = _SMU_PPUPATD0_BURTC_SHIFT,          /**< SMU peripheral identifier for BURTC     */
#if defined(_SMU_PPUPATD0_I2C1_SHIFT)
  smuPeripheralI2C1         = _SMU_PPUPATD0_I2C1_SHIFT,           /**< SMU peripheral identifier for I2C1      */
#endif
  smuPeripheralCHIPTESTCTRL = _SMU_PPUPATD0_CHIPTESTCTRL_SHIFT,   /**< SMU peripheral identifier for CHIPTESTCTRL */
#if defined(_SMU_PPUPATD0_SYSCFGCFGNS_SHIFT)
  smuPeripheralSYSCFGCFGNS  = _SMU_PPUPATD0_SYSCFGCFGNS_SHIFT,    /**< SMU peripheral identifier for SYSCFGCFGNS */
#endif

#if defined(SMU_PPUPATD0_LVGD)
  smuPeripheralLVGD         = _SMU_PPUPATD0_LVGD_SHIFT,           /**< SMU peripheral identifier for LVGD      */
#endif
  smuPeripheralSYSCFG       = _SMU_PPUPATD0_SYSCFG_SHIFT,         /**< SMU peripheral identifier for SYSCFG    */
  smuPeripheralBURAM        = _SMU_PPUPATD0_BURAM_SHIFT,          /**< SMU peripheral identifier for BURAM     */
#if defined(_SMU_PPUPATD0_IFADCDEBUG_SHIFT)
  smuPeripheralIFADCDEBUG   = _SMU_PPUPATD0_IFADCDEBUG_SHIFT,     /**< SMU peripheral identifier for IFADCDEBUG*/
#endif
  smuPeripheralGPCRC        = _SMU_PPUPATD0_GPCRC_SHIFT,          /**< SMU peripheral identifier for GPCRC     */
#if defined(_SMU_PPUPATD0_DCDC_SHIFT)
  smuPeripheralDCDC         = _SMU_PPUPATD0_DCDC_SHIFT,           /**< SMU peripheral identifier for DCDC      */
#endif
#if defined(_SMU_PPUPATD0_RTCC_SHIFT)
  smuPeripheralRTCC         = _SMU_PPUPATD0_RTCC_SHIFT,           /**< SMU peripheral identifier for RTCC      */
#endif
#if defined(_SMU_PPUPATD0_HOSTMAILBOX_SHIFT)
  smuPeripheralHOSTMAILBOX  = _SMU_PPUPATD0_HOSTMAILBOX_SHIFT,    /**< SMU peripheral identifier for HOSTMAILBOX */
#endif
#if defined(_SMU_PPUPATD0_EUSART0_SHIFT)
  smuPeripheralEUSART0      = _SMU_PPUPATD0_EUSART0_SHIFT,        /**< SMU peripheral identifier for EUSART0   */
#endif
#if defined(_SMU_PPUPATD0_EUSART1_SHIFT)
  smuPeripheralEUSART1      = _SMU_PPUPATD0_EUSART1_SHIFT,        /**< SMU peripheral identifier for EUSART1   */
#endif
#if defined(_SMU_PPUPATD1_EUSART1_SHIFT)
  smuPeripheralEUSART1      = _SMU_PPUPATD1_EUSART1_SHIFT,        /**< SMU peripheral identifier for EUSART1   */
#endif
#if defined(_SMU_PPUPATD0_EUSART2_SHIFT)
  smuPeripheralEUSART2      = _SMU_PPUPATD0_EUSART2_SHIFT,        /**< SMU peripheral identifier for EUSART2   */
#endif
#if defined(_SMU_PPUPATD1_EUSART2_SHIFT)
  smuPeripheralEUSART2      = _SMU_PPUPATD1_EUSART2_SHIFT,        /**< SMU peripheral identifier for EUSART2   */
#endif
#if defined(_SMU_PPUPATD1_EUSART3_SHIFT)
  smuPeripheralEUSART3      = _SMU_PPUPATD1_EUSART3_SHIFT,        /**< SMU peripheral identifier for EUSART3   */
#endif
#if defined(_SMU_PPUPATD1_EUSART4_SHIFT)
  smuPeripheralEUSART4      = _SMU_PPUPATD1_EUSART4_SHIFT,        /**< SMU peripheral identifier for EUSART4   */
#endif
#if defined(_SMU_PPUPATD1_RTCC_SHIFT)
  smuPeripheralRTCC         = 32 + _SMU_PPUPATD1_RTCC_SHIFT,      /**< SMU peripheral identifier for RTCC      */
#endif
#if defined(_SMU_PPUPATD1_SYSRTC_SHIFT)
  smuPeripheralSYSRTC       = 32 + _SMU_PPUPATD1_SYSRTC_SHIFT,    /**< SMU peripheral identifier for SYSRTC    */
#endif
#if defined(_SMU_PPUPATD1_LCD_SHIFT)
  smuPeripheralLCD          = 32 + _SMU_PPUPATD1_LCD_SHIFT,       /**< SMU peripheral identifier for LCD       */
#endif
#if defined(_SMU_PPUPATD1_KEYSCAN_SHIFT)
  smuPeripheralKEYSCAN      = 32 + _SMU_PPUPATD1_KEYSCAN_SHIFT,   /**< SMU peripheral identifier for KEYSCAN   */
#endif
#if defined(_SMU_PPUPATD1_DMEM_SHIFT)
  smuPeripheralDMEM         = 32 + _SMU_PPUPATD1_DMEM_SHIFT,      /**< SMU peripheral identifier for DMEM      */
#endif
#if defined(_SMU_PPUPATD1_LCDRF_SHIFT)
  smuPeripheralLCDRF        = 32 + _SMU_PPUPATD1_LCDRF_SHIFT,     /**< SMU peripheral identifier for LCDRF     */
#endif
#if defined(_SMU_PPUPATD1_PFMXPPRF_SHIFT)
  smuPeripheralPFMXPPRF     = 32 + _SMU_PPUPATD1_PFMXPPRF_SHIFT,  /**< SMU peripheral identifier for PFMXPPRF  */
#endif
#if defined(_SMU_PPUPATD1_RFFPLL0_SHIFT)
  smuPeripheralRFFPLL0      = 32 + _SMU_PPUPATD1_RFFPLL0_SHIFT,    /**< SMU peripheral identifier for RFFPLL0   */
#endif
#if defined(_SMU_PPUPATD1_ETAMPDET_SHIFT)
  smuPeripheralETAMPDET     = 32 + _SMU_PPUPATD1_ETAMPDET_SHIFT,  /**< SMU peripheral identifier for ETAMPDET  */
#endif
#if defined(_SMU_PPUPATD1_VDAC0_SHIFT)
  smuPeripheralVDAC0        = 32 + _SMU_PPUPATD1_VDAC0_SHIFT,     /**< SMU peripheral identifier for VDAC0     */
#endif
#if defined(_SMU_PPUPATD1_PCNT_SHIFT)
  smuPeripheralPCNT         = 32 + _SMU_PPUPATD1_PCNT_SHIFT,      /**< SMU peripheral identifier for PCNT      */
#endif
#if defined(_SMU_PPUPATD1_LESENSE_SHIFT)
  smuPeripheralLESENSE      = 32 + _SMU_PPUPATD1_LESENSE_SHIFT,   /**< SMU peripheral identifier for LESENSE   */
#endif
#if defined(_SMU_PPUPATD1_HFRCO1_SHIFT)
  smuPeripheralHFRCO1       = 32 + _SMU_PPUPATD1_HFRCO1_SHIFT,    /**< SMU peripheral identifier for HFRCO1    */
#endif
#if defined(_SMU_PPUPATD1_HFXO0_SHIFT)
  smuPeripheralHFXO0        = 32 + _SMU_PPUPATD1_HFXO0_SHIFT,     /**< SMU peripheral identifier for HFXO0     */
#endif
#if defined(_SMU_PPUPATD1_DCDC_SHIFT)
  smuPeripheralDCDC         = 32 + _SMU_PPUPATD1_DCDC_SHIFT,      /**< SMU peripheral identifier for DCDC      */
#endif
#if defined(_SMU_PPUPATD1_PDM_SHIFT)
  smuPeripheralPDM          = 32 + _SMU_PPUPATD1_PDM_SHIFT,       /**< SMU peripheral identifier for PDM       */
#endif
#if defined(_SMU_PPUPATD1_RFSENSE_SHIFT)
  smuPeripheralRFSENSE      = 32 + _SMU_PPUPATD1_RFSENSE_SHIFT,   /**< SMU peripheral identifier for RFSENSE   */
#endif
#if defined(_SMU_PPUPATD1_SEPUF_SHIFT)
  smuPeripheralSEPUF        = 32 + _SMU_PPUPATD1_SEPUF_SHIFT,     /**< SMU peripheral identifier for SEPUF     */
#endif
  smuPeripheralLETIMER0     = 32 + _SMU_PPUPATD1_LETIMER0_SHIFT,  /**< SMU peripheral identifier for LETIMER   */
#if defined(_SMU_PPUPATD1_IADC0_SHIFT)
  smuPeripheralIADC0        = 32 + _SMU_PPUPATD1_IADC0_SHIFT,     /**< SMU peripheral identifier for IADC0     */
#endif
#if defined(_SMU_PPUPATD1_ACMP0_SHIFT)
  smuPeripheralACMP0        = 32 + _SMU_PPUPATD1_ACMP0_SHIFT,     /**< SMU peripheral identifier for ACMP0     */
#endif
#if defined(_SMU_PPUPATD1_ACMP1_SHIFT)
  smuPeripheralACMP1        = 32 + _SMU_PPUPATD1_ACMP1_SHIFT,     /**< SMU peripheral identifier for ACMP1     */
#endif
#if defined(_SMU_PPUPATD1_I2C0_SHIFT)
  smuPeripheralI2C0         = 32 + _SMU_PPUPATD1_I2C0_SHIFT,      /**< SMU peripheral identifier for I2C0      */
#endif
#if defined(_SMU_PPUPATD1_HFRCOEM23_SHIFT)
  smuPeripheralHFRCOEM23    = 32 + _SMU_PPUPATD1_HFRCOEM23_SHIFT, /**< SMU peripheral identifier for HFRCOEM23 */
#endif
  smuPeripheralWDOG0        = 32 + _SMU_PPUPATD1_WDOG0_SHIFT,     /**< SMU peripheral identifier for WDOG0     */
#if defined(_SMU_PPUPATD1_WDOG1_SHIFT)
  smuPeripheralWDOG1        = 32 + _SMU_PPUPATD1_WDOG1_SHIFT,     /**< SMU peripheral identifier for WDOG1     */
#endif
#if defined(_SMU_PPUPATD1_EUSART0_SHIFT)
  smuPeripheralEUSART0        = 32 + _SMU_PPUPATD1_EUSART0_SHIFT, /**< SMU peripheral identifier for EUSART0   */
#endif
  smuPeripheralAMUXCP0      = 32 + _SMU_PPUPATD1_AMUXCP0_SHIFT,   /**< SMU peripheral identifier for AMUXCP0   */
#if defined(_SMU_PPUPATD1_RADIOAES_SHIFT)
  smuPeripheralRADIOAES     = 32 + _SMU_PPUPATD1_RADIOAES_SHIFT,  /**< SMU peripheral identifier for RADIOAES  */
#endif
#if defined(_SMU_PPUPATD1_EUART0_SHIFT)
  smuPeripheralEUART0       = 32 + _SMU_PPUPATD1_EUART0_SHIFT,    /**< SMU peripheral identifier for EUART0    */
#endif
#if defined(_SMU_PPUPATD1_BUFC_SHIFT)
  smuPeripheralBUFC         = 32 + _SMU_PPUPATD1_BUFC_SHIFT,      /**< SMU peripheral identifier for BUFC      */
#endif
  smuPeripheralSMU          = 32 + _SMU_PPUPATD1_SMU_SHIFT,       /**< SMU peripheral identifier for SMU       */
#if defined(_SMU_PPUPATD1_SMUCFGNS_SHIFT)
  smuPeripheralSMUCFGNS     = 32 + _SMU_PPUPATD1_SMUCFGNS_SHIFT,  /**< SMU peripheral identifier for SMUCFGNS  */
#endif
#if defined(_SMU_PPUPATD1_AHBUSBSYS_SHIFT)
  smuPeripheralAHBUSBSYS    = 32 + _SMU_PPUPATD1_AHBUSBSYS_SHIFT, /**< SMU peripheral identifier for AHBUSBSYS */
#endif
#if defined(_SMU_PPUPATD1_AHBRADIO_SHIFT)
  smuPeripheralAHBRADIO     = 32 + _SMU_PPUPATD1_AHBRADIO_SHIFT,  /**< SMU peripheral identifier for AHBRADIO  */
#endif
#if defined(_SMU_PPUPATD1_SEMAILBOX_SHIFT)
  smuPeripheralSEMAILBOX    = 32 + _SMU_PPUPATD1_SEMAILBOX_SHIFT, /**< SMU peripheral identifier for SEMAILBOX */
#endif
#if defined(_SMU_PPUPATD1_CRYPTOACC_SHIFT)
  smuPeripheralCRYPTOACC    = 32 + _SMU_PPUPATD1_CRYPTOACC_SHIFT, /**< SMU peripheral identifier for CRYPTOACC */
#endif
#else
#error "No peripherals defined for SMU for this device configuration."
#endif
  smuPeripheralEnd /**< SMU peripheral end. */
} SMU_Peripheral_TypeDef;

/** SMU peripheral privileged access enablers. */
typedef struct {
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_84)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedReserved0  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved1  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedReserved2  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO0    : 1;     /**< Privileged access enabler for CRYPTO0.   */
  bool privilegedCRYPTO1    : 1;     /**< Privileged access enabler for CRYPTO1.   */
  bool privilegedCSEN       : 1;     /**< Privileged access enabler for CSEN.      */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedFPUEH      : 1;     /**< Privileged access enabler for FPUEH.     */
  bool privilegedReserved3  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedI2C1       : 1;     /**< Privileged access enabler for I2C1.      */
  bool privilegedIDAC0      : 1;     /**< Privileged access enabler for IDAC0.     */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MSC.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLEUART0    : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedReserved4  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedPCNT1      : 1;     /**< Privileged access enabler for PCNT1.     */
  bool privilegedPCNT2      : 1;     /**< Privileged access enabler for PCNT2.     */
  bool privilegedReserved5  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved6  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved7  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */
  bool privilegedReserved8  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedUSART2     : 1;     /**< Privileged access enabler for USART2.    */
  bool privilegedUSART3     : 1;     /**< Privileged access enabler for USART3.    */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWDOG1      : 1;     /**< Privileged access enabler for WDOG1.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */
  bool privilegedWTIMER1    : 1;     /**< Privileged access enabler for WTIMER1.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedReserved0  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved1  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedReserved2  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO0    : 1;     /**< Privileged access enabler for CRYPTO0.   */
  bool privilegedCRYPTO1    : 1;     /**< Privileged access enabler for CRYPTO1.   */
  bool privilegedCSEN       : 1;     /**< Privileged access enabler for CSEN.      */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedFPUEH      : 1;     /**< Privileged access enabler for FPUEH.     */
  bool privilegedReserved3  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedI2C1       : 1;     /**< Privileged access enabler for I2C1.      */
  bool privilegedIDAC0      : 1;     /**< Privileged access enabler for IDAC0.     */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MSC.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLEUART0    : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedReserved4  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedReserved5  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved6  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved7  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved8  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */
  bool privilegedReserved9  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedUSART2     : 1;     /**< Privileged access enabler for USART2.    */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWDOG1      : 1;     /**< Privileged access enabler for WDOG1.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedReserved0  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved1  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedReserved2  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO     : 1;     /**< Privileged access enabler for CRYPTO.    */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedFPUEH      : 1;     /**< Privileged access enabler for FPUEH.     */
  bool privilegedReserved3  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedIDAC0      : 1;     /**< Privileged access enabler for IDAC0.     */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MSC.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLEUART     : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedReserved4  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedReserved5  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved6  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved7  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved8  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */

  bool privilegedReserved9  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWDOG1      : 1;     /**< Privileged access enabler for WDOG1.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedACMP2      : 1;     /**< Privileged access enabler for ACMP2.     */
  bool privilegedACMP3      : 1;     /**< Privileged access enabler for ACMP3.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedADC1       : 1;     /**< Privileged access enabler for ADC1.      */
  bool privilegedCAN0       : 1;     /**< Privileged access enabler for CAN0.      */
  bool privilegedCAN1       : 1;     /**< Privileged access enabler for CAN1.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO0    : 1;     /**< Privileged access enabler for CRYPTO0.   */
  bool privilegedCSEN       : 1;     /**< Privileged access enabler for CSEN.      */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEBI        : 1;     /**< Privileged access enabler for EBI.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedETH        : 1;     /**< Privileged access enabler for ETH.       */
  bool privilegedFPUEH      : 1;     /**< Privileged access enabler for FPUEH.     */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedI2C1       : 1;     /**< Privileged access enabler for I2C1.      */
  bool privilegedI2C2       : 1;     /**< Privileged access enabler for I2C2.      */
  bool privilegedIDAC0      : 1;     /**< Privileged access enabler for IDAC0.     */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MAC.       */
  bool privilegedLCD        : 1;     /**< Privileged access enabler for LCD.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLETIMER1   : 1;     /**< Privileged access enabler for LETIMER1.  */
  bool privilegedLEUART0    : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedLEUART1    : 1;     /**< Privileged access enabler for LEUART1.   */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedPCNT1      : 1;     /**< Privileged access enabler for PCNT1.     */
  bool privilegedPCNT2      : 1;     /**< Privileged access enabler for PCNT2.     */
  bool privilegedQSPI0      : 1;     /**< Privileged access enabler for QSPI0.     */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTC        : 1;     /**< Privileged access enabler for RTC.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSDIO       : 1;     /**< Privileged access enabler for SDIO.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTIMER2     : 1;     /**< Privileged access enabler for TIMER2.    */
  bool privilegedTIMER3     : 1;     /**< Privileged access enabler for TIMER3.    */
  bool privilegedTIMER4     : 1;     /**< Privileged access enabler for TIMER4.    */
  bool privilegedTIMER5     : 1;     /**< Privileged access enabler for TIMER5.    */
  bool privilegedTIMER6     : 1;     /**< Privileged access enabler for TIMER6.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUART0      : 1;     /**< Privileged access enabler for UART0.     */
  bool privilegedUART1      : 1;     /**< Privileged access enabler for UART1.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedUSART2     : 1;     /**< Privileged access enabler for USART2.    */
  bool privilegedUSART3     : 1;     /**< Privileged access enabler for USART3.    */
  bool privilegedUSART4     : 1;     /**< Privileged access enabler for USART4.    */
  bool privilegedUSART5     : 1;     /**< Privileged access enabler for USART5.    */
  bool privilegedUSB        : 1;     /**< Privileged access enabler for USB.       */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWDOG1      : 1;     /**< Privileged access enabler for WDOG1.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */
  bool privilegedWTIMER1    : 1;     /**< Privileged access enabler for WTIMER1.   */
  bool privilegedWTIMER2    : 1;     /**< Privileged access enabler for WTIMER2.   */
  bool privilegedWTIMER3    : 1;     /**< Privileged access enabler for WTIMER3.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_103)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedCAN0       : 1;     /**< Privileged access enabler for CAN0.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO0    : 1;     /**< Privileged access enabler for CRYPTO0.   */
  bool privilegedCSEN       : 1;     /**< Privileged access enabler for CSEN.      */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedI2C1       : 1;     /**< Privileged access enabler for I2C1.      */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MAC.       */
  bool privilegedLCD        : 1;     /**< Privileged access enabler for LCD.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLEUART0    : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUART0      : 1;     /**< Privileged access enabler for UART0.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedUSART2     : 1;     /**< Privileged access enabler for USART2.    */
  bool privilegedUSART3     : 1;     /**< Privileged access enabler for USART3.    */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */
  bool privilegedWTIMER1    : 1;     /**< Privileged access enabler for WTIMER1.   */

#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_106)
  bool privilegedACMP0      : 1;     /**< Privileged access enabler for ACMP0.     */
  bool privilegedACMP1      : 1;     /**< Privileged access enabler for ACMP1.     */
  bool privilegedACMP2      : 1;     /**< Privileged access enabler for ACMP2.     */
  bool privilegedADC0       : 1;     /**< Privileged access enabler for ADC0.      */
  bool privilegedADC1       : 1;     /**< Privileged access enabler for ADC1.      */
  bool privilegedCAN0       : 1;     /**< Privileged access enabler for CAN0.      */
  bool privilegedCAN1       : 1;     /**< Privileged access enabler for CAN1.      */
  bool privilegedCMU        : 1;     /**< Privileged access enabler for CMU.       */
  bool privilegedCRYOTIMER  : 1;     /**< Privileged access enabler for CRYOTIMER. */
  bool privilegedCRYPTO0    : 1;     /**< Privileged access enabler for CRYPTO0.   */
  bool privilegedCSEN       : 1;     /**< Privileged access enabler for CSEN.      */
  bool privilegedVDAC0      : 1;     /**< Privileged access enabler for VDAC0.     */
  bool privilegedPRS        : 1;     /**< Privileged access enabler for PRS.       */
  bool privilegedEBI        : 1;     /**< Privileged access enabler for EBI.       */
  bool privilegedEMU        : 1;     /**< Privileged access enabler for EMU.       */
  bool privilegedFPUEH      : 1;     /**< Privileged access enabler for FPUEH.     */
  bool privilegedGPCRC      : 1;     /**< Privileged access enabler for GPCRC.     */
  bool privilegedGPIO       : 1;     /**< Privileged access enabler for GPIO.      */
  bool privilegedI2C0       : 1;     /**< Privileged access enabler for I2C0.      */
  bool privilegedI2C1       : 1;     /**< Privileged access enabler for I2C1.      */
  bool privilegedIDAC0      : 1;     /**< Privileged access enabler for IDAC0.     */
  bool privilegedMSC        : 1;     /**< Privileged access enabler for MAC.       */
  bool privilegedLCD        : 1;     /**< Privileged access enabler for LCD.       */
  bool privilegedLDMA       : 1;     /**< Privileged access enabler for LDMA.      */
  bool privilegedLESENSE    : 1;     /**< Privileged access enabler for LESENSE.   */
  bool privilegedLETIMER0   : 1;     /**< Privileged access enabler for LETIMER0.  */
  bool privilegedLETIMER1   : 1;     /**< Privileged access enabler for LETIMER1.  */
  bool privilegedLEUART0    : 1;     /**< Privileged access enabler for LEUART0.   */
  bool privilegedLEUART1    : 1;     /**< Privileged access enabler for LEUART1.   */
  bool privilegedReserved0  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved1  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedReserved2  : 1;     /**< Reserved privileged access enabler.      */
  bool privilegedPCNT0      : 1;     /**< Privileged access enabler for PCNT0.     */
  bool privilegedPCNT1      : 1;     /**< Privileged access enabler for PCNT1.     */
  bool privilegedPCNT2      : 1;     /**< Privileged access enabler for PCNT2.     */
  bool privilegedPDM        : 1;     /**< Privileged access enabler for PDM.       */
  bool privilegedQSPI0      : 1;     /**< Privileged access enabler for QSPI0.     */
  bool privilegedRMU        : 1;     /**< Privileged access enabler for RMU.       */
  bool privilegedRTC        : 1;     /**< Privileged access enabler for RTC.       */
  bool privilegedRTCC       : 1;     /**< Privileged access enabler for RTCC.      */
  bool privilegedSDIO       : 1;     /**< Privileged access enabler for SDIO.      */
  bool privilegedSMU        : 1;     /**< Privileged access enabler for SMU.       */
  bool privilegedTIMER0     : 1;     /**< Privileged access enabler for TIMER0.    */
  bool privilegedTIMER1     : 1;     /**< Privileged access enabler for TIMER1.    */
  bool privilegedTIMER2     : 1;     /**< Privileged access enabler for TIMER2.    */
  bool privilegedTIMER3     : 1;     /**< Privileged access enabler for TIMER3.    */
  bool privilegedTRNG0      : 1;     /**< Privileged access enabler for TRNG0.     */
  bool privilegedUART0      : 1;     /**< Privileged access enabler for UART0.     */
  bool privilegedUART1      : 1;     /**< Privileged access enabler for UART1.     */
  bool privilegedUSART0     : 1;     /**< Privileged access enabler for USART0.    */
  bool privilegedUSART1     : 1;     /**< Privileged access enabler for USART1.    */
  bool privilegedUSART2     : 1;     /**< Privileged access enabler for USART2.    */
  bool privilegedUSART3     : 1;     /**< Privileged access enabler for USART3.    */
  bool privilegedUSART4     : 1;     /**< Privileged access enabler for USART4.    */
  bool privilegedUSB        : 1;     /**< Privileged access enabler for USB.       */
  bool privilegedWDOG0      : 1;     /**< Privileged access enabler for WDOG0.     */
  bool privilegedWDOG1      : 1;     /**< Privileged access enabler for WDOG1.     */
  bool privilegedWTIMER0    : 1;     /**< Privileged access enabler for WTIMER0.   */
  bool privilegedWTIMER1    : 1;     /**< Privileged access enabler for WTIMER1.   */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  bool privilegedReserved0    : 1;  /**< Reserved privileged access enabler.        */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS          */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedUSART0       : 1;  /**< Privileged access enabler for USART0       */
  bool privilegedUSART1       : 1;  /**< Privileged access enabler for USART1       */
  bool privilegedUSART2       : 1;  /**< Privileged access enabler for USART2       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedLVGD         : 1;  /**< Privileged access enabler for LVGD         */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedIFADCDEBUG   : 1;  /**< Privileged access enabler for IFADCDEBUG   */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedRTCC         : 1;  /**< Privileged access enabler for RTCC         */

  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER      */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedACMP0        : 1;  /**< Privileged access enabler for ACMP0        */
  bool privilegedACMP1        : 1;  /**< Privileged access enabler for ACMP1        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedHFRCOEM23    : 1;  /**< Privileged access enabler for HFRCOEM23    */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedWDOG1        : 1;  /**< Privileged access enabler for WDOG1        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedBUFC         : 1;  /**< Privileged access enabler for BUFC         */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */
  bool privilegedSEMAILBOX    : 1;  /**< Privileged access enabler for SEMAILBOX    */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  bool privilegedSCRATCHPAD   : 1;  /**< Privileged access enabler for SCRATCHPAD   */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS0         */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedTIMER4       : 1;  /**< Privileged access enabler for TIMER4       */
  bool privilegedUSART0       : 1;  /**< Privileged access enabler for USART0       */
  bool privilegedUSART1       : 1;  /**< Privileged access enabler for USART1       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedSYSCFGCFGNS  : 1;  /**< Privileged access enabler for SYSCFGCFGNS  */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedIFADCDEBUG   : 1;  /**< Privileged access enabler for IFADCDEBUG   */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedDCI          : 1;  /**< Privileged access enabler for DCI          */

  bool privilegedROOTCFG      : 1;  /**< Privileged access enabler for ROOTCFG      */
  bool privilegedDCDC         : 1;  /**< Privileged access enabler for DCDC         */
  bool privilegedPDM          : 1;  /**< Privileged access enabler for PDM          */
  bool privilegedRFSENSE      : 1;  /**< Privileged access enabler for RFSENSE      */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedSMUCFGNS     : 1;  /**< Privileged access enabler for SMUCFGNS     */
  bool privilegedRTCC         : 1;  /**< Privileged access enabler for RTCC         */
  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER0     */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedEUART0       : 1;  /**< Privileged access enabler for EUART0       */
  bool privilegedCRYPTOACC    : 1;  /**< Privileged access enabler for CRYPTOACC    */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
  bool privilegedReserved0    : 1;  /**< Reserved privileged access enabler         */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS0         */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedTIMER4       : 1;  /**< Privileged access enabler for TIMER4       */
  bool privilegedUSART0       : 1;  /**< Privileged access enabler for USART0       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedSYSCFGCFGNS  : 1;  /**< Privileged access enabler for SYSCFGCFGNS  */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedDCDC         : 1;  /**< Privileged access enabler for DCDC         */
  bool privilegedHOSTMAILBOX  : 1;  /**< Privileged access enabler for HOSTMAILBOX  */
  bool privilegedEUSART1      : 1;  /**< Privileged access enabler for EUSART1      */
  bool privilegedEUSART2      : 1;  /**< Privileged access enabler for EUSART2      */

  bool privilegedSYSRTC       : 1;  /**< Privileged access enabler for SYSRTC       */
  bool privilegedLCD          : 1;  /**< Privileged access enabler for LCD          */
  bool privilegedKEYSCAN      : 1;  /**< Privileged access enabler for KEYSCAN      */
  bool privilegedDMEM         : 1;  /**< Privileged access enabler for DMEM         */
  bool privilegedLCDRF        : 1;  /**< Privileged access enabler for LCDRF        */
  bool privilegedPFMXPPRF     : 1;  /**< Privileged access enabler for PFMXPPRF     */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedSMUCFGNS     : 1;  /**< Privileged access enabler for SMUCFGNS     */
  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER0     */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedACMP0        : 1;  /**< Privileged access enabler for ACMP0        */
  bool privilegedACMP1        : 1;  /**< Privileged access enabler for ACMP1        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedVDAC0        : 1;  /**< Privileged access enabler for VDAC0        */
  bool privilegedPCNT         : 1;  /**< Privileged access enabler for PCNT         */
  bool privilegedLESENSE      : 1;  /**< Privileged access enabler for LESENSE      */
  bool privilegedHFRCO1       : 1;  /**< Privileged access enabler for HFRCO1       */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedWDOG1        : 1;  /**< Privileged access enabler for WDOG1        */
  bool privilegedEUSART0      : 1;  /**< Privileged access enabler for EUSART0      */
  bool privilegedSEMAILBOX    : 1;  /**< Privileged access enabler for SEMAILBOX    */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  bool privilegedReserved0    : 1;  /**< Reserved privileged access enabler         */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS0         */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedTIMER4       : 1;  /**< Privileged access enabler for TIMER4       */
  bool privilegedUSART0       : 1;  /**< Privileged access enabler for USART0       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedSYSCFGCFGNS  : 1;  /**< Privileged access enabler for SYSCFGCFGNS  */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedDCDC         : 1;  /**< Privileged access enabler for DCDC         */
  bool privilegedHOSTMAILBOX  : 1;  /**< Privileged access enabler for HOSTMAILBOX  */
  bool privilegedEUSART1      : 1;  /**< Privileged access enabler for EUSART1      */
  bool privilegedSYSRTC       : 1;  /**< Privileged access enabler for SYSRTC       */

  bool privilegedKEYPAD       : 1;  /**< Privileged access enabler for KEYPAD       */
  bool privilegedDMEM         : 1;  /**< Privileged access enabler for DMEM         */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedSMUCFGNS     : 1;  /**< Privileged access enabler for SMUCFGNS     */
  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER0     */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedACMP0        : 1;  /**< Privileged access enabler for ACMP0        */
  bool privilegedACMP1        : 1;  /**< Privileged access enabler for ACMP1        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedVDAC0        : 1;  /**< Privileged access enabler for VDAC0        */
  bool privilegedVDAC1        : 1;  /**< Privileged access enabler for VDAC1        */
  bool privilegedPCNT         : 1;  /**< Privileged access enabler for PCNT         */
  bool privilegedHFRCO1       : 1;  /**< Privileged access enabler for HFRCO1       */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedWDOG1        : 1;  /**< Privileged access enabler for WDOG1        */
  bool privilegedEUSART0      : 1;  /**< Privileged access enabler for EUSART0      */
  bool privilegedSEMAILBOX    : 1;  /**< Privileged access enabler for SEMAILBOX    */
  bool privilegedMVP          : 1;  /**< Privileged access enabler for MVP          */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
  bool privilegedReserved0    : 1;  /**< Reserved privileged access enabler         */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS0         */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedTIMER4       : 1;  /**< Privileged access enabler for TIMER4       */
  bool privilegedTIMER5       : 1;  /**< Privileged access enabler for TIMER5       */
  bool privilegedTIMER6       : 1;  /**< Privileged access enabler for TIMER6       */
  bool privilegedTIMER7       : 1;  /**< Privileged access enabler for TIMER7       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedSYSCFGCFGNS  : 1;  /**< Privileged access enabler for SYSCFGCFGNS  */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedDCDC         : 1;  /**< Privileged access enabler for DCDC         */
  bool privilegedHOSTMAILBOX  : 1;  /**< Privileged access enabler for HOSTMAILBOX  */

  bool privilegedEUSART1      : 1;  /**< Privileged access enabler for EUSART1      */
  bool privilegedEUSART2      : 1;  /**< Privileged access enabler for EUSART2      */
  bool privilegedEUSART3      : 1;  /**< Privileged access enabler for EUSART3      */
  bool privilegedEUSART4      : 1;  /**< Privileged access enabler for EUSART4      */
  bool privilegedSYSRTC       : 1;  /**< Privileged access enabler for SYSRTC       */
  bool privilegedDMEM         : 1;  /**< Privileged access enabler for DMEM         */
  bool privilegedPFMXPPRF     : 1;  /**< Privileged access enabler for PFMXPPRF     */
  bool privilegedRFFPLL0      : 1;  /**< Privileged access enabler for RFFPLL0      */
  bool privilegedETAMPDET     : 1;  /**< Privileged access enabler for ETAMPDET     */
  bool privilegedBUFC         : 1;  /**< Privileged access enabler for BUFC         */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedSMUCFGNS     : 1;  /**< Privileged access enabler for SMUCFGNS     */
  bool privilegedAHBUSBSYS    : 1;  /**< Privileged access enabler for AHBUSBSYS    */
  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER0     */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedACMP0        : 1;  /**< Privileged access enabler for ACMP0        */
  bool privilegedACMP1        : 1;  /**< Privileged access enabler for ACMP1        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedVDAC0        : 1;  /**< Privileged access enabler for VDAC0        */
  bool privilegedPCNT         : 1;  /**< Privileged access enabler for PCNT         */
  bool privilegedLESENSE      : 1;  /**< Privileged access enabler for LESENSE      */
  bool privilegedHFRCO1       : 1;  /**< Privileged access enabler for HFRCO1       */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedWDOG1        : 1;  /**< Privileged access enabler for WDOG1        */
  bool privilegedEUSART0      : 1;  /**< Privileged access enabler for EUSART0      */
  bool privilegedSEMAILBOX    : 1;  /**< Privileged access enabler for SEMAILBOX    */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)
  bool privilegedSCRATCHPAD   : 1;  /**< Privileged access enabler for SCRATCHPAD   */
  bool privilegedEMU          : 1;  /**< Privileged access enabler for EMU          */
  bool privilegedCMU          : 1;  /**< Privileged access enabler for CMU          */
  bool privilegedHFXO0        : 1;  /**< Privileged access enabler for HFXO0        */
  bool privilegedHFRCO0       : 1;  /**< Privileged access enabler for HFRCO0       */
  bool privilegedFSRCO        : 1;  /**< Privileged access enabler for FSRCO        */
  bool privilegedDPLL0        : 1;  /**< Privileged access enabler for DPLL0        */
  bool privilegedLFXO         : 1;  /**< Privileged access enabler for LFXO         */
  bool privilegedLFRCO        : 1;  /**< Privileged access enabler for LFRCO        */
  bool privilegedULFRCO       : 1;  /**< Privileged access enabler for ULFRCO       */
  bool privilegedMSC          : 1;  /**< Privileged access enabler for MSC          */
  bool privilegedICACHE0      : 1;  /**< Privileged access enabler for ICACHE0      */
  bool privilegedPRS          : 1;  /**< Privileged access enabler for PRS0         */
  bool privilegedGPIO         : 1;  /**< Privileged access enabler for GPIO         */
  bool privilegedLDMA         : 1;  /**< Privileged access enabler for LDMA         */
  bool privilegedLDMAXBAR     : 1;  /**< Privileged access enabler for LDMAXBAR     */
  bool privilegedTIMER0       : 1;  /**< Privileged access enabler for TIMER0       */
  bool privilegedTIMER1       : 1;  /**< Privileged access enabler for TIMER1       */
  bool privilegedTIMER2       : 1;  /**< Privileged access enabler for TIMER2       */
  bool privilegedTIMER3       : 1;  /**< Privileged access enabler for TIMER3       */
  bool privilegedTIMER4       : 1;  /**< Privileged access enabler for TIMER4       */
  bool privilegedUSART0       : 1;  /**< Privileged access enabler for USART0       */
  bool privilegedUSART1       : 1;  /**< Privileged access enabler for USART1       */
  bool privilegedBURTC        : 1;  /**< Privileged access enabler for BURTC        */
  bool privilegedI2C1         : 1;  /**< Privileged access enabler for I2C1         */
  bool privilegedCHIPTESTCTRL : 1;  /**< Privileged access enabler for CHIPTESTCTRL */
  bool privilegedSYSCFGCFGNS  : 1;  /**< Privileged access enabler for SYSCFGCFGNS  */
  bool privilegedSYSCFG       : 1;  /**< Privileged access enabler for SYSCFG       */
  bool privilegedBURAM        : 1;  /**< Privileged access enabler for BURAM        */
  bool privilegedIFADCDEBUG   : 1;  /**< Privileged access enabler for IFADCDEBUG   */
  bool privilegedGPCRC        : 1;  /**< Privileged access enabler for GPCRC        */
  bool privilegedDCI          : 1;  /**< Privileged access enabler for DCI          */

  bool privilegedReserved0    : 1;  /**< Reserved privileged access enabler         */
  bool privilegedDCDC         : 1;  /**< Privileged access enabler for DCDC         */
  bool privilegedPDM          : 1;  /**< Privileged access enabler for PDM          */
  bool privilegedRFSENSE      : 1;  /**< Privileged access enabler for RFSENSE      */
  bool privilegedSEPUF        : 1;  /**< Privileged access enabler for SEPUF        */
  bool privilegedETAMPDET     : 1;  /**< Privileged access enabler for ETAMPDET     */
  bool privilegedRADIOAES     : 1;  /**< Privileged access enabler for RADIOAES     */
  bool privilegedSMU          : 1;  /**< Privileged access enabler for SMU          */
  bool privilegedSMUCFGNS     : 1;  /**< Privileged access enabler for SMUCFGNS     */
  bool privilegedRTCC         : 1;  /**< Privileged access enabler for RTCC         */
  bool privilegedLETIMER0     : 1;  /**< Privileged access enabler for LETIMER0     */
  bool privilegedIADC0        : 1;  /**< Privileged access enabler for IADC0        */
  bool privilegedACMP0        : 1;  /**< Privileged access enabler for ACMP0        */
  bool privilegedI2C0         : 1;  /**< Privileged access enabler for I2C0         */
  bool privilegedWDOG0        : 1;  /**< Privileged access enabler for WDOG0        */
  bool privilegedAMUXCP0      : 1;  /**< Privileged access enabler for AMUXCP0      */
  bool privilegedEUSART0      : 1;  /**< Privileged access enabler for EUSART0      */
  bool privilegedCRYPTOACC    : 1;  /**< Privileged access enabler for CRYPTOACC    */
  bool privilegedAHBRADIO     : 1;  /**< Privileged access enabler for AHBRADIO     */

#else
#error "No peripherals defined for SMU for this device configuration"
#endif
} SMU_PrivilegedAccess_TypeDef;

/*******************************************************************************
 ******************************   STRUCTS   ************************************
 ******************************************************************************/

/** SMU initialization structure. */
typedef struct {
  union {
    uint32_t reg[2];                      /**< Peripheral access control array.*/
    SMU_PrivilegedAccess_TypeDef access;  /**< Peripheral access control array.*/
  } ppu;                                  /**< PPU init array.*/
  bool enable;                            /**< SMU enable flag. When set, SMU_Init() will enable SMU.*/
} SMU_Init_TypeDef;

/** Default SMU initialization structure settings. */
#define SMU_INIT_DEFAULT {                                        \
    { { 0 } },             /* No peripherals access protected. */ \
    true                   /* Enable SMU.*/                       \
}

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enable or disable PPU of SMU.
 *
 * @param[in] enable
 *   Set to true to enable PPU; set to false otherwise.
 ******************************************************************************/
__STATIC_INLINE void SMU_EnablePPU(bool enable)
{
#if defined (_SMU_PPUCTRL_ENABLE_SHIFT)
  BUS_RegBitWrite(&SMU->PPUCTRL, _SMU_PPUCTRL_ENABLE_SHIFT, enable);
#else
  (void)enable;
#endif
}

/***************************************************************************//**
 * @brief
 *   Initialize PPU of SMU.
 *
 * @param[in] init
 *   Pointer to initialization structure that defines which peripherals should
 *   only be accessed from privileged mode, and if PPU should be enabled.
 ******************************************************************************/
__STATIC_INLINE void SMU_Init(const SMU_Init_TypeDef *init)
{
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  SMU_NS_CFGNS->PPUNSPATD0 = init->ppu.reg[0];
  SMU_NS_CFGNS->PPUNSPATD1 = init->ppu.reg[1];
#else
  SMU->PPUPATD0 = init->ppu.reg[0];
  SMU->PPUPATD1 = init->ppu.reg[1];
#endif //SL_TRUSTZONE_SECURE

  SMU_EnablePPU(init->enable);
}
/***************************************************************************//**
 * @brief
 *   Change access settings for a peripheral.
 *
 * @details
 *   Set to limit access of a peripheral from privileged mode.
 *
 * @param[in] peripheral
 *   ID of the peripheral to change access settings for.
 *
 * @param[in] privileged
 *   Set to true if the peripheral should only be accessed from privileged
 *   mode; set to false otherwise.
 ******************************************************************************/
__STATIC_INLINE void SMU_SetPrivilegedAccess(SMU_Peripheral_TypeDef peripheral,
                                             bool privileged)
{
  EFM_ASSERT(peripheral < smuPeripheralEnd);

#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  if (peripheral < 32) {
    BUS_RegBitWrite(&SMU_NS_CFGNS->PPUNSPATD0, peripheral, privileged);
  } else {
    BUS_RegBitWrite(&SMU_NS_CFGNS->PPUNSPATD1, peripheral - 32, privileged);
  }
#else
  if (peripheral < 32) {
    BUS_RegBitWrite(&SMU->PPUPATD0, peripheral, privileged);
  } else {
    BUS_RegBitWrite(&SMU->PPUPATD1, peripheral - 32, privileged);
  }
#endif //SL_TRUSTZONE_SECURE
}

/***************************************************************************//**
 * @brief
 *   Get the ID of the peripheral that caused an access fault.
 *
 * @note
 *   The return value is only valid if SMU_IF_PPUPRIV interrupt flag
 *   is set.
 *
 * @return
 *   ID of the peripheral that caused an access fault.
 ******************************************************************************/
__STATIC_INLINE SMU_Peripheral_TypeDef SMU_GetFaultingPeripheral(void)
{
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  return (SMU_Peripheral_TypeDef)SMU_NS_CFGNS->PPUNSFS;
#else
  return (SMU_Peripheral_TypeDef)SMU->PPUFS;
#endif //SL_TRUSTZONE_SECURE
}

/***************************************************************************//**
 * @brief
 *   Clear one or more pending SMU interrupts.
 *
 * @param[in] flags
 *   Bitwise logic OR of SMU interrupt sources to clear.
 ******************************************************************************/
__STATIC_INLINE void SMU_IntClear(uint32_t flags)
{
#if defined (SMU_HAS_SET_CLEAR)
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  SMU_NS_CFGNS->NSIF_CLR = flags;
#else
  SMU->IF_CLR = flags;
#endif //SL_TRUSTZONE_SECURE
#else
  SMU->IFC = flags;
#endif //SMU_HAS_SET_CLEAR
}

/***************************************************************************//**
 * @brief
 *   Disable one or more SMU interrupts.
 *
 * @param[in] flags
 *   SMU interrupt sources to disable.
 ******************************************************************************/
__STATIC_INLINE void SMU_IntDisable(uint32_t flags)
{
#if defined (SMU_HAS_SET_CLEAR)
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  SMU_NS_CFGNS->NSIEN_CLR = flags;
#else
  SMU->IEN_CLR = flags;
#endif //SL_TRUSTZONE_SECURE
#else
  SMU->IEN &= ~flags;
#endif //SMU_HAS_SET_CLEAR
}

/***************************************************************************//**
 * @brief
 *   Enable one or more SMU interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   SMU_IntClear() prior to enabling the interrupt.
 *
 * @param[in] flags
 *   SMU interrupt sources to enable.
 ******************************************************************************/
__STATIC_INLINE void SMU_IntEnable(uint32_t flags)
{
#if defined (SMU_HAS_SET_CLEAR)
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  SMU_NS_CFGNS->NSIEN_SET = flags;
#else
  SMU->IEN_SET = flags;
#endif //SL_TRUSTZONE_SECURE
#else
  SMU->IEN |= flags;
#endif //SMU_HAS_SET_CLEAR
}

/***************************************************************************//**
 * @brief
 *   Get pending SMU interrupts.
 *
 * @return
 *   SMU interrupt sources pending.
 ******************************************************************************/
__STATIC_INLINE uint32_t SMU_IntGet(void)
{
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  return SMU_NS_CFGNS->NSIF;
#else
  return SMU->IF;
#endif //SL_TRUSTZONE_SECURE
}

/***************************************************************************//**
 * @brief
 *   Get enabled and pending SMU interrupt flags.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by this function.
 *
 * @return
 *   Pending and enabled SMU interrupt sources.
 *   The return value is the bitwise AND combination of
 *   - the OR combination of enabled interrupt sources in SMU_IEN register
 *     and
 *   - the OR combination of valid interrupt flags in SMU_IF register.
 ******************************************************************************/
__STATIC_INLINE uint32_t SMU_IntGetEnabled(void)
{
  uint32_t tmp;

#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  // Store SMU->IEN in temporary variable to define explicit order
  // of volatile accesses.
  tmp = SMU_NS_CFGNS->NSIEN;

  // Bitwise AND of pending and enabled interrupts.
  return SMU_NS_CFGNS->NSIF & tmp;
#else
  // Store SMU->IEN in temporary variable to define explicit order
  // of volatile accesses.
  tmp = SMU->IEN;

  // Bitwise AND of pending and enabled interrupts.
  return SMU->IF & tmp;
#endif //SL_TRUSTZONE_SECURE
}

/***************************************************************************//**
 * @brief
 *   Set one or more pending SMU interrupts from SW.
 *
 * @param[in] flags
 *   SMU interrupt sources to set to pending.
 ******************************************************************************/
__STATIC_INLINE void SMU_IntSet(uint32_t flags)
{
#if defined (SMU_HAS_SET_CLEAR)
#if !defined (SL_TRUSTZONE_SECURE) && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  SMU_NS_CFGNS->NSIF_SET = flags;
#else
  SMU->IF_SET = flags;
#endif //SL_TRUSTZONE_SECURE
#else
  SMU->IFS = flags;
#endif //SMU_HAS_SET_CLEAR
}

/**************************************************************************//**
* @brief
*   SMU secure IRQ Handler.
*
* @details
*   When a PPU detects an access to a secure peripheral at its non-secure
*   address or an access to a non-secure peripheral at its secure
*   address, PPUSECIF in SMU_IF is set and the ID of the peripheral being
*   accessed is written to SMU_PPUFS. If PPUSECIEN is set and the SMU's
*   Secure IRQ enabled, the CPU will be interrupted and SMU_SECURE_IRQHandler
*   Will handle the interrupt.
******************************************************************************/
#if !defined (SL_TRUSTZONE_SECURE) && defined (_SILICON_LABS_32B_SERIES_2)
void SMU_SECURE_IRQHandler(void)
{
  if (SMU_IF_PPUSEC) {
    EFM_ASSERT(SMU->IF & SMU_IF_PPUSEC);
  }

  if (SMU_IF_BMPUSEC) {
    EFM_ASSERT(SMU->IF & SMU_IF_BMPUSEC);
  }

  // PPUFS contains the ID of the peripheral caused the fault
  // The ID is ordered after the PPUSATD0-PPUSATD1 register bit fields.
  EFM_ASSERT(SMU->PPUFS);

  while (1) {
    // do nothing
  }
}
#endif //SL_TRUSTZONE_SECURE

/** @} (end addtogroup smu) */

#ifdef __cplusplus
}
#endif

#endif // defined(SMU_COUNT) && (SMU_COUNT > 0)
#endif // EM_SMU_H
