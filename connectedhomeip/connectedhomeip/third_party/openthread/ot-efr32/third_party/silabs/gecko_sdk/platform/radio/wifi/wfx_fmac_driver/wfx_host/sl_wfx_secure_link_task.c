/***************************************************************************//**
 * @file sl_wfx_secure_link_task.c
 * @brief WFX FMAC driver host Secure Link task
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "secure_link/sl_wfx_secure_link.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>

// Securelink Task Configurations
#define WFX_SECURELINK_TASK_PRIO        24u
#define WFX_SECURELINK_TASK_STK_SIZE   512u

//Task Data Structures
static CPU_STK wfx_securelink_task_stack[WFX_SECURELINK_TASK_STK_SIZE];
OS_TCB wfx_securelink_task_tcb;

OS_MUTEX   wfx_securelink_rx_mutex;

/*
 * The task that implements the securelink renegotiation with WFX.
 */
static void wfx_securelink_task(void *p_arg)
{
  RTOS_ERR err;
  sl_status_t result;
  (void)p_arg;

  OSMutexCreate(&wfx_securelink_rx_mutex, "wfx secure link RX mutex", &err);
  for (;; ) {
    OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, 0, &err);
    result = sl_wfx_secure_link_renegotiate_session_key();
    if (result != SL_STATUS_OK) {
      printf("session key negotiation error %lu\n", result);
    }
  }
}

/***************************************************************************//**
 * @brief Creates WF200 securelink key renegotiation task.
 ******************************************************************************/
void wfx_securelink_task_start(void)
{
  RTOS_ERR err;

  OSTaskCreate(&wfx_securelink_task_tcb,
               "WFX SecureLink Task",
               wfx_securelink_task,
               DEF_NULL,
               WFX_SECURELINK_TASK_PRIO,
               &wfx_securelink_task_stack[0],
               (WFX_SECURELINK_TASK_STK_SIZE / 10u),
               WFX_SECURELINK_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}
