/******************************************************************************

 @file  main.c

 @brief Main entry of the 15.4 & BLE remote display sample application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2020-2021, Texas Instruments Incorporated
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


/* Standard Library Header files */
#include <ioc.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


/* Header files required to enable instruction fetch cache */
#include <vims.h>
#include <hw_memmap.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

#include <chipinfo.h>
#include <aon_rtc.h>
#include <prcm.h>

#include "cpu.h"
#include "pwrmon.h"

#ifdef FREERTOS
#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>
#endif

#ifndef FREERTOS
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#else
/* For bget Heap init */
#include <bget.h>
#include <pthread.h>
#define TOTAL_ICALL_HEAP_SIZE (24000)

#if !defined(__GNUC__)
#pragma DATA_SECTION(GlobalHeapZoneBuffer,".heap");
uint8_t       GlobalHeapZoneBuffer[TOTAL_ICALL_HEAP_SIZE];
uint32_t heapSize = TOTAL_ICALL_HEAP_SIZE;
#else

__attribute__ ((section(".heap")))
uint8_t       GlobalHeapZoneBuffer[TOTAL_ICALL_HEAP_SIZE];
uint32_t heapSize = TOTAL_ICALL_HEAP_SIZE;
#endif //__GNUC__
#endif

#include "sys_ctrl.h"

#include "ti_drivers_config.h"
#ifdef THREAD_START
#include <ti/drivers/GPIO.h>
#include <ti/drivers/NVS.h>
#include <ti/drivers/UART.h>

#include <ti/drivers/AESECB.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/ECJPAKE.h>
#include <ti/drivers/dpl/HwiP.h>
/* Header files required for the temporary idle task function */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* Private configuration Header files */
#include "task_config.h"
/* Example/Board Header files */
#include "ti_drivers_config.h"
#include "otstack.h"
#else
#include <ti/drivers/dpl/HwiP.h>
#endif

#ifndef USEOT
#include "crypto_mac_api.h"
#endif

#include <icall.h>
#include "ble_user_config.h"

#ifdef NV_RESTORE
#include "nvocmp.h"
#endif

/* Include DMM module */
#include <dmm/dmm_scheduler.h>
#include <dmm/dmm_policy.h>

#include "ti_dmm_application_policy.h"
#include <dmm/dmm_priority_ble_thread.h>
#include "remote_display.h"
#include "cui.h"

#ifdef DMM_OAD
#include <ti/drivers/GPIO.h>
#include <profiles/oad/cc26xx/mark_switch_factory_img.h>
#endif

#ifdef USE_ITM_DBG
#include <ti/drivers/ITM.h>
#endif

// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;

/******************************************************************************
 Constants
 *****************************************************************************/
#include MBEDTLS_CONFIG_FILE
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


/******************************************************************************
 External Variables
 *****************************************************************************/
//! \brief Customer configuration area.
//!
//extern const ccfg_t __ccfg;

extern void sampleApp_task(NVINTF_nvFuncts_t *pfnNV);
#ifdef FREERTOS
void mainFxn(void *arg0);
#endif
/******************************************************************************
 Global Variables
 *****************************************************************************/
/*
 When assert happens, this field will be filled with the reason:
       MAIN_ASSERT_HWI_TIRTOS,
       MAIN_ASSERT_ICALL,
       MAIN_ASSERT_MAC
 */
uint8 Main_assertReason = 0;

/******************************************************************************
 Local Variables
 *****************************************************************************/

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
    HwiP_disable();
    while(1)
    {
        /* Put you code here to do something if in assert */
    }
#endif
}

/*!
 * @brief       TIRTOS HWI Handler.  The name of this function is set to
 *              M3Hwi.excHandlerFunc in app.cfg, you can disable this by
 *              setting it to null.
 *
 * @param       excStack - TIROS variable
 * @param       lr - TIROS variable
 */
#ifndef FREERTOS
xdc_Void Main_excHandler(UInt *excStack, UInt lr)
#else
void Main_excHandler(void *excStack, uint32_t lr)
#endif
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
      CUI_assert("***ERROR*** >> OUT OF MEMORY!", false);
      break;

    case HAL_ASSERT_CAUSE_INTERNAL_ERROR:
      // check the subcause
      if (assertSubcause == HAL_ASSERT_SUBCAUSE_FW_INERNAL_ERROR)
      {
        CUI_assert("***ERROR*** >> INTERNAL FW ERROR!", false);
      }
      else
      {
        CUI_assert("***ERROR*** >> INTERNAL ERROR!", false);
      }
      break;

    case HAL_ASSERT_CAUSE_ICALL_ABORT:
      CUI_assert("***ERROR*** >> ICALL ABORT!", true);
      HAL_ASSERT_SPINLOCK;
      break;

    case HAL_ASSERT_CAUSE_ICALL_TIMEOUT:
      CUI_assert("***ERROR*** >> ICALL TIMEOUT!", true);
      HAL_ASSERT_SPINLOCK;
      break;

    case HAL_ASSERT_CAUSE_WRONG_API_CALL:
      CUI_assert("***ERROR*** >> WRONG API CALL!", true);
      HAL_ASSERT_SPINLOCK;
      break;

  default:
      CUI_assert("***ERROR*** >> DEFAULT SPINLOCK!", true);
      HAL_ASSERT_SPINLOCK;
  }

  return;
}

