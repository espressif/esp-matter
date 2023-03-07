/***************************************************************************//**
 * @file sl_cmsis_os2_common.h
 * @brief OS-agnostic header to provide CMSIS OS-Specific APIs like typedefs.
 * @version x.y.z
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

#ifndef SL_CMSIS_OS2_COMMON_H
#define SL_CMSIS_OS2_COMMON_H

#include <stdint.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

// Validate the chosen RTOS
#if !defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && !defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#error "The extended CMSIS RTOS2 API currently only supports FreeRTOS or MicriumOS"
#endif

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
#include "FreeRTOS.h"
#elif defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#include "os.h"
#if (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
// needed for osTimer_t struct
#include "cmsis_os2.h"
#endif
#endif

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)

#define   osEventFlagsCbSize   sizeof(StaticEventGroup_t)
#define   osThreadCbSize       sizeof(StaticTask_t)
#define   osTimerCbSize        sizeof(StaticTimer_t)
#define   osMutexCbSize        sizeof(StaticSemaphore_t)
#define   osSemaphoreCbSize    sizeof(StaticSemaphore_t)
#define   osMessageQueueCbSize sizeof(StaticQueue_t)

typedef StaticEventGroup_t osEventFlags_t;
typedef StaticTask_t       osThread_t;
typedef StaticTimer_t      osTimer_t;
typedef StaticSemaphore_t  osMutex_t;
typedef StaticSemaphore_t  osSemaphore_t;
typedef StaticQueue_t      osMessageQueue_t;

#elif defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)

typedef struct {
  OS_TCB      tcb; // This must be the first element, used by OSTCBCurPtr
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  OS_FLAG_GRP flag_grp;
#endif
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_MUTEX    join_mutex;
#endif
  uint8_t     obj_dyn_alloc;
  uint8_t     stack_dyn_alloc;
  uint32_t    attr_bits;
} osThread_t;

#if (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
typedef struct {
  sl_sleeptimer_timer_handle_t  handle;
  osTimerFunc_t                 callback;
  void                          *callback_data;
  osTimerType_t                 type;
  const char                    *name;
  uint8_t                       dyn_alloc;
} osTimer_t;
#endif

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
typedef struct {
  OS_FLAG_GRP flag_grp;
  uint8_t     dyn_alloc;
  uint32_t    flags;
} osEventFlags_t;
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
typedef struct {
  OS_MUTEX    mutex;
  uint8_t     dyn_alloc;
  uint8_t     recursive;
} osMutex_t;
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
typedef struct {
  OS_SEM      sem;
  uint8_t     dyn_alloc;
  uint32_t    max_ctr;
} osSemaphore_t;
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
typedef struct {
  OS_SEM      sem_put;
  OS_SEM      sem_get;
  uint8_t    *buf;
  uint8_t     obj_dyn_alloc;
  uint8_t     buf_dyn_alloc;
  uint32_t    msg_count;
  uint32_t    msg_size;
  uint32_t    msg_queued;
  uint32_t    msg_head;
  uint32_t    msg_tail;
} osMessageQueue_t;
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
typedef struct {
  OS_SEM      sem;
  uint8_t    *buf;
  uint8_t     obj_dyn_alloc;
  uint8_t     buf_dyn_alloc;
  uint32_t    block_count;
  uint32_t    block_size;
  uint32_t    free_count;
  uint32_t    free_head;
} osMemoryPool_t;
#endif

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
#define   osEventFlagsCbSize     sizeof(osEventFlags_t)
#endif

#define   osThreadCbSize         sizeof(osThread_t)

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#define   osTimerCbSize          sizeof(osTimer_t)
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
#define   osMutexCbSize          sizeof(osMutex_t)
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
#define   osSemaphoreCbSize      sizeof(osSemaphore_t)
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
#define   osMessageQueueCbSize   sizeof(osMessageQueue_t)
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
#define   osMemoryPoolCbSize     sizeof(osMemoryPool_t)
#endif

#endif // SL_CATALOG_MICRIUMOS_KERNEL_PRESENT

#endif // SL_CMSIS_OS2_COMMON_H
