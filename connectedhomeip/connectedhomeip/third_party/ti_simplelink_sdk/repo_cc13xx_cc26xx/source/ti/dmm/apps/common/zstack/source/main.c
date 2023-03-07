/******************************************************************************

 @file  main.c

 @brief Main entry of the 15.4 & BLE remote display sample application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2021, Texas Instruments Incorporated
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

/******************************************************************************
 Includes
 *****************************************************************************/

#include <string.h>
#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <ioc.h>

#include "sys_ctrl.h"

#include "ti_drivers_config.h"

#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

/* Header files required for the temporary idle task function */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <aon_rtc.h>
#include <prcm.h>

#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>

//#include "crypto_board.h"
#include "crypto_mac_api.h"
#include <chipinfo.h>

/* Header files required to enable instruction fetch cache */
#include <vims.h>
#include <hw_memmap.h>

#include <ti/sysbios/hal/Hwi.h>
#include "pwrmon.h"

#include "cpu.h"

#include <icall.h>

#ifdef NV_RESTORE
#include "nvocmp.h"
#endif

#ifdef ZSTACK_START
#include "zstackstartup.h"
#if defined(DMM_LNT)
#include <ti/sysbios/knl/Semaphore.h>
#include "testapp_lnt.h"
#elif defined(DMM_ZEDSWITCH) || defined(DMM_ZCSWITCH)
#include "zcl_samplesw.h"
#elif defined(DMM_ZRLIGHT)
#include "zcl_samplelight.h"
#endif
#include "zstackconfig.h"
#include "zstackapi.h"
#endif

/* Include DMM module */
#include <dmm/dmm_scheduler.h>

#include "ti_dmm_application_policy.h"
#if ZG_BUILD_ENDDEVICE_TYPE
#include <dmm/dmm_priority_ble_zigbee_zed.h>
#elif ZG_BUILD_RTRONLY_TYPE
#include <dmm/dmm_priority_ble_zigbee_zr.h>
#elif ZG_BUILD_COORDINATOR_TYPE
#include <dmm/dmm_priority_ble_zigbee_zc.h>
#endif

#include "remote_display.h"
#include "ble_user_config.h"

#ifndef CUI_DISABLE
#include "cui.h"
#endif /* CUI_DISABLE */

#ifdef DMM_OAD
#include <profiles/oad/cc26xx/mark_switch_factory_img.h>
#endif

#ifdef USE_ITM_DBG
#include "itm.h"
#include "ioc.h"
#endif

// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;

/******************************************************************************
 Constants
 *****************************************************************************/

#define CONFIG_PHY_ID 0

/* Assert Reasons */
#define MAIN_ASSERT_ICALL        2
#define MAIN_ASSERT_MAC          3
#define MAIN_ASSERT_HWI_TIRTOS   4

#define RFC_MODE_BLE                 PRCM_RFCMODESEL_CURR_MODE1
#define RFC_MODE_IEEE                PRCM_RFCMODESEL_CURR_MODE2
#define RFC_MODE_ANT                 PRCM_RFCMODESEL_CURR_MODE4
#define RFC_MODE_EVERYTHING_BUT_ANT  PRCM_RFCMODESEL_CURR_MODE5
#define RFC_MODE_EVERYTHING          PRCM_RFCMODESEL_CURR_MODE6

/* Extended Address offset in FCFG (LSB..MSB) */
#define EXTADDR_OFFSET 0x2F0


#define APP_TASK_STACK_SIZE 3000

#define SET_RFC_MODE(mode) HWREG( PRCM_BASE + PRCM_O_RFCMODESEL ) = (mode)

// Exented Address Length
#define EXTADDR_LEN 8

// Macro used to break a uint32_t into individual bytes
//#define BREAK_UINT32(var, ByteNum) \
//    (uint8)((uint32)(((var) >> ((ByteNum) * 8)) & 0x00FF))

/******************************************************************************
 External Variables
 *****************************************************************************/
//! \brief Customer configuration area.
//!
//extern const ccfg_t __ccfg;

extern void sampleApp_task(NVINTF_nvFuncts_t *pfnNV);

/******************************************************************************
 Global Variables
 *****************************************************************************/

Task_Struct zstackAppTask;
Char zstackAppTaskStack[APP_TASK_STACK_SIZE];

#ifdef ZSTACK_START
#if !defined(TIMAC_ROM_IMAGE_BUILD)
/* Crypto driver function table */
uint32_t *macCryptoDrvTblPtr = (uint32_t*)macCryptoDriverTable;
#endif
#endif

/*
 When assert happens, this field will be filled with the reason:
       MAIN_ASSERT_HWI_TIRTOS,
       MAIN_ASSERT_ICALL,
       MAIN_ASSERT_MAC
 */
uint8 Main_assertReason = 0;

/******************************************************************************
 * ZStack Configuration Structure
 */
#ifdef ZSTACK_START
zstack_Config_t zstack_user0Cfg =
{
    {0, 0, 0, 0, 0, 0, 0, 0}, // Extended Address
    {0, 0, 0, 0, 0, 0, 0, 0}, // NV function pointers
    0,                        // ICall application thread ID
    0,                        // stack image init fail flag
    MAC_USER_CFG
};

