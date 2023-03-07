/******************************************************************************

 @file  main.c

 @brief main entry of the BLE stack sample application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2018-2021, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */

#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/BIOS.h>

#include <icall.h>
#include "hal_assert.h"
#include "bcomdef.h"
#include "remote_display.h"
#ifndef CUI_DISABLE
#include "cui.h"
#endif /* CUI_DISABLE */

/* Header files required to enable instruction fetch cache */
#include <inc/hw_memmap.h>
#include <driverlib/vims.h>

#ifndef USE_DEFAULT_USER_CFG
#include "ble_user_config.h"
// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;
#endif // USE_DEFAULT_USER_CFG

#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>

/* Wsn Application Header files */
#include "wsn_node/NodeRadioTask.h"
#include "wsn_node/NodeTask.h"

/* Include DMM module */
#include <dmm/dmm_scheduler.h>
#include "ti_dmm_application_policy.h"
#include <dmm/dmm_priority_ble_wsn.h>

#if defined(RESET_ASSERT)
#include <driverlib/sys_ctrl.h>
#endif

#ifdef DMM_OAD
#include <profiles/oad/cc26xx/mark_switch_factory_img.h>
#endif

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * EXTERNS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);


/*******************************************************************************
 * @fn          Main
 *
 * @brief       Application Main
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
int main()
{
  Task_Handle* pBleTaskHndl;
  Task_Handle* pWsnTaskHndl;
  DMMPolicy_Params dmmPolicyParams;
  DMMSch_Params dmmSchedulerParams;
#ifndef CUI_DISABLE
  CUI_params_t cuiParams;
#endif /* CUI_DISABLE */

  /* Register Application callback to trap asserts raised in the Stack */
  RegisterAssertCback(AssertHandler);

  /* GPIO init must be called prior to Board_initGeneral for proper pin configuration */
  GPIO_init();
  Board_initGeneral();

#ifdef DMM_OAD
    /* If DMM_OAD is enabled, look for a left button
     *  press on reset. This indicates to revert to some
     *  factory image
     */
    if (!GPIO_read(CONFIG_GPIO_BTN1))
    {
        markSwitchFactoryImg();
    }
#endif /* DMM_OAD */

  // Enable iCache prefetching
  VIMSConfigure(VIMS_BASE, TRUE, TRUE);
  // Enable cache
  VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);

#if !defined( POWER_SAVING )
  /* Set constraints for Standby, powerdown and idle mode */
  // PowerCC26XX_SB_DISALLOW may be redundant
  Power_setConstraint(PowerCC26XX_SB_DISALLOW);
  Power_setConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
#endif // POWER_SAVING

  /* Update User Configuration of the stack */
  user0Cfg.appServiceInfo->timerTickPeriod = Clock_tickPeriod;
  user0Cfg.appServiceInfo->timerMaxMillisecond  = ICall_getMaxMSecs();

#ifndef CUI_DISABLE
  /* Initialize CUI UART */
  CUI_paramsInit(&cuiParams);

#ifndef POWER_MEAS_DMM_WSN
  /* One-time initialization of the CUI */
  CUI_init(&cuiParams);
#endif
#endif /* CUI_DISABLE */

  /* Initialize ICall module */
  ICall_init();

  /* Start tasks of external images */
  ICall_createRemoteTasks();
  pBleTaskHndl = ICall_getRemoteTaskHandle(0);

  RemoteDisplay_createTask();

  /* Initialize wsn sensor node tasks */
  pWsnTaskHndl = NodeRadioTask_init();
  NodeTask_init();

  /* initialize and open the DMM policy manager */
  DMMPolicy_init();
  DMMPolicy_Params_init(&dmmPolicyParams);
  dmmPolicyParams.numPolicyTableEntries = DMMPolicy_ApplicationPolicySize;
  dmmPolicyParams.policyTable = DMMPolicy_ApplicationPolicyTable;
  dmmPolicyParams.globalPriorityTable = globalPriorityTable_bleLwsnH;
  DMMPolicy_open(&dmmPolicyParams);

  /* initialize and open the DMM scheduler */
  DMMSch_init();
  DMMSch_Params_init(&dmmSchedulerParams);

  //Copy stack roles and index table
  memcpy(dmmSchedulerParams.stackRoles, DMMPolicy_ApplicationPolicyTable.stackRole, sizeof(DMMPolicy_StackRole) * DMMPOLICY_NUM_STACKS);
  dmmSchedulerParams.indexTable = DMMPolicy_ApplicationPolicyTable.indexTable;
  DMMSch_open(&dmmSchedulerParams);

  /* register clients with DMM scheduler */
  DMMSch_registerClient(pBleTaskHndl, DMMPolicy_StackRole_BlePeripheral);
  DMMSch_registerClient(pWsnTaskHndl, DMMPolicy_StackRole_WsnNode);

  /* set the stacks in default states */
  DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_ADV);
  DMMPolicy_updateApplicationState(DMMPolicy_StackRole_WsnNode, DMMPOLICY_WSN_SLEEPING);

  /* enable interrupts and start SYS/BIOS */
  BIOS_start();

  return 0;
}

