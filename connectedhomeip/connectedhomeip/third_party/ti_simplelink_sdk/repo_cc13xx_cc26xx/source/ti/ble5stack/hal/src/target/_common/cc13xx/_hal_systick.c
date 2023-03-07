/******************************************************************************

 @file  _hal_systick.c

 @brief This module contains the HAL power management procedures for the CC2538.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "osal.h"
#include "osal_clock.h"
#include "hal_systick.h"
#include "hal_mcu.h"

#include <inc/hw_nvic.h>
#include <inc/hw_ints.h>
#include <inc/hw_sysctl.h>


/* ------------------------------------------------------------------------------------------------
 *                                          Macros
 * ------------------------------------------------------------------------------------------------
 */
#define SYSTICK 1000 /* 1 millisecond = 1/SYSTICK */
#define TICK_IN_MS 1 /* 1 millisecond */

void SysTickIntHandler(void);
/* ------------------------------------------------------------------------------------------------
 *                                      Function Prototypes
 * ------------------------------------------------------------------------------------------------
 */

/***************************************************************************************************
 * @fn      SysTickIntHandler
 *
 * @brief   The Systick Interrupt module
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void SysTickIntHandler(void)
{
  /* Update OSAL timer and clock */
  osalAdjustTimer(TICK_IN_MS);

  /* Clear Sleep Mode */
  CLEAR_SLEEP_MODE();
}

/***************************************************************************************************
 * @fn      SysTickSetup
 *
 * @brief   Setup the Systick module
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void SysTickSetup(void)
{
  unsigned long clockval;
  clockval = SysCtrlClockGet();
  clockval /= SYSTICK;

  /* 1ms period for systick */
  SysTickPeriodSet(clockval);

  /* Enable SysTick */
  SysTickEnable();

  /* Enable Systick interrupt */
  SysTickIntEnable();
}

/***************************************************************************************************
 * @fn      SysCtrlClockGet
 *
 * @brief   Get the Systick clock rate
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
unsigned long SysCtrlClockGet(void)
{
  return (GET_MCU_CLOCK);
}
