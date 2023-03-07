/**************************************************************************//**
* @file     system_em34x.h
* @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Header File for
*           Device em34x
* @version 5.8.3
* @date     30. October 2018
*
* @note
*
******************************************************************************
* @section License
* <b>(C) Copyright 2018 Silicon Labs, www.silabs.com</b>
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

#ifndef SYSTEM_EM34X_H
#define SYSTEM_EM34X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/* Cortex-M3 Processor Handler prototypes */
void Start_Handler(void) __attribute__ ((noreturn));
void Reset_Handler(void) __attribute__ ((noreturn));
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
/* Device Specific IRQHandler prototypes */
void TIM1_IRQHandler(void);
void TIM2_IRQHandler(void);
void MGMT_IRQHandler(void);
void BB_IRQHandler(void);
void SLEEPTMR_IRQHandler(void);
void SC1_IRQHandler(void);
void SC2_IRQHandler(void);
void AESCCM_IRQHandler(void);
void MACTMR_IRQHandler(void);
void MACTX_IRQHandler(void);
void MACRX_IRQHandler(void);
void ADC_IRQHandler(void);
void IRQA_IRQHandler(void);
void IRQB_IRQHandler(void);
void IRQC_IRQHandler(void);
void IRQD_IRQHandler(void);
void DEBUG_IRQHandler(void);

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_EM34X_H */
