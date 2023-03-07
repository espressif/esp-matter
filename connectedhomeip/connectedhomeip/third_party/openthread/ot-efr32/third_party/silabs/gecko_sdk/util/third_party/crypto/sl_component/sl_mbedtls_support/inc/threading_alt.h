/**************************************************************************/ /**
 * @file
 * @brief Threading primitive implementation for mbed TLS
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

#ifndef THREADING_ALT_H
#define THREADING_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_threading Threading Primitives
 * \brief Threading primitive implementation for mbed TLS
 *
 * This file contains the glue logic between the mbed TLS threading API
 * and CMSIS RTOS2 API.
 *
 * In order to enable support for Micrium OS backend
 * the user must make sure SL_CATALOG_MICRIUMOS_KERNEL_PRESENT is defined.
 * In order to enable support for FreeRTOS backend the user must make sure
 * SL_CATALOG_FREERTOS_KERNEL_PRESENT is defined.
 *
 * Applications created using Simplicity Studio 5 the sl_component_catalog.h
 * file will define one of the above in order to declare the presence
 * of a specific RTOS.
 *
 * \note
 * In order to use the Silicon Labs Hardware Acceleration plugins in
 * multi-threaded applications, select
 * <b>Mbed TLS support for EFM32/EFR32 crypto acceleration</b> component.
 *
 * \{
 ******************************************************************************/

#include "mbedtls/threading.h"

#if defined(MBEDTLS_THREADING_ALT) && defined(MBEDTLS_THREADING_C)

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)

#include "cmsis_os2.h"
#include "sl_assert.h"

#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  #include "FreeRTOSConfig.h"
  #if (configSUPPORT_STATIC_ALLOCATION == 1)
    #include "FreeRTOS.h"  // StaticSemaphore_t
    #include <string.h>
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SL_THREADING_ALT

/// SE manager mutex definition for CMSIS RTOS2.
typedef struct mbedtls_threading_mutex {
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT) && (configSUPPORT_STATIC_ALLOCATION == 1)
  osMutexAttr_t mutex_attr;
  StaticSemaphore_t static_sem_object;
#endif
  osMutexId_t   mutex_ID;
} mbedtls_threading_mutex_t;

/**
 * \brief          Initialize a given mutex
 *
 * \param mutex    Pointer to the mutex needing initialization
 */
static inline void THREADING_InitMutex(mbedtls_threading_mutex_t *mutex)
{
  if (mutex == NULL) {
    return;
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

  EFM_ASSERT(mutex->mutex_ID != NULL);
}

/**
 * \brief          Free a given mutex
 *
 * \param mutex    Pointer to the mutex being freed
 */
static inline void THREADING_FreeMutex(mbedtls_threading_mutex_t *mutex)
{
  if (mutex == NULL) {
    return;
  }

  osStatus_t status = osMutexDelete(mutex->mutex_ID);
  EFM_ASSERT(status == osOK);
}

/**
 * \brief          Pend on a mutex
 *
 * \param mutex    Pointer to the mutex being pended on
 *
 * \return         RTOS_ERR_NONE on success, error code otherwise.
 */
static inline int THREADING_TakeMutexBlocking(mbedtls_threading_mutex_t *mutex)
{
  if (mutex == NULL) {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexAcquire(mutex->mutex_ID, osWaitForever);
  }
  return (status == osOK ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

/**
 * \brief          Try to own a mutex without waiting
 *
 * \param mutex    Pointer to the mutex being tested
 *
 * \return         RTOS_ERR_NONE on success (= mutex successfully owned), error code otherwise.
 */
static inline int THREADING_TakeMutexNonBlocking(mbedtls_threading_mutex_t *mutex)
{
  if (mutex == NULL) {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexAcquire(mutex->mutex_ID, 0u);
  }
  return (status == osOK ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

/**
 * \brief          Release a mutex
 *
 * \param mutex    Pointer to the mutex being released
 *
 * \return         RTOS_ERR_NONE on success, error code otherwise.
 */
static inline int THREADING_GiveMutex(mbedtls_threading_mutex_t *mutex)
{
  if (mutex == NULL) {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  osStatus_t status = osOK;
  if (osKernelGetState() == osKernelRunning) {
    status = osMutexRelease(mutex->mutex_ID);
  }
  return (status == osOK ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR);
}

#ifdef __cplusplus
}
#endif

#endif // SL_CATALOG_MICRIUMOS_KERNEL_PRESENT || SL_CATALOG_FREERTOS_KERNEL_PRESENT

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of threading_set_alt */
void mbedtls_threading_set_alt(void (*mutex_init)(mbedtls_threading_mutex_t *),
                               void (*mutex_free)(mbedtls_threading_mutex_t *),
                               int (*mutex_lock)(mbedtls_threading_mutex_t *),
                               int (*mutex_unlock)(mbedtls_threading_mutex_t *) );

/**
 * \brief          Helper function for setting up the mbed TLS threading subsystem
 */
static inline void THREADING_setup(void)
{
  mbedtls_threading_set_alt(&THREADING_InitMutex,
                            &THREADING_FreeMutex,
                            &THREADING_TakeMutexBlocking,
                            &THREADING_GiveMutex);
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT && MBEDTLS_THREADING_C */

/** \} (end addtogroup sl_crypto_threading) */
/** \} (end addtogroup sl_crypto) */

#endif /* THREADING_ALT_H */
