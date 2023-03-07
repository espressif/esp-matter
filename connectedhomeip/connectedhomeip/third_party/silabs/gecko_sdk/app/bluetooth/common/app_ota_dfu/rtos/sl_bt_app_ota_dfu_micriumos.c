/***************************************************************************//**
 * @file
 * @brief Core Application OTA DFU MicriumOS logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "os.h"
#include "em_common.h"
#include "app_assert.h"
#include "sl_bt_app_ota_dfu.h"
#include "sli_bt_app_ota_dfu.h"

#include "btl_interface.h"
#include "btl_interface_storage.h"

/// Task names and stacksize.
#define APP_OTA_DFU_TASK_PRIO         5u
#define APP_OTA_DFU_TASK_STACK        (2048u / sizeof(CPU_STK))

/// Task stack definition and identifier.
static CPU_STK app_ota_dfu_task_stack[APP_OTA_DFU_TASK_STACK];
static OS_TCB app_ota_dfu_task_tcb;

/// Semaphore to run the task only if its really necessary.
static OS_SEM semaphoreOn;

static sl_bt_app_ota_dfu_status_evt_t ota_event;

/// Task function prototype.
static void app_ota_dfu_task(void *p_arg);

/**************************************************************************//**
 * Strong sl_bt_app_ota_dfu_init() function implementation for MicriumOS.
 *****************************************************************************/
void sl_bt_app_ota_dfu_init(void)
{
  RTOS_ERR err;

  // Initialize semaphore for scheduling.
  OSSemCreate(&semaphoreOn, "Semaphore On", 0, &err);

  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Semaphore create failed!",
             (int)err.Code);

  OSTaskCreate(&app_ota_dfu_task_tcb, "App OTA DFU Task", app_ota_dfu_task,
               0u, APP_OTA_DFU_TASK_PRIO, &app_ota_dfu_task_stack[0u],
               (APP_OTA_DFU_TASK_STACK / 10u),
               APP_OTA_DFU_TASK_STACK, 0u, 0u, 0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err);

  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] App OTA DFU task creation failed!",
             (int)err.Code);
}

/**************************************************************************//**
 * Strong sl_bt_app_ota_dfu_restart_progress() function implementation for
 * MicriumOS.
 *****************************************************************************/
void sl_bt_app_ota_dfu_restart_progress(void)
{
  RTOS_ERR err;
  // Delete current semaphore without considering any pending tasks.
  OSSemDel(&semaphoreOn, OS_OPT_DEL_ALWAYS, &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Semaphore delete failed!",
             (int)err.Code);
  // Delete current task.
  OSTaskDel(&app_ota_dfu_task_tcb, &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Task delete failed!",
             (int)err.Code);
  // Do the initialization again.
  sl_bt_app_ota_dfu_init();
}

/**************************************************************************//**
 * Strong implementation of sli_bt_app_ota_dfu_proceed() for MicriumOS.
 * This function realize the semaphore-based scheduling of the
 * app_ota_dfu_task().
 * This is required to avoid unnecessary power-consumption because of unwanted
 * task cycles.
 *****************************************************************************/
void sli_bt_app_ota_dfu_proceed(void)
{
  RTOS_ERR err;
  OS_SEM_CTR sem_cnt;
  // Trigger app_ota_dfu_task() to proceed with the semaphore.
  sem_cnt = OSSemPost(&semaphoreOn, OS_OPT_POST_1, &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] Semaphore post failed! (Cnt: %d)",
             (int)err.Code, sem_cnt);
}

/**************************************************************************//**
 * Application OTA DFU task content.
 *****************************************************************************/
static void app_ota_dfu_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  OS_SEM_CTR sem_cnt;

  int32_t boot_retv = BOOTLOADER_OK;
  sl_bt_app_ota_dfu_error_t ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;
  sl_bt_app_ota_dfu_status_t ota_sts = SL_BT_APP_OTA_DFU_UNINIT;

  // Bootloader init must be called before using bootloader_* API calls!
  boot_retv = bootloader_init();
  if (boot_retv == BOOTLOADER_OK) {
    ota_sts = SL_BT_APP_OTA_DFU_INIT;
  } else {
    ota_sts = SL_BT_APP_OTA_DFU_ERROR;
    ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
  }

  // Forward state change information to application.
  sli_bt_app_ota_dfu_set_main_status(ota_sts);
  ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
  ota_event.ota_error_code = ota_error;
  ota_event.btl_api_retval = boot_retv;
  ota_event.evt_info.sts.status = ota_sts;
  ota_event.evt_info.sts.prev_status = SL_BT_APP_OTA_DFU_UNINIT;
  sl_bt_app_ota_dfu_on_status_event(&ota_event);

  // Trigger task.
  sli_bt_app_ota_dfu_proceed();

  // Start task main loop.
  while (DEF_TRUE) {
    // Waiting for the semaphore forever, without blocking.
    // No timestamp needed.
    sem_cnt = OSSemPend(&semaphoreOn,
                        (OS_TICK)0,
                        OS_OPT_PEND_BLOCKING,
                        DEF_NULL,
                        &err);
    app_assert(err.Code == RTOS_ERR_NONE,
               "[E: 0x%04x] Semaphore post failed! Count: %d",
               (int)err.Code, (int)sem_cnt);
    // Execute cyclic process of Application OTA DFU.
    sli_bt_app_ota_dfu_step();
  }
}
