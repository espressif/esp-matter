/**************************************************************************/ /**
 * @file
 * @brief OS abstraction layer primitives for SE manager on CMSIS RTOS2
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SE_MANAGER_OSAL_CMSIS_RTOS_H
#define SE_MANAGER_OSAL_CMSIS_RTOS_H

#include "cmsis_os2.h"

#if defined (SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  #include "FreeRTOSConfig.h"
  #if (configSUPPORT_STATIC_ALLOCATION == 1)
    #include "FreeRTOS.h"  // StaticSemaphore_t
    #include <string.h>
  #endif
#else
  #include "em_core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

/// In order to wait forever in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_WAIT_FOREVER  (osWaitForever)
/// In order to return immediately in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_NON_BLOCKING  (0)

/// Priority to use for SEMBRX IRQ
#if defined(SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY)
  #if (SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY >= (1U << __NVIC_PRIO_BITS) )
      #error Illegal SEMBRX priority level.
  #endif
  #if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
    #if (SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY < (configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8U - __NVIC_PRIO_BITS) ) )
      #error Illegal SEMBRX priority level.
    #endif
  #else
    #if (SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY < CORE_ATOMIC_BASE_PRIORITY_LEVEL)
      #error Illegal SEMBRX priority level.
    #endif
  #endif
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY
#else
  #if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
    #define SE_MANAGER_SEMBRX_IRQ_PRIORITY (configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8U - __NVIC_PRIO_BITS) )
  #else
    #define SE_MANAGER_SEMBRX_IRQ_PRIORITY (CORE_ATOMIC_BASE_PRIORITY_LEVEL)
  #endif
#endif

/// Determine if executing at interrupt level on ARM Cortex-M.
#define RUNNING_AT_INTERRUPT_LEVEL (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

// -----------------------------------------------------------------------------
// Typedefs

/// Completion object used to wait for and signal end of an operation.
typedef struct se_manager_osal_completion {
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && (configSUPPORT_STATIC_ALLOCATION == 1)
  osSemaphoreAttr_t semaphore_attr;
  StaticSemaphore_t static_sem_object;
#endif
  osSemaphoreId_t   semaphore_ID;
} se_manager_osal_completion_t;

/// SE manager mutex definition for CMSIS RTOS2.
typedef struct se_manager_osal_mutex {
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && (configSUPPORT_STATIC_ALLOCATION == 1)
  osMutexAttr_t mutex_attr;
  StaticSemaphore_t static_sem_object;
#endif
  osMutexId_t   mutex_ID;
} se_manager_osal_mutex_t;

// -----------------------------------------------------------------------------
// Functions

/// Initialize a mutex object.
__STATIC_INLINE
sl_status_t se_manager_osal_init_mutex(se_manager_osal_mutex_t *mutex)
{
  if (mutex == NULL) {
    return SL_STATUS_FAIL;
  }

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && (configSUPPORT_STATIC_ALLOCATION == 1)
  // Zeroize all members of the mutex attributes object and setup the static control block.
  memset(&mutex->mutex_attr, 0, sizeof(mutex->mutex_attr));
  mutex->mutex_attr.cb_mem = &mutex->static_sem_object;
  mutex->mutex_attr.cb_size = sizeof(mutex->static_sem_object);
  mutex->mutex_ID = osMutexNew(&mutex->mutex_attr);
#else
  mutex->mutex_ID = osMutexNew(NULL);
#endif

  return (mutex->mutex_ID == NULL ? SL_STATUS_FAIL : SL_STATUS_OK);
}

/// Free a mutex object.
__STATIC_INLINE
sl_status_t se_manager_osal_free_mutex(se_manager_osal_mutex_t *mutex)
{
  if (mutex == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osMutexDelete(mutex->mutex_ID);
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Acquire ownership of a mutex. If busy, wait until available.
__STATIC_INLINE
sl_status_t se_manager_osal_take_mutex(se_manager_osal_mutex_t *mutex)
{
  if (mutex == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexAcquire(mutex->mutex_ID, SE_MANAGER_OSAL_WAIT_FOREVER);
  }
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Try to acquire ownership of a mutex without waiting.
__STATIC_INLINE
sl_status_t se_manager_osal_take_mutex_non_blocking(se_manager_osal_mutex_t *mutex)
{
  if (mutex == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexAcquire(mutex->mutex_ID, SE_MANAGER_OSAL_NON_BLOCKING);
  }
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Release ownership of a mutex.
__STATIC_INLINE
sl_status_t se_manager_osal_give_mutex(se_manager_osal_mutex_t *mutex)
{
  if (mutex == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexRelease(mutex->mutex_ID);
  }
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Initialize a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp)
{
  if (p_comp == NULL) {
    return SL_STATUS_FAIL;
  }

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && (configSUPPORT_STATIC_ALLOCATION == 1)
  // Zeroize all members of the semaphore attributes object and setup the static control block.
  memset(&p_comp->semaphore_attr, 0, sizeof(p_comp->semaphore_attr));
  p_comp->semaphore_attr.cb_mem = &p_comp->static_sem_object;
  p_comp->semaphore_attr.cb_size = sizeof(p_comp->static_sem_object);
  p_comp->semaphore_ID = osSemaphoreNew(1u, 0u, &p_comp->semaphore_attr);
#else
  p_comp->semaphore_ID = osSemaphoreNew(1u, 0u, NULL);
#endif

  return (p_comp->semaphore_ID == NULL ? SL_STATUS_FAIL : SL_STATUS_OK);
}

/// Free a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp)
{
  if (p_comp == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osSemaphoreDelete(p_comp->semaphore_ID);
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

// Wait for a completion object to be completed.
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp, int ticks)
{
  if (p_comp == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osSemaphoreAcquire(p_comp->semaphore_ID,
                                (uint32_t)ticks);
  }
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

// Complete a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_complete(se_manager_osal_completion_t* p_comp)
{
  if (p_comp == NULL) {
    return SL_STATUS_FAIL;
  }

  osStatus_t status = osOK;
  osKernelState_t state = osKernelGetState();
  if ((state == osKernelRunning) || (state == osKernelLocked)) {
    status = osSemaphoreRelease(p_comp->semaphore_ID);
  }
  return (status == osOK ? SL_STATUS_OK : SL_STATUS_FAIL);
}

// Lock the RTOS Kernel scheduler.
__STATIC_INLINE int32_t
se_manager_osal_kernel_lock(void)
{
  return osKernelLock();
}

// Restore the RTOS Kernel scheduler lock state.
__STATIC_INLINE int32_t
se_manager_osal_kernel_restore_lock(int32_t lock)
{
  return osKernelRestoreLock(lock);
}

// Current RTOS kernel state.
__STATIC_INLINE osKernelState_t
se_manager_osal_kernel_get_state(void)
{
  return osKernelGetState();
}

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_CMSIS_RTOS_H