/*!
 * @brief       TIRTOS HWI Handler.  The name of this function is set to
 *              M3Hwi.excHandlerFunc in app.cfg, you can disable this by
 *              setting it to null.
 *
 * @param       excStack - TIROS variable
 * @param       lr - TIROS variable
 */
xdc_Void Main_excHandler(UInt *excStack, UInt lr)
{
#if defined(RESET_ASSERT)
    /* Pull the plug and start over */
    SysCtrlSystemReset();
#else
    //spin here
    while(1);
#endif
}

/*******************************************************************************
 * @fn          AssertHandler
 *
 * @brief       This is the Application's callback handler for asserts raised
 *              in the stack.  When EXT_HAL_ASSERT is defined in the Stack
 *              project this function will be called when an assert is raised,
 *              and can be used to observe or trap a violation from expected
 *              behavior.
 *
 *              As an example, for Heap allocation failures the Stack will raise
 *              HAL_ASSERT_CAUSE_OUT_OF_MEMORY as the assertCause and
 *              HAL_ASSERT_SUBCAUSE_NONE as the assertSubcause.  An application
 *              developer could trap any malloc failure on the stack by calling
 *              HAL_ASSERT_SPINLOCK under the matching case.
 *
 *              An application developer is encouraged to extend this function
 *              for use by their own application.  To do this, add hal_assert.c
 *              to your project workspace, the path to hal_assert.h (this can
 *              be found on the stack side). Asserts are raised by including
 *              hal_assert.h and using macro HAL_ASSERT(cause) to raise an
 *              assert with argument assertCause.  the assertSubcause may be
 *              optionally set by macro HAL_ASSERT_SET_SUBCAUSE(subCause) prior
 *              to asserting the cause it describes. More information is
 *              available in hal_assert.h.
 *
 * input parameters
 *
 * @param       assertCause    - Assert cause as defined in hal_assert.h.
 * @param       assertSubcause - Optional assert subcause (see hal_assert.h).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void AssertHandler(uint8 assertCause, uint8 assertSubcause)
{

  // check the assert cause
  switch (assertCause)
  {
    case HAL_ASSERT_CAUSE_OUT_OF_MEMORY:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> OUT OF MEMORY!", false);
#endif /* CUI_DISABLE */
      break;

    case HAL_ASSERT_CAUSE_INTERNAL_ERROR:
#ifndef CUI_DISABLE
      // check the subcause
      if (assertSubcause == HAL_ASSERT_SUBCAUSE_FW_INERNAL_ERROR)
      {
        CUI_assert("***ERROR*** >> INTERNAL FW ERROR!", false);
      }
      else
      {
        CUI_assert("***ERROR*** >> INTERNAL ERROR!", false);
      }
#endif /* CUI_DISABLE */
      break;

    case HAL_ASSERT_CAUSE_ICALL_ABORT:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> ICALL ABORT!", true);
#endif /* CUI_DISABLE */
      break;

    case HAL_ASSERT_CAUSE_ICALL_TIMEOUT:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> ICALL TIMEOUT!", true);
#endif /* CUI_DISABLE */
      break;

    case HAL_ASSERT_CAUSE_WRONG_API_CALL:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> WRONG API CALL!", true);
#endif /* CUI_DISABLE */
      break;

  default:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> DEFAULT SPINLOCK!", true);
#endif /* CUI_DISABLE */
      break;
  }

  return;
}


/*******************************************************************************
 * @fn          smallErrorHook
 *
 * @brief       Error handler to be hooked into TI-RTOS.
 *
 * input parameters
 *
 * @param       eb - Pointer to Error Block.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void smallErrorHook(Error_Block *eb)
{
  for (;;);
}

/*******************************************************************************
 */
