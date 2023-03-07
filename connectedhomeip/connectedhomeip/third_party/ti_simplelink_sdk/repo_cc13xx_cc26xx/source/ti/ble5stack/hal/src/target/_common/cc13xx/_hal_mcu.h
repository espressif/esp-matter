/******************************************************************************

 @file  _hal_mcu.h

 @brief Describe the purpose and contents of the file.

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

#ifndef HAL_MCU_H
#define HAL_MCU_H



/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_defs.h"
#include "hal_types.h"
#include <inc/hw_nvic.h>
#include <inc/hw_ints.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>
#include <driverlib/systick.h>
#include <driverlib/interrupt.h>
#include <driverlib/uart.h>
#include <driverlib/flash.h>

/* ------------------------------------------------------------------------------------------------
 *                                        Target Defines
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MCU_CC2538


/* ------------------------------------------------------------------------------------------------
 *                                     Compiler Abstraction
 * ------------------------------------------------------------------------------------------------
 */

/* ---------------------- IAR Compiler ---------------------- */
#if defined (__IAR_SYSTEMS_ICC__)
#define HAL_COMPILER_IAR
#define HAL_MCU_LITTLE_ENDIAN()   __LITTLE_ENDIAN__

/* ---------------------- Keil Compiler ---------------------- */
#elif defined (__KEIL__)
#define HAL_COMPILER_KEIL
#define HAL_MCU_LITTLE_ENDIAN()   0


/* ------------------ Unrecognized Compiler ------------------ */
#elif defined (ccs)
#define HAL_MCU_LITTLE_ENDIAN()   0
//do nothing for now
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                       Interrupt Macros
 * ------------------------------------------------------------------------------------------------
 */

typedef tBoolean halIntState_t;

/* Enable RF interrupt */
#define HAL_ENABLE_RF_INTERRUPT()    \
{                                    \
  IntEnable(INT_RFCORERTX);          \
}

/* Enable RF error interrupt */
#define HAL_ENABLE_RF_ERROR_INTERRUPT() \
{                                       \
  IntEnable(INT_RFCOREERR);             \
}

/* Enable interrupts */
#define HAL_ENABLE_INTERRUPTS()     IntMasterEnable()

/* Disable interrupts */
#define HAL_DISABLE_INTERRUPTS()    IntMasterDisable()

static tBoolean halIntsAreEnabled(void)
{
  tBoolean status = !IntMasterDisable();
  if (status)
  {
    IntMasterEnable();
  }
  return status;
}

#ifdef TIMAC_ON_RTOS
#define HAL_INTERRUPTS_ARE_ENABLED() (rtosCSDepth == 0)
#else
#define HAL_INTERRUPTS_ARE_ENABLED() halIntsAreEnabled()
#endif

#ifdef TIMAC_ON_RTOS
/* TODO: HAL must not have a dependency on OS, but as it is now,
 * too many OS dependent components use HAL.
 * The FreeRTOS dependency has to be removed eventually when
 * OSAL heap is ported not to use the same critical section macro,
 * and when MAC thread unsafe API is called from within the same thread only.
 */
extern uint32 rtosCSDepth;
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define HAL_ENTER_CRITICAL_SECTION(x)  \
  do { \
    (void) (x); \
    vPortEnterCritical(); \
    rtosCSDepth++; \
  } while (0)
#else /* TIMAC_ON_RTOS */
//#define HAL_ENTER_CRITICAL_SECTION(x)
#define HAL_ENTER_CRITICAL_SECTION(x)  \
  do { (x) = !IntMasterDisable(); } while (0)
#endif /* TIMAC_ON_RTOS */

#ifdef TIMAC_ON_RTOS
#define HAL_EXIT_CRITICAL_SECTION(x)   \
  do { \
    (void) (x); \
    rtosCSDepth--; \
    vPortExitCritical(); \
 } while (0)
#else /* TIMAC_ON_RTOS */
//#define HAL_EXIT_CRITICAL_SECTION(x)
#define HAL_EXIT_CRITICAL_SECTION(x) \
  do { if (x) { (void) IntMasterEnable(); } } while (0)
#endif /* TIMAC_ON_RTOS */

#define HAL_NON_ISR_ENTER_CRITICAL_SECTION(x)  HAL_ENTER_CRITICAL_SECTION(x)
#define HAL_NON_ISR_EXIT_CRITICAL_SECTION(x)   HAL_EXIT_CRITICAL_SECTION(x)

/* Hal Critical statement definition */
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t s; HAL_ENTER_CRITICAL_SECTION(s); x; HAL_EXIT_CRITICAL_SECTION(s); )

/* Enable Key/button interrupts */
#define HAL_ENABLE_PUSH_BUTTON_PORT_INTERRUPTS()  \
{                                                 \
  IntEnable(INT_GPIOC);                           \
  IntEnable(INT_GPIOA);                           \
}

/* Disable Key/button interrupts */
#define HAL_DISABLE_PUSH_BUTTON_PORT_INTERRUPTS()  \
{                                                  \
  IntDisable(INT_GPIOC);                           \
  IntDisable(INT_GPIOA);                           \
}

/* ------------------------------------------------------------------------------------------------
 *                                        Reset Macro
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_SYSTEM_RESET()  *((uint32 *)0x40082270) = 1;
#define WD_EN               BV(3)
#define WD_MODE             BV(2)
#define WD_INT_1900_USEC    (BV(0) | BV(1))
#define WD_RESET1           (0xA0 | WD_EN | WD_INT_1900_USEC)
#define WD_RESET2           (0x50 | WD_EN | WD_INT_1900_USEC)
#define WD_KICK()           st( WDCTL = (0xA0 | WDCTL & 0x0F); WDCTL = (0x50 | WDCTL & 0x0F); )

/* ------------------------------------------------------------------------------------------------
 *                                        CC253x rev numbers
 * ------------------------------------------------------------------------------------------------
 */
#define REV_A          0x00    /* workaround turned off */
#define REV_B          0x11    /* PG1.1 */
#define REV_C          0x20    /* PG2.0 */
#define REV_D          0x21    /* PG2.1 */
#define REV_CC2538     0xB964  /* CC2538 Rev*/

/* ------------------------------------------------------------------------------------------------
 *                                        CC2538 sleep common code
 * ------------------------------------------------------------------------------------------------
 */

/* PCON bit definitions */
#define PCON_IDLE  BV(0)            /* Writing 1 to force CC2530 to enter sleep mode */

/* STLOAD */
#define LDRDY            BV(0) /* Load Ready. This bit is 0 while the sleep timer
                                * loads the 24-bit compare value and 1 when the sleep
                                * timer is ready to start loading a new compare value. */

#ifdef POWER_SAVING
extern volatile uint8 halSleepOverride;

// Any ISR that is used to wake the CM3 must call this macro to prevent a race
// condition that results when the ISR that sets an OSAL event occurs after
// the OSAL loop finishes checking for events.
#define CLEAR_SLEEP_MODE()        st( halSleepOverride = TRUE; )
#define ALLOW_SLEEP_MODE()        st( halSleepOverride = FALSE; )

#else

#define CLEAR_SLEEP_MODE()
#define ALLOW_SLEEP_MODE()

#endif

/**************************************************************************************************
 */
#endif