void assertHandler(void)
{
    // Call the more detailed AssertHandler
    AssertHandler(HAL_ASSERT_CAUSE_TRUE, 0x00);
}



void mainFxn(void *arg0)
{
#ifndef FREERTOS
    Task_Handle* pBleTaskHndl;
#else
    TaskHandle_t pBleTaskHndl;
#endif
    DMMPolicy_Params dmmPolicyParams;
    DMMSch_Params dmmSchedulerParams;

    /* Update User Configuration of the stack */
    user0Cfg.appServiceInfo->timerTickPeriod = ICall_getTickPeriod();
    user0Cfg.appServiceInfo->timerMaxMillisecond  = ICall_getMaxMSecs();

    CUI_params_t cuiParams;
    CUI_paramsInit(&cuiParams);
    CUI_init(&cuiParams);

    /* initialize and open the DMM policy manager */
    DMMPolicy_init();
    DMMPolicy_Params_init(&dmmPolicyParams);
    dmmPolicyParams.numPolicyTableEntries = DMMPolicy_ApplicationPolicySize;
    dmmPolicyParams.policyTable = DMMPolicy_ApplicationPolicyTable;
    dmmPolicyParams.globalPriorityTable = globalPriorityTable_bleLthreadH;
    DMMPolicy_open(&dmmPolicyParams);

    /* initialize and open the DMM scheduler */
    DMMSch_init();
    DMMSch_Params_init(&dmmSchedulerParams);

    //Copy stack roles and index table
    memcpy(dmmSchedulerParams.stackRoles, DMMPolicy_ApplicationPolicyTable.stackRole, sizeof(DMMPolicy_StackRole) * DMMPOLICY_NUM_STACKS);
    dmmSchedulerParams.indexTable = DMMPolicy_ApplicationPolicyTable.indexTable;
    DMMSch_open(&dmmSchedulerParams);

#ifdef BLE_START
    /* Initialize ICall module */
    ICall_init();

    /* Start tasks of external images */
    ICall_createRemoteTasks();
#ifdef FREERTOS
    pBleTaskHndl = (TaskHandle_t)(*((TaskHandle_t* )ICall_getRemoteTaskHandle(0)));
#else
    pBleTaskHndl = ICall_getRemoteTaskHandle(0);
#endif

    RemoteDisplay_createTask();
#endif

#ifdef THREAD_START
    GPIO_init();

    NVS_init();

    UART_init();

    ECJPAKE_init();

    AESECB_init();

    SHA2_init();

    Thermostat_taskCreate();
#endif

#ifdef USE_ITM_DBG
    /* Open the ITM driver, spin here on fail */
    if (false == ITM_open())
    {
        /* Failed to open ITM driver, check for SWO pin conflicts */
        while(1);
    }
    ITM_disableExceptionTrace();
    ITM_disablePCAndEventSampling();

#endif

#ifdef BLE_START
    /* register clients with DMM scheduler */
#ifndef FREERTOS
    DMMSch_registerClient((Task_Handle *)pBleTaskHndl, DMMPolicy_StackRole_BlePeripheral);
#else
    DMMSch_registerClient((TaskHandle_t)pBleTaskHndl, DMMPolicy_StackRole_BlePeripheral);
#endif
    /* set the stacks in default states */
    DMMPolicy_updateStackState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_IDLE);
#endif
#ifdef FREERTOS
    vTaskDelete( NULL );
#endif
}
/*!
 * @brief       "main()" function - starting point
 */
#ifndef FREERTOS
Void main()
#else
int main()
#endif
{
#ifdef FREERTOS
    TaskHandle_t        thread;
#endif
    /* Register Application callback to trap asserts raised in the Stack */
    RegisterAssertCback(AssertHandler);

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

#ifdef FREERTOS
  bpool((void*)GlobalHeapZoneBuffer,TOTAL_ICALL_HEAP_SIZE);
#endif

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

    /* Create temporary wrapper thread for Task creation to allow for PThread API
     * that have limitations in main() */

#ifdef FREERTOS
     BaseType_t xReturned;

     /* Create the task, storing the handle. */
     xReturned = xTaskCreate(
             mainFxn,                  /* Function that implements the task. */
             "main",                   /* Text name for the task. */
             1024 / sizeof(uint32_t),  /* Stack size in words, not bytes. */
             ( void * ) NULL,          /* Parameter passed into the task. */
             5,                        /* Priority at which the task is created. */
             &thread );                /* Used to pass out the created task's handle. */

     if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
     {
         /* Creation of FreeRTOS task failed */
         while(1);
     }
#else
     mainFxn(NULL);
#endif

#ifdef DEBUG_SW_TRACE
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT
                    | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
#endif /* DEBUG_SW_TRACE */


#ifndef FREERTOS
    BIOS_start(); /* enable interrupts and start SYS/BIOS */

#else
/* Start the FreeRTOS scheduler */
 vTaskStartScheduler();
 return 0;
#endif /* FREERTOS */
}

#ifdef FREERTOS

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

#endif
/*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!         In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
******************************************************************************/

#if defined (__GNUC__)
void * _sbrk(uint32_t delta)
{
    extern char _end;     /* Defined by the linker */
    extern char __HeapLimit;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__HeapLimit;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}
#endif
/*******************************************************************************
 */
