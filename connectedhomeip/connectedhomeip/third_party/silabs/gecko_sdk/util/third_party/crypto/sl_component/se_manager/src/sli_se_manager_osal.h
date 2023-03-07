/**************************************************************************/ /**
 * @file
 * @brief OS abstraction layer primitives for the SE Manager
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

#ifndef SE_MANAGER_OSAL_H
#define SE_MANAGER_OSAL_H

#if !defined(SE_MANAGER_CONFIG_FILE)
  #include "sl_se_manager_config.h"
#else
  #include SE_MANAGER_CONFIG_FILE
#endif

#if defined (SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#include "sl_status.h"

#if (defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)) \
  && (defined(SL_SE_MANAGER_THREADING)                                                            \
  || defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION))
// Include CMSIS RTOS2 kernel abstraction layer:
  #include "sli_se_manager_osal_cmsis_rtos2.h"
#else
// Include bare metal abstraction layer:
  #include "sli_se_manager_osal_baremetal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SL_SE_MANAGER_THREADING) || defined(DOXYGEN)

/***************************************************************************//**
 * @brief          Initialize a given mutex
 *
 * @param mutex    Pointer to the mutex needing initialization
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_init_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Free a given mutex
 *
 * @param mutex    Pointer to the mutex being freed
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_free_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Pend on a mutex
 *
 * @param mutex    Pointer to the mutex being pended on
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_take_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Try to own a mutex without waiting
 *
 * @param mutex    Pointer to the mutex being tested
 *
 * @return         SL_STATUS_OK on success (= mutex successfully owned), error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_take_mutex_non_blocking(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Release a mutex
 *
 * @param mutex    Pointer to the mutex being released
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_give_mutex(se_manager_osal_mutex_t *mutex);

#endif // SL_SE_MANAGER_THREADING

/***************************************************************************//**
 * @brief          Initialize a completion object.
 *
 * @param p_comp   Pointer to an se_manager_osal_completion_t object allocated
 *                 by the user.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp);

/***************************************************************************//**
 * @brief          Free a completion object.
 *
 * @param p_comp   Pointer to an se_manager_osal_completion_t object.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp);

/***************************************************************************//**
 * @brief          Wait for completion event.
 *
 * @param p_comp   Pointer to completion object which must be initialized by
 *                 calling se_manager_osal_completion_init before calling this
 *                 function.
 *
 * @param ticks    Ticks to wait for the completion.
 *                 Pass a value of SE_MANAGER_OSAL_WAIT_FOREVER in order to
 *                 wait forever.
 *                 Pass a value of SE_MANAGER_OSAL_NON_BLOCKING in order to
 *                 return immediately.
 *
 * @return         Status code, @ref sl_status.h. Typcally SL_STATUS_OK if success,
 *                 or SL_STATUS_TIMEOUT if no completion within the given ticks.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp,
                                int ticks);

/***************************************************************************//**
 * @brief          Signal completion.
 *
 * @param p_comp   Pointer to completion object which must be initialized by
 *                 calling se_manager_osal_completion_init before calling this
 *                 function.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_complete(se_manager_osal_completion_t *p_comp);

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_H