/* MAC user defined configuration */
macUserCfg_t macUser0Cfg[] = MAC_USER_CFG;
#endif

#ifdef DMM_LNT
/* Stack TIRTOS Task semaphore */
Semaphore_Struct npiInitializationMutex;
Semaphore_Handle npiInitializationMutexHandle;
#endif

/******************************************************************************
 Local Variables
 *****************************************************************************/

static uint8_t stackServiceTaskId;

/* Used to check for a valid extended address */
static const uint8_t dummyExtAddr[] =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*!
 * @brief       Fill in your own assert function.
 *
 * @param       assertReason - reason: MAIN_ASSERT_HWI_TIRTOS,
 *                                     MAIN_ASSERT_ICALL, or
 *                                     MAIN_ASSERT_MAC
 */
void Main_assertHandler(uint8_t assertReason)
{
    Main_assertReason = assertReason;

#if defined(RESET_ASSERT)
     /* Pull the plug and start over */
    SysCtrlSystemReset();
#else
    Hwi_disable();
    while(1)
    {
        /* Put you code here to do something if in assert */
    }
#endif
}

#ifdef ZSTACK_START
/*!
 * @brief       Main task function
 *
 * @param       a0 -
 * @param       a1 -
 */
Void zstackAppTaskFxn(UArg a0, UArg a1)
{
#if defined(USE_CACHE_RAM)
    /* Retain the Cache RAM */
    Power_setConstraint(PowerCC26XX_SB_VIMS_CACHE_RETAIN);
#endif

    /* get the service taskId of the Stack */
    stackServiceTaskId = stackTask_getStackServiceId();

    /* configure the message API the application will use to communicate with
       the stack */
    Zstackapi_init(stackServiceTaskId);

    /* Kick off application */
    sampleApp_task(&zstack_user0Cfg.nvFps);
}
#endif

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
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_HWI_TIRTOS);
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
      HAL_ASSERT_SPINLOCK;
      break;

    case HAL_ASSERT_CAUSE_ICALL_TIMEOUT:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> ICALL TIMEOUT!", true);
#endif /* CUI_DISABLE */
      HAL_ASSERT_SPINLOCK;
      break;

    case HAL_ASSERT_CAUSE_WRONG_API_CALL:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> WRONG API CALL!", true);
#endif /* CUI_DISABLE */
      HAL_ASSERT_SPINLOCK;
      break;

  default:
#ifndef CUI_DISABLE
      CUI_assert("***ERROR*** >> DEFAULT SPINLOCK!", true);
#endif /* CUI_DISABLE */
      HAL_ASSERT_SPINLOCK;
  }

  return;
}

void assertHandler(void)
{
    // Call the more detailed AssertHandler
    AssertHandler(HAL_ASSERT_CAUSE_TRUE, 0x00);
}

/*!
 * @brief       "main()" function - starting point
 */
Void main()
{
#ifdef BLE_START
    Task_Handle* pBleTaskHndl;
#endif
#ifdef ZSTACK_START
    Task_Handle* pZstackTaskkHndl;
#endif
    Task_Params taskParams;
    DMMPolicy_Params dmmPolicyParams;
    DMMSch_Params dmmSchedulerParams;

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

#ifdef BLE_START
    /* Initialize ICall module */
    ICall_init();

    /* Start tasks of external images */
    ICall_createRemoteTasks();
    pBleTaskHndl = ICall_getRemoteTaskHandle(0);

    RemoteDisplay_createTask();
#endif

#ifdef ZSTACK_START

#ifndef USE_DEFAULT_USER_CFG
    macUser0Cfg[0].pAssertFP = assertHandler;
#endif

#ifndef CUI_DISABLE
    /* Initialize UI for key and LED */
    CUI_params_t cuiParams;
    CUI_paramsInit(&cuiParams);

#if defined(USE_ITM_DBG) || defined(DMM_LNT)
    cuiParams.manageUart = false;
#endif

#ifndef Z_POWER_TEST
    // One-time initialization of the CUI
    CUI_init(&cuiParams);
#endif
#endif /* CUI_DISABLE */

#ifdef USE_ITM_DBG
    // Configure SWO Traces on pin 26
    IOCPortConfigureSet(26, IOC_PORT_MCU_SWV, IOC_STD_OUTPUT);
    ITM_config itm_config =
    {
      48000000,
      ITM_6000000
    };
    ITM_initModule(itm_config);
    ITM_enableModule();
#endif /* USE_ITM_DBG */

    /*
     * Copy the extended address from the CCFG area
     * Assumption: the memory in CCFG_IEEE_MAC_0 and CCFG_IEEE_MAC_1
     * is contiguous and LSB first.
     */
    memcpy(zstack_user0Cfg.extendedAddress, (uint8_t *)&(__ccfg.CCFG_IEEE_MAC_0),
           (APIMAC_SADDR_EXT_LEN));

    /* Check to see if the CCFG IEEE is valid */
    if(memcmp(zstack_user0Cfg.extendedAddress, dummyExtAddr, APIMAC_SADDR_EXT_LEN) == 0)
    {
        /* No, it isn't valid.  Get the Primary IEEE Address */
        memcpy(zstack_user0Cfg.extendedAddress, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET),
                       (APIMAC_SADDR_EXT_LEN));
    }

