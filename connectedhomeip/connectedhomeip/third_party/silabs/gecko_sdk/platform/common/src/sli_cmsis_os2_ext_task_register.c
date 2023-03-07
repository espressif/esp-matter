/***************************************************************************//**
 * @file sli_cmsis_os2_ext_task_register.c
 * @brief Abstraction for Task Registers (Thread Local Variables)
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_assert.h"
#include "sli_cmsis_os2_ext_task_register.h"
#include "sl_cmsis_os2_common.h"

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Get a task register ID
 ******************************************************************************/
sl_status_t sli_osTaskRegisterNew(sli_task_register_id_t *reg_id)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (reg_id == NULL) {
    return SL_STATUS_FAIL;
  }

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
  RTOS_ERR err;

  *reg_id = OSTaskRegGetID(&err);
  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    status = SL_STATUS_OK;
  }

#elif defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  static uint8_t register_count = 0;
  if (register_count > (configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS - 1)) {
    return SL_STATUS_FAIL;
  }

  *reg_id = register_count + configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS;
  ++register_count;
  status = SL_STATUS_OK;
#else
#error "Task registers abstraction only supports MicriumOS or FreeRTOS"
#endif
  return status;
}

/***************************************************************************//**
 * Get the task register
 ******************************************************************************/
sl_status_t sli_osTaskRegisterGetValue(const osThreadId_t thread_id,
                                       const sli_task_register_id_t reg_id,
                                       uint32_t *value)
{
  sl_status_t status = SL_STATUS_FAIL;
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
  RTOS_ERR    err;
  osThread_t *thread;

  if (value == NULL) {
    return SL_STATUS_FAIL;
  }

  if (thread_id != NULL) {
    thread = (osThread_t *)thread_id;
    *value = OSTaskRegGet(&thread->tcb, reg_id, &err);
  } else {
    *value = OSTaskRegGet(NULL, reg_id, &err);
  }

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    status = SL_STATUS_OK;
  }
#elif defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  *value = (uint32_t)pvTaskGetThreadLocalStoragePointer(thread_id, reg_id);
  status = SL_STATUS_OK;
#else
#error "Task registers abstraction only supports MicriumOS or FreeRTOS"
#endif

  return status;
}

/***************************************************************************//**
 * Set the task register
 ******************************************************************************/
sl_status_t sli_osTaskRegisterSetValue(const osThreadId_t thread_id,
                                       const sli_task_register_id_t reg_id,
                                       const uint32_t value)
{
  sl_status_t status = SL_STATUS_FAIL;
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
  RTOS_ERR err;
  osThread_t *thread;

  if (thread_id != NULL) {
    thread = (osThread_t *)thread_id;
    OSTaskRegSet(&thread->tcb, reg_id, (OS_REG)value, &err);
  } else {
    OSTaskRegSet(NULL, reg_id, (OS_REG)value, &err);
  }

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    status = SL_STATUS_OK;
  }
#elif defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  vTaskSetThreadLocalStoragePointer(thread_id, reg_id, (void *)value);
  status = SL_STATUS_OK;
#else
#error "Task registers abstraction only supports MicriumOS or FreeRTOS"
#endif

  return status;
}
