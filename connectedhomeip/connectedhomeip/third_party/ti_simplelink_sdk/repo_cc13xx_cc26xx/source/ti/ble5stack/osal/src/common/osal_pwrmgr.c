/******************************************************************************

 @file  osal_pwrmgr.c

 @brief This file contains the OSAL Power Management API.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include "comdef.h"
#include "onboard.h"
#include "osal.h"
#include "osal_tasks.h"
#include "osal_timers.h"
#include "osal_pwrmgr.h"

#ifdef USE_ICALL
#ifdef ICALL_JT
  #include "icall_jt.h"
#else
  #include <icall.h>
#endif /* ICALL_JT */
#endif /* USE_ICALL */

#ifdef OSAL_PORT2TIRTOS
/* Direct port to TI-RTOS API */
#if defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#elif defined(CC26XX)
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#endif /* CC26XX */
#endif /* OSAL_PORT2TIRTOS */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/* This global variable stores the power management attributes.
 */
pwrmgr_attribute_t pwrmgr_attribute;
#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
uint8 pwrmgr_initialized = FALSE;
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osal_pwrmgr_init
 *
 * @brief   Initialize the power management system.
 *
 * @param   none.
 *
 * @return  none.
 */
void osal_pwrmgr_init( void )
{
#if !defined USE_ICALL && !defined OSAL_PORT2TIRTOS
  pwrmgr_attribute.pwrmgr_device = PWRMGR_ALWAYS_ON; // Default to no power conservation.
#endif /* USE_ICALL */
  pwrmgr_attribute.pwrmgr_task_state = 0;            // Cleared.  All set to conserve
#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
  pwrmgr_initialized = TRUE;
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
}

#if !defined USE_ICALL && !defined OSAL_PORT2TIRTOS
/*********************************************************************
 * @fn      osal_pwrmgr_device
 *
 * @brief   Sets the device power characteristic.
 *
 * @param   pwrmgr_device - type of power devices. With PWRMGR_ALWAYS_ON
 *          selection, there is no power savings and the device is most
 *          likely on mains power. The PWRMGR_BATTERY selection allows the
 *          HAL sleep manager to enter sleep.
 *
 * @return  none
 */
void osal_pwrmgr_device( uint8 pwrmgr_device )
{
  pwrmgr_attribute.pwrmgr_device = pwrmgr_device;
}
#endif /* !defined USE_ICALL && !defined OSAL_PORT2TIRTOS*/

/*********************************************************************
 * @fn      osal_pwrmgr_task_state
 *
 * @brief   This function is called by each task to state whether or
 *          not this task wants to conserve power.
 *
 * @param   task_id - calling task ID.
 *          state - whether the calling task wants to
 *          conserve power or not.
 *
 * @return  TRUE if power is required; FALSE is power is not required
 */
uint8 osal_pwrmgr_task_state( uint8 task_id, uint8 state )
{
  halIntState_t intState;
  bool pwrRequired = TRUE;

  if ( task_id >= tasksCnt )
    return ( pwrRequired );

#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
  if ( !pwrmgr_initialized )
  {
    /* If voting is made before this module is initialized,
     * pwrmgr_task_state will reset later when the module is
     * initialized, and cause incorrect activity count.
     */
    return ( pwrRequired );
  }
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */

  HAL_ENTER_CRITICAL_SECTION( intState );

  if ( state == PWRMGR_CONSERVE )
  {
#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
    uint16 cache = pwrmgr_attribute.pwrmgr_task_state;
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
    // Clear the task state flag
    pwrmgr_attribute.pwrmgr_task_state &= ~(1 << task_id );
#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
    if (cache != 0 && pwrmgr_attribute.pwrmgr_task_state == 0)
    {
#ifdef USE_ICALL
      /* Decrement activity counter */
      pwrRequired = ICall_pwrUpdActivityCounter(FALSE);
#else /* USE_ICALL */
      Power_releaseConstraint(PowerCC26XX_SD_DISALLOW);
      Power_releaseConstraint(PowerCC26XX_SB_DISALLOW);
#endif /* USE_ICALL */
    }
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
  }
  else
  {
#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
    if (pwrmgr_attribute.pwrmgr_task_state == 0)
    {
#ifdef USE_ICALL
      /* Increment activity counter */
      (void)ICall_pwrUpdActivityCounter(TRUE);
#else /* USE_ICALL */
      Power_setConstraint(PowerCC26XX_SD_DISALLOW);
      Power_setConstraint(PowerCC26XX_SB_DISALLOW);
#endif /* USE_ICALL */
    }
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
    // Set the task state flag
    pwrmgr_attribute.pwrmgr_task_state |= (1 << task_id);
  }

  HAL_EXIT_CRITICAL_SECTION( intState );

  return ( pwrRequired );
}

#if defined( POWER_SAVING ) && !(defined USE_ICALL || defined OSAL_PORT2TIRTOS)
/*********************************************************************
 * @fn      osal_pwrmgr_powerconserve
 *
 * @brief   This function is called from the main OSAL loop when there are
 *          no events scheduled and shouldn't be called from anywhere else.
 *
 * @param   none.
 *
 * @return  none.
 */
void osal_pwrmgr_powerconserve( void )
{
  uint32        next;
  halIntState_t intState;

  // Should we even look into power conservation
  if ( pwrmgr_attribute.pwrmgr_device != PWRMGR_ALWAYS_ON )
  {
    // Are all tasks in agreement to conserve
    if ( pwrmgr_attribute.pwrmgr_task_state == 0 )
    {
      // Hold off interrupts.
      HAL_ENTER_CRITICAL_SECTION( intState );

      // Get next time-out
      next = osal_next_timeout();

      // Re-enable interrupts.
      HAL_EXIT_CRITICAL_SECTION( intState );

      // Put the processor into sleep mode
      OSAL_SET_CPU_INTO_SLEEP( next );
    }
  }
}
#endif /* POWER_SAVING */

/*********************************************************************
*********************************************************************/