#ifdef NV_RESTORE
    /* Setup the NV driver */
    NVOCMP_loadApiPtrs(&zstack_user0Cfg.nvFps);
#if !defined(BLE_START) && defined(ZSTACK_START)
	// The init is done by BLE
    if(zstack_user0Cfg.nvFps.initNV)
    {
        zstack_user0Cfg.nvFps.initNV( NULL);
    }
#endif
#endif

    /* configure stack task */
    stackTask_init(&zstack_user0Cfg);
    pZstackTaskkHndl = stackTaskGetTaskHndl();

#ifdef DMM_LNT
    extern void NPITask_createTask(void);
    NPITask_createTask();

    Semaphore_Params semParam;
    Semaphore_Params_init(&semParam);
    semParam.mode = ti_sysbios_knl_Semaphore_Mode_COUNTING;
    Semaphore_construct(&npiInitializationMutex, 0, &semParam);
    npiInitializationMutexHandle = Semaphore_handle(&npiInitializationMutex);
#endif

    /* Configure app task. */
    Task_Params_init(&taskParams);
    taskParams.stack = zstackAppTaskStack;
    taskParams.stackSize = APP_TASK_STACK_SIZE;
    taskParams.priority = 1;
    Task_construct(&zstackAppTask, zstackAppTaskFxn, &taskParams, NULL);
#endif

    /* initialize and open the DMM policy manager */
    DMMPolicy_init();
    DMMPolicy_Params_init(&dmmPolicyParams);
    dmmPolicyParams.numPolicyTableEntries = DMMPolicy_ApplicationPolicySize;
    dmmPolicyParams.policyTable = DMMPolicy_ApplicationPolicyTable;
#ifdef ZSTACK_START
#if ZG_BUILD_ENDDEVICE_TYPE
    dmmPolicyParams.globalPriorityTable = globalPriorityTable_bleLzigbeeZedH;
#elif ZG_BUILD_RTRONLY_TYPE
    dmmPolicyParams.globalPriorityTable = globalPriorityTable_bleLzigbeeZrH;
#elif ZG_BUILD_COORDINATOR_TYPE
    dmmPolicyParams.globalPriorityTable = globalPriorityTable_bleLzigbeeZcH;
#endif
#endif

    DMMPolicy_open(&dmmPolicyParams);

    /* initialize and open the DMM scheduler */
    DMMSch_init();
    DMMSch_Params_init(&dmmSchedulerParams);

    //Copy stack roles and index table
    memcpy(dmmSchedulerParams.stackRoles, DMMPolicy_ApplicationPolicyTable.stackRole, sizeof(DMMPolicy_StackRole) * DMMPOLICY_NUM_STACKS);
    dmmSchedulerParams.indexTable = DMMPolicy_ApplicationPolicyTable.indexTable;
    DMMSch_open(&dmmSchedulerParams);

    /* register clients with DMM scheduler */
#ifdef BLE_START
    DMMSch_registerClient(pBleTaskHndl, DMMPolicy_StackRole_BlePeripheral);
#endif
#ifdef ZSTACK_START
#if ZG_BUILD_ENDDEVICE_TYPE
    DMMSch_registerClient(pZstackTaskkHndl, DMMPolicy_StackRole_ZigbeeEndDevice);
#elif ZG_BUILD_RTRONLY_TYPE
    DMMSch_registerClient(pZstackTaskkHndl, DMMPolicy_StackRole_ZigbeeRouter);
#elif ZG_BUILD_COORDINATOR_TYPE
    DMMSch_registerClient(pZstackTaskkHndl, DMMPolicy_StackRole_ZigbeeCoordinator);
#endif
#endif

    /* set the stacks in default states */
#ifdef BLE_START
    DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_IDLE);
#endif
#ifdef ZSTACK_START
#if ZG_BUILD_ENDDEVICE_TYPE
    DMMPolicy_updateStackState(DMMPolicy_StackRole_ZigbeeEndDevice, DMMPOLICY_ZB_UNINIT);
#elif ZG_BUILD_RTRONLY_TYPE
    DMMPolicy_updateStackState(DMMPolicy_StackRole_ZigbeeRouter, DMMPOLICY_ZB_UNINIT);
#elif ZG_BUILD_COORDINATOR_TYPE
    DMMPolicy_updateStackState(DMMPolicy_StackRole_ZigbeeCoordinator, DMMPOLICY_ZB_UNINIT);
#endif
#endif

#ifdef DEBUG_SW_TRACE
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT
                    | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
#endif /* DEBUG_SW_TRACE */

    BIOS_start(); /* enable interrupts and start SYS/BIOS */
}
/*******************************************************************************
 */
