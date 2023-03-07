/**************************************************************************//**
* @file     system_sky66107.c
* @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File for
*           Device sky66107
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

#include PLATFORM_HEADER
#include "em_device.h"
// need micro.h for RESET_ defines and halInternalContextSaveRestore() prototype
#include "hal/micro/micro.h"
// need diagnostic.h for halResetInfo structure
#include "hal/micro/cortexm3/diagnostic.h"
// need memmap.h for HalVectorTableType in cstartup-common.h
#include "hal/micro/cortexm3/memmap.h"
// need cstartup-common.h for __vector_table
#include "hal/micro/cortexm3/cstartup-common.h"

#if defined (__ICCARM__)
  #define STACKLESS __noreturn __stackless
#elif defined (__GNUC__)
  #define STACKLESS __attribute__ ((noreturn, used, naked))
#else
  #error Compiler unknown for defining STACKLESS for SystemInit()
#endif

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
STACKLESS void SystemInit(void)
{
  // When the Cortex-M3 exits reset, interrupts are enable.  Explicitely
  // disable them immediately.
  __disable_irq();

  // VREG is a High Voltage register and retains its state across deep sleep.
  if ((CMHV->RESETEVENT & CMHV_RESETEVENT_DSLEEP) != CMHV_RESETEVENT_DSLEEP) {
    // Ensure there is enough margin on VREG_1V8 for stable RAM reads by
    // setting it to a code of 6.  VREG_1V2 can be left at its reset value.
    CMHV->VREGCTRL = (6 << _CMHV_VREGCTRL_1V8TRIM_SHIFT)
                     | CMHV_VREGCTRL_1V2TRIM_DEFAULT;
  }

  // Source FCLK from SYSCLK.  While faster execution causes a slight
  // increase in current consumption, faster FCLK reduces execution
  // time and overall energy usage goes down.
  // Reminder:
  //   if(CMLV->CPUCLKSEL & CMLV_CPUCLKSEL_CPUCLKSELFIELD == 1) {FCLK = SYSCLK}
  //   if(CMLV->CPUCLKSEL & CMLV_CPUCLKSEL_CPUCLKSELFIELD == 0) {FCLK = PCLK}
  CMLV->CPUCLKSEL = CMLV_CPUCLKSEL_CPUCLKSELFIELD;

  // Configure BASEPRI to be at the interrupts disabled level so that when we
  // turn interrupts back on nothing fires immediately. This must happen up here
  // or we will not do this during deepsleep startup and interrupts will fire
  // before we're done with the deepsleep wakeup.
  INTERRUPTS_OFF();

  // It is quite possible that when the Cortex-M3 begins executing code the
  // Core Reset Vector Catch is still left enabled.  Because this VC would
  // cause us to halt at reset if another reset event tripped, we should
  // clear it as soon as possible.  If a debugger wants to halt at reset,
  // it will set this bit again.
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_VC_CORERESET_Msk;

  //Configure flash access for optimal current consumption early
  //during boot to save as much current as we can.
  FLASHCTRL->ACCESS = (FLASHCTRL_ACCESS_FLASHLP
                       | FLASHCTRL_ACCESS_PREFETCHEN
                       //BugzId:13727 workaround
                       | (1 << _FLASHCTRL_ACCESS_CODELATENCY_SHIFT));

  // GPIO that aren't pinned out need to be set to output for lowest current
  // consumption
  SET_CMSIS_REG(GPIO->P[4].CFGH,
                (_GPIO_P_CFGH_Px4_MASK
                 | _GPIO_P_CFGH_Px5_MASK
                 | _GPIO_P_CFGH_Px6_MASK
                 | _GPIO_P_CFGH_Px7_MASK),
                (GPIO_P_CFGH_Px4_OUT
                 | GPIO_P_CFGH_Px5_OUT
                 | GPIO_P_CFGH_Px6_OUT
                 | GPIO_P_CFGH_Px7_OUT));
  SET_CMSIS_REG(GPIO->P[5].CFGL,
                (_GPIO_P_CFGL_Px0_MASK
                 | _GPIO_P_CFGL_Px1_MASK
                 | _GPIO_P_CFGL_Px2_MASK
                 | _GPIO_P_CFGL_Px3_MASK),
                (GPIO_P_CFGL_Px0_OUT
                 | GPIO_P_CFGL_Px1_OUT
                 | GPIO_P_CFGL_Px2_OUT
                 | GPIO_P_CFGL_Px3_OUT));
  SET_CMSIS_REG(GPIO->P[5].CFGL,
                (_GPIO_P_CFGH_Px4_MASK
                 | _GPIO_P_CFGH_Px5_MASK
                 | _GPIO_P_CFGH_Px6_MASK
                 | _GPIO_P_CFGH_Px7_MASK),
                (GPIO_P_CFGH_Px4_OUT
                 | GPIO_P_CFGH_Px5_OUT
                 | GPIO_P_CFGH_Px6_OUT
                 | GPIO_P_CFGH_Px7_OUT));

  ////---- Always remap the vector table ----////
  // We might be coming from a bootloader at the base of flash, or even in the
  // NULL_BTL case, the BAT/AAT will be at the beginning of the image.
  // Setting VECTOR_TABLE is required.
#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t)VECTOR_TABLE;
#endif

  ////---- Always Configure Interrupt Priorities ----////
  // Vector 1,2,3 priorities are fixed and not used.
  #undef FIXED_EXCEPTION
  #define FIXED_EXCEPTION(vectorNumber, functionName, deviceIrqn, deviceIrqHandler)
  #define EXCEPTION(vectorNumber, functionName, deviceIrqn, deviceIrqHandler, priorityLevel, subpriority)  \
  const uint32_t vect##vectorNumber##PriorityLevel = (0xFF                                                 \
                                                      & (((priorityLevel)     << ((PRIGROUP_POSITION) +1)) \
                                                         | ((subpriority) & ((1 << ((PRIGROUP_POSITION) +1)) - 1))));
    #include NVIC_CONFIG
  #undef  EXCEPTION

  // With optimization turned on, the compiler will identify all the values
  // and variables used here as constants at compile time and will truncate
  // this entire block of code to 98 bytes, comprised of 7 load-load-store
  // operations.
  //vect1PriorityLevel //vect01 is fixed
  //vect2PriorityLevel //vect02 is fixed
  //vect3PriorityLevel //vect03 is fixed
  *((volatile uint32_t *)&SCB->SHP[0])  = ( (vect4PriorityLevel << 0)
                                            | (vect5PriorityLevel << 8)
                                            | (vect6PriorityLevel << 16)
                                            | (vect7PriorityLevel << 24) );
  *((volatile uint32_t *)&SCB->SHP[4])  = ( (vect8PriorityLevel << 0)
                                            | (vect9PriorityLevel << 8)
                                            | (vect10PriorityLevel << 16)
                                            | (vect11PriorityLevel << 24) );
  *((volatile uint32_t *)&SCB->SHP[8])  = ( (vect12PriorityLevel << 0)
                                            | (vect13PriorityLevel << 8)
                                            | (vect14PriorityLevel << 16)
                                            | (vect15PriorityLevel << 24) );
  *((volatile uint32_t *)&NVIC->IP[0])  = ( (vect16PriorityLevel << 0)
                                            | (vect17PriorityLevel << 8)
                                            | (vect18PriorityLevel << 16)
                                            | (vect19PriorityLevel << 24) );
  *((volatile uint32_t *)&NVIC->IP[4])  = ( (vect20PriorityLevel << 0)
                                            | (vect21PriorityLevel << 8)
                                            | (vect22PriorityLevel << 16)
                                            | (vect23PriorityLevel << 24) );
  *((volatile uint32_t *)&NVIC->IP[8])  = ( (vect24PriorityLevel << 0)
                                            | (vect25PriorityLevel << 8)
                                            | (vect26PriorityLevel << 16)
                                            | (vect27PriorityLevel << 24) );
  *((volatile uint32_t *)&NVIC->IP[12]) = ( (vect28PriorityLevel << 0)
                                            | (vect29PriorityLevel << 8)
                                            | (vect30PriorityLevel << 16)
                                            | (vect31PriorityLevel << 24) );
  *((volatile uint32_t *)&NVIC->IP[16]) = ( (vect32PriorityLevel << 0)
                                            | (vect33PriorityLevel << 8)
                                            | (vect34PriorityLevel << 16)
                                            | (vect35PriorityLevel << 24) );
  #pragma diag_default=Pe177

  // Now that all the individual priority bits are set, we have to set the
  // distinction between preemptive priority and non-preemptive subpriority
  // This sets the priority grouping binary position.  The value 0x05FA0000
  // is a special key required to enable the write to this register.
  // PRIGROUP_POSITION is defined inside of nvic-config.h.
  SCB->AIRCR = (0x05FA0000 | (PRIGROUP_POSITION << SCB_AIRCR_PRIGROUP_Pos));

  ////---- Always Configure System Handlers Control and Configuration ----////
  SCB->CCR = SCB_CCR_DIV_0_TRP_Msk | SCB_CCR_STKALIGN_Msk;
  SCB->SHCSR = (SCB_SHCSR_USGFAULTENA_Msk
                | SCB_SHCSR_BUSFAULTENA_Msk
                | SCB_SHCSR_MEMFAULTENA_Msk);

  // Compiler seg_init occurs after the low level startup, so seg_init
  // won't be run at all due to halInternalContextSaveRestore() changing
  // code execution path when coming out of deep sleep.

  if (((CMHV->RESETEVENT & CMHV_RESETEVENT_DSLEEP) == CMHV_RESETEVENT_DSLEEP)
      && (!((halResetInfo.crash.resetSignature == RESET_VALID_SIGNATURE)
            && (halResetInfo.crash.resetReason == RESET_BOOTLOADER_DEEPSLEEP)))
      ) {
    // Do this when waking up from deep sleep, but only when *not* the
    // RESET_BOOTLOADER_DEEPSLEEP deep sleep.
    //
    //Since the first 13 NVIC vectors are fixed values, they are restored
    //when they get set during normal boot sequences, instead of inside of the
    //halSleep code.

    //NOTE: cstartup *must* not push-pop the stack until *after* deep sleep
    //      context restore has occurred.  Otherwise, cstartup would
    //      corrupt the existing stack data from prior to deep sleep!
    halInternalContextSaveRestore(false); //triggers a context restore from deep sleep
    //if the context restore worked properly, we should never return here
  }

  __enable_irq();

  // Since SystemInit creates a stack frame, and consumes (at present)
  // four additional words of stack at startup, it's important to
  // start the system executing with that stack frame in place so that
  // when we come back here on a reset following sleep, the stack frame
  // created by SystemInit won't overwrite part of the running program's
  // stack.

  Start_Handler();
}
