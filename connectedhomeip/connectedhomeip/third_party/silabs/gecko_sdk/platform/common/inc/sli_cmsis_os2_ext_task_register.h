/***************************************************************************//**
 * @file sli_cmsis_os2_ext_task_register.h
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

#ifndef SLI_CMSIS_OS2_EXT_TASK_REGISTER_H
#define SLI_CMSIS_OS2_EXT_TASK_REGISTER_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

// Validate the chosen RTOS
#if !defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && !defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#error "The task register API currently only supports FreeRTOS or MicriumOS"
#endif

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
#include "FreeRTOS.h"
#include "task.h"
// Validate maximum of task registers
#if configSDK_THREAD_LOCAL_STORAGE_POINTERS > 255
#error "The task register API currently only supports a maximum of 255 registers"
#endif
// Check if the user has overwritten the configNUM_THREAD_LOCAL_STORAGE_POINTERS config
#if configNUM_THREAD_LOCAL_STORAGE_POINTERS < (configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS + \
                                               configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS)
#error "Please use the configUSER_NUM_THREAD_LOCAL_STORAGE_POINTERS to configure the local storage pointers"
#endif
#elif defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#include "os.h"
#endif

#include "sl_status.h"
#include "cmsis_os2.h"

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
typedef uint8_t sli_task_register_id_t;
#elif defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
typedef OS_REG_ID sli_task_register_id_t;
#endif

/***************************************************************************//**
 * Get the task register ID.
 *
 * @param[out] reg_id     The task register id
 * @return sl_status_t    The status result
 ******************************************************************************/
sl_status_t sli_osTaskRegisterNew(sli_task_register_id_t *reg_id);

/***************************************************************************//**
 * Get the task register value.
 *
 * @param thread_id       CMSIS-RTOS2 thread identification
 * @param reg_id          Task register ID
 * @param[out] value      Value of the task register requested
 * @return sl_status_t    The status result
 ******************************************************************************/
sl_status_t sli_osTaskRegisterGetValue(const osThreadId_t thread_id,
                                       const sli_task_register_id_t reg_id,
                                       uint32_t *value);

/***************************************************************************//**
 * Set the task register to the provided value.
 *
 * @param thread_id       CMSIS-RTOS2 thread identification
 * @param reg_id          Task register ID
 * @param[out] value      Value of the task register to set
 * @return sl_status_t    The status result
 ******************************************************************************/
sl_status_t sli_osTaskRegisterSetValue(const osThreadId_t thread_id,
                                       const sli_task_register_id_t reg_id,
                                       const uint32_t value);

#endif // SLI_CMSIS_OS2_EXT_TASK_REGISTER_H
